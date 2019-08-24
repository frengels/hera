#include "hera/view/filter.hpp"
#include "hera/view/tuple.hpp"

int main()
{
    // we only care about the integers in this tuple
    auto tup = std::make_tuple("Hello hera", 50, "Don't care about me", 50);

    auto tup_view = hera::tuple_view{tup};
}