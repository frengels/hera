#pragma once

#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<std::ptrdiff_t I, std::ptrdiff_t Until, typename T>
struct integer_sequence_iterator_value_type
{};

template<std::ptrdiff_t I, std::ptrdiff_t Until, typename T> // clang-format off
    requires (I < Until) && (I >= 0)
struct integer_sequence_iterator_value_type<I, Until, T> // clang-format on
{
    using value_type = T;
};

template<std::size_t, typename T, T... Ns>
struct ith_value;

template<std::size_t I, typename T, T First, T... Ns>
struct ith_value<I, T, First, Ns...> : ith_value<I - 1, T, Ns...>
{};

template<typename T, T Val, T... Rest>
struct ith_value<0, T, Val, Rest...>
{
    using type = std::integral_constant<T, Val>;
};
} // namespace detail

template<typename T, T... Is>
class integer_sequence : public view_interface<integer_sequence<T, Is...>>
{
public:
    using type       = integer_sequence;
    using value_type = T;

public:
    constexpr std::integral_constant<std::size_t, sizeof...(Is)> size() const
        noexcept
    {
        return {};
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Is))
    constexpr auto get() const noexcept // clang-format on
    {
        return typename detail::ith_value<I, value_type, Is...>::type{};
    }
};

template<std::size_t... Is>
using index_sequence = integer_sequence<std::size_t, Is...>;

namespace detail
{

// create an integer sequence with logarithmic complexity.
// The use of integral_constant is necessary to avoid a template parameter
// error.
template<typename T, typename Seq1, typename Seq2>
struct merge_and_renumber;

template<typename T, T... I1s, T... I2s>
struct merge_and_renumber<T,
                          hera::integer_sequence<T, I1s...>,
                          hera::integer_sequence<T, I2s...>>
{
    using type = hera::integer_sequence<T, I1s..., (sizeof...(I1s) + I2s)...>;
};

template<typename IntegralConstant>
struct sequence_gen;

template<typename T, T N>
struct sequence_gen<std::integral_constant<T, N>>
    : detail::merge_and_renumber<
          T,
          typename sequence_gen<std::integral_constant<T, (N / 2)>>::type,
          typename sequence_gen<std::integral_constant<T, (N - N / 2)>>::type>
{};

template<typename T>
struct sequence_gen<std::integral_constant<T, 0>>
{
    using type = hera::integer_sequence<T>;
};

template<typename T>
struct sequence_gen<std::integral_constant<T, 1>>
{
    using type = hera::integer_sequence<T, 0>;
};
} // namespace detail

template<typename T, T N>
using make_integer_sequence =
    typename detail::sequence_gen<std::integral_constant<T, N>>::type;

template<std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;

template<typename... Ts>
using index_sequence_for = make_index_sequence<sizeof...(Ts)>;
} // namespace hera