#pragma once

#include "hera/algorithm/unpack.hpp"
#include "hera/metafunction.hpp"
#include "hera/type_identity.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<std::ptrdiff_t I, typename... Ts>
struct type_list_iterator_maybe_value_type
{};

template<std::ptrdiff_t I, typename... Ts> // clang-format off
    requires (I >= 0) &&
        (I < sizeof...(Ts)) // clang-format on
    struct type_list_iterator_maybe_value_type<I, Ts...>
{
    using value_type = hera::type_identity<
        std::tuple_element_t<static_cast<std::size_t>(I), std::tuple<Ts...>>>;
};
} // namespace detail

template<typename... Ts>
class type_list {
private:
    template<std::ptrdiff_t I>
    class iterator : detail::type_list_iterator_maybe_value_type<I, Ts...> {
    public:
        using difference_type = std::ptrdiff_t;

    public:
        iterator() = default;

        template<difference_type J>
        constexpr auto operator==(iterator<J>) const noexcept
        {
            return std::bool_constant<I == J>{};
        }

        template<difference_type J>
        constexpr auto operator!=(iterator<J>) const noexcept
        {
            return std::bool_constant<I != J>{};
        }

        template<difference_type J>
        constexpr auto operator<(iterator<J>) const noexcept
        {
            return std::bool_constant<(I < J)>{};
        }

        template<difference_type J>
        constexpr auto operator>(iterator<J>) const noexcept
        {
            return std::bool_constant<(I > J)>{};
        }

        template<difference_type J>
        constexpr auto operator<=(iterator<J>) const noexcept
        {
            return std::bool_constant<(I <= J)>{};
        }

        template<difference_type J>
        constexpr auto operator>=(iterator<J>) const noexcept
        {
            return std::bool_constant<(I >= J)>{};
        }

        constexpr auto operator++() const noexcept
        {
            return iterator<I + 1>{};
        }

        constexpr auto operator--() const noexcept
        {
            return iterator<I - 1>{};
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr auto operator+(C) const noexcept
        {
            return iterator<I + C::value>{};
        }

        template<hera::constant_convertible_to<difference_type> C>
        friend constexpr auto operator+(C, iterator) noexcept
        {
            return iterator<I + C::value>{};
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr auto operator-(C) const noexcept
        {
            return iterator<I - C::value>{};
        }

        template<difference_type J>
        constexpr auto operator-(iterator<J>) const noexcept
        {
            return std::integral_constant<difference_type, I - J>{};
        }

        template<difference_type J = I> // clang-format off
            requires (J >= 0) && (J < sizeof...(Ts))
        constexpr auto operator*() const noexcept // clang-format on
        {
            return hera::type_identity<
                std::tuple_element_t<static_cast<std::size_t>(I),
                                     std::tuple<Ts...>>>{};
        }

        template<difference_type J = I> // clang-format off
            requires (J >= 0) && (J < sizeof...(Ts))
        friend constexpr auto iter_move(iterator it) noexcept
        {
            return *it;
        }

        template<hera::constant_convertible_to<difference_type>
                     C> // clang-format off
            requires
                requires (const iterator<I>& it, C offset)
                {
                    *(it + offset);
                } // clang-format off
        constexpr auto operator[](C offset) const noexcept
        {
            return *(*this + offset);
        }
    };

public:
    type_list() = default;

    constexpr type_list(hera::type_identity<Ts>...) noexcept
    {}

    constexpr auto begin() const noexcept
    {
        return iterator<0>{};
    }

    constexpr auto end() const noexcept
    {
        return iterator<sizeof...(Ts)>{};
    }

    friend constexpr auto begin(type_list&& tl) noexcept
    {
        return tl.begin();
    }

    friend constexpr auto end(type_list&& tl) noexcept
    {
        return tl.end();
    }

    template<hera::constant_convertible_to<std::size_t> C> // clang-format off
        requires C::value < sizeof...(Ts)
    constexpr auto operator[](C idx) const noexcept // clang-format on
    {
        return begin()[idx];
    }

    template<std::size_t I> // clang-format off
        requires I < sizeof...(Ts)
    friend constexpr auto get(type_list tl) noexcept // clang-format on
    {
        return tl[std::integral_constant<std::size_t, I>{}];
    }
};

template<typename... Ts>
type_list(Ts&&... ts)
    ->type_list<typename decltype(hera::type_identity{
        std::forward<Ts>(ts)})::type...>;

struct to_type_list_fn : detail::pipeable_interface<to_type_list_fn>
{
    template<forward_range R>
    constexpr auto operator()(const R& r) const
    {
        // we can pass everything by const reference because it will internally
        // decay everything anyway
        return hera::unpack(
            r, [](const auto&... xs) { return hera::type_list{xs...}; });
    }
};

constexpr auto to_type_list = to_type_list_fn{};
} // namespace hera

namespace std
{
template<typename... Ts>
struct tuple_size<hera::type_list<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)>
{};

template<std::size_t I, typename... Ts>
struct tuple_element<I, hera::type_list<Ts...>>
{
    using type = decltype(get<I>(std::declval<hera::type_list<Ts...>>()));
};
} // namespace std