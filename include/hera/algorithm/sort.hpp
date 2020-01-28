#pragma once

#include <array>
#include <variant>

#include "hera/algorithm/unpack.hpp"
#include "hera/container/integer_sequence.hpp"
#include "hera/container/type_list.hpp"
#include "hera/utility/detail/sort.hpp"
#include "hera/view/array.hpp"
#include "hera/view/reorder.hpp"

namespace hera
{
struct sort_fn
{
private:
    template<std::size_t I, typename T>
    struct unique_box
    {
        static constexpr auto index = I;
        using type                  = T;
    };

    template<hera::bounded_range R, typename Compare>
    static constexpr auto impl(R&& r, Compare comp) noexcept
    {
        constexpr std::size_t dist = decltype(hera::size(r))::value;

        return hera::unpack(hera::make_index_sequence<dist>{}, [&](auto... is) {
            // create a type list of all types within the given iterator range
            auto types = hera::unpack(r, [](auto&&... values) {
                return hera::type_list<decltype(values)...>{};
            });

            constexpr auto cx_types = decltype(types){};

            constexpr auto var_arr =
                hera::unpack(cx_types, [&](auto... idents) {
                    using variant_type = std::variant<
                        unique_box<decltype(is)::value,
                                   typename decltype(idents)::type>...>;
                    return std::array{variant_type{
                        std::in_place_index<decltype(is)::value>}...};
                });

            constexpr auto sorted_arr = hera::detail::sort_array(
                var_arr, [&](auto var_lhs, auto var_rhs) {
                    return std::visit(
                        [&](auto lhs) {
                            return std::visit(
                                [&](auto rhs) {
                                    return decltype(
                                        comp(std::declval<
                                                 typename std::remove_cvref_t<
                                                     decltype(lhs)>::type>(),
                                             std::declval<
                                                 typename std::remove_cvref_t<
                                                     decltype(rhs)>::type>()))::
                                        value;
                                },
                                var_rhs);
                        },
                        var_lhs);
                });

            // return the sorted sequence
            return static_cast<R&&>(r) |
                   hera::views::reorder(
                       hera::index_sequence<std::visit(
                           [](auto boxed_type) {
                               return decltype(boxed_type)::index;
                           },
                           sorted_arr[decltype(is)::value])...>{});
        });
    }

public:
    template<hera::bounded_range R, typename Compare>
    constexpr auto operator()(R&& r, Compare comp) const noexcept
    {
        return impl(static_cast<R&&>(r), std::move(comp));
    }
};

constexpr auto sort = sort_fn{};
} // namespace hera