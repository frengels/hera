#pragma once

#include "hera/begin_end.hpp"
#include "hera/empty.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
namespace detail
{
template<typename R>
void FUN(R&);

template<typename R>
void FUN(R&&) = delete;
} // namespace detail

template<hera::range R> // clang-format off
    requires std::is_object_v<R>
class ref_view : public view_interface<ref_view<R>> // clang-format on
{
private:
    R* base_;

public:
    template<typename T> // clang-format off
        requires !hera::same_as<T, ref_view> && 
            hera::convertible_to<T, R&> &&
            requires {
                hera::detail::FUN<R>(std::declval<T>());
            }
    constexpr ref_view(T&& t) // clang-format on
        : base_{std::addressof(static_cast<R&>(std::forward<T>(t)))}
    {}

    constexpr auto size() const noexcept(noexcept(hera::size(*base_)))
        -> decltype(hera::size(*base_))
    {
        return hera::size(*base_);
    }

    template<std::size_t I>
    constexpr auto at() const noexcept(noexcept(hera::at<I>(*base_)))
        -> decltype(hera::at<I>(*base_))
    {
        return hera::at<I>(*base_);
    }
};

template<typename R>
ref_view(R&)->ref_view<R>;

template<typename T>
inline constexpr bool enable_safe_range<ref_view<T>> = true;

/*
template<hera::range R>
requires std::is_object_v<R> class ref_view : public view_interface<ref_view<R>>
{
private:
    R& base_;

public:
    template<typename D = R> // clang-format off
        requires !same_as<D, ref_view> // clang-format on
        constexpr ref_view(R & range) noexcept : base_{range}
    {}

    constexpr R& base() const noexcept
    {
        return base_;
    }

    constexpr auto begin() const noexcept(noexcept(hera::begin(base())))
    {
        return hera::begin(base());
    }

    constexpr auto end() const noexcept(noexcept(hera::end(base())))
    {
        return hera::end(base());
    }

    template<typename D = R> // clang-format off
        requires requires(D& d)
        {
            hera::empty(d);
        } // clang-format on
    constexpr auto empty() const noexcept(noexcept(hera::empty(base())))
    {
        return hera::empty(base());
    }

    template<hera::sized_range D = R>
    constexpr auto size() const noexcept(noexcept(hera::size(base())))
    {
        return hera::size(base());
    }

    friend constexpr auto begin(ref_view r) noexcept(noexcept(r.begin()))
    {
        return r.begin();
    }

    friend constexpr auto end(ref_view r) noexcept(noexcept(r.end()))
    {
        return r.end();
    }
};
*/
} // namespace hera