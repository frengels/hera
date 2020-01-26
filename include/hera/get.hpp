#pragma once

#include <utility>

#include "hera/constant.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace try_get_impl
{
template<std::size_t, typename T>
void try_get(T&&) = delete;

template<std::size_t I>
struct fn
{
private:
    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<4>, R&& r) noexcept
        -> decltype(std::forward<R>(r).template try_get<I>())
    {
        return std::forward<R>(r).template try_get<I>();
    }

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<3>, R&& r) noexcept
        -> decltype(try_get<I>(std::forward<R>(r)))
    {
        return try_get<I>(std::forward<R>(r));
    }

public:
    template<typename R>
    constexpr auto operator()(R&& r) const noexcept
        -> decltype(impl(hera::detail::max_priority_tag, std::forward<R>(r)))
    {
        return impl(hera::detail::max_priority_tag, std::forward<R>(r));
    }
};
} // namespace try_get_impl

inline namespace cpo
{
template<std::size_t I>
inline constexpr auto try_get = hera::try_get_impl::fn<I>{};
}

namespace get_impl
{
template<std::size_t, typename T>
void get(T&&) = delete;

template<std::size_t Idx>
struct fn
{
private:
    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<4>, R&& r) noexcept
        -> decltype(std::forward<R>(r).template get<Idx>())
    {
        return std::forward<R>(r).template get<Idx>();
    }

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<3>, R&& r) noexcept
        -> decltype(get<Idx>(std::forward<R>(r)))
    {
        return get<Idx>(std::forward<R>(r));
    }

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<2>, R&& r) noexcept
        -> decltype(*hera::try_get<Idx>(std::forward<R>(r)))
    {
        return *hera::try_get<Idx>(std::forward<R>(r));
    }

public:
    template<typename R>
    constexpr auto operator()(R&& r) const noexcept
        -> decltype(impl(hera::detail::max_priority_tag, std::forward<R>(r)))
    {
        return impl(hera::detail::max_priority_tag, std::forward<R>(r));
    }
};
} // namespace get_impl

inline namespace cpo
{
template<std::size_t Idx>
inline constexpr auto get = hera::get_impl::fn<Idx>{};
}
} // namespace hera
