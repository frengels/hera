#pragma once

#include "hera/algorithm/accumulate.hpp"
#include "hera/container/tuple.hpp"
#include "hera/size.hpp"
#include "hera/view.hpp"
#include "hera/view/all.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<hera::view... Vs>
constexpr auto smallest_size()
{
    return decltype(
        hera::accumulate(std::declval<hera::tuple<Vs...>&>(),
                         hera::infinite_constant{},
                         [](auto curr_size, auto view) {
                             if constexpr (decltype(curr_size)::value <
                                           hera::size_v<decltype(view)>)
                             {
                                 return curr_size;
                             }
                             else
                             {
                                 return hera::size(view);
                             }
                         })){};
}
} // namespace detail

/// zips multiple ranges together, the size() will be of the shortest view
template<hera::range... Vs> // clang-format off
    requires (hera::view<Vs> && ...) && (sizeof...(Vs) >= 2)
class zip_view : public view_interface<zip_view<Vs...>> { // clang-format on
private:
    [[no_unique_address]] hera::tuple<Vs...> base_;

public:
    constexpr zip_view(Vs... base) noexcept(
        (std::is_nothrow_move_constructible_v<Vs> && ...))
        : base_{static_cast<Vs&&>(base)...}
    {}

    constexpr hera::tuple<Vs...> base() const
        noexcept(std::is_nothrow_copy_constructible_v<hera::tuple<Vs...>>)
    {
        return base_;
    }

    constexpr auto size() const noexcept
    {
        return detail::smallest_size<Vs...>();
    }

    template<std::size_t I> // clang-format off
        requires (I < decltype(detail::smallest_size<Vs...>())::value)
    constexpr decltype(auto) get() const // clang-format on
    {
        return hera::unpack(base_, [](const Vs&... views) {
            return hera::forward_as_tuple(hera::get<I>(views)...);
        });
    }
};

template<hera::range... Rs>
zip_view(Rs&&...)->zip_view<hera::all_view<Rs>...>;
} // namespace hera