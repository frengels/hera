#pragma once

#include <cstddef>
#include <type_traits>

#include "hera/concepts.hpp"
#include "hera/constant.hpp"

namespace hera
{
// a special marker type to indicate the size is infinite
struct infinite
{};

// bounded ranges are any integral constant convertible to size_t
template<typename B>
concept bounded = constant_convertible_to<B, std::size_t>;

// unbounded must be indicated with the type unreachable
template<typename B>
concept unbounded = hera::same_as<B, infinite>;

// check whether B indicates a bound
template<typename B>
concept bounds = bounded<B> || unbounded<B>;
} // namespace hera