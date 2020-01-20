#include <iostream>

#include "hera/algorithm/for_each.hpp"
#include "hera/view/filter.hpp"
#include "hera/view/tuple.hpp"

int main()
{
    // we only care about the integers in this tuple
    auto tup = std::make_tuple("Hello hera!", 50, "Don't care about me", 50);
    auto tup_view = hera::tuple_view{tup};

    // Predicate must return an integral constant of type bool.
    auto only_ints = tup_view | hera::views::filter([](auto x) {
                         return std::is_same<decltype(x), int>{};
                     });

    auto res = 0;
    hera::for_each(only_ints, [&](auto filtered) { return res += filtered; });

    std::cout << "result is: " << res << '\n';

    // now we only care about the strings
    auto only_str = tup_view | hera::views::filter([](auto str) {
                        return std::is_same<decltype(str), const char*>{};
                    });

    hera::for_each(only_str, [](auto str) { std::cout << str << '|'; });
    std::cout << '\n';
}