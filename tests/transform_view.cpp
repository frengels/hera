#include <catch2/catch.hpp>

#include "hera/algorithm/unpack.hpp"
#include "hera/begin_end.hpp"
#include "hera/container/pair.hpp"
#include "hera/container/tuple.hpp"
#include "hera/view/filter.hpp"
#include "hera/view/iota.hpp"
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

    SECTION("pipe_with_filter")
    {
        auto ints = hera::iota_view<0>{};

        auto triple_even =
            ints | hera::views::filter([](auto integral_const) {
                return std::bool_constant<(decltype(integral_const)::value %
                                           2) == 0>{};
            }) |
            hera::views::transform([](auto even_consts) {
                using const_type = decltype(even_consts);
                return std::integral_constant<typename const_type::value_type,
                                              const_type::value * 3>{};
            });

        auto expect_0 = hera::begin(triple_even); // 0 * 3

        static_assert(decltype(*expect_0)::value == 0);
        auto expect_6 = hera::next(expect_0); // 2 * 3

        static_assert(decltype(*expect_6)::value == 6);

        auto expect_12 = hera::next(expect_6); // 4 * 3
        static_assert(decltype(*expect_12)::value == 12);
    }
}