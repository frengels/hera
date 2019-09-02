#pragma once

#include "hera/algorithm/accumulate.hpp"
#include "hera/algorithm/find_if.hpp"
#include "hera/algorithm/unpack.hpp"
#include "hera/container/tuple.hpp"
#include "hera/container/type_list.hpp"
#include "hera/distance.hpp"
#include "hera/get.hpp"
#include "hera/iterator.hpp"
#include "hera/view.hpp"
#include "hera/view/all.hpp"
#include "hera/view/filter.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<typename... Its>
struct zip_iterator_maybe_value_type
{};

template<typename... Its> // clang-format off
    requires
        requires 
        {
            typename std::void_t<iter_value_t<Its>...>;
        } // clang-format on
struct zip_iterator_maybe_value_type<Its...>
{
    using value_type = hera::tuple<iter_value_t<Its>...>;
};

template<typename... Its>
constexpr auto all_bidirectional_iterators(hera::type_list<Its...>)
{
    return std::bool_constant<(hera::bidirectional_iterator<Its> && ...)>{};
};

template<typename... Its>
constexpr auto all_random_access_iterators(hera::type_list<Its...>) noexcept
{
    return std::bool_constant<(hera::random_access_iterator<Its> && ...)>{};
}

template<typename... Its>
constexpr auto any_random_access_iterators(hera::type_list<Its...>) noexcept
{
    return std::bool_constant<(hera::random_access_iterator<Its> || ...)>{};
}

template<typename... Its>
constexpr auto all_dereferenceable(hera::type_list<Its...>) noexcept
{
    return std::bool_constant<(hera::dereferenceable<Its> && ...)>{};
}
} // namespace detail

// zips 2 ranges together, the size() will be of the shortest view
template<forward_range... Vs> // clang-format off
    requires (view<Vs> && ...)
class zip_view : public view_interface<zip_view<Vs...>> { // clang-format on
private:
    template<typename... Its>
    class iterator;

    template<typename... Sents>
    class sentinel;

private:
    [[no_unique_address]] hera::tuple<Vs...> base_;

public:
    constexpr zip_view(Vs... base) noexcept(
        (std::is_nothrow_move_constructible_v<Vs> && ...))
        : base_{std::move(base)...}
    {}

    constexpr hera::tuple<Vs...> base() const
        noexcept(std::is_nothrow_copy_constructible_v<hera::tuple<Vs...>>)
    {
        return base_;
    }

    constexpr auto begin() const noexcept
    {
        return hera::unpack(base_, [](auto&&... bases) {
            return iterator{
                hera::begin(std::forward<decltype(bases)>(bases))...};
        });
    }

    constexpr auto end() const noexcept
    {
        return hera::unpack(base_, [](auto&&... bases) {
            return sentinel{hera::end(std::forward<decltype(bases)>(bases))...};
        });
    }

    friend constexpr auto begin(zip_view&& v) noexcept(noexcept(v.begin()))
    {
        return v.begin();
    }

    friend constexpr auto end(zip_view&& v) noexcept(noexcept(v.end()))
    {
        return v.end();
    }
};

template<forward_range... Rs>
zip_view(Rs&&...)->zip_view<hera::all_view<Rs>...>;

namespace zip_impl
{
constexpr auto less_than = [](auto&& lhs, auto&& rhs) -> decltype(lhs < rhs) {
    return lhs < rhs;
};

constexpr auto less_than_equal =
    [](auto&& lhs, auto&& rhs) -> decltype(lhs <= rhs) { return lhs <= rhs; };

constexpr auto greater_than =
    [](auto&& lhs, auto&& rhs) -> decltype(lhs > rhs) { return lhs > rhs; };

constexpr auto greater_than_equal =
    [](auto&& lhs, auto&& rhs) -> decltype(lhs >= rhs) { return lhs >= rhs; };
} // namespace zip_impl

