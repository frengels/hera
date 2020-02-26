#pragma once

#include <limits>
#include <type_traits>

#include "hera/constant.hpp"
#include "hera/metafunction.hpp"
#include "hera/type_.hpp"

namespace hera
{
template<std::size_t N>
using variant_index_t = typename decltype([] {
    if constexpr (N <= std::numeric_limits<uint8_t>::max())
    {
        return hera::type_<uint8_t>{};
    }
    else if constexpr (N <= std::numeric_limits<uint16_t>::max())
    {
        return hera::type_<uint16_t>{};
    }
    else if constexpr (N <= std::numeric_limits<uint32_t>::max())
    {
        return hera::type_<uint32_t>{};
    }
    else
    {
        return hera::type_<uint64_t>{};
    }
}())::type;

struct monostate
{
    monostate() = default;

    constexpr bool operator==(const monostate&) const noexcept
    {
        return true;
    }

    constexpr bool operator!=(const monostate&) const noexcept
    {
        return false;
    }

    constexpr bool operator<(const monostate&) const noexcept
    {
        return false;
    }

    constexpr bool operator>(const monostate&) const noexcept
    {
        return false;
    }

    constexpr bool operator<=(const monostate&) const noexcept
    {
        return true;
    }

    constexpr bool operator>=(const monostate&) const noexcept
    {
        return true;
    }
};

namespace detail
{
template<typename... Ts>
union variant_storage;

template<>
union variant_storage<>
{};

template<typename T, typename... Ts>
union variant_storage<T, Ts...>
{
private:
    T                      first_;
    variant_storage<Ts...> rest_;

public:
    template<typename... Args>
    explicit constexpr variant_storage(
        std::integral_constant<std::size_t, 0>,
        Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : first_(static_cast<Args&&>(args)...)
    {}

    template<std::size_t I, typename... Args>
    explicit constexpr variant_storage(
        std::integral_constant<std::size_t, I>,
        Args&&... args) noexcept(std::
                                     is_nothrow_constructible_v<
                                         variant_storage<Ts...>,
                                         Args...>)
        : rest_(std::integral_constant<std::size_t, (I - 1)>{},
                static_cast<Args&&>(args)...)
    {}

    ~variant_storage() = default;

    template<std::size_t I> // clang-format off
        requires (I == 0)
    constexpr T& get() & noexcept // clang-format on
    {
        return static_cast<T&>(first_);
    }

    template<std::size_t I> // clang-format off
        requires (I == 0)
    constexpr const T& get() const & noexcept // clang-format on
    {
        return static_cast<const T&>(first_);
    }

    template<std::size_t I> // clang-format off
        requires (I == 0)
    constexpr T&& get() && noexcept // clang-format on
    {
        return static_cast<T&&>(first_);
    }

    template<std::size_t I> // clang-format off
        requires (I == 0)
    constexpr const T&& get() const && noexcept // clang-format on
    {
        return static_cast<const T&&>(first_);
    }

    template<std::size_t I> // clang-format off
        requires (I != 0)
    constexpr decltype(auto) get() & noexcept // clang-format on
    {
        return rest_.template get<(I - 1)>();
    }

    template<std::size_t I> // clang-format off
        requires (I != 0)
    constexpr decltype(auto) get() const & noexcept // clang-format on
    {
        return rest_.template get<(I - 1)>();
    }

    template<std::size_t I> // clang-format off
        requires (I != 0)
    constexpr decltype(auto) get() && noexcept // clang-format on
    {
        return std::move(rest_).template get<(I - 1)>();
    }

    template<std::size_t I> // clang-format off
        requires (I != 0)
    constexpr decltype(auto) get() const && noexcept // clang-format on
    {
        return std::move(rest_).template get<(I - 1)>();
    }
};
} // namespace detail

namespace detail
{
struct none
{};

template<std::size_t N>
struct invoke_with_index_constant_impl
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
#define FWD(x) static_cast<decltype(x)&&>(x)
#define DO_CASE(fn, i, offs)                                                   \
    case i:                                                                    \
        return FWD(fn)(std::integral_constant<std::size_t, (i + offs)>{})

#define DO_CASE_DEFAULT(fn, i, offs)                                           \
    default:                                                                   \
        return FWD(fn)(std::integral_constant<std::size_t, (i + offs)>{})

        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE(fn, 4, Offset);
            DO_CASE(fn, 5, Offset);
            DO_CASE(fn, 6, Offset);
            DO_CASE(fn, 7, Offset);
            DO_CASE(fn, 8, Offset);
            DO_CASE(fn, 9, Offset);
            DO_CASE(fn, 10, Offset);
            DO_CASE(fn, 11, Offset);
            DO_CASE(fn, 12, Offset);
            DO_CASE(fn, 13, Offset);
            DO_CASE(fn, 14, Offset);
            DO_CASE(fn, 15, Offset);
        }

        return invoke_with_index_constant_impl<N - 16>::template invoke<Offset +
                                                                        16>(
            i - 16, static_cast<F&&>(fn));
    }
};

