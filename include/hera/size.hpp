#pragma once

#include <tuple>
#include <utility>

#include "hera/constant.hpp"
#include "hera/utility/detail/decay_copy.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace size_impl
{
using detail::priority_tag;

template<typename T, std::size_t N>
constexpr auto size(priority_tag<4>,
                    T (&)[N]) noexcept(noexcept(std::extent<T[N]>{}))
    -> decltype(std::extent<T[N]>{})
{
    return std::extent<T[N]>{};
}

template<typename T, std::size_t N>
constexpr auto size(priority_tag<4>,
                    T(&&)[N]) noexcept(noexcept(std::extent<T[N]>{}))
    -> decltype(std::extent<T[N]>{})
{
    return std::extent<T[N]>{};
}

template<typename R>
constexpr auto size(priority_tag<3>, R&& r) noexcept(noexcept(
    std::integral_constant<std::size_t,
                           decltype(std::forward<R>(r).size())::value>{}))
    -> decltype(
        std::integral_constant<std::size_t,
                               decltype(std::forward<R>(r).size())::value>{})
{
    return std::integral_constant<std::size_t,
                                  decltype(std::forward<R>(r).size())::value>{};
}

template<typename T>
void size(T&&) = delete;

template<typename R>
constexpr auto size(priority_tag<2>, R&& r) noexcept(noexcept(
    std::integral_constant<std::size_t,
                           decltype(size(std::forward<R>(r)))::value>{}))
    -> decltype(
        std::integral_constant<std::size_t,
                               decltype(size(std::forward<R>(r)))::value>{})
{
    return std::integral_constant<std::size_t,
                                  decltype(size(std::forward<R>(r)))::value>{};
}

template<typename R>
constexpr auto
    size(priority_tag<1>,
         R&& r) noexcept(noexcept(std::tuple_size<std::remove_cvref_t<R>>{}))
        -> decltype(std::tuple_size<std::remove_cvref_t<R>>{})
{
    return std::tuple_size<std::remove_cvref_t<R>>{};
}
} // namespace size_impl

inline namespace cpo
{
struct size_fn
{
    template<typename R>
    constexpr auto operator()(R&& r) const
        noexcept(noexcept(::hera::size_impl::size(detail::max_priority_tag,
                                                  std::forward<R>(r))))
            -> decltype(::hera::size_impl::size(detail::max_priority_tag,
                                                std::forward<R>(r)))
    {
        return ::hera::size_impl::size(detail::max_priority_tag,
                                       std::forward<R>(r));
    }
};

constexpr auto size = size_fn{};
} // namespace cpo
} // namespace hera
