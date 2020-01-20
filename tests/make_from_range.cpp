#include <catch2/catch.hpp>

#include "hera/algorithm/make_from_range.hpp"
#include "hera/container/tuple.hpp"
#include "hera/view/filter.hpp"

template<std::size_t I>
constexpr auto size_ = std::integral_constant<std::size_t, I>{};

TEST_CASE("make_from_range")
{
    auto tup = hera::tuple{1, 'a', 2, 'b', 3, 'c'};

    auto int_tup =
        hera::make_from_range<hera::tuple<int, int, int>>(hera::filter_view{
            tup, [](auto val) { return std::is_same<int, decltype(val)>{}; }});

    REQUIRE(hera::size(int_tup) == 3);
    REQUIRE(hera::at<0>(int_tup) == 1);
    REQUIRE(hera::at<1>(int_tup) == 2);
    REQUIRE(hera::at<2>(int_tup) == 3);

    auto char_tup =
        hera::make_from_range<hera::tuple<char, char, char>>(hera::filter_view{
            tup, [](auto val) { return std::is_same<char, decltype(val)>{}; }});

    REQUIRE(hera::size(char_tup) == 3);
    REQUIRE(hera::at<0>(char_tup) == 'a');
    REQUIRE(hera::at<1>(char_tup) == 'b');
    REQUIRE(hera::at<2>(char_tup) == 'c');
}