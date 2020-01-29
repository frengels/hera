#pragma once

#include "hera/algorithm/find_if.hpp"
#include "hera/concepts.hpp"
#include "hera/type_identity.hpp"
#include "hera/view/all.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/drop.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<std::size_t BasePos, std::size_t Count, hera::view V, typename Pred>
constexpr auto count_size_impl() noexcept
{
    using dropped_view = decltype(hera::drop_view(
        std::declval<V&>(), std::integral_constant<std::size_t, BasePos>{}));

    constexpr auto opt_index = decltype(
        hera::find_if(std::declval<dropped_view>(), std::declval<Pred>())){};

    if constexpr (decltype(opt_index.has_value())::value)
    {
        return count_size_impl<BasePos + *opt_index + 1, Count + 1, V, Pred>();
    }
    else
    {
        return std::integral_constant<std::size_t, Count>{};
    }
}

template<hera::view V, typename Pred>
constexpr auto count_size() noexcept
{
    if constexpr (hera::unbounded_range<V>)
    {
        return decltype(hera::size(std::declval<V&>())){};
    }
    else
    {
        return count_size_impl<0, 0, V, Pred>();
    }
}

} // namespace detail

template<hera::range V, typename Pred> // clang-format off
    requires hera::view<V> && std::is_object_v<Pred>
class filter_view : public hera::view_interface<filter_view<V, Pred>> // clang-format on
{
private:
    [[no_unique_address]] V base_;

public:
    constexpr filter_view(V base, Pred) noexcept(
        std::is_nothrow_move_constructible_v<V>)
        : base_(std::move(base))
    {}

    template<hera::range R> // clang-format off
        requires hera::viewable_range<R> && hera::constructible_from<V, hera::all_view<R>>
    constexpr filter_view(R&& range, Pred) // clang-format on
        noexcept(noexcept(hera::views::all(std::forward<R>(range))))
        : base_{hera::views::all(std::forward<R>(range))}
    {}

    constexpr V base() const
    {
        return base_;
    }

    constexpr auto size() const noexcept
    {
        return detail::count_size<V, Pred>();
    }

private:
    // solve this mess later
    template<std::size_t Pos, std::size_t Its>
    constexpr decltype(auto) get_impl() const noexcept
    {
        auto dropped_view =
            hera::drop_view(base_, std::integral_constant<std::size_t, Pos>{});

        constexpr auto opt_index =
            decltype(hera::find_if(dropped_view, std::declval<Pred>())){};

        static_assert(opt_index,
                      "We looked past the end and couldn't find matching Pred");

        constexpr std::size_t index = *opt_index;

        if constexpr (Its == 0)
        {
            return hera::get<index + Pos>(base_);
        }
        else
        {
            return get_impl<index + Pos + 1, Its - 1>();
        }
    }

public:
    template<std::size_t I> // clang-format off
        requires (I < decltype(detail::count_size<V, Pred>())::value)
    constexpr auto get() const // clang-format on
    {
        return get_impl<0, I>();
    }
};

template<hera::range R, typename Pred>
filter_view(R&&, Pred)->filter_view<hera::all_view<R>, Pred>;

namespace views
{
struct filter_fn
{
    template<hera::range R,
             typename Pred> // clang-format off
        requires viewable_range<R>
    constexpr auto operator()(R&& r, Pred p) const // clang-format on
        noexcept(noexcept(hera::filter_view{std::forward<R>(r), std::move(p)}))
            -> decltype(hera::filter_view{std::forward<R>(r), std::move(p)})
    {
        return hera::filter_view{std::forward<R>(r), std::move(p)};
    }

    template<typename Pred>
    constexpr auto operator()(Pred p) const
        noexcept(noexcept(detail::view_closure{*this, std::move(p)}))
    {
        return detail::view_closure{*this, std::move(p)};
    }
};

constexpr auto filter = filter_fn{};
} // namespace views
} // namespace hera