#pragma once

#include "hera/algorithm/unpack.hpp"
#include "hera/metafunction.hpp"
#include "hera/optional.hpp"
#include "hera/type_identity.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<typename... Ts>
class type_list : public hera::view_interface<type_list<Ts...>>
{
public:
    type_list() = default;

    constexpr std::integral_constant<std::size_t, sizeof...(Ts)> size() const
        noexcept
    {
        return {};
    }

    constexpr std::bool_constant<sizeof...(Ts) == 0> empty() const noexcept
    {
        return {};
    }

    template<typename F> // clang-format off
        requires (invocable<F, Ts> && ...)
    constexpr auto transform(F&&) const noexcept // clang-format on
    {
        return hera::type_list<std::invoke_result_t<F, Ts>...>{};
    }

    template<typename... Fs> // clang-format off
        requires (sizeof...(Ts) == sizeof...(Fs))
    constexpr auto transform(Fs&&...) const noexcept // clang-format on
    {
        // TODO move to constraints once gcc stops bugging
        static_assert((invocable<Fs, Ts> && ...));
        return hera::type_list<std::invoke_result_t<Fs, Ts>...>{};
    }

    template<std::size_t I>
    constexpr auto try_get() const noexcept
    {
        if constexpr (I < sizeof...(Ts))
        {
            using type = std::tuple_element_t<I, std::tuple<Ts...>>;
            return hera::just<hera::type_identity<type>>{};
        }
        else
        {
            return hera::none{};
        }
    }
};

template<hera::metafunction... Ms>
constexpr auto make_type_list_from_metafunctions(Ms...) noexcept
{
    return hera::type_list<typename Ms::type...>{};
}

template<typename... Ts>
constexpr auto forward_as_type_list(Ts&&... ts) noexcept
{
    return make_type_list_from_metafunctions(
        hera::forward_type{std::forward<Ts>(ts)}...);
}

struct to_base_type_list_fn
    : public detail::pipeable_interface<to_base_type_list_fn>
{
    template<hera::bounded_range R>
    constexpr auto operator()(const R& r) const noexcept
    {
        // we can pass everything by const reference because it will internally
        // decay everything anyway
        return hera::unpack(r, [](const auto&... xs) {
            return hera::type_list<std::decay_t<decltype(xs)>...>{};
        });
    }
};

constexpr auto to_base_type_list = to_base_type_list_fn{};

struct forward_to_type_list_fn
    : public detail::pipeable_interface<forward_to_type_list_fn>
{
    template<hera::bounded_range R>
    constexpr auto operator()(R&& r) const noexcept
    {
        return hera::unpack(std::forward<R>(r), [](auto&&... xs) {
            return hera::type_list<decltype(
                std::forward<decltype(xs)>(xs))...>{};
        });
    }
};

constexpr auto forward_to_type_list = forward_to_type_list_fn{};
} // namespace hera
