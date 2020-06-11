#pragma once

#include "hera/element_type.hpp"
#include "hera/get.hpp"
#include "hera/optional.hpp"
#include "hera/size.hpp"

namespace hera
{
/// \brief if `R` fulfills the `sized` concept it is a valid range
/// \see sized
template<typename R>
concept range = constant_sized<R>;

template<typename R>
concept bounded_range = range<R>&& constant_sized_bounded<R>;

template<typename R>
concept unbounded_range = range<R>&& constant_sized_unbounded<R>;

template<typename R>
concept empty_range = bounded_range<R>&& // clang-format off
    requires
    {
        requires hera::size_v<R> == 0;
    }; // clang-format on

namespace detail
{
template<typename R, std::size_t I>
struct range_out_of_bounds_impl;

template<hera::unbounded_range R, std::size_t I>
struct range_out_of_bounds_impl<R, I> : std::false_type
{};

template<hera::bounded_range R, std::size_t I>
struct range_out_of_bounds_impl<R, I>
    : std::bool_constant<!(I < decltype(hera::size(std::declval<R&>()))::value)>
{};
} // namespace detail

template<typename R, std::size_t I>
concept range_out_of_bounds = detail::range_out_of_bounds_impl<R, I>::value;

template<hera::range T>
inline constexpr bool enable_safe_range = false;

template<typename T>
concept safe_range = range<T> && (std::is_lvalue_reference_v<T> ||
                                  enable_safe_range<std::remove_cvref_t<T>>);

template<typename R, std::size_t I>
concept range_reachable_at = range<R>&& // clang-format off
    requires (R range)
    {
        hera::get<I>(static_cast<R&&>(range));
    }; // clang-format on

template<hera::range R, std::size_t I> // clang-format off
    requires (range_reachable_at<R, I>)
using range_value_at_t = hera::element_type_t<R, I>; // clang-format on

template<typename R, std::size_t I, template<typename> typename Pred>
concept range_at_pred = hera::range<R>&& range_reachable_at<R, I>&&
                                         Pred<range_value_at_t<R, I>>::value;

template<typename R, std::size_t I, typename T>
concept range_at_same_as = hera::range<R>&& range_reachable_at<R, I>&&
                                            hera::same_as<T, range_value_at_t<R, I>>;

namespace detail
{
template<typename R, typename T, std::size_t... Is> // clang-format off
        requires ((hera::range_at_same_as<R, Is, T> && ...))
constexpr void range_values_same_as_iter(std::index_sequence<Is...>) // clang-format on
{}
} // namespace detail

template<typename R, typename T>
concept range_values_same_as = hera::bounded_range<R>&& // clang-format off
    requires
    {
        detail::range_values_same_as_iter<R, T>(
            std::make_index_sequence<hera::size_v<R>>{});
    }; // clang-format on

namespace detail
{
template<typename R,
         template<typename>
         typename Pred,
         std::size_t... Is> // clang-format off
    requires ((hera::range_at_pred<R, Is, Pred> && ...))
constexpr void range_values_pred_iter(std::index_sequence<Is...>) // clang-format on
{}
} // namespace detail

template<typename R, template<typename> typename Pred>
concept range_values_pred = hera::bounded_range<R>&& // clang-format off
    requires
    {
        detail::range_values_pred_iter<R, Pred>(
            std::make_index_sequence<hera::size_v<R>>{});
    }; // clang-format on

template<hera::range R, std::size_t I> // clang-format off
    requires (!hera::range_reachable_at<R, I>)
using range_reference_at_t = decltype(hera::get<I>(std::declval<R&>())); // clang-format on
} // namespace hera