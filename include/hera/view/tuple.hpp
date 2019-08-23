#pragma once

#include <tuple>

#include "hera/constant.hpp"
#include "hera/get.hpp"
#include "hera/sentinel.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
// required to be defined like this because gcc doesn't respect the callees
// constraints when evaluating.
template<typename Tuple, std::ptrdiff_t I>
concept tuple_has_get_for = (I >= 0) && requires(Tuple& tup)
{
    hera::get<static_cast<std::size_t>(I)>(tup);
};

template<typename Tuple, std::ptrdiff_t I>
struct tuple_iterator_value_type
{};

template<typename Tuple, std::ptrdiff_t I> // clang-format off
    requires (I >= 0) && (I < std::tuple_size_v<std::remove_cvref_t<Tuple>>)
struct tuple_iterator_value_type<Tuple, I> // clang-format on
{
    using value_type =
        std::remove_cvref_t<decltype(hera::get<I>(std::declval<Tuple&>()))>;
};
} // namespace detail

// takes anything that acts or smells like a tuple and makes it workable with
// heterogeneous ranges.
template<typename Tuple>
class tuple_view : public hera::view_interface<tuple_view<Tuple>> {
    using size_type = std::size_t;

    template<typename Tuple_>
    friend class tuple_view;

    template<std::ptrdiff_t I>
    struct iterator : public detail::tuple_iterator_value_type<Tuple, I>
    {
    public:
        using difference_type = std::ptrdiff_t;

    private:
        static constexpr std::size_t bound =
            std::tuple_size_v<std::remove_const_t<Tuple>>;

    private:
        Tuple& tuple_;

    public:
        explicit constexpr iterator(Tuple& tup) noexcept : tuple_{tup}
        {}

        template<typename OtherTuple> // clang-format off
            requires std::is_const_v<Tuple> && 
                same_as<std::remove_const_t<Tuple>, OtherTuple>
        constexpr iterator( // clang-format on
                const typename tuple_view<OtherTuple>::template iterator<I>&
                    other) noexcept
            : tuple_{other.tuple_}
        {}

        template<std::ptrdiff_t J>
        constexpr auto operator==(const iterator<J>& other) const noexcept
        {
            return std::bool_constant<(I == J)>{};
        }

        template<std::ptrdiff_t J>
        constexpr auto operator!=(const iterator<J>& other) const noexcept
        {
            return std::bool_constant<(I != J)>{};
        }

        constexpr auto operator==(const hera::bounded_sentinel&) const noexcept
        {
            return std::bool_constant<I == bound>{};
        }

        friend constexpr auto operator==(const hera::bounded_sentinel&,
                                         const iterator&) noexcept
        {
            return std::bool_constant<bound == I>{};
        }

        constexpr auto operator!=(const hera::bounded_sentinel&) const noexcept
        {
            return std::bool_constant<I != bound>{};
        }

        friend constexpr auto operator!=(const hera::bounded_sentinel&,
                                         const iterator&) noexcept
        {
            return std::bool_constant<bound != I>{};
        }

        template<difference_type J>
        constexpr auto operator<(const iterator<J>&) const noexcept
        {
            return std::bool_constant<(I < J)>{};
        }

        constexpr auto operator<(const hera::bounded_sentinel&) const noexcept
        {
            return std::bool_constant<(I < bound)>{};
        }

        friend constexpr auto operator<(const hera::bounded_sentinel&,
                                        const iterator&) noexcept
        {
            return std::bool_constant<(bound < I)>{};
        }

        template<difference_type J>
        constexpr auto operator>(const iterator<J>&) const noexcept
        {
            return std::bool_constant<(I > J)>{};
        }

        constexpr auto operator>(const hera::bounded_sentinel&) const noexcept
        {
            return std::bool_constant<(I > bound)>{};
        }

        friend constexpr auto operator>(const hera::bounded_sentinel&,
                                        const iterator&) noexcept
        {
            return std::bool_constant<(bound > I)>{};
        }

        template<difference_type J>
        constexpr auto operator<=(const iterator<J>&) const noexcept
        {
            return std::bool_constant<(I <= J)>{};
        }

        constexpr auto operator<=(const hera::bounded_sentinel&) const noexcept
        {
            return std::bool_constant<(I <= bound)>{};
        }

        friend constexpr auto operator<=(const hera::bounded_sentinel&,
                                         const iterator&) noexcept
        {
            return std::bool_constant<(bound <= I)>{};
        }

        template<difference_type J>
        constexpr auto operator>=(const iterator<J>&) const noexcept
        {
            return std::bool_constant<(I >= J)>{};
        }

        constexpr auto operator>=(const hera::bounded_sentinel&) const noexcept
        {
            return std::bool_constant<(I >= bound)>{};
        }

        friend constexpr auto operator>=(const hera::bounded_sentinel&,
                                         const iterator&) noexcept
        {
            return std::bool_constant<(bound >= bound)>{};
        }

        constexpr auto operator++() const noexcept
        {
            return iterator<I + 1>{tuple_};
        }

        constexpr auto operator--() const noexcept
        {
            return iterator<I - 1>{tuple_};
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr auto operator+(C) const noexcept
        {
            return iterator<I + C::value>{tuple_};
        }

        template<hera::constant_convertible_to<difference_type> C>
        friend constexpr auto operator+(C n, const iterator& it) noexcept
        {
            return it + n;
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr auto operator-(C) const noexcept
        {
            return iterator<I - C::value>{tuple_};
        }

        template<difference_type J>
        constexpr auto operator-(const iterator<J>&) const noexcept
        {
            return std::integral_constant<difference_type, I - J>{};
        }

        constexpr auto operator-(const hera::bounded_sentinel&) const noexcept
        {
            return std::integral_constant<difference_type, I - bound>{};
        }

        friend constexpr auto operator-(const hera::bounded_sentinel&,
                                        const iterator&) noexcept
        {
            return std::integral_constant<difference_type, bound - I>{};
        }

        template<detail::tuple_has_get_for<I> D = Tuple>
        constexpr decltype(auto) operator*() const
            noexcept(noexcept(hera::get<I>(tuple_)))
        {
            return hera::get<I>(tuple_);
        }

        template<hera::constant_convertible_to<difference_type> C>
        constexpr auto operator[](C n) const noexcept(noexcept(*((*this) + n)))
            -> decltype(*((*this) + n))
        {
            return *((*this) + n);
        }
    };

private:
    Tuple& tuple_;

public:
    explicit constexpr tuple_view(Tuple& tup) noexcept : tuple_{tup}
    {}

    constexpr auto begin() const noexcept
    {
        return iterator<0>{tuple_};
    }

    constexpr auto end() const noexcept
    {
        return hera::bounded_sentinel{};
    }

    friend constexpr auto begin(tuple_view&& tv) noexcept(noexcept(tv.end()))
    {
        return tv.begin();
    }

    friend constexpr auto end(tuple_view&& tv) noexcept(noexcept(tv.end()))
    {
        return tv.end();
    }

    template<hera::constant_convertible_to<size_type> C>
    constexpr auto operator[](C) const
        noexcept(noexcept(hera::get<C::value>(tuple_)))
            -> decltype(hera::get<C::value>(tuple_))
    {
        return hera::get<C::value>(tuple_);
    }
};
} // namespace hera