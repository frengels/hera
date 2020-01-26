#include <catch2/catch.hpp>

#include "hera/get.hpp"
#include "hera/container/pair.hpp"
#include "hera/container/tuple.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("pair")
{
    auto p = hera::pair{5, 6.0f};

    auto other_pair = hera::pair{'c', "hello"};

    REQUIRE(p.first == 5);
    REQUIRE(p.second == 6.0f);

    p.first = 10;

    REQUIRE(p.first == 10);

    p.first = 5;

    SECTION("construct")
    {
        auto tup = std::make_tuple(5, 6);
        auto tv  = hera::tuple_view{tup};

        auto p1 = hera::pair<int, int>{hera::get<0>(tv), hera::get<1>(tv)};
    }

    SECTION("at")
    {
        REQUIRE(hera::get<0>(p) == 5);
        REQUIRE(hera::get<1>(p) == 6.0f);

        static_assert(
            hera::same_as<int&&, decltype(hera::get<0>(std::move(p)))>);

        static_assert(decltype(hera::size(p))::value == 2);
    }
}