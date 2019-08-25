#include <catch2/catch.hpp>

#include "hera/view/filter.hpp"
#include "hera/view/tuple.hpp"

#include "hera/algorithm/for_each.hpp"

TEST_CASE("for_each")
{
    auto tup = std::make_tuple("hi", "bye", 1, 2, 3, "again", 4, 5);

    auto filtered =
        hera::views::tuple(tup) | hera::views::filter([](auto x) {
            return std::bool_constant<hera::same_as<int, decltype(x)>>{};
        });

    // iterator based
    hera::for_each(hera::begin(filtered),
                   hera::end(filtered),
                   [i = 1](auto x) mutable { REQUIRE(x == i++); });

    // range based
    hera::for_each(filtered, [i = 1](auto x) mutable { REQUIRE(x == i++); });

    static_assert(noexcept(hera::for_each(
        filtered, [i = 1](auto x) mutable noexcept { REQUIRE(x == i++); })));
}