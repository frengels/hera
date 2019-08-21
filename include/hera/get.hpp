#pragma once

#include <utility>

#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace get_impl
{
using hera::detail::priority_tag;

template<std::size_t I, typename T> // clang-format off
    requires (I < std::tuple_size_v<std::remove_cvref_t<T>>) &&
        requires(T&& t)
        {
            std::forward<T>(t).template get<I>();
        }
constexpr decltype(auto) // clang-format on
    get(priority_tag<4>,
        T&& t) noexcept(noexcept(std::forward<T>(t).template get<I>()))
{
    return std::forward<T>(t).template get<I>();
}

template<std::size_t I, typename T> // clang-format off
    requires (I < std::tuple_size_v<std::remove_cvref_t<T>>) &&
        requires(T&& t)
        {
            get<I>(std::forward<T>(t));
        } // clang-format on
constexpr decltype(auto)
    get(priority_tag<3>, T&& t) noexcept(noexcept(get<I>(std::forward<T>(t))))
{
    return get<I>(std::forward<T>(t));
}
} // namespace get_impl

inline namespace cpo
{
namespace impl
{
// we need to move this into its own concept because gcc doesn't play well with
// short circuiting logical or and and in concepts. For whatever reason in this
// form it's correctly short circuiting and doesn't evaluate this requirement.
template<typename T, std::size_t I>
concept has_get_impl = requires(T&& t)
{
    hera::get_impl::get<I>(hera::detail::max_priority_tag, std::forward<T>(t));
};
} // namespace impl

template<std::size_t I>
struct get_fn
{
    template<typename T> // clang-format off
    constexpr auto // clang-format on
    operator()(T&& t) const
        noexcept(noexcept(hera::get_impl::get<I>(detail::max_priority_tag,
                                                 std::forward<T>(t))))
            -> decltype(hera::get_impl::get<I>(detail::max_priority_tag,
                                               std::forward<T>(t)))
    {
        return hera::get_impl::get<I>(detail::max_priority_tag,
                                      std::forward<T>(t));
    }
};

// provides sfinae compatible get access to tuple like containers, compared to
// std::get which does not provide sfinae
template<std::size_t I>
constexpr auto get = get_fn<I>{};
} // namespace cpo
} // namespace hera