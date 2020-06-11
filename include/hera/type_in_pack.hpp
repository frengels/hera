#pragma once

#include "hera/algorithm/accumulate.hpp"
#include "hera/algorithm/any_of.hpp"
#include "hera/algorithm/find_if.hpp"
#include "hera/container/integer_sequence.hpp"
#include "hera/container/type_list.hpp"
#include "hera/nth_element.hpp"

namespace hera
{

namespace detail
{
template<typename U, typename... Ts>
constexpr bool type_in_pack_impl()
{
    return decltype(hera::any_of(hera::type_list<Ts...>{}, [](auto el_type) {
        using type = typename decltype(el_type)::type;
        return std::is_same<type, U>{};
    }))::value;
}

template<typename U, typename... Ts>
constexpr bool type_in_pack_unique_impl()
{
    constexpr std::integral_constant<std::size_t, 0> start_count{};
    constexpr auto                                   count = hera::accumulate(
        hera::type_list<Ts...>{}, start_count, [](auto count, auto type) {
            if constexpr (std::is_same_v<U, typename decltype(type)::type>)
            {
                return std::integral_constant<std::size_t,
                                              decltype(count)::value + 1>{};
            }
            else
            {
                return count;
            }
        });

    return decltype(count)::value == 1; // 1 guarantees present and unique
}
} // namespace detail

template<typename U, typename... Ts>
concept type_in_pack = detail::type_in_pack_impl<U, Ts...>();

template<typename U, typename... Ts>
concept type_in_pack_unique = type_in_pack<U, Ts...>&&
detail::type_in_pack_unique_impl<U, Ts...>();

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
inline constexpr std::size_t
    type_in_pack_index_v = detail::type_in_pack_index_impl<U, Ts...>();
} // namespace hera