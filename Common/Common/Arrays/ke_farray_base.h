#pragma once
#include <string>
#include <functional>
#include <fstream>
#include <filesystem>
#include <memory>
#include <cmath>
#include <vector>
#include <stdint.h>
#include <map>
#include <filesystem>
#include <tchar.h>
#include "ke_array_indexer.h"
#include "..\Helpers\StringHelper.h"
#include "..\Helpers\DoubleHelper.h"

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

	void Add(void* value)
	{
		Add(value, _buffer_size);
	}

	//value - data for write
	//size - size of the write object
	//count - count of the write objects
	void Add(void* value, object_size size, size_t count)
	{
		Add(value, size * count);
	}

	void Add(void* value, object_size size)
	{
		_mapper.Add(_mapper.CreateIndex());
		_Add(value, size);
	}

	void SetAt(const index_type& idx, void* object)
	{
		SetAt(idx, object, _buffer_size);
	}

	void SetAt(const index_type& idx, void* storage, object_size size)
	{
		_ReplaceAtOrAdd(idx, storage, size);
	}

	//index - pos for get
	//storage - container for read data
	//size - size of the read object
	//count - count of the read objects
	void GetAt(const index_type& index, void* storage, const object_size& size, size_t count) const
	{
		if (!Contains(index))
			ThrowOutOfRange(index);

		_GetAt(_mapper.RealIndex(index), storage, size, count);
	}

	void GetAt(const index_type& index, void* storage, size_t count) const
	{
		if (!Contains(index))
			ThrowOutOfRange(index);

		_GetAt(_mapper.RealIndex(index), storage, _buffer_size, count);
	}

	void GetAt(const index_type& index, void* storage) const
	{
		if (!Contains(index))
			ThrowOutOfRange(index);
		
		_GetAt(_mapper.RealIndex(index), storage, _buffer_size,1);
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

	inline size_t Count() const noexcept { return _end_pos / _buffer_size; }

	object_size GetObjectSize()const{return _buffer_size;}

	template <class Type>
	size_t GetCountElement() const
	{
		auto type_size = static_cast<double>(sizeof(std::remove_pointer_t<std::remove_cvref_t<Type>>));
		auto size = static_cast<double>(_buffer_size) / type_size;
		double remainder = modf(size, &type_size);
		_ASSERT(DoubleHelper::Compare(remainder, 0) != 0);
		return static_cast<size_t>(type_size);
	}

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
		return _mapper.RealIndex(idx) != IndexMapper::npos;
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

	iterator operator [](index_type idx)
	{
		return iterator(_mapper.RealIndex(idx), this,_mapper);
	}

	iterator begin()
	{
		return FArrayBaseProxyObject<IndexMapper>(_mapper.First(),this,_mapper);
	}

	iterator end()
	{
		return FArrayBaseProxyObject<IndexMapper>(IndexerType::npos, this, _mapper);
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
	std::wstring _file_name;

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
			_mapper.Add(external_idx);
			_Add(value, size);
			return;
		}
			
		inner_index_type index = _mapper.RealIndex(external_idx);

		std::streamoff sizeoffset = static_cast<std::streamoff>(index * size);
		std::streamoff seek = sizeoffset - _current_putpos;

		auto posp = _fs.tellp();
		auto posg = _fs.tellg();
		if (seek != static_cast<std::streamoff>(0))
			_fs.seekp(seek, std::ios::cur);

		posp = _fs.tellp();
		posg = _fs.tellg();
#ifdef _DEBUG
		if (_fs.fail())
			throw std::exception("Stream has invalid state");
#endif // DEBUG	
		_fs.write(reinterpret_cast<const char*>(value), size);
		_current_putpos = _current_putpos + seek + static_cast<std::streamoff>(size);
		_need_flush = true;
		posp = _fs.tellp();
		posg = _fs.tellg();
	}

	void _GetAt(const index_type& index, void* value, object_size size, size_t count) const
	{
		if (_need_flush)
		{
			_fs.flush();
			_need_flush = false;
		}

		_ASSERT(size != 0);
		std::streamoff sizeoffset = static_cast<std::streamoff>(index * size);
		std::streamoff seek = sizeoffset - _current_putpos;
		
		auto posp = _fs.tellp();
		auto posg = _fs.tellg();
		if(seek != static_cast<std::streamoff>(0))
			_fs.seekg(seek, std::ios::cur);

#ifdef _DEBUG
		if (_fs.fail())
			throw std::exception("Stream has invalid state");
#endif // DEBUG	
		posp = _fs.tellp();
		posg = _fs.tellg();

		_fs.read(reinterpret_cast<char*>(value), size * count);
		_current_putpos = _current_putpos + seek + static_cast<std::streamoff>(size) * count;
		
		posp = _fs.tellp();
		posg = _fs.tellg();
	}

	void _Add(void* value, object_size size)
	{
		_ASSERT(size != 0);		
		_ASSERT(size >=_buffer_size);		
		if(_end_pos != _current_putpos)
		{
			size_t seek_size = _end_pos - _current_putpos;
			_ASSERT(seek_size > 0);
			_fs.seekp(static_cast<std::streamoff>(seek_size), std::ios::cur);
		}
		_end_pos += size;
		_current_putpos = _end_pos;

		_fs.write(reinterpret_cast<char*>(value), size);
		if (!_need_flush)
			_need_flush = true;
#ifdef _DEBUG
		if (_fs.fail())
			throw std::exception("Stream has invalid state");
#endif // DEBUG	
	}

	void ThrowOutOfRange(const index_type& idx ) const
	{
		throw std::out_of_range(StringHelper::Concatenate(" ", "Index:", idx, "doest contains in array"));
	}
};

