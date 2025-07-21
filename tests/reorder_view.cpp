#include <catch2/catch_test_macros.hpp>

#include "hera/container/tuple.hpp"
#include "hera/view/reorder.hpp"

TEST_CASE("reordered_view")
{
    auto tup = hera::tuple{5, 4, 3, 2, 1, 0};

    auto reordered =
        hera::reorder_view{tup, hera::index_sequence<5, 4, 3, 2, 1, 0>{}};

    REQUIRE(hera::get<0>(reordered) == 0);
    REQUIRE(hera::get<1>(reordered) == 1);
    REQUIRE(hera::get<2>(reordered) == 2);
    REQUIRE(hera::get<3>(reordered) == 3);
    REQUIRE(hera::get<4>(reordered) == 4);
    REQUIRE(hera::get<5>(reordered) == 5);

    REQUIRE(hera::size(reordered) == 6);

    SECTION("partial")
    {
        auto partial_reorder =
            hera::reorder_view{tup, hera::index_sequence<1, 0>{}};

        REQUIRE(hera::get<0>(partial_reorder) == 4);
        REQUIRE(hera::get<1>(partial_reorder) == 5);

        REQUIRE(hera::size(partial_reorder) == 2);
    }

    SECTION("superset")
    {
        auto larger = tup | hera::views::reorder(
                                hera::index_sequence<0, 0, 0, 0, 0, 0, 5>{});

        REQUIRE(hera::size(larger) == 7);

        REQUIRE(hera::get<0>(larger) == 5);
        REQUIRE(hera::get<1>(larger) == 5);
        REQUIRE(hera::get<2>(larger) == 5);
        REQUIRE(hera::get<3>(larger) == 5);
        REQUIRE(hera::get<4>(larger) == 5);
        REQUIRE(hera::get<5>(larger) == 5);
        REQUIRE(hera::get<6>(larger) == 0);

        REQUIRE(larger.front() == 5);
        REQUIRE(larger.back() == 0);
    }

    // I have verified the out of bounds check works, don't know how to add it
    // as a test case right now
}