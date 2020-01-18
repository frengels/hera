#pragma once

#include <memory>
#include <tuple>

#include "hera/constant.hpp"
#include "hera/get.hpp"
#include "hera/iterator/sentinel.hpp"
#include "hera/view/detail/closure.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<typename Tuple>
class tuplelike_view : public view_interface<tuplelike_view<Tuple>>
{
private:
    Tuple* tuple_;

public:
    explicit constexpr tuplelike_view(Tuple& tup) noexcept
        : tuple_{std::addressof(tup)}
    {}

    constexpr auto size() const noexcept
        -> std::tuple_size<std::remove_const_t<Tuple>>
    {
        return {};
    }

    template<std::size_t Idx>
    constexpr decltype(auto) at() const
        noexcept requires(Idx < std::tuple_size_v<Tuple>)
    {
        using std::get;
        return get<Idx>(*tuple_);
    }
};

namespace views
{
struct tuplelike_fn
{
    template<typename Tuple>
    constexpr auto operator()(Tuple& tup) const
        noexcept(noexcept(hera::tuplelike_view{tup}))
            -> decltype(hera::tuplelike_view{tup})
    {
        return hera::tuplelike_view{tup};
    }
};

inline constexpr auto tuplelike = tuplelike_fn{};
} // namespace views
} // namespace hera