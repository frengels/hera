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
} // namespace hera