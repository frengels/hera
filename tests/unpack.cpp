#include <catch2/catch.hpp>

#include "hera/algorithm/unpack.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("unpack")
{
    auto tup      = std::make_tuple(1, 2, 8, 4, 5.0f, 69.69f);
    auto tup_view = hera::views::tuple(tup);

    hera::unpack(tup_view,
                 [&](int i1, int i2, int i8, int i4, float f5, float f69) {
                     REQUIRE(i1 == get<0>(tup));
                     REQUIRE(i2 == get<1>(tup));
                     REQUIRE(i8 == get<2>(tup));
                     REQUIRE(i4 == get<3>(tup));
                     REQUIRE(f5 == get<4>(tup));
                     REQUIRE(f69 == get<5>(tup));
                 });

    auto unpacks_equal = hera::unpack(tup_view, [&](auto&&... xs) {
        return std::apply([=](auto&&... ys) { return ((xs == ys) && ...); },
                          tup);
    });

    REQUIRE(unpacks_equal);
}