template<forward_range... Vs>
template<forward_iterator... Its>
class zip_view<Vs...>::iterator
    : public detail::zip_iterator_maybe_value_type<Its...> {
public:
    using difference_type =
        hera::common_type_t<hera::iter_difference_t<Its>...>;

private:
    [[no_unique_address]] hera::tuple<Its...> iterators_;

public:
    constexpr iterator(Its... its) noexcept(
        std::is_nothrow_constructible_v<hera::tuple<Its...>, Its...>)
        : iterators_{std::move(its)...}
    {}

    constexpr hera::tuple<Its...> base() const
        noexcept(std::is_nothrow_copy_constructible_v<hera::tuple<Its...>>)
    {
        return iterators_;
    }

    template<typename... UIts> // clang-format off
        requires sizeof...(Its) == sizeof...(UIts) &&
            (forward_iterator<UIts> && ...)
    constexpr auto operator==(const iterator<UIts...>& other) const noexcept // clang-format on
    {
        return hera::unpack(iterators_, [&](const auto&... its) {
            return hera::unpack(other.iterators_, [&](const auto&... otherits) {
                return std::bool_constant<(
                    decltype(its == otherits)::value&&...)>{};
            });
        });
    }

    template<typename... UIts> // clang-format off
        requires sizeof...(Its) == sizeof...(UIts) &&
            (forward_iterator<UIts> && ...)
    constexpr auto operator!=(const iterator<UIts...>& other) const noexcept // clang-format on
    {
        return hera::unpack(iterators_, [&](const auto&... its) {
            return hera::unpack(other.iterators_, [&](const auto&... otherits) {
                return std::bool_constant<(decltype(its != otherits)::value ||
                                           ...)>{};
            });
        });
    }

    constexpr auto operator++() const
    {
        return hera::unpack(iterators_, [](auto&&... its) {
            return iterator<decltype(++std::forward<decltype(its)>(its))...>{
                ++std::forward<decltype(its)>(its)...};
        });
    };

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires 
            requires(ItList il)
            {
                requires decltype(detail::all_bidirectional_iterators<ItList>())::value
            } // clang-format on
    constexpr auto operator--() const noexcept
    {
        return hera::unpack(iterators_, [](auto&&... its) {
            return iterator<decltype(--std::forward<decltype(its)>(its))...>{
                --std::forward<decltype(its)>(its)...};
        });
    }

    template<hera::constant_convertible_to<difference_type> C,
             typename ItList = hera::type_list<Its...>> // clang-format off
        requires 
            requires(ItList il) 
            {
                requires decltype(detail::all_random_access_iterators(il))::value;
            } // clang-format on
    constexpr auto operator+(C n) const noexcept
    {
        return hera::unpack(iterators_, [&](auto&&... its) {
            return iterator<decltype(std::forward<decltype(its)>(its) + n)...>{
                (std::forward<decltype(its)>(its) + n)...};
        });
    }

    template<hera::constant_convertible_to<difference_type> C,
             typename ItList = hera::type_list<Its...>> // clang-format off
        requires 
            requires(ItList il)
            {
                requires decltype(detail::all_random_access_iterators(il))::value;
            } // clang-format on
    friend constexpr auto operator+(C n, const iterator& it) noexcept
    {
        return hera::unpack(it.iterators_, [&](auto&&... its) {
            return iterator<decltype(n + std::forward<decltype(its)>(its))...>{
                (n + std::forward<decltype(its)>(its))...};
        });
    }

    template<hera::constant_convertible_to<difference_type> C,
             typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList il)
            {
                requires decltype(detail::all_random_access_iterators(il))::value;
            } // clang-format on
    constexpr auto operator-(C n) const noexcept
    {
        return hera::unpack(iterators_, [&](auto&&... its) {
            return iterator<decltype(std::forward<decltype(its)>(its) - n)...>{
                (std::forward<decltype(its)>(its) - n)...};
        });
    }

private:
    template<typename BinaryOp, typename... UIts> // clang-format off
        requires sizeof...(Its) == sizeof...(UIts) &&
            requires(hera::type_list<UIts> tl)
            {
                // only one iterator needs to be random access
                // for ordering.
                requires decltype(detail::any_random_access_iterators(tl))::value;
            } // clang-format on
    constexpr auto generic_comparison(const iterator<UIts...>& other,
                                      BinaryOp&&               op) const
    {
        auto random_access_its = hera::filter_view{
            iterators_, [](const auto& it) {
                if constexpr (hera::random_access_iterator<
                                  std::remove_cvref_t<decltype(it)>>)
                {
                    return std::true_type{};
                }
                else
                {
                    return std::false_type{};
                }
            }};

        auto first_ra_it = hera::begin(random_access_its).base();

        auto offset_constant =
            hera::distance(hera::begin(iterators_), first_ra_it);

        return std::forward<BinaryOp>(op)(
            first_ra_it,
            hera::next(hera::begin(other.iterators_), offset_constant));
    }

