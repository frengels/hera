#include <catch2/catch.hpp>

#include "hera/at.hpp"

TEST_CASE("at")
{
    auto tup = std::make_tuple("hello", 42, 53);
    int  arr[]{0, 1, 2, 3, 4, 5};

    SECTION("tuple")
    {
        REQUIRE(hera::at(tup, std::integral_constant<std::size_t, 1>{}) == 42);
        REQUIRE(hera::at(tup, std::integral_constant<std::size_t, 2>{}) == 53);
    }
}
