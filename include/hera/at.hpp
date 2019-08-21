#pragma once

#include <utility>

#include "hera/constant.hpp"
#include "hera/size.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace at_impl
{
using detail::priority_tag;
using std::get;

// will try to detect out of bounds at compile time, we need to detect out of
// bounds rather than
template<typename R, std::size_t I>
concept detected_out_of_bounds = requires(R&& r)
{ // clang-format off
    { hera::size(std::forward<R>(r)) } -> hera::constant_greater_than<I>;
}; // clang-format on

template<typename R>
concept sized = requires(R&& r)
{
    hera::size(std::forward<R>(r));
};

template<typename R, std::size_t I>
concept index_in_bounds = sized<R>&& // clang-format off
    requires(R&& r)
    {
        { hera::size(std::forward<R>(r)) } -> hera::constant_greater_than<I>;
    }; // clang-format on

// can detect out of bounds errors on sized ranges, if not sized simply
// passes
template<typename R, std::size_t I>
concept index_probably_in_bounds = index_in_bounds<R, I> || !sized<R>;

template<typename R, hera::constant_convertible_to<std::size_t> C>
constexpr decltype(auto) at(priority_tag<4>, R&& r, const C& constant) noexcept(
    noexcept(std::forward<R>(r)[constant])) // clang-format off
        requires index_probably_in_bounds<R, C::value>&& 
            requires
            {
                std::forward<R>(r)[constant];
            } // clang-format on
{
    return std::forward<R>(r)[constant];
}

template<typename R, hera::constant_convertible_to<std::size_t> C>
constexpr decltype(auto) at(priority_tag<3>, R&& r, const C& constant) noexcept(
    noexcept(std::forward<R>(r).at(constant))) // clang-format off
        requires index_probably_in_bounds<R, C::value>&& 
            requires
        {
            std::forward<R>(r).at(constant);
        } // clang-format on
{
    return std::forward<R>(r).at(constant);
}

template<typename R, hera::constant_convertible_to<std::size_t> C>
constexpr decltype(auto) at(priority_tag<2>, R&& r, const C& constant) noexcept(
    noexcept(at(std::forward<R>(r),
                constant))) // clang-format off
        requires index_probably_in_bounds<R, C::value>&& 
            requires
        {
            at(std::forward<R>(r), constant);
        } // clang-format on
{
    return at(std::forward<R>(r), constant);
}
} // namespace at_impl

inline namespace cpo
{
struct at_fn
{
    template<typename R, hera::constant_convertible_to<std::size_t> C>
    constexpr decltype(auto) operator()(R&& r, const C& constant) const
        noexcept(noexcept(::hera::at_impl::at(detail::max_priority_tag,
                                              std::forward<R>(r),
                                              constant))) // clang-format off
            requires hera::at_impl::index_probably_in_bounds<R, C::value>&& 
                requires
                {
                    ::hera::at_impl::at(
                        detail::max_priority_tag, std::forward<R>(r), constant);
                } // clang-format on
    {
        return ::hera::at_impl::at(
            detail::max_priority_tag, std::forward<R>(r), constant);
    }
}; // namespace cpo

constexpr auto at = at_fn{};
} // namespace cpo
} // namespace hera
