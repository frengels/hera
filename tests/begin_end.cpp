#include <catch2/catch.hpp>

#include "hera/begin_end.hpp"

#include <cstring>
#include <utility>

TEST_CASE("begin_end")
{
    auto tup = std::make_tuple("hello", 42, 3.5f);
    int  arr[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto tfirst = hera::begin(tup);
    auto tmid   = ++tfirst;
    auto tlast  = ++tmid;
    auto tend   = ++tlast;

    REQUIRE(std::strcmp("hello", *tfirst) == 0);
    REQUIRE(42 == *tmid);
    REQUIRE(3.5f == *tlast);

    auto deref = [](auto&& val) -> decltype(*std::forward<decltype(val)>(val)) {
        return *std::forward<decltype(val)>(val);
    };

    // all dereferenceable except past the end
    static_assert(std::is_invocable_v<decltype(deref), decltype(tfirst)>);
    static_assert(std::is_invocable_v<decltype(deref), decltype(tmid)>);
    static_assert(std::is_invocable_v<decltype(deref), decltype(tlast)>);
    static_assert(!std::is_invocable_v<decltype(deref), decltype(tend)>);
}
