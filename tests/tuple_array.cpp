#include <catch2/catch.hpp>

#include "hera/view/array.hpp"
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

    static_assert(decltype(hera::end(tup_view) == tup_end)::value);
}

TEST_CASE("array")
{
    int arr[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto arr_view = hera::views::array(arr);

    SECTION("unrolling")
    {
        std::vector<int> vec{0, 1, 2, 3, 4, 5};

        auto vec_view = hera::array_view{
            vec.data(), std::integral_constant<std::size_t, 6>{}};
    }
}