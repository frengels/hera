#pragma once

#include <tuple>

#include "hera/bound.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace detail
{
template<typename R>
concept static_consteval_bounded_tuple_size = // clang-format off
    requires
    {
        { R::size() } -> hera::bounded;
        //{ std::integral_constant<std::size_t, R::size()>{} }
        //    -> hera::bounded_constant;
    }; // clang-format on

template<typename R>
concept static_consteval_unbounded_tuple_size = // clang-format off
    requires
    {
        { R::size() } -> hera::unbounded;
        { hera::infinite_constant{} } -> hera::unbounded_constant;
    }; // clang-format on

template<typename R>
concept member_constant_tuple_size = // clang-format off
    requires (R&& r)
    {
        { static_cast<R&&>(r).size() }
            -> hera::bound_constant;
    }; // clang-format on

template<typename R>
concept adl_constant_tuple_size = // clang-format off
    requires (R&& r)
    {
        { size(static_cast<R&&>(r)) }
            -> hera::bound_constant;
    }; // clang-format on

template<typename R>
concept std_tuple_size = // clang-format off
    requires
    {
        std::tuple_size<R>{};
    }; // clang-format on
} // namespace detail

namespace size_impl
{
template<typename T>
void size(T&&) = delete;

struct fn
{
private:
    template<hera::detail::member_constant_tuple_size R>
    static constexpr auto impl(hera::detail::priority_tag<4>, R&& r)
    {
        return static_cast<R&&>(r).size();
    }

    template<hera::detail::adl_constant_tuple_size R>
    static constexpr auto impl(hera::detail::priority_tag<3>, R&& r)
    {
        return size(static_cast<R&&>(r));
    }

    template<hera::detail::static_consteval_bounded_tuple_size R>
    static constexpr auto impl(hera::detail::priority_tag<2>, const R&) noexcept
    {
        return std::integral_constant<std::size_t, R::size()>{};
    }

    template<hera::detail::static_consteval_unbounded_tuple_size R>
    static constexpr auto impl(hera::detail::priority_tag<2>, const R&) noexcept
    {
        return hera::infinite_constant{};
    }

    template<hera::detail::std_tuple_size R> // clang-format off
    static constexpr auto impl(hera::detail::priority_tag<1>, R&) noexcept // clang-format on
    {
        return std::tuple_size<R>{};
    }

public:
    template<typename R>
    constexpr auto operator()(R&& r) const
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

/// \brief `R` has a compile time defined size obtainable through `hera::size`
template<typename R>
concept constant_sized = // clang-format off
    requires (R& r)
    {
        hera::size(r);
    }; // clang-format off

template<typename R>
concept constant_sized_bounded = constant_sized<R> && // clang-format off
    requires (R& r)
    {
        { hera::size(r) } -> hera::bounded_constant;
    }; // clang-format on

template<typename R>
concept constant_sized_unbounded = constant_sized<R>&& // clang-format off
    requires (R& r)
    {
        { hera::size(r) } -> hera::unbounded_constant;
    }; // clang-format off
} // namespace hera
