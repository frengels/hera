#pragma once

#include "hera/iterator.hpp"
#include "hera/normal_iterator.hpp"
#include "hera/utility/detail/decay_copy.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace begin_impl
{
template<typename R>
constexpr auto begin(detail::priority_tag<4>, R&& r) noexcept(
    noexcept(detail::decay_copy(static_cast<R&&>(r).begin())))
    -> decltype(detail::decay_copy(static_cast<R&&>(r).begin()))
{
    return detail::decay_copy(static_cast<R&&>(r).begin());
}

template<typename R>
void begin(R&&) = delete;

template<typename R>
constexpr auto begin(detail::priority_tag<3>, R&& r) noexcept(
    noexcept(detail::decay_copy(begin(static_cast<R&&>(r)))))
    -> decltype(detail::decay_copy(begin(static_cast<R&&>(r))))
{
    return detail::decay_copy(begin(static_cast<R&&>(r)));
}

template<typename R>
constexpr auto begin(detail::priority_tag<2>,
                     R& r) noexcept(noexcept(hera::normal_iterator<R, 0>{r}))
    -> decltype(hera::normal_iterator<R, 0>{r})
{
    return hera::normal_iterator<R, 0>{r};
}
} // namespace begin_impl

inline namespace cpo
{
struct begin_fn
{
    template<typename R>
    constexpr auto operator()(R&& r) const noexcept(
        noexcept(::hera::begin_impl::begin(hera::detail::max_priority_tag,
                                           static_cast<R&&>(r))))
        -> decltype(::hera::begin_impl::begin(hera::detail::max_priority_tag,
                                              static_cast<R&&>(r)))
    {
        return ::hera::begin_impl::begin(hera::detail::max_priority_tag,
                                         static_cast<R&&>(r));
    }
};

constexpr auto begin = begin_fn{};
} // namespace cpo
} // namespace hera
