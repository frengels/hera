#include <catch2/catch_test_macros.hpp>

#include "hera/view/move.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("move")
{
    auto tup       = std::make_tuple(std::string("hello"), std::string("test"));
    auto tupv      = hera::tuple_view{tup};
    auto move_tupv = hera::move_view{tupv};

    auto move_pipe = tupv | hera::views::move;
    static_assert(hera::same_as<decltype(move_tupv), decltype(move_pipe)>);

    auto&& ref  = hera::get<0>(tupv);
    auto&& move = hera::get<0>(move_tupv);

    static_assert(hera::same_as<std::string&, decltype(ref)>);
    static_assert(hera::same_as<std::string&&, decltype(move)>);
}