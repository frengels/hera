#pragma once

#include <type_traits>

namespace hera
{
template<typename From, typename To>
concept convertible_to = std::is_convertible_v<From, To>&& requires(From (&f)())
{
    static_cast<To>(f());
};
} // namespace hera
