#pragma once

#include "hera/algorithm/unpack.hpp"
#include "hera/ranges.hpp"
#include "hera/view.hpp"
#include "hera/view/detail/closure.hpp"

namespace hera
{
template<template<typename...> typename Container>
struct to_fn : public detail::pipeable_interface<to_fn<Container>>
{
    template<hera::bounded_range R> // clang-format off
        requires hera::viewable_range<R>
    constexpr auto operator()(R&& range) const // clang-format on
    {
        return hera::unpack(static_cast<R&&>(range), [](auto&&... xs) {
            return Container(static_cast<decltype(xs)&&>(xs)...);
        });
    }
};

/// Collect elements from a view/range into a container
template<template<typename...> typename Container>
inline constexpr auto to = to_fn<Container>{};
} // namespace hera