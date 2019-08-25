#pragma once

#include "hera/algorithm/find_if.hpp"
#include "hera/concepts.hpp"
#include "hera/iter_move.hpp"
#include "hera/sentinel.hpp"
#include "hera/type_identity.hpp"
#include "hera/view/all.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
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
class filter_view : public view_interface<filter_view<V, Pred>> { // clang-format on
private:
    template<hera::forward_iterator I>
    class iterator : public detail::filter_iterator_maybe_value_type<I> {
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
        static constexpr auto predicate_next(const J& it, NextFn op)
        {
            if constexpr (!dereferenceable<const J>)
            {
                return it;
            }
            else if constexpr (std::invoke_result_t<Pred, decltype(*it)>::value)
            {
                return it;
            }
            else
            {
                return predicate_next(op(it), std::move(op));
            }
        }

    public:
        constexpr auto operator++() const
        {
            using iterator_type =
                decltype(predicate_next(hera::next(it_), hera::next));
            return iterator<iterator_type>{
                predicate_next(hera::next(it_), hera::next)};
        }

        template<typename J = I> // clang-format off
            requires hera::bidirectional_iterator<J> // clang-format on
            constexpr auto operator--() const
        {
            using iterator_type =
                decltype(predicate_next(hera::prev(it_), hera::prev));
            return iterator<iterator_type>{
                predicate_next(hera::prev(it_), hera::prev)};
        }

        template<typename D = const I> // clang-format off
            requires requires(D d)
            {
                { *d } -> can_reference;
            } // clang-format on
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
    class sentinel {
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

    template<forward_range R, hera::metafunction F>
    constexpr filter_view(R&& r, F) noexcept(
        noexcept(hera::views::all(std::forward<R>(r))))
        : base_{hera::views::all(std::forward<R>(r))}
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

namespace views
{
struct filter_fn
{
    template<forward_range R,
             typename Pred> // clang-format off
        requires viewable_range<R> // clang-format on
        constexpr auto operator()(R&& r, Pred p) const
        noexcept(noexcept(hera::filter_view<hera::all_view<R>, Pred>{
            std::forward<R>(r),
            hera::type_identity<Pred>{}}))
            -> decltype(hera::filter_view<hera::all_view<R>, Pred>{
                std::forward<R>(r),
                hera::type_identity<Pred>{}})
    {
        return hera::filter_view<hera::all_view<R>, Pred>{
            std::forward<R>(r), hera::type_identity<Pred>{}};
    }

    template<forward_range R, metafunction PredMeta> // clang-format off
        requires viewable_range<R> // clang-format on
        constexpr auto operator()(R&& r, PredMeta pm) const
    {
        return hera::filter_view<hera::all_view<R>, typename PredMeta::type>{
            std::forward<R>(r), pm};
    }

    template<typename Pred>
    constexpr auto operator()(Pred p) const
        noexcept(noexcept(detail::view_closure{*this,
                                               hera::type_identity<Pred>{}}))
    {
        return detail::view_closure{*this, hera::type_identity<Pred>{}};
    }

    template<metafunction PredMeta>
    constexpr auto operator()(PredMeta pm) const
        noexcept(noexcept(detail::view_closure{*this, std::move(pm)}))
    {
        return detail::view_closure{*this, std::move(pm)};
    }
};

constexpr auto filter = filter_fn{};
} // namespace views
} // namespace hera