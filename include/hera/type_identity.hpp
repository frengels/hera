#pragma once

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
    constexpr type_identity(U) noexcept
    {}
};

template<hera::metafunction U>
type_identity(U)->type_identity<typename U::type>;

template<typename U>
type_identity(U)->type_identity<U>;

template<typename T>
using type_identity_t = typename type_identity<T>::type;
} // namespace hera