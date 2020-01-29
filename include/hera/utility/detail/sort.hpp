#pragma once

#include <array>

// includes functions required to sort get compile time.
// stl sorting algorithms are currently not yet marked constexpr.

namespace hera
{
namespace detail
{
template<typename T>
constexpr void swap(T& lhs,
                    T& rhs) noexcept(std::is_nothrow_move_assignable_v<T>)
{
    T tmp = std::move(lhs);
    lhs   = std::move(rhs);
    rhs   = std::move(tmp);
}

template<typename ForwardIt>
constexpr void iter_swap(ForwardIt lhs, ForwardIt rhs)
{
    ::hera::detail::swap(*lhs, *rhs);
}

template<typename ForwardIt>
constexpr ForwardIt
rotate(ForwardIt first, ForwardIt middle, ForwardIt last) noexcept
{
    if (first == middle)
        return last;
    if (middle == last)
        return first;

    ForwardIt read      = middle;
    ForwardIt write     = first;
    ForwardIt next_read = first;

    while (read != last)
    {
        if (write == next_read)
        {
            next_read = read;
        }
        hera::detail::iter_swap(write++, read++);
    }

    hera::detail::rotate(write, next_read, last);
    return write;
}

template<typename ForwardIt, typename T, typename Compare>
constexpr ForwardIt upper_bound(ForwardIt first,
                                ForwardIt last,
                                const T&  value,
                                Compare   comp) noexcept
{
    ForwardIt it    = first;
    auto      count = last - first;

    while (count > 0)
    {
        it        = first;
        auto step = count / 2;
        it += step;

        if (!comp(value, *it))
        {
            first = ++it;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    return first;
}

template<typename ForwardIt, typename Compare>
constexpr void insertion_sort(ForwardIt first, ForwardIt last, Compare comp)
{
    for (auto i = first; i != last; ++i)
    {
        hera::detail::rotate(
            hera::detail::upper_bound(first, i, *i, comp), i, i + 1);
    }
}

// a helper function to aid in sorting a constexpr array
template<typename T, std::size_t N, typename Compare>
constexpr std::array<T, N> sort_array(std::array<T, N> arr,
                                      Compare          comp) noexcept
{
    insertion_sort(arr.begin(), arr.end(), std::move(comp));
    return arr;
}
} // namespace detail
} // namespace hera