#pragma once

#include <utility>

#include "hera/iterator.hpp"
#include "hera/normal_iterator.hpp"
#include "hera/size.hpp"
#include "hera/utility/detail/decay_copy.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace begin_impl
{
using hera::detail::decay_copy;
using hera::detail::priority_tag;

// constraint is satisfied if the passed type is a heterogeneous iterator
template<typename I> // clang-format off
    requires hera::heterogeneous_iterator<std::remove_cvref_t<I>>
constexpr decltype(auto) // clang-format on
cond_het(I&& i) noexcept(noexcept(decay_copy(std::forward<I>(i))))
{
    return decay_copy(std::forward<I>(i));
}

template<typename R>
constexpr auto begin(priority_tag<4>,
                     R& r) noexcept(noexcept(cond_het(r.begin())))
    -> decltype(cond_het(r.begin()))
{
    return cond_het(r.begin());
}

template<typename R>
void begin(R&&) = delete;

template<typename R>
constexpr auto
    begin(priority_tag<3>,
          R&& r) noexcept(noexcept(cond_het(begin(std::forward<R>(r)))))
        -> decltype(cond_het(begin(std::forward<R>(r))))
{
    return cond_het(begin(std::forward<R>(r)));
}
} // namespace begin_impl

inline namespace cpo
{
// begin on a zero sized range will not be valid for the heterogeneous iterator,
// find a solution for that.
struct begin_fn
{
    template<typename R>
    constexpr auto operator()(R&& r) const noexcept(
        noexcept(::hera::begin_impl::begin(hera::detail::max_priority_tag,
                                           std::forward<R>(r))))
        -> decltype(::hera::begin_impl::begin(hera::detail::max_priority_tag,
                                              std::forward<R>(r)))
    {
        return ::hera::begin_impl::begin(hera::detail::max_priority_tag,
                                         std::forward<R>(r));
    }
};

constexpr auto begin = begin_fn{};
} // namespace cpo

namespace end_impl
{
using hera::detail::decay_copy;
using hera::detail::priority_tag;

template<typename R>
constexpr auto
    end(priority_tag<4>,
        R& r) noexcept(noexcept(decay_copy(std::forward<R>(r).end())))
        -> decltype(decay_copy(std::forward<R>(r).end()))
{
    return decay_copy(std::forward<R>(r).end());
}

template<typename R>
void end(R&&) = delete;

template<typename R>
constexpr auto
    end(priority_tag<3>,
        R&& r) noexcept(noexcept(decay_copy(end(std::forward<R>(r)))))
        -> decltype(decay_copy(end(std::forward<R>(r))))
{
    return decay_copy(end(std::forward<R>(r)));
}
} // namespace end_impl

inline namespace cpo
{
struct end_fn
{
    // past the end iterators have other requirements than regular iterators,
    // therefore they don't need to be a heterogeneous iterator.
    template<typename R>
    constexpr auto operator()(R&& r) const
        noexcept(noexcept(::hera::end_impl::end(hera::detail::max_priority_tag,
                                                std::forward<R>(r))))
            -> decltype(::hera::end_impl::end(hera::detail::max_priority_tag,
                                              std::forward<R>(r)))
    {
        return ::hera::end_impl::end(hera::detail::max_priority_tag,
                                     std::forward<R>(r));
    }
};

constexpr auto end = end_fn{};
} // namespace cpo
} // namespace hera
