#include <catch2/catch.hpp>

#include "hera/view/tuple.hpp"

TEST_CASE("tuple")
{
    auto tup = std::make_tuple(1, 2, 3, 432.f, "hello world", 4, 5, 'c', 6);

    auto tup_view = hera::tuple_view{tup};

    auto tup1 = hera::begin(tup_view);
    REQUIRE(*tup1 == 1);
    auto tup2 = hera::next(tup1);
    REQUIRE(*tup2 == 2);
    auto tup3 = hera::next(tup2);
    REQUIRE(*tup3 == 3);

    auto tup_end = tup3 + std::integral_constant<std::ptrdiff_t, 7>{};

    static_assert(hera::end(tup_view) == tup_end);

    SECTION("pipeable")
    {
        auto tup_pipe     = tup | hera::views::tuple();
        auto another_pipe = tup | hera::views::tuple;

        static_assert(hera::same_as<decltype(tup_view), decltype(tup_pipe)>);
        static_assert(
            hera::same_as<decltype(tup_pipe), decltype(another_pipe)>);
    }
}