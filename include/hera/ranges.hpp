#pragma once

#include "hera/at.hpp"
#include "hera/begin_end.hpp"
#include "hera/constant.hpp"
#include "hera/size.hpp"

namespace hera
{
template<typename R>
using iterator_t = decltype(hera::begin(std::declval<R&>()));

template<typename R>
using sentinel_t = decltype(hera::end(std::declval<R&>()));

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

// this is for ranges which can be adapted to work with normal_iterator. Primary
// candidates for this are fixed size std structures such as std::tuple<> and
// std::array, as they implement std::get<I> and std::tuple_size.
// anything that can be decomposed as a structured binding will also work.
template<typename R>
concept normal_range = // clang-format off
    requires(R& r)
    {
        hera::size(r);
        detail::at_exists_until(r, hera::size(r));
    }; // clang-format on

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
} // namespace hera