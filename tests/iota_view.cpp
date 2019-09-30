#include <catch2/catch.hpp>

#include "hera/view/iota.hpp"

template<hera::unbounded_range R>
constexpr auto verify_unbounded_range(R)
{}

TEST_CASE("iota_view")
{
    auto iota = hera::iota_view<0>{};

    verify_unbounded_range(iota);
    static_assert(!hera::bounded_range<decltype(iota)>);
    static_assert(hera::forwarding_range<decltype(iota)>);

    auto beg = iota.begin();

    REQUIRE(*beg == 0);

    REQUIRE(beg[std::integral_constant<std::ptrdiff_t, 50>{}] == 50);
    REQUIRE(*++beg == 1);
}