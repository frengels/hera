#pragma once

#include "hera/at.hpp"
#include "hera/concepts.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"
#include "hera/view.hpp"
#include "hera/view/all.hpp"
#include "hera/view/detail/closure.hpp"

namespace hera
{
template<hera::range V, typename F> // clang-format off
    requires view<V> && std::is_object_v<F>
class transform_view : public view_interface<transform_view<V, F>> // clang-format on
{
private:
    [[no_unique_address]] V base_;
    [[no_unique_address]] F transform_fn_;

public:
    constexpr transform_view(V base, F transform_fn) noexcept(
        std::is_nothrow_move_constructible_v<V>&&
            std::is_nothrow_move_constructible_v<F>)
        : base_{std::move(base)}, transform_fn_{std::move(transform_fn)}
    {}

    constexpr V base() const noexcept(std::is_nothrow_copy_constructible_v<V>)
    {
        return base_;
    }

    constexpr auto size() const noexcept
    {
        return hera::size(base_);
    }

    template<std::size_t I>
    constexpr auto try_at() const noexcept
    {
        return hera::try_at<I>(base_).transform(transform_fn_);
    }
};

template<hera::range R, typename F>
transform_view(R&&, F)->transform_view<hera::all_view<R>, F>;

namespace views
{
struct transform_fn
{
    template<hera::range R, typename F>
    constexpr auto operator()(R&& r, F transform_fn) const
        noexcept(noexcept(hera::transform_view{std::forward<R>(r),
                                               std::move(transform_fn)}))
            -> decltype(hera::transform_view{std::forward<R>(r),
                                             std::move(transform_fn)})
    {
        return hera::transform_view{std::forward<R>(r),
                                    std::move(transform_fn)};
    }

    template<typename F>
    constexpr auto operator()(F transform_fn) const
        noexcept(noexcept(detail::view_closure{*this, std::move(transform_fn)}))
            -> decltype(detail::view_closure{*this, std::move(transform_fn)})
    {
        return detail::view_closure{*this, std::move(transform_fn)};
    }
};

constexpr auto transform = transform_fn{};
} // namespace views
} // namespace hera