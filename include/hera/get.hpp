#pragma once

#include <utility>

#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace get_impl
{
using hera::detail::priority_tag;

template<std::size_t I, typename T>
constexpr auto
    get(priority_tag<4>,
        T&& t) noexcept(noexcept(std::forward<T>(t).template get<I>()))
        -> decltype(std::forward<T>(t).template get<I>())
{
    return std::forward<T>(t).template get<I>();
}

template<std::size_t I, typename T>
constexpr auto get(priority_tag<3>,
                   T&& t) noexcept(noexcept(get<I>(std::forward<T>(t))))
    -> decltype(get<I>(std::forward<T>(t)))
{
    return get<I>(std::forward<T>(t));
}
} // namespace get_impl

inline namespace cpo
{
template<std::size_t I>
struct get_fn
{
    template<typename T> // clang-format off
        requires 
            (I < std::tuple_size_v<std::remove_cvref_t<T>>) 
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

template<std::size_t I>
constexpr auto get = get_fn<I>{};
} // namespace cpo
} // namespace hera