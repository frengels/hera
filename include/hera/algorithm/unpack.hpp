#pragma once

#include "hera/distance.hpp"
#include "hera/iterator/sentinel.hpp"
#include "hera/next_prev.hpp"
#include "hera/ranges.hpp"

namespace hera
{
struct unpack_fn
{
    template<typename DiffT, DiffT... Is, hera::forward_iterator I, typename F>
    static constexpr decltype(auto) invoke_with_sequence(
        std::integer_sequence<DiffT, Is...>,
        const I i,
        F&&     fn) noexcept(noexcept(std::
                                      forward<F>(fn)(*hera::next(
                                          i,
                                          std::integral_constant<DiffT,
                                                                 Is>{})...)))
    {
        return std::forward<F>(fn)(
            *hera::next(i, std::integral_constant<DiffT, Is>{})...);
    }

    template<hera::forward_iterator        I,
             hera::bounded_sentinel_for<I> S,
             typename F>
    constexpr decltype(auto) operator()(I i, const S s, F&& fn) const
        noexcept(noexcept(invoke_with_sequence(
            std::make_integer_sequence<iter_difference_t<I>,
                                       decltype(hera::distance(i, s))::value>{},
            std::move(i),
            std::forward<F>(fn))))
    {
        return invoke_with_sequence(
            std::make_integer_sequence<iter_difference_t<I>,
                                       decltype(hera::distance(i, s))::value>{},
            std::move(i),
            std::forward<F>(fn));
    }

    template<hera::bounded_range R, typename F>
    constexpr decltype(auto) operator()(R&& r, F&& fn) const
        noexcept(noexcept((*this)(hera::begin(std::forward<R>(r)),
                                  hera::end(std::forward<R>(r)),
                                  std::forward<F>(fn))))
    {
        return (*this)(hera::begin(std::forward<R>(r)),
                       hera::end(std::forward<R>(r)),
                       std::forward<F>(fn));
    }
};

constexpr auto unpack = unpack_fn{};
} // namespace hera