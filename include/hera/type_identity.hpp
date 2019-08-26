#pragma once

#include "hera/concepts.hpp"
#include "hera/metafunction.hpp"

namespace hera
{
template<typename T>
struct type_identity
{
    using type = T;

    type_identity() = default;

    template<hera::metafunction U>
    constexpr type_identity(U) noexcept
    {}

    template<typename U>
    constexpr type_identity(U&&) noexcept
    {}

    template<typename U>
    constexpr auto operator==(type_identity<U>) const noexcept
    {
        return std::bool_constant<hera::same_as<T, U>>{};
    }

    template<typename U>
    constexpr auto operator!=(type_identity<U>) const noexcept
    {
        return std::bool_constant<!hera::same_as<T, U>>{};
    }
};

template<hera::metafunction MF>
type_identity(MF)->type_identity<typename MF::type>;

template<typename U>
type_identity(U &&)->type_identity<std::decay_t<U>>;

template<typename T>
using type_identity_t = typename type_identity<T>::type;
} // namespace hera