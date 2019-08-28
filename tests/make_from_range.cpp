#include <catch2/catch.hpp>

#include "hera/algorithm/make_from_range.hpp"
#include "hera/container/tuple.hpp"
#include "hera/view/filter.hpp"

template<std::size_t I>
constexpr auto size_ = std::integral_constant<std::size_t, I>{};

TEST_CASE("make_from_range")
{
    auto tup = hera::tuple{1, 'a', 2, 'b', 3, 'c'};

    auto int_tup = hera::make_from_range<hera::tuple<int, int, int>>(
        hera::filter_view{tup, [](auto val) {
                              if constexpr (hera::same_as<int, decltype(val)>)
                              {
                                  return std::true_type{};
                              }
                              else
                              {
                                  return std::false_type{};
                              }
                          }});

    REQUIRE(int_tup.size() == 3);
    REQUIRE(int_tup[size_<0>] == 1);
    REQUIRE(int_tup[size_<1>] == 2);
    REQUIRE(int_tup[size_<2>] == 3);

    auto char_tup = hera::make_from_range<hera::tuple<char, char, char>>(
        hera::filter_view{tup, [](auto val) {
                              if constexpr (hera::same_as<char, decltype(val)>)
                              {
                                  return std::true_type{};
                              }
                              else
                              {
                                  return std::false_type{};
                              }
                          }});

    REQUIRE(char_tup.size() == 3);
    REQUIRE(char_tup[size_<0>] == 'a');
    REQUIRE(char_tup[size_<1>] == 'b');
    REQUIRE(char_tup[size_<2>] == 'c');
}