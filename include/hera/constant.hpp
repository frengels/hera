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

template<typename C>
inline constexpr auto constant_value_v = std::remove_cvref_t<C>::value;

// clang-format off
template<typename C>
concept constant = 
    requires
    {
        typename C::value_type;
        { hera::constant_value_v<C> }
            -> detail::same_as_no_const<typename C::value_type>;
    } && std::is_empty_v<C> && std::is_trivial_v<C>;
// clang-format on

template<typename T>
concept character = // clang-format off
    requires
    {
        typename std::char_traits<T>::char_type;
    }; // clang-format off

template<typename T>
concept constant_char = constant<T>&& character<typename T::value_type>;

template<typename C, typename T>
concept constant_convertible_to =
    constant<C>&& convertible_to<typename C::value_type, T>;

template<typename C, typename T>
concept constant_same_as = constant<C>&& same_as<typename C::value_type, T>;

template<typename C, auto X>
concept constant_equal_to = constant_same_as<C, decltype(X)> &&
                            (X == hera::constant_value_v<C>);


template<typename C, auto I>
concept constant_less_than = constant_convertible_to<C, decltype(I)> &&
                             (C::value < I);

template<typename C, auto I>
concept constant_less_than_equal = constant_convertible_to<C, decltype(I)> &&
                                   (C::value <= I);

template<typename C, auto I>
concept constant_greater_than = constant_convertible_to<C, decltype(I)> &&
                                (C::value > I);

template<typename C, auto I>
concept constant_greater_than_equal = constant_convertible_to<C, decltype(I)> &&
                                      (C::value >= I);

template<typename C>
concept constant_boolean = constant<C>&& boolean<typename C::value_type>;

namespace detail
{
template<typename T, typename U>
concept constant_weakly_equality_comparable_with =
    requires(const std::remove_reference_t<T>& t,
             const std::remove_reference_t<U>& u) // clang-format off
    {
        { t == u };
        requires constant_boolean<decltype(t == u)>;
        { t != u };
        requires constant_boolean<decltype(t != u)>;
        { u == t };
        requires constant_boolean<decltype(u == t)>;
        { u != t };
        requires constant_boolean<decltype(u != t)>;
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
        { a < b };
        requires constant_boolean<decltype(a < b)>;
        { a > b };
        requires constant_boolean<decltype(a > b)>;
        { a <= b };
        requires constant_boolean<decltype(a <= b)>;
        { a >= b };
        requires constant_boolean<decltype(a >= b)>;
    }; // clang-format on

template<typename F, typename... Args>
concept constant_predicate = regular_invocable<F, Args...>&&
    constant_same_as<std::invoke_result_t<F, Args...>, bool>;

inline constexpr auto true_  = std::true_type{};
inline constexpr auto false_ = std::false_type{};

} // namespace hera
