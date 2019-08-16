#pragma once

#include <tuple>

#include "hera/integral_constant.hpp"
#include "hera/utility/detail/decay_copy.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace size_impl
{
template<typename T, std::size_t N>
constexpr auto size(detail::priority_tag<4>,
                    T (&)[N]) noexcept(noexcept(std::extent<T[N]>{}))
    -> decltype(std::extent<T[N]>{})
{
    return std::extent<T[N]>{};
}

template<typename T, std::size_t N>
constexpr auto size(detail::priority_tag<4>,
                    T(&&)[N]) noexcept(noexcept(std::extent<T[N]>{}))
    -> decltype(std::extent<T[N]>{})
{
    return std::extent<T[N]>{};
}

template<typename R> // clang-format off
    requires requires(R&& r)
	{
	    { static_cast<R&&>(r).size() } ->
		hera::integral_constant_for<std::size_t>;
	}
                     // clang-format on
constexpr auto size(detail::priority_tag<3>, R&& r) noexcept(noexcept(
    std::integral_constant<std::size_t,
                           decltype(static_cast<R&&>(r).size())::value>{}))
{
    return std::integral_constant<std::size_t,
                                  decltype(
                                      static_cast<R&&>(r).size())::value>{};
}

template<typename T>
void size(T&&) = delete;

template<typename R> // clang-format off
    requires requires(R&& r)
	{
	    { size(static_cast<R&&>(r)) } ->
		hera::integral_constant_for<std::size_t>;
	}
                     // clang-format on
constexpr auto size(detail::priority_tag<2>, R&& r) noexcept(noexcept(
    std::integral_constant<std::size_t,
                           decltype(size(static_cast<R&&>(r)))::value>{}))
{
    return std::integral_constant<std::size_t,
                                  decltype(size(static_cast<R&&>(r)))::value>{};
}

template<typename R>
constexpr auto
    size(detail::priority_tag<1>,
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
                                                  static_cast<R&&>(r))))
            -> decltype(::hera::size_impl::size(detail::max_priority_tag,
                                                static_cast<R&&>(r)))
    {
        return ::hera::size_impl::size(detail::max_priority_tag,
                                       static_cast<R&&>(r));
    }
};

constexpr auto size = size_fn{};
} // namespace cpo
} // namespace hera
