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

    auto t0  = hera::begin(sorted);
    auto t1  = hera::next(t0);
    auto t2  = hera::next(t1);
    auto t3  = hera::next(t2);
    auto end = hera::next(t3);

    REQUIRE(end == hera::end(sorted));
    REQUIRE(*t0 == hera::type_identity<uint8_t>{});
    REQUIRE(*t1 == hera::type_identity<uint16_t>{});
    REQUIRE(*t2 == hera::type_identity<uint32_t>{});
    REQUIRE(*t3 == hera::type_identity<uint64_t>{});

    auto sorted_from_it_sent = hera::sort(
        hera::begin(types), hera::end(types), [](auto lhs, auto rhs) {
            using lhs_type = typename decltype(lhs)::type;
            using rhs_type = typename decltype(rhs)::type;

            return std::bool_constant<(sizeof(lhs_type) < sizeof(rhs_type))>{};
        });

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