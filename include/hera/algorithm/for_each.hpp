#pragma once

#include "hera/indirect_unary.hpp"

namespace hera
{
struct for_each_fn
{
private:
    enum policy : unsigned
    {
        throws = 1,
        equal  = 1 << 1,
        invoke = 2 << 1
    };

    template<hera::forward_iterator        I,
             hera::bounded_sentinel_for<I> S,
             typename F>
    static constexpr unsigned choose() noexcept
    {
        if constexpr (decltype(std::declval<I>() == std::declval<S>())::value)
        {
            return equal | noexcept(std::declval<I>() == std::declval<S>());
        }
        else
        {
            return invoke |
                   std::is_nothrow_invocable_v<F, decltype(*std::declval<I>())>;
        }
    }

    template<hera::forward_iterator        I,
             hera::bounded_sentinel_for<I> S,
             typename F,
             unsigned Choice = choose<I, S, F>()>
    static constexpr F
    invoke_recursively(I i, S s, F fn) noexcept(Choice& throws)
    {
        constexpr auto strategy = Choice & ~throws;
        if constexpr (strategy == equal)
        {
            return fn;
        }
        else
        {
            std::invoke(fn, *i);
            return invoke_recursively(
                hera::next(i), std::move(s), std::move(fn));
        }
    }

public:
    template<hera::forward_iterator        I,
             hera::bounded_sentinel_for<I> S,
             typename F>
    constexpr F operator()(I i, S s, F f) const noexcept(
        noexcept(invoke_recursively(std::move(i), std::move(s), std::move(f))))
    {
        return invoke_recursively(std::move(i), std::move(s), std::move(f));
    }

    template<hera::bounded_range R, typename F>
    constexpr F operator()(R&& r, F f) const
        noexcept(noexcept(invoke_recursively(hera::begin(std::forward<R>(r)),
                                             hera::end(std::forward<R>(r)),
                                             std::move(f))))
    {
        return invoke_recursively(hera::begin(std::forward<R>(r)),
                                  hera::end(std::forward<R>(r)),
                                  std::move(f));
    }
};

constexpr auto for_each = for_each_fn{};
} // namespace hera