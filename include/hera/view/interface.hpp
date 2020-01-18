#pragma once

#include <type_traits>

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
    constexpr std::false_type empty() const
        noexcept requires hera::unbounded_size<D>
    {
        return {};
    }

    constexpr auto empty() const noexcept requires hera::bounded_size<D>
    {
        constexpr std::size_t sz = hera::size(derived());

        if constexpr (sz == 0)
        {
            return std::true_type{};
        }
        else
        {
            return std::false_type{};
        }
    }
};
} // namespace hera