#pragma once

#include <type_traits>

#include "hera/concepts.hpp"
#include "hera/ranges.hpp"
#include "hera/view.hpp"

namespace hera
{
namespace detail
{
struct pipeable_base
{};

template<typename T>
concept pipeable = derived_from<T, pipeable_base>&& copy_constructible<T>;

template<typename D>
class pipeable_interface : public pipeable_base
{
    template<typename R>
    friend constexpr auto
    operator|(R&& r, D& drv) noexcept(noexcept(drv(std::forward<R>(r))))
        -> decltype(drv(std::forward<R>(r)))
    {
        return drv(std::forward<R>(r));
    }

    template<typename R>
    friend constexpr auto
    operator|(R&& r, const D& drv) noexcept(noexcept(drv(std::forward<R>(r))))
        -> decltype(drv(std::forward<R>(r)))
    {
        return drv(std::forward<R>(r));
    }

    template<typename R>
    friend constexpr auto
    operator|(R&& r,
              D&& drv) noexcept(noexcept(std::move(drv)(std::forward<R>(r))))
        -> decltype(std::move(drv)(std::forward<R>(r)))
    {
        return std::move(drv)(std::forward<R>(r));
    }

    template<typename R>
    friend constexpr auto operator|(R&& r, const D&& drv) noexcept(
        noexcept(std::move(drv)(std::forward<R>(r))))
        -> decltype(std::move(drv)(std::forward<R>(r)))
    {
        return std::move(drv)(std::forward<R>(r));
    }
};

template<typename Indices, typename F, typename... Ts>
struct view_closure_impl;

template<std::size_t, typename T>
struct box
{
    [[no_unique_address]] T value_;
};

template<std::size_t... Is, typename F, typename... Ts>
struct view_closure_impl<std::index_sequence<Is...>, F, Ts...>
    : private box<Is, Ts>...
{
private:
    [[no_unique_address]] F fn_{};

public:
    view_closure_impl() = default;

    constexpr explicit view_closure_impl(F fn, Ts... ts)
        : box<Is, Ts>{std::forward<Ts>(ts)}..., fn_{std::move(fn)}
    {}

    template<hera::range R> // clang-format off
        requires viewable_range<R>&& 
            invocable<F, R, Ts...>&&
            view<std::invoke_result_t<F, R, Ts...>> // clang-format on
        constexpr auto operator()(R&& r) &&
        noexcept(noexcept(
            std::move(fn_)(std::forward<R>(r),
                           static_cast<box<Is, Ts>&&>(*this).value_...)))
            -> decltype(
                std::move(fn_)(std::forward<R>(r),
                               static_cast<box<Is, Ts>&&>(*this).value_...))
    {
        return std::move(fn_)(std::forward<R>(r),
                              static_cast<box<Is, Ts>&&>(*this).value_...);
    }

    template<hera::range R> // clang-format off
        requires viewable_range<R>&&
            invocable<F, R, Ts&...>&&
            view<std::invoke_result_t<F, R, Ts&...>> // clang-format on
        constexpr auto operator()(R&& r) &
        noexcept(noexcept(fn_(std::forward<R>(r),
                              static_cast<box<Is, Ts>&>(*this).value_...)))
            -> decltype(fn_(std::forward<R>(r),
                            static_cast<box<Is, Ts>&>(*this).value_...))
    {
        return fn_(std::forward<R>(r),
                   static_cast<box<Is, Ts>&>(*this).value_...);
    }

    template<hera::range R> // clang-format off
        requires viewable_range<R>&&
            invocable<F, R, const Ts&...>&&
            view<std::invoke_result_t<F, R, const Ts&...>> // clang-format on
        constexpr auto operator()(R&& r) const& noexcept(
            noexcept(fn_(std::forward<R>(r),
                         static_cast<const box<Is, Ts>&>(*this).value_...)))
            -> decltype(fn_(std::forward<R>(r),
                            static_cast<const box<Is, Ts>&>(*this).value_...))
    {
        return fn_(std::forward<R>(r),
                   static_cast<const box<Is, Ts>&>(*this).value_...);
    }

    template<hera::range R> // clang-format off
        requires viewable_range<R>&&
            invocable<F, R, const Ts&...>&&
            view<std::invoke_result_t<F, R, const Ts&...>> // clang-format on
        constexpr auto operator()(R&& r) const&& noexcept(noexcept(
            std::move(fn_)(std::forward<R>(r),
                           static_cast<const box<Is, Ts>&&>(*this).value_...)))
            -> decltype(fn_(std::forward<R>(r),
                            static_cast<const box<Is, Ts>&&>(*this).value_...))
    {
        return fn_(std::forward<R>(r),
                   static_cast<const box<Is, Ts>&&>(*this).value_...);
    }
};

template<typename F, copy_constructible... Ts>
struct view_closure
    : pipeable_interface<view_closure<F, Ts...>>,
      view_closure_impl<std::index_sequence_for<Ts...>, F, Ts...>
{
    using view_closure_impl<std::index_sequence_for<Ts...>, F, Ts...>::
        view_closure_impl;
};

template<typename F, copy_constructible... Ts>
view_closure(F, Ts&&...)->view_closure<F, Ts...>;

template<pipeable A, pipeable B>
struct view_pipeline : pipeable_interface<view_pipeline<A, B>>
{
private:
    [[no_unique_address]] A left_;
    [[no_unique_address]] B right_;

public:
    view_pipeline() = default;

    constexpr view_pipeline(A&& left, A&& right) noexcept(
        std::is_move_constructible_v<A>&& std::is_move_constructible_v<B>)
        : left_{std::move(left)}, right_{std::move(right)}
    {}

    template<viewable_range R> // clang-format off
        requires invocable<A, R> &&
            invocable<B, std::invoke_result_t<A, R>> // clang-format on
        constexpr decltype(auto) operator()(R&& r) &&
        noexcept(std::is_nothrow_invocable_v<B, std::invoke_result_t<A, R>>)
    {
        return std::move(right_)(std::move(left_)(std::forward<R>(r)));
    }

    template<viewable_range R> // clang-format off
        requires invocable<A&, R> &&
            invocable<B&, std::invoke_result_t<A&, R>> // clang-format on
        constexpr decltype(auto) operator()(R&& r) &
        noexcept(std::is_nothrow_invocable_v<B&, std::invoke_result_t<B&, R>>)
    {
        return right_(left_(std::forward<R>(r)));
    }

    template<viewable_range R> // clang-format off
        requires invocable<const A&, R> &&
            invocable<const B&, std::invoke_result_t<const A&, R>> // clang-format on
        constexpr decltype(auto) operator()(R&& r) const& noexcept(
            std::is_nothrow_invocable_v<const B&,
                                        std::invoke_result_t<const A&, R>>)
    {
        return right_(left_(std::forward<R>(r)));
    }

    template<viewable_range R> // clang-format off
        requires invocable<const A&&, R> &&
            invocable<const B&&, std::invoke_result_t<const A&&, R>> // clang-format on
        constexpr decltype(auto) operator()(R&& r) const&& noexcept(
            std::is_nothrow_invocable_v<const B&&,
                                        std::invoke_result_t<const A&&, R>>)
    {
        return std::move(right_)(std::move(left_)(std::forward<R>(r)));
    }
};
} // namespace detail
} // namespace hera