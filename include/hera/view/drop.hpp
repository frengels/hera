#pragma once

#include "hera/ranges.hpp"
#include "hera/view.hpp"
#include "hera/view/all.hpp"

namespace hera
{
template<hera::range V, std::size_t N> // clang-format off
    requires hera::view<V>
class drop_view : public hera::view_interface<drop_view<V, N>> // clang-format on
{
private:
    [[no_unique_address]] V base_;

public:
    constexpr drop_view(V base) noexcept(
        std::is_nothrow_move_constructible_v<V>)
        : base_(std::move(base))
    {}

    template<hera::constant_convertible_to<size_t> Len>
    constexpr drop_view(V base,
                        Len) noexcept(std::is_nothrow_move_constructible_v<V>)
        : base_(std::move(base))
    {}

    constexpr V base() const
    {
        return base_;
    }

    constexpr auto size() const
    {
        if constexpr (hera::unbounded_range<V>)
        {
            return hera::size(base_);
        }
        else
        {
            auto                  size_constant = hera::size(base_);
            constexpr std::size_t size          = size_constant;
            return std::integral_constant<std::size_t, size - N>{};
        }
    }

    template<std::size_t I>
    constexpr auto try_at() const noexcept
    {
        return hera::try_at<I + N>(base_);
    }
};

template<hera::range R, hera::constant_convertible_to<std::size_t> Len>
drop_view(R&&, Len)->drop_view<hera::all_view<R>, Len::value>;
} // namespace hera