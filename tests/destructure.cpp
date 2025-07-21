#include <catch2/catch_test_macros.hpp>

#include "hera/container/tuple.hpp"
#include "hera/destructure.hpp"

TEST_CASE("destructure")
{
    auto tup = hera::make_tuple(5.0, 4.0f, 1, "hello");

    auto& [d, f, i, str] = tup;
}