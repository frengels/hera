#pragma once

#include <type_traits>

#include "hera/at.hpp"
#include "hera/concepts.hpp"
#include "hera/constant.hpp"
#include "hera/size.hpp"

namespace hera
{
namespace detail
{
template<typename R, std::ptrdiff_t I>
struct normal_iterator_value_type
{};

template<typename R, std::ptrdiff_t I> // clang-format off
    requires I < 0 
struct normal_iterator_value_type<R, I>
{}; // clang-format on

template<typename R, std::ptrdiff_t I> // clang-format off
    requires (I >= 0) &&
        requires(R& r)
        {
            hera::at(r, std::integral_constant<std::size_t, I>{});
        }
struct normal_iterator_value_type<R, I> // clang-format on
{
    using value_type = std::remove_cvref_t<
        std::invoke_result_t<decltype(hera::at),
                             R&,
                             std::integral_constant<std::size_t, I>>>;
};
} // namespace detail

template<typename R, std::ptrdiff_t I>
class normal_iterator : public detail::normal_iterator_value_type<R, I> {
private:
    template<typename R_, std::ptrdiff_t I_>
    friend class normal_iterator;

    using range_type = R;

    static constexpr std::ptrdiff_t index = I;
    static constexpr std::ptrdiff_t end_index =
        decltype(hera::size(std::declval<range_type&>()))::value;
    static constexpr bool out_of_bounds = (index < 0) || (index >= end_index);

public:
    using difference_type = std::ptrdiff_t;

private:
    range_type& range_;

public:
    constexpr normal_iterator(range_type& r) noexcept : range_{r}
    {}

    // construct from non const range_type
    template<typename UR> // clang-format off
        requires !same_as<R, UR> && same_as<std::remove_const_t<R>, UR> 
    constexpr normal_iterator( // clang-format on
            const normal_iterator<UR, index>& other) noexcept
        : range_{other.range_}
    {}

    template<std::ptrdiff_t J>
    constexpr auto operator==(const normal_iterator<R, J>&) const noexcept
    {
        return std::bool_constant<I == J>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator!=(const normal_iterator<R, J>&) const noexcept
    {
        return std::bool_constant<I != J>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator<(const normal_iterator<R, J>&) const noexcept
    {
        return std::bool_constant<(I < J)>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator>(const normal_iterator<R, J>&) const noexcept
    {
        return std::bool_constant<(I > J)>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator<=(const normal_iterator<R, J>&) const noexcept
    {
        return std::bool_constant<(I <= J)>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator>=(const normal_iterator<R, J>&) const noexcept
    {
        return std::bool_constant<(I >= J)>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator-(const normal_iterator<R, J>&) const noexcept
    {
        return std::integral_constant<difference_type, I - J>{};
    }

    template<hera::constant_convertible_to<difference_type> C>
    constexpr auto operator+(const C&) const noexcept
    {
        return normal_iterator<R, I + C::value>{range_};
    }

    template<hera::constant_convertible_to<difference_type> C>
    friend constexpr auto operator+(const C&,
                                    const normal_iterator& it) noexcept
    {
        return normal_iterator<R, I + C::value>{it.range_};
    }

    template<hera::constant_convertible_to<difference_type> C>
    constexpr auto operator-(const C&) const noexcept
    {
        return normal_iterator<R, I - C::value>{range_};
    }

    constexpr auto operator++() const noexcept
    {
        return normal_iterator<R, I + 1>{range_};
    }

    constexpr auto operator--() const noexcept
    {
        return normal_iterator<R, I - 1>{range_};
    }

    template<typename D = R> // clang-format off
        requires 
            requires(D& r)
            {
                hera::at(r, std::integral_constant<std::size_t, I>{});
            } // clang-format on
    constexpr decltype(auto) operator*() const
        noexcept(noexcept(hera::at(range_,
                                   std::integral_constant<std::size_t, I>{})))
    {
        return hera::at(range_, std::integral_constant<std::size_t, I>{});
    }

    template<hera::constant_convertible_to<std::ptrdiff_t> C>
    constexpr auto operator[](const C& constant) const
        noexcept(noexcept(*(*this + constant))) -> decltype(*(*this + constant))
    {
        *(*this + constant);
    }
};
} // namespace hera
