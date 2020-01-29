#pragma once

#include "hera/constant.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace get_impl
{
template<std::size_t, typename T>
void get(T&&) = delete;

template<std::size_t Idx>
struct fn
{
private:
    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<4>, R&& r)
        -> decltype(static_cast<R&&>(r).template get<Idx>())
    {
        return static_cast<R&&>(r).template get<Idx>();
    }

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<3>, R&& r)
        -> decltype(get<Idx>(static_cast<R&&>(r)))
    {
        return get<Idx>(static_cast<R&&>(r));
    }

public:
    template<typename R>
    constexpr auto operator()(R&& r) const
        -> decltype(impl(hera::detail::max_priority_tag, static_cast<R&&>(r)))
    {
        return impl(hera::detail::max_priority_tag, static_cast<R&&>(r));
    }
};
} // namespace get_impl

inline namespace cpo
{
template<std::size_t Idx>
inline constexpr auto get = hera::get_impl::fn<Idx>{};
}
} // namespace hera
