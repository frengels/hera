#include <catch2/catch.hpp>

#include "hera/container/type_list.hpp"
#include "hera/ranges.hpp"
#include "hera/view.hpp"
#include "hera/view/tuple.hpp"

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

    auto tup = std::make_tuple('c', "hello"); // char, const char*

    auto tl2 = hera::tuple_view{tup} | hera::to_type_list;

    static_assert(
        hera::same_as<decltype(tl2), hera::type_list<char, const char*>>);

    static_assert(
        hera::same_as<hera::type_identity<const char*>,
                      decltype(tl2[std::integral_constant<std::size_t, 1>{}])>);
}