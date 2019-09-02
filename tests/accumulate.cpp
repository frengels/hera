#include <catch2/catch.hpp>

#include "hera/algorithm/accumulate.hpp"
#include "hera/container/tuple.hpp"

TEST_CASE("accumulate")
{
    auto tup = hera::tuple{1, 2, 3, 4, 5};

    auto negate = [](auto&& lhs, auto&& rhs) {
        return std::forward<decltype(lhs)>(lhs) -
               std::forward<decltype(rhs)>(rhs);
    };

    SECTION("range")
    {
        SECTION("default_op")
        {
            REQUIRE(hera::accumulate(tup, 0) == 15);
        }

        SECTION("custom_op")
        {
            REQUIRE(hera::accumulate(tup, 15, negate) == 0);
        }
    }

    SECTION("iterator")
    {
        SECTION("default_op")
        {
            REQUIRE(hera::accumulate(hera::begin(tup), hera::end(tup), 0) ==
                    15);
        }

        SECTION("custom_op")
        {
            REQUIRE(hera::accumulate(
                        hera::begin(tup), hera::end(tup), 15, negate) == 0);
        }
    }
}