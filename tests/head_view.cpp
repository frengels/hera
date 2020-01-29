#include <catch2/catch.hpp>

#include "hera/view/array.hpp"
#include "hera/view/head.hpp"

TEST_CASE("head")
{

    int arr[]{1, 2, 3, 4, 5, 6, 7};

    auto head_arr = hera::views::array(arr) | hera::views::head;

    static_assert(decltype(hera::size(head_arr))::value == 1);

    REQUIRE(hera::get<0>(head_arr) == 1);
}