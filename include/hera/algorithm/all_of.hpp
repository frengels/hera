#pragma once

#include "hera/algorithm/find_if.hpp"
#include "hera/ranges.hpp"

namespace hera
{
namespace all_of_impl
{
struct fn
{
    template<hera::bounded_range R, typename Pred>
    constexpr auto operator()(R&& range, Pred p) const noexcept
    {
        auto res = hera::find_if(static_cast<R&&>(range), [&](auto&&... xs) {
            auto accept = p(static_cast<decltype(xs)&&>(xs)...);
            return std::bool_constant<!decltype(accept)::value>{};
        });

        return res.transform([](auto) { return std::false_type{}; })
            .value_or(std::true_type{});
    }
};
} // namespace all_of_impl

inline constexpr auto all_of = all_of_impl::fn{};
} // namespace hera