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
    static_assert((Bound - Start) >= 0, "Bound smaller than Start");

    template<std::size_t I>
    static constexpr bool in_range = []() -> bool {
        if constexpr (hera::same_as<hera::infinite,
                                    std::remove_cvref_t<decltype(Bound)>>)
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
        constexpr auto dist = Bound - Start;

        if constexpr (hera::same_as<std::remove_cvref_t<decltype(dist)>,
                                    hera::infinite>)
        {
            return hera::infinite_constant{};
        }
        else
        {
            return std::integral_constant<std::size_t,
                                          static_cast<std::size_t>(dist)>{};
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