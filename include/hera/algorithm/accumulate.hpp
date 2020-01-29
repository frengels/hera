#pragma once

#include <utility>

#include "hera/algorithm/unpack.hpp"
#include "hera/ranges.hpp"

namespace hera
{
namespace accumulate_impl
{
struct fn
{
public:
    template<hera::bounded_range R, typename T>
    constexpr T operator()(R&& range, T init) const
    {
        auto add_to_init = [&](auto&& x) { init = std::move(init) + x; };

        hera::unpack(static_cast<R&&>(range), [&](auto&&... xs) {
            (add_to_init(static_cast<decltype(xs)&&>(xs)), ...);
        });

        return init;
    }

    template<hera::bounded_range R, typename T, typename BinaryOp>
    constexpr T operator()(R&& range, T init, BinaryOp&& op) const
    {
        auto add_to_init = [&](auto&& x) { init = op(std::move(init), x); };

        hera::unpack(static_cast<R&&>(range), [&](auto&&... xs) {
            (add_to_init(static_cast<decltype(xs)&&>(xs)), ...);
        });

        return init;
    }
};
} // namespace accumulate_impl

inline namespace cpo
{
inline constexpr auto accumulate = accumulate_impl::fn{};
}
} // namespace hera