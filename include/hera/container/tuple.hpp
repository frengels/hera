#pragma once

#include <utility>

#include "hera/algorithm/unpack.hpp"
#include "hera/concepts.hpp"
#include "hera/optional.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"
#include "hera/type_.hpp"

namespace hera
{
namespace detail
{
template<std::size_t, typename T>
struct tuple_box
{
    [[no_unique_address]] T value;
};

template<typename Indices, typename... Ts>
class tuple_impl;

template<std::size_t... Is, typename... Ts>
class tuple_impl<std::index_sequence<Is...>, Ts...>
    : protected tuple_box<Is, Ts>...
{
public:
    tuple_impl() = default;

    template<typename... Us> // clang-format off
        requires (sizeof...(Ts) == sizeof...(Us)) // clang-format on
        constexpr tuple_impl(Us&&... us) noexcept(
            (std::is_nothrow_constructible_v<Ts, Us> && ...))
        : tuple_box<Is, Ts>{.value = Ts(std::forward<Us>(us))}...
    {}
};
} // namespace detail

template<typename... Ts>
class tuple : private detail::tuple_impl<std::index_sequence_for<Ts...>, Ts...>
{
private:
    using base_type_ =
        detail::tuple_impl<std::index_sequence_for<Ts...>, Ts...>;

public:
    using base_type_::base_type_;

private:
    template<std::size_t... Is, typename... Us>
    constexpr tuple(std::index_sequence<Is...>, const hera::tuple<Us...>& other)
        : base_type_{hera::get<Is>(other)...}
    {}

    template<std::size_t... Is, typename... Us>
    constexpr tuple(std::index_sequence<Is...>, hera::tuple<Us...>&& other)
        : base_type_{hera::get<Is>(std::move(other))...}
    {}

public:
    template<typename... Us> // clang-format off
            requires (sizeof...(Ts) == sizeof...(Us))
    constexpr tuple(const hera::tuple<Us...>& other) // clang-format on
        : tuple{std::index_sequence_for<Us...>{}, other}
    {}

    template<typename... Us> // clang-format off
            requires (sizeof...(Ts) == sizeof...(Us))
        constexpr tuple(hera::tuple<Us...>&& other) // clang-format on
        : tuple{std::index_sequence_for<Us...>{}, std::move(other)}
    {}

private:
    template<std::size_t... Is, typename... Us>
    constexpr void assign_impl(
        std::index_sequence<Is...>,
        const hera::tuple<Us...>&
            other) noexcept((std::is_nothrow_assignable_v<Ts, Us> && ...))
    {
        (((*this)[std::integral_constant<std::size_t, Is>{}] =
              other[std::integral_constant<std::size_t, Is>{}]),
         ...);
    }

    template<std::size_t... Is, typename... Us>
    constexpr void assign_impl(
        std::index_sequence<Is...>,
        hera::tuple<Us...>&&
            other) noexcept((std::is_nothrow_assignable_v<Ts, Us&&> && ...))
    {
        ((hera::get<Is>(*this) = hera::get<Is>(std::move(other))), ...);
    }

public:
    template<typename... Us> // clang-format off
        requires (sizeof...(Ts) == sizeof...(Us))
    constexpr tuple& operator=(const hera::tuple<Us...>& other) noexcept( // clang-format on
        noexcept(assign_impl(std::index_sequence_for<Us...>{}, other)))
    {
        assign_impl(std::index_sequence_for<Us...>{}, other);
        return *this;
    }

    template<typename... Us> // clang-format off
        requires (sizeof...(Ts) == sizeof...(Us))
    constexpr tuple& operator=(hera::tuple<Us...>&& other) noexcept(noexcept( // clang-format on
        assign_impl(std::index_sequence_for<Us...>{}, std::move(other))))
    {
        assign_impl(std::index_sequence_for<Us...>{}, std::move(other));
        return *this;
    }

    constexpr std::integral_constant<std::size_t, sizeof...(Ts)>
    size() const noexcept
    {
        return {};
    }

    constexpr decltype(auto) front() noexcept requires(sizeof...(Ts) >= 1)
    {
        return get<0>();
    }

