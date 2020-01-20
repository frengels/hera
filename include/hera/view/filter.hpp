#pragma once

#include "hera/algorithm/find_if.hpp"
#include "hera/concepts.hpp"
#include "hera/iterator/iter_move.hpp"
#include "hera/iterator/sentinel.hpp"
#include "hera/type_identity.hpp"
#include "hera/view/all.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/drop.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
/*
namespace detail
{
template<typename I>
struct filter_iterator_maybe_value_type
{};

template<typename I> // clang-format off
requires
    requires
    {
        typename iter_value_t<I>;
    } // clang-format on
struct filter_iterator_maybe_value_type<I>
{
using value_type = iter_value_t<I>;
};
} // namespace detail

template<forward_range V,
     typename Pred> // clang-format off
requires view<V> && std::is_object_v<Pred>
class filter_view : public view_interface<filter_view<V, Pred>> { //
clang-format on private: template<hera::forward_iterator I> class iterator :
public detail::filter_iterator_maybe_value_type<I>
{
    template<typename S>
    friend class sentinel;

    template<typename I_>
    friend class iterator;

public:
    using difference_type = iter_difference_t<I>;

private:
    [[no_unique_address]] const I it_;

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

private:
    template<typename J, typename NextFn>
    static constexpr auto predicate_next_impl(J it, NextFn op) noexcept
    {
        if constexpr (!dereferenceable<J>)
        {
            return it;
        }
        // return type is a constant so it's safe to ::value
        else if constexpr (std::invoke_result_t<Pred, decltype(*it)>::value)
        {
            return it;
        }
        else
        {
            return predicate_next_impl(op(std::move(it)), std::move(op));
        }
    }

    template<typename J, typename NextFn>
    static constexpr auto predicate_next(J it, NextFn op) noexcept(
        noexcept(predicate_next_impl(op(std::move(it)), std::move(op))))
    {
        return predicate_next_impl(op(std::move(it)), std::move(op));
    }

public:
    constexpr auto operator++() const noexcept(
        noexcept(iterator<decltype(predicate_next(it_, hera::next))>{
            predicate_next(it_, hera::next)}))
    {
        using iterator_type = decltype(predicate_next(it_, hera::next));
        return iterator<iterator_type>{predicate_next(it_, hera::next)};
    }

    template<hera::bidirectional_iterator J = I>
    constexpr auto operator--() const noexcept(
        noexcept(iterator<decltype(predicate_next(it_, hera::prev))>{
            predicate_next(it_, hera::prev)}))
    {
        using iterator_type = decltype(predicate_next(it_, hera::prev));
        return iterator<iterator_type>{predicate_next(it_, hera::prev)};
    }

    template<dereferenceable D = const I>
    constexpr decltype(auto) operator*() const noexcept(noexcept(*it_))
    {
        return *it_;
    }

    template<typename D = const I> // clang-format off
        requires requires(D d)
        {
            hera::iter_move(d);
        } // clang-format on
    friend constexpr decltype(auto) iter_move(const iterator& it) noexcept(
        noexcept(hera::iter_move(it.it_)))
    {
        return hera::iter_move(it.it_);
    }
};

template<typename S>
class sentinel
{
private:
    [[no_unique_address]] S sent_;

public:
    constexpr sentinel(S sent) noexcept(
        std::is_nothrow_move_constructible_v<S>)
        : sent_{std::move(sent)}
    {}

    constexpr S base() const
        noexcept(std::is_nothrow_copy_constructible_v<S>)
    {
        return sent_;
    }

    template<typename I> // clang-format off
        requires hera::sentinel_for<S, I> // clang-format on
        constexpr auto operator==(const iterator<I>& it) const
        noexcept(noexcept(sent_ == it.it_))
    {
        return sent_ == it.it_;
    }

    template<typename I> // clang-format off
        requires hera::sentinel_for<S, I> // clang-format on
        friend constexpr auto
        operator==(const iterator<I>& it,
                   const sentinel<S>& sent) noexcept(noexcept(it.it_ ==
                                                              sent.sent_))
    {
        return it.it_ == sent.sent_;
    }

    template<typename I> // clang-format off
        requires hera::sentinel_for<S, I> // clang-format on
        constexpr auto operator!=(const iterator<I>& it) const
        noexcept(noexcept(sent_ != it.it_))
    {
        return sent_ != it.it_;
    }

    template<typename I> // clang-format off
        requires hera::sentinel_for<S, I> // clang-format on
        friend constexpr auto
        operator!=(const iterator<I>& it,
                   const sentinel<S>& sent) noexcept(noexcept(it.it_ !=
                                                              sent.sent_))
    {
        return it.it_ != sent.sent_;
    }
};

private:
[[no_unique_address]] V base_;
// we don't actually need to store the function, we only need its type to
// deduce the return types of invoking the predicate. Pred pred_;

public:
constexpr filter_view(V base, Pred) noexcept(
    std::is_nothrow_move_constructible_v<V>)
    : base_{std::move(base)}
{}

template<forward_range R> // clang-format off
    requires viewable_range<R> && constructible_from<V, all_view<R>>
constexpr filter_view(R&& r, Pred) noexcept( // clang-format on
        noexcept(hera::views::all(std::forward<R>(r))))
    : base_{hera::views::all(std::forward<R>(r))}
{}

template<hera::metafunction F>
constexpr filter_view(V base,
                      F) noexcept(std::is_nothrow_move_constructible_v<V>)
    : base_{std::move(base)}
{}

constexpr V base() const noexcept(std::is_nothrow_copy_constructible_v<V>)
{
    return base_;
}

constexpr auto begin() const noexcept
{
    return iterator{hera::find_if(
        hera::begin(base_), hera::end(base_), hera::type_identity<Pred>{})};
}

constexpr auto end() const noexcept
{
    return sentinel{hera::end(base_)};
}

friend constexpr auto begin(filter_view&& v) noexcept(noexcept(v.begin()))
{
    return v.begin();
}

friend constexpr auto end(filter_view&& v) noexcept(noexcept(v.end()))
{
    return v.end();
}
};

template<forward_range R, typename Pred>
filter_view(R&&, Pred)->filter_view<hera::all_view<R>, Pred>;

template<forward_range R, metafunction F>
filter_view(R&&, F)->filter_view<hera::all_view<R>, typename F::type>;

*/
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

private:
    template<std::size_t I>
    constexpr auto find_size() const noexcept
    {
        if constexpr (hera::bounded_range<V>)
        {
            if constexpr (hera::same_as<decltype(try_at<I>()), hera::none>)
            {
                return std::integral_constant<std::size_t, I>{};
            }
            else
            {
                return find_size<I + 1>();
            }
        }
        else
        {
            return base_.size();
        }
    }

public:
    constexpr auto size() const noexcept
    {
        return find_size<0>();
    }

private:
    template<std::size_t Pos, std::size_t Its>
    constexpr decltype(auto) try_at_impl() const noexcept
    {
        auto dropped_view =
            hera::drop_view(base_, std::integral_constant<std::size_t, Pos>{});

        return decltype(hera::find_if(dropped_view, std::declval<Pred>())){}
            .and_then([&](auto pos_constant) {
                constexpr std::size_t pos = pos_constant;
                if constexpr (Its == 0)
                {
                    return hera::try_at<pos + Pos>(base_);
                }
                else
                {
                    return try_at_impl<pos + Pos + 1, Its - 1>();
                }
            });
    }

public:
    /// returns just<X> if an element was found or none<void> if not
    template<std::size_t I>
    constexpr auto try_at() const noexcept
    {
        return try_at_impl<0, I>();
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