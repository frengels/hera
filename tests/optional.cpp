#include <catch2/catch_test_macros.hpp>

#include <string>

#include <hera/optional.hpp>

TEST_CASE("optional")
{
    SECTION("traits")
    {
        SECTION("trivial")
        {
            using trivial_just = hera::just<int>;
            static_assert(std::is_trivially_destructible_v<trivial_just>);
            static_assert(std::is_trivially_move_assignable_v<trivial_just>);
            static_assert(std::is_trivially_copy_assignable_v<trivial_just>);
        }

        SECTION("non-trivial")
        {
            using nontriv_just = hera::just<std::string>;
            static_assert(!std::is_trivially_destructible_v<nontriv_just>);
            static_assert(!std::is_trivially_move_assignable_v<nontriv_just>);
            static_assert(!std::is_trivially_copy_assignable_v<nontriv_just>);
        }
    }

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