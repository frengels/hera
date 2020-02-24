#pragma once

#include <type_traits>
#include <utility>

#include "hera/concepts.hpp"
#include "hera/constant.hpp"
#include "hera/optional.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"
#include "hera/type_.hpp"

namespace hera
{
template<hera::bounded_range R>
concept pair_like = // clang-format off
    requires (R&& range)
    {
        hera::size(static_cast<R&&>(range));
        requires (decltype(hera::size(static_cast<R&&>(range)))::value == 2);
        hera::get<0>(static_cast<R&&>(range));
        hera::get<1>(static_cast<R&&>(range));
    }; // clang-format on

template<typename T, typename U>
class pair
{
public:
    using first_type  = T;
    using second_type = U;

public:
    [[no_unique_address]] T first{};
    [[no_unique_address]] U second{};

    constexpr std::integral_constant<std::size_t, 2> size() const noexcept
    {
        return {};
    }

    constexpr std::false_type empty() const noexcept
    {
        return {};
    }

    template<std::size_t I> // clang-format off
        requires (I < 2)
    constexpr auto element_type() noexcept // clang-format on
    {
        if constexpr (I == 0)
        {
            return hera::type_<first_type>{};
        }
        else
        {
            return hera::type_<second_type>{};
        }
    }

    template<std::size_t I> // clang-format off
        requires (I < 2)
    constexpr auto element_type() const noexcept // clang-format on
    {
        if constexpr (I == 0)
        {
            return hera::type_<const first_type>{};
        }
        else
        {
            return hera::type_<const second_type>{};
        }
    }

    template<std::size_t I> // clang-format off
        requires (I < 2)
    constexpr decltype(auto) get() & noexcept // clang-format on
    {
        if constexpr (I == 0)
        {
            return static_cast<first_type&>(first);
        }
        else
        {
            return static_cast<second_type&>(second);
        }
    }

    template<std::size_t I> // clang-format off
        requires (I < 2)
    constexpr decltype(auto) get() const & noexcept // clang-format on
    {
        if constexpr (I == 0)
        {
            return static_cast<const first_type&>(first);
        }
        else
        {
            return static_cast<const second_type&>(second);
        }
    }

    template<std::size_t I> // clang-format off
        requires (I < 2)
    constexpr decltype(auto) get() && noexcept // clang-format on
    {
        if constexpr (I == 0)
        {
            return static_cast<first_type&&>(first);
        }
        else
        {
            return static_cast<second_type&&>(second);
        }
    }

    template<std::size_t I> // clang-format off
        requires (I < 2)
    constexpr decltype(auto) get() const && noexcept // clang-format on
    {
        if constexpr (I == 0)
        {
            return static_cast<const first_type&&>(first);
        }
        else
        {
            return static_cast<const second_type&&>(second);
        }
    }

    constexpr void
    swap(pair& other) noexcept(std::is_nothrow_swappable_v<first_type>&&
                                   std::is_nothrow_swappable_v<second_type>)
    {
        using std::swap;
        swap(first, other.first);
        swap(second, other.second);
    }
};

template<typename T, typename U>
pair(T&&, U &&)->pair<std::decay_t<T>, std::decay_t<U>>;

// basically deduce the decayed type of each entry
template<pair_like P>
pair(P&& p)
    ->pair<std::decay_t<decltype(hera::get<0>(static_cast<P&&>(p)))>,
           std::decay_t<decltype(hera::get<1>(static_cast<P&&>(p)))>>;

template<typename T, typename U>
constexpr hera::pair<std::decay_t<T>, std::decay_t<U>>
make_pair(T&& t, U&& u) noexcept(std::is_nothrow_constructible_v<
                                 hera::pair<std::decay_t<T>, std::decay_t<U>>,
                                 T,
                                 U>)
{
    return hera::pair<std::decay_t<T>, std::decay_t<U>>{static_cast<T&&>(t),
                                                        static_cast<U&&>(u)};
}
} // namespace hera