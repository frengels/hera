#include <catch2/catch.hpp>

#include "hera/view/array.hpp"
#include "hera/view/subrange.hpp"

TEST_CASE("subrange")
{
    int  arr[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto arr_view = hera::array_view{arr};

    auto subr = hera::subrange{hera::begin(arr_view),
                               hera::begin(arr_view) +
                                   std::integral_constant<std::ptrdiff_t, 5>{}};

    SECTION("begin")
    {
        auto beg = hera::begin(subr);
    }

    SECTION("front_back")
    {
        REQUIRE(subr.front() == 1);
        // back not yet implemented
        // REQUIRE(subr.back() == 5);
    }
}