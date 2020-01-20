#pragma once

#include <cstddef>
#include <type_traits>

#include "hera/concepts.hpp"
#include "hera/constant.hpp"

namespace hera
{
// a special marker type to indicate the size is infinite
struct infinite
{
    constexpr std::true_type operator==(const infinite&) const noexcept
    {
        return {};
    }

    constexpr std::false_type operator!=(const infinite&) const noexcept
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
    friend constexpr std::false_type operator==(const C&,
                                                const infinite&) noexcept
    {
        return {};
    }

    template<hera::constant_convertible_to<std::size_t> C>
    friend constexpr std::true_type operator!=(const C&,
                                               const infinite&) noexcept
    {
        return {};
    }
};

// bounded ranges are any integral constant convertible to size_t
template<typename B>
concept bounded = constant_convertible_to<B, std::size_t>;

// unbounded must be indicated with the type unreachable
template<typename B>
concept unbounded = hera::same_as<B, infinite>;

// check whether B indicates a bound
template<typename B>
concept bound = bounded<B> || unbounded<B>;
} // namespace hera