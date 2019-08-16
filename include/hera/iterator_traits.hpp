#pragma once

#include "hera/incrementable_traits.hpp"

namespace hera
{
template<typename I>
struct iterator_traits
{};

template<typename I>
requires requires {
  typename hera::incrementable_traits<I>::difference_type;
  typename hera::readable_traits<I>::value_type;
}
} // namespace hera
