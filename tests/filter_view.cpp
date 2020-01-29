#include <catch2/catch.hpp>

#include "hera/container/tuple.hpp"
#include "hera/view/array.hpp"
#include "hera/view/filter.hpp"
#include "hera/view/iota.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("filter_view")
{
    auto tup      = std::make_tuple(1, 2.0f, 3, 4.0f, 5, 6.0f);
    auto tup_view = hera::tuple_view{tup};

    auto int_predicate = [](auto x) {
        return std::bool_constant<hera::same_as<decltype(x), int>>{};
    };

    auto filt = hera::filter_view{tup_view, int_predicate};

    REQUIRE(hera::get<0>(filt) == 1);
    REQUIRE(hera::get<1>(filt) == 3);
    REQUIRE(hera::get<2>(filt) == 5);

    REQUIRE(hera::size(filt) == 3);

    SECTION("unbounded")
    {
        auto v = hera::iota_view<0>{};

        // filter even values
        auto iota_filt = hera::filter_view{
            v, [](auto x) {
                return std::bool_constant<(decltype(x)::value % 2) == 0>{};
            }};

        static_assert(hera::unbounded_range<decltype(iota_filt)>);
        static_assert(std::is_empty_v<decltype(iota_filt)>);

        static_assert(decltype(hera::get<0>(iota_filt))::value == 0);
        static_assert(decltype(hera::get<1>(iota_filt))::value == 2);
    }

    SECTION("pipe")
    {
        int arr[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto arr_view = hera::array_view{arr};

        // nothing is float so empty filter_view as a result
        auto filt_arr =
            arr_view | hera::views::filter([](auto x) {
                return std::bool_constant<hera::same_as<float, decltype(x)>>{};
            });

        static_assert(decltype(hera::size(filt_arr))::value == 0);
    }

    SECTION("single_element")
    {
        auto tup  = std::tuple{"hello", 50, 49, 32};
        auto tupv = hera::tuple_view(tup);

        auto single_match = hera::filter_view{
            tupv, [](auto x) {
                return std::bool_constant<
                    hera::same_as<const char*, decltype(x)>>{};
            }};

        static_assert(decltype(hera::size(single_match))::value == 1);

        auto no_match =
            hera::filter_view{tupv, [](auto) { return hera::false_; }};

        static_assert(decltype(hera::size(no_match))::value == 0);

        SECTION("match_all")
        {
            auto all_match =
                hera::filter_view{tupv, [](auto) { return std::true_type{}; }};

            static_assert(decltype(hera::size(all_match))::value == 4);
        }
    }
}