#pragma once

#include <type_traits>

#include "hera/concepts.hpp"
#include "hera/incrementable_traits.hpp"
#include "hera/readable_traits.hpp"

namespace hera
{
template<typename I>
using iter_difference_t =
    typename hera::incrementable_traits<I>::difference_type;
template<typename I>
using iter_value_t = typename hera::readable_traits<I>::value_type;

namespace detail
{
template<typename T>
using with_reference = T&;

template<typename T>
concept can_reference = // clang-format off
    requires
    {
	typename with_reference<T>;
    };

// clang-format on

template<typename T, typename U>
concept different_from = !same_as<T, U>;

template<typename T, typename U>
concept remove_cvref_different_from =
    different_from<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
} // namespace detail

// for heterogeneous iterators the weakly_incrementable trait requires a signed
// difference_type and the ability to preincrement the type, yielding a
// different type as result. Different from the std weakly_incrementable is that
// it doesn't require default_constructible, this is because heterogeneous
// iterators can be seen as quasi constant expression. This makes them mostly
// immutable and therefore assigning to would not be possible.
template<typename I>
concept weakly_incrementable = // clang-format off
    requires(I i)
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
// member function.
template<typename I>
concept heterogeneous_iterator = weakly_incrementable<I>;
} // namespace hera
