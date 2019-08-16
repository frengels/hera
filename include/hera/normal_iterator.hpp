#pragma once

#include <type_traits>

#include "hera/integral_constant.hpp"
#include "hera/same_as.hpp"
#include "hera/size.hpp"

namespace hera
{
template<typename R, std::ptrdiff_t I>
class normal_iterator {
private:
    template<typename R_, std::ptrdiff_t I_>
    friend class normal_iterator;

    using range_type = R;

    static constexpr std::ptrdiff_t index = I;
    static constexpr std::ptrdiff_t out_of_range_index =
        decltype(hera::size(std::declval<range_type&>()))::value;

public:
    using difference_type = std::ptrdiff_t;

private:
    range_type* const range_{nullptr};

public:
    normal_iterator() = default;

    constexpr normal_iterator(range_type& r) noexcept
        : range_{std::addressof(r)}
    {}

    // construct from non const range_type
    template<typename UR> // clang-format off
        requires !same_as<R, UR> && same_as<std::remove_const_t<R>, UR> 
    constexpr normal_iterator( // clang-format on
            const normal_iterator<UR, index>& other) noexcept
        : range_{other.range_}
    {}

    template<std::ptrdiff_t J>
    constexpr auto operator==(const iterator<R, J>&) const noexcept
    {
        return std::bool_constant<I == J>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator!=(const iterator<R, J>&) const noexcept
    {
        return std::bool_constant<I != J>;
    }

    template<std::ptrdiff_t J>
    constexpr auto operator<(const iterator<R, J>&) const noexcept
    {
        return std::bool_constant<(I < J)>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator>(const iterator<R, J>&) const noexcept
    {
        return std::bool_constant<(I > J)>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator<=(const iterator<R, J>&) const noexcept
    {
        return std::bool_constant<(I <= J)>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator>=(const iterator<R, J>&) const noexcept
    {
        return std::bool_constant<(I >= J)>{};
    }

    template<std::ptrdiff_t J>
    constexpr auto operator-(const iterator<R, J>&) const noexcept
    {
        return std::integral_constant<difference_type, I - J>{};
    }

    template<hera::integral_constant_for<difference_type> C>
    constexpr auto operator+(const C&) const noexcept
    {
        return normal_iterator<R, I + C::value>{*range_};
    }

    template<hera::integral_constant_for<difference_type> C>
    constexpr auto operator-(const C&) const noexcept
    {
        return normal_iterator<R, I - C::value>{*range_};
    }

    constexpr decltype(auto) operator*() const noexcept(noexcept(hera::at(
        *range_,
        std::integral_constant<std::ptrdiff_t,
                               I>{}))) requires(index < out_of_range_index) &&
        (index >= 0)
    {
        return hera::at(*range_, std::integral_constant<std::ptrdiff_t, I>{});
    }

    constexpr decltype(auto) operator-> () const noexcept(
        noexcept(std::addressof(**this))) requires requires( // clang-format off
	    const normal_iterator<R, I>& i) { *i; } // clang-format on
    {
        return **this;
    }

    template<hera::integral_constant_for<std::ptrdiff_t> Integral>
    constexpr auto operator[](const Integral& integral) const
        noexcept(noexcept(*(*this + integral))) -> decltype(*(*this + integral))
    {
        *(*this + integral);
    }
};
} // namespace hera
