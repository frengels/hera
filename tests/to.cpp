#include <catch2/catch.hpp>

#include <tuple>

#include "hera/algorithm/to.hpp"
#include "hera/view/filter.hpp"

TEST_CASE("to")
{
    // tuple of ints and doubles
    auto tup = std::make_tuple(1, 1.0, 2, 2.0, 3, 3.0);

    auto double_filt = hera::filter_view(
        tup, [](auto x) { return std::is_same<double, decltype(x)>{}; });

    auto collected_doubles = double_filt | hera::to<std::tuple>;

    REQUIRE(hera::get<0>(collected_doubles) == 1.0);
    REQUIRE(hera::get<1>(collected_doubles) == 2.0);
    REQUIRE(hera::get<2>(collected_doubles) == 3.0);
}