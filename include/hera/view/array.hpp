#pragma once

#include "hera/constant.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<typename T, std::size_t Extent>
class array_view : public view_interface<array_view<T, Extent>> {
public:
    using size_type = std::size_t;

private:
    T* ptr_;

public:
    constexpr array_view(T (&arr)[Extent]) noexcept : ptr_{arr + 0}
    {}

    template<hera::constant_convertible_to<size_type> Len>
    constexpr array_view(T* ptr, Len) noexcept : ptr_{ptr}
    {}

    template<hera::constant_convertible_to<size_type> Idx> // clang-format off
        requires (Idx::value < Extent) // clang-format on
        constexpr auto
        operator[](Idx) const noexcept -> decltype(ptr_[Idx::value])
    {
        return ptr_[Idx::value];
    }
};

template<typename T, hera::constant_convertible_to<std::size_t> Len>
array_view(T* ptr, Len)->array_view<T, Len::value>;
} // namespace hera