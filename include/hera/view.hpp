#pragma once

#include "hera/concepts.hpp"
#include "hera/ranges.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<typename T>
constexpr bool deduce_enable_view() noexcept
{
    if constexpr (hera::derived_from<T, hera::view_base>)
    {
        return true;
    }
    else if constexpr (hera::range<T> && hera::range<const T> &&
                       !same_as<range_reference_t<T>,
                                range_reference_t<const T>>)
    {
        return false;
    }
    else
    {
        return true;
    }
}
} // namespace detail

template<typename T>
inline constexpr bool enable_view = detail::deduce_enable_view<T>();

template<typename T>
concept view = range<T>&& enable_view<T>;

template<typename R>
concept viewable_range = range<R> &&
                         (forwarding_range<R> || view<std::decay_t<R>>);
} // namespace hera