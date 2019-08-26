#pragma once

#include "hera/algorithm/unpack.hpp"
#include "hera/view/detail/closure.hpp"

namespace hera
{
template<typename ContainerFn>
struct to_fn : public detail::pipeable_interface<to_fn<ContainerFn>>
{
    template<forward_range R>
    constexpr auto operator()(R&& r) const
    {
        return hera::unpack(std::forward<R>(r), [](auto&&... xs) {
            return ContainerFn{}(std::forward<decltype(xs)>(xs)...);
        });
    }
};

template<typename ContainerFn>
constexpr auto to = to_fn<ContainerFn>{};
} // namespace hera