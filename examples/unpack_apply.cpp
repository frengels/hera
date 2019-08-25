#include <cassert>
#include <iostream>
#include <string>
#include <tuple>

#include "hera/algorithm/unpack.hpp"
#include "hera/view/tuple.hpp"

constexpr auto concat = [](auto&&... text) {
    return (std::string(text) + ...);
};

template<typename Tuple>
constexpr auto concat_unpack(Tuple&& tup)
{
    auto tup_view = hera::tuple_view{std::forward<Tuple>(tup)};
    return hera::unpack(tup_view, concat);
}

template<typename Tuple>
constexpr auto concat_apply(Tuple&& tup)
{
    return std::apply(concat, std::forward<Tuple>(tup));
}

int main()
{
    auto tup = std::make_tuple("hello, ", "world", "!!!");

    auto unpack_res = concat_unpack(tup);
    auto apply_res  = concat_apply(tup);

    std::cout << unpack_res << '\n';
    std::cout << apply_res << '\n';
}