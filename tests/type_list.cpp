#include <catch2/catch.hpp>

#include "hera/container/type_list.hpp"
#include "hera/ranges.hpp"
#include "hera/view.hpp"

TEST_CASE("type_list")
{
    hera::type_list<int, float, char> types{};

    static_assert(hera::range<decltype(types)>);
    static_assert(hera::random_access_range<decltype(types)>);
    static_assert(hera::bounded_range<decltype(types)>);
    static_assert(hera::view<decltype(types)>);

    auto tl1 = hera::type_list{5, 5.0f, 'c', hera::type_identity<int>{}};
    static_assert(
        hera::same_as<hera::type_list<int, float, char, int>, decltype(tl1)>);
}