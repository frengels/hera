#include <catch2/catch.hpp>

#include "hera/algorithm/unpack.hpp"
#include "hera/begin_end.hpp"
#include "hera/container/pair.hpp"
#include "hera/container/tuple.hpp"
#include "hera/view/transform.hpp"

TEST_CASE("transform")
{
    auto tup =
        hera::tuple{hera::pair{1, 5}, hera::pair{2, 6}, hera::pair{3, 7}};

    auto pairs_first = hera::transform_view{
        tup,
        [](auto&& pair) { return std::forward<decltype(pair)>(pair).first(); }};

    auto first = hera::begin(pairs_first);
    REQUIRE(1 == *first);

    auto sum_first =
        hera::unpack(pairs_first, [](auto... xs) { return (xs + ...); });

    REQUIRE(sum_first == 6);

    SECTION("pipe")
    {
        auto pairs_second =
            tup | hera::views::transform([](auto&& pair) {
                return std::forward<decltype(pair)>(pair).second();
            });

        auto sum_second =
            hera::unpack(pairs_second, [](auto... xs) { return (xs + ...); });

        REQUIRE(sum_second == 18);
    }
}