#pragma once

#include "hera/begin_end.hpp"

namespace hera
{
template<typename R>
using iterator_t = decltype(hera::begin(std::declval<R&>()));

template<typename R>
using sentinel_t = decltype(hera::end(std::declval<R&>()));
} // namespace hera