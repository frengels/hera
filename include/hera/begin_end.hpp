#pragma once

#include "hera/iterator.hpp"
#include "hera/normal_iterator.hpp"
#include "hera/size.hpp"
#include "hera/utility/detail/decay_copy.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace detail
{
template<typename I> // clang-format off
    requires hera::heterogeneous_iterator<std::remove_cvref_t<I>>
constexpr std::decay_t<I> // clang-format on
cond_het(I&& i) noexcept(noexcept(static_cast<I&&>(i)))
{
    return static_cast<I&&>(i);
}
} // namespace detail

namespace begin_impl
{
using hera::detail::cond_het;
using hera::detail::decay_copy;
using hera::detail::priority_tag;

template<typename R>
constexpr auto begin(priority_tag<4>, R&& r) noexcept(
    noexcept(decay_copy(cond_het(static_cast<R&&>(r).begin()))))
    -> decltype(decay_copy(cond_het(static_cast<R&&>(r).begin())))
{
    return decay_copy(cond_het(static_cast<R&&>(r).begin()));
}

template<typename R>
void begin(R&&) = delete;

template<typename R>
constexpr auto begin(priority_tag<3>, R&& r) noexcept(
    noexcept(decay_copy(cond_het(begin(static_cast<R&&>(r))))))
    -> decltype(decay_copy(cond_het(begin(static_cast<R&&>(r)))))
{
    return decay_copy(cond_het(begin(static_cast<R&&>(r))));
}

template<typename R>
constexpr auto begin(priority_tag<2>, R& r) noexcept(
    noexcept(decay_copy(cond_het(hera::normal_iterator<R, 0>{r}))))
    -> decltype(decay_copy(cond_het(hera::normal_iterator<R, 0>{r})))
{
    return decay_copy(cond_het(hera::normal_iterator<R, 0>{r}));
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

namespace end_impl
{
using hera::detail::cond_het;
using hera::detail::decay_copy;
using hera::detail::priority_tag;

template<typename R>
constexpr auto end(priority_tag<4>, R&& r) noexcept(
    noexcept(decay_copy(cond_het(static_cast<R&&>(r).end()))))
    -> decltype(decay_copy(cond_het(static_cast<R&&>(r).end())))
{
    return decay_copy(cond_het(static_cast<R&&>(r).end()));
}

template<typename R>
void end(R&&) = delete;

template<typename R>
constexpr auto end(priority_tag<3>, R&& r) noexcept(
    noexcept(decay_copy(cond_het(end(static_cast<R&&>(r))))))
    -> decltype(decay_copy(cond_het(end(static_cast<R&&>(r)))))
{
    return decay_copy(cond_het(end(static_cast<R&&>(r))));
}

template<typename R>
constexpr auto end(priority_tag<2>, R& r) noexcept(noexcept(decay_copy(
    cond_het(hera::normal_iterator<R, decltype(hera::size(r))::value>{r}))))
    -> decltype(decay_copy(
        cond_het(hera::normal_iterator<R, decltype(hera::size(r))::value>{r})))
{
    return decay_copy(
        cond_het(hera::normal_iterator<R, decltype(hera::size(r))::value>{r}));
}
} // namespace end_impl

inline namespace cpo
{
struct end_fn
{
    template<typename R>
    constexpr auto operator()(R&& r) const
        noexcept(noexcept(::hera::end_impl::end(hera::detail::max_priority_tag,
                                                static_cast<R&&>(r))))
            -> decltype(::hera::end_impl::end(hera::detail::max_priority_tag,
                                              static_cast<R&&>(r)))
    {
        return ::hera::end_impl::end(hera::detail::max_priority_tag,
                                     static_cast<R&&>(r));
    }
};

constexpr auto end = end_fn{};
} // namespace cpo
} // namespace hera
