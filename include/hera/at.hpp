#pragma once

#include <utility>

#include "hera/constant.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace try_at_impl
{
template<std::size_t, typename T>
void try_at(T&&) = delete;

template<std::size_t I>
struct fn
{
private:
    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<4>, R&& r) noexcept
        -> decltype(std::forward<R>(r).template try_at<I>())
    {
        return std::forward<R>(r).template try_at<I>();
    }

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<3>, R&& r) noexcept
        -> decltype(try_at<I>(std::forward<R>(r)))
    {
        return try_at<I>(std::forward<R>(r));
    }

public:
    template<typename R>
    constexpr auto operator()(R&& r) const noexcept
        -> decltype(impl(hera::detail::max_priority_tag, std::forward<R>(r)))
    {
        return impl(hera::detail::max_priority_tag, std::forward<R>(r));
    }
};
} // namespace try_at_impl

inline namespace cpo
{
template<std::size_t I>
inline constexpr auto try_at = hera::try_at_impl::fn<I>{};
}

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

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<2>, R&& r) noexcept
        -> decltype(*hera::try_at<Idx>(std::forward<R>(r)))
    {
        return *hera::try_at<Idx>(std::forward<R>(r));
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
