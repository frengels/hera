#pragma once

#include <type_traits>

#include "hera/concepts.hpp"
#include "hera/iterator/base.hpp"
#include "hera/iterator/sentinel.hpp"

namespace hera
{
namespace detail
{
template<typename I>
concept unbounded = // clang-format off
    requires {
        I::unbounded;
        requires I::unbounded == true;
    }; // clang-format on

template<typename I>
concept bounded = // clang-format off
    requires {
        { I::distance } -> hera::convertible_to<std::ptrdiff_t>;
    }; // clang-format on
} // namespace detail

template<typename D>
class iterator_interface : hera::iterator_base {
    // automatically generate all comparisons to unbounded sentinels
    template<detail::unbounded E = D>
    constexpr std::false_type operator==(const hera::unbounded_sentinel&) const
        noexcept
    {
        return {};
    }

    template<detail::unbounded E = D>
    constexpr std::true_type operator!=(const hera::unbounded_sentinel&) const
        noexcept
    {
        return {};
    }

    template<detail::unbounded E = D>
    constexpr std::true_type operator<(const hera::unbounded_sentinel&) const
        noexcept
    {
        return {};
    }

    template<detail::unbounded E = D>
    constexpr std::true_type operator<=(const hera::unbounded_sentinel&) const
        noexcept
    {
        return {};
    }

    template<detail::unbounded E = D>
    constexpr std::false_type operator>(const hera::unbounded_sentinel&) const
        noexcept
    {
        return {};
    }

    template<detail::unbounded E = D>
    constexpr std::false_type operator>=(const hera::unbounded_sentinel&) const
        noexcept
    {
        return {};
    }

    // automatically generate all bounded_sentinel comparisons
    template<detail::bounded E = D>
    constexpr std::bool_constant<(E::distance == 0)>
    operator==(const hera::bounded_sentinel&) const noexcept
    {
        return {};
    }

    template<detail::bounded E = D>
    constexpr std::bool_constant<(E::distance != 0)>
    operator!=(const hera::bounded_sentinel&) const noexcept
    {
        return {};
    }

    template<detail::bounded E = D>
    constexpr std::bool_constant<(E::distance > 0)>
    operator<(const hera::bounded_sentinel&) const noexcept
    {
        return {};
    }

    template<detail::bounded E = D>
    constexpr std::bool_constant<(E::distance >= 0)>
    operator<=(const hera::bounded_sentinel&) const noexcept
    {
        return {};
    }

    template<detail::bounded E = D>
    constexpr std::bool_constant<(E::distance < 0)>
    operator>(const hera::bounded_sentinel&) const noexcept
    {
        return {};
    }

    template<detail::bounded E = D>
    constexpr std::bool_constant<(E::distance <= 0)>
    operator>=(const hera::bounded_sentinel&) const noexcept
    {
        return {};
    }
};
} // namespace hera