#include <catch2/catch.hpp>

#include "hera/container/tuple.hpp"
#include "hera/view/zip.hpp"

TEST_CASE("zip_view")
{
    auto tup1 = hera::tuple{0, 1, 2, 3, 4, 5};
    auto tup2 = hera::tuple{'a', 'b', 'c'};
    auto tup3 = hera::tuple{1.0f, 2.0f, 3.0f, 4.0f};

    auto zip = hera::zip_view{tup1, tup2, tup3};

    auto zip_beg  = hera::begin(zip);
    auto zip_mid  = hera::next(zip_beg);
    auto zip_last = hera::next(zip_mid);
    auto zip_end  = hera::next(zip_last);

    SECTION("end_compare")
    {
        auto true_end = hera::end(zip);

        static_assert(!decltype(zip_beg == true_end)::value);
        static_assert(!decltype(zip_mid == true_end)::value);
        static_assert(!decltype(zip_last == true_end)::value);
        static_assert(decltype(zip_end == true_end)::value);
    }

    SECTION("sized_sentinel")
    {
        auto sent_end = hera::end(zip);

        static_assert(decltype(sent_end - zip_beg)::value == 3);
        static_assert(decltype(zip_beg - sent_end)::value == -3);
    }

    SECTION("deref")
    {
        auto [a1, b1, c1] = *zip_beg;
        REQUIRE(a1 == 0);
        REQUIRE(b1 == 'a');
        REQUIRE(c1 == 1.0f);
    }
}