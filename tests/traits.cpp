#include <catch2/catch.hpp>

#include <type_traits>

#include "hera/constant.hpp"
#include "hera/iterator.hpp"

template<hera::constant_incrementable I>
void verify_const_incrementable(I)
{}

TEST_CASE("traits")
{
    SECTION("constant")
    {
        static_assert(hera::constant<std::integral_constant<bool, false>>);
        static_assert(hera::constant<std::true_type>);
        static_assert(hera::constant<std::false_type>);

        static_assert(hera::constant_same_as<std::true_type, bool>);
    }

    SECTION("constant_convertible_to")
    {
        static_assert(hera::constant_convertible_to<std::true_type, int>);
        static_assert(hera::constant_convertible_to<
                      std::integral_constant<std::ptrdiff_t, 0>,
                      std::size_t>);
    }

    SECTION("constant_same_as")
    {
        static_assert(
            hera::constant_same_as<std::integral_constant<int, 5>, int>);
    }
}
