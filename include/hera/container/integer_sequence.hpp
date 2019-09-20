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
class integer_sequence : public view_interface<integer_sequence<T, Is...>> {
public:
    using type       = integer_sequence;
    using value_type = T;

private:
    template<std::ptrdiff_t I>
    class iterator
        : public detail::
              integer_sequence_iterator_value_type<I, sizeof...(Is), T> {
    public:
        using difference_type = std::ptrdiff_t;

    public:
        iterator() = default;

        template<difference_type J>
        constexpr auto operator==(iterator<J>) const noexcept
            -> std::bool_constant<I == J>
        {
            return {};
        }

        template<difference_type J>
        constexpr auto operator!=(iterator<J>) const noexcept
            -> std::bool_constant<I != J>
        {
            return {};
        }

        template<difference_type J>
        constexpr auto operator<(iterator<J>) const noexcept
            -> std::bool_constant<(I < J)>
        {
            return {};
        }

        template<difference_type J>
        constexpr auto operator<=(iterator<J>) const noexcept
            -> std::bool_constant<(I <= J)>
        {
            return {};
        }

        template<difference_type J>
        constexpr auto operator>(iterator<J>) const noexcept
            -> std::bool_constant<(I > J)>
        {
            return {};
        }

        template<difference_type J>
        constexpr auto operator>=(iterator<J>) const noexcept
            -> std::bool_constant<(I >= J)>
        {
            return {};
        }

        constexpr auto operator++() const noexcept -> iterator<I + 1>
        {
            return {};
        }

        constexpr auto operator--() const noexcept -> iterator<I - 1>
        {
            return {};
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr auto operator+(C) const noexcept -> iterator<I + C::value>
        {
            return {};
        }

        template<hera::constant_convertible_to<difference_type> C>
        friend constexpr auto operator+(C, iterator<I>) noexcept
            -> iterator<C::value + I>
        {
            return {};
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr auto operator-(C) const noexcept -> iterator<I - C::value>
        {
            return {};
        }

        template<difference_type J>
        constexpr auto operator-(iterator<J>) const noexcept
            -> std::integral_constant<difference_type, (I - J)>
        {
            return {};
        }

        template<difference_type J = I> // clang-format off
            requires (J < sizeof...(Is))
        constexpr auto operator*() const noexcept // clang-format on
        {
            // returns the correct integral_constant
            return typename detail::ith_value<J, T, Is...>::type{};
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr decltype(auto) operator[](C n) const noexcept
        {
            return *((*this) + n);
        }
    };

public:
    constexpr auto begin() const noexcept -> iterator<0>
    {
        return {};
    }

    constexpr auto end() const noexcept -> iterator<sizeof...(Is)>
    {
        return {};
    }

    friend constexpr auto begin(integer_sequence seq) noexcept
    {
        return seq.begin();
    }

    friend constexpr auto end(integer_sequence seq) noexcept
    {
        return seq.end();
    }

    constexpr auto size() const noexcept
        -> std::integral_constant<std::size_t, sizeof...(Is)>
    {
        return {};
    }

    constexpr auto empty() const noexcept
        -> std::bool_constant<sizeof...(Is) == 0>
    {
        return {};
    }

    template<hera::constant_convertible_to<std::size_t> C>
    constexpr auto operator[](C n) const noexcept
    {
        return begin()[n];
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