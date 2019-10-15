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
template<typename R>
void end(R&&) = delete;

struct fn
{
private:
    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<4>,
                               R& r) noexcept(noexcept(r.end()))
        -> decltype(r.end())
    {
        return r.end();
    }

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<3>, R&& r) noexcept(
        noexcept(end(std::forward<R>(r)))) -> decltype(end(std::forward<R>(r)))
    {
        return end(std::forward<R>(r));
    }

public:
    template<typename R>
    constexpr auto operator()(R&& r) const noexcept(
        noexcept(impl(hera::detail::max_priority_tag, std::forward<R>(r))))
        -> decltype(impl(hera::detail::max_priority_tag, std::forward<R>(r)))
    {
        return impl(hera::detail::max_priority_tag, std::forward<R>(r));
    }
};
} // namespace end_impl

inline namespace cpo
{
inline constexpr auto end = end_impl::fn{};
} // namespace cpo
} // namespace hera
