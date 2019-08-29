#pragma once

#include <functional>

#include "hera/concepts.hpp"
#include "hera/metafunction.hpp"

namespace hera
{
template<typename T>
struct type_identity
{
    using type = T;

    type_identity() = default;

    template<hera::metafunction Meta>
    constexpr auto operator==(Meta) const noexcept
    {
        return std::bool_constant<hera::same_as<T, typename Meta::type>>{};
    }

    template<hera::metafunction Meta>
    friend constexpr auto operator==(Meta, type_identity) noexcept
    {
        return std::bool_constant<hera::same_as<typename Meta::type, T>>{};
    }

    template<hera::metafunction Meta>
    constexpr auto operator!=(Meta) const noexcept
    {
        return std::bool_constant<!hera::same_as<T, typename Meta::type>>{};
    }

    template<hera::metafunction Meta>
    friend constexpr auto operator!=(Meta, type_identity) noexcept
    {
        return std::bool_constant<!hera::same_as<typename Meta::type, T>>{};
    }

    template<hera::invocable<T> F>
    constexpr auto transform(F&& fn) const noexcept // clang-format on
    {
        return type_identity<std::invoke_result_t<F, T>>{};
    }
};

template<hera::metafunction M>
constexpr type_identity<typename M::type> make_type_identity_mf(M) noexcept
{
    return {};
}

template<typename T>
using type_identity_t = typename type_identity<T>::type;

template<typename T>
struct forward_type : type_identity<T>
{
    forward_type() = default;

    constexpr forward_type(type_identity<T>) noexcept
    {}

    template<typename U>
    constexpr forward_type(U&&) noexcept
    {}
};

template<typename U>
forward_type(U &&)->forward_type<U&&>;

template<typename T>
constexpr hera::type_identity<T> typeid_(T) noexcept
{
    return {};
}

template<hera::metafunction M>
constexpr hera::type_identity<typename M::type> typeid_(M) noexcept
{
    return {};
}
} // namespace hera