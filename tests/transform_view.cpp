#include <catch2/catch.hpp>

#include "hera/algorithm/unpack.hpp"
#include "hera/begin_end.hpp"
#include "hera/container/pair.hpp"
#include "hera/container/tuple.hpp"
#include "hera/view/filter.hpp"
#include "hera/view/iota.hpp"
#include "hera/view/transform.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("transform")
{
    auto tup = std::tuple{hera::pair{1, 5}, hera::pair{2, 6}, hera::pair{3, 7}};
    auto tup_view = hera::tuple_view(tup);

    auto pairs_first =
        hera::transform_view{tup_view, [](auto& pair) { return pair.first; }};

    REQUIRE(hera::at<0>(pairs_first) == 1);

    auto sum_first =
        hera::unpack(pairs_first, [](auto... xs) { return (xs + ...); });

    REQUIRE(sum_first == 6);

    SECTION("pipe")
    {
        auto pairs_second = tup_view | hera::views::transform([](auto& pair) {
                                return pair.second;
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

        static_assert(decltype(hera::at<0>(triple_even))::value == 0);
        static_assert(decltype(hera::at<1>(triple_even))::value == 6);
        static_assert(decltype(hera::at<2>(triple_even))::value == 12);
    }
}