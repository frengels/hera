#pragma once

#include "hera/iterator.hpp"
#include "hera/ranges.hpp"

namespace hera
{
struct distance_fn
{
    template<hera::heterogeneous_iterator                       I,
             hera::sentinel_for<I>                              S,
             hera::constant_same_as<hera::iter_difference_t<I>> C>
    constexpr auto increment_until(I curr, S last, C count) const
    {
        if constexpr (curr == last)
        {
            return count;
        }
        else
        {
            return increment_until(
                ++curr,
                last,
                std::integral_constant<iter_difference_t<I>, C::value + 1>{});
        }
    }

    template<hera::heterogeneous_iterator I, hera::sentinel_for<I> S>
    constexpr auto operator()(I first, S last) const
    {
        if constexpr (hera::sized_sentinel_for<S, I>)
        {
            return std::integral_constant<hera::iter_difference_t<I>,
                                          decltype(last - first)::value>{};
        }
        else
        {
            return increment_until(
                first,
                last,
                std::integral_constant<hera::iter_difference_t<I>, 0>{});
        }
    }

    template<typename R> // clang-format off
        requires hera::sized_range<std::remove_cvref_t<R>>
    constexpr decltype(auto) operator()(R&& r) const // clang-format on
        noexcept(noexcept(hera::size(std::forward<R>(r))))
    {
        return std::integral_constant<range_difference_t<R>,
                                      decltype(hera::size(
                                          std::forward<R>(r)))::value>{};
    }

    template<typename R> // clang-format off
        requires !hera::sized_range<std::remove_cvref_t<R>>
    constexpr decltype(auto) operator()(R&& r) const
    {
        return (*this)(hera::begin(std::forward<R>(r)), hera::end(std::forward<R>(r)));
    }
};

constexpr auto distance = distance_fn{};
} // namespace hera