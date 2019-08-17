#pragma once

#include "hera/constant.hpp"

namespace hera
{
template<typename I>
struct incrementable_traits
{};

template<typename T>
struct incrementable_traits<const T> : incrementable_traits<T>
{};

// clang-format off
template<typename T>
    requires requires { typename T::difference_type; }
struct incrementable_traits<T>
{
    // clang-format on
    using difference_type = typename T::difference_type;
};

// clang-format off
template<typename T>
    requires(!requires { typename T::difference_type; }) &&
    requires(const T& a, const T& b)
{
    { a - b } -> hera::integral_constant;
}
// clang-format on
struct incrementable_traits<T>
{
    using difference_type = std::make_signed_t<typename decltype(
        std::declval<const T&>() - std::declval<const T&>())::value_type>;
};
} // namespace hera
