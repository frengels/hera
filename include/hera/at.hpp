#pragma once

#include <utility>

#include "hera/constant.hpp"
#include "hera/size.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace at_impl
{
template<std::size_t, typename T>
void at(T&&) = delete;

template<std::size_t Idx>
struct fn
{
private:
    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<4>, R&& r) noexcept
        -> decltype(std::forward<R>(r).template at<Idx>())
    {
        return std::forward<R>(r).template at<Idx>();
    }

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<3>, R&& r) noexcept
        -> decltype(at<Idx>(std::forward<R>(r)))
    {
        return at<Idx>(std::forward<R>(r));
    }

public:
    template<typename R>
    constexpr auto operator()(R&& r) const noexcept
        -> decltype(impl(hera::detail::max_priority_tag, std::forward<R>(r)))
    {
        return impl(hera::detail::max_priority_tag, std::forward<R>(r));
    }
};
} // namespace at_impl

inline namespace cpo
{
template<std::size_t Idx>
inline constexpr auto at = hera::at_impl::fn<Idx>{};
}
} // namespace hera
