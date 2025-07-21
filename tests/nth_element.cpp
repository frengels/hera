#include <catch2/catch_test_macros.hpp>

#include "hera/concepts.hpp"
#include "hera/nth_element.hpp"

TEST_CASE("nth_element")
{
    static_assert(hera::same_as<int, hera::nth_element_t<0, int, bool, float>>);

    static_assert(
        hera::same_as<
            float&&,
            hera::nth_element_t<1, float, float&&, float&, const float&>>);
}