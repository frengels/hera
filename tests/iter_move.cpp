#include <catch2/catch.hpp>

#include "hera/begin_end.hpp"
#include "hera/concepts.hpp"
#include "hera/iterator/iter_move.hpp"
#include "hera/view/array.hpp"

struct void_derefable
{
    void operator*()
    {}
};

TEST_CASE("iter_move")
{
    auto arr = std::array{1, 2, 3, 4, 5};

    auto it = arr.begin();

    SECTION("lvalue")
    {
        static_assert(hera::same_as<int&, decltype(*it)>);
        static_assert(hera::same_as<int&&, decltype(hera::iter_move(it))>);
    }
    SECTION("rvalue")
    {
        auto move_it = std::make_move_iterator(it);
        static_assert(hera::same_as<int&&, decltype(*move_it)>);
        static_assert(hera::same_as<int&&, decltype(hera::iter_move(move_it))>);
    }
    SECTION("void_return")
    {
        static_assert(hera::same_as<void,
                                    decltype(hera::iter_move(
                                        std::declval<void_derefable&>()))>);
    }

    SECTION("heterogeneous")
    {
        auto it = hera::begin(hera::array_view{arr});

        static_assert(hera::same_as<int&&, decltype(hera::iter_move(it))>);
    }
}