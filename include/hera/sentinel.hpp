#pragma once

#include "hera/iterator.hpp"

namespace hera
{
// the default sentinel for views which have non infinite bounds
struct bounded_sentinel
{
    constexpr bounded_sentinel() noexcept = default;
};

// the default sentinel for views which have infinite bounds, such as iota_view.
struct unbounded_sentinel
{
    constexpr unbounded_sentinel() noexcept = default;

    template<hera::constant_incrementable I>
    constexpr std::false_type operator==(const I&) const noexcept
    {
        return {};
    }

    template<hera::constant_incrementable I>
    friend constexpr std::false_type
    operator==(const I&, const unbounded_sentinel&) noexcept
    {
        return {};
    }

    template<hera::constant_incrementable I>
    constexpr std::true_type operator!=(const I&) const noexcept
    {
        return {};
    }

    template<hera::constant_incrementable I>
    friend constexpr std::true_type
    operator!=(const I&, const unbounded_sentinel&) noexcept
    {
        return {};
    }

    template<hera::constant_incrementable I>
    constexpr std::false_type operator<(const I&) const noexcept
    {
        return {}; // never smaller
    }

    template<hera::constant_incrementable I>
    friend constexpr std::true_type
    operator<(const I&, const unbounded_sentinel&) noexcept
    {
        return {};
    }

    template<hera::constant_incrementable I>
    constexpr std::true_type operator>(const I&) const noexcept
    {
        return {};
    }

    template<hera::constant_incrementable I>
    friend constexpr std::false_type
    operator>(const I&, const unbounded_sentinel&) noexcept
    {
        return {};
    }

    template<hera::constant_incrementable I>
    constexpr std::false_type operator<=(const I&) const noexcept
    {
        return {};
    }

    template<hera::constant_incrementable I>
    friend constexpr std::true_type
    operator<=(const I&, const unbounded_sentinel&) noexcept
    {
        return {};
    }

    template<hera::constant_incrementable I>
    constexpr std::true_type operator>=(const I&) const noexcept
    {
        return {};
    }

    template<hera::constant_incrementable I>
    friend constexpr std::false_type
    operator>=(const I&, const unbounded_sentinel&) noexcept
    {
        return {};
    }
};
} // namespace hera