#pragma once

#include "hera/concepts.hpp"

namespace hera
{
// the default sentinel for views which have non infinite bounds
struct bounded_sentinel final
{
    constexpr bounded_sentinel() noexcept = default;

    constexpr auto operator==(bounded_sentinel) const noexcept -> std::true_type
    {
        return {};
    }

    constexpr auto operator!=(bounded_sentinel) const noexcept
        -> std::false_type
    {
        return {};
    }

    constexpr auto operator<(bounded_sentinel) const noexcept -> std::false_type
    {
        return {};
    }

    constexpr auto operator>(bounded_sentinel) const noexcept -> std::false_type
    {
        return {};
    }

    constexpr auto operator<=(bounded_sentinel) const noexcept -> std::true_type
    {
        return {};
    }

    constexpr auto operator>=(bounded_sentinel) const noexcept -> std::true_type
    {
        return {};
    }
};

// the default sentinel for views which have infinite bounds, such as iota_view.
struct unbounded_sentinel final
{

    unbounded_sentinel() = default;

    constexpr auto operator==(unbounded_sentinel) const noexcept
        -> std::true_type
    {
        return {};
    }

    constexpr auto operator!=(unbounded_sentinel) const noexcept
        -> std::false_type
    {
        return {};
    }

    constexpr auto operator<(unbounded_sentinel) const noexcept
        -> std::false_type
    {
        return {};
    }

    constexpr auto operator>(unbounded_sentinel) const noexcept
        -> std::false_type
    {
        return {};
    }

    constexpr auto operator<=(unbounded_sentinel) const noexcept
        -> std::true_type
    {
        return {};
    }

    constexpr auto operator>=(unbounded_sentinel) const noexcept
        -> std::true_type
    {
        return {};
    }
};
} // namespace hera