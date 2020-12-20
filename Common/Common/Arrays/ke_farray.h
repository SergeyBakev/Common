#pragma once
#include "ke_farray_base.h"


template <class U, class IndexMapper> class FArrayObject;

template <class T, class IndexMapper = SparseIndexMapper>
class FArray : protected FArrayBase<IndexMapper>
{
public:
	using index_type = typename typename IndexMapper::external_idx_type;
	using inner_index_type = typename typename IndexMapper::inner_idx_type;
	using Base = FArrayBase<IndexMapper>;
	using value_type = T;
	const size_t value_size = sizeof(T);

	FArray()
	{
		Base::SetObjectSize(value_size);
	}

	void SetAt(const index_type& index, const value_type& value)
	{
		Base::SetAt(index, &const_cast<T&>(value));
	}

	value_type GetAt(const index_type& index) const
	{
		value_type ret = {-1,-1,-1,-1};
		Base::GetAt(index, &ret);
		return ret;
	}

	index_type Add(const value_type& value)
	{
		return Base::Add(&const_cast<value_type&>(value));
	}

	void push_back(const value_type& data)
	{
		Add(data);
	}

    value_type at(const index_type& index)
	{
		return GetAt(index);
	}

	size_t size() const
	{
		return Count();
	}

	FArrayObject<T,IndexMapper> operator [](int index)
	{
		auto idx = GetMapper().GetRealIndex(index);
		if (idx == IndexMapper::npos)
			throw std::exception();
		return FArrayObject<T, IndexMapper>();
	}

	using Base::CreateNew;

	using Base::AssignExsitArray;

	using Base::Count;

	using Base::SetTemporary;

	using Base::IsTemporary;

	using Base::Clear;

	using Base::GetFileName;

	using Base::GetMapper;

	using Base::Contains;

};

template <class U, class IndexMapper = SparseIndexMapper>
class FArrayObject
{
	std::shared_ptr<FArray<U, IndexMapper>> _arr = nullptr;
	mutable typename IndexMapper::external_idx_type _index = IndexMapper::npos;
	U _data{};
	bool _need_flush = false;

	template <class U, class M> friend class HFArray;
protected:

	

	FArrayObject(std::shared_ptr<FArray<U, IndexMapper>> ptr, typename IndexMapper::external_idx_type index) : _arr(ptr), _index(index)
	{
		_data = _arr->GetAt(_index);
	}

	void Flush()
	{
		_arr->SetAt(_index, _data);
}

public:

	FArrayObject(const FArrayObject& other) = default;
	FArrayObject(FArrayObject&& other) = default;

	
	U*  operator ->() noexcept
	{
		return &_data;
	}

	FArrayObject& operator = (const U& value)
	{
		_need_flush = true;
		_data = value;
		return *this;
	}

	FArrayObject& operator = (const U&& value)
	{
		_need_flush = true;
		_data = std::move(value);
		return *this;
	}

	U operator *()
	{
		return Current();
	}


	U Current()
	{
		return _data;
	}

	FArrayObject Next()
	{
		typename IndexMapper::external_idx_type index = _arr->GetMapper().GetNext(_index);
		return FArrayObject(_arr, index);
	}

	operator bool()
	{
		return _index != -1;
	}

	FArrayObject& operator= (const FArrayObject& other)
	{
		_data = other._data;
		_index = other._index;
		_arr = other._arr;
		return *this;
	}

	~FArrayObject()
	{
		if (_need_flush)
			Flush();
	}
};

template <class T, class IndexMapper = SparseIndexMapper>
class HFArray
{
public:
	using ArrayObject = FArrayObject < T, IndexMapper>;
	FArray<T, IndexMapper>* operator ->() const noexcept
	{
		return _impl.get();
	}

	ArrayObject First() const
	{
		auto first_index = _impl->GetMapper().GetFirst();
		return ArrayObject(_impl, first_index);
	}

	ArrayObject Last() const
	{
		auto last_index = _impl->GetMapper().GetLast();

		return ArrayObject(_impl, last_index);
	}

	ArrayObject AppendNew()
	{
		auto index = _impl->GetMapper().CreateNewIndex();
		return ArrayObject(_impl, index);
	}

	HFArray()
	{
		_impl = std::make_shared<FArray<T, IndexMapper>>();
	}

	ArrayObject operator [](typename IndexMapper::external_idx_type index)
	{
		auto idx = _impl->GetMapper().GetRealIndex(index);
		if (idx == IndexMapper::npos)
			throw std::exception();
		return ArrayObject(_impl, idx);
	}
private:
	std::shared_ptr<FArray<T, IndexMapper>> _impl;
};

struct FArrayCRC32
{
	template <class T>
	static uint32_t DataCRC(const HFArray<T>& data, uint32_t current_remainder)
	{
		auto current = data.First();
		while (current)
		{
			std::function<uint32_t(uint32_t)> func = std::bind(&T::DataCRC, current.Current(), std::placeholders::_1);
			current_remainder = func(current_remainder);
			current = current.Next();
		}
		return current_remainder;
	}
};