#pragma once

#include "hera/concepts.hpp"
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

template<typename S>
static constexpr bool enable_unbounded_sentinel = std::conditional_t<
    hera::same_as<hera::unbounded_sentinel, std::remove_cvref_t<S>>,
    std::true_type,
    std::false_type>::value;

template<typename S, typename I>
concept unbounded_sentinel_for =
    enable_unbounded_sentinel<S>&& hera::sentinel_for<S, I>;

template<typename S, typename I>
concept bounded_sentinel_for =
    hera::sentinel_for<S, I> && !hera::unbounded_sentinel_for<S, I>;
} // namespace hera