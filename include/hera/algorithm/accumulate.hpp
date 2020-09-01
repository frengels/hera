#pragma once

#include "hera/ranges.hpp"

namespace hera
{
namespace accumulate_impl
{
struct fn
{
private:
    template<std::size_t         I,
             hera::bounded_range R,
             typename T,
             typename BinaryOp>
        static constexpr decltype(auto)
        recurse(R&& range, T&& accu, BinaryOp&& op) requires(I <
                                                             hera::size_v<R>) &&
        (std::is_invocable_v<BinaryOp,
                             T,
                             decltype(hera::get<I>(static_cast<R&&>(range)))>)

    {
        return recurse<I + 1>(
            static_cast<R&&>(range),
            op(static_cast<T&&>(accu), hera::get<I>(static_cast<R&&>(range))),
            static_cast<BinaryOp&&>(op));
    }

    template<std::size_t         I,
             hera::bounded_range R,
             typename T,
             typename BinaryOp>
    static constexpr decltype(auto)
    recurse(R&& range, T&& final_res, BinaryOp&&) requires(I == hera::size_v<R>)
    {
        return static_cast<T&&>(final_res);
    }

public:
    template<hera::bounded_range R, typename T>
    constexpr decltype(auto) operator()(R&& range, T&& init) const
    {
        return recurse<0>(static_cast<R&&>(range),
                          static_cast<T&&>(init),
                          [](auto&& accu, auto&& curr) {
                              return static_cast<decltype(accu)&&>(accu) +
                                     static_cast<decltype(curr)&&>(curr);
                          });
    }

    template<hera::bounded_range R, typename T, typename BinaryOp>
    constexpr decltype(auto)
    operator()(R&& range, T&& init, BinaryOp&& op) const
    {
        return recurse<0>(static_cast<R&&>(range),
                          static_cast<T&&>(init),
                          static_cast<BinaryOp&&>(op));
    }
};
} // namespace accumulate_impl

inline constexpr auto accumulate = accumulate_impl::fn{};
} // namespace hera