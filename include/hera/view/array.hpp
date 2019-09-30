#pragma once

#include <array>

#include "hera/constant.hpp"
#include "hera/iterator/sentinel.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<typename T, std::size_t Extent, std::ptrdiff_t I>
class array_iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type      = T;

private:
    T* const ptr_;

public:
    array_iterator(T* ptr) noexcept : ptr_{ptr}
    {}

    template<difference_type J>
    constexpr auto operator==(const array_iterator<T, Extent, J>&) const
        noexcept
    {
        return std::bool_constant<I == J>{};
    }

    constexpr auto operator==(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<I == Extent>{};
    }

    friend constexpr auto operator==(const hera::bounded_sentinel&,
                                     const array_iterator&) noexcept
    {
        return std::bool_constant<Extent == I>{};
    }

    template<difference_type J>
    constexpr auto operator!=(const array_iterator<T, Extent, J>&) const
        noexcept
    {
        return std::bool_constant<I != J>{};
    }

    constexpr auto operator!=(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<I != Extent>{};
    }

    friend constexpr auto operator!=(const hera::bounded_sentinel&,
                                     const array_iterator&) noexcept
    {
        return std::bool_constant<Extent != I>{};
    }

    template<difference_type J>
    constexpr auto operator<(const array_iterator<T, Extent, J>&) const noexcept
    {
        return std::bool_constant<(I < J)>{};
    }

    constexpr auto operator<(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<(I < Extent)>{};
    }

    friend constexpr auto operator<(const hera::bounded_sentinel&,
                                    const array_iterator&) noexcept
    {
        return std::bool_constant<(Extent < I)>{};
    }

    template<difference_type J>
    constexpr auto operator>(const array_iterator<T, Extent, J>&) const noexcept
    {
        return std::bool_constant<(I > J)>{};
    }

    constexpr auto operator>(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<(I > Extent)>{};
    }

    friend constexpr auto operator>(const hera::bounded_sentinel&,
                                    const array_iterator&) noexcept
    {
        return std::bool_constant<(Extent > I)>{};
    }

    template<difference_type J>
    constexpr auto operator<=(const array_iterator<T, Extent, J>&) const
        noexcept
    {
        return std::bool_constant<(I <= J)>{};
    }

    constexpr auto operator<=(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<(I <= Extent)>{};
    }

    friend constexpr auto operator<=(const hera::bounded_sentinel&,
                                     const array_iterator&) noexcept
    {
        return std::bool_constant<(Extent <= I)>{};
    }

    template<difference_type J>
    constexpr auto operator>=(const array_iterator<T, Extent, J>&) const
        noexcept
    {
        return std::bool_constant<(I >= J)>{};
    }

    constexpr auto operator>=(const hera::bounded_sentinel&) const noexcept
    {
        return std::bool_constant<(I >= Extent)>{};
    }

    friend constexpr auto operator>=(const hera::bounded_sentinel&,
                                     const array_iterator&) noexcept
    {
        return std::bool_constant<(Extent >= I)>{};
    }

    constexpr auto operator++() const noexcept
    {
        return array_iterator<T, Extent, I + 1>{ptr_};
    }

    constexpr auto operator--() const noexcept
    {
        return array_iterator<T, Extent, I - 1>{ptr_};
    }

    template<hera::constant_convertible_to<difference_type> C>
    constexpr auto operator+(C) const noexcept
    {
        return array_iterator<T, Extent, I + C::value>{ptr_};
    }

    template<hera::constant_convertible_to<difference_type> C>
    friend constexpr auto operator+(C n, const array_iterator& it) noexcept
    {
        return it + n;
    }

    template<hera::constant_convertible_to<difference_type> C>
    constexpr auto operator-(C) const noexcept
    {
        return array_iterator<T, Extent, I - C::value>{ptr_};
    }

    template<difference_type J>
    constexpr auto operator-(const array_iterator<T, Extent, J>&) const noexcept
    {
        return std::integral_constant<difference_type, I - J>{};
    }

    constexpr auto operator-(const hera::bounded_sentinel&) const noexcept
    {
        return std::integral_constant<difference_type, I - Extent>{};
    }

    friend constexpr auto operator-(const hera::bounded_sentinel&,
                                    const array_iterator&) noexcept
    {
        return std::integral_constant<difference_type, Extent - I>{};
    }

    template<difference_type I_  = I,
             std::size_t Extent_ = Extent> // clang-format off
        requires (I_ >= 0) && (I_ < Extent_) // clang-format on
        constexpr decltype(auto)
        operator*() const noexcept
    {
        return ptr_[I];
    }

    template<hera::constant_convertible_to<difference_type> C>
    constexpr auto operator[](C n) const noexcept(noexcept(*((*this) + n)))
        -> decltype(*((*this) + n))
    {
        return *((*this) + n);
    }

    constexpr decltype(auto) operator[](difference_type idx) const noexcept
    {
        return ptr_[I + idx];
    }
};

template<typename T, std::size_t Extent>
class array_view : public view_interface<array_view<T, Extent>> {
public:
    using size_type = std::size_t;

private:
    T* ptr_;

public:
    constexpr array_view(T (&arr)[Extent]) noexcept : ptr_{arr + 0}
    {}

    constexpr array_view(std::array<T, Extent>& arr) noexcept : ptr_{arr.data()}
    {}

    constexpr array_view(const std::array<T, Extent>& arr) noexcept
        : ptr_{arr.data()}
    {}

    template<hera::constant_convertible_to<size_type> Len>
    constexpr array_view(T* ptr, Len) noexcept : ptr_{ptr}
    {}

    template<typename Container,
             hera::constant_convertible_to<size_type> Len> // clang-format off
        requires 
            requires (Container& c)
            {
                c.data();
            } // clang-format on
    constexpr array_view(Container& cont, Len l) noexcept
        : array_view{cont.data(), std::move(l)}
    {}

    constexpr auto begin() const noexcept
    {
        return array_iterator<T, Extent, 0>{ptr_};
    }

    constexpr auto end() const noexcept
    {
        return hera::bounded_sentinel{};
    }

    friend constexpr auto begin(array_view&& av) noexcept
    {
        return av.begin();
    }

    friend constexpr auto end(array_view&& av) noexcept
    {
        return av.end();
    }

    constexpr T* data() const noexcept
    {
        return ptr_;
    }

    template<hera::constant_convertible_to<size_type> Idx> // clang-format off
        requires (Idx::value < Extent) // clang-format on
        constexpr auto
        operator[](Idx) const noexcept -> decltype(ptr_[Idx::value])
    {
        return ptr_[Idx::value];
    }

    constexpr decltype(auto) operator[](size_type idx) const noexcept
    {
        return ptr_[idx];
    }
};

template<typename T, std::size_t N>
array_view(const std::array<T, N>&)->array_view<const T, N>;

template<typename Container, hera::constant_convertible_to<std::size_t> Len>
array_view(Container&, Len)
    ->array_view<
        std::remove_pointer_t<decltype(std::declval<Container&>().data())>,
        Len::value>;

template<typename T, hera::constant_convertible_to<std::size_t> Len>
array_view(T* ptr, Len)->array_view<T, Len::value>;

namespace views
{
struct array_fn
{
    template<typename... Args>
    constexpr auto operator()(Args&&... args) const
        noexcept(noexcept(hera::array_view{std::forward<Args>(args)...}))
            -> decltype(hera::array_view{std::forward<Args>(args)...})
    {
        return hera::array_view{std::forward<Args>(args)...};
    }
};

constexpr auto array = array_fn{};
} // namespace views
} // namespace hera