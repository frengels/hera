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

    auto beg  = hera::get<0>(zip);
    auto mid  = hera::get<1>(zip);
    auto last = hera::get<2>(zip);

    REQUIRE(hera::get<0>(beg) == 0);
    REQUIRE(hera::get<1>(beg) == 'a');
    REQUIRE(hera::get<2>(beg) == 1.0f);

    REQUIRE(hera::get<0>(mid) == 1);
    REQUIRE(hera::get<1>(mid) == 'b');
    REQUIRE(hera::get<2>(mid) == 2.0f);

    REQUIRE(hera::get<0>(last) == 2);
    REQUIRE(hera::get<1>(last) == 'c');
    REQUIRE(hera::get<2>(last) == 3.0f);
}