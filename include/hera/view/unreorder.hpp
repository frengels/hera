#pragma once

#include "hera/algorithm/accumulate.hpp"
#include "hera/algorithm/for_each.hpp"
#include "hera/algorithm/unpack.hpp"
#include "hera/container/integer_sequence.hpp"
#include "hera/ranges.hpp"
#include "hera/view/all.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/enumerate.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
/// sum all numbers from 1 + 2 + ... + num
/// This function is technically not safe from overflow, but since this function
/// is only used with templated, I doubt any templates will go deep enough to
/// hit this limit.
constexpr std::size_t sum_to(std::size_t num) noexcept
{
    /*
    auto res = num;
    while (--num)
    {
        res += num;
    }

    return res;
    */

    auto res = num;

    for (auto i = 0; i != num; ++i)
    {
        res += i;
    }

    return res;
}

template<std::size_t... Is>
constexpr std::size_t sum_all(hera::index_sequence<Is...> seq) noexcept
{
    return hera::accumulate(
        seq, std::size_t{}, [](std::size_t current_sum, auto index) {
            return current_sum + decltype(index)::value;
        });
}
} // namespace detail

// Takes a given sequence of indices, and will undo any reordering which might
// have been performed by these indices, effectively undoing a reorder_view's
// operation
template<hera::range V, std::size_t... Is> // clang-format off
    requires hera::view<V> && (!hera::range_out_of_bounds<V, Is> && ...)
class unreorder_view : public hera::view_interface<unreorder_view<V, Is...>> // clang-format on
{
    static constexpr auto expected_sum = detail::sum_to(sizeof...(Is) - 1);
    static constexpr auto actual_sum =
        detail::sum_all(hera::index_sequence<Is...>{});
    static_assert(expected_sum == actual_sum,
                  "All possible indices must occur, too much information was "
                  "lost to undo reorder");

private:
    [[no_unique_address]] V base_;

public:
    /// the sequence we are trying to undo
    static constexpr hera::index_sequence<Is...> sequence{};
    /// the sequence used to undo the sequence indicated by Is...
    static constexpr auto unsequence = []() {
        // array we will fill with corrected sequence
        auto seq_arr = std::array<std::size_t, sizeof...(Is)>{};

        constexpr auto enumerated_seq = hera::enumerate_view{sequence};

        // obtain a constexpr array containing the new indices
        constexpr auto unseq_arr = [&]() {
            auto unseq_arr = std::array<std::size_t, sizeof...(Is)>{};

            hera::for_each(enumerated_seq, [&](auto enumerated_index) {
                auto i         = enumerated_index.first;
                auto seq_index = enumerated_index.second;

                unseq_arr[seq_index] = i;
            });

            return unseq_arr;
        }();

        // create an index sequence to iterator our array, using hera algorithms
        // would make use lose constexpr context
        auto iter_seq = hera::make_index_sequence<unseq_arr.size()>{};

        return hera::unpack(iter_seq, [&](auto... is) {
            return hera::index_sequence<unseq_arr[decltype(is)::value]...>{};
        });
    }();

public:
    constexpr unreorder_view(V base) : base_{std::move(base)}
    {}

    constexpr unreorder_view(V base, hera::index_sequence<Is...>)
        : unreorder_view(std::move(base))
    {}

    constexpr V base() const
    {
        return base_;
    }

    constexpr auto size() const
    {
        return hera::size(sequence);
    }

    template<std::size_t I>
    constexpr auto try_get() const noexcept
    {
        if constexpr (I < sizeof...(Is))
        {
            auto                  pos_const = hera::get<I>(unsequence);
            constexpr std::size_t pos       = pos_const;

            return hera::try_get<pos>(base_);
        }
        else
        {
            return hera::none{};
        }
    }
};

template<hera::range R, std::size_t... Is>
reorder_view(R&&, hera::index_sequence<Is...>)
    ->reorder_view<hera::all_view<R>, Is...>;

namespace views
{
struct unreorder_fn
{
    template<hera::range R, std::size_t... Is> // clang-format off
        requires hera::viewable_range<R>
    constexpr auto operator()(R&& range, hera::index_sequence<Is...> indices) // clang-format on
        const -> decltype(hera::unreorder_view{std::forward<R>(range),
                                               std::move(indices)})
    {
        return hera::unreorder_view{std::forward<R>(range), std::move(indices)};
    }

    template<std::size_t... Is>
    constexpr auto operator()(hera::index_sequence<Is...> indices) const
    {
        return detail::view_closure{*this, std::move(indices)};
    }
};

inline constexpr auto unreorder = unreorder_fn{};
} // namespace views
} // namespace hera