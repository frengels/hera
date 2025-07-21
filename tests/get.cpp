#include <catch2/catch_test_macros.hpp>

#include <cstring>

#include "hera/get.hpp"
#include "hera/view/array.hpp"
#include "hera/view/tuple.hpp"

struct ungettable
{};

TEST_CASE("get")
{

    auto tup      = std::make_tuple("hello", 42, 53);
    auto tup_view = hera::tuple_view{tup};
    int  arr[]{0, 1, 2, 3, 4, 5};
    auto arr_view = hera::array_view{arr};

    SECTION("tuple")
    {
        REQUIRE(hera::get<1>(tup_view) == 42);
        REQUIRE(hera::get<2>(tup_view) == 53);

        // back not currently available
        // REQUIRE(tup_view.back() == 53);
        REQUIRE(std::strcmp(tup_view.front(), "hello") == 0);

        static_assert(
            !std::is_invocable_v<decltype(hera::get<50>), decltype(tup_view)>);
    }

    SECTION("array")
    {
        REQUIRE(hera::get<0>(arr_view) == 0);
        REQUIRE(hera::get<5>(arr_view) == 5);
        REQUIRE(arr_view.back() == 5);

        static_assert(
            !std::is_invocable_v<decltype(hera::get<100>), decltype(arr_view)>);
    }

    SECTION("std_tuple")
    {
        static_assert(
            std::is_invocable_v<decltype(hera::get<0>), decltype((tup))>);

        // this is not a property I can achieve at the moment
        // because the program is ill formed on exceeded bounds (static_assert)
        // static_assert(
        //    !std::is_invocable_v<decltype(hera::get<200>), decltype((tup))>);
    }

    SECTION("ungettable")
    {
        static_assert(!std::is_invocable_v<decltype(hera::get<0>), ungettable>);
    }
}
