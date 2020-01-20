#include <catch2/catch.hpp>

#include "hera/algorithm/accumulate.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("accumulate")
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