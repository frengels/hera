#include <catch2/catch.hpp>

#include "hera/get_range.hpp"

template<hera::get_range T>
constexpr void verify_get_range(const T&)
{}

TEST_CASE("get_range")
{
    auto tup = std::make_tuple(5, 5);

    std::tuple_size_v<decltype(tup)>;
    hera::get<0>(tup);
    hera::get<1>(tup);
    verify_get_range(tup);
}