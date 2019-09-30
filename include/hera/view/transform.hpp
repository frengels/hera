#pragma once

#include "hera/concepts.hpp"
#include "hera/iterator/concepts.hpp"
#include "hera/view/all.hpp"
#include "hera/view/detail/closure.hpp"

namespace hera
{
namespace detail
{
template<typename I>
struct transform_iterator_maybe_value_type
{};

template<typename I> // clang-format off
    requires
        requires
        {
            typename iter_value_t<I>;
        } // clang-format on
struct transform_iterator_maybe_value_type<I>
{
    using value_type = iter_value_t<I>;
};
} // namespace detail

template<forward_range V, typename F> // clang-format off
    requires view<V> && std::is_object_v<F>
class transform_view : public view_interface<transform_view<V, F>> { // clang-format on
private:
    template<hera::forward_iterator I>
    class iterator : public detail::transform_iterator_maybe_value_type<I> {
        template<typename S>
        friend class sentinel;

        template<typename I_>
        friend class iterator;

    public:
        using difference_type = iter_difference_t<I>;

    private:
        [[no_unique_address]] const transform_view& parent_;
        [[no_unique_address]] const I               it_;

    public:
        constexpr iterator(const transform_view& parent, I it) noexcept(
            std::is_nothrow_move_constructible_v<I>)
            : parent_{parent}, it_{std::move(it)}
        {}

        constexpr I base() const
            noexcept(std::is_nothrow_copy_constructible_v<I>)
        {
            return it_;
        }

        template<typename J>
        constexpr auto operator==(const iterator<J>& other) const
            noexcept(noexcept(it_ == other.it_)) -> decltype(it_ == other.it_)
        {
            return it_ == other.it_;
        }

        template<typename J>
        constexpr auto operator!=(const iterator<J>& other) const
            noexcept(noexcept(it_ != other.it_)) -> decltype(it_ == other.it_)
        {
            return it_ == other.it_;
        }

        template<hera::random_access_iterator J>
        constexpr auto operator<(const iterator<J>& other) const
            noexcept(noexcept(it_ < other.it_))
        {
            return it_ < other.it_;
        }

        template<hera::random_access_iterator J>
        constexpr auto operator>(const iterator<J>& other) const
            noexcept(noexcept(it_ > other.it_))
        {
            return it_ > other.it_;
        }

        template<hera::random_access_iterator J>
        constexpr auto operator<=(const iterator<J>& other) const
            noexcept(noexcept(it_ <= other.it_))
        {
            return it_ <= other.it_;
        }

        template<hera::random_access_iterator J>
        constexpr auto operator>=(const iterator<J>& other) const
            noexcept(noexcept(it_ >= other.it_))
        {
            return it_ >= other.it_;
        }

        constexpr auto operator++() const noexcept(noexcept(
            iterator<decltype(hera::next(it_))>{parent_, hera::next(it_)}))
        {
            return iterator<decltype(hera::next(it_))>{parent_,
                                                       hera::next(it_)};
        }

        template<hera::bidirectional_iterator J = I>
        constexpr auto operator--() const noexcept(noexcept(
            iterator<decltype(hera::prev(it_))>{parent_, hera::prev(it_)}))
        {
            return iterator<decltype(hera::prev(it_))>{parent_,
                                                       hera::prev(it_)};
        }

        template<hera::constant_convertible_to<difference_type> C,
                 hera::random_access_iterator                   J = I>
        constexpr auto operator+(C n) const
            noexcept(noexcept(iterator<decltype(it_ + n)>{parent_, it_ + n}))
        {
            return iterator<decltype(it_ + n)>{parent_, it_ + n};
        }

        template<hera::constant_convertible_to<difference_type> C,
                 hera::random_access_iterator                   J = I>
        friend constexpr auto operator+(C n, const iterator& it) noexcept(
            noexcept(iterator<decltype(n + it.it_)>{it.parent_, n + it.it_}))
            -> decltype(iterator<decltype(n + std::declval<const J&>())>{
                it.parent_,
                n + std::declval<const J&>()}) // this hefty workaround is
                                               // required to make the function
                                               // depend on J
        {
            return iterator<decltype(n + it.it_)>{it.parent_, n + it.it_};
        }

        template<hera::constant_convertible_to<difference_type> C,
                 hera::random_access_iterator                   J = I>
        constexpr auto operator-(C n) const
            noexcept(noexcept(iterator<decltype(it_ - n)>{parent_, it_ - n}))
        {
            return iterator<decltype(it_ - n)>{parent_, it_ - n};
        }

        template<hera::sized_sentinel_for<I> J>
        constexpr auto operator-(const iterator<J>& other) const
            noexcept(noexcept(it_ - other.it_))
        {
            return it_ - other.it_;
        }

        template<dereferenceable J = const I>
        constexpr decltype(auto) operator*() const
            noexcept(noexcept(std::invoke(parent_.transform_fn_, *it_)))
        {
            return std::invoke(parent_.transform_fn_, *it_);
        }
    };

    template<typename S>
    class sentinel {
    private:
        [[no_unique_address]] S sent_;

    public:
        constexpr sentinel(S sent) noexcept(std::is_move_constructible_v<S>)
            : sent_{std::move(sent)}
        {}

        template<forward_iterator I> // clang-format off
            requires sentinel_for<S, I> // clang-format on
            constexpr auto operator==(const iterator<I>& other) const
            noexcept(noexcept(sent_ == other.it_))
        {
            return sent_ == other.it_;
        }

        template<forward_iterator I> // clang-format off
            requires sentinel_for<S, I> // clang-format on
            friend constexpr auto operator==(
                const iterator<I>& it,
                const sentinel& sent) noexcept(noexcept(it.it_ == sent.sent_))
        {
            return it.it_ == sent.sent_;
        }

        template<forward_iterator I> // clang-format off
            requires sentinel_for<S, I> // clang-format on
            constexpr auto operator!=(const iterator<I>& other) const
            noexcept(noexcept(sent_ != other.it_))
        {
            return sent_ != other.it_;
        }

        template<forward_iterator I> // clang-format off
            requires sentinel_for<S, I> // clang-format on
            friend constexpr auto operator!=(
                const iterator<I>& it,
                const sentinel& sent) noexcept(noexcept(it.it_ != sent.sent_))
        {
            return it.it_ != sent.sent_;
        }

        template<forward_iterator I> // clang-format off
            requires sized_sentinel_for<S, I> // clang-format on
            constexpr auto operator-(const iterator<I>& it) const
            noexcept(noexcept(sent_ - it.it_))
        {
            return sent_ - it.it_;
        }

        template<forward_iterator I> // clang-format off
            requires sized_sentinel_for<S, I> // clang-format on
            friend constexpr auto operator-(
                const iterator<I>& it,
                const sentinel&    sent) noexcept(noexcept(it.it_ - sent.sent_))
        {
            return it.it_ - sent.sent_;
        }
    };

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

    constexpr auto begin() const
        noexcept(noexcept(iterator{*this, hera::begin(base_)}))
    {
        return iterator{*this, hera::begin(base_)};
    }

    constexpr auto end() const noexcept(noexcept(sentinel{hera::end(base_)}))
    {
        return sentinel{hera::end(base_)};
    }
};

template<forward_range R, typename F>
transform_view(R&&, F)->transform_view<hera::all_view<R>, F>;

namespace views
{
struct transform_fn
{
    template<forward_range R, typename F>
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