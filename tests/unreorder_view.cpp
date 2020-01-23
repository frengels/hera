#include <catch2/catch.hpp>

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

    REQUIRE(hera::at<0>(tup) == hera::at<0>(unreordered));
    REQUIRE(hera::at<1>(tup) == hera::at<1>(unreordered));
    REQUIRE(hera::at<2>(tup) == hera::at<2>(unreordered));
    REQUIRE(hera::at<3>(tup) == hera::at<3>(unreordered));
    REQUIRE(hera::at<4>(tup) == hera::at<4>(unreordered));
    REQUIRE(hera::at<5>(tup) == hera::at<5>(unreordered));
}