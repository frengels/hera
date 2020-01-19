#pragma once

#include <type_traits>

#include "hera/concepts.hpp"

namespace hera
{
struct nullopt_t
{};

inline constexpr auto nullopt = nullopt_t{};

namespace detail
{
template<typename T, bool = std::is_reference_v<T>>
struct just_storage_base;

template<typename T>
struct just_storage_base<T, false>
{
    using value_type = T;

    T val_;

    template<typename... Args>
    explicit constexpr just_storage_base(std::in_place_t, Args&&... args)
        : val_(std::forward<Args>(args)...)
    {}

    constexpr value_type& get() & noexcept
    {
        return val_;
    }

    constexpr const value_type& get() const& noexcept
    {
        return val_;
    }

    constexpr value_type&& get() && noexcept
    {
        return std::move(val_);
    }

    constexpr const value_type&& get() const&& noexcept
    {
        return std::move(val_);
    }
};

template<typename T>
struct just_storage_base<T, true>
{
    using value_type = T;
    using raw_type   = std::remove_reference_t<T>;

    raw_type* val_;

    template<hera::convertible_to<T&> U>
    explicit constexpr just_storage_base(std::in_place_t, U&& u) noexcept
        : val_{std::addressof(static_cast<T&>(std::forward<U>(u)))}
    {}

    constexpr value_type& get() & noexcept
    {
        return *val_;
    }

    constexpr const value_type& get() const& noexcept
    {
        return *val_;
    }

    constexpr value_type&& get() && noexcept
    {
        return std::forward<value_type>(*val_);
    }

    constexpr const value_type&& get() const&& noexcept
    {
        return std::forward<value_type>(*val_);
    }
};

template<>
struct just_storage_base<void, false>
{
    just_storage_base() = default;

    constexpr just_storage_base(std::in_place_t) noexcept
    {}
};
} // namespace detail

template<typename T>
class just;

class none;

template<typename Opt>
concept optional =
    hera::specialization_of<Opt, hera::just> || hera::same_as<Opt, hera::none>;

template<typename T>
class just : private detail::just_storage_base<T>
{
private:
    using base = detail::just_storage_base<T>;

public:
    using value_type = T;

public:
    constexpr just() noexcept(std::is_nothrow_default_constructible_v<
                              T>) requires hera::constructible_from<T>
        : base(std::in_place)
    {}

    template<typename... Args>
    explicit constexpr just(std::in_place_t, Args&&... args)
        : base(std::in_place, std::forward<Args>(args)...)
    {}

    template<typename U = value_type>
    constexpr just(U&& value) : base(std::in_place, std::forward<U>(value))
    {}

    template<typename U> // clang-format off
        requires !hera::same_as<T, U>
    explicit(!hera::convertible_to<const U&, T>) // clang-format on
        constexpr just(const just<U>& other)
        : just(*other)
    {}

    template<typename U> // clang-format off
        requires !hera::same_as<T, U>
    explicit(!hera::convertible_to<U&&, T>) // clang-format on
        constexpr just(just<U> && other)
        : just(*std::move(other))
    {}

    constexpr bool has_value() const noexcept
    {
        return true;
    }

    explicit constexpr operator bool() const noexcept
    {
        return true;
    }

    constexpr decltype(auto) operator*() &
        noexcept requires !hera::same_as<void, T>
    {
        return this->get();
    }

    constexpr decltype(auto)
    operator*() const& noexcept requires !hera::same_as<void, T>
    {
        return this->get();
    }

    constexpr decltype(auto) operator*() &&
        noexcept requires !hera::same_as<void, T>
    {
        return this->get();
    }

    constexpr decltype(auto)
    operator*() const&& noexcept requires !hera::same_as<void, T>
    {
        return this->get();
    }

private:
    template<typename F>
    constexpr auto transform_void(F&& fn) const requires hera::same_as<void, T>
    {
        static_assert(hera::invocable<F>, "cannot invoke F");

        using result = std::invoke_result_t<F>;

        if constexpr (hera::same_as<void, result>)
        {
            std::forward<F>(fn)();
            return hera::just<void>{std::in_place};
        }
        else
        {
            return hera::just<result>{std::forward<F>(fn)()};
        }
    }

public:
    template<typename F>
    constexpr auto transform(F&& fn) &
    {
        if constexpr (hera::same_as<T, void>)
        {
            return transform_void(std::forward<F>(fn));
        }
        else
        {
            static_assert(hera::invocable<F, value_type&>, "cannot invoke F");

            using result = std::invoke_result_t<F, value_type&>;

            if constexpr (hera::same_as<void, result>)
            {
                std::forward<F>(fn)(**this);
                return hera::just<void>{std::in_place};
            }
            else
            {
                return hera::just<result>{std::forward<F>(fn)(**this)};
            }
        }
    }

    template<typename F>
    constexpr auto transform(F&& fn) const&
    {
        if constexpr (hera::same_as<T, void>)
        {
            return transform_void(std::forward<F>(fn));
        }
        else
        {
            static_assert(hera::invocable<F, const value_type&>,
                          "cannot invoke F");

            using result = std::invoke_result_t<F, const value_type&>;

            if constexpr (hera::same_as<void, result>)
            {
                std::forward<F>(fn)(**this);
                return hera::just<void>{std::in_place};
            }
            else
            {
                return hera::just<result>{std::forward<F>(fn)(**this)};
            }
        }
    }

