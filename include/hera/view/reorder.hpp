#pragma once

#include "hera/begin_end.hpp"
#include "hera/container/integer_sequence.hpp"
#include "hera/container/type_list.hpp"
#include "hera/distance.hpp"
#include "hera/iterator/concepts.hpp"
#include "hera/next_prev.hpp"
#include "hera/size.hpp"
#include "hera/view/all.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<typename I, bool Active>
struct reorder_iterator_maybe_value_type
{};

template<typename I> // clang-format off
    requires 
        requires
        {
            typename iter_value_t<I>;
        } // clang-format on
class reorder_iterator_maybe_value_type<I, true> {
    using value_type = iter_value_t<I>;
};
} // namespace detail

template<forward_range V, std::size_t... Is> // clang-format off
    requires view<V>
class reorder_view : public view_interface<reorder_view<V, Is...>> { // clang-format on
private:
    [[no_unique_address]] V base_;

    static constexpr hera::index_sequence<Is...> indices_{};

private:
    using base_iterator =
        std::remove_cvref_t<decltype(hera::begin(std::declval<V&>()))>;

    template<std::ptrdiff_t I>
    static constexpr bool
        in_range = (I < decltype(indices_.size())::value) && I >= 0;

    template<std::ptrdiff_t I>
    static constexpr std::ptrdiff_t iter_offset = [] {
        if constexpr (!in_range<I>)
        {
            return I;
        }
        else
        {
            return decltype(
                indices_[std::integral_constant<std::size_t, I>{}])::value;
        }
    }();

    template<std::ptrdiff_t I>
    using ith_iterator = std::remove_cvref_t<decltype(
        hera::next(std::declval<base_iterator&>(),
                   std::integral_constant<std::ptrdiff_t, iter_offset<I>>{}))>;

    template<std::ptrdiff_t I>
    class iterator : detail::reorder_iterator_maybe_value_type<ith_iterator<I>,
                                                               in_range<I>> {
        using begin_iterator_type           = base_iterator;
        using current_iterator_type         = ith_iterator<I>;
        static constexpr auto offset        = iter_offset<I>;
        static constexpr bool iter_in_range = in_range<I>;

    public:
        using difference_type = std::ptrdiff_t;

    private:
        [[no_unique_address]] begin_iterator_type base_;

    public:
        constexpr iterator(begin_iterator_type begin) noexcept(
            std::is_nothrow_move_constructible_v<begin_iterator_type>)
            : base_{std::move(begin)}
        {}

        constexpr current_iterator_type base() const noexcept(
            std::is_nothrow_copy_constructible_v<current_iterator_type>)
        {
            return hera::next(base_,
                              std::integral_constant<
                                  hera::iter_difference_t<begin_iterator_type>,
                                  offset>{});
        }

        template<difference_type J>
        constexpr auto operator==(const iterator<J>&) const noexcept
            -> std::bool_constant<I == J>
        {
            return {};
        }

        template<difference_type J>
        constexpr auto operator!=(const iterator<J>&) const noexcept
            -> std::bool_constant<I != J>
        {
            return {};
        }

        template<difference_type J>
        constexpr auto operator<(const iterator<J>&) const noexcept
            -> std::bool_constant<(I < J)>
        {
            return {};
        }

        template<difference_type J>
        constexpr auto operator<=(const iterator<J>&) const noexcept
            -> std::bool_constant<(I <= J)>
        {
            return {};
        }

        template<difference_type J>
        constexpr auto operator>(const iterator<J>&) const noexcept
            -> std::bool_constant<(I > J)>
        {
            return {};
        }

        template<difference_type J>
        constexpr auto operator>=(const iterator<J>&) const noexcept
            -> std::bool_constant<(I >= J)>
        {
            return {};
        }

        constexpr auto operator++() const noexcept
        {
            return iterator<I + 1>{base_};
        }

        template<bidirectional_iterator It = begin_iterator_type>
        constexpr auto operator--() const noexcept
        {
            return iterator<I - 1>{base_};
        }

        template<bool InRange = iter_in_range> // clang-format off
            requires InRange
        constexpr decltype(auto) operator*() const noexcept(noexcept(*base())) // clang-format on
        {
            return *base();
        }
    };

public:
    constexpr reorder_view(V base, hera::index_sequence<Is...>) noexcept(
        std::is_nothrow_move_constructible_v<V>)
        : base_{std::move(base)}
    {}

    constexpr reorder_view(V base, std::index_sequence<Is...>) noexcept(
        std::is_nothrow_move_constructible_v<V>)
        : base_{std::move(base)}
    {}

    constexpr V base() const noexcept(std::is_nothrow_copy_constructible_v<V>)
    {
        return base_;
    }

    constexpr auto begin() const noexcept
    {
        return iterator<0>{hera::begin(base_)};
    }

    constexpr auto end() const noexcept
    {
        return iterator<sizeof...(Is)>{hera::begin(base_)};
    }

    friend constexpr auto begin(reorder_view rv) noexcept
    {
        return rv.begin();
    }

    friend constexpr auto end(reorder_view rv) noexcept
    {
        return rv.end();
    }

    constexpr auto size() const noexcept
    {
        return indices_.size();
    }

    constexpr auto empty() const noexcept
    {
        return indices_.empty();
    }
};

template<typename R, std::size_t... Is>
reorder_view(R&&, hera::index_sequence<Is...>)
    ->reorder_view<hera::all_view<R>, Is...>;

template<typename R, std::size_t... Is>
reorder_view(R&&, std::index_sequence<Is...>)
    ->reorder_view<hera::all_view<R>, Is...>;

namespace views
{
struct reorder_fn
{
    template<forward_range R, std::size_t... Is> // clang-format off
            requires viewable_range<R> // clang-format on
        constexpr auto operator()(R&&                         r,
                                  hera::index_sequence<Is...> indices) const
        noexcept(noexcept(hera::reorder_view{std::forward<R>(r),
                                             std::move(indices)}))
            -> decltype(hera::reorder_view{std::forward<R>(r),
                                           std::move(indices)})
    {
        return hera::reorder_view{std::forward<R>(r), std::move(indices)};
    }

    template<forward_range R, std::size_t... Is> // clang-format off
        requires viewable_range<R> // clang-format on
        constexpr auto operator()(R&&                        r,
                                  std::index_sequence<Is...> indices) const
        noexcept(noexcept(hera::reorder_view{std::forward<R>(r),
                                             std::move(indices)}))
            -> decltype(hera::reorder_view{std::forward<R>(r),
                                           std::move(indices)})
    {
        return hera::reorder_view{std::forward<R>(r), std::move(indices)};
    }

    template<std::size_t... Is>
    constexpr auto operator()(hera::index_sequence<Is...> indices) const
        noexcept(noexcept(detail::view_closure{*this, std::move(indices)}))
    {
        return detail::view_closure{*this, std::move(indices)};
    }

    template<std::size_t... Is>
    constexpr auto operator()(std::index_sequence<Is...> indices) const
        noexcept(noexcept(detail::view_closure{*this, std::move(indices)}))
    {
        return detail::view_closure{*this, std::move(indices)};
    }
};

constexpr auto reorder = reorder_fn{};
} // namespace views
} // namespace hera