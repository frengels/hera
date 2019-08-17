#pragma once

#include "hera/integral_constant.hpp"

namespace hera
{
template<typename T>
concept bool_constant = integral_constant_for<T, bool>;
}
