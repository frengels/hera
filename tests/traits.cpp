#include <catch2/catch.hpp>

#include <type_traits>

#include "hera/bool_constant.hpp"
#include "hera/integral_constant.hpp"

TEST_CASE("traits")
{
    SECTION("integral_constant")
    {
        static_assert(
            hera::integral_constant<std::integral_constant<bool, false>>);
        static_assert(hera::integral_constant<std::true_type>);
        static_assert(hera::integral_constant<std::false_type>);

        static_assert(hera::integral_constant_for<std::true_type, bool>);
    }

    SECTION("bool_constant")
    {
        // can be converted to bool
        static_assert(hera::bool_constant<std::integral_constant<int, 5>>);
        static_assert(hera::bool_constant<std::true_type>);
        static_assert(hera::bool_constant<std::false_type>);
    }

    SECTION("integral_constant_for")
    {
        static_assert(hera::integral_constant_for<std::true_type, int>);
        static_assert(hera::integral_constant_for<
                      std::integral_constant<std::ptrdiff_t, 0>,
                      std::size_t>);
    }
}
