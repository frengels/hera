#include <catch2/catch.hpp>

#include <cstring>

#include "hera/begin_end.hpp"
#include "hera/container/tuple.hpp"
#include "hera/get.hpp"
#include "hera/next_prev.hpp"
#include "hera/ranges.hpp"

template<std::size_t I>
constexpr auto size_ = std::integral_constant<std::size_t, I>{};

TEST_CASE("tuple")
{
    auto tup    = hera::tuple{"hello", "tuple", 42};
    auto stdtup = std::tuple{"hello", "tuple", 42};

    // op []
    REQUIRE(std::strcmp("tuple", tup[size_<1>]) == 0);

    static_assert(hera::same_as<int&&, decltype(std::move(tup)[size_<2>])>);
    static_assert(hera::same_as<int&, decltype(tup[size_<2>])>);

    static_assert(hera::same_as<int&&, decltype(get<2>(std::move(stdtup)))>);
    static_assert(hera::same_as<int&, decltype(get<2>(stdtup))>);

    auto first  = hera::begin(tup);
    auto cfirst = tup.cbegin();
    static_assert(decltype(first == cfirst)::value);
    static_assert(hera::constructible_from<decltype(cfirst), decltype(first)>);
    REQUIRE(std::strcmp("hello", *first) == 0);
    auto second  = hera::next(first);
    auto csecond = hera::next(cfirst);
    REQUIRE(std::strcmp("tuple", *second) == 0);
    auto last  = hera::next(second);
    auto clast = hera::next(csecond);
    REQUIRE(42 == *last);
    auto end = hera::next(last);

    static_assert(decltype(end == hera::end(tup))::value);

    static_assert(hera::random_access_iterator<decltype(first)>);
    static_assert(hera::random_access_range<decltype(tup)>);

    auto deref = [](auto x) -> decltype(*x) { return *x; };

    static_assert(!std::is_invocable_v<decltype(deref), decltype(end)>);

    static_assert(decltype(end == hera::end(tup))::value);

    auto [h, t, n] = tup;

    SECTION("forward")
    {
        auto tup    = hera::forward_as_tuple(5, 5.0f);
        auto stdtup = std::forward_as_tuple(5, 5.0f);
        static_assert(hera::same_as<int&, decltype(get<0>(stdtup))>);

        static_assert(hera::same_as<int&, decltype(tup[size_<0>])>);
    }
}