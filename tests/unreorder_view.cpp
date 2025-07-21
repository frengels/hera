#include <catch2/catch_test_macros.hpp>

#include "hera/container/tuple.hpp"
#include "hera/view/reorder.hpp"
#include "hera/view/unreorder.hpp"

TEST_CASE("unreorder_view")
{
    auto tup = hera::tuple{5, 4, 3, 2, 1, 0};

    auto reordered =
        hera::reorder_view{tup, hera::index_sequence<0, 3, 1, 2, 5, 4>{}};

    auto unreordered =
        hera::unreorder_view{reordered, decltype(reordered)::sequence};

    REQUIRE(hera::get<0>(tup) == hera::get<0>(unreordered));
    REQUIRE(hera::get<1>(tup) == hera::get<1>(unreordered));
    REQUIRE(hera::get<2>(tup) == hera::get<2>(unreordered));
    REQUIRE(hera::get<3>(tup) == hera::get<3>(unreordered));
    REQUIRE(hera::get<4>(tup) == hera::get<4>(unreordered));
    REQUIRE(hera::get<5>(tup) == hera::get<5>(unreordered));
}