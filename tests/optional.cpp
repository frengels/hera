#include <catch2/catch.hpp>

#include "hera/optional.hpp"

TEST_CASE("optional")
{
    SECTION("just")
    {
        auto j = hera::just<int>(50);

        static_assert(j);

        auto void_just = j.transform([](auto) {});

        static_assert(
            hera::same_as<void, typename decltype(void_just)::value_type>);

        auto ij = void_just.transform([]() { return 42; });

        REQUIRE(*ij == 42);

        ij.or_else([]() { return hera::none<int>{}; });
        ij.or_else([]() { return hera::just<int>{5}; });
        ij.and_then([](int i) { return hera::none<char>{}; });
    }

    SECTION("none")
    {
        auto n = hera::none<int>{};

        static_assert(!n);

        auto no_char = n.transform([](auto i) { return 'c'; });

        auto no_void = no_char.transform([](auto c) {});

        no_void.and_then([]() { return hera::just<int>{5}; }).or_else([]() {
            return hera::none<int>{};
        });

        static_assert(
            hera::same_as<void, typename decltype(no_void)::value_type>);

        static_assert(
            hera::same_as<char, typename decltype(no_char)::value_type>);
    }
}