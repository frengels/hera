#pragma once

#include <utility>

#include "hera/begin_end.hpp"
#include "hera/concepts.hpp"
#include "hera/iterator/concepts.hpp"
#include "hera/next_prev.hpp"
#include "hera/ranges.hpp"

namespace hera
{
struct accumulate_fn
{
    template<forward_iterator I,
             sentinel_for<I>  S,
             typename T,
             typename BinaryOp>
    constexpr auto operator()(I first, S last, T init, BinaryOp&& op) const
    {
        if constexpr (decltype(first == last)::value)
        {
            return init;
        }
        else
        {
            using op_res_type = decltype(op(std::move(init), *first));

            if constexpr (hera::same_as<T, std::remove_cvref_t<op_res_type>> &&
                          hera::constructible_from<T, op_res_type>)
            {
                init = op(std::move(init), *first);
                return (*this)(hera::next(first),
                               last,
                               std::move(init),
                               std::forward<BinaryOp>(op));
            }
            else
            {
                auto&& next_init = op(std::move(init), *first);
                return (*this)(hera::next(first),
                               last,
                               std::forward<decltype(next_init)>(next_init),
                               std::forward<BinaryOp>(op));
            }
        }
    }

    template<forward_iterator I, sentinel_for<I> S, typename T>
    constexpr auto operator()(I first, S last, T init) const
    {
        return (*this)(std::move(first),
                       std::move(last),
                       std::move(init),
                       [](auto&& lhs, auto&& rhs) {
                           return std::forward<decltype(lhs)>(lhs) +
                                  std::forward<decltype(rhs)>(rhs);
                       });
    }

    template<forward_range R, typename T, typename BinaryOp>
    constexpr auto operator()(R&& r, T init, BinaryOp&& op) const
    {
        return (*this)(hera::begin(r),
                       hera::end(r),
                       std::move(init),
                       std::forward<BinaryOp>(op));
    }

    template<forward_range R, typename T>
    constexpr auto operator()(R&& r, T init) const
    {
        return (*this)(hera::begin(r), hera::end(r), std::move(init));
    }
};

constexpr auto accumulate = accumulate_fn{};
} // namespace hera