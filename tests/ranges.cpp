#include <catch2/catch.hpp>

#include "hera/ranges.hpp"

template<hera::normal_range R>
void verify_normal_range(R)
{}

TEST_CASE("ranges")
{
    SECTION("normal_range")
    {
        auto tup = std::make_tuple(50, "hi", "bye");
        verify_normal_range(tup);

        std::array<int, 1000000> arr;

        verify_normal_range(arr);
    }
}