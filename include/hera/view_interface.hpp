#pragma once

#include <type_traits>

#include "hera/same_as.hpp"

namespace hera
{
struct view_base
{};

template<typename D>
requires std::is_class_v<D>&&
    same_as<D, std::remove_cv_t<D>> class view_interface {};
} // namespace hera