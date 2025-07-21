#include <catch2/catch_test_macros.hpp>

#include "hera/algorithm/all_of.hpp"
#include "hera/algorithm/any_of.hpp"
#include "hera/container/type_list.hpp"

TEST_CASE("any_of")
{
    // these are all empty
    auto empty_tl    = hera::type_list<hera::type_<int>, hera::type_<float>>{};
    auto nonempty_tl = hera::type_list<int, float>{}; // none of these are empty
    auto half_tl = hera::type_list<int, hera::type_<float>>{}; // one is empty

    static_assert(!decltype(hera::any_of(empty_tl, [](auto ident) {
        return std::bool_constant<
            !std::is_empty_v<typename decltype(ident)::type>>{};
    }))::value);

    static_assert(decltype(hera::any_of(nonempty_tl, [](auto ident) {
        return std::bool_constant<
            !std::is_empty_v<typename decltype(ident)::type>>{};
    }))::value);

    static_assert(decltype(hera::any_of(half_tl, [](auto ident) {
        return std::is_empty<typename decltype(ident)::type>{};
    }))::value);

    static_assert(decltype(hera::any_of(half_tl, [](auto ident) {
        return std::bool_constant<
            !std::is_empty_v<typename decltype(ident)::type>>{};
    }))::value);
}

TEST_CASE("all_of")
{
    // these are all empty
    auto empty_tl    = hera::type_list<hera::type_<int>, hera::type_<float>>{};
    auto nonempty_tl = hera::type_list<int, float>{}; // none of these are empty
    auto half_tl = hera::type_list<int, hera::type_<float>>{}; // one is empty

    static_assert(decltype(hera::all_of(empty_tl, [](auto ident) {
        return std::is_empty<typename decltype(ident)::type>{};
    }))::value);

    static_assert(!decltype(hera::all_of(nonempty_tl, [](auto ident) {
        return std::is_empty<typename decltype(ident)::type>{};
    }))::value);

    static_assert(!decltype(hera::all_of(half_tl, [](auto ident) {
        return std::is_empty<typename decltype(ident)::type>{};
    }))::value);

    static_assert(!decltype(hera::all_of(half_tl, [](auto ident) {
        return std::bool_constant<
            !std::is_empty_v<typename decltype(ident)::type>>{};
    }))::value);
}