#include <catch2/catch_test_macros.hpp>

#include <tuple>

#include "hera/algorithm/accumulate.hpp"
#include "hera/container/tuple.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("accumulate")
{
    SECTION("runtime")
    {
        auto tup = std::tuple{1, 2, 3, 4, 5};

        auto tup_view = hera::tuple_view(tup);

        auto negate = [](auto&& lhs, auto&& rhs) {
            return std::forward<decltype(lhs)>(lhs) -
                   std::forward<decltype(rhs)>(rhs);
        };

        SECTION("range")
        {
            SECTION("default_op")
            {
                REQUIRE(hera::accumulate(tup_view, 0) == 15);
            }

            SECTION("custom_op")
            {
                REQUIRE(hera::accumulate(tup_view, 15, negate) == 0);
            }
        }
    }

    SECTION("constexpr")
    {
        constexpr auto tup = hera::tuple{1, 2, 3, 4, 5};

        constexpr auto negate = [](auto lhs, auto rhs) { return lhs - rhs; };

        SECTION("range")
        {
            SECTION("default_op")
            {
                constexpr auto init = 0;
                REQUIRE(hera::accumulate(tup, init) == 15);
            }

            SECTION("custom_op")
            {
                REQUIRE(hera::accumulate(tup, 15, negate) == 0);
            }
        }
    }
}