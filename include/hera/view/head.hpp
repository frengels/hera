#pragma once

#include "hera/optional.hpp"
#include "hera/view.hpp"
#include "hera/view/all.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<hera::range V> // clang-format off
    requires view<V> && (!empty_range<V>)
class head_view : public hera::view_interface<head_view<V>> // clang-format on
{
private:
    [[no_unique_address]] V base_;

public:
    explicit constexpr head_view(V base) noexcept(
        std::is_nothrow_move_constructible_v<V>)
        : base_{std::move(base)}
    {}

    constexpr V base() const noexcept(std::is_nothrow_copy_constructible_v<V>)
    {
        return base_;
    }

    constexpr std::integral_constant<std::size_t, 1> size() const noexcept
    {
        return {};
    }

    constexpr std::false_type empty() const noexcept
    {
        return {};
    }

    template<std::size_t I> // clang-format off
        requires (I == 0)
    constexpr auto get() const // clang-format on
        -> decltype(hera::get<I>(base_))
    {
        return hera::get<I>(base_);
    }
}; // namespace hera

template<hera::range R>
head_view(R &&) -> head_view<hera::all_view<R>>;

namespace views
{
struct head_fn : public detail::pipeable_interface<head_fn>
{
    template<hera::range R>
    constexpr auto operator()(R&& r) const
        noexcept(noexcept(head_view{std::forward<R>(r)}))
    {
        return head_view{std::forward<R>(r)};
    }
};

inline constexpr auto head = head_fn{};
} // namespace views
} // namespace hera