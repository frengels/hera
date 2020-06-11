#pragma once

#include "hera/container/pair.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"
#include "hera/view.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<hera::range V> // clang-format off
    requires hera::view<V>
class enumerate_view : public hera::view_interface<enumerate_view<V>> // clang-format on
{
private:
    [[no_unique_address]] V base_;

public:
    explicit constexpr enumerate_view(V base) : base_(static_cast<V&&>(base_))
    {}

    constexpr V base() const
    {
        return base_;
    }

    constexpr auto size() const
    {
        return hera::size(base_);
    }

    template<std::size_t I>
    constexpr decltype(auto) get() const noexcept
        requires hera::range_reachable_at<V, I>
    {
        return hera::pair<std::integral_constant<std::size_t, I>,
                          decltype(hera::get<I>(base_))>{{},
                                                         hera::get<I>(base_)};
    }
};

namespace views
{
struct enumerate_fn : public detail::pipeable_interface<enumerate_fn>
{
    template<hera::range R> // clang-format off
        requires hera::viewable_range<R>
    constexpr auto operator()(R&& r) const // clang-format on
        -> decltype(hera::enumerate_view{static_cast<R&&>(r)})
    {
        return hera::enumerate_view{static_cast<R&&>(r)};
    }
};

inline constexpr auto enumerate = enumerate_fn{};
} // namespace views
} // namespace hera