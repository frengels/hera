#pragma once

#include <type_traits>

#include "hera/same_as.hpp"

namespace hera
{
// clang-format off
template<typename T>
concept integral_constant = 
    requires
    {
	typename T::value_type;
	requires std::is_integral_v<typename T::value_type>;
	{ T::value } -> typename T::value_type;
    } && std::is_empty_v<T> && std::is_trivial_v<T>;
// clang-format on

template<typename Integral, typename T>
concept integral_constant_for = std::is_integral_v<T>&&
    integral_constant<Integral>&& same_as<T, typename Integral::value_type>;
} // namespace hera
