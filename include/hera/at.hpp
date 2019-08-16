#pragma once

#include "hera/integral_constant.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace at_impl
{
template<typename T, std::size_t N, hera::integral_constant_for<std::size_t> C>
constexpr auto at(detail::priority_tag<6>,
                  T (&arr)[N],
                  const C&) noexcept(noexcept(arr[C::value]))
    -> decltype(arr[C::value])
{
    return arr[C::value];
}

template<typename T, std::size_t N, hera::integral_constant_for<std::size_t> C>
constexpr auto at(detail::priority_tag<6>,
                  T(&&arr)[N],
                  const C&) noexcept(noexcept(static_cast<T&&>(arr[C::value])))
    -> decltype(static_cast<T&&>(arr[C::value]))
{
    return static_cast<T&&>((arr[C::value]));
}

template<typename R, hera::integral_constant_for<std::size_t> C>
constexpr auto at(detail::priority_tag<5>, R&& r, const C& constant) noexcept(
    noexcept(static_cast<R&&>(r)[constant]))
    -> decltype(static_cast<R&&>(r)[constant])
{
    return static_cast<R&&>(r)[constant];
}

template<typename R, hera::integral_constant_for<std::size_t> C>
constexpr auto at(detail::priority_tag<4>, R&& r, const C& constant) noexcept(
    noexcept(static_cast<R&&>(r).at(constant)))
    -> decltype(static_cast<R&&>(r).at(constant))
{
    return static_cast<R&&>(r).at(constant);
}

template<typename R, hera::integral_constant_for<std::size_t> C>
void at(R&&, const C&) = delete;

template<typename R, hera::integral_constant_for<std::size_t> C>
constexpr auto at(detail::priority_tag<3>, R&& r, const C& constant) noexcept(
    noexcept(at(static_cast<R&&>(r), constant)))
    -> decltype(at(static_cast<R&&>(r), constant))
{
    return at(static_cast<R&&>(r), constant);
}

template<typename R, hera::integral_constant_for<std::size_t> C>
constexpr auto at(detail::priority_tag<2>, R&& r, const C&) noexcept(
    noexcept(static_cast<R&&>(r).template get<C::value>()))
    -> decltype(static_cast<R&&>(r).template get<C::value>())
{
    return static_cast<R&&>(r).template get<C::value>();
}

template<std::size_t I, typename R>
void get(R&&) = delete;

template<typename R, hera::integral_constant_for<std::size_t> C>
constexpr auto at(detail::priority_tag<1>, R&& r, const C&) noexcept(
    noexcept(get<C::value>(static_cast<R&&>(r))))
    -> decltype(get<C::value>(static_cast<R&&>(r)))
{
    return get<C::value>(static_cast<R&&>(r));
}

template<typename R, hera::integral_constant_for<std::size_t> C>
constexpr auto at(detail::priority_tag<0>, R&& r, const C&) noexcept(
    noexcept(std::get<C::value>(static_cast<R&&>(r))))
    -> decltype(std::get<C::value>(static_cast<R&&>(r)))
{
    return std::get<C::value>(static_cast<R&&>(r));
}
} // namespace at_impl

inline namespace cpo
{
struct at_fn
{
    template<typename R, hera::integral_constant_for<std::size_t> C>
    constexpr auto operator()(R&& r, const C& constant) const
        noexcept(noexcept(::hera::at_impl::at(detail::priority_tag<6>{},
                                              static_cast<R&&>(r),
                                              constant)))
            -> decltype(::hera::at_impl::at(detail::priority_tag<6>{},
                                            static_cast<R&&>(r),
                                            constant))
    {
        return ::hera::at_impl::at(
            detail::priority_tag<6>{}, static_cast<R&&>(r), constant);
    }
};

constexpr auto at = at_fn{};
} // namespace cpo
} // namespace hera
