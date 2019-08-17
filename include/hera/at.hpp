#pragma once

#include <utility>

#include "hera/constant.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace at_impl
{
using detail::priority_tag;
using std::get;

template<typename T, std::size_t N, hera::integral_constant_for<std::size_t> C>
constexpr auto
    at(priority_tag<6>, T (&arr)[N], const C&) noexcept(noexcept(arr[C::value]))
        -> decltype(arr[C::value])
{
    return arr[C::value];
}

template<typename T, std::size_t N, hera::integral_constant_for<std::size_t> C>
constexpr auto at(priority_tag<6>,
                  T(&&arr)[N],
                  const C&) noexcept(noexcept(std::move(arr[C::value])))
    -> decltype(std::move(arr[C::value]))
{
    return std::move(arr[C::value]);
}

template<typename R, hera::integral_constant_for<std::size_t> C>
constexpr auto at(priority_tag<5>, R&& r, const C& constant) noexcept(noexcept(
    std::forward<R>(r)[constant])) -> decltype(std::forward<R>(r)[constant])
{
    return std::forward<R>(r)[constant];
}

template<typename R, hera::integral_constant_for<std::size_t> C>
constexpr auto at(priority_tag<4>, R&& r, const C& constant) noexcept(
    noexcept(std::forward<R>(r).at(constant)))
    -> decltype(std::forward<R>(r).at(constant))
{
    return std::forward<R>(r).at(constant);
}

template<typename R, hera::integral_constant_for<std::size_t> C>
void at(R&&, const C&) = delete;

template<typename R, hera::integral_constant_for<std::size_t> C>
constexpr auto at(priority_tag<3>, R&& r, const C& constant) noexcept(
    noexcept(at(std::forward<R>(r), constant)))
    -> decltype(at(std::forward<R>(r), constant))
{
    return at(std::forward<R>(r), constant);
}

template<typename R, hera::integral_constant_for<std::size_t> C>
constexpr auto at(priority_tag<2>, R&& r, const C&) noexcept(
    noexcept(std::forward<R>(r).template get<C::value>()))
    -> decltype(std::forward<R>(r).template get<C::value>())
{
    return std::forward<R>(r).template get<C::value>();
}

template<std::size_t I, typename R>
void get(R&&) = delete;

template<typename R, hera::integral_constant_for<std::size_t> C>
constexpr auto at(priority_tag<1>, R&& r, const C&) noexcept(
    noexcept(get<C::value>(std::forward<R>(r))))
    -> decltype(get<C::value>(std::forward<R>(r)))
{
    return get<C::value>(std::forward<R>(r));
}
} // namespace at_impl

inline namespace cpo
{
struct at_fn
{
    template<typename R, hera::integral_constant_for<std::size_t> C>
    constexpr auto operator()(R&& r, const C& constant) const
        noexcept(noexcept(::hera::at_impl::at(detail::priority_tag<6>{},
                                              std::forward<R>(r),
                                              constant)))
            -> decltype(::hera::at_impl::at(detail::priority_tag<6>{},
                                            std::forward<R>(r),
                                            constant))
    {
        return ::hera::at_impl::at(
            detail::priority_tag<6>{}, std::forward<R>(r), constant);
    }
};

constexpr auto at = at_fn{};
} // namespace cpo
} // namespace hera
