#pragma once

#include "hera/next_prev.hpp"
#include "hera/view.hpp"
#include "hera/view/all.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<typename I, typename J>
struct head_iterator_maybe_value_type
{};

template<typename I, same_as<I> J>
struct head_iterator_maybe_value_type<I, J>
{
    using value_type = iter_value_t<I>;
};
} // namespace detail

template<forward_range V> // clang-format off
    requires view<V> && !empty_range<V> 
class head_view : public view_interface<head_view<V>> { // clang-format on
private:
    template<forward_iterator I>
    class iterator
        : public detail::head_iterator_maybe_value_type<iterator_t<V>, I> {
    private:
        using head_iterator_type = iterator_t<V>;

        template<typename S>
        friend class sentinel;

    public:
        using difference_type = iter_difference_t<I>;

    private:
        [[no_unique_address]] I it_;

    public:
        constexpr iterator(I it) noexcept(
            std::is_nothrow_move_constructible_v<I>)
            : it_{std::move(it)}
        {}

        constexpr I base() const
            noexcept(std::is_nothrow_copy_constructible_v<I>)
        {
            return it_;
        }

        template<forward_iterator J>
        constexpr auto operator==(const iterator<J>& other) const noexcept
        {
            return it_ == other.it_;
        }

        template<forward_iterator J>
        constexpr auto operator!=(const iterator<J>& other) const noexcept
        {
            return it_ != other.it_;
        }

        // minimum requirements to be considered an iterator
        constexpr auto operator++() const noexcept
        {
            using next_type = decltype(++it_);
            return iterator<next_type>{++it_};
        }

        template<bidirectional_iterator J = I>
        constexpr auto operator--() const noexcept
        {
            using prev_type = decltype(--it_);
            return iterator<prev_type>{--it_};
        }

        template<same_as<head_iterator_type> J = I>
        constexpr decltype(auto) operator*() const noexcept(noexcept(*it_))
        {
            return *it_;
        }

        template<same_as<head_iterator_type> J = I>
        friend constexpr decltype(auto) iter_move(const iterator& it) noexcept(
            noexcept(hera::iter_move(it.it_)))
        {
            return hera::iter_move(it.it_);
        }
    };

    template<typename S>
    class sentinel {
    private:
        using head_iterator_type = iterator_t<V>;

    private:
        [[no_unique_address]] S sent_;

    public:
        constexpr sentinel(S sent) noexcept(
            std::is_nothrow_move_constructible_v<S>)
            : sent_{std::move(sent)}
        {}

        template<forward_iterator I>
        constexpr auto operator==(const iterator<I>& it) const
            noexcept(noexcept(sent_ == it.it_))
        {
            return sent_ == it.it_;
        }

        template<forward_iterator I>
        friend constexpr auto operator==(
            const iterator<I>& it,
            const sentinel<S>& sent) noexcept(noexcept(it.it_ == sent.sent_))
        {
            return it.it_ == sent.sent_;
        }

        template<forward_iterator I>
        constexpr auto operator!=(const iterator<I>& it) const
            noexcept(noexcept(sent_ != it.it_))
        {
            return sent_ != it.it_;
        }

        template<forward_iterator I>
        friend constexpr auto operator!=(
            const iterator<I>& it,
            const sentinel<S>& sent) noexcept(noexcept(it.it_ != sent.sent_))
        {
            return it.it_ != sent.sent_;
        }
    };

private:
    [[no_unique_address]] V base_;

public:
    constexpr head_view(V base) noexcept(
        std::is_nothrow_move_constructible_v<V>)
        : base_{std::move(base)}
    {}

    constexpr V base() const noexcept(std::is_nothrow_copy_constructible_v<V>)
    {
        return base_;
    }

    constexpr auto begin() const
        noexcept(noexcept(iterator{hera::begin(base_)}))
    {
        return iterator{hera::begin(base_)};
    }

    constexpr auto end() const
        noexcept(noexcept(sentinel{hera::next(hera::begin(base_))}))
    {
        return sentinel{hera::next(hera::begin(base_))};
    }

    friend constexpr auto begin(head_view&& v) noexcept(noexcept(v.begin()))
    {
        return v.begin();
    }

    friend constexpr auto end(head_view&& v) noexcept(noexcept(v.end()))
    {
        return v.end();
    }

    constexpr std::integral_constant<std::size_t, 1> size() const noexcept
    {
        return {};
    }

    constexpr std::false_type empty() const noexcept
    {
        return {};
    }
};

template<forward_range R>
head_view(R &&)->head_view<hera::all_view<R>>;

namespace views
{
struct head_fn : public detail::pipeable_interface<head_fn>
{
    template<typename R>
    constexpr auto operator()(R&& r) const
        noexcept(noexcept(head_view{std::forward<R>(r)}))
    {
        return head_view{std::forward<R>(r)};
    }
};

constexpr auto head = head_fn{};
} // namespace views
} // namespace hera