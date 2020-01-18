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
} // namespace hera