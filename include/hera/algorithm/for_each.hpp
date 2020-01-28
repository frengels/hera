#pragma once

#include "hera/algorithm/unpack.hpp"
#include "hera/ranges.hpp"

namespace hera
{
namespace for_each_impl
{
struct fn
{
public:
    template<hera::range R, typename F>
    constexpr decltype(auto) operator()(R&& range, F fn) const
    {
        return hera::unpack(static_cast<R&&>(range), [&](auto&&... xs) {
            return (fn(static_cast<decltype(xs)&&>(xs)), ...);
        });
    }
};
} // namespace for_each_impl

inline constexpr auto for_each = for_each_impl::fn{};
} // namespace hera