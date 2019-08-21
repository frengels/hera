#include <catch2/catch.hpp>

#include "hera/begin_end.hpp"
#include "hera/view/array.hpp"
#include "hera/view/tuple.hpp"

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

template<hera::forward_iterator I>
void req_fwd_it(I)
{}

template<hera::random_access_iterator I>
void req_random_it(I)
{}

template<typename T, hera::writable<T> I>
void req_writable(I)
{}

TEST_CASE("begin_end")
{
    auto tup_ = std::make_tuple("hello", 42, 3.5f);
    auto tup  = hera::tuple_view{tup_};
    int  arr_[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto arr = hera::array_view{arr_};

    auto tfirst = hera::begin(tup);
    auto tmid   = ++tfirst;
    auto tlast  = ++tmid;
    auto tend   = ++tlast; // should be able to go past last

    static_assert(hera::bidirectional_iterator<decltype(tfirst)>);
    req_random_it(tfirst);
    static_assert(hera::readable<decltype(tfirst)>);
    req_writable<int&>(tmid);

    req_it_sent_pair(tfirst, tmid);
    req_it_sent_sized(tfirst, tmid);

    static_assert(
        hera::same_as<const char*,
                      hera::readable_traits<decltype(tfirst)>::value_type>);

    req_fwd_it(tfirst);

    auto call_end = hera::end(tup);

    req_it_sent_sized(tfirst, call_end);

    static_assert(tend == call_end);

    REQUIRE(std::strcmp("hello", *tfirst) == 0);
    REQUIRE(42 == *tmid);
    REQUIRE(3.5f == *tlast);

    auto deref = [](auto&& val) -> decltype(*std::forward<decltype(val)>(val)) {
        return *std::forward<decltype(val)>(val);
    };

    // begin and end shouldn't be callable for an rvalue
    static_assert(
        !std::is_invocable_v<decltype(hera::begin), std::tuple<int>&&>);
    static_assert(!std::is_invocable_v<decltype(hera::end), std::tuple<int>&&>);

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

    SECTION("empty_range")
    {
        auto empty_tup_ = std::make_tuple();
        auto empty_tup  = hera::tuple_view{empty_tup_};

        // should be valid to call
        auto empty_beg = hera::begin(empty_tup);
    }

    SECTION("heterogeneous")
    {
        auto not_het = almost_heterogeneous_range{};
        static_assert(
            !std::is_invocable_v<decltype(hera::begin), decltype(not_het)>);
    }
}
