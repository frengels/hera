#pragma once

#include <type_traits>
#include <utility>

#include "hera/get.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"

namespace hera
{
struct view_base
{};

template<typename D> // clang-format off
    requires std::is_class_v<D> && hera::same_as<D, std::remove_cv_t<D>>
class view_interface : public view_base // clang-format on
{
private:
    constexpr D& derived() noexcept
    {
        static_assert(hera::derived_from<D, view_interface<D>>);
        return static_cast<D&>(*this);
    }

    constexpr const D& derived() const noexcept
    {
        static_assert(hera::derived_from<D, view_interface<D>>);
        return static_cast<const D&>(*this);
    }

public:
    constexpr auto empty() const noexcept
    {
        if constexpr (hera::empty_range<decltype(derived())>)
        {
            return std::true_type{};
        }
        else
        {
            return std::false_type{};
        }
    }

    constexpr decltype(auto) front() noexcept requires !hera::empty_range<D>
    {
        return hera::get<0>(derived());
    }

    constexpr decltype(auto) front() const
        noexcept requires !hera::empty_range<D>
    {
        return hera::get<0>(derived());
    }

    constexpr decltype(auto) back() noexcept requires !hera::empty_range<D> &&
        hera::bounded_range<D>
    {

        return hera::get<(hera::size_v<D> - 1)>(derived());
    }

    constexpr decltype(auto) back() const
        noexcept requires !hera::empty_range<D> &&
        hera::bounded_range<D>
    {
        return hera::get<(hera::size_v<const D> - 1)>(derived());
    }
};
} // namespace hera