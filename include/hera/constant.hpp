#pragma once

#include <type_traits>

#include "hera/concepts.hpp"

namespace hera
{
namespace detail
{
template<typename T, typename U>
concept same_as_no_const = same_as<std::remove_const_t<T>, U>;
}

// clang-format off
template<typename C>
concept constant = 
    requires
    {
        typename C::value_type;
        { C::value } -> detail::same_as_no_const<typename C::value_type>;
    } && std::is_empty_v<C> && std::is_trivial_v<C>;
// clang-format on

template<typename C, typename T>
concept constant_convertible_to =
    constant<C>&& convertible_to<typename C::value_type, T>;

template<typename C, typename T>
concept constant_same_as = constant<C>&& same_as<typename C::value_type, T>;

template<typename T>
concept boolean_constant = constant_convertible_to<T, bool>;
} // namespace hera
