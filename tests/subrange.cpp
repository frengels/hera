#include <catch2/catch.hpp>

#include "hera/subrange.hpp"

TEST_CASE("subrange")
{
    int arr[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto subr = hera::subrange{hera::begin(arr),
                               hera::begin(arr) +
                                   std::integral_constant<std::ptrdiff_t, 5>{}};

    SECTION("begin")
    {
        auto beg = hera::begin(subr);
        // beg.sdf();
    }

    /*
    SECTION("front_back")
    {
        REQUIRE(subr.front() == 1);
        REQUIRE(subr.back() == 5);
    }
    */
}