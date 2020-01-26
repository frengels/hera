#pragma once

#include "hera/ranges.hpp"

namespace std
{
// this will not override any other specializations because they are more
// specialized than this one
template<hera::bounded_range R>
struct tuple_size<R>
    : std::integral_constant<std::size_t,
                             decltype(
                                 hera::size(std::declval<const R&>()))::value>
{};
} // namespace std