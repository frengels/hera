#pragma once

#include <utility>

#include "hera/iterator/concepts.hpp"
#include "hera/size.hpp"
#include "hera/utility/detail/decay_copy.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace begin_impl
{
template<typename R>
void begin(R&&) = delete;

struct fn
{
private:
    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<4>,
                               R& r) noexcept(noexcept(r.begin()))
        -> decltype(r.begin())
    {
        return r.begin();
    }

    template<typename R>
    static constexpr auto
    impl(hera::detail::priority_tag<3>,
         R&& r) noexcept(noexcept(begin(std::forward<R>(r))))
        -> decltype(begin(std::forward<R>(r)))
    {
        return begin(std::forward<R>(r));
    }

public:
    template<typename R>
    constexpr auto operator()(R&& r) const
        noexcept(noexcept(decay_copy(impl(hera::detail::max_priority_tag,
                                          std::forward<R>(r)))))
            -> decltype(decay_copy(impl(hera::detail::max_priority_tag,
                                        std::forward<R>(r))))
    {
        return decay_copy(
            impl(hera::detail::max_priority_tag, std::forward<R>(r)));
    }
};
} // namespace begin_impl

inline namespace cpo
{
inline constexpr auto begin = begin_impl::fn{};
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
