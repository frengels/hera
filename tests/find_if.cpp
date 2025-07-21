#include <catch2/catch_test_macros.hpp>

#include <tuple>

#include "hera/algorithm/find_if.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("find_if")
{
    auto tup      = std::make_tuple(1, 2, 3, "hello");
    auto tup_view = hera::views::tuple(tup);

    auto opt_str_idx = hera::find_if(tup_view, [](auto x) {
        return std::is_same<decltype(x), const char*>{};
    });

    auto str_idx = *opt_str_idx;

    static_assert(decltype(str_idx)::value == 3);

    auto opt_none = hera::find_if(
        tup_view, [](auto x) { return std::is_same<decltype(x), double>{}; });

    static_assert(!opt_none.has_value());
}