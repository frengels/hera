#include <catch2/catch.hpp>

#include "hera/container/type_list.hpp"
#include "hera/ranges.hpp"
#include "hera/view.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("type_list")
{
    hera::type_list<int, float, char> types{};

    static_assert(hera::range<decltype(types)>);
    static_assert(hera::bounded_range<decltype(types)>);
    static_assert(hera::view<decltype(types)>);

    auto tl1 =
        hera::make_type_list_from_metafunctions(hera::typeid_(5),
                                                hera::typeid_(5.0f),
                                                hera::typeid_('c'),
                                                hera::type_identity<int>{});

    static_assert(
        hera::same_as<hera::type_list<int, float, char, int>, decltype(tl1)>);

    auto tup = std::make_tuple('c', "hello"); // char, const char*

    auto tl2 = hera::tuple_view{tup} | hera::to_base_type_list;

    static_assert(
        hera::same_as<decltype(tl2), hera::type_list<char, const char*>>);

    static_assert(hera::same_as<hera::type_identity<const char*>,
                                decltype(hera::get<1>(tl2))>);

    SECTION("destructure")
    {
        // TODO once hera/destructure is done
        // auto [i1, f1, c1, i2] = std::move(tl1); // shouldn't be different
        // static_assert(hera::same_as<hera::type_identity<int>, decltype(i1)>);

        // auto [c, str] = tl2;

        // static_assert(decltype(c == hera::type_identity<char>{})::value);
        // static_assert(
        //    decltype(str == hera::type_identity<const char*>{})::value);
    }
}