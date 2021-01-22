#pragma once
#include "ke_farray_base.h"


template <class U, class IndexMapper> class FArrayObject;

template <class T, class IndexMapper = SparseIndexMapper>
class FArray : protected FArrayBase<IndexMapper>
{
public:
	static_assert(std::is_trivial_v<T>, "Typed array should contains only trivial data");
	using index_type = typename typename IndexMapper::external_idx_type;
	using inner_index_type = typename typename IndexMapper::inner_idx_type;
	using iterator = FArrayObject<T, IndexMapper>;
	using Base = FArrayBase<IndexMapper>;
	using value_type = T;
	const size_t value_size = sizeof(T);


	FArray()
	{
		Base::SetObjectSize(value_size);
	}

	void Add(const value_type& value)
	{	
		Base::Add(&const_cast<value_type&>(value));
	}

	void SetAt(const index_type& index, const value_type& value)
	{
		Base::SetAt(index,&const_cast<value_type&>(value));
	}

	value_type GetAt(const index_type& index)
	{
		value_type v{};
		Base::GetAt(index, &v);
		return v;
	}

	bool CreateNew(const TCHAR* file_name, bool isTemp = true)
	{
		Base::CreateNew(file_name, isTemp);
		Base::SetObjectSize(value_size);
	}

	iterator begin() { return iterator(Base::begin()); }

	iterator end() { return iterator(Base::end()); }

	value_type operator [](index_type idx)
	{
		return GetAt(idx);
	}

	using Base::Count;

	using Base::GetMapper;

	using Base::Clear;

	using Base::Contains;

	using Base::SetTemporary;

	using Base::GetFileName;

	using Base::IsTemporary;

	using Base::Serialize;

	using Base::Deserialize;

};

template<class T,class IndexMapper>
class FArrayObject : protected FArrayBaseProxyObject<IndexMapper>
{
public:
	template <class,class> friend class FArray;
	using index_type = typename IndexMapper::external_idx_type;
	using inner_index_type = typename IndexMapper::inner_idx_type;
	using value_type = T;
	using pointer = value_type*;
	using reference = value_type&;
	using Base = FArrayBaseProxyObject<IndexMapper>;

public:
	FArrayObject() = delete;

	reference operator*()
	{
		this->_need_flush = true;
		return *(this->template Get<value_type>());
	}

	reference operator*() const
	{
		this->_need_flush = true;
		return  *(this->template Get<value_type>());
	}

	bool operator ==(const FArrayObject& other) const
	{
		return this->Equals(other);
	}

	bool operator !=(const FArrayObject& other) const
	{
		return !this->Equals(other);
	}

	FArrayObject(const FArrayObject& other) : Base(other)
	{
		
	}

	FArrayObject(FArrayObject&& other) : Base(other)
	{

	}

	FArrayObject operator=(const FArrayObject& other)
	{
		Base::operator=(other);
		return *this;
	}

	FArrayObject operator=(FArrayObject&& other)
	{
		Base::operator=(other);
		return *this;
	}

	reference operator=(const value_type& obj)
	{
		Base::template Set<value_type>(&obj);
		return *this;
	}

	FArrayObject operator++()
	{
		Base::operator++();
		return *this;
	}

	FArrayObject operator++(int)
	{
		auto tmp = *this;
		Base::operator++();
		return tmp;

	}
private:

	FArrayObject(const Base& base) : Base(base)
	{

	}

	FArrayObject(Base&& base) : Base(base)
	{

	}
};