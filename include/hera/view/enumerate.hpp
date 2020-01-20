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
    explicit constexpr enumerate_view(V base) : base_(std::move(base_))
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
    constexpr auto try_at() const noexcept
    {
        return hera::try_at<I>(base_).transform([](auto&& val) {
            return hera::pair<std::integral_constant<std::size_t, I>,
                              decltype(val)>{{},
                                             std::forward<decltype(val)>(val)};
        });
    }
};

namespace views
{
struct enumerate_fn : public detail::pipeable_interface<enumerate_fn>
{
    template<hera::range R> // clang-format off
        requires hera::viewable_range<R>
    constexpr auto operator()(R&& r) const // clang-format on
        -> decltype(hera::enumerate_view{std::forward<R>(r)})
    {
        return hera::enumerate_view{std::forward<R>(r)};
    }
};

inline constexpr auto enumerate = enumerate_fn{};
} // namespace views
} // namespace hera