#pragma once

#include "hera/container/integer_sequence.hpp"

namespace hera
{
namespace detail
{
template<std::size_t, typename T>
struct indexed
{
    using type = T;
};

template<typename Is, typename... Ts>
struct indexer;

template<std::size_t... Is, typename... Ts>
struct indexer<hera::index_sequence<Is...>, Ts...> : indexed<Is, Ts>...
{};

template<std::size_t I, typename T>
static indexed<I, T> select(indexed<I, T>)
{}
} // namespace detail

template<std::size_t I, typename... Ts> // clang-format off
    requires (I < sizeof...(Ts))
using nth_element_t = typename decltype( // clang-format on
    select<I>(detail::indexer<hera::index_sequence_for<Ts...>, Ts...>{}))::type;
} // namespace hera