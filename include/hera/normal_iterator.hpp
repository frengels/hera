#pragma once

#include <type_traits>

#include "hera/at.hpp"
#include "hera/concepts.hpp"
#include "hera/constant.hpp"
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
        return normal_iterator<R, I + C::value>{*range_};
    }

    template<hera::constant_convertible_to<difference_type> C>
    constexpr auto operator-(const C&) const noexcept
    {
        return normal_iterator<R, I - C::value>{*range_};
    }

    constexpr auto operator++() const noexcept
    {
        return normal_iterator<R, I + 1>{*range_};
    }

    constexpr auto operator--() const noexcept
    {
        return normal_iterator<R, I - 1>{*range_};
    }

    constexpr decltype(auto) operator*() const noexcept(noexcept(hera::at(
        *range_,
        std::integral_constant<std::ptrdiff_t,
                               I>{}))) requires((index < out_of_range_index) &&
                                                (index >= 0))
    {
        return hera::at(*range_, std::integral_constant<std::size_t, I>{});
    }

    constexpr decltype(auto) operator-> () const
        noexcept(noexcept(std::addressof(**this))) requires(
            (index < out_of_range_index) && (index >= 0))
    {
        return std::addressof(**this);
    }

    template<hera::constant_convertible_to<std::ptrdiff_t> C>
    constexpr auto operator[](const C& constant) const
        noexcept(noexcept(*(*this + constant))) -> decltype(*(*this + constant))
    {
        *(*this + constant);
    }
};
} // namespace hera
