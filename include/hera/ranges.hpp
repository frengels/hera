#pragma once

#include "hera/at.hpp"
#include "hera/begin_end.hpp"
#include "hera/constant.hpp"
#include "hera/iterator/concepts.hpp"
#include "hera/sentinel.hpp"
#include "hera/size.hpp"

namespace hera
{
namespace detail
{
template<typename R, std::size_t I>
concept has_at_for = requires(R&&                                           r,
                              const std::integral_constant<std::size_t, I>& c)
{
    hera::at(std::forward<R>(r), c);
};

template<hera::constant_convertible_to<std::size_t> Curr,
         hera::constant_convertible_to<std::size_t> End,
         typename R> // clang-format off
    requires Curr::value == End::value 
constexpr void at_exists_until_impl(const Curr&, const End&, R&&)
{} // clang-format on

template<hera::constant_convertible_to<std::size_t> Curr,
         hera::constant_convertible_to<std::size_t> End,
         typename R> // clang-format off
    requires (Curr::value != End::value) && has_at_for<R, Curr::value>
constexpr void at_exists_until_impl(const Curr& current, const End& end, R&& r) noexcept
{
    // clang-format on
    at_exists_until_impl(std::integral_constant<std::size_t, Curr::value + 1>{},
                         end,
                         std::forward<R>(r));
}

// will perform the at call until we hit the integral constant passed. This is
// to verify that all elements can be accessed through the hera::at function.
template<typename R, hera::constant_convertible_to<std::size_t> C>
constexpr auto at_exists_until(R&& r, const C& end) noexcept -> decltype(
    hera::detail::at_exists_until_impl(std::integral_constant<std::size_t, 0>{},
                                       end,
                                       std::forward<R>(r)))
{}
} // namespace detail

namespace detail
{
template<typename R>
concept range_impl = requires(R&& r)
{
    hera::begin(std::forward<R>(r));
    hera::end(std::forward<R>(r));
};
} // namespace detail

template<typename R>
concept range = detail::range_impl<R&>;

template<range R>
using iterator_t = decltype(hera::begin(std::declval<R&>()));

template<range R>
using sentinel_t = decltype(hera::end(std::declval<R&>()));

template<range R>
using range_difference_t = iter_difference_t<iterator_t<R>>;

template<range R>
using range_value_t = iter_value_t<iterator_t<R>>;

template<range R>
using range_reference_t = iter_reference_t<iterator_t<R>>;

template<range R>
using range_rvalue_reference_t = iter_rvalue_reference_t<iterator_t<R>>;

template<typename R>
concept forwarding_range = range<R>&& detail::range_impl<R>;

template<typename R>
concept unbounded_range =
    range<R>&& unbounded_sentinel_for<sentinel_t<R>, iterator_t<R>>;

template<typename R>
concept bounded_range =
    range<R>&& bounded_sentinel_for<sentinel_t<R>, iterator_t<R>>;

template<typename R>
concept empty_range = range<R>&& requires(R& r)
{
    requires decltype(hera::begin(r) == hera::end(r))::value;
};

template<typename R>
concept sized_range = range<R>&& // clang-format off
    requires(R& r)
    {
        hera::size(r);
    }; // clang-format on

template<typename R>
concept forward_range = range<R>&& forward_iterator<iterator_t<R>>;

template<typename R>
concept bidirectional_range =
    forward_range<R>&& bidirectional_iterator<iterator_t<R>>;

template<typename R>
concept random_access_range =
    bidirectional_range<R>&& random_access_iterator<iterator_t<R>>;
} // namespace hera