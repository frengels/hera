#pragma once

#include "hera/concepts.hpp"
#include "hera/constant.hpp"
#include "hera/iterator/incrementable.hpp"
#include "hera/iterator/readable.hpp"
#include "hera/iterator/sentinel.hpp"
#include "hera/iterator/traits.hpp"

namespace hera
{
namespace detail
{
template<typename T, typename U>
concept different_from = !same_as<T, U>;

template<typename T, typename U>
concept remove_cvref_different_from =
    different_from<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
} // namespace detail

template<typename In>
concept readable = // clang-format off
    requires
    {
        typename iter_value_t<In>;
        typename iter_reference_t<In>;
        typename iter_rvalue_reference_t<In>;
    } // clang-format on
&&common_reference_with<iter_reference_t<In>&&, iter_value_t<In>&>&&
        common_reference_with<iter_reference_t<In>&&,
                          iter_rvalue_reference_t<In>&&>&&
        common_reference_with<iter_rvalue_reference_t<In>&&,
                              const iter_value_t<In>&>;

template<readable T>
using iter_common_reference_t =
    common_reference_t<iter_reference_t<T>, iter_value_t<T>&>;

template<typename Out, typename T>
concept writable = // clang-format off
    requires(Out&& o, T&& t) {
        *o = std::forward<T>(t);
        *std::forward<Out>(o) = std::forward<T>(t);
        const_cast<const iter_reference_t<Out>&&>(*o) =
            std::forward<T>(t);
        const_cast<const iter_reference_t<Out>&&>(*std::forward<Out>(o)) =
            std::forward<T>(t);
    }; // clang-format on

// the constant incrementable requires that the following type is different from
// the current. This is a key property of heterogeneous iteration as the current
// position and type has to be encoded in the iterator's type. Post
// incrementation is never used because the implied semantics are very different
// from what we can offer.
template<typename I>
concept constant_incrementable = // clang-format off
    requires(const I& i)
    {
        typename hera::iter_difference_t<I>;
        requires std::is_signed_v<hera::iter_difference_t<I>>;
        { ++i } -> detail::remove_cvref_different_from<I>;
    };
// clang-format on

// A heterogeneous iterator requires the heterogeneous weakly_incrementable
// concept to ensure each following iterator has a different type.
// it does not need to be dereferenceable as it's able to
// detect at compile time whether the dereferencing is valid and disable the
// member function. This can only happen with the begin iterator if the range is
// empty.
template<typename I>
concept heterogeneous_iterator = constant_incrementable<I>;

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

template<typename I>
concept forward_iterator = heterogeneous_iterator<I>&& sentinel_for<I, I>;

template<typename I>
concept bidirectional_iterator = forward_iterator<I>&& // clang-format off
    requires(const I i)
    {
        { --i } -> detail::remove_cvref_different_from<I>;
    }; // clang-format on

namespace detail
{
template<typename I>
concept random_access_iterator_nonrecursive = bidirectional_iterator<I>&&
    constant_totally_ordered<I>&& sized_sentinel_for<I, I>&& // clang-format off
    requires(
        const I i,
        const std::integral_constant<iter_difference_t<I>, 1>& n)
    {
        { i + n } -> detail::remove_cvref_different_from<I>;
        { n + i } -> detail::remove_cvref_different_from<I>;
        { i - n } -> detail::remove_cvref_different_from<I>;
    }; // clang-format on
} // namespace detail

template<typename I>
concept random_access_iterator = bidirectional_iterator<I>&&
    constant_totally_ordered<I>&& sized_sentinel_for<I, I>&& // clang-format off
    requires(
        const I                                         i,
        std::integral_constant<iter_difference_t<I>, 1> n)
    {
        { i + n } -> detail::random_access_iterator_nonrecursive;
        { n + i } -> detail::random_access_iterator_nonrecursive;
        { i - n } -> detail::random_access_iterator_nonrecursive;
    }; // clang-format on

template<typename I>
concept unbounded_iterator =
    heterogeneous_iterator<I>&& sentinel_for<hera::unbounded_sentinel, I>;

template<typename I>
concept bounded_iterator = heterogeneous_iterator<I> && !unbounded_iterator<I>;

template<typename S, typename I>
concept bounded_sentinel_for = bounded_iterator<I>&& sentinel_for<S, I>;

template<typename S, typename I>
concept unbounded_sentinel_for = unbounded_iterator<I>&&
    same_as<hera::unbounded_sentinel,
            S>; // sentinel_for checked in unbounded_iterator
} // namespace hera