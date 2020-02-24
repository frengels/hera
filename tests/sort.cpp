#include <catch2/catch.hpp>

#include "hera/algorithm/sort.hpp"

TEST_CASE("sort")
{
    hera::type_list<uint32_t, uint64_t, uint16_t, uint8_t> types{};

    auto sorted = hera::sort(types, [](auto lhs, auto rhs) {
        using lhs_type = typename decltype(lhs)::type;
        using rhs_type = typename decltype(rhs)::type;

        return std::bool_constant<(sizeof(lhs_type) < sizeof(rhs_type))>{};
    });

    REQUIRE(hera::size(sorted) == 4);
    REQUIRE(hera::get<0>(sorted) == hera::type_<uint8_t>{});
    REQUIRE(hera::get<1>(sorted) == hera::type_<uint16_t>{});
    REQUIRE(hera::get<2>(sorted) == hera::type_<uint32_t>{});
    REQUIRE(hera::get<3>(sorted) == hera::type_<uint64_t>{});

    // also support duplicate types
    SECTION("with duplicates")
    {
        hera::type_list<int, int, int> dup_types{};

        auto sorted_dups = hera::sort(dup_types, [](auto lhs, auto rhs) {
            return std::bool_constant<(sizeof(typename decltype(lhs)::type) <
                                       sizeof(typename decltype(rhs)::type))>{};
        });
    }
}