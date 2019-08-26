#include <catch2/catch.hpp>

#include "hera/at.hpp"
#include "hera/container/pair.hpp"
#include "hera/get.hpp"
#include "hera/next_prev.hpp"
#include "hera/view/head.hpp"
#include "hera/view/subrange.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("pair")
{
    constexpr auto p = hera::pair{5, 6.0f};

    auto other_pair = hera::pair{'c', "hello"};

    static_assert(p.first == 5);
    static_assert(p.second == 6.0f);

    SECTION("construct")
    {
        auto tup  = std::make_tuple(5, 6);
        auto tv   = hera::tuple_view{tup};
        auto head = tv | hera::views::head;

        auto tail = hera::subrange{hera::next(hera::begin(tv)), hera::end(tv)};

        auto p1 = hera::pair<int, int>{std::piecewise_construct, head, tail};
    }

    SECTION("op[]")
    {
        static_assert(p[std::integral_constant<std::size_t, 0>{}] == 5);
        static_assert(p[std::integral_constant<std::size_t, 1>{}] == 6.0f);

        static_assert(hera::at(p, std::integral_constant<std::size_t, 0>{}) ==
                      5);
        static_assert(hera::at(p, std::integral_constant<std::size_t, 1>{}) ==
                      6.0f);

        static_assert(
            hera::same_as<const int&&,
                          decltype(hera::at(
                              std::move(p),
                              std::integral_constant<std::size_t, 0>{}))>);

        static_assert(decltype(hera::size(p))::value == 2);
    }

    SECTION("get")
    {
        static_assert(hera::get<0>(p) == 5);
        static_assert(hera::get<1>(p) == 6.0f);

        static_assert(std::tuple_size_v<decltype(p)> == 2);
    }
}