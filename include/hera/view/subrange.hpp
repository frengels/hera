#pragma once

#include <type_traits>
#include <utility>

#include "hera/iterator.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<hera::heterogeneous_iterator I, hera::sentinel_for<I> S>
class subrange : public view_interface<subrange<I, S>> {
private:
    [[no_unique_address]] I begin_;
    [[no_unique_address]] S end_;

public:
    constexpr subrange(I i, S s) noexcept(
        std::is_nothrow_move_constructible_v<I>&&
            std::is_nothrow_move_constructible_v<S>)
        : begin_{std::move(i)}, end_{std::move(s)}
    {}

    // TODO pair constructor

    constexpr I begin() const noexcept(std::is_nothrow_copy_constructible_v<I>)
    {
        return begin_;
    }

    constexpr S end() const noexcept(std::is_nothrow_copy_constructible_v<I>)
    {
        return end_;
    }

    friend constexpr I begin(subrange sr) noexcept(noexcept(sr.begin()))
    {
        return sr.begin();
    }

    friend constexpr S end(subrange sr) noexcept(noexcept(sr.end()))
    {
        return sr.end();
    }
};
} // namespace hera

namespace std
{
template<typename I, typename S>
struct tuple_size<hera::subrange<I, S>> : std::integral_constant<std::size_t, 2>
{};

template<typename I, typename S>
struct tuple_element<0, hera::subrange<I, S>>
{
    using type = I;
};

template<typename I, typename S>
struct tuple_element<1, hera::subrange<I, S>>
{
    using type = S;
};
} // namespace std