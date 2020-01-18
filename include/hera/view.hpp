#pragma once

#include "hera/concepts.hpp"
#include "hera/ranges.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<typename T>
concept container_like =
    hera::range<T>&& hera::range<const T> && !hera::empty_range<T> &&
    !same_as<hera::range_reference_at_t<T, 0>,
             hera::range_reference_at_t<const T, 0>>;

template<typename T>
concept enable_view_default =
    derived_from<T, hera::view_base> || !container_like<T>;
} // namespace detail

template<typename T>
inline constexpr bool enable_view = detail::enable_view_default<T>;

template<typename T>
concept view = range<T>&& enable_view<T>;

template<typename R>
concept viewable_range = range<R> && (safe_range<R> || view<std::decay_t<R>>);
} // namespace hera