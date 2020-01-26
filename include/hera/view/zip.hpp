#pragma once

#include "hera/algorithm/accumulate.hpp"
#include "hera/algorithm/any_of.hpp"
#include "hera/algorithm/find_if.hpp"
#include "hera/algorithm/unpack.hpp"
#include "hera/container/tuple.hpp"
#include "hera/container/type_list.hpp"
#include "hera/view.hpp"
#include "hera/view/all.hpp"
#include "hera/view/enumerate.hpp"
#include "hera/view/filter.hpp"
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
    template<std::size_t I, typename Sz>
    constexpr auto find_smallest_base(Sz smallest) const noexcept
    {
        if constexpr (I < sizeof...(Vs))
        {
            auto curr_size = hera::size(hera::get<I>(base_));

            constexpr auto cmp = [](auto lhs, auto rhs) {
                if constexpr (hera::same_as<hera::infinite, decltype(lhs)>)
                {
                    return std::false_type{};
                }
                else if constexpr (hera::same_as<hera::infinite, decltype(rhs)>)
                {
                    return std::true_type{};
                }
                else
                {
                    return std::bool_constant<(decltype(lhs)::value <
                                               decltype(rhs)::value)>{};
                }
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
        return find_smallest_base<0>(hera::infinite{});
    }

    template<std::size_t I>
    constexpr auto try_get() const noexcept
    {
        if constexpr (decltype(hera::any_of(base_, [](auto view) {
                          return std::bool_constant<
                              hera::bounded_range<decltype(view)>>{};
                      }))::value)
        {
            // TODO somehow DRY this without type errors
            if constexpr (I < decltype(size())::value)
            {
                // in bounds
                return hera::unpack(base_, [](auto... views) {
                    return hera::just{
                        hera::forward_as_tuple(hera::get<I>(views)...)};
                });
            }
            else
            {
                // out of bounds
                return hera::none{};
            }
        }
        else
        {
            // always valid
            return hera::unpack(base_, [](auto... views) {
                return hera::just{
                    hera::forward_as_tuple(hera::get<I>(views)...)};
            });
        }
    }
};

template<hera::range... Rs>
zip_view(Rs&&...)->zip_view<hera::all_view<Rs>...>;
} // namespace hera