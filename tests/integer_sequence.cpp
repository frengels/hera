#include <catch2/catch_test_macros.hpp>

#include "hera/algorithm/unpack.hpp"
#include "hera/container/integer_sequence.hpp"

template<std::size_t I>
constexpr auto size_ = std::integral_constant<std::size_t, I>{};

TEST_CASE("integer_sequence")
{
    hera::integer_sequence<int, 0, 1, 2, 3, 6> seq{};

    static_assert(decltype(seq.front())::value == 0);
    static_assert(decltype(seq.back())::value == 6);

    static_assert(decltype(seq.size())::value == 5);

    hera::unpack(seq, [](auto i1, auto i2, auto i3, auto i4, auto i5) {
        static_assert(decltype(i1)::value == 0);
        static_assert(decltype(i2)::value == 1);
        static_assert(decltype(i3)::value == 2);
        static_assert(decltype(i4)::value == 3);
        static_assert(decltype(i5)::value == 6);
    });

    auto seq1 = hera::make_index_sequence<3>{};

    static_assert(decltype(seq1.size())::value == 3);

    hera::unpack(seq1, [](auto i0, auto i1, auto i2) {
        static_assert(decltype(i0)::value == 0);
        static_assert(decltype(i1)::value == 1);
        static_assert(decltype(i2)::value == 2);
    });

    auto seq2 = hera::index_sequence_for<int, int, int, float, double>{};

    static_assert(decltype(seq2.size())::value == 5);
}