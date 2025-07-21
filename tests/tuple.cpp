#include <catch2/catch_test_macros.hpp>

#include <cstring>
#include <string>

#include "hera/container/tuple.hpp"
#include "hera/ranges.hpp"

template<std::size_t I>
constexpr auto size_ = std::integral_constant<std::size_t, I>{};

TEST_CASE("tuple")
{
    auto tup    = hera::tuple{"hello", "tuple", 42};
    auto stdtup = std::tuple{"hello", "tuple", 42};

    // op []
    REQUIRE(std::strcmp("tuple", hera::get<1>(tup)) == 0);

    // front
    REQUIRE(std::strcmp("hello", tup.front()) == 0);
    REQUIRE(42 == tup.back());

    static_assert(hera::same_as<int&&, decltype(hera::get<2>(std::move(tup)))>);
    static_assert(hera::same_as<int&, decltype(hera::get<2>(tup))>);

    // TODO structured bindings
    // auto [h, t, n] = tup;

    SECTION("forward")
    {
        auto tup    = hera::forward_as_tuple(5, 5.0f);
        auto stdtup = std::forward_as_tuple(5, 5.0f);
        static_assert(hera::same_as<int&, decltype(get<0>(stdtup))>);
        static_assert(
            hera::same_as<int&&, decltype(get<0>(std::move(stdtup)))>);

        static_assert(hera::same_as<int&, decltype(hera::get<0>(tup))>);
        // static_assert(hera::same_as<int&&,
        // decltype(hera::get<0>(std::move(tup))>);
    }

    SECTION("assignable")
    {
        auto tup = hera::tuple<int, float>{5, 5.0f};

        // tup = hera::tuple<int, int>{42, 2};

        // tup = hera::tuple{43, 43};

        hera::tuple<int, int> another_tup{tup};
    }

    SECTION("emplace_back/front")
    {
        auto tup = hera::tuple{1, 2, 3};

        auto tup1 = tup.push_back(4);
        REQUIRE(tup1.back() == 4);
        REQUIRE(tup1.size() == 4);

        auto tup_str = hera::tuple<std::string, std::string>{"hi", "world"};
        auto tup_str1 =
            std::move(tup_str).emplace_front<std::string>("knock knock");

        REQUIRE(tup_str.back() != "world"); // show the moved from state
        REQUIRE(tup_str1.back() == "world");
        REQUIRE(tup_str1.front() == "knock knock");
    }
}