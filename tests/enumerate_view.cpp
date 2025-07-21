#include <catch2/catch_test_macros.hpp>

#include "hera/view/enumerate.hpp"
#include "hera/view/iota.hpp"

TEST_CASE("enumerate")
{
    auto iota = hera::iota_view<0>{};

    auto enumerated = hera::enumerate_view(iota);

    REQUIRE(hera::get<0>(enumerated).first == 0);
    REQUIRE(hera::get<0>(enumerated).second == 0);

    REQUIRE(hera::get<50>(enumerated).first == 50);
    REQUIRE(hera::get<50>(enumerated).second == 50);

    iota | hera::views::enumerate;
}