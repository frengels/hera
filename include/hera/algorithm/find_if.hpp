#pragma once

#include <functional>

#include "hera/iterator.hpp"
#include "hera/next_prev.hpp"

namespace hera
{
struct find_if_fn
{
    template<hera::forward_iterator I, hera::sentinel_for<I> S, typename Pred>
    constexpr decltype(auto) operator()(I first, S last, Pred&& pred) const
        noexcept
    {
        // when we've reached the end
        if constexpr (decltype(first == last)::value)
        {
            return first;
        }
        // or when we hit a valid result
        else if constexpr (decltype(std::invoke(pred, *first))::value)
        {
            return first;
        }
        else // otherwise keep on iterating
        {
            return (*this)(
                hera::next(first), std::move(last), std::forward<Pred>(pred));
        }
    }
};

constexpr auto find_if = find_if_fn{};
} // namespace hera