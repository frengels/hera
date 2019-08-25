#pragma once

#include "hera/iter_move.hpp"
#include "hera/ranges.hpp"
#include "hera/view.hpp"
#include "hera/view/all.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<forward_range V> // clang-format off
    requires view<V> // clang-format on
    class move_view : public view_interface<move_view<V>> {
private:
    template<forward_iterator I>
    class iterator : public I {
    public:
        constexpr iterator(I base) noexcept(
            std::is_nothrow_move_constructible_v<I>)
            : I{std::move(base)}
        {}

    public:
        constexpr I base() const
            noexcept(std::is_nothrow_copy_constructible_v<I>)
        {
            return *this;
        }

        template<typename D = const I> // clang-format off
            requires dereferenceable<D>
        constexpr decltype(auto) operator*() const // clang-format on
            noexcept(noexcept(hera::iter_move(static_cast<const I&>(*this))))
        {
            return hera::iter_move(static_cast<const I&>(*this));
        }

        // no need to implement iter_move separately, the base should already
        // have this
    };

    template<typename S>
    class sentinel : public S {
    public:
        constexpr sentinel(S sent) noexcept(std::is_move_constructible_v<S>)
            : S{std::move(sent)}
        {}

    private:
        template<typename D = const S> // clang-format off
            requires dereferenceable<D>
        constexpr decltype(auto) operator*() const // clang-format on
            noexcept(noexcept(hera::iter_move(static_cast<const S&>(*this))))
        {
            return hera::iter_move(static_cast<const S&>(*this));
        }

        // no need to implement iter_move separately, the base should already
        // have this
    };

private:
    [[no_unique_address]] V base_;

public:
    constexpr move_view(V base) noexcept(
        std::is_nothrow_move_constructible_v<V>)
        : base_{std::move(base)}
    {}

    constexpr auto begin() const
    {
        return iterator{hera::begin(base_)};
    }

    constexpr auto end() const
    {
        return sentinel{hera::end(base_)};
    }

    constexpr V base() const noexcept(std::is_nothrow_copy_constructible_v<V>)
    {
        return base_;
    }
};

template<forward_range R>
move_view(R &&)->move_view<hera::all_view<R>>;

namespace views
{
struct move_fn : detail::pipeable_interface<move_fn>
{
    template<forward_range R>
    constexpr auto operator()(R&& r) const
        noexcept(noexcept(move_view(std::forward<R>(r))))
    {
        return move_view(std::forward<R>(r));
    }
};

constexpr auto move = move_fn{};
} // namespace views
} // namespace hera