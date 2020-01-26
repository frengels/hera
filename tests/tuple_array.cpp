#include <catch2/catch.hpp>

#include "hera/view/array.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("tuple")
{
    auto tup = std::make_tuple(1, 2, 3, 432.f, "hello world", 4, 5, 'c', 6);

    auto tup_view = hera::tuple_view{tup};

    REQUIRE(hera::get<0>(tup_view) == 1);
    REQUIRE(hera::get<1>(tup_view) == 2);
    REQUIRE(hera::get<2>(tup_view) == 3);

    static_assert(hera::view<decltype(tup_view)>);
}

TEST_CASE("array")
{
    int arr[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto arr_view = hera::views::array(arr);

    auto std_arr = std::array{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto std_arr_view = hera::views::array(std_arr);

    const auto const_std_arr = std_arr;

    auto const_std_arr_view = hera::views::array(const_std_arr);

    SECTION("unrolling")
    {
        std::vector<int> vec{0, 1, 2, 3, 4, 5};

        auto vec_view = hera::array_view<int, 6>{vec};
        static_assert(hera::view<decltype(vec_view)>);
    }
}