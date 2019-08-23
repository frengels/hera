#pragma once

#include "hera/begin_end.hpp"
#include "hera/empty.hpp"
#include "hera/ranges.hpp"
#include "hera/size.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<hera::range R>
requires std::is_object_v<R> class ref_view
    : public view_interface<ref_view<R>> {
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
} // namespace hera