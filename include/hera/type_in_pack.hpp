#pragma once

#include "hera/algorithm/accumulate.hpp"
#include "hera/algorithm/any_of.hpp"
#include "hera/algorithm/find_if.hpp"
#include "hera/container/integer_sequence.hpp"
#include "hera/container/type_list.hpp"
#include "hera/nth_element.hpp"
#include <initializer_list>
#include <numeric>

namespace hera
{
template<typename U, typename... Ts>
inline constexpr std::size_t type_in_pack_count_v = []() {
    std::initializer_list<std::size_t> il = {
        (std::size_t) std::is_same_v<U, Ts>...};
    return std::accumulate(il.begin(), il.end(), 0);
}();

template<typename U, typename... Ts>
concept type_in_pack = type_in_pack_count_v<U, Ts...> > 0;

template<typename U, typename... Ts>
concept type_in_pack_unique = type_in_pack_count_v<U, Ts...> == 1;

namespace detail
{
template<typename U, typename... Ts> // clang-format off
    requires hera::type_in_pack_unique<U, Ts...>
constexpr std::size_t type_in_pack_index_impl() // clang-format on
{
    return *hera::find_if(hera::type_list<Ts...>{}, [](auto el_type) {
        using type = typename decltype(el_type)::type;

        return std::is_same<U, type>{};
    });
}
} // namespace detail

template<typename U, typename... Ts>
inline constexpr std::size_t type_in_pack_index_v =
    detail::type_in_pack_index_impl<U, Ts...>();
} // namespace hera