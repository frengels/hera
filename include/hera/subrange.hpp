#pragma once

#include <type_traits>
#include <utility>

#include "hera/iterator.hpp"
#include "hera/sentinel.hpp"
#include "hera/view_interface.hpp"

namespace hera
{
namespace detail
{
struct no_size
{};
} // namespace detail

enum class subrange_kind : bool
{
    unsized,
    sized
};

template<hera::heterogeneous_iterator I,
         typename S,
         subrange_kind K> // TODO constraints
class subrange {
private:
    static constexpr bool store_size =
        K == subrange_kind::sized && !sized_sentinel_for<S, I>;

private:
    [[no_unique_address]] I begin_{};
    [[no_unique_address]] S end_{};
    [[no_unique_address]] std::conditional_t<
        store_size,
        std::make_unsigned_t<iter_difference_t<I>>,
        detail::no_size>
        size_;

public:
    subrange() = default;

    constexpr subrange(I i, S s) noexcept(
        std::is_nothrow_move_constructible_v<I>&&
            std::is_nothrow_move_constructible_v<S>)
        : begin_{std::move(i)}, end_{std::move(s)}
    {}

    // TODO pair constructor

    constexpr I begin() const
    {
        return begin_;
    }

    constexpr S end() const
    {
        return end_;
    }

    friend constexpr begin(subrange&& sr) noexcept
    {
        return r.begin();
    }

    friend constexpr end(subrange&& sr) noexcept
    {
        return r.end();
    }
};
} // namespace hera

namespace std
{
template<typename I, typename S, hera::subrange_kind K>
struct tuple_size<hera::subrange<I, S, K>>
    std::integral_constant<std::size_t, 2>
{};

template<typename I, typename S, hera::subrange_kind K>
struct tuple_element<0, hera::subrange<I, S, K>>
{
    using type = I;
};

template<typename I, typename S, hera::subrange_kind K>
struct tuple_element<1, hera::subrange<I, S, K>>
{
    using type = S;
};
} // namespace std