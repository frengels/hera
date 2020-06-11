#pragma once

#include "hera/begin_end.hpp"
#include "hera/concepts.hpp"
#include "hera/constant.hpp"
#include "hera/iterator/concepts.hpp"
#include "hera/next_prev.hpp"
#include "hera/ranges.hpp"
#include "hera/type.hpp"

namespace hera
{
template<typename F, typename I>
concept constant_indirect_unary_predicate =
    readable<I>&& constant_predicate<F&, iter_value_t<I>&>&&
                  constant_predicate<F&, iter_reference_t<I>>&&
                  constant_predicate<F&, iter_common_reference_t<I>>;

template<typename F, typename I>
concept indirect_unary_invocable = readable<I>&&
    invocable<F&, iter_value_t<I>&>&& invocable<F&, iter_reference_t<I>>&&
        invocable<F&, iter_common_reference_t<I>>&& common_reference_with<
            std::invoke_result_t<F&, iter_value_t<I>&>,
            std::invoke_result_t<F&, iter_reference_t<I>&&>>;
} // namespace hera