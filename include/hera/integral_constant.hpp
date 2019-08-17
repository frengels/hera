#pragma once

#include <type_traits>

#include "hera/concepts.hpp"

namespace hera
{
// clang-format off
template<typename C>
concept integral_constant = 
    requires
    {
	typename C::value_type;
	requires std::is_integral_v<typename C::value_type>;
	{ C::value } -> typename C::value_type;
    } && std::is_empty_v<C> && std::is_trivial_v<C>;
// clang-format on

template<typename C, typename T>
concept integral_constant_for = std::is_integral_v<T>&& integral_constant<C>&&
                                                        convertible_to<typename C::value_type, T>;
} // namespace hera
