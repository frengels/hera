#pragma once

#include <type_traits>
#include <utility>

#include "hera/concepts.hpp"
#include "hera/constant.hpp"
#include "hera/distance.hpp"
#include "hera/next_prev.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"

namespace hera
{
template<typename T>
concept pair_like = // clang-format off
    requires (T&& t, 
              std::integral_constant<std::size_t, 0> idx0, 
              std::integral_constant<std::size_t, 1> idx1)
    {
        hera::size(std::forward<T>(t));
        requires (decltype(hera::size(std::forward<T>(t)))::value == 2);
        hera::at(std::forward<T>(t), idx0);
        hera::at(std::forward<T>(t), idx1);
    }; // clang-format on

template<typename T, typename U>
class pair {
public:
    using first_type  = T;
    using second_type = U;

private:
    [[no_unique_address]] T first_{};
    [[no_unique_address]] U second_{};

public:
    pair() = default;

    template<typename U1, typename U2>
    constexpr pair(U1&& u1,
                   U2&& u2) noexcept(std::is_nothrow_constructible_v<T, U1>&&
                                         std::is_nothrow_constructible_v<U, U2>)
        : first_{std::forward<U1>(u1)}, second_{std::forward<U2>(u2)}
    {
        // using constructible_from concept doesn't work to constrain somehow
    }

    template<pair_like P>
    constexpr pair(P&& p)
        : first_{hera::at(std::forward<P>(p),
                          std::integral_constant<std::size_t, 0>{})},
          second_{hera::at(std::forward<P>(p),
                           std::integral_constant<std::size_t, 1>{})}
    {}

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
        : first_{*hera::next(hera::begin(args_first),
                             std::integral_constant<std::size_t, IsT>{})...},
          second_{*hera::next(hera::begin(args_second),
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

    constexpr T& first() & noexcept
    {
        return static_cast<T&>(first_);
    }

    constexpr U& second() & noexcept
    {
        return static_cast<U&>(second_);
    }

    constexpr const T& first() const& noexcept
    {
        return static_cast<const T&>(first_);
    }

    constexpr const U& second() const& noexcept
    {
        return static_cast<const U&>(second_);
    }

    constexpr T&& first() && noexcept
    {
        return static_cast<T&&>(first_);
    }

    constexpr U&& second() && noexcept
    {
        return static_cast<U&&>(second_);
    }

    constexpr const T&& first() const&& noexcept
    {
        return static_cast<const T&&>(first_);
    }

    constexpr const U&& second() const&& noexcept
    {
        return static_cast<const U&&>(second_);
    }

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
            return first();
        }
        else if constexpr (C::value == 1)
        {
            return second();
        }
    }

    template<hera::constant_convertible_to<std::size_t> C> // clang-format off
        requires (C::value < 2) // clang-format on
        constexpr decltype(auto)
        operator[](C) const& noexcept
    {
        if constexpr (C::value == 0)
        {
            return first();
        }
        else if constexpr (C::value == 1)
        {
            return second();
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
            return std::move(*this).first();
        }
        else if constexpr (C::value == 1)
        {
            return std::move(*this).second();
        }
    }

    template<hera::constant_convertible_to<std::size_t> C> // clang-format off
        requires (C::value < 2) // clang-format on
        constexpr decltype(auto)
        operator[](C) const&& noexcept
    {
        if constexpr (C::value == 0)
        {
            return std::move(*this).first();
        }
        else if constexpr (C::value == 1)
        {
            return std::move(*this).second();
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

// basically deduce the decayed type of each entry
template<pair_like P>
pair(P&& p)
    ->pair<std::decay_t<
               decltype(hera::at(std::forward<P>(p),
                                 std::integral_constant<std::size_t, 0>{}))>,
           std::decay_t<
               decltype(hera::at(std::forward<P>(p),
                                 std::integral_constant<std::size_t, 1>{}))>>;

template<typename T, typename U>
constexpr hera::pair<std::decay_t<T>, std::decay_t<U>>
make_pair(T&& t, U&& u) noexcept(std::is_nothrow_constructible_v<
                                 hera::pair<std::decay_t<T>, std::decay_t<U>>,
                                 T,
                                 U>)
{
    return hera::pair<std::decay_t<T>, std::decay_t<U>>{std::forward<T>(t),
                                                        std::forward<U>(u)};
}
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