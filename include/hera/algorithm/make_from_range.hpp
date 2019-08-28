#pragma once

#include "hera/algorithm/unpack.hpp"
#include "hera/ranges.hpp"

namespace hera
{
namespace detail
{
template<typename T>
struct construct_fn
{
    template<typename... Args>
    constexpr T operator()(Args&&... args) const
        noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        return T(std::forward<Args>(args)...);
    }
};
} // namespace detail

template<typename T>
struct make_from_range_fn
{

    template<hera::forward_range R>
    constexpr T operator()(R&& r) const
        noexcept(noexcept(hera::unpack(std::forward<R>(r),
                                       detail::construct_fn<T>{})))
    {
        return hera::unpack(std::forward<R>(r), detail::construct_fn<T>{});
    }
};

template<typename T>
constexpr auto make_from_range = make_from_range_fn<T>{};
} // namespace hera