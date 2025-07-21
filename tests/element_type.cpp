#include <catch2/catch_test_macros.hpp>

#include <tuple>

#include "hera/concepts.hpp"
#include "hera/container/tuple.hpp"
#include "hera/element_type.hpp"

struct no_element_type
{};

TEST_CASE("element_type")
{
    SECTION("hera")
    {
        hera::tuple<int, int, double> t{};

        static_assert(
            hera::same_as<typename decltype(hera::element_type<0>(t))::type,
                          int>);
    }

    SECTION("std")
    {
        std::tuple<int, int, double> std_tup{};

        static_assert(hera::same_as<typename decltype(
                                        hera::element_type<0>(std_tup))::type,
                                    int>);
    }

    SECTION("sfinae")
    {
        static_assert(!std::is_invocable_v<decltype(hera::element_type<0>),
                                           no_element_type>);

        // this is impossible to achieve
        // static_assert(!std::is_invocable_v<decltype(hera::element_type<100>),
        //                                   hera::tuple<>>); std::tuple<>>);
    }
}