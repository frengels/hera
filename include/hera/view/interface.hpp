#pragma once

#include <type_traits>

#include "hera/at.hpp"
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
        static_assert(hera::derived_from<D, view_interface>);
        return static_cast<D&>(*this);
    }

    constexpr const D& derived() const noexcept
    {
        static_assert(hera::derived_from<D, view_interface>);
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
        return hera::at<0>(derived());
    }

    constexpr decltype(auto) front() const
        noexcept requires !hera::empty_range<D>
    {
        return hera::at<0>(derived());
    }

    constexpr decltype(auto) back() noexcept requires !hera::empty_range<D> &&
        hera::bounded_range<D>
    {
        auto                  size_integral = hera::size(derived());
        constexpr std::size_t size          = size_integral;
        constexpr auto        last_index    = size - 1;
        return hera::at<last_index>(derived());
    }

    constexpr decltype(auto) back() const
        noexcept requires !hera::empty_range<D> &&
        hera::bounded_range<D>
    {
        auto                  size_integral = hera::size(derived());
        constexpr std::size_t size          = size_integral;
        constexpr auto        last_index    = size - 1;
        return hera::at<last_index>(derived());
    }
};
} // namespace hera