#include <catch2/catch.hpp>

#include <optional>

#include "hera/type_.hpp"

constexpr auto deref = [](auto&& x) -> decltype(*std::forward<decltype(x)>(x)) {
    return *std::forward<decltype(x)>(x);
};

TEST_CASE("type_")
{
    auto ti = hera::type_<int>{};
    static_assert(hera::same_as<int, typename decltype(ti)::type>);

    static_assert(hera::metafunction<decltype(ti)>);
    static_assert(hera::metafunction<std::integral_constant<int, 0>>);

    auto tf = hera::type_<float>{}; // float

    // typeid passes metafunctions through
    auto typeid_tf = hera::typeid_(tf);

    static_assert(decltype(typeid_tf == tf)::value);

    tf.transform(
        [](auto&& f) { static_assert(hera::same_as<float&&, decltype(f)>); });

    tf.transform(
        [](auto f) { static_assert(hera::same_as<float, decltype(f)>); });

    tf.transform([](const auto& f) {
        static_assert(hera::same_as<const float&, decltype(f)>);
    });

    SECTION("base")
    {
        auto topt = hera::type_<std::optional<int>>{};

        auto tint_ref = topt.transform(deref);

        static_assert(hera::same_as<int&&, typename decltype(tint_ref)::type>);
    }

    SECTION("const base")
    {
        auto topt = hera::type_<const std::optional<int>>{};

        auto tint_ref = topt.transform(deref);

        static_assert(
            hera::same_as<const int&&, typename decltype(tint_ref)::type>);
    }

    SECTION("reference")
    {
        auto topt_ref = hera::type_<std::optional<int>&>{};

        auto t = topt_ref.transform(deref);

        static_assert(hera::same_as<int&, typename decltype(t)::type>);
    }

    SECTION("cref")
    {
        auto topt_ref = hera::type_<const std::optional<int>&>{};

        auto t = topt_ref.transform(deref);

        static_assert(hera::same_as<const int&, typename decltype(t)::type>);
    }

    SECTION("rvalue")
    {
        auto topt_ref = hera::type_<std::optional<int>&&>{};

        auto t = topt_ref.transform(deref);

        static_assert(hera::same_as<int&&, typename decltype(t)::type>);
    }

    SECTION("crvalue")
    {
        auto topt_ref = hera::type_<const std::optional<int>&&>{};

        auto t = topt_ref.transform(deref);

        static_assert(hera::same_as<const int&&, typename decltype(t)::type>);
    }
}

TEST_CASE("forward_type_")
{
    auto i     = hera::type_<int&>{};
    auto fwd_i = hera::forward_type{5};
    auto ref_i = hera::forward_type{i};

    static_assert(hera::same_as<int&, typename decltype(ref_i)::type>);

    static_assert(hera::same_as<int&&, typename decltype(fwd_i)::type>);

    auto fwd_opt = hera::forward_type{std::optional<int>{5}};

    static_assert(
        hera::same_as<std::optional<int>&&, typename decltype(fwd_opt)::type>);
}