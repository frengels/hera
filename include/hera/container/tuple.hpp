#pragma once

#include <utility>

#include "hera/concepts.hpp"
#include "hera/next_prev.hpp"
#include "hera/sentinel.hpp"

namespace hera
{
namespace detail
{
template<std::ptrdiff_t I, typename... Ts>
struct tuple_iterator_maybe_value_type
{};

template<std::ptrdiff_t I, typename... Ts> // clang-format off
    requires (I >= 0) && (I < sizeof...(Ts))
struct tuple_iterator_maybe_value_type<I, Ts...> // clang-format on
{
    using value_type = std::remove_cvref_t<
        std::tuple_element_t<static_cast<std::size_t>(I), std::tuple<Ts...>>>;
};

template<std::size_t, typename T>
struct tuple_box
{
private:
    [[no_unique_address]] T value_;

public:
    template<typename... Args>
    constexpr tuple_box(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : value_{std::forward<Args>(args)...}
    {}

    constexpr T& get() & noexcept
    {
        return static_cast<T&>(value_);
    }

    constexpr const T& get() const& noexcept
    {
        return static_cast<const T&>(value_);
    }

    constexpr T&& get() && noexcept
    {
        return static_cast<T&&>(value_);
    }

    constexpr const T&& get() const&& noexcept
    {
        return static_cast<const T&&>(value_);
    }
};

template<typename Indices, typename... Ts>
class tuple_impl;

template<std::size_t... Is, typename... Ts>
class tuple_impl<std::index_sequence<Is...>, Ts...>
    : protected tuple_box<Is, Ts>... {
    template<typename... Us> // clang-format off
        requires (sizeof...(Ts) == sizeof...(Us)) // clang-format on
        constexpr tuple_impl(Us&&... us) noexcept(
            (std::is_nothrow_constructible_v<Ts, Us> && ...))
        : tuple_box<Is, Ts>{std::forward<Us>(us)}...
    {}
};
} // namespace detail

template<typename... Ts>
class tuple
    : private detail::tuple_impl<std::index_sequence_for<Ts...>, Ts...> {
private:
    template<std::ptrdiff_t I, bool Const>
    class iterator_base : detail::tuple_iterator_maybe_value_type<I, Ts...> {
    private:
        template<std::ptrdiff_t J, bool Const_>
        friend class iterator_base;

        using tuple_type = std::conditional_t<Const, const tuple, tuple>;

    public:
        using difference_type = std::ptrdiff_t;

    private:
        [[no_unique_address]] tuple_type& parent_;

    public:
        constexpr iterator_base(tuple_type& tup) noexcept : parent_{tup}
        {}

        template<bool OtherConst> // clang-format off
            requires !OtherConst && Const // clang-format on
            constexpr iterator_base(
                const iterator_base<I, OtherConst>& other) noexcept
            : parent_{other.parent_}
        {}

        template<difference_type J, bool UConst>
        constexpr auto operator==(iterator_base<J, UConst>) const noexcept
        {
            return std::bool_constant<I == J>{};
        }

        constexpr auto operator==(hera::bounded_sentinel) const noexcept
        {
            return std::bool_constant<I == sizeof...(Ts)>{};
        }

        friend constexpr auto operator==(hera::bounded_sentinel,
                                         iterator_base<I, Const>) noexcept
        {
            return std::bool_constant<sizeof...(Ts) == I>{};
        }

        template<difference_type J, bool UConst>
        constexpr auto operator!=(iterator_base<J, UConst>) const noexcept
        {
            return std::bool_constant<I != J>{};
        }

        constexpr auto operator!=(hera::bounded_sentinel) const noexcept
        {
            return std::bool_constant<I != sizeof...(Ts)>{};
        }

        friend constexpr auto operator!=(hera::bounded_sentinel,
                                         iterator_base<I, Const>) noexcept
        {
            return std::bool_constant<sizeof...(Ts) != I>{};
        }

        template<difference_type J, bool UConst>
        constexpr auto operator<(iterator_base<J, UConst>) const noexcept
        {
            return std::bool_constant<(I < J)>{};
        }

        constexpr auto operator<(hera::bounded_sentinel) const noexcept
        {
            return std::bool_constant<(I < sizeof...(Ts))>{};
        }

        friend constexpr auto operator<(hera::bounded_sentinel,
                                        iterator_base<I, Const>) noexcept
        {
            return std::bool_constant<(sizeof...(Ts) < I)>{};
        }

        template<difference_type J, bool UConst>
        constexpr auto operator>(iterator_base<J, UConst>) const noexcept
        {
            return std::bool_constant<(I > J)>{};
        }

        constexpr auto operator>(hera::bounded_sentinel) const noexcept
        {
            return std::bool_constant<(I > sizeof...(Ts))>{};
        }

        friend constexpr auto operator>(hera::bounded_sentinel,
                                        iterator_base<I, Const>) noexcept
        {
            return std::bool_constant<(sizeof...(Ts) > I)>{};
        }

        template<difference_type J, bool UConst>
        constexpr auto operator<=(iterator_base<J, UConst>) const noexcept
        {
            return std::bool_constant<(I <= J)>{};
        }

        constexpr auto operator<=(hera::bounded_sentinel) const noexcept
        {
            return std::bool_constant<(sizeof...(Ts) <= I)>{};
        }

        friend constexpr auto operator<=(hera::bounded_sentinel,
                                         iterator_base<I, Const>) noexcept
        {
            return std::bool_constant<(sizeof...(Ts) <= I)>{};
        }

        template<difference_type J, bool UConst>
        constexpr auto operator>=(iterator_base<J, UConst>) const noexcept
        {
            return std::bool_constant<(I >= J)>{};
        }

        constexpr auto operator>=(hera::bounded_sentinel) const noexcept
        {
            return std::bool_constant<(I >= sizeof...(Ts))>{};
        }

        friend constexpr auto operator>=(hera::bounded_sentinel,
                                         iterator_base<I, Const>) noexcept
        {
            return std::bool_constant<(sizeof...(Ts) >= I)>{};
        }

        constexpr auto operator++() const noexcept
        {
            return iterator_base<I + 1, Const>{parent_};
        }

        constexpr auto operator--() const noexcept
        {
            return iterator_base<I - 1, Const>{parent_};
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr auto operator+(C) const noexcept
        {
            return iterator_base<I + C::value, Const>{parent_};
        }

        template<hera::constant_convertible_to<difference_type> C>
        friend constexpr auto
        operator+(C, const iterator_base<I, Const> it) noexcept
        {
            return iterator_base<I + C::value, Const>{it.parent_};
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr auto operator-(C) const noexcept
        {
            return iterator_base<I - C::value, Const>{parent_};
        }

        template<difference_type J, bool UConst>
        constexpr auto operator-(iterator_base<J, UConst>) const noexcept
        {
            return std::integral_constant<difference_type, I - J>{};
        }

        constexpr auto operator-(hera::bounded_sentinel) const noexcept
        {
            return std::integral_constant<difference_type, I - sizeof...(Ts)>{};
        }

        friend constexpr auto operator-(hera::bounded_sentinel,
                                        iterator_base<I, Const>) noexcept
        {
            return std::integral_constant<difference_type, sizeof...(Ts) - I>{};
        }

        template<difference_type J = I> // clang-format off
            requires(J >= 0) &&
                (J < sizeof...(Ts)) 
        constexpr decltype(auto) operator*() const // clang-format on
            noexcept
        {
            return parent_[std::integral_constant<std::size_t,
                                                  static_cast<std::size_t>(
                                                      I)>{}];
        }

        template<hera::constant_convertible_to<difference_type>
                     C> // clang-format off
            requires
                requires (const iterator_base<I, Const>& it, C offset)
                {
                    *(it + offset);
                } // clang-format on
        constexpr decltype(auto) operator[](C offset) const noexcept
        {
            return *(*this + offset);
        }
    };

public:
    template<std::ptrdiff_t I>
    class iterator : public iterator_base<I, false> {
        using iterator_base<I, false>::iterator_base;
    };

    template<std::ptrdiff_t I>
    class const_iterator : public iterator_base<I, true> {
        using iterator_base<I, true>::iterator_base;
    };

public:
    using detail::tuple_impl<std::index_sequence_for<Ts...>, Ts...>::tuple_impl;

    constexpr auto begin() noexcept
    {
        return iterator<0>{*this};
    }

    constexpr auto begin() const noexcept
    {
        return const_iterator<0>{*this};
    }

    constexpr auto cbegin() const noexcept
    {
        return const_iterator<0>{*this};
    }

    constexpr auto end() const noexcept
    {
        return hera::bounded_sentinel{};
    }

    constexpr auto cend() const noexcept
    {
        return hera::bounded_sentinel{};
    }

    constexpr std::integral_constant<std::size_t, sizeof...(Ts)> size() const
        noexcept
    {
        return {};
    }

    template<std::size_t N = sizeof...(Ts)> // clang-format off
        requires(N >= 1) // clang-format on
        constexpr decltype(auto) front() noexcept
    {
        return *begin();
    }

    template<std::size_t N = sizeof...(Ts)> // clang-format off
        requires (N >= 1) // clang-format on
        constexpr decltype(auto) front() const noexcept
    {
        return *begin();
    }

    template<std::size_t N = sizeof...(Ts)> // clang-format off
        requires (N >= 1) // clang-format on
        constexpr decltype(auto) back() noexcept
    {
        return *(hera::prev(begin() + size()));
    }

    template<std::size_t N = sizeof...(Ts)> // clang-format off
        requires (N >= 1) // clang-format on
        constexpr decltype(auto) back() const noexcept
    {
        return *(hera::prev(begin() + size()));
    }

    constexpr std::bool_constant<sizeof...(Ts) == 0> empty() const noexcept
    {
        return {};
    }

    template<hera::constant_convertible_to<std::size_t> C> // clang-format off
        requires(C::value < sizeof...(Ts)) // clang-format on
        constexpr decltype(auto)
        operator[](C) &
        noexcept
    {
        return static_cast<detail::tuple_box<
            C::value,
            std::tuple_element_t<C::value, std::tuple<Ts...>>>&>(*this)
            .get();
    }

    template<hera::constant_convertible_to<std::size_t> C> // clang-format off
        requires(C::value < sizeof...(Ts)) // clang-format on
        constexpr decltype(auto)
        operator[](C) const& noexcept
    {
        return static_cast<const detail::tuple_box<
            C::value,
            std::tuple_element_t<C::value, std::tuple<Ts...>>>&>(*this)
            .get();
    }

    template<hera::constant_convertible_to<std::size_t> C> // clang-format off
        requires (C::value < sizeof...(Ts)) // clang-format on
        constexpr decltype(auto)
        operator[](C) &&
        noexcept
    {
        return static_cast<detail::tuple_box<
            C::value,
            std::tuple_element_t<C::value, std::tuple<Ts...>>>&&>(*this)
            .get();
    }

    template<hera::constant_convertible_to<std::size_t> C> // clang-format off
        requires (C::value < sizeof...(Ts)) // clang-format on
        constexpr decltype(auto)
        operator[](C) const&& noexcept
    {
        return static_cast<const detail::tuple_box<
            C::value,
            std::tuple_element_t<C::value, std::tuple<Ts...>>>&&>(*this)
            .get();
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts)) // clang-format on
        friend constexpr decltype(auto) get(tuple& t) noexcept
    {
        return t[std::integral_constant<std::size_t, I>{}];
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts)) // clang-format on
        friend constexpr decltype(auto) get(const tuple& t) noexcept
    {
        return t[std::integral_constant<std::size_t, I>{}];
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts)) // clang-format on
        friend constexpr decltype(auto) get(tuple&& t) noexcept
    {
        return std::move(t)[std::integral_constant<std::size_t, I>{}];
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts)) // clang-format on
        friend constexpr decltype(auto) get(const tuple&& t) noexcept
    {
        return std::move(t)[std::integral_constant<std::size_t, I>{}];
    }
};

template<typename... Ts>
tuple(Ts&&...)->tuple<std::decay_t<Ts>...>;

template<typename... Ts>
constexpr hera::tuple<Ts&&...> forward_as_tuple(Ts&&... ts) noexcept(
    std::is_nothrow_constructible_v<hera::tuple<Ts&&...>, Ts...>)
{
    return hera::tuple<Ts&&...>{std::forward<Ts>(ts)...};
}

template<typename... Ts>
constexpr hera::tuple<std::decay_t<Ts>...> make_tuple(Ts&&... ts) noexcept(
    std::is_nothrow_constructible_v<hera::tuple<std::decay_t<Ts>...>, Ts...>)
{
    return hera::tuple<std::decay_t<Ts>...>{std::forward<Ts>(ts)...};
}
} // namespace hera

namespace std
{
template<typename... Ts>
struct tuple_size<hera::tuple<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)>
{};

template<std::size_t I, typename... Ts> // clang-format off
    requires (I < sizeof...(Ts)) // clang-format on
    struct tuple_element<I, hera::tuple<Ts...>>
{
    using type = std::tuple_element_t<I, std::tuple<Ts...>>;
};
} // namespace std