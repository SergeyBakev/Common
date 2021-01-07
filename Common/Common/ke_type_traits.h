#pragma once
#include <type_traits>
#include <vector>

template <typename>
struct is_vector_default_alloc : std::false_type
{
	using type = void;
};

template <typename _Ty>
struct is_vector_default_alloc<std::vector<_Ty>> : std::true_type
{
	using type = typename _Ty;
};


	
/*<T, typename std::enable_if_t<std::is_trivial_v<T>>> : std::true_type {};*/

//template<class T, class Enable = void>
//struct is_vector_of_trivial : std::false_type {};

