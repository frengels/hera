#include <catch2/catch.hpp>

#include "hera/view/iota.hpp"

template<hera::unbounded_range R>
constexpr auto verify_unbounded_range(R)
{}

TEST_CASE("iota_view")
{
    SECTION("finite")
    {
        auto iota = hera::iota_view<0>{};

        verify_unbounded_range(iota);
        static_assert(!hera::bounded_range<decltype(iota)>);
        REQUIRE(hera::at<50>(iota) == 50);
    }

    SECTION("finite")
    {
        auto iota = hera::iota_view<10, 50>{};

        static_assert(hera::bounded_range<decltype(iota)>);
        REQUIRE(hera::at<10>(iota) == 20);
    }

    SECTION("empty")
    {
        auto iota = hera::iota_view<20, 20>{};

        static_assert(hera::empty_range<decltype(iota)>);

        REQUIRE(iota.empty());
    }
}