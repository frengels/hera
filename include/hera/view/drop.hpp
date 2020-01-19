#pragma once

#include "hera/ranges.hpp"
#include "hera/view.hpp"
#include "hera/view/all.hpp"
#include "hera/view/detail/closure.hpp"

namespace hera
{
template<hera::range V, std::size_t N> // clang-format off
    requires hera::view<V>
class drop_view : public hera::view_interface<drop_view<V, N>> // clang-format on
{
private:
    [[no_unique_address]] V base_;

public:
    explicit constexpr drop_view(V base) noexcept(
        std::is_nothrow_move_constructible_v<V>)
        : base_(std::move(base))
    {}

    template<hera::constant_convertible_to<std::size_t> Len>
    constexpr drop_view(V base, Len) : base_(std::move(base))
    {}

    template<hera::range R> // clang-format off
        requires hera::viewable_range<R> && 
            hera::constructible_from<V, hera::all_view<R>>
    constexpr drop_view(R&& r) // clang-format on
        : base_{hera::views::all(std::forward<R>(r))}
    {}

    template<hera::range                                R,
             hera::constant_convertible_to<std::size_t> Len> // clang-format off
        requires hera::viewable_range<R> &&
            hera::constructible_from<V, hera::all_view<R>>
    constexpr drop_view(R&& r, Len) // clang-format on
        : base_{hera::views::all(std::forward<R>(r))}
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

template<hera::range R, std::size_t N>
drop_view(R &&)->drop_view<hera::all_view<R>, N>;

namespace views
{
struct drop_fn
{
    template<hera::range R, hera::constant_convertible_to<std::size_t> Len>
    constexpr auto operator()(R&& r, Len l) const
        -> decltype(hera::drop_view{std::forward<R>(r), std::move(l)})
    {
        return hera::drop_view{std::forward<R>(r), std::move(l)};
    }

    template<hera::constant_convertible_to<std::size_t> Len>
    constexpr auto operator()(Len l) const
        -> decltype(detail::view_closure{*this, std::move(l)})
    {
        return detail::view_closure{*this, std::move(l)};
    }
};

inline constexpr auto drop = drop_fn{};
} // namespace views
} // namespace hera