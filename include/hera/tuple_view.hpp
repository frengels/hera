#pragma once

#include <tuple>

#include "hera/constant.hpp"
#include "hera/get.hpp"
#include "hera/sentinel.hpp"
#include "hera/view_interface.hpp"

namespace hera
{
namespace detail
{
template<typename Tuple, std::ptrdiff_t I>
struct tuple_iterator_value_type
{};

template<typename Tuple, std::ptrdiff_t I> // clang-format off
    requires I >= 0 &&
        requires
        {
            typename std::tuple_element_t<I, Tuple>;
        } // clang-format on
struct tuple_iterator_value_type<Tuple, I>
{
    using value_type = std::tuple_element_t<I, Tuple>;
};
} // namespace detail

template<typename Tuple, std::ptrdiff_t I> // clang-format off
    requires std::is_object_v<Tuple>
class tuple_iterator : detail::tuple_iterator_value_type<Tuple, I> {
public:
    using difference_type = std::ptrdiff_t;

private:
    static constexpr std::size_t bound = std::tuple_size_v<std::remove_const_t<Tuple>>;

private:
    Tuple& tuple_;

public:
    explicit constexpr tuple_iterator(Tuple& tup) noexcept : tuple_{tup}
    {}

    template<typename OtherTuple> // clang-format off
        requires std::is_const_v<Tuple> && 
            same_as<std::remove_const_t<Tuple>, OtherTuple>
    constexpr tuple_iterator( // clang-format on
            const tuple_iterator<OtherTuple, I>& other) noexcept
        : tuple_{other.tuple_}
    {}

    template<std::ptrdiff_t J>
    constexpr auto operator==(const tuple_iterator<Tuple, J>& other) const
        noexcept
    {
        return std::bool_constant<(I == J)>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator!=(const tuple_iterator<Tuple, J>& other) const
        noexcept
    {
        return std::bool_constant<(I != J)>{};
    }

    constexpr auto operator==(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<I == bound>{};
    }

    friend constexpr auto operator==(const hera::bounded_sentinel&,
                                     const tuple_iterator&) noexcept
    {
        return std::bool_constant<bound == I>{};
    }

    constexpr auto operator!=(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<I != bound>{};
    }

    friend constexpr auto operator!=(const hera::bounded_sentinel&,
                                     const tuple_iterator&) noexcept
    {
        return std::bool_constant<bound != I>{};
    }

    constexpr auto operator<(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<(I < bound)>{};
    }

    friend constexpr auto operator<(const hera::bounded_sentinel&,
                                    const tuple_iterator&) noexcept
    {
        return std::bool_constant<(bound < I)>{};
    }

    constexpr auto operator>(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<(I > bound)>{};
    }

    friend constexpr auto operator>(const hera::bounded_sentinel&,
                                    const tuple_iterator&) noexcept
    {
        return std::bool_constant<(bound > I)>{};
    }

    constexpr auto operator<=(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<(I <= bound)>{};
    }

    friend constexpr auto operator<=(const hera::bounded_sentinel&,
                                     const tuple_iterator&) noexcept
    {
        return std::bool_constant<(bound <= I)>{};
    }

    constexpr auto operator>=(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<(I >= bound)>{};
    }

    friend constexpr auto operator>=(const hera::bounded_sentinel&,
                                     const tuple_iterator&) noexcept
    {
        return std::bool_constant<(bound >= bound)>{};
    }

    constexpr auto operator++() const noexcept
    {
        return tuple_iterator<Tuple, I + 1>{tuple_};
    }

    constexpr auto operator--() const noexcept
    {
        return tuple_iterator<Tuple, I - 1>{tuple_};
    }

    template<hera::constant_convertible_to<difference_type> C>
    constexpr auto operator+(C) const noexcept
    {
        return tuple_iterator<Tuple, I + C::value>{tuple_};
    }

    template<hera::constant_convertible_to<difference_type> C>
    friend constexpr auto operator+(C n, const tuple_iterator& it) noexcept
    {
        return it + n;
    }

    template<hera::constant_convertible_to<difference_type> C>
    constexpr auto operator-(C) const noexcept
    {
        return tuple_iterator<Tuple, I - C::value>{tuple_};
    }

    template<difference_type J>
    constexpr auto operator-(const tuple_iterator<Tuple, J>&) const noexcept
    {
        return std::integral_constant<difference_type, I - J>{};
    }

    constexpr auto operator-(const hera::bounded_sentinel&) const noexcept
    {
        return std::integral_constant<difference_type, I - bound>{};
    }

    friend constexpr auto operator-(const hera::bounded_sentinel&,
                                    const tuple_iterator&) noexcept
    {
        return std::integral_constant<difference_type, bound - I>{};
    }

    template<typename D       = Tuple,
             std::ptrdiff_t N = I> // clang-format off
        requires (N >= 0) &&
            requires(D& d)
            {
                hera::get<N>(d); // will sfinae if out of range
            } // clang-format on
    constexpr decltype(auto) operator*() const
        noexcept(noexcept(hera::get<I>(tuple_)))
    {
        return hera::get<I>(tuple_);
    }

    template<hera::constant_convertible_to<difference_type> C>
    constexpr decltype(auto) operator[](C) const noexcept
    {
        return hera::get<I + C::value>(tuple_);
    }
};

// takes anything that acts or smells like a tuple and makes it workable with
// heterogeneous ranges.
template<typename Tuple>
class tuple_view : public hera::view_interface<tuple_view<Tuple>> {
    using size_type = std::size_t;

private:
    Tuple& tuple_;

public:
    explicit constexpr tuple_view(Tuple& tup) noexcept : tuple_{tup}
    {}

    constexpr auto begin() const noexcept
    {
        return tuple_iterator<Tuple, 0>{tuple_};
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