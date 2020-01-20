#include <catch2/catch.hpp>

#include <string_view>

#include "hera/view.hpp"
#include "hera/view/ref.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("ref_view")
{
    auto tup = std::tuple(1, 2, 3, 'c', std::string_view("hi"));

    auto tup_view = hera::views::tuple(tup);

    auto ref_view = hera::ref_view(tup_view);

    static_assert(hera::view<decltype(ref_view)>);

    REQUIRE(tup_view.size() == ref_view.size());
    REQUIRE(hera::at<0>(tup_view) == hera::at<0>(ref_view));

    // == doesn't work because I'm using catch2 a little wrong
    REQUIRE(tup_view.back().compare(ref_view.back()) == 0);
}