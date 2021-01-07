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



};
