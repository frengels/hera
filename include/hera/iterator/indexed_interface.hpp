#pragma once

#include "hera/concepts.hpp"
#include "hera/iterator/sentinel.hpp"

namespace hera
{
enum struct extent
{
    bounded,
    unbounded
};

namespace detail
{
template<hera::extent E, auto N = 0>
struct iterator_extent
{
    static constexpr auto extent_type = E;
    static constexpr auto value       = N;
};
} // namespace detail

using unbounded_extent = hera::detail::iterator_extent<hera::extent::unbounded>;

template<auto N>
using bounded_extent = hera::detail::iterator_extent<hera::extent::bounded, N>;

template<typename E>
concept iterator_extent = requires
{
    E::extent_type;
    requires same_as<hera::extent, std::remove_cv_t<decltype(E::extent_type)>>;
    E::value;
};

namespace detail
{
template<typename E>
concept bounded_concept = hera::iterator_extent<E> &&
                          (E::extent_type == hera::extent::bounded);

template<typename E>
concept unbounded_concept = hera::iterator_extent<E> &&
                            (E::extent_type == hera::extent::unbounded);
} // namespace detail

template<typename D,
         hera::signed_integral Diff,
         Diff                  I,
         hera::iterator_extent E>
class indexed_interface {
private:
    static constexpr auto S = E::value;

public:
    using difference_type = Diff;

public:
    indexed_interface() = default;

    // self comparisons
    template<difference_type J>
    constexpr auto
    operator==(const indexed_interface<D, difference_type, J, E>&) const
        noexcept -> std::bool_constant<(I == J)>
    {
        return {};
    }

    template<difference_type J>
    constexpr auto
    operator!=(const indexed_interface<D, difference_type, J, E>&) const
        noexcept -> std::bool_constant<(I != J)>
    {
        return {};
    }

    template<difference_type J>
    constexpr auto
    operator<(const indexed_interface<D, difference_type, J, E>&) const noexcept
        -> std::bool_constant<(I < J)>
    {
        return {};
    }

    template<difference_type J>
    constexpr auto
    operator>(const indexed_interface<D, difference_type, J, E>&) const noexcept
        -> std::bool_constant<(I > J)>
    {
        return {};
    }

    template<difference_type J>
    constexpr auto
    operator<=(const indexed_interface<D, difference_type, J, E>&) const
        noexcept -> std::bool_constant<(I <= J)>
    {
        return {};
    }

    template<difference_type J>
    constexpr auto
    operator>=(const indexed_interface<D, difference_type, J, E>&) const
        noexcept -> std::bool_constant<(I >= J)>
    {
        return {};
    }

    // bounded comparisons
    template<detail::bounded_concept B = E>
    constexpr auto operator==(hera::bounded_sentinel) const noexcept
        -> std::bool_constant<(I == S)>
    {
        return {};
    }

    template<detail::bounded_concept B = E>
    constexpr auto operator!=(hera::bounded_sentinel) const noexcept
        -> std::bool_constant<(I != S)>
    {
        return {};
    }

    template<detail::bounded_concept B = E>
    constexpr auto operator<(hera::bounded_sentinel) const noexcept
        -> std::bool_constant<(I < S)>
    {
        return {};
    }

    template<detail::bounded_concept B = E>
    constexpr auto operator>(hera::bounded_sentinel) const noexcept
        -> std::bool_constant<(I > S)>
    {
        return {};
    }

    template<detail::bounded_concept B = E>
    constexpr auto operator<=(hera::bounded_sentinel) const noexcept
        -> std::bool_constant<(I <= S)>
    {
        return {};
    }

    template<detail::bounded_concept B = E>
    constexpr auto operator>=(hera::bounded_sentinel) const noexcept
        -> std::bool_constant<(I >= S)>
    {
        return {};
    }

    // bounded friend comparisons
    template<detail::bounded_concept B = E>
    friend constexpr auto operator==(hera::bounded_sentinel,
                                     const indexed_interface&) noexcept
        -> std::bool_constant<(S == I)>
    {
        return {};
    }

    template<detail::bounded_concept B = E>
    friend constexpr auto operator!=(hera::bounded_sentinel,
                                     const indexed_interface&) noexcept
        -> std::bool_constant<(S != I)>
    {
        return {};
    }

    template<detail::bounded_concept B = E>
    friend constexpr auto operator<(hera::bounded_sentinel,
                                    const indexed_interface&) noexcept
        -> std::bool_constant<(S < I)>
    {
        return {};
    }

    template<detail::bounded_concept B = E>
    friend constexpr auto operator>(hera::bounded_sentinel,
                                    const indexed_interface&) noexcept
        -> std::bool_constant<(S > I)>
    {
        return {};
    }

    template<detail::bounded_concept B = E>
    friend constexpr auto operator<=(hera::bounded_sentinel,
                                     const indexed_interface&) noexcept
        -> std::bool_constant<(S <= I)>
    {
        return {};
    }

    template<detail::bounded_concept B = E>
    friend constexpr auto operator>=(hera::bounded_sentinel,
                                     const indexed_interface&) noexcept
        -> std::bool_constant<(S >= I)>
    {
        return {};
    }

    // unbounded comparisons
    template<detail::unbounded_concept B = E>
    constexpr auto operator==(hera::unbounded_sentinel) const noexcept
        -> std::false_type
    {
        return {};
    }

    template<detail::unbounded_concept B = E>
    constexpr auto operator!=(hera::unbounded_sentinel) const noexcept
        -> std::true_type
    {
        return {};
    }

    template<detail::unbounded_concept B = E>
    constexpr auto operator<(hera::unbounded_sentinel) const noexcept
        -> std::true_type
    {
        return {};
    }

    template<detail::unbounded_concept B = E>
    constexpr auto operator>(hera::unbounded_sentinel) const noexcept
        -> std::false_type
    {
        return {};
    }

    template<detail::unbounded_concept B = E>
    constexpr auto operator<=(hera::unbounded_sentinel) const noexcept
        -> std::true_type
    {
        return {};
    }

    template<detail::unbounded_concept B = E>
    constexpr auto operator>=(hera::unbounded_sentinel) const noexcept
        -> std::false_type
    {
        return {};
    }

    // unbounded friend comparisons
    template<detail::unbounded_concept B = E>
    friend constexpr auto operator==(hera::unbounded_sentinel,
                                     const indexed_interface&) noexcept
        -> std::false_type
    {
        return {};
    }

    template<detail::unbounded_concept B = E>
    friend constexpr auto operator!=(hera::unbounded_sentinel,
                                     const indexed_interface&) noexcept
        -> std::true_type
    {
        return {};
    }

    template<detail::unbounded_concept B = E>
    friend constexpr auto operator<(hera::unbounded_sentinel,
                                    const indexed_interface&) noexcept
        -> std::false_type
    {
        return {};
    }

    template<detail::unbounded_concept B = E>
    friend constexpr auto operator>(hera::unbounded_sentinel,
                                    const indexed_interface&) noexcept
        -> std::true_type
    {
        return {};
    }

    template<detail::unbounded_concept B = E>
    friend constexpr auto operator<=(hera::unbounded_sentinel,
                                     const indexed_interface&) noexcept
        -> std::false_type
    {
        return {};
    }

    template<detail::unbounded_concept B = E>
    friend constexpr auto operator>=(hera::unbounded_sentinel,
                                     const indexed_interface&) noexcept
        -> std::true_type
    {
        return {};
    }
};
} // namespace hera