    constexpr decltype(auto) front() const noexcept requires(sizeof...(Ts) >= 1)
    {
        return get<0>();
    }

    constexpr decltype(auto) back() noexcept requires(sizeof...(Ts) >= 1)
    {
        auto                  sz  = size();
        constexpr std::size_t sz_ = sz;

        return get<sz_ - 1>();
    }

    constexpr decltype(auto) back() const noexcept requires(sizeof...(Ts) >= 1)
    {
        auto                  sz  = size();
        constexpr std::size_t sz_ = sz;

        return get<sz_ - 1>();
    }

    constexpr std::bool_constant<sizeof...(Ts) == 0> empty() const noexcept
    {
        return {};
    }

    // push_back & emplace_back variants

    template<typename T, typename... Args>
    [[nodiscard]] constexpr hera::tuple<Ts..., T>
    emplace_back(Args&&... args) const&
    {
        return hera::unpack(*this, [&](const Ts&... xs) {
            return hera::tuple<Ts..., T>{xs..., T(std::forward<Args>(args)...)};
        });
    }

    template<typename T, typename... Args>
    [[nodiscard]] constexpr hera::tuple<Ts..., T>
    emplace_back(Args&&... args) &&
    {
        return hera::unpack(std::move(*this), [&](Ts&&... xs) {
            return hera::tuple<Ts..., T>{std::forward<Ts>(xs)...,
                                         T(std::forward<Args>(args)...)};
        });
    }

    template<typename T>
    [[nodiscard]] constexpr hera::tuple<Ts..., T> push_back(const T& val) const&
    {
        return emplace_back<T>(val);
    }

    template<typename T>
    [[nodiscard]] constexpr hera::tuple<Ts..., T> push_back(const T& val) &&
    {
        return std::move(*this).template emplace_back<T>(val);
    }

    template<typename T>
    [[nodiscard]] constexpr hera::tuple<Ts..., T> push_back(T&& val) const&
    {
        return emplace_back<T>(std::move(val));
    }

    template<typename T>
    [[nodiscard]] constexpr hera::tuple<Ts..., T> push_back(T&& val) &&
    {
        return std::move(*this).template emplace_back<T>(std::move(val));
    }

    // push_front & emplace_front variants

    template<typename T, typename... Args>
    [[nodiscard]] constexpr hera::tuple<T, Ts...>
    emplace_front(Args&&... args) const&
    {
        return hera::unpack(*this, [&](const Ts&... xs) {
            return hera::tuple<T, Ts...>{T(std::forward<Args>(args)...), xs...};
        });
    }

    template<typename T, typename... Args>
    [[nodiscard]] constexpr hera::tuple<T, Ts...>
    emplace_front(Args&&... args) &&
    {
        return hera::unpack(std::move(*this), [&](Ts&&... xs) {
            return hera::tuple<T, Ts...>{T(std::forward<Args>(args)...),
                                         std::forward<Ts>(xs)...};
        });
    }

    template<typename T>
    [[nodiscard]] constexpr hera::tuple<T, Ts...>
    push_front(const T& val) const&
    {
        return emplace_front<T>(val);
    }

    template<typename T>
    [[nodiscard]] constexpr hera::tuple<T, Ts...> push_front(const T& val) &&
    {
        return std::move(*this).template emplace_front<T>(val);
    }

    template<typename T>
    [[nodiscard]] constexpr hera::tuple<T, Ts...> push_front(T&& val) const&
    {
        return emplace_front<T>(std::move(val));
    }

