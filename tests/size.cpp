#include <catch2/catch.hpp>

#include "hera/size.hpp"

struct infinite_range
{
    constexpr hera::infinite size() const
    {
        return {};
    }
};

struct finite_range
{
    constexpr std::integral_constant<std::size_t, 5> size() const
    {
        return {};
    }
};

TEST_CASE("size")
{
    SECTION("infinite")
    {
        static_assert(hera::sized<infinite_range>);
        static_assert(hera::unbounded_size<infinite_range>);

        auto inf = infinite_range{};
        auto sz  = hera::size(inf);

        static_assert(hera::unbounded<decltype(sz)>);
    }

    SECTION("finite")
    {
        static_assert(hera::sized<finite_range>);
        static_assert(hera::bounded_size<finite_range>);

        auto fin = finite_range{};
        auto sz  = hera::size(fin);

        static_assert(hera::bounded<decltype(sz)>);
    }

    SECTION("invalid")
    {
        // has size method but not considered sized to us
        static_assert(!hera::sized<std::vector<int>>);
    }

    SECTION("std_tuple")
    {
        auto tup = std::make_tuple(5, 5, 5, 5, 5);

        static_assert(hera::size_v<decltype(tup)> == 5);
    }
}
