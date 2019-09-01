#pragma once

#include "hera/begin_end.hpp"
#include "hera/iterator.hpp"
#include "hera/metafunction.hpp"
#include "hera/next_prev.hpp"
#include "hera/ranges.hpp"
#include "hera/type_identity.hpp"

namespace hera
{
struct all_of_fn
{
    template<bool                   B,
             hera::forward_iterator I,
             hera::sentinel_for<I>  S,
             hera::metafunction PredMeta> // clang-format off
    static constexpr auto invoke(I first, S last, PredMeta pm) noexcept // clang-format on
    {
        if constexpr (decltype(first == last)::value)
        {
            return std::bool_constant<B>{};
        }
        else
        {
            using invoke_res = std::invoke_result_t<typename PredMeta::type,
                                                    iter_reference_t<I>>;
            return invoke<(B && invoke_res::value)>(
                hera::next(first), std::move(last), std::move(pm));
        }
    }

    template<hera::forward_iterator I,
             hera::sentinel_for<I>  S,
             hera::metafunction     PredMeta>
    constexpr auto operator()(I first, S last, PredMeta pm) const noexcept
        -> decltype(invoke<true>(std::move(first),
                                 std::move(last),
                                 std::move(pm)))
    {
        return invoke<true>(std::move(first), std::move(last), std::move(pm));
    }

    template<hera::forward_iterator I, hera::sentinel_for<I> S, typename Pred>
    constexpr auto operator()(I first, S last, Pred) const noexcept
        -> decltype(invoke<true>(std::move(first),
                                 std::move(last),
                                 hera::type_identity<Pred>{}))
    {
        return invoke<true>(
            std::move(first), std::move(last), hera::type_identity<Pred>{});
    }

    template<hera::forward_range R, typename Pred>
    constexpr auto operator()(R&& r, Pred) const noexcept -> decltype(
        invoke<true>(hera::begin(r), hera::end(r), hera::type_identity<Pred>{}))
    {
        return invoke<true>(
            hera::begin(r), hera::end(r), hera::type_identity<Pred>{});
    }

    template<hera::forward_range R, hera::metafunction PredMeta>
    constexpr auto operator()(R&& r, PredMeta pm) const noexcept
        -> decltype(invoke<true>(hera::begin(r), hera::end(r), std::move(pm)))
    {
        return invoke<true>(hera::begin(r), hera::end(r), std::move(pm));
    }
};

constexpr auto all_of = all_of_fn{};
} // namespace hera