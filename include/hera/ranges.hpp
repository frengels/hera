#pragma once

#include "hera/size.hpp"

namespace hera
{
template<typename R>
concept range = sized<R>;

template<typename R>
concept bounded_range = range<R>&& bounded_size<R>;

template<typename R>
concept unbounded_range = range<R>&& unbounded_size<R>;

template<typename R>
concept empty_range = bounded_range<R>&& // clang-format off
    requires (const R& r)
    {
        requires decltype(hera::size(r))::value == 0;
    }; // clang-format on
} // namespace hera