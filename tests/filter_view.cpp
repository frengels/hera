#include <catch2/catch.hpp>

#include "hera/view/filter.hpp"
#include "hera/view/iota.hpp"
#include "hera/view/tuple.hpp"

TEST_CASE("filter_view")
{
    auto tup      = std::make_tuple(1, 2.0f, 3, 4.0f, 5);
    auto tup_view = hera::tuple_view{tup};

    auto first_i =
        hera::find_if(hera::begin(tup_view), hera::end(tup_view), [](auto x) {
            return std::bool_constant<hera::same_as<float, decltype(x)>>{};
        });

    REQUIRE(*first_i == 2.0f);

    auto int_predicate = [](auto x) {
        return std::bool_constant<hera::same_as<decltype(x), int>>{};
    };

    auto filt  = hera::filter_view{tup_view, int_predicate};
    auto filt2 = hera::filter_view{
        tup_view, hera::type_identity<decltype(int_predicate)>{}};

    // if int_predicate were declared constexpr this wouldn't pass
    static_assert(hera::same_as<decltype(filt), decltype(filt2)>);

    static_assert(hera::forwarding_range<decltype(filt)>);

    auto filt_beg = hera::begin(filt);
    REQUIRE(*filt_beg == 1);

    auto filt_mid = hera::next(filt_beg);
    REQUIRE(*filt_mid == 3);

    auto filt_last = hera::next(filt_mid);
    REQUIRE(*filt_last == 5);

    SECTION("unbounded")
    {
        auto v = hera::iota_view<0>{};

        static_assert(hera::unbounded_range<decltype(v)>);

        // filter even values
        auto iota_filt = hera::filter_view{
            v, [](auto x) {
                return std::bool_constant<(decltype(x)::value % 2) == 0>{};
            }};

        static_assert(hera::unbounded_range<decltype(iota_filt)>);
        static_assert(hera::forwarding_range<decltype(iota_filt)>);
        static_assert(std::is_empty_v<decltype(iota_filt)>);

        auto first = hera::begin(iota_filt);
        static_assert(decltype(*first)::value == 0);
        auto it_2 = hera::next(first);
        static_assert(decltype(*it_2)::value == 2);
    }
}