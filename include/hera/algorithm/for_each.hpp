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
        return hera::unpack(std::forward<R>(range), [&](auto&&... xs) {
            return (fn(std::forward<decltype(xs)>(xs)), ...);
        });
    }
};
} // namespace for_each_impl

inline constexpr auto for_each = for_each_impl::fn{};
} // namespace hera