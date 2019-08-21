#include <catch2/catch.hpp>

#include "hera/view/iota.hpp"

TEST_CASE("iota_view")
{
    auto iota = hera::iota_view<0>{};

    static_assert(hera::unbounded_range<decltype(iota)>);
    static_assert(!hera::bounded_range<decltype(iota)>);
    static_assert(hera::forwarding_range<decltype(iota)>);

    auto beg = iota.begin();

    REQUIRE(*beg == 0);

    REQUIRE(beg[std::integral_constant<std::ptrdiff_t, 50>{}] == 50);
    REQUIRE(*++beg == 1);
}