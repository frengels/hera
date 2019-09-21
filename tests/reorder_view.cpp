#include <catch2/catch.hpp>

#include "hera/container/tuple.hpp"
#include "hera/view/reorder.hpp"

TEST_CASE("reordered_view")
{
    auto tup = hera::tuple{5, 4, 3, 2, 1, 0};

    auto reordered =
        hera::reorder_view{tup, hera::index_sequence<5, 4, 3, 2, 1, 0>{}};

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
            hera::reorder_view{tup, hera::index_sequence<1, 0>{}};

        auto pbeg = hera::begin(partial_reorder);
        REQUIRE(*pbeg == 4);

        auto pn1 = hera::next(pbeg);
        REQUIRE(*pn1 == 5);

        auto pn2 = hera::next(pn1);
        static_assert(!hera::dereferenceable<decltype(pn2)>);
    }

    SECTION("superset")
    {
        auto larger = tup | hera::views::reorder(
                                hera::index_sequence<0, 0, 0, 0, 0, 0, 5>{});

        REQUIRE(larger.size() == 7);

        auto s0 = hera::begin(larger);
        REQUIRE(*s0 == 5);

        auto s1 = hera::next(s0);
        REQUIRE(*s1 == 5);

        auto s2 = hera::next(s1);
        REQUIRE(*s2 == 5);

        auto s3 = hera::next(s2);
        REQUIRE(*s3 == 5);

        auto s4 = hera::next(s3);
        REQUIRE(*s4 == 5);

        auto s5 = hera::next(s4);
        REQUIRE(*s5 == 5);

        auto s6 = hera::next(s5);
        REQUIRE(*s6 == 0);

        REQUIRE(hera::next(s6) == hera::end(larger));

        REQUIRE(larger.front() == 5);
        REQUIRE(larger.back() == 0);
    }
}