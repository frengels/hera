#pragma once

#include "hera/constant.hpp"
#include "hera/iterator/concepts.hpp"
#include "hera/utility/detail/decay_copy.hpp"

namespace hera
{
struct next_fn
{
    // returns the next following iterator
    template<hera::heterogeneous_iterator I>
    [[nodiscard]] constexpr decltype(auto) operator()(I i) const
        noexcept(noexcept(detail::decay_copy(++i)))
    {
        return detail::decay_copy(++i);
    }

    template<hera::heterogeneous_iterator                              I,
             hera::constant_convertible_to<hera::iter_difference_t<I>> C>
    [[nodiscard]] constexpr decltype(auto) operator()(I i, C n) const
    {
        if constexpr (hera::random_access_iterator<I>)
        {
            return detail::decay_copy(i + n);
        }
        else if constexpr (C::value < 0)
        {
            // if the next count is under 1 then we need to keep calling next
            // until we arrived.
            return (*this)(--i,
                           std::integral_constant<hera::iter_difference_t<I>,
                                                  C::value + 1>{});
        }
        else if constexpr (C::value > 0)
        {
            return (*this)(++i,
                           std::integral_constant<hera::iter_difference_t<I>,
                                                  C::value - 1>{});
        }
        else
        {
            // if the count is 0 then there's no more movement to be done
            return detail::decay_copy(i);
        }
    }

    template<hera::heterogeneous_iterator I, hera::sentinel_for<I> S>
    [[nodiscard]] constexpr decltype(auto) operator()(I i, S bound) const
    {
        // actual incrementing has to take place because a sentinel is not
        // required to ever be dereferenceable.

        if constexpr (hera::sized_sentinel_for<S, I>)
        {
            // call next with the calculated number
            return (*this)(i, bound - i);
        }
        else
        {
            if constexpr (decltype(i == bound)::value)
            {
                return detail::decay_copy(i);
            }
            else
            {
                // increment until we land in the true if case
                return (*this)(++i, bound);
            }
        }
    }

    template<hera::heterogeneous_iterator                              I,
             hera::constant_convertible_to<hera::iter_difference_t<I>> C,
             hera::sentinel_for<I>                                     S,
             hera::constant_convertible_to<hera::iter_difference_t<I>> Count>
    [[nodiscard]] constexpr auto
    next_i_n_bound(I i, C n, S bound, Count cnt) const
    {
        if constexpr ((decltype(i == bound)::value || C::value == 0))
        {
            // return a pair which contains the final iterator and the number of
            // steps required to get there.
            return std::make_pair(std::move(i), std::move(cnt));
        }
        else
        {
            return next_i_n_bound(
                ++i,
                std::integral_constant<hera::iter_difference_t<I>,
                                       C::value - 1>{},
                std::move(bound),
                std::integral_constant<hera::iter_difference_t<I>,
                                       Count::value + 1>{});
        }
    }

    template<hera::heterogeneous_iterator                              I,
             hera::constant_convertible_to<hera::iter_difference_t<I>> C,
             hera::sentinel_for<I>                                     S>
    [[nodiscard]] constexpr decltype(auto) operator()(I i, C n, S bound) const
    {
        return next_i_n_bound(
            std::move(i),
            std::move(n),
            std::move(bound),
            std::integral_constant<hera::iter_difference_t<I>, 0>{});
    }
};

constexpr auto next = next_fn{};

struct prev_fn
{
    template<hera::bidirectional_iterator I>
    [[nodiscard]] constexpr auto operator()(I i) const
        noexcept(noexcept(detail::decay_copy(--i)))
            -> decltype(detail::decay_copy(--i))
    {
        return detail::decay_copy(--i);
    }

    template<hera::bidirectional_iterator                              I,
             hera::constant_convertible_to<hera::iter_difference_t<I>> C>
    [[nodiscard]] constexpr decltype(auto) operator()(I i, C n) const
    {
        return hera::next(
            std::move(i),
            std::integral_constant<hera::iter_difference_t<I>,
                                   -static_cast<hera::iter_difference_t<I>>(
                                       C::value)>{});
    }

    template<hera::bidirectional_iterator                               I1,
             hera::constant_convertible_to<hera::iter_difference_t<I1>> C,
             hera::bidirectional_iterator                               I2>
    [[nodiscard]] constexpr decltype(auto) operator()(I1 i, C n, I2 bound) const
    {
        // if one has to be bidirectional, both should be.
        // sentinel_for is required in the following call to next.
        return hera::next(
            std::move(i),
            std::integral_constant<hera::iter_difference_t<I1>,
                                   -static_cast<hera::iter_difference_t<I1>>(
                                       C::value)>{},
            std::move(bound));
    }
};

constexpr auto prev = prev_fn{};
} // namespace hera