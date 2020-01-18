#pragma once

#include "hera/at.hpp"
#include "hera/size.hpp"

namespace hera
{
template<typename R>
concept range = sized<R>;

template<typename R>
concept bounded_range = range<R>&& bounded_size<R>;

template<typename R>
concept unbounded_range = range<R>&& unbounded_size<R>;

template<typename R>
concept empty_range = bounded_range<R>&& // clang-format off
    requires (const R& r)
    {
        requires decltype(hera::size(r))::value == 0;
    }; // clang-format on

template<hera::range T>
inline constexpr bool enable_safe_range = false;

template<typename T>
concept safe_range = range<T> && (std::is_lvalue_reference_v<T> ||
                                  enable_safe_range<std::remove_cvref_t<T>>);

template<hera::range R, std::size_t I> // clang-format off
    requires !hera::empty_range<R> // clang-format on
    using range_reference_at_t = decltype(hera::at<I>(std::declval<R&>()));
} // namespace hera