#pragma once

#include <type_traits>
#include <utility>

#include "hera/concepts.hpp"
#include "hera/constant.hpp"
#include "hera/distance.hpp"
#include "hera/next_prev.hpp"
#include "hera/ranges.hpp"

namespace hera
{
template<typename T, typename U>
class pair {
public:
    [[no_unique_address]] T first{};
    [[no_unique_address]] U second{};

public:
    pair() = default;

    template<typename U1, typename U2>
    constexpr pair(U1&& u1,
                   U2&& u2) noexcept(std::is_nothrow_constructible_v<T, U1>&&
                                         std::is_nothrow_constructible_v<U, U2>)
        : first{std::forward<U1>(u1)}, second{std::forward<U2>(u2)}
    {
        // using constructible_from concept doesn't work to constrain somehow
    }

private:
    template<forward_range RT,
             forward_range RU,
             std::size_t... IsT,
             std::size_t... IsU>
    constexpr pair(std::piecewise_construct_t,
                   RT&& args_first,
                   RU&& args_second,
                   std::index_sequence<IsT...>,
                   std::index_sequence<IsU...>)
        : first{*hera::next(hera::begin(args_first),
                            std::integral_constant<std::size_t, IsT>{})...},
          second{*hera::next(hera::begin(args_second),
                             std::integral_constant<std::size_t, IsU>{})...}
    {}

public:
    template<forward_range RT, forward_range RU>
    constexpr pair(std::piecewise_construct_t pw,
                   RT&&                       args_first,
                   RU&&                       args_second)
        : pair(pw,
               std::forward<RT>(args_first),
               std::forward<RU>(args_second),
               std::make_index_sequence<decltype(hera::distance(
                   hera::begin(std::forward<RT>(args_first)),
                   hera::end(std::forward<RT>(args_first))))::value>{},
               std::make_index_sequence<decltype(hera::distance(
                   hera::begin(std::forward<RU>(args_second)),
                   hera::end(std::forward<RU>(args_second))))::value>{})
    {}

    constexpr std::integral_constant<std::size_t, 2> size() const noexcept
    {
        return {};
    }

    constexpr std::false_type empty() const noexcept
    {
        return {};
    }

    template<hera::constant_convertible_to<std::size_t> C> // clang-format off
        requires (C::value < 2) // clang-format on 
        constexpr decltype(auto) operator[](C) & noexcept
    {
        if constexpr (C::value == 0)
        {
            return first;
        }
        else if constexpr (C::value == 1)
        {
            return second;
        }
    }

    template<hera::constant_convertible_to<std::size_t> C> // clang-format off
        requires (C::value < 2) // clang-format on
        constexpr decltype(auto)
        operator[](C) const& noexcept
    {
        if constexpr (C::value == 0)
        {
            return first;
        }
        else if constexpr (C::value == 1)
        {
            return second;
        }
    }

    template<hera::constant_convertible_to<std::size_t> C> // clang-format off
        requires (C::value < 2) // clang-format on
        constexpr decltype(auto)
        operator[](C) &&
        noexcept
    {
        if constexpr (C::value == 0)
        {
            return std::move(first);
        }
        else if constexpr (C::value == 1)
        {
            return std::move(second);
        }
    }

    template<hera::constant_convertible_to<std::size_t> C> // clang-format off
        requires (C::value < 2) // clang-format on
        constexpr decltype(auto)
        operator[](C) const&& noexcept
    {
        if constexpr (C::value == 0)
        {
            return std::move(first);
        }
        else if constexpr (C::value == 1)
        {
            return std::move(second);
        }
    }

    template<std::size_t I> // clang-format off
        requires (I < 2) // clang-format on
        friend constexpr decltype(auto) get(hera::pair<T, U>& p)
    {
        return p[std::integral_constant<std::size_t, I>{}];
    }

    template<std::size_t I> // clang-format off
        requires (I < 2) // clang-format on
        friend constexpr decltype(auto) get(const hera::pair<T, U>& p)
    {
        return p[std::integral_constant<std::size_t, I>{}];
    }

    template<std::size_t I> // clang-format off
        requires (I < 2) // clang-format on
        friend constexpr decltype(auto) get(hera::pair<T, U>&& p)
    {
        return std::move(p)[std::integral_constant<std::size_t, I>{}];
    }

    template<std::size_t I> // clang-format off
        requires (I < 2) // clang-format on
        friend constexpr decltype(auto) get(const hera::pair<T, U>&& p)
    {
        return std::move(p)[std::integral_constant<std::size_t, I>{}];
    }

    constexpr void swap(pair& other)
    {
        using std::swap;
        swap(first, other.first);
        swap(second, other.second);
    }
};

template<typename T, typename U>
pair(T&&, U &&)->pair<std::decay_t<T>, std::decay_t<U>>;
} // namespace hera

namespace std
{
template<typename T, typename U>
struct tuple_size<hera::pair<T, U>> : std::integral_constant<std::size_t, 2>
{};

template<typename T, typename U>
struct tuple_element<0, hera::pair<T, U>>
{
    using type = T;
};

template<typename T, typename U>
struct tuple_element<1, hera::pair<T, U>>
{
    using type = U;
};
} // namespace std