#pragma once

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
};
} // namespace hera