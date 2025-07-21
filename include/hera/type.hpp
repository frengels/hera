#pragma once

#include <functional>

#include "hera/concepts.hpp"
#include "hera/metafunction.hpp"

namespace hera
{
template<typename T>
struct type_
{
    using type = T;

    type_() = default;

    template<typename U>
    constexpr auto operator==(hera::type_<U>) const noexcept
    {
        return std::bool_constant<hera::same_as<T, U>>{};
    }

    template<typename U>
    constexpr auto operator!=(hera::type_<U>) const noexcept
    {
        return std::bool_constant<!hera::same_as<T, U>>{};
    }

    template<hera::invocable<T> F>
    static constexpr auto transform(F&& fn) noexcept // clang-format on
    {
        return type_<std::invoke_result_t<F, T>>{};
    }

    static constexpr bool contains_metafunction() noexcept
    {
        return hera::metafunction<T>;
    }

    static constexpr T unpack() noexcept
        requires(hera::metafunction<T>)
    {
        return {};
    }

    static constexpr std::type_identity<T> as_type_identity() noexcept
    {
        return {};
    }

    static constexpr std::in_place_type_t<T> as_in_place_type() noexcept
    {
        return {};
    }
};

template<hera::metafunction M>
constexpr type_<typename M::type> make_type__mf(M) noexcept
{
    return {};
}

template<typename T>
using type__t = typename type_<T>::type;

template<typename T>
struct forward_type : type_<T>
{
    forward_type() = default;

    constexpr forward_type(type_<T>) noexcept
    {}

    template<typename U>
    constexpr forward_type(U&&) noexcept
    {}
};

template<typename U>
forward_type(U&&) -> forward_type<U&&>;

// deduce the base type of the passed object and put it into a type_
template<typename T>
    requires(!hera::metafunction<std::remove_cvref_t<T>>)
constexpr hera::type_<std::remove_cvref_t<T>> typeid_(T&&) noexcept
{
    return {};
}

// simply forward metafunctions into a type_
template<typename M>
    requires(hera::metafunction<std::remove_cvref_t<M>>)
constexpr hera::type_<typename M::type> typeid_(M) noexcept
{
    return {};
}
} // namespace hera