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

namespace detail
{
template<typename T>
struct most_base_type
{
    using type = T;
};

template<typename T> // clang-format off
    requires 
        requires (const T& t) 
        {
            t.base();
        } // clang-format on
struct most_base_type<T>
{
    using type = typename most_base_type<
        std::remove_cvref_t<decltype(std::declval<const T&>().base())>>::type;
};

template<typename T>
concept default_enable_unbounded_sentinel =
    same_as<hera::unbounded_sentinel, T> ||
    derived_from<T, hera::unbounded_sentinel> ||
    same_as<hera::unbounded_sentinel, typename most_base_type<T>::type> ||
    derived_from<typename most_base_type<T>::type, hera::unbounded_sentinel>;
} // namespace detail

// checks for any of the following conditions:
// - the type S is hera::unbounded_sentinel
// - the type S is derived from hera::unbounded_sentinel
// - the type called by recursively calling .base() is hera::unbounded_sentinel
// - the type called by recrusively calling .base() is derived from
//   hera::unbounded_sentinel
template<typename S>
static constexpr bool enable_unbounded_sentinel =
    detail::default_enable_unbounded_sentinel<S>;

template<typename S, typename I>
concept unbounded_sentinel_for =
    enable_unbounded_sentinel<S>&& hera::sentinel_for<S, I>;

template<typename S, typename I>
concept bounded_sentinel_for =
    hera::sentinel_for<S, I> && !hera::unbounded_sentinel_for<S, I>;
} // namespace hera