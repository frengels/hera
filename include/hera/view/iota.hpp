#pragma once

#include "hera/constant.hpp"
#include "hera/iterator/concepts.hpp"
#include "hera/iterator/indexed_interface.hpp"
#include "hera/iterator/sentinel.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<auto W,
         auto Bound     = static_cast<decltype(W)>(0),
         bool Unbounded = true> // clang-format off
    requires same_as<decltype(W), decltype(Bound)>
class iota_view : public view_interface<iota_view<W, Bound, Unbounded>> { // clang-format on
private:
    template<decltype(W) I>
    class iterator : public hera::indexed_interface<
                         iterator<I>,
                         std::ptrdiff_t,
                         I,
                         std::conditional_t<Unbounded,
                                            hera::unbounded_extent,
                                            hera::bounded_extent<Bound>>> {
    public:
        using value_type      = std::integral_constant<decltype(I), I>;
        using difference_type = std::ptrdiff_t;

    public:
        constexpr iterator() noexcept = default;

        constexpr auto operator++() const noexcept
        {
            return iterator<I + 1>{};
        }

        constexpr auto operator--() const noexcept
        {
            return iterator<I - 1>{};
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr auto operator+(C) const noexcept
        {
            return iterator<I + C::value>{};
        }

        template<hera::constant_convertible_to<difference_type> C>
        friend constexpr auto operator+(C, const iterator&) noexcept
        {
            return iterator<C::value + I>{};
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr auto operator-(C) const noexcept
        {
            return iterator<I - C::value>{};
        }

        template<auto X>
        constexpr auto operator-(const iterator&) const noexcept
        {
            return iterator<I - X>{};
        }

        constexpr decltype(auto) operator*() const noexcept
        {
            return std::integral_constant<decltype(I), I>{};
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr decltype(auto) operator[](C n) const noexcept
        {
            return *((*this) + n);
        }
    };

public:
    constexpr iota_view() noexcept = default;

    template<hera::constant C>
    constexpr iota_view(C) noexcept
    {}

    template<hera::constant                                 C,
             hera::constant_same_as<typename C::value_type> End>
    constexpr iota_view(C, End) noexcept
    {}

    constexpr auto begin() const noexcept
    {
        return iterator<W>{};
    }

    constexpr auto end() const noexcept
    {
        if constexpr (Unbounded)
        {
            return hera::unbounded_sentinel{};
        }
        else
        {
            return hera::bounded_sentinel{};
        }
    }

    friend constexpr auto begin(iota_view iv) noexcept(noexcept(iv.begin()))
    {
        return iv.begin();
    }

    friend constexpr auto end(iota_view iv) noexcept(noexcept(iv.end()))
    {
        return iv.end();
    }
};

template<hera::constant C>
iota_view(C)->iota_view<C::value>;

template<hera::constant C, hera::constant_same_as<typename C::value_type> Bound>
iota_view(C, Bound)->iota_view<C::value, Bound::value, false>;
} // namespace hera