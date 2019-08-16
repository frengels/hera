#include <catch2/catch.hpp>

#include "hera/size.hpp"

#include <tuple>

TEST_CASE("size")
{
    SECTION("tuple")
    {
        auto       tup       = std::make_tuple(1, 2, 3, 4, 5);
        const auto const_tup = tup;

        static_assert(hera::size(tup) == 5);
        static_assert(hera::size(const_tup) == 5);
    }

    SECTION("array")
    {
        int       arr[]{1, 2, 3};
        const int const_arr[]{1, 2, 3, 4};

        static_assert(hera::size(arr) == 3);
        static_assert(hera::size(const_arr) == 4);
    }

    SECTION("non_constant")
    {
        auto vec = std::vector<int>{};

        static_assert(
            !std::is_invocable_v<decltype(hera::size), decltype(vec)>);
    }
}
