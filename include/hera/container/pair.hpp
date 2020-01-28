#pragma once

#include <type_traits>
#include <utility>

#include "hera/concepts.hpp"
#include "hera/constant.hpp"
#include "hera/optional.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"

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

    template<std::size_t I>
    constexpr auto try_get() & noexcept
    {
        if constexpr (I == 0)
        {
            return hera::just<T&>(first);
        }
        else if constexpr (I == 1)
        {
            return hera::just<U&>(second);
        }
        else
        {
            return hera::none{};
        }
    }

    template<std::size_t I>
    constexpr auto try_get() const& noexcept
    {
        if constexpr (I == 0)
        {
            return hera::just<const T&>(first);
        }
        else if constexpr (I == 1)
        {
            return hera::just<const U&>(second);
        }
        else
        {
            return hera::none{};
        }
    }

    template<std::size_t I>
    constexpr auto try_get() && noexcept
    {
        if constexpr (I == 0)
        {
            return hera::just<T&&>(first);
        }
        else if constexpr (I == 1)
        {
            return hera::just<U&&>(second);
        }
        else
        {
            return hera::none{};
        }
    }

    template<std::size_t I>
    constexpr auto try_get() const&& noexcept
    {
        if constexpr (I == 0)
        {
            return hera::just<const T&&>(first);
        }
        else if constexpr (I == 1)
        {
            return hera::just<const U&&>(second);
        }
        else
        {
            return hera::none{};
        }
    }

    constexpr void swap(pair& other)
    {
        using std::swap;
        swap(first, other.first);
        swap(second, other.second);
    }
};

template<typename T, typename U>
pair(T&&, U&&) -> pair<std::decay_t<T>, std::decay_t<U>>;

// basically deduce the decayed type of each entry
template<pair_like P>
pair(P&& p) -> pair<std::decay_t<decltype(hera::get<0>(static_cast<P&&>(p)))>,
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