public:
    template<typename... UIts>
    constexpr auto operator<(const iterator<UIts...>& other) const
        -> decltype(generic_comparison(other, zip_impl::less_than))
    {
        return generic_comparison(other, zip_impl::less_than);
    }

    template<typename... UIts>
    constexpr auto operator<=(const iterator<UIts...>& other) const
        -> decltype(generic_comparison(other, zip_impl::less_than_equal))
    {
        return generic_comparison(other, zip_impl::less_than_equal);
    }

    template<typename... UIts>
    constexpr auto operator>(const iterator<UIts...>& other) const
        -> decltype(generic_comparison(other, zip_impl::greater_than))
    {
        return generic_comparison(other, zip_impl::greater_than);
    }

    template<typename... UIts>
    constexpr auto operator>=(const iterator<UIts...>& other) const
        -> decltype(generic_comparison(other, zip_impl::greater_than_equal))
    {
        return generic_comparison(other, zip_impl::greater_than_equal);
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList il)
            {
                requires decltype(detail::all_dereferenceable(il))::value;
            } // clang-format on
    constexpr auto operator*() const
    {
        return hera::unpack(iterators_, [](auto&&... its) {
            return hera::tuple<decltype(*std::forward<decltype(its)>(its))...>{
                *std::forward<decltype(its)>(its)...};
        });
    }
};

namespace detail
{
// extremely simple abs function only to be used in the context of zip's
// iterator/sentinel
template<typename T>
constexpr auto zip_abs(T t) noexcept
{
    if (t < 0)
    {
        // prevent positive overflows
        if (t == std::numeric_limits<T>::min())
        {
            ++t;
        }
        return -t;
    }
    else
    {
        return t;
    }
}

// used in op- functions
auto nearest_0_constant = [](auto lhs, auto rhs) {
    // make sure both have the same signedness
    static_assert(
        ((decltype(lhs)::value <= 0) && (decltype(rhs)::value <= 0)) ||
            ((decltype(lhs)::value >= 0) && (decltype(rhs)::value >= 0)),
        "Something went horribly wrong");

    constexpr auto lhs_abs = detail::zip_abs(decltype(lhs)::value);
    constexpr auto rhs_abs = detail::zip_abs(decltype(rhs)::value);

    constexpr auto smallest = std::min(lhs_abs, rhs_abs);

    if constexpr (smallest == lhs_abs)
    {
        return lhs;
    }
    else
    {
        return rhs;
    }
};

} // namespace detail

template<forward_range... Vs>
template<typename... Sents>
class zip_view<Vs...>::sentinel {
private:
    [[no_unique_address]] hera::tuple<Sents...> sentinels_;

public:
    constexpr sentinel(Sents... sents) noexcept(
        std::is_nothrow_constructible_v<hera::tuple<Sents...>, Sents...>)
        : sentinels_{std::move(sents)...}
    {}

    constexpr hera::tuple<Sents...> base() const
        noexcept(std::is_nothrow_copy_constructible_v<hera::tuple<Sents...>>)
    {
        return sentinels_;
    }

    template<typename... Its> // clang-format off
        requires sizeof...(Its) == sizeof...(Sents)
    constexpr auto operator==(const iterator<Its...>& it) const noexcept // clang-format on
    {
        // add to constraint once it stops complaining about mismatched argument
        // pack lengths.
        // static_assert((sentinel_for<Sents, Its> && ...),
        //              "Not valid sentinel for passed iterator");

        // if any are equal to the end then we're at the end.
        // now if we iterate past the end and another sentinel which happens to
        // be further along evaluates to true, then so be it, this is UB anyway.
        return hera::unpack(sentinels_, [&](const auto&... sents) {
            return hera::unpack(it.iterators_, [&](const auto&... its) {
                return std::bool_constant<(decltype(sents == its)::value ||
                                           ...)>{};
            });
        });
    }

