#pragma once

#include <type_traits>

namespace hera
{
namespace detail
{
template<typename T>
struct cond_value_type
{};

// clang-format off
template<typename T>
    requires std::is_object_v<T>
struct cond_value_type<T>
{
    // clang-format on
    using value_type = std::remove_cv_t<T>;
};
} // namespace detail

template<typename I>
struct readable_traits
{};

// clang-format off
template<typename I>
    requires std::is_array_v<I>
struct readable_traits<I>
{
    // clang-format on
    using value_type = std::remove_cv_t<std::remove_extent_t<I>>;
};

template<typename I>
struct readable_traits<const I> : readable_traits<I>
{};

// clang-format off
template<typename I>
    requires requires { typename I::value_type; }
// clang-format on
struct readable_traits<I> : detail::cond_value_type<typename I::value_type>
{};

// clang-format off
template<typename I>
    requires requires { typename I::element_type; }
// clang-format on
struct readable_traits<I> : detail::cond_value_type<typename I::element_type>
{};
} // namespace hera
