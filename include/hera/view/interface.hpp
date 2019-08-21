#pragma once

#include <type_traits>

#include "hera/empty.hpp"
#include "hera/next_prev.hpp"
#include "hera/ranges.hpp"

namespace hera
{
struct view_base
{};

namespace detail
{
template<typename D, typename C>
concept can_bracket_for = // clang-format off
    requires(D& d, C n)
    {
        { hera::begin(d)[n] } -> can_reference;
    }; // clang-format on
} // namespace detail

template<typename D>
requires std::is_class_v<D>&&
    same_as<D, std::remove_cv_t<D>> class view_interface : public view_base {
private:
    constexpr D& derived() noexcept
    {
        static_assert(derived_from<D, view_interface>);
        return static_cast<D&>(*this);
    }

    constexpr const D& derived() const noexcept
    {
        static_assert(derived_from<D, view_interface>);
        return static_cast<const D&>(*this);
    }

public:
    template<forward_range R = D>
    constexpr auto
    empty() noexcept(noexcept(hera::begin(derived()) == hera::end(derived())))
    {
        return hera::begin(derived()) == hera::end(derived());
    }

    template<forward_range R = const D>
    constexpr auto empty() const
        noexcept(noexcept(hera::begin(derived() == hera::end(derived()))))
    {
        return hera::begin(derived()) == hera::end(derived());
    }

    template<typename R = D> // clang-format off
        requires requires(R& r)
        {
            hera::empty(r);
        } // clang-format on
    constexpr explicit operator bool() noexcept(noexcept(
        std::bool_constant<!decltype(hera::empty(derived()))::value>{}))
    {
        return std::bool_constant<!decltype(hera::empty(derived()))::value>{};
    }

    template<typename R = const D> // clang-format off
        requires requires(R& r)
        {
            hera::empty(r);
        } // clang-format on
    constexpr explicit operator bool() const noexcept(noexcept(
        std::bool_constant<!decltype(hera::empty(derived()))::value>{}))
    {
        return std::bool_constant<!decltype(hera::empty(derived()))::value>{};
    }

    template<forward_range R = D> // clang-format off
        requires sized_sentinel_for<sentinel_t<R>, iterator_t<R>> 
    constexpr auto // clang-format on
    size() noexcept(noexcept(hera::end(derived()) - hera::begin(derived())))
    {
        return hera::end(derived()) - hera::begin(derived());
    }

    template<forward_range R = const D> // clang-format off
        requires sized_sentinel_for<sentinel_t<R>, iterator_t<R>>
    constexpr auto size() const // clang-format on 
        noexcept(noexcept(hera::end(derived()) - hera::begin(derived())))
    {
        return hera::end(derived()) - hera::begin(derived());
    }
    

    template<typename R = D> // clang-format off
        requires forward_range<R> && readable<iterator_t<R>> // clang-format on 
    constexpr decltype(auto) front() noexcept(noexcept(*hera::begin(derived())))
    {
        return *hera::begin(derived());
    }

    template<forward_range R = const D> // clang-format off
        requires readable<iterator_t<R>> // clang-format on 
    constexpr decltype(auto) front() const
        noexcept(noexcept(*hera::begin(derived())))
    {
        return *hera::begin(derived());
    }

    template<bidirectional_range R = D> // clang-format off
        requires bidirectional_iterator<sentinel_t<R>>&& readable<
            std::invoke_result_t<decltype(hera::prev),
                                 sentinel_t<R>>> // clang-format on 
    constexpr decltype(auto) back() 
        noexcept(noexcept(*(hera::prev(hera::end(derived())))))
    {
        return *(hera::prev(hera::end(derived())));
    }
    

    template<bidirectional_range R = const D> // clang-format off
        requires bidirectional_iterator<sentinel_t<R>>&& readable<
            std::invoke_result_t<decltype(hera::prev),
                                 sentinel_t<R>>> // clang-format on 
    constexpr decltype(auto) back() const
        noexcept(noexcept(*(hera::prev(hera::end(derived())))))
    {
        return *(hera::prev(hera::end(derived())));
    }

    template<forward_range R = D> // clang-format off
        requires !bidirectional_iterator<sentinel_t<R>> &&
            bidirectional_iterator<iterator_t<R>> &&
            sized_range<R> &&
            requires(R& r)
            {
                requires 
                    readable<decltype(hera::prev(
                        hera::next(hera::begin(r),
                                   hera::end(r))))>;
            } // clang-format on
    constexpr decltype(auto) back() noexcept(noexcept(
        *hera::prev(hera::next(hera::begin(derived()), hera::end(derived())))))
    {
        return *hera::prev(
            hera::next(hera::begin(derived()), hera::end(derived())));
    }

    template<forward_range R = const D> // clang-format off
        requires !bidirectional_iterator<sentinel_t<R>> &&
            bidirectional_iterator<iterator_t<R>> &&
            sized_range<R> &&
            requires(R& r)
            {
                requires
                    readable<decltype(hera::prev(
                        hera::next(hera::begin(r), 
                                   hera::end(r))))>;
            } // clang-format on
    constexpr decltype(auto) back() const
        noexcept(noexcept(*hera::prev(hera::next(hera::begin(derived()),
                                                 hera::end(derived())))))
    {
        return *hera::prev(
            hera::next(hera::begin(derived()), hera::end(derived())));
    }

    /*
    template<hera::constant_convertible_to<range_difference_t<D>>
C, typename R = D> // clang-format off requires requires(R& r, C idx)
        {
            hera::begin(r.derived())[idx];
        }
    constexpr decltype(auto) // clang-format on
    operator[](C idx) noexcept(noexcept(hera::begin(derived())[idx]))

    {
        return hera::begin(derived())[idx];
    }
    * /
    */

    /*
        template<hera::constant_convertible_to<range_difference_t<D>> C,
                 detail::can_bracket_for<C>                           R = const
       D> constexpr decltype(auto) operator[](C idx) const
            noexcept(noexcept(hera::begin(derived())[idx]))
        {
            return hera::begin(derived())[idx];
        }
        */
};
} // namespace hera