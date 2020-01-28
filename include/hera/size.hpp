#pragma once

#include "hera/bound.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace size_impl
{
template<typename T>
void size(T&&) = delete;

struct fn
{
private:
    template<typename T> // clang-format off
        requires hera::bound<std::decay_t<T>>
    static constexpr auto check(T&& t) noexcept // clang-format on
        -> decltype(std::decay_t<T>(static_cast<T&&>(t)))
    {
        return std::decay_t<T>(static_cast<T&&>(t));
    }

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<4>, R&& r) noexcept
        -> decltype(check(static_cast<R&&>(r).size()))
    {
        return check(static_cast<R&&>(r).size());
    }

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<3>, R&& r) noexcept
        -> decltype(check(size(static_cast<R&&>(r))))
    {
        return check(size(static_cast<R&&>(r)));
    }

public:
    template<typename R>
    constexpr auto operator()(R&& r) const noexcept
        -> decltype(impl(hera::detail::max_priority_tag, static_cast<R&&>(r)))
    {
        return impl(hera::detail::max_priority_tag, static_cast<R&&>(r));
    }
};
} // namespace size_impl

inline namespace cpo
{
inline constexpr auto size = hera::size_impl::fn{};
} // namespace cpo

template<typename R>
inline constexpr auto size_v = decltype(hera::size(std::declval<R&>()))::value;

template<typename R>
concept sized = // clang-format off
    requires (const R& r)
    {
        hera::size(r);
    }; // clang-format off

template<typename R>
concept bounded_size = sized<R> && // clang-format off
    requires (const R& r)
    {
        { hera::size(r) } -> hera::bounded;
    }; // clang-format on

template<typename R>
concept unbounded_size = sized<R>&& // clang-format off
    requires (const R& r)
    {
        { hera::size(r) } -> hera::unbounded;
    }; // clang-format off
} // namespace hera
