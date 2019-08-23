#pragma once

#include <functional>

#include "hera/indirect_unary_predicate.hpp"
#include "hera/iterator.hpp"
#include "hera/metafunction.hpp"
#include "hera/next_prev.hpp"
#include "hera/type_identity.hpp"

namespace hera
{
struct find_if_fn
{
    template<hera::forward_iterator I,
             hera::sentinel_for<I>  S,
             hera::metafunction     Pred> // clang-format off
        requires hera::constant_indirect_unary_predicate<typename Pred::type, I>
    constexpr decltype(auto) operator()(I first, S last, Pred) const noexcept // clang-format on
    {
        using predicate_type = typename Pred::type;

        if constexpr (decltype(first == last)::value)
        {
            return first;
        }
        else if constexpr (decltype(std::invoke(std::declval<predicate_type&>(),
                                                *first))::value)
        {
            return first;
        }
        else
        {
            return (*this)(hera::next(first),
                           std::move(last),
                           hera::type_identity<predicate_type>{});
        }
    }

    template<hera::forward_iterator                     I,
             hera::sentinel_for<I>                      S,
             hera::constant_indirect_unary_predicate<I> Pred>
    constexpr decltype(auto) operator()(I first, S last, Pred) const noexcept
    {
        return (*this)(
            std::move(first), std::move(last), hera::type_identity<Pred>{});
    }
};

constexpr auto find_if = find_if_fn{};
} // namespace hera