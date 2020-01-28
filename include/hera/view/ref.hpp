#pragma once

#include "hera/element_type.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<typename R>
void FUN(R&);

template<typename R>
void FUN(R&&) = delete;
} // namespace detail

template<hera::range R> // clang-format off
    requires std::is_object_v<R>
class ref_view : public view_interface<ref_view<R>> // clang-format on
{
private:
    R* base_;

public:
    template<typename T> // clang-format off
        requires !hera::same_as<T, ref_view> && 
            hera::convertible_to<T, R&> &&
            requires {
                hera::detail::FUN<R>(std::declval<T>());
            }
    constexpr ref_view(T&& t) // clang-format on
        : base_{std::addressof(static_cast<R&>(std::forward<T>(t)))}
    {}

    template<std::size_t I>
    constexpr auto element_type() const
        noexcept(noexcept(hera::element_type<I>(*base_)))
            -> decltype(hera::element_type<I>(*base_))
    {
        hera::element_type<I>(*base_);
    }

    constexpr auto size() const noexcept(noexcept(hera::size(*base_)))
        -> decltype(hera::size(*base_))
    {
        return hera::size(*base_);
    }

    template<std::size_t I>
    constexpr auto try_get() const noexcept(noexcept(hera::try_get<I>(*base_)))
        -> decltype(hera::try_get<I>(*base_))
    {
        return hera::try_get<I>(*base_);
    }

    template<std::size_t I>
    constexpr auto get() const noexcept(noexcept(hera::try_get<I>(*base_)))
        -> decltype(hera::get<I>(*base_))
    {
        return hera::get<I>(*base_);
    }
};

template<typename R>
ref_view(R&)->ref_view<R>;

template<typename T>
inline constexpr bool enable_safe_range<ref_view<T>> = true;
} // namespace hera