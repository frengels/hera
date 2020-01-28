#pragma once

#include "hera/algorithm/find_if.hpp"
#include "hera/ranges.hpp"
#include "hera/type_identity.hpp"

namespace hera
{
namespace any_of_impl
{
struct fn
{
public:
    template<hera::range R, typename Pred>
    constexpr auto operator()(R&& range, Pred p) const noexcept
    {
        // this will find the first occurence
        auto res = hera::find_if(static_cast<R&&>(range), std::move(p));

        return res.transform([](auto&&) { return std::true_type{}; })
            .value_or(std::false_type{});
    }
};
} // namespace any_of_impl

inline constexpr auto any_of = any_of_impl::fn{};
} // namespace hera