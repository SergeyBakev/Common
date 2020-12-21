#pragma once
#include <string>
#include <functional>
#include <fstream>
#include <filesystem>
#include <memory>
#include <vector>
#include <stdint.h>
#include <map>
#include <filesystem>
#include "ke_array_indexer.h"
#include <tchar.h>

class FArrayFileStorage
{
	using Vector = std::vector<std::wstring>;
public:
	static void AddTemporaryFile(const TCHAR* fileName)
	{
		Vector& _files = GetVector();
		_files.push_back({ fileName });
	}

	static void ClearAll()
	{
		Vector& _files = GetVector();
		for (auto& file : _files)
			RemoveTemporaryFile(file.c_str());
	}

	static void RemoveTemporaryFile(const TCHAR* fileName)
	{
		Vector& _files = GetVector();
		std::wstring file(fileName);
		auto it = std::find(std::begin(_files), std::end(_files), file);
		if (it != std::end(_files))
		{
			FILE* tmp;
			_wfopen_s(&tmp, fileName, _T("r"));
			if (tmp != nullptr)
			{
				fclose(tmp);
				_wremove(fileName);
			}

			_files.erase(it);
		}

	}

private:
	static Vector& GetVector()
	{
		static std::vector<std::wstring> _files;
		return _files;
	}
};

template<class IndexMapper> class FArrayBaseProxyObject;

template <class IndexMapper = SparseIndexMapper>
class FArrayBase
{
	

public:
	using object_size = size_t;
	using index_type = typename IndexMapper::external_idx_type;
	using inner_index_type = typename IndexMapper::inner_idx_type;
	using IndexerType = IndexMapper;

	using iterator = FArrayBaseProxyObject<IndexMapper>;

	FArrayBase()
	{
		std::wstring fileName = CreateTemoraryFileName();
		if (!CreateNew(fileName.c_str(), true))
			throw std::exception();
	}

	FArrayBase(object_size size) : FArrayBase()
	{
		_buffer_size = size;
	}

	bool CreateNew(const TCHAR* file_name, bool isTemp = true)
	{
		TryDestroy();
		int mode = std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc;
		_fs.open(file_name, mode);
		if (!_fs.is_open())
			return false;

		SetTemporary(isTemp);
		_file_name = file_name;
		if (!IsTemporary())
			FArrayFileStorage::AddTemporaryFile(file_name);

		return true;
	}

	bool AssignExsitArray(const TCHAR* file_name, bool isTemp = false)
	{
		TryDestroy();
		int mode = std::ios::in | std::ios::out | std::ios::binary;
		_fs.open(file_name, mode);
		if (!_fs.is_open())
			return false;

		SetTemporary(isTemp);
		_file_name = file_name;

		if (!IsTemporary())
			FArrayFileStorage::AddTemporaryFile(file_name);
		return false;
	}

	inline void Add(void* value)
	{
		return Add(value, _buffer_size);
	}

	void Add(void* value, object_size size)
	{
		_mapper.Insert(_mapper.CreateNewIndex());
		_Add(value, size);
	}

	void SetAt(const index_type& idx, void* object)
	{
		SetAt(idx, object, _buffer_size);
	}

	void SetAt(const index_type& idx, void* object, object_size size)
	{
		_ReplaceAtOrAdd(idx, object, size);
	}

	void GetAt(const index_type& index, void* object) const
	{
		if (!Contains(index))
			throw std::out_of_range("Invalid index");
		
		return _GetAt(_mapper.GetRealIndex(index), object, _buffer_size);
	}

	void GetAt(const index_type& index, void* object, object_size size) const
	{
		if (!Contains(index))
			return;

		return _GetAt(_mapper.GetRealIndex(index), object, size);
	}

	void Clear()
	{
		if(_fs.is_open())
			_fs.close();

		int mode = std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc;
		_fs.open(_file_name, mode);
		if (!_fs.is_open())
			throw std::exception("Clear error");
		_mapper.Clear();

	}

	inline size_t Count() const noexcept { return _mapper.GetCount(); }