    template<typename... Its> // clang-format off
        requires sizeof...(Its) == sizeof...(Sents)
    friend constexpr auto operator==(const iterator<Its...>& it, const sentinel& sent) noexcept // clang-format on
    {
        return hera::unpack(it.iterators_, [&](const auto&... its) {
            return hera::unpack(sent.sentinels_, [&](const auto&... sents) {
                return std::bool_constant<(decltype(its == sents)::value ||
                                           ...)>{};
            });
        });
    }

    template<typename... Its> // clang-format off
        requires (sizeof...(Its) == sizeof...(Sents))
    constexpr auto operator!=(const iterator<Its...>& it) const noexcept // clang-format on
    {
        // add the sentinel_for constraint once gcc accepts it

        // all must be unequal, otherwise we might be at the end of the shortest
        // stick.
        return hera::unpack(sentinels_, [&](const auto&... sents) {
            return hera::unpack(it.iterators_, [&](const auto&... its) {
                return std::bool_constant<(
                    decltype(sents != its)::value&&...)>{};
            });
        });
    }

    template<typename... Its> // clang-format off
        requires sizeof...(Its) == sizeof...(Sents)
    friend constexpr auto operator!=(const iterator<Its...>& it, const sentinel& sent) noexcept // clang-format on
    {
        return hera::unpack(it.iterators_, [&](const auto&... its) {
            return hera::unpack(sent.sentinels_, [&](const auto&... sents) {
                return std::bool_constant<(
                    decltype(its != sents)::value&&...)>{};
            });
        });
    }

    template<typename... Its> // clang-format off
        requires sizeof...(Its) == sizeof...(Sents)
    constexpr auto operator-(const iterator<Its...>& it) const noexcept // clang-format on
    {
        // move to constraint once gcc stops bugging around. This requirement is
        // necessary because otherwise we might get the size difference for one
        // which is not the shortest stick in the zip.
        static_assert((sized_sentinel_for<Sents, Its> && ...),
                      "All sentinels must model sized_sentinel_for");

        using difference_type = iter_difference_t<iterator<Its...>>;

        // all the results from performing op- as integral_constants
        auto size_results = hera::unpack(sentinels_, [&](const auto&... sents) {
            return hera::unpack(it.iterators_, [&](const auto&... its) {
                return hera::tuple{
                    std::integral_constant<difference_type,
                                           decltype(sents - its)::value>{}...};
            });
        });

        constexpr difference_type init = [&]() {
            if constexpr (std::remove_cvref_t<decltype(
                              hera::get<0>(size_results))>::value < 0)
            {
                return std::numeric_limits<difference_type>::min();
            }
            else
            {
                return std::numeric_limits<difference_type>::max();
            }
        }();

        return std::integral_constant<
            difference_type,
            hera::accumulate(size_results,
                             std::integral_constant<difference_type, init>{},
                             detail::nearest_0_constant)>{};
    }

    template<typename... Its> // clang-format off
        requires sizeof...(Its) == sizeof...(Sents)
    friend constexpr auto operator-(const iterator<Its...>& it, const sentinel& sent) noexcept // clang-format on
    {
        static_assert((sized_sentinel_for<Sents, Its> && ...),
                      "All sentinels must model sized_sentinel_for");

        using difference_type = iter_difference_t<iterator<Its...>>;

        auto size_results =
            hera::unpack(it.iterators_, [&](const auto&... its) {
                return hera::unpack(sent.sentinels_, [&](const auto&... sents) {
                    return hera::tuple{std::integral_constant<
                        difference_type,
                        decltype(its - sents)::value>{}...};
                });
            });

        constexpr difference_type init = [&]() {
            if constexpr (std::remove_cvref_t<decltype(
                              hera::get<0>(size_results))>::value < 0)
            {
                return std::numeric_limits<difference_type>::min();
            }
            else
            {
                return std::numeric_limits<difference_type>::max();
            }
        }();

        return std::integral_constant<
            difference_type,
            hera::accumulate(size_results,
                             std::integral_constant<difference_type, init>{},
                             detail::nearest_0_constant)>{};
    }
}; // namespace hera
} // namespace hera