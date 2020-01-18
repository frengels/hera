#include <catch2/catch.hpp>

#include "hera/view/all.hpp"
#include "hera/view/array.hpp"
#include "hera/view/tuplelike.hpp"

TEST_CASE("all_view")
{
    auto tup      = std::make_tuple(5, 5, 5, 5);
    auto tup_view = hera::tuplelike_view{tup};

    auto all_tup = hera::views::all(tup_view);

    static_assert(hera::same_as<decltype(tup_view), decltype(all_tup)>);
}