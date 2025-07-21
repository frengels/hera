#include <catch2/catch_test_macros.hpp>

#include "hera/view/drop.hpp"
#include "hera/view/iota.hpp"

TEST_CASE("drop_view")
{
    auto iota = hera::iota_view<0>{};

    auto drop = hera::drop_view{iota, std::integral_constant<std::size_t, 5>{}};

    static_assert(decltype(hera::get<0>(drop))::value == 5);

    static_assert(hera::size(iota) == hera::size(drop));

    auto drop2 =
        hera::views::drop(iota, std::integral_constant<std::size_t, 5>{});

    auto drop3 =
        iota | hera::views::drop(std::integral_constant<std::size_t, 10>{});

    static_assert(hera::get<0>(drop3) == 10);
}