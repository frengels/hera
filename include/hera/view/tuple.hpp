#pragma once

#include <memory>
#include <tuple>

#include "hera/constant.hpp"
#include "hera/get.hpp"
#include "hera/iterator/sentinel.hpp"
#include "hera/optional.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<typename Tuple>
class tuple_view : public view_interface<tuple_view<Tuple>>
{
private:
    Tuple* tuple_;

public:
    explicit constexpr tuple_view(Tuple& tup) noexcept
        : tuple_{std::addressof(tup)}
    {}

    constexpr auto size() const noexcept
        -> std::tuple_size<std::remove_const_t<Tuple>>
    {
        return {};
    }

    template<std::size_t I>
    constexpr auto try_at() const noexcept
    {
        if constexpr (I < std::tuple_size_v<std::remove_const_t<Tuple>>)
        {
            return hera::just{std::get<I>(*tuple_)};
        }
        else
        {
            return hera::none{};
        }
    }
};

namespace views
{
struct tuple_fn
{
    template<typename Tuple>
    constexpr auto operator()(Tuple& tup) const
        noexcept(noexcept(hera::tuple_view{tup}))
            -> decltype(hera::tuple_view{tup})
    {
        return hera::tuple_view{tup};
    }
};

inline constexpr auto tuple = tuple_fn{};
} // namespace views
} // namespace hera