#pragma once

#include "hera/algorithm/unpack.hpp"
#include "hera/container/tuple.hpp"
#include "hera/view.hpp"
#include "hera/view/all.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
/// zips multiple ranges together, the size() will be of the shortest view
template<hera::range... Vs> // clang-format off
    requires (hera::view<Vs> && ...) && (sizeof...(Vs) >= 2)
class zip_view : public view_interface<zip_view<Vs...>> { // clang-format on
private:
    [[no_unique_address]] hera::tuple<Vs...> base_;

public:
    constexpr zip_view(Vs... base) noexcept(
        (std::is_nothrow_move_constructible_v<Vs> && ...))
        : base_{std::move(base)...}
    {}

    constexpr hera::tuple<Vs...> base() const
        noexcept(std::is_nothrow_copy_constructible_v<hera::tuple<Vs...>>)
    {
        return base_;
    }

private:
    // recursively find the smallest base
    template<std::size_t I, typename Sz>
    constexpr auto find_smallest_base(Sz smallest) const noexcept
    {
        if constexpr (I < sizeof...(Vs))
        {
            auto curr_size = hera::size(hera::get<I>(base_));

            constexpr auto cmp = [](auto lhs, auto rhs) {
                constexpr auto lhs_val = decltype(lhs)::value;
                constexpr auto rhs_val = decltype(rhs)::value;

                return std::bool_constant<(lhs_val < rhs_val)>{};
            };

            if constexpr (decltype(cmp(curr_size, smallest))::value)
            {
                return find_smallest_base<I + 1>(curr_size);
            }
            else
            {
                return find_smallest_base<I + 1>(smallest);
            }
        }
        else
        {
            return smallest;
        }
    }

public:
    constexpr auto size() const noexcept
    {
        return find_smallest_base<0>(hera::infinite_constant{});
    }

    template<std::size_t I> // clang-format off
    constexpr decltype(auto) get() const // clang-format on
    {
        return hera::unpack(base_, [](const Vs&... views) {
            return hera::forward_as_tuple(hera::get<I>(views)...);
        });
    }
};

template<hera::range... Rs>
zip_view(Rs&&...)->zip_view<hera::all_view<Rs>...>;

namespace views
{
struct zip_with_fn
{
    template<hera::range... Rs>
    constexpr auto operator()(Rs&&... ranges) const
        -> decltype(hera::zip_view{static_cast<Rs&&>(ranges)...})
    {
        return hera::zip_view{static_cast<Rs&&>(ranges)...};
    }
};

inline constexpr auto zip_with = zip_with_fn{};
} // namespace views
} // namespace hera