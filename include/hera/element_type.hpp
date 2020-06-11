#pragma once

#include <type_traits>

#include "hera/get.hpp"
#include "hera/metafunction.hpp"
#include "hera/type.hpp"
#include "hera/utility/detail/priority_tag.hpp"

namespace hera
{
namespace element_type_impl
{
template<typename T>
void element_type(T&&) = delete;

template<std::size_t I>
struct fn
{
private:
    template<typename MetaF> // clang-format off
        requires (hera::metafunction<std::remove_cvref_t<MetaF>>)
    static constexpr auto check(MetaF&& mf) noexcept // clang-format on
        -> decltype(std::decay_t<MetaF>(static_cast<MetaF&&>(mf)))
    {
        return std::decay_t<MetaF>(static_cast<MetaF&&>(mf));
    }

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<4>,
                               R&& range) noexcept
        -> decltype(check(static_cast<R&&>(range).template element_type<I>()))
    {
        return check(static_cast<R&&>(range).template element_type<I>());
    }

    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<3>,
                               R&& range) noexcept
        -> decltype(check(element_type<I>(static_cast<R&&>(range))))
    {
        return check(element_type<I>(static_cast<R&&>(range)));
    }

    template<typename R> // clang-format off
        requires 
            requires
            {
                // require that tuple_element is complete otherwise because the 
                // const variant is already defined
                std::tuple_element<I, std::remove_cvref_t<R>>{};
            }
    static constexpr auto impl(hera::detail::priority_tag<2>, R&&) noexcept // clang-format on
    {
        return std::tuple_element<I, std::remove_reference_t<R>>{};
    }

    // if none of the above then try to derive it ourselves
    template<typename R>
    static constexpr auto impl(hera::detail::priority_tag<1>, R&&) noexcept
        -> decltype(hera::type_<decltype(hera::get<I>(std::declval<R>()))>{})
    {
        return hera::type_<decltype(hera::get<I>(std::declval<R>()))>{};
    }

public:
    template<typename R>
    constexpr auto operator()(R&& range) const noexcept
        -> decltype(impl(hera::detail::max_priority_tag,
                         static_cast<R&&>(range)))
    {
        return impl(hera::detail::max_priority_tag, static_cast<R&&>(range));
    }
};
} // namespace element_type_impl

inline namespace cpo
{
template<std::size_t I>
inline constexpr auto element_type = hera::element_type_impl::fn<I>{};
}

template<typename R, std::size_t I>
using element_type_t =
    typename decltype(hera::element_type<I>(std::declval<R&>()))::type;
} // namespace hera