	object_size GetObjectSize()const{return _buffer_size;}

	void SetObjectSize(object_size size)
	{
		_ASSERT(size != 0);
		_buffer_size = size;
	}

	void SetTemporary(bool bTemp) noexcept
	{
		if (!bTemp)
			FArrayFileStorage::AddTemporaryFile(_file_name.c_str());

		_isTemporary = bTemp;
	}

	bool IsTemporary() const noexcept
	{
		return _isTemporary;
	}

	const TCHAR* GetFileName()  const noexcept
	{
		return _file_name.c_str();
	}

	IndexMapper& GetMapper() noexcept
	{
		return _mapper;
	}

	const IndexMapper& GetMapper() const noexcept
	{
		return _mapper;
	}

	bool Contains(const index_type& idx) const
	{
		return _mapper.GetRealIndex(idx) != IndexMapper::npos;
	}

	void Serialize(std::ofstream& storage) const
	{
		_mapper.Serialize(storage);
		auto cur_pos = _fs.tellg();
		size_t length = Count() * _buffer_size;
		
		storage.write(reinterpret_cast<const char*>(&length), sizeof(length));
		storage.write(reinterpret_cast<const char*>(&_buffer_size), sizeof(_buffer_size));
		storage.write(reinterpret_cast<const char*>(&_end_pos), sizeof(_end_pos));
		storage.write(reinterpret_cast<const char*>(&_current_putpos), sizeof(_current_putpos));
		_fs.seekg(std::ios::beg);
	
		const size_t blockSize = 4096;
		char data[blockSize] = { 0 };
		for(size_t i = 0; i < length / blockSize; ++i)
		{
			_fs.read(data, _countof(data));
			storage.write(reinterpret_cast<const char*>(data), blockSize);
		}
		if (length %= blockSize)
		{
			_fs.read(data, length);
			storage.write(reinterpret_cast<const char*>(data), length);
		}
		
		_fs.seekp(cur_pos);
	}

	void Deserialize(std::ifstream& storage)
	{
		Clear();

		_mapper.Deserialize(storage);
		size_t length = 0;
		
		storage.read(reinterpret_cast<char*>(&length), sizeof(length));
		storage.read(reinterpret_cast<char*>(&_buffer_size), sizeof(_buffer_size));
		storage.read(reinterpret_cast<char*>(&_end_pos), sizeof(_end_pos));
		storage.read(reinterpret_cast<char*>(&_current_putpos), sizeof(_current_putpos));
		const size_t blockSize = 4096;
		char data[blockSize] = { 0 };
		for (size_t i = 0; i < length / blockSize; ++i)
		{
			storage.read(data, _countof(data));
			_fs.write(data, _countof(data));
		}
				
		if (length %= blockSize)
		{
			storage.read(data, length);
			_fs.write(data, length);
		}	
	}

	FArrayBaseProxyObject<IndexMapper> begin()
	{
		return FArrayBaseProxyObject<IndexMapper>(_mapper.GetFirst(),this);
	}

	~FArrayBase()
	{
		TryDestroy();
	}

protected:

	IndexMapper _mapper;
	object_size _buffer_size = 0;
	mutable std::streamoff _current_putpos = 0;
	mutable bool _need_flush = false;
	mutable std::streamoff _end_pos = 0;
	mutable std::fstream _fs;

#ifdef UNICODE
	std::wstring _file_name;
#else
	std::string _file_name;
#endif // UNICODE


	bool _isTemporary = true;

	void TryDestroy()
	{
		if (_fs.is_open())
		{
			_fs.close();
			if (IsTemporary())
				_wremove(_file_name.c_str());

			_file_name = _T("");
		}
	}

	std::wstring CreateTemoraryFileName()
	{
		std::wstring file_name;
#ifndef UUID
		wchar_t buffer[_MAX_PATH];
		_wtmpnam_s(buffer, _countof(buffer));
		file_name = buffer;
#else
		file_name = _uuid_t::New().ToString();
		file_name = temp_dir.c_str();
#endif 

#ifdef UUID
		std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
		temp_dir /= file_name;
#endif

		return file_name;
	}

