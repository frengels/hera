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

template<typename C, auto I>
concept constant_less_than =
    constant_convertible_to<C, decltype(I)>&& C::value < I;

template<typename C, auto I>
concept constant_less_than_equal =
    constant_convertible_to<C, decltype(I)>&& C::value <= I;

template<typename C, auto I>
concept constant_greater_than =
    constant_convertible_to<C, decltype(I)>&& C::value > I;

template<typename C, auto I>
concept constant_greater_than_equal =
    constant_convertible_to<C, decltype(I)>&& C::value >= I;

template<typename T>
concept boolean_constant = constant_convertible_to<T, bool>;

namespace detail
{
template<typename T, typename U>
concept constant_weakly_equality_comparable_with =
    requires(const std::remove_reference_t<T>& t,
             const std::remove_reference_t<U>& u) // clang-format off
    {
        { t == u } -> constant_same_as<bool>;
        { t != u } -> constant_same_as<bool>;
        { u == t } -> constant_same_as<bool>;
        { u != t } -> constant_same_as<bool>;
    }; // clang-format on
} // namespace detail

template<typename T>
concept constant_equality_comparable =
    detail::constant_weakly_equality_comparable_with<T, T>;

template<typename T>
concept constant_totally_ordered =
    constant_equality_comparable<T>&& // clang-format off
    requires(
        const std::remove_reference_t<T>& a,
        const std::remove_reference_t<T>& b)
    {
        { a < b } -> constant_same_as<bool>;
        { a > b } -> constant_same_as<bool>;
        { a <= b } -> constant_same_as<bool>;
        { a >= b } -> constant_same_as<bool>;
    }; // clang-format on
} // namespace hera
