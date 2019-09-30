#pragma once

#include "hera/iter_move.hpp"
#include "hera/iterator/dereferenceable.hpp"
#include "hera/iterator/incrementable.hpp"

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

} // namespace hera