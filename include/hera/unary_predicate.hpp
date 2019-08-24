#pragma once

#include "hera/begin_end.hpp"
#include "hera/constant.hpp"
#include "hera/iterator.hpp"
#include "hera/next_prev.hpp"
#include "hera/ranges.hpp"
#include "hera/type_identity.hpp"

namespace hera
{
template<typename F, typename I>
concept constant_indirect_unary_predicate =
    readable<I>&& constant_predicate<F&, iter_value_t<I>&>&&
                  constant_predicate<F&, iter_reference_t<I>>&&
                  constant_predicate<F&, iter_common_reference_t<I>>;
} // namespace hera