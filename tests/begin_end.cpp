#include <catch2/catch.hpp>

#include "hera/begin_end.hpp"
#include "hera/sentinel.hpp"

#include <cstring>
#include <string>
#include <utility>

// not heterogeneous because the return type is the same as the called upon type
struct not_heterogeneous_iterator
{
    using difference_type = std::ptrdiff_t;

    auto operator*()
    {
        return 5;
    }

    auto& operator++()
    {
        return *this;
    }
};

struct almost_heterogeneous_range
{
    not_heterogeneous_iterator begin()
    {
        return {};
    }

    std::integral_constant<std::size_t, 5> size()
    {
        return {};
    }
};

template<hera::heterogeneous_iterator I, hera::sentinel_for<I> S>
void req_it_sent_pair(I, S)
{}

template<hera::heterogeneous_iterator I, hera::sized_sentinel_for<I> S>
void req_it_sent_sized(I, S)
{}

TEST_CASE("begin_end")
{
    auto tup = std::make_tuple("hello", 42, 3.5f);
    int  arr[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto tfirst = hera::begin(tup);
    auto tmid   = ++tfirst;
    auto tlast  = ++tmid;
    auto tend   = ++tlast;

    req_it_sent_pair(tfirst, tmid);
    req_it_sent_sized(tfirst, tmid);

    auto call_end = hera::end(tup);

    req_it_sent_sized(tfirst, call_end);

    static_assert(tend == call_end);

    REQUIRE(std::strcmp("hello", *tfirst) == 0);
    REQUIRE(42 == *tmid);
    REQUIRE(3.5f == *tlast);

    auto deref = [](auto&& val) -> decltype(*std::forward<decltype(val)>(val)) {
        return *std::forward<decltype(val)>(val);
    };

    // all dereferenceable except past the end
    static_assert(std::is_invocable_v<decltype(deref), decltype(tfirst)>);
    static_assert(std::is_invocable_v<decltype(deref), decltype(tmid)>);
    static_assert(std::is_invocable_v<decltype(deref), decltype(tlast)>);
    static_assert(!std::is_invocable_v<decltype(deref), decltype(tend)>);

    SECTION("arr")
    {
        auto arr_first = hera::begin(arr);

        REQUIRE(*arr_first == 1);

        // decltype sadly required to lift this back to compile time
        static_assert(
            decltype((arr_first + std::integral_constant<int, 10>{}) ==
                     hera::end(arr))::value);
    }

    SECTION("hetrogeneous")
    {
        auto not_het = almost_heterogeneous_range{};
        static_assert(
            !std::is_invocable_v<decltype(hera::begin), decltype(not_het)>);
    }
}
