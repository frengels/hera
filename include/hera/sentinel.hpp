#pragma once

#include "hera/concepts.hpp"
#include "hera/constant.hpp"
#include "hera/iterator.hpp"

namespace hera
{
namespace detail
{
template<typename T, typename U>
concept constant_weakly_equality_comparable_with =
    requires(const std::remove_reference_t<T>& t,
             const std::remove_reference_t<U>& u) // clang-format off
    {
        { t == u } -> constant_convertible_to<bool>;
        { t != u } -> constant_convertible_to<bool>;
        { u == t } -> constant_convertible_to<bool>;
        { u != t } -> constant_convertible_to<bool>;
    }; // clang-format on
} // namespace detail

template<typename S, typename I>
concept sentinel_for = heterogeneous_iterator<I>&&
    detail::constant_weakly_equality_comparable_with<S, I>;

template<typename S, typename I>
concept sized_sentinel_for = sentinel_for<S, I>&& // clang-format off
    requires(const I& i, const S& s)
    {
        { s - i } -> hera::constant_same_as<iter_difference_t<I>>;
        { i - s } -> hera::constant_same_as<iter_difference_t<I>>;
    }; // clang-format on
} // namespace hera