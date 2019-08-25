#pragma once

#include "hera/utility/detail/decay_copy.hpp"
#include "hera/utility/detail/priority_tag.hpp"
#include "hera/view.hpp"
#include "hera/view/ref.hpp"
#include "hera/view/subrange.hpp"

namespace hera
{
namespace views
{
// take inspiration from the implementation of cmcstl2
struct all_fn
{
private:
    enum : unsigned
    {
        throws   = 1,
        decay    = 1 << 1,
        ref      = 2 << 1,
        subrange = 3 << 1
    };

    template<hera::viewable_range R>
    static constexpr unsigned choose() noexcept
    {
        if constexpr (hera::view<std::remove_cvref_t<R>>)
        {
            return decay |
                   std::is_nothrow_constructible_v<std::remove_cvref_t<R>, R>;
        }
        else if constexpr (std::is_lvalue_reference_v<R>)
        {
            return ref | noexcept(hera::ref_view{std::declval<R>()});
        }
        else
        {
            return subrange | noexcept(hera::subrange(std::declval<R>()));
        }
    }

public:
    template<hera::viewable_range R, unsigned Choice = choose<R>()>
    constexpr auto operator()(R&& r) const noexcept(Choice& throws)
    {
        constexpr auto strategy = Choice & ~throws;
        if constexpr (strategy == decay)
        {
            // auto return type does decay_copy
            return std::forward<R>(r);
        }
        else if constexpr (strategy == ref)
        {
            return hera::ref_view{r};
        }
        else
        {
            return hera::subrange{std::forward<R>(r)};
        }
    }
};

constexpr auto all = all_fn{};
} // namespace views

template<viewable_range R>
using all_view = decltype(views::all(std::declval<R>()));
} // namespace hera