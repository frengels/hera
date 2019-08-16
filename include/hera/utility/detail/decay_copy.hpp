#pragma once

#include <type_traits>

namespace hera
{
namespace detail
{
template<typename T>
constexpr std::decay_t<T>
decay_copy(T&& t) noexcept(noexcept(static_cast<T&&>(t)))
{
    return static_cast<T&&>(t);
}
} // namespace detail
} // namespace hera