    template<typename F>
    constexpr auto transform(F&& fn) &&
    {
        if constexpr (hera::same_as<T, void>)
        {
            return transform_void(std::forward<F>(fn));
        }
        else
        {
            static_assert(hera::invocable<F, value_type&&>, "cannot invoke F");

            using result = std::invoke_result_t<F, value_type&&>;

            if constexpr (hera::same_as<void, result>)
            {
                std::forward<F>(fn)(*std::move(*this));
                return hera::just<void>{std::in_place};
            }
            else
            {
                return hera::just<result>{
                    std::forward<F>(fn)(*std::move(*this))};
            }
        }
    }

    template<typename F>
    constexpr auto transform(F&& fn) const&&
    {
        if constexpr (hera::same_as<T, void>)
        {
            return transform_void(std::forward<F>(fn));
        }
        else
        {
            static_assert(hera::invocable<F, const value_type&&>,
                          "cannot invoke F");

            using result = std::invoke_result_t<F, const value_type&&>;

            if constexpr (hera::same_as<void, result>)
            {
                std::forward<F>(fn)(*std::move(*this));
                return hera::just<void>{std::in_place};
            }
            else
            {
                return hera::just<result>{
                    std::forward<F>(fn)(*std::move(*this))};
            }
        }
    }

private:
    template<typename F>
    constexpr decltype(auto) and_then_void(F&& fn) const
    {
        static_assert(hera::invocable<F>, "cannot invoke F");
        static_assert(hera::specialization_of<
                          std::remove_cvref_t<std::invoke_result_t<F>>,
                          hera::just> ||
                          hera::specialization_of<
                              std::remove_cvref_t<std::invoke_result_t<F>>,
                              hera::none>,
                      "F must return just or none");

        return std::forward<F>(fn)();
    }

public:
    template<typename F>
    constexpr decltype(auto) and_then(F&& fn) &
    {
        if constexpr (hera::same_as<void, T>)
        {
            return and_then_void(std::forward<F>(fn));
        }
        else
        {
            static_assert(hera::invocable<F, value_type&>, "cannot invoke F");
            static_assert(
                hera::optional<
                    std::remove_cvref_t<std::invoke_result_t<F, value_type&>>>,
                "F must return just/none");

            return std::forward<F>(fn)(**this);
        }
    }

    template<typename F>
    constexpr decltype(auto) and_then(F&& fn) const&
    {
        if constexpr (hera::same_as<void, T>)
        {
            return and_then_void(std::forward<F>(fn));
        }
        else
        {
            static_assert(hera::invocable<F, const value_type&>,
                          "cannot invoke F");
            static_assert(hera::optional<std::remove_cvref_t<
                              std::invoke_result_t<F, const value_type&>>>,
                          "F must return just/none");

            return std::forward<F>(fn)(**this);
        }
    }

    template<typename F>
    constexpr decltype(auto) and_then(F&& fn) &&
    {
        if constexpr (hera::same_as<void, T>)
        {
            return and_then_void(std::forward<F>(fn));
        }
        else
        {
            static_assert(hera::invocable<F, value_type&&>, "cannot invoke F");
            static_assert(
                hera::optional<
                    std::remove_cvref_t<std::invoke_result_t<F, value_type&&>>>,
                "F must return just/none");

            return std::forward<F>(fn)(*std::move(*this));
        }
    }

    template<typename F>
    constexpr decltype(auto) and_then(F&& fn) const&&
    {
        if constexpr (hera::same_as<void, T>)
        {
            return and_then_void(std::forward<F>(fn));
        }
        else
        {
            static_assert(hera::invocable<F, const value_type&&>,
                          "cannot invoke F");
            static_assert(hera::optional<std::remove_cvref_t<
                              std::invoke_result_t<F, const value_type&&>>>,
                          "F must return just/none");

            return std::forward<F>(fn)(*std::move(*this));
        }
    }

    template<typename F>
    constexpr just or_else(F&&) const& noexcept
    {
        return *this;
    }

    template<typename F>
        constexpr just&& or_else(F&&) && noexcept
    {
        return std::move(*this);
    }

    constexpr void swap(just& other)
    {
        using std::swap;

        swap(this->val_, other.val_);
    }
};

just()->just<void>;

template<typename T>
just(T &&)->just<T>;

just(std::in_place_t)->just<void>;

class none
{
    using value_type = void;

public:
    none() = default;

    constexpr none(hera::nullopt_t) noexcept
    {}

    constexpr std::false_type has_value() const noexcept
    {
        return {};
    }

    explicit constexpr operator bool() const noexcept
    {
        return false;
    }

    template<typename U>
    constexpr U&& value_or(U&& u) const noexcept
    {
        return std::forward<U>(u);
    }

    template<typename F>
    constexpr hera::none transform(F&&) const noexcept
    {
        return {};
    }

    template<typename F>
    constexpr hera::none and_then(F&&) const noexcept
    {
        return {};
    }

    template<hera::invocable F>
    constexpr decltype(auto) or_else(F&& fn) const
        noexcept(std::is_nothrow_invocable_v<F>)
    {
        static_assert(
            hera::optional<std::remove_cvref_t<std::invoke_result_t<F>>>,
            "F must return just/none");

        return std::forward<F>(fn)();
    }
};
} // namespace hera