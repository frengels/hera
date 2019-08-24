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
template<forward_range V,
         typename Pred> // clang-format off
    requires view<V> && std::is_object_v<Pred>
class filter_view : public view_interface<filter_view<V, Pred>> { // clang-format on
private:
    template<hera::heterogeneous_iterator I, hera::sentinel_for<I> S>
    class iterator {
    public:
        using difference_type = iter_difference_t<I>;
        using value_type      = iter_value_t<I>;

    private:
        [[no_unique_address]] const I current_;
        [[no_unique_address]] const S end_;

    public:
        constexpr iterator(I current, S end) noexcept(
            std::is_nothrow_move_constructible_v<I>)
            : current_{std::move(current)}, end_{std::move(end)}
        {}

        template<typename J>
        constexpr auto operator==(const iterator<J, S>& other) const
            noexcept(noexcept(current_ == other.current_))
                -> decltype(current_ == other.current_)
        {
            return current_ == other.current_;
        }

        // only implement these comparisons for bounded ranges
        template<bounded_range W = const V>
        constexpr auto operator==(const hera::bounded_sentinel&) const
            noexcept(noexcept(current_ == end_))
        {
            return current_ == end_;
        }

        template<bounded_range W = const V>
        friend constexpr auto
        operator==(const hera::bounded_sentinel& sent,
                   const iterator& it) noexcept(noexcept(it == sent))
        {
            return it == sent;
        }

        template<bounded_range W = const V>
        constexpr auto operator!=(const hera::bounded_sentinel&) const
            noexcept(noexcept(current_ == end_))
        {
            return current_ != end_;
        }

        template<bounded_range W = const V>
        friend constexpr auto
        operator!=(const hera::bounded_sentinel& sent,
                   const iterator& it) noexcept(noexcept(it != sent))
        {
            return it != sent;
        }

        template<typename J>
        constexpr auto operator!=(const iterator<J, S>& other) const
            noexcept(noexcept(current_ != other.current_))
                -> decltype(current_ == other.current_)
        {
            return current_ == other.current_;
        }

        constexpr auto operator++() const
        {
            if constexpr (dereferenceable<decltype(hera::next(current_))>)
            {
                using next_type = decltype(hera::find_if(
                    hera::next(current_), end_, hera::type_identity<Pred>{}));

                return iterator<next_type, S>{
                    hera::find_if(hera::next(current_),
                                  end_,
                                  hera::type_identity<Pred>{}),
                    end_};
            }
            else
            {
                using next_type = decltype(hera::next(current_));
                return iterator<next_type, S>{hera::next(current_), end_};
            }
        }

        template<typename D = const I> // clang-format off
            requires requires(D d)
            {
                { *d } -> can_reference;
            } // clang-format on
        constexpr decltype(auto) operator*() const noexcept(noexcept(*current_))
        {
            return *current_;
        }

        template<typename D = const I> // clang-format off
            requires requires(D d)
            {
                hera::iter_move(d);
            } // clang-format on
        friend constexpr decltype(auto)
        iter_move(const iterator& it) noexcept(noexcept(hera::iter_move(it)))
        {
            return hera::iter_move(it);
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
        return iterator{hera::find_if(hera::begin(base_),
                                      hera::end(base_),
                                      hera::type_identity<Pred>{}),
                        hera::end(base_)};
    }

    constexpr auto end() const noexcept
    {
        if constexpr (bounded_range<V>)
        {
            return hera::bounded_sentinel{};
        }
        else
        {
            return hera::unbounded_sentinel{};
        }
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