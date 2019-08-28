#include <catch2/catch.hpp>

#include <optional>

#include "hera/type_identity.hpp"

constexpr auto deref = [](auto&& x) -> decltype(*std::forward<decltype(x)>(x)) {
    return *std::forward<decltype(x)>(x);
};

TEST_CASE("type_identity")
{
    auto ti = hera::type_identity{5}; // int
    static_assert(hera::same_as<int, typename decltype(ti)::type>);

    auto tf = hera::type_identity{5.0f}; // float

    tf.transform(
        [](auto&& f) { static_assert(hera::same_as<float&&, decltype(f)>); });

    tf.transform(
        [](auto f) { static_assert(hera::same_as<float, decltype(f)>); });

    tf.transform([](const auto& f) {
        static_assert(hera::same_as<const float&, decltype(f)>);
    });

    SECTION("base")
    {
        auto topt = hera::type_identity<std::optional<int>>{};

        auto tint_ref = topt.transform(deref);

        static_assert(hera::same_as<int&&, typename decltype(tint_ref)::type>);
    }

    SECTION("const base")
    {
        auto topt = hera::type_identity<const std::optional<int>>{};

        auto tint_ref = topt.transform(deref);

        static_assert(
            hera::same_as<const int&&, typename decltype(tint_ref)::type>);
    }

    SECTION("reference")
    {
        auto topt_ref = hera::type_identity<std::optional<int>&>{};

        auto t = topt_ref.transform(deref);

        static_assert(hera::same_as<int&, typename decltype(t)::type>);
    }

    SECTION("cref")
    {
        auto topt_ref = hera::type_identity<const std::optional<int>&>{};

        auto t = topt_ref.transform(deref);

        static_assert(hera::same_as<const int&, typename decltype(t)::type>);
    }

    SECTION("rvalue")
    {
        auto topt_ref = hera::type_identity<std::optional<int>&&>{};

        auto t = topt_ref.transform(deref);

        static_assert(hera::same_as<int&&, typename decltype(t)::type>);
    }

    SECTION("crvalue")
    {
        auto topt_ref = hera::type_identity<const std::optional<int>&&>{};

        auto t = topt_ref.transform(deref);

        static_assert(hera::same_as<const int&&, typename decltype(t)::type>);
    }
}