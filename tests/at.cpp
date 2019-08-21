#include <catch2/catch.hpp>

#include <cstring>

#include "hera/at.hpp"
#include "hera/view/array.hpp"
#include "hera/view/tuple.hpp"

template<std::size_t I>
inline constexpr auto size_t_ = std::integral_constant<std::size_t, I>{};

TEST_CASE("at")
{
    auto tup      = std::make_tuple("hello", 42, 53);
    auto tup_view = hera::tuple_view{tup};
    int  arr[]{0, 1, 2, 3, 4, 5};
    auto arr_view = hera::array_view{arr};

    SECTION("tuple")
    {
        REQUIRE(hera::at(tup_view, size_t_<1>) == 42);
        REQUIRE(hera::at(tup_view, size_t_<2>) == 53);

        // back not currently available
        // REQUIRE(tup_view.back() == 53);
        REQUIRE(std::strcmp(tup_view.front(), "hello") == 0);

        static_assert(!std::is_invocable_v<decltype(hera::at),
                                           decltype(tup_view),
                                           decltype(size_t_<50>)>);
    }

    SECTION("array")
    {
        REQUIRE(hera::at(arr_view, size_t_<0>) == 0);
        REQUIRE(hera::at(arr_view, size_t_<5>) == 5);

        static_assert(!std::is_invocable_v<decltype(hera::at),
                                           decltype(arr_view),
                                           decltype(size_t_<100>)>);
    }
}
