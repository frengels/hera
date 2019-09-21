#pragma once

#include "hera/begin_end.hpp"
#include "hera/container/integer_sequence.hpp"
#include "hera/container/type_list.hpp"
#include "hera/distance.hpp"
#include "hera/iterator.hpp"
#include "hera/next_prev.hpp"
#include "hera/size.hpp"
#include "hera/view/all.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<typename I>
struct reordered_iterator_maybe_value_type
{};

template<typename I> // clang-format off
    requires 
        requires
        {
            typename iter_value_t<I>;
        } // clang-format on
class reordered_iterator_maybe_value_type<I> {
    using value_type = iter_value_t<I>;
};
} // namespace detail

template<forward_range V, std::size_t... Is> // clang-format off
    requires view<V>
class reordered_view : public view_interface<reordered_view<V, Is...>> { // clang-format on
private:
    [[no_unique_address]] V base_;

    static constexpr hera::index_sequence<Is...> indices_{};

private:
    using base_iterator =
        std::remove_cvref_t<decltype(hera::begin(std::declval<V&>()))>;

    template<std::ptrdiff_t I>
    static constexpr std::ptrdiff_t iter_offset = [] {
        if constexpr (I >= decltype(indices_.size())::value || I < 0)
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
    class iterator
        : detail::reordered_iterator_maybe_value_type<ith_iterator<I>> {
        using begin_iterator_type    = base_iterator;
        using current_iterator_type  = ith_iterator<I>;
        static constexpr auto offset = iter_offset<I>;

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

        template<dereferenceable It = ith_iterator<I>>
        constexpr decltype(auto) operator*() const noexcept(noexcept(*base()))
        {
            return *base();
        }
    };

public:
    constexpr reordered_view(V base, hera::index_sequence<Is...>) noexcept(
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
        auto dist = hera::distance(hera::begin(base_), hera::end(base_));
        return iterator<decltype(dist)::value>(hera::begin(base_));
    }

    friend constexpr auto begin(reordered_view rv) noexcept
    {
        return rv.begin();
    }

    friend constexpr auto end(reordered_view rv) noexcept
    {
        return rv.end();
    }

    constexpr auto size() const noexcept
    {
        return std::integral_constant<std::size_t, sizeof...(Is)>{};
    }
};

template<typename R, std::size_t... Is>
reordered_view(R&&, hera::index_sequence<Is...>)
    ->reordered_view<hera::all_view<R>, Is...>;
} // namespace hera