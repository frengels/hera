#pragma once

#include <utility>

#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace iter_move_impl
{
using detail::priority_tag;

template<typename I>
constexpr auto
    iter_move(priority_tag<4>,
              I&& i) noexcept(noexcept(iter_move(std::forward<I>(i))))
        -> decltype(iter_move(std::forward<I>(i)))
{
    return iter_move(std::forward<I>(i));
}

template<typename I> // clang-format off
    requires requires(I&& i)
    {
        *std::forward<I>(i); // can return void according to draft
    } // clang-format on
constexpr decltype(auto)
    iter_move(priority_tag<3>, I&& i) noexcept(noexcept(*std::forward<I>(i)))
{
    if constexpr (std::is_lvalue_reference_v<decltype(*std::forward<I>(i))>)
    {
        return std::move(*std::forward<I>(i));
    }
    else
    {
        return *std::forward<I>(i);
    }
    // should still sfinae if op* is not present
}
} // namespace iter_move_impl

inline namespace cpo
{
struct iter_move_fn
{
    template<typename I>
    constexpr auto operator()(I&& i) const noexcept(
        noexcept(::hera::iter_move_impl::iter_move(detail::max_priority_tag,
                                                   std::forward<I>(i))))
        -> decltype(::hera::iter_move_impl::iter_move(detail::max_priority_tag,
                                                      std::forward<I>(i)))
    {
        return ::hera::iter_move_impl::iter_move(detail::max_priority_tag,
                                                 std::forward<I>(i));
    }
};

constexpr auto iter_move = iter_move_fn{};
} // namespace cpo
} // namespace hera