#pragma once

#include <functional>

#include "hera/constant.hpp"
#include "hera/metafunction.hpp"
#include "hera/optional.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"
#include "hera/type.hpp"

namespace hera
{
namespace find_if_impl
{
struct fn
{
private:
    template<std::size_t I, hera::range R, typename Pred>
    static constexpr decltype(auto) check_get(R&&                     range,
                                              [[maybe_unused]] Pred&& pred)
    {
        if constexpr (hera::range_out_of_bounds<R, I>)
        {
            // return with integral_constant to not break transform, and_then
            // operations
            return hera::none{};
        }
        else
        {
            using element_type =
                decltype(hera::get<I>(static_cast<R&&>(range)));
            using pred_result = std::invoke_result_t<Pred, element_type>;

            if constexpr (pred_result::value)
            {
                return hera::just<std::integral_constant<std::size_t, I>>{
                    std::in_place};
            }
            else
            {
                return check_get<I + 1>(static_cast<R&&>(range),
                                        static_cast<Pred&&>(pred));
            }
        }
    }

public:
    template<hera::range R, typename Pred>
    constexpr decltype(auto) operator()(R&& range, Pred&& pred) const noexcept
    {
        return check_get<0>(static_cast<R&&>(range), static_cast<Pred&&>(pred));
    }
}; // namespace find_if_impl
} // namespace find_if_impl

inline namespace cpo
{
/// returns the index as just<integral_constant> or none<void> if nothing
/// matches the predicate.
inline constexpr auto find_if = hera::find_if_impl::fn{};
} // namespace cpo
} // namespace hera