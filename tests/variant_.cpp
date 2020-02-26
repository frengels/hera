#include <catch2/catch.hpp>

#include <hera/variant/variant.hpp>

TEST_CASE("variant")
{
    static_assert(std::is_trivially_destructible_v<hera::variant<int, bool>>);
}