	void _ReplaceAtOrAdd(index_type external_idx, void* value, object_size size)
	{
		_ASSERT(size != 0);
		if (!Contains(external_idx))
		{
			_mapper.Insert(external_idx);
			_Add(value, size);
			return;
		}

		if (_need_flush)
		{
			_fs.flush();
			_need_flush = false;
		}
			
		auto t = _fs.tellp();
		inner_index_type index = _mapper.GetRealIndex(external_idx);

		std::streamoff sizeoffset = static_cast<std::streamoff>(index * size);
		std::streamoff seek = sizeoffset - _current_putpos;

		
		_fs.seekp(seek, std::ios::cur);

#ifdef _DEBUG
		if (_fs.fail())
			throw std::exception("Stream has invalid state");
#endif // DEBUG	
		_fs.write(reinterpret_cast<const char*>(value), size);

		auto t2 = _fs.tellp();

		_current_putpos = _current_putpos + seek + static_cast<std::streamoff>(size);


	}

	void _GetAt(const index_type& index, void* value, object_size size) const
	{
		if (_need_flush)
		{
			_fs.flush();
			_need_flush = false;
		}

		_ASSERT(size != 0);
		std::streamoff sizeoffset = static_cast<std::streamoff>(index * size);
		std::streamoff seek = sizeoffset - _current_putpos;
		
		if(seek != static_cast<std::streamoff>(0))
			_fs.seekg(seek, std::ios::cur);

#ifdef _DEBUG
		if (_fs.fail())
			throw std::exception("Stream has invalid state");
#endif // DEBUG	
		_fs.read(reinterpret_cast<char*>(value), size);
		_current_putpos = _current_putpos + seek + static_cast<std::streamoff>(size);
	}

	void _Add(void* value, object_size size)
	{
		_ASSERT(size != 0);
		
		if(_end_pos != _current_putpos)
		{
			size_t seek_size = _end_pos - _current_putpos;
			_ASSERT(seek_size > 0);
			_fs.seekp(static_cast<std::streamoff>(seek_size), std::ios::cur);
		}
		_end_pos += size;
		_current_putpos = _end_pos;

		_fs.write(reinterpret_cast<char*>(value), size);
		_need_flush = true;
#ifdef _DEBUG
		if (_fs.fail())
			throw std::exception("Stream has invalid state");
#endif // DEBUG	
	}
};

template<class IndexMapper>
class FArrayBaseProxyObject
{
public:
	template <class Indexer> friend class FArrayBase;
	using index_type = typename IndexMapper::external_idx_type;
	using inner_index_type = typename IndexMapper::inner_idx_type;
	
	struct MemoryDeleter{void operator()(void* mem) { free(mem); }};

private:

	template <class IndexMapper>
	FArrayBaseProxyObject(index_type idx, FArrayBase<IndexMapper>* arr) :
		_arr(arr), _idx(idx)
	{
		_buffer.reset(malloc(_arr->GetObjectSize()));
	}

public:
	FArrayBaseProxyObject() = delete;

	FArrayBaseProxyObject(FArrayBaseProxyObject&& other)
		: _buffer(std::move(other._buffer)),
		_arr(other._arr),
		_idx(other._idx)
	{

	}

	void* operator*()
	{
#ifdef _DEBUG
		_ASSERT(_idx >= 0 && _idx != IndexMapper::npos);
		_ASSERT(_idx < _arr->Count());
#endif 
		_arr->GetAt(_idx, _buffer.get());
		return _buffer.get();
	}
	
	void* operator*() const
	{
#ifdef _DEBUG
		_ASSERT(_idx >= 0 && _idx != IndexMapper::npos );
		_ASSERT(_idx < _arr->Count());
#endif 
		_arr->GetAt(_idx, _buffer.get());
		return _buffer.get();
	}
	template <typename Type>
	Type* Get() const
	{
		return (Type*)this->operator*();
	}

private:
	std::unique_ptr<void, MemoryDeleter> _buffer;
	FArrayBase<IndexMapper>* _arr;
	index_type _idx;
};