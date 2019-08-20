#pragma once

#include "hera/begin_end.hpp"
#include "hera/size.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace empty_impl
{
using hera::detail::priority_tag;

template<hera::constant_convertible_to<bool> C>
constexpr auto cond_bool(const C&) noexcept
{
    return std::bool_constant<C::value>{};
}

template<typename R>
constexpr auto
    empty(priority_tag<4>,
          R&& r) noexcept(noexcept(cond_bool(std::forward<R>(r).empty())))
        -> decltype(cond_bool(std::forward<R>(r).empty()))
{
    return cond_bool(std::forward<R>(r).empty());
}

template<typename R>
constexpr auto empty(priority_tag<3>, R&& r) noexcept(noexcept(
    std::bool_constant<decltype(hera::size(std::forward<R>(r)))::value == 0>{}))
    -> decltype(std::bool_constant<
                decltype(hera::size(std::forward<R>(r)))::value == 0>{})
{
    return std::bool_constant<decltype(hera::size(std::forward<R>(r)))::value ==
                              0>{};
}

template<typename R>
constexpr auto empty(priority_tag<2>, R&& r) noexcept(noexcept(
    std::bool_constant<decltype(hera::begin(std::forward<R>(r)) ==
                                hera::end(std::forward<R>(r)))::value>{}))
    -> decltype(
        std::bool_constant<decltype(hera::begin(std::forward<R>(r)) ==
                                    hera::end(std::forward<R>(r)))::value>{})
{
    return std::bool_constant<decltype(hera::begin(std::forward<R>(r)) ==
                                       hera::end(std::forward<R>(r)))::value>{};
}
} // namespace empty_impl

inline namespace cpo
{
struct empty_fn
{
    template<typename R>
    constexpr auto operator()(R&& r) const
        noexcept(noexcept(hera::empty_impl::empty(detail::max_priority_tag,
                                                  std::forward<R>(r))))
            -> decltype(hera::empty_impl::empty(detail::max_priority_tag,
                                                std::forward<R>(r)))
    {
        return hera::empty_impl::empty(detail::max_priority_tag,
                                       std::forward<R>(r));
    }
};

constexpr auto empty = empty_fn{};
} // namespace cpo
} // namespace hera