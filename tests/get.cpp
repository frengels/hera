#include <catch2/catch.hpp>

#include "hera/get.hpp"

TEST_CASE("get")
{
    auto empty_tup = std::make_tuple();
    auto tup       = std::make_tuple(0, 1, 2);

    SECTION("empty")
    {
        static_assert(
            !std::is_invocable_v<decltype(hera::get<0>), decltype(empty_tup)>);
        static_assert(!std::is_invocable_v<decltype(hera::get<50000>),
                                           decltype(empty_tup)>);
    }

    SECTION("nonempty")
    {
        static_assert(
            std::is_invocable_v<decltype(hera::get<0>), decltype(tup)>);
        static_assert(
            std::is_invocable_v<decltype(hera::get<1>), decltype(tup)>);
        static_assert(
            std::is_invocable_v<decltype(hera::get<2>), decltype(tup)>);
        static_assert(
            !std::is_invocable_v<decltype(hera::get<3>), decltype(tup)>);
        static_assert(
            !std::is_invocable_v<decltype(hera::get<9001>), decltype(tup)>);
    }
}