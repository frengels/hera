#include <catch2/catch.hpp>

#include "hera/at.hpp"

template<std::size_t I>
inline constexpr auto size_t_ = std::integral_constant<std::size_t, I>{};

TEST_CASE("at")
{
    auto tup = std::make_tuple("hello", 42, 53);
    int  arr[]{0, 1, 2, 3, 4, 5};

    SECTION("tuple")
    {
        REQUIRE(hera::at(tup, size_t_<1>) == 42);
        REQUIRE(hera::at(tup, size_t_<2>) == 53);

        static_assert(!std::is_invocable_v<decltype(hera::at),
                                           decltype(tup),
                                           decltype(size_t_<50>)>);
    }

    SECTION("array")
    {
        REQUIRE(hera::at(arr, size_t_<0>) == 0);
        REQUIRE(hera::at(arr, size_t_<5>) == 5);

        static_assert(!std::is_invocable_v<decltype(hera::at),
                                           decltype(arr),
                                           decltype(size_t_<100>)>);
    }
}
