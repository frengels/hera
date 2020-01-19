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

        ij.or_else([]() { return hera::none{}; });
        ij.or_else([]() { return hera::just<int>{5}; });
        ij.and_then([](int i) { return hera::none{}; });
    }

    SECTION("none")
    {
        auto n = hera::none{};

        static_assert(!n);

        auto no_char = n.transform([](auto i) { return 'c'; });

        auto no_void = no_char.transform([](auto c) {});

        auto none_again =
            no_void.and_then([]() { return hera::just<int>{5}; }).or_else([]() {
                return hera::none{};
            });

        static_assert(hera::same_as<decltype(none_again), hera::none>);
    }
}