template<class IndexMapper>
class FArrayBaseProxyObject
{
public:
	template <class Indexer> friend class FArrayBase;
	using index_type = typename IndexMapper::external_idx_type;
	using inner_index_type = typename IndexMapper::inner_idx_type;
	struct MemoryDeleter { void operator()(void* mem) { free(mem); } };

private:

	FArrayBaseProxyObject(index_type idx, FArrayBase<IndexMapper>* arr, IndexMapper& mapper) :
		_arr(arr), _idx(idx), _mapper(mapper), _need_flush(false)
		
	{
		_buffer.reset(malloc(_arr->GetObjectSize()));
	}

	FArrayBaseProxyObject(index_type idx, FArrayBase<IndexMapper>* arr,std::unique_ptr<void, MemoryDeleter> ptr, IndexMapper& mapper) :
		_arr(arr), _idx(idx),
		_buffer(std::move(ptr)),
		_mapper(mapper)
	{
		_buffer.reset(malloc(_arr->GetObjectSize()));
	}

public:
	FArrayBaseProxyObject() = delete;
	
	FArrayBaseProxyObject(FArrayBaseProxyObject&& other) noexcept:
		_buffer(std::move(other._buffer)),
		_arr(other._arr),
		_idx(other._idx),
		_mapper(other._mapper),
		_need_flush(false)
	{
		_ASSERT(_need_flush == false);
	}

	size_t ObjectSize() const { return _arr->GetObjectSize(); }
	
	FArrayBaseProxyObject(const FArrayBaseProxyObject& other) :
		_arr(other._arr),
		_idx(other._idx),
		_mapper(other._mapper),
		_need_flush(_need_flush)
	{
		const_cast<FArrayBaseProxyObject&>(other)._need_flush = false;
		_buffer.reset(malloc(_arr->GetObjectSize()));
		memcpy(_buffer.get(), other._buffer.get(), _arr->GetObjectSize());
		
	}

	FArrayBaseProxyObject& operator*()
	{
		return *this;
	}

	bool operator ==(const FArrayBaseProxyObject& other) const
	{
		return _idx == other._idx;
	}

	bool operator !=(const FArrayBaseProxyObject& other) const
	{
		return !this->operator==(other);
	}

	FArrayBaseProxyObject& operator=(FArrayBaseProxyObject&& other) noexcept
	{
		TryFlushIfDataWrite();
		_idx = other._idx;
		_buffer = std::move(other._buffer);
		_arr = other._arr;
		_need_flush = other._need_flush;
		other._need_flush = false;
		return *this;
	}

	FArrayBaseProxyObject& operator=(const FArrayBaseProxyObject& other)
	{
		TryFlushIfDataWrite();
		_idx = other._idx;
		memcpy(_buffer.get(), other._buffer.get(), _arr->GetObjectSize());
		_arr = other._arr;
		_need_flush = other._need_flush;
		const_cast<FArrayBaseProxyObject&>(other)._need_flush = false;
		return *this;
	}

	template <typename Type>
	std::remove_pointer_t<std::remove_cvref_t<Type>>* Get() const
	{
		return (std::remove_pointer_t<std::remove_cvref_t<Type>>*)Get();
	}

	void* Get() const
	{
		return GetData_(_idx);
	}

	template <typename Type>
	void Set(std::remove_pointer_t<std::remove_cvref_t<Type>>* object)
	{
		using type = std::remove_pointer_t<std::remove_cvref_t<Type>>;
#ifdef _DEBUG
		_ASSERT((sizeof(type) * _arr->GetCountElement<type>()) == ObjectSize());
#endif // _DEBUG

		Set(object);
	}

	void Set(void* object)
	{
		memcpy(_buffer.get(), object, _arr->GetObjectSize());
		_need_flush = true;
	}

	FArrayBaseProxyObject& operator++()
	{
		TryFlushIfDataWrite();
		_idx = _mapper.Next(_idx);
		return *this;
	}

	FArrayBaseProxyObject operator++(int)
	{
		auto tmp = *this;
		this->operator++();
		return tmp;
		
	}

	~FArrayBaseProxyObject()
	{
		TryFlushIfDataWrite();
	}

private:

	void* GetData_(size_t idx) const
	{
#ifdef _DEBUG
		_ASSERT(_idx >= 0 && _idx != IndexMapper::npos);
		_ASSERT(_idx < _arr->Count());
#endif 
		_arr->GetAt(_idx, _buffer.get());
		return _buffer.get();
	}

	void TryFlushIfDataWrite()
	{
		if (_need_flush)
		{
			_arr->SetAt(_idx, _buffer.get());
			_need_flush = false;
		}
	}

private:
	std::unique_ptr<void, MemoryDeleter> _buffer;
	FArrayBase<IndexMapper>* _arr;
	IndexMapper& _mapper;
	index_type _idx;
	bool _need_flush;
};


using SparseFArray = FArrayBase<SparseIndexMapper>;
using FArray = FArrayBase<StandartIndexMapper>;