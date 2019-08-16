#pragma once

#include "hera/integral_constant.hpp"
#include "hera/same_as.hpp"

namespace hera
{
template<typename T>
concept bool_constant = integral_constant_for<T, bool>;
}
