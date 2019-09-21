#include <catch2/catch.hpp>

#include "hera/container/tuple.hpp"
#include "hera/view/reordered.hpp"

TEST_CASE("reordered_view")
{
    auto tup = hera::tuple{5, 4, 3, 2, 1, 0};

    auto reordered =
        hera::reordered_view{tup, hera::index_sequence<5, 4, 3, 2, 1, 0>{}};

    auto beg = hera::begin(reordered);

    REQUIRE(*beg == 0);

    auto n1 = hera::next(beg);
    REQUIRE(*n1 == 1);

    auto n2 = hera::next(n1);
    REQUIRE(*n2 == 2);

    auto n3 = hera::next(n2);
    REQUIRE(*n3 == 3);

    auto n4 = hera::next(n3);
    REQUIRE(*n4 == 4);

    auto n5 = hera::next(n4);
    REQUIRE(*n5 == 5);

    REQUIRE(hera::next(n5) == hera::end(reordered));

    SECTION("partial")
    {
        auto partial_reorder =
            hera::reordered_view{tup, hera::index_sequence<1, 0>{}};

        auto pbeg = hera::begin(partial_reorder);
        CHECK(*pbeg == 4);

        auto pn1 = hera::next(pbeg);
        CHECK(*pn1 == 5);

        auto pn2 = hera::next(pn1);
        REQUIRE(*pn2 == 3);
    }
}