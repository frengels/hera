#pragma once

#include "hera/element_type.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"

namespace std
{
// this will not override any other specializations because they are more
// specialized than this one
template<hera::bounded_range R>
struct tuple_size<R> : std::integral_constant<std::size_t, hera::size_v<R>>
{};

// implement if element_type is a valid function on the range
template<std::size_t I, hera::range R> // clang-format off
    requires 
        requires (R& r)
        {
            hera::element_type<I>(r);
        } // clang-format on
struct tuple_element<I, R>
{
    using type = hera::element_type_t<I, R>;
};
} // namespace std