template<>
struct invoke_with_index_constant_impl<0>
{
    template<std::size_t, typename F>
    static constexpr void invoke(std::size_t, F&&) noexcept
    {}
};

template<>
struct invoke_with_index_constant_impl<1>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t, F&& fn)
    {
        return static_cast<F&&>(fn)(
            std::integral_constant<std::size_t, Offset + 0>{});
    }
};

template<>
struct invoke_with_index_constant_impl<2>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE_DEFAULT(fn, 1, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<3>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE_DEFAULT(fn, 2, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<4>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE_DEFAULT(fn, 3, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<5>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE_DEFAULT(fn, 4, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<6>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE(fn, 4, Offset);
            DO_CASE_DEFAULT(fn, 5, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<7>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE(fn, 4, Offset);
            DO_CASE(fn, 5, Offset);
            DO_CASE_DEFAULT(fn, 6, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<8>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE(fn, 4, Offset);
            DO_CASE(fn, 5, Offset);
            DO_CASE(fn, 6, Offset);
            DO_CASE_DEFAULT(fn, 7, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<9>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE(fn, 4, Offset);
            DO_CASE(fn, 5, Offset);
            DO_CASE(fn, 6, Offset);
            DO_CASE(fn, 7, Offset);
            DO_CASE_DEFAULT(fn, 8, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<10>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE(fn, 4, Offset);
            DO_CASE(fn, 5, Offset);
            DO_CASE(fn, 6, Offset);
            DO_CASE(fn, 7, Offset);
            DO_CASE(fn, 8, Offset);
            DO_CASE_DEFAULT(fn, 9, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<11>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE(fn, 4, Offset);
            DO_CASE(fn, 5, Offset);
            DO_CASE(fn, 6, Offset);
            DO_CASE(fn, 7, Offset);
            DO_CASE(fn, 8, Offset);
            DO_CASE(fn, 9, Offset);
            DO_CASE_DEFAULT(fn, 10, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<12>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE(fn, 4, Offset);
            DO_CASE(fn, 5, Offset);
            DO_CASE(fn, 6, Offset);
            DO_CASE(fn, 7, Offset);
            DO_CASE(fn, 8, Offset);
            DO_CASE(fn, 9, Offset);
            DO_CASE(fn, 10, Offset);
            DO_CASE_DEFAULT(fn, 11, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<13>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE(fn, 4, Offset);
            DO_CASE(fn, 5, Offset);
            DO_CASE(fn, 6, Offset);
            DO_CASE(fn, 7, Offset);
            DO_CASE(fn, 8, Offset);
            DO_CASE(fn, 9, Offset);
            DO_CASE(fn, 10, Offset);
            DO_CASE(fn, 11, Offset);
            DO_CASE_DEFAULT(fn, 12, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<14>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE(fn, 4, Offset);
            DO_CASE(fn, 5, Offset);
            DO_CASE(fn, 6, Offset);
            DO_CASE(fn, 7, Offset);
            DO_CASE(fn, 8, Offset);
            DO_CASE(fn, 9, Offset);
            DO_CASE(fn, 10, Offset);
            DO_CASE(fn, 11, Offset);
            DO_CASE(fn, 12, Offset);
            DO_CASE_DEFAULT(fn, 13, Offset);
        }
    }
};

template<>
struct invoke_with_index_constant_impl<15>
{
    template<std::size_t Offset, typename F>
    static constexpr decltype(auto) invoke(std::size_t i, F&& fn)
    {
        switch (i)
        {
            DO_CASE(fn, 0, Offset);
            DO_CASE(fn, 1, Offset);
            DO_CASE(fn, 2, Offset);
            DO_CASE(fn, 3, Offset);
            DO_CASE(fn, 4, Offset);
            DO_CASE(fn, 5, Offset);
            DO_CASE(fn, 6, Offset);
            DO_CASE(fn, 7, Offset);
            DO_CASE(fn, 8, Offset);
            DO_CASE(fn, 9, Offset);
            DO_CASE(fn, 10, Offset);
            DO_CASE(fn, 11, Offset);
            DO_CASE(fn, 12, Offset);
            DO_CASE(fn, 13, Offset);
            DO_CASE_DEFAULT(fn, 14, Offset);
        }
    }
};

#undef DO_CASE
#undef FWD
#undef DO_CASE_DEFAULT

template<std::size_t N, typename F>
constexpr decltype(auto) invoke_with_index_constant(std::size_t i, F&& fn)
{
    assert(i < N);
    return invoke_with_index_constant_impl<N>::template invoke<0>(
        i, static_cast<F&&>(fn));
}
} // namespace detail

template<typename... Ts>
class variant
{
private:
    // none is simply used to allow construction easier
    detail::variant_storage<detail::none, Ts...> storage_;
    variant_index_t<sizeof...(Ts)>               index_{};

public:
    template<hera::constant_convertible_to<std::size_t> Index, typename... Args>
    constexpr variant(Index, Args&&... args) noexcept(
        std::is_nothrow_constructible_v<
            detail::variant_storage<detail::none, Ts...>,
            std::integral_constant<std::size_t, Index::value + 1>,
            Args...>)
        : storage_(std::integral_constant<std::size_t, Index::value + 1>{},
                   static_cast<Args&&>(args)...),
          index_(Index::value)
    {}

private:
    template<std::size_t I, typename... Args>
    constexpr void construct(Args&&... args)
    {
        new (static_cast<void*>(std::addressof(*this)))
            detail::variant_storage<detail::none, Ts...>(
                std::integral_constant<std::size_t, I + 1>{},
                static_cast<Args&&>(args)...);
    }

    constexpr void destroy() noexcept
    {
        detail::invoke_with_index_constant<sizeof...(Ts)>(index(), [&](auto i) {
            static constexpr auto index = decltype(i)::value;
            using type = std::tuple_element_t<index, std::tuple<Ts...>>;

            unsafe_get<index>().~type();
        });
    }

public:
    ~variant() requires(std::is_trivially_destructible_v<Ts>&&...) = default;

    ~variant() requires(!std::is_trivially_destructible_v<Ts> || ...)
    {
        destroy();
    }

    ~variant() requires(!std::is_destructible_v<Ts> || ...) = delete;

    // copy constructor
    variant(const variant& v) noexcept requires(
        (std::is_trivially_copy_constructible_v<Ts> && ...)) = default;

    constexpr variant(const variant& v) requires(
        !(std::is_trivially_copy_constructible_v<Ts> && ...))
        : storage_(std::integral_constant<std::size_t, 0>{}), index_(v.index())
    {
        detail::invoke_with_index_constant<sizeof...(Ts)>(
            v.index(), [&](auto i) {
                construct(decltype(i)::value,
                          v.template unsafe_get<decltype(i)::value>());
            });
    }

    variant(const variant& v) requires((!std::is_copy_constructible_v<Ts>) ||
                                       ...) = delete;

    // move constructor
    variant(variant&&) noexcept requires(
        (std::is_trivially_move_constructible_v<Ts> && ...)) = default;

    constexpr variant(variant&& v) requires(
        !(std::is_trivially_move_constructible_v<Ts> && ...))
        : storage_(std::integral_constant<std::size_t, 0>{}), index_(v.index())
    {
        detail::invoke_with_index_constant<sizeof...(Ts)>(
            v.index(), [&](auto i) {
                construct(
                    decltype(i)::value,
                    std::move(v).template unsafe_get<decltype(i)::value>());
            });
    }

    variant(variant&&) requires((!std::is_move_constructible_v<Ts>) ||
                                ...) = delete;

    // copy assign
    variant& operator=(const variant&) noexcept requires(
        std::is_trivially_copy_assignable_v<Ts>&&...) = default;

    constexpr variant& operator=(const variant& v) requires(
        !std::is_trivially_copy_assignable_v<Ts> || ...)
    {
        destroy();

        detail::invoke_with_index_constant<sizeof...(Ts)>(
            v.index(), [&](auto i) {
                construct(decltype(i)::value,
                          v.template unsafe_get<decltype(i)::value>());
            });

        index_ = v.index();
        return *this;
    }

    variant&
    operator=(const variant&) requires(!std::is_copy_assignable_v<Ts> ||
                                       ...) = delete;

    // move assign
    variant& operator                                 =(variant&&) requires(
        std::is_trivially_move_assignable_v<Ts>&&...) = default;

    constexpr variant&
    operator=(variant&& v) requires(!std::is_trivially_move_assignable_v<Ts> ||
                                    ...)
    {
        destroy();

        detail::invoke_with_index_constant<sizeof...(Ts)>(
            v.index(), [&](auto i) {
                construct(
                    decltype(i)::value,
                    std::move(v).template unsafe_get<decltype(i)::value>());
            });

        index_ = v.index();
        return *this;
    }

    variant& operator=(variant&&) requires(!std::is_move_assignable_v<Ts> ||
                                           ...) = delete;

    constexpr std::size_t index() const noexcept
    {
        return static_cast<std::size_t>(index_);
    }

    constexpr auto variant_size() const noexcept
    {
        return std::integral_constant<std::size_t, sizeof...(Ts)>{};
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts))
    constexpr decltype(auto) unsafe_get() & noexcept // clang-format on
    {
        assert(I == index());

        return storage_.template get<I + 1>();
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts))
    constexpr decltype(auto) unsafe_get() const & noexcept // clang-format on
    {
        assert(I == index());

        return storage_.template get<I + 1>();
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts))
    constexpr decltype(auto) unsafe_get() && noexcept // clang-format on
    {
        assert(I == index());

        return std::move(storage_).template get<I + 1>();
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts))
    constexpr decltype(auto) unsafe_get() const && noexcept // clang-format on
    {
        assert(I == index());

        return std::move(storage_).template get<I + 1>();
    }
};
} // namespace hera