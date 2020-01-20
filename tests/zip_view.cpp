#include <catch2/catch.hpp>

#include "hera/container/tuple.hpp"
#include "hera/view/zip.hpp"

TEST_CASE("zip_view")
{
    auto tup1 = hera::tuple{0, 1, 2, 3, 4, 5};
    auto tup2 = hera::tuple{'a', 'b', 'c'};
    auto tup3 = hera::tuple{1.0f, 2.0f, 3.0f, 4.0f};

    auto zip = hera::zip_view{tup1, tup2, tup3};

    REQUIRE(hera::size(zip) == 3);

    auto beg  = hera::at<0>(zip);
    auto mid  = hera::at<1>(zip);
    auto last = hera::at<2>(zip);

    REQUIRE(hera::at<0>(beg) == 0);
    REQUIRE(hera::at<1>(beg) == 'a');
    REQUIRE(hera::at<2>(beg) == 1.0f);

    REQUIRE(hera::at<0>(mid) == 1);
    REQUIRE(hera::at<1>(mid) == 'b');
    REQUIRE(hera::at<2>(mid) == 2.0f);

    REQUIRE(hera::at<0>(last) == 2);
    REQUIRE(hera::at<1>(last) == 'c');
    REQUIRE(hera::at<2>(last) == 3.0f);
}