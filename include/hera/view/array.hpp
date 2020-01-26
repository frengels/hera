#pragma once

#include <array>
#include <cassert>

#include "hera/constant.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<typename T>
concept arraylike_container = // clang-format off
    requires (T& t)
    {
        std::size(t);
        std::data(t);
    }; // clang-format on

template<typename T, std::size_t N>
class array_view : public hera::view_interface<array_view<T, N>>
{
public:
    using size_type = std::size_t;

private:
    T* data_;

public:
    explicit constexpr array_view(T (&arr)[N]) noexcept : data_{arr + 0}
    {}

    explicit constexpr array_view(std::array<T, N>& arr) noexcept
        : data_{arr.data()}
    {}

    template<arraylike_container Cont> // clang-format off
        requires !std::is_array_v<Cont>
    explicit constexpr array_view(Cont& container) noexcept // clang-format on
        : data_{std::data(container)}
    {
        assert(std::size(container) >= N);
    }

    constexpr auto size() const noexcept
        -> std::integral_constant<std::size_t, N>
    {
        return {};
    }

    template<std::size_t I>
    constexpr auto try_get() const noexcept
    {
        if constexpr (I < N)
        {
            return hera::just{data_[I]};
        }
        else
        {
            return hera::none{};
        }
    }
};

template<typename T, std::size_t N>
array_view(T (&)[N])->array_view<T, N>;

template<typename T, std::size_t N>
array_view(const T (&)[N])->array_view<const T, N>;

template<typename T, std::size_t N>
array_view(const std::array<T, N>&)->array_view<const T, N>;

namespace views
{
struct array_fn
{
    template<typename... Args>
    constexpr auto operator()(Args&&... args) const
        noexcept(noexcept(hera::array_view{std::forward<Args>(args)...}))
            -> decltype(hera::array_view{std::forward<Args>(args)...})
    {
        return hera::array_view{std::forward<Args>(args)...};
    }
};

constexpr auto array = array_fn{};
} // namespace views
} // namespace hera
