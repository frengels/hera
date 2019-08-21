#include <catch2/catch.hpp>

#include "hera/ranges.hpp"
#include "hera/view/tuple.hpp"

template<hera::random_access_range R>
void verify_random_access_range(const R&)
{}

TEST_CASE("ranges")
{
    auto tup_ = std::make_tuple(50, "hi", "bye");
    auto tup  = hera::tuple_view{tup_};

    SECTION("sized_range")
    {
        static_assert(hera::sized_range<decltype(tup)>);
    }

    SECTION("forwarding_range")
    {
        static_assert(hera::forwarding_range<decltype(tup)>);
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
        verify_random_access_range(tup);
    }
}