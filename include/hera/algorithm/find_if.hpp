#pragma once

#include <functional>

#include "hera/constant.hpp"
#include "hera/indirect_unary.hpp"
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
             hera::metafunction     PredMeta> // clang-format off
    static constexpr decltype(auto)
    invoke(I first, S last, PredMeta pm) noexcept
    {
        using predicate_type = typename PredMeta::type;

        if constexpr (decltype(first == last)::value)
        {
            return first;
        }
        else if constexpr (std::invoke_result_t<predicate_type,
                                                decltype(*first)>::value)
        {
            return first;
        }
        else
        {
            return invoke(hera::next(first), std::move(last), std::move(pm));
        }
    }

    template<hera::forward_iterator I, hera::sentinel_for<I> S, typename Pred>
    constexpr auto operator()(I first, S last, Pred p) const noexcept(noexcept(
        invoke(std::move(first), std::move(last), hera::type_identity{p})))
    {
        return invoke(
            std::move(first), std::move(last), hera::type_identity{p});
    }

    // use find_if on a range
    template<hera::forward_range R, hera::metafunction PredMeta>
    constexpr auto operator()(R&& r, PredMeta predm) const noexcept
        -> decltype(invoke(hera::begin(std::forward<R>(r)),
                           hera::end(std::forward<R>(r)),
                           predm))
    {
        return invoke(hera::begin(std::forward<R>(r)),
                      hera::end(std::forward<R>(r)),
                      predm);
    }

    template<hera::forward_range R, typename Pred>
    constexpr auto operator()(R&& r, Pred) const noexcept
        -> decltype(invoke(hera::begin(std::forward<R>(r)),
                           hera::end(std::forward<R>(r)),
                           hera::type_identity<Pred>{}))
    {
        return invoke(hera::begin(std::forward<R>(r)),
                      hera::end(std::forward<R>(r)),
                      hera::type_identity<Pred>{});
    }
};

constexpr auto find_if = find_if_fn{};
} // namespace hera