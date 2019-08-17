#pragma once

#include <functional>
#include <type_traits>
#include <utility>

#include "hera/common_reference.hpp"

namespace hera
{
template<typename T, typename U>
concept same_as = std::is_same_v<T, U>&& std::is_same_v<U, T>;

template<typename From, typename To>
concept convertible_to = std::is_convertible_v<From, To>&& requires(From (&f)())
{
    static_cast<To>(f());
};

template<typename T>
concept destructible = std::is_nothrow_destructible_v<T>;

template<typename T, typename... Args>
concept constructible_from =
    destructible<T>&& std::is_constructible_v<T, Args...>;

template<typename T>
concept default_constructible = constructible_from<T>;

template<typename T>
concept move_constructible = constructible_from<T, T>&& convertible_to<T, T>;

template<typename T>
concept copy_constructible =
    move_constructible<T>&& constructible_from<T, T&>&& convertible_to<T&, T>&&
        constructible_from<T, const T&>&& convertible_to<const T&, T>&&
            constructible_from<T, const T>&& convertible_to<const T, T>;

template<typename T, typename U>
concept common_reference_with =
    same_as<common_reference_t<T, U>, common_reference_t<U, T>>&&
            convertible_to<T, common_reference_t<T, U>>&&
            convertible_to<U, common_reference_t<T, U>>;

template<typename LHS, typename RHS>
concept assignable_from = std::is_lvalue_reference_v<LHS>&&
    common_reference_with<
        const std::remove_reference_t<LHS>&,
        const std::remove_reference_t<RHS>&>&& // clang-format off
    requires(LHS lhs, RHS&& rhs)
    {
        { lhs = static_cast<RHS&&>(rhs) } -> same_as<LHS>;
    }; // clang-format on

template<typename T>
concept movable = // swappable implied from move_constructible + assignable_from
    std::is_object_v<T>&& move_constructible<T>&& assignable_from<T&, T>;

template<typename B>
concept boolean = movable<std::remove_cvref_t<B>>&& // clang-format off
    requires(const std::remove_reference_t<B>& b1, 
        const std::remove_reference_t<B>& b2, const bool a) 
    {
        { b1 } -> convertible_to<bool>;
        { !b1 } -> convertible_to<bool>;
        { b1 && b2 } -> same_as<bool>;
        { b1 && a } -> same_as<bool>;
        { a && b2 } -> same_as<bool>;
        { b1 || b2 } -> same_as<bool>;
        { b1 || a } -> same_as<bool>;
        { a || b2 } -> same_as<bool>;
        { b1 == b2 } -> same_as<bool>;
        { b1 == a } -> same_as<bool>;
        { a == b2 } -> same_as<bool>;
        { b1 != b2 } -> same_as<bool>;
        { b1 != a } -> same_as<bool>;
        { a != b2 } -> same_as<bool>;
};
// clang-format on

template<typename T>
concept copyable =
    copy_constructible<T>&& movable<T>&& assignable_from<T&, const T&>;

template<typename T>
concept semiregular = copyable<T>&& default_constructible<T>;

namespace detail
{
template<typename T, typename U>
concept weakly_equality_comparable_with =
    requires(const std::remove_reference_t<T>& t,
             const std::remove_reference_t<U>& u) // clang-format off
{
    { t == u } -> boolean;
    { t != u } -> boolean;
    { u == t } -> boolean;
    { u != t } -> boolean;
}; // clang-format on
} // namespace detail

template<typename T>
concept equality_comparable = detail::weakly_equality_comparable_with<T, T>;

template<typename T, typename U>
concept equality_comparable_with = equality_comparable<T>&& equality_comparable<
    U>&& common_reference_with<const std::remove_reference_t<T>&,
                               const std::remove_reference_t<U>&>&&
         equality_comparable<common_reference_t<const std::remove_reference_t<T>&,
                                           const std::remove_reference_t<U>&>>&&
         detail::weakly_equality_comparable_with<T, U>;

template<typename T>
concept regular = semiregular<T>&& equality_comparable<T>;

template<typename T>
concept totally_ordered = equality_comparable<T>&& // clang-format off
    requires(const std::remove_reference_t<T>& a,
             const std::remove_reference_t<T>& b)
    {
        { a < b } -> boolean;
        { a > b } -> boolean;
        { a <= b } -> boolean;
        { a >= b } -> boolean;
    };
// clang-format on

template<typename T, typename U>
concept totally_ordered_with = totally_ordered<T>&& totally_ordered<U>&&
                                                    common_reference_with<const std::remove_reference_t<T>&,
                          const std::remove_reference_t<U>&>&&
                                                    totally_ordered<common_reference_t<const std::remove_reference_t<T>&,
                                           const std::remove_reference_t<U>&>>&&
                                                    equality_comparable_with<T, U>&& // clang-format off
    requires(const std::remove_reference_t<T>& t,
             const std::remove_reference_t<U>& u)
    { // clang-format off
        { t < u } -> boolean;
        { t > u } -> boolean;
        { t <= u } -> boolean;
        { t >= u } -> boolean;
        { u < t } -> boolean;
        { u > t } -> boolean;
        { u <= t } -> boolean;
        { u >= t } -> boolean;
    };
// clang-format on

template<typename F, typename... Args>
concept invocable = // clang-format off
    requires(F&& f, Args&&... args)
    {
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    }; // clang-format on

template<typename F, typename... Args>
concept regular_invocable = invocable<F, Args...>;

template<typename F, typename... Args>
concept predicate =
    regular_invocable<F, Args...>&& boolean<std::invoke_result_t<F, Args...>>;

template<typename R, typename T, typename U>
concept relation = predicate<R, T, T>&& predicate<R, U, U>&&
    predicate<R, T, U>&& predicate<R, U, T>;

template<typename R, typename T, typename U>
concept strict_weak_order = relation<R, T, U>;
} // namespace hera