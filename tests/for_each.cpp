#include <catch2/catch.hpp>

#include "hera/view/tuple.hpp"

#include "hera/algorithm/for_each.hpp"

TEST_CASE("for_each")
{
    auto tup = std::make_tuple("hi", "bye", "again");

    // iterator based
    auto last_len =
        hera::for_each(hera::tuple_view(tup), [len = 0](auto x) mutable {
            len = std::string_view(x).size();
            return len;
        });
    REQUIRE(last_len == 5);
}