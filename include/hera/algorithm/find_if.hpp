#pragma once

#include <functional>

#include "hera/constant.hpp"
#include "hera/metafunction.hpp"
#include "hera/optional.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"
#include "hera/type_identity.hpp"

namespace hera
{
namespace find_if_impl
{
struct fn
{
private:
    template<std::size_t I, hera::range R, typename Pred>
    static constexpr decltype(auto) check_at(R&&                     range,
                                             [[maybe_unused]] Pred&& pred)
    {
        if constexpr (hera::range_out_of_bounds<R, I>)
        {
            return hera::none<void>{};
        }
        else
        {
            using element_type = decltype(hera::at<I>(std::forward<R>(range)));
            using pred_result  = std::invoke_result_t<Pred, element_type>;

            if constexpr (pred_result::value)
            {
                return hera::just<std::integral_constant<std::size_t, I>>{
                    std::in_place};
            }
            else
            {
                return check_at<I + 1>(std::forward<R>(range),
                                       std::forward<Pred>(pred));
            }
        }
    }

public:
    template<hera::range R, typename Pred>
    constexpr decltype(auto) operator()(R&& range, Pred&& pred) const noexcept
    {
        return check_at<0>(std::forward<R>(range), std::forward<Pred>(pred));
    }
}; // namespace find_if_impl
} // namespace find_if_impl

inline namespace cpo
{
inline constexpr auto find_if = hera::find_if_impl::fn{};
}
} // namespace hera