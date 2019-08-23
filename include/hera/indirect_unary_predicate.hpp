#pragma once

#include "hera/constant.hpp"
#include "hera/iterator.hpp"

namespace hera
{
template<typename F, typename I>
concept constant_indirect_unary_predicate =
    readable<I>&& constant_predicate<F&, iter_value_t<I>&>&&
                  constant_predicate<F&, iter_reference_t<I>>&&
                  constant_predicate<F&, iter_common_reference_t<I>>;
}