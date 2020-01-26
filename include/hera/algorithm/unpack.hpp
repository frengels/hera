#pragma once

#include <type_traits>

#include "hera/get.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"

namespace hera
{
namespace unpack_impl
{
struct fn
{
private:
    template<std::size_t... Is, hera::bounded_range R, typename F>
    static constexpr decltype(auto)
    impl(std::index_sequence<Is...>, R&& r, F&& fn) noexcept
    {
        return std::forward<F>(fn)(hera::get<Is>(std::forward<R>(r))...);
    }

public:
    template<hera::bounded_range R, typename F>
    constexpr decltype(auto) operator()(R&& r, F&& fn) const noexcept
    {
        constexpr std::size_t sz  = hera::size(r);
        constexpr auto        seq = std::make_index_sequence<sz>{};
        return impl(seq, std::forward<R>(r), std::forward<F>(fn));
    }
};
} // namespace unpack_impl

inline namespace cpo
{
inline constexpr auto unpack = hera::unpack_impl::fn{};
}
} // namespace hera