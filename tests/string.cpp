#include <catch2/catch_test_macros.hpp>

#include <tuple>

#include "hera/container/string.hpp"

template<hera::constant_string_of<char> S>
void verify_string(S)
{}

TEST_CASE("string")
{
    using namespace hera::literals;

    auto str = "hello"_s;

    verify_string(str);

    auto tup = std::tuple<std::integral_constant<char, 'h'>,
                          std::integral_constant<char, 'i'>>{};

    verify_string(tup);

    static_assert(hera::size_v<decltype(str)> == 5);

    static_assert(decltype(hera::get<0>(str))::value == 'h');
    static_assert(decltype(hera::get<1>(str))::value == 'e');
    static_assert(decltype(hera::get<4>(str))::value == 'o');

    auto str_sub = str.substr<0>();

    static_assert(decltype(str == str_sub)::value);

    REQUIRE(str[0] == 'h');

    SECTION("clear")
    {
        auto str_cl = str.clear();

        static_assert(decltype(str_cl.empty())::value);

        auto str_pushed = str_cl.push_back<'h'>().push_back<'i'>();

        static_assert(decltype(str_pushed == "hi"_s)::value);
    }

    SECTION("erase")
    {
        auto str_erased = str.template erase<4>();

        static_assert(decltype(str_erased == "hell"_s)::value);

        auto str_erased1 = str_erased.template erase<1>();

        static_assert(decltype(str_erased1 == "hll"_s)::value);
    }
}