    template<typename T>
    [[nodiscard]] constexpr hera::tuple<T, Ts...> push_front(T&& val) &&
    {
        return std::move(*this).template emplace_front<T>(std::move(val));
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts))
    constexpr auto element_type() noexcept // clang-format on
    {
        return hera::type_<std::tuple_element_t<I, std::tuple<Ts...>>>{};
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts))
    constexpr auto element_type() const noexcept // clang-format on
    {
        return hera::type_<std::tuple_element_t<I, const std::tuple<Ts...>>>{};
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts))
    constexpr std::tuple_element_t<I, std::tuple<Ts...>>&
    get() & noexcept // clang-format on
    {
        return (
            static_cast<
                detail::tuple_box<I,
                                  std::tuple_element_t<I, std::tuple<Ts...>>>&>(
                *this)
                .value);
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts))
    constexpr const std::tuple_element_t<I, std::tuple<Ts...>>& 
    get() const & noexcept // clang-format on
    {
        return (static_cast<const detail::tuple_box<
                    I,
                    std::tuple_element_t<I, std::tuple<Ts...>>>&>(*this)
                    .value);
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts))
    constexpr std::tuple_element_t<I, std::tuple<Ts...>>&&
    get() && noexcept // clang-format on
    {
        using type =
            decltype(static_cast<const detail::tuple_box<
                         I,
                         std::tuple_element_t<I, std::tuple<Ts...>>>&&>(*this)
                         .value);

        return static_cast<type&&>(
            static_cast<detail::tuple_box<
                I,
                std::tuple_element_t<I, std::tuple<Ts...>>>&&>(*this)
                .value);
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts))
    constexpr const std::tuple_element_t<I, std::tuple<Ts...>>&&
    get() const && noexcept // clang-format on
    {
        using type =
            decltype(static_cast<const detail::tuple_box<
                         I,
                         std::tuple_element_t<I, std::tuple<Ts...>>>&&>(*this)
                         .value);

        return static_cast<const type&&>(
            static_cast<const detail::tuple_box<
                I,
                std::tuple_element_t<I, std::tuple<Ts...>>>&&>(*this)
                .value);
    }
};

template<typename... Ts>
tuple(Ts&&...) -> tuple<std::decay_t<Ts>...>;

template<typename... Ts,
         typename... Us,
         template<typename>
         typename TQual,
         template<typename>
         typename UQual> // clang-format off
    requires (sizeof...(Ts) == sizeof...(Us)) &&
        (common_reference_with<TQual<Ts>, UQual<Us>> && ...) // clang-format on
    struct basic_common_reference<hera::tuple<Ts...>,
                                  hera::tuple<Us...>,
                                  TQual,
                                  UQual>
{
    using type = hera::tuple<common_reference_t<TQual<Ts>, UQual<Us>>...>;
};

template<typename... Ts>
constexpr hera::tuple<Ts&&...> forward_as_tuple(Ts&&... ts) noexcept(
    std::is_nothrow_constructible_v<hera::tuple<Ts&&...>, Ts...>)
{
    return hera::tuple<Ts&&...>{static_cast<Ts&&>(ts)...};
}

template<typename... Ts>
constexpr hera::tuple<std::decay_t<Ts>...> make_tuple(Ts&&... ts) noexcept(
    std::is_nothrow_constructible_v<hera::tuple<std::decay_t<Ts>...>, Ts...>)
{
    return hera::tuple<std::decay_t<Ts>...>{static_cast<Ts&&>(ts)...};
}

template<typename T, typename... Ts> // clang-format off
    requires (std::is_object_v<T>) && hera::constructible_from<T, Ts&...>
constexpr T make_from_tuple(hera::tuple<Ts...>& tup) // clang-format on
{
    return hera::unpack(tup, [](Ts&... ts) { return T(ts...); });
}

template<typename T, typename... Ts> // clang-format off
    requires (std::is_object_v<T>) && hera::constructible_from<T, const Ts&...>
constexpr T make_from_tuple(const hera::tuple<Ts...>& tup) // clang-format on
{
    return hera::unpack(tup, [](const Ts&... ts) { return T(ts...); });
}

template<typename T, typename... Ts> // clang-format off
    requires (std::is_object_v<T>) && hera::constructible_from<T, Ts&&...>
constexpr T make_from_tuple(hera::tuple<Ts...>&& tup) // clang-format on
{
    return hera::unpack(std::move(tup),
                        [](Ts&&... ts) { return T(static_cast<Ts&&>(ts)...); });
}

template<typename T, typename... Ts> // clang-format off
    requires (std::is_object_v<T>) && hera::constructible_from<T, const Ts&&...>
constexpr T make_from_tuple(const hera::tuple<Ts...>&& tup) // clang-format on
{
    return hera::unpack(std::move(tup), [](const Ts&&... ts) {
        return T(static_cast<const Ts&&>(ts)...);
    });
}
} // namespace hera