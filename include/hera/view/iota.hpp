#pragma once

#include "hera/bound.hpp"
#include "hera/optional.hpp"
#include "hera/ranges.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<auto Start, auto Bound = hera::infinite{}>
class iota_view : public hera::view_interface<iota_view<Start, Bound>>
{
public:
    using value_type = decltype(Start);

private:
    static_assert(
        []() -> bool {
            if constexpr (hera::unbounded<std::remove_cvref_t<decltype(Bound)>>)
            {
                return true;
            }
            else
            {
                return Bound >= Start;
            }
        }(),
        "Bound smaller than Start");

    template<std::size_t I>
    static constexpr bool in_range = []() -> bool {
        if constexpr (hera::unbounded<std::remove_cvref_t<decltype(Bound)>>)
        {
            return true;
        }
        else
        {
            return I < (Bound - Start);
        }
    }();

public:
    iota_view() = default;

    constexpr auto size() const noexcept
    {
        if constexpr (hera::unbounded<std::remove_cvref_t<decltype(Bound)>>)
        {
            return hera::infinite_constant{};
        }
        else
        {
            return std::integral_constant<std::make_unsigned_t<decltype(Bound)>,
                                          Bound - Start>{};
        }
    }

    template<std::size_t I>
    constexpr auto try_get() const noexcept
    {
        if constexpr (in_range<I>)
        {
            return hera::just<std::integral_constant<value_type, Start + I>>{};
        }
        else
        {
            return hera::none{};
        }
    }

    template<std::size_t I> // clang-format off
        requires (in_range<I>)
    constexpr auto get() const noexcept // clang-format on
    {
        return std::integral_constant<value_type, Start + I>{};
    }
};
} // namespace hera