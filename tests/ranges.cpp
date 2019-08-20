#include <catch2/catch.hpp>

#include "hera/ranges.hpp"

template<hera::normal_range R>
void verify_normal_range(R)
{}

TEST_CASE("ranges")
{
    auto tup = std::make_tuple(50, "hi", "bye");

    SECTION("normal_range")
    {
        verify_normal_range(tup);

        std::array<int, 1000000> arr;

        verify_normal_range(arr);
    }

    SECTION("sized_range")
    {
        static_assert(hera::sized_range<decltype(tup)>);
    }

    SECTION("forwarding_range")
    {
        static_assert(!hera::forwarding_range<decltype(tup)>);
    }

    SECTION("forward_range")
    {
        static_assert(hera::forward_range<decltype(tup)>);
    }

    SECTION("bidirectional_range")
    {
        static_assert(hera::bidirectional_range<decltype(tup)>);
    }

    SECTION("random_access_range")
    {
        static_assert(hera::random_access_range<decltype(tup)>);
    }
}