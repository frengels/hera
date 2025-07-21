#include <catch2/catch_test_macros.hpp>

#include "hera/type_in_pack.hpp"

TEST_CASE("type_in_pack")
{
    static_assert(hera::type_in_pack<int, int>);
    static_assert(hera::type_in_pack<int, int&, int&&, int, const int>);
    static_assert(
        !hera::
            type_in_pack<int, int&, const int, const int&, int&&, const int&&>);

    SECTION("unique")
    {
        static_assert(hera::type_in_pack_unique<int, int>);
        static_assert(!hera::type_in_pack_unique<int, int, float, int>);
        static_assert(!hera::type_in_pack_unique<int>);
    }

    SECTION("index")
    {
        static_assert(hera::type_in_pack_index_v<int, int> == 0);
        static_assert(
            hera::type_in_pack_index_v<int, double, float, int, char> == 2);
    }
}