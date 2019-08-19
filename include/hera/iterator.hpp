#pragma once

#include <type_traits>

#include "hera/concepts.hpp"
#include "hera/incrementable_traits.hpp"
#include "hera/iter_move.hpp"
#include "hera/readable_traits.hpp"

namespace hera
{
template<typename I>
using iter_difference_t =
    typename hera::incrementable_traits<I>::difference_type;
template<typename I>
using iter_value_t = typename hera::readable_traits<I>::value_type;

template<dereferenceable T>
using iter_reference_t = decltype(*std::declval<T&>());

template<dereferenceable T> // clang-format off
    requires requires(T& t)
    {
        { hera::iter_move(t) } -> can_reference;
    } // clang-format on
using iter_rvalue_reference_t = decltype(hera::iter_move(std::declval<T&>()));

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

template<typename Out, typename T>
concept writable = // clang-format off
    requires(Out&& o, T&& t) {
        *o = std::forward<T>(t);
        *std::forward<Out>(o) = std::forward<T>(t);
        const_cast<const iter_reference_t<Out>&&>(*o) =
            std::forward<T>(t);
        const_cast<const iter_reference_t<Out>&&>(*std::forward<T>(t)) =
            std::forward<T>(t);
    };

// for heterogeneous iterators the weakly_incrementable trait requires a signed
// difference_type and the ability to preincrement the type, yielding a
// different type as result. Different from the std weakly_incrementable is that
// it doesn't require default_constructible, this is because heterogeneous
// iterators can be seen as quasi constant expression. This makes them mostly
// immutable and therefore assigning to would not be possible.
template<typename I>
concept constant_incrementable = // clang-format off
    requires(const I& i)
    {
        typename hera::iter_difference_t<I>;
        // requires detail::signed_integer_like<hera::iter_difference_t<I>>;
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
} // namespace hera
