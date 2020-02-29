#pragma once

#include <cstddef>
#include <type_traits>

#include "hera/concepts.hpp"
#include "hera/constant.hpp"

namespace hera
{
struct infinite
{
    constexpr bool operator==(const infinite&) const noexcept
    {
        return true;
    }

    constexpr bool operator!=(const infinite&) const noexcept
    {
        return false;
    }

    constexpr bool operator<(const infinite&) const noexcept
    {
        return false;
    }

    constexpr bool operator>(const infinite&) const noexcept
    {
        return false;
    }

    constexpr bool operator<=(const infinite&) const noexcept
    {
        return true;
    }

    constexpr bool operator>=(const infinite&) const noexcept
    {
        return true;
    }

    template<hera::integral I>
    constexpr bool operator==(const I&) const noexcept
    {
        return false;
    }

    template<hera::integral I>
    friend constexpr bool operator==(const I&, const infinite&) noexcept
    {
        return false;
    }

    template<hera::integral I>
    constexpr bool operator!=(const I&) const noexcept
    {
        return true;
    }

    template<hera::integral I>
    friend constexpr bool operator!=(const I&, const infinite&) noexcept
    {
        return true;
    }

    template<hera::integral I>
    constexpr bool operator<(const I&) const noexcept
    {
        return false;
    }

    template<hera::integral I>
    friend constexpr bool operator<(const I&, const infinite&) noexcept
    {
        return true;
    }

    template<hera::integral I>
    constexpr bool operator>(const I&) const noexcept
    {
        return true;
    }

    template<hera::integral I>
    friend constexpr bool operator>(const I&, const infinite&) noexcept
    {
        return false;
    }

    template<hera::integral I>
    constexpr bool operator<=(const I&) const noexcept
    {
        return false;
    }

    template<hera::integral I>
    friend constexpr bool operator<=(const I&, const infinite&) noexcept
    {
        return true;
    }

    template<hera::integral I>
    constexpr bool operator>=(const I&) const noexcept
    {
        return true;
    }

    template<hera::integral I>
    friend constexpr bool operator>=(const I&, const infinite&) noexcept
    {
        return false;
    }
};

// a special marker type to indicate the size is infinite
struct infinite_constant
{
    using type                  = infinite_constant;
    using value_type            = infinite;
    static constexpr auto value = infinite{};

    constexpr std::true_type operator==(const infinite_constant&) const noexcept
    {
        return {};
    }

    constexpr std::false_type operator!=(const infinite_constant&) const
        noexcept
    {
        return {};
    }

    constexpr std::false_type operator<(const infinite_constant&) const noexcept
    {
        return {};
    }

    constexpr std::false_type operator>(const infinite_constant&) const noexcept
    {
        return {};
    }

    constexpr std::true_type operator<=(const infinite_constant&) const noexcept
    {
        return {};
    }

    constexpr std::true_type operator>=(const infinite_constant&) const noexcept
    {
        return {};
    }

    template<hera::constant_convertible_to<std::size_t> C>
    constexpr std::false_type operator==(const C&) const noexcept
    {
        return {};
    }

    template<hera::constant_convertible_to<std::size_t> C>
    constexpr std::true_type operator!=(const C&) const noexcept
    {
        return {};
    }

    template<hera::constant_convertible_to<std::size_t> C>
    friend constexpr std::false_type
    operator==(const C&, const infinite_constant&) noexcept
    {
        return {};
    }

    template<hera::constant_convertible_to<std::size_t> C>
    friend constexpr std::true_type
    operator!=(const C&, const infinite_constant&) noexcept
    {
        return {};
    }
};

template<typename B>
concept bounded = hera::same_as<B, std::size_t>;

template<typename B>
concept unbounded = hera::same_as<B, infinite>;

template<typename B>
concept bound = bounded<B> || unbounded<B>;

// bounded ranges are any integral constant convertible to size_t
template<typename B>
concept bounded_constant = constant_same_as<B, std::size_t>;

// unbounded must be indicated with the type unreachable
template<typename B>
concept unbounded_constant = hera::same_as<B, infinite_constant>;

// check whether B indicates a bound
template<typename B>
concept bound_constant = bounded_constant<B> || unbounded_constant<B>;
} // namespace hera