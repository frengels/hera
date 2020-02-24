#pragma once

#include <iterator>

#include "hera/algorithm/accumulate.hpp"
#include "hera/algorithm/unpack.hpp"
#include "hera/constant.hpp"
#include "hera/container/integer_sequence.hpp"
#include "hera/view/interface.hpp"

namespace hera
{
template<typename CharT, CharT... Chs>
class basic_string : public view_interface<basic_string<CharT, Chs...>>
{
public:
    using value_type      = CharT;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;

    static constexpr size_type npos = std::numeric_limits<size_type>::max();

private:
    static constexpr value_type str_[sizeof...(Chs) + 1]{Chs..., '\0'};

public:
    basic_string() = default;

    constexpr operator std::basic_string_view<value_type>() const
    {
        return std::basic_string_view<value_type>(str_, sizeof...(Chs));
    }

    template<value_type... UChs>
    [[nodiscard]] constexpr auto
    operator==(hera::basic_string<value_type, UChs...>) const
    {
        constexpr auto strv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, Chs...>{});
        constexpr auto ustrv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, UChs...>{});

        return std::bool_constant<(strv == ustrv)>{};
    }

    template<value_type... UChs>
    [[nodiscard]] constexpr auto
    operator!=(hera::basic_string<value_type, UChs...>) const
    {
        constexpr auto strv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, Chs...>{});
        constexpr auto ustrv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, UChs...>{});

        return std::bool_constant<(strv != ustrv)>{};
    }

    static constexpr const_pointer data()
    {
        return str_;
    }

    static std::basic_string<value_type> str() noexcept
    {
        // -- to not count the terminating null byte
        return std::string(std::begin(str_), --std::end(str_));
    }

    constexpr std::integral_constant<size_type, sizeof...(Chs)> size() const
    {
        return {};
    }

    constexpr std::bool_constant<sizeof...(Chs) == 0> empty() const
    {
        return {};
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Chs))
    constexpr auto get() const // clang-format on
    {
        return typename detail::ith_value<I, CharT, Chs...>::type{};
    }

    constexpr value_type operator[](std::size_t idx) const
    {
        return str_[idx];
    }

    [[nodiscard]] constexpr auto clear() const
    {
        return hera::basic_string<value_type>{};
    }

    [[nodiscard]] constexpr auto pop_back() const
    {
        constexpr auto strv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, Chs...>{});

        constexpr auto seq = hera::make_index_sequence<strv.size() - 1>{};

        return hera::unpack(seq, [&](auto... is) {
            return hera::basic_string<value_type,
                                      strv[decltype(is)::value]...>{};
        });
    }

    [[nodiscard]] constexpr auto pop_front() const
    {
        constexpr auto strv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, Chs...>{});

        constexpr auto seq = hera::make_index_sequence<strv.size() - 1>{};

        return hera::unpack(seq, [&](auto... is) {
            return hera::basic_string<value_type,
                                      strv[decltype(is)::value + 1]...>{};
        });
    }

    template<value_type Char>
    [[nodiscard]] constexpr basic_string<value_type, Chs..., Char>
    push_back() const
    {
        return {};
    }

    template<value_type Char>
    [[nodiscard]] constexpr basic_string<value_type, Char, Chs...>
    push_front() const
    {
        return {};
    }

    template<size_type Pos, size_type Count = npos> // clang-format off
        requires (Pos <= sizeof...(Chs))
    constexpr auto substr() const // clang-format on
    {
        constexpr auto strv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, Chs...>{});
        constexpr auto sub_strv = strv.substr(Pos, Count);
        constexpr auto new_size = sub_strv.size();

        constexpr auto seq = hera::make_index_sequence<new_size>();

        return hera::unpack(seq, [&](auto... is) {
            return hera::basic_string<value_type,
                                      sub_strv[decltype(is)::value]...>{};
        });
    }

    template<value_type... UChs>
    [[nodiscard]] constexpr auto
        compare(hera::basic_string<value_type, UChs...>) const
    {
        constexpr auto strv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, Chs...>{});
        constexpr auto ustrv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, UChs...>{});

        return std::integral_constant<int, strv.compare(ustrv)>{};
    }

    template<value_type... UChs>
    [[nodiscard]] constexpr auto
        append(hera::basic_string<value_type, UChs...>) const
    {
        constexpr auto strv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, Chs...>{});
        constexpr auto ustrv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, UChs...>{});

        constexpr auto seq =
            hera::make_index_sequence<strv.size() + ustrv.size()>{};

        constexpr auto select_char =
            [](std::basic_string_view<value_type> first,
               std::basic_string_view<value_type> second,
               std::size_t                        index) {
                if (index < first.size())
                {
                    return first[index];
                }
                else
                {
                    return second[index - first.size()];
                }
            };

        return hera::unpack(seq, [&](auto... is) {
            return hera::basic_string<
                value_type,
                select_char(strv, ustrv, decltype(is)::value)...>{};
        });
    }

    template<size_type I> // clang-format off
        requires (I < sizeof...(Chs))
    constexpr auto erase() const // clang-format on
    {
        constexpr auto strv = static_cast<std::basic_string_view<value_type>>(
            hera::basic_string<value_type, Chs...>{});

        constexpr auto seq = hera::make_index_sequence<strv.size()>{};

        return hera::accumulate(
            seq, hera::basic_string<value_type>{}, [&](auto str, auto index) {
                constexpr auto index_const = decltype(index)::value;

                if constexpr (index_const != I)
                {
                    return str.template push_back<(
                        decltype(get<index_const>())::value)>();
                }
                else
                {
                    return str;
                }
            });
    }
};

template<char... Chs>
using string = basic_string<char, Chs...>;

template<wchar_t... Chs>
using wstring = basic_string<wchar_t, Chs...>;

template<char8_t... Chs>
using u8string = basic_string<char8_t, Chs...>;

template<char16_t... Chs>
using u16string = basic_string<char16_t, Chs...>;

template<char32_t... Chs>
using u32string = basic_string<char32_t, Chs...>;

namespace literals
{
template<typename CharT, CharT... Chs>
constexpr hera::basic_string<CharT, Chs...> operator""_s() noexcept
{
    return {};
}
} // namespace literals

namespace detail
{
template<typename CharT>
struct is_constant_char
{
    template<typename C>
    struct apply
        : std::bool_constant<hera::constant_char<C> &&
                             hera::same_as<CharT, typename C::value_type>>
    {};
};
} // namespace detail

template<typename T, typename CharT>
concept constant_string_of = hera::character<CharT>&&
    hera::range_values_pred<T, detail::is_constant_char<CharT>::template apply>;

template<typename T>
concept constant_string = constant_string_of<T, char>;

template<typename T>
concept constant_wstring = constant_string_of<T, wchar_t>;

template<typename T>
concept constant_u8string = constant_string_of<T, char8_t>;

template<typename T>
concept constant_u16string = constant_string_of<T, char16_t>;

template<typename T>
concept constant_u32string = constant_string_of<T, char32_t>;
} // namespace hera

namespace std
{
template<typename CharT, CharT... Chs>
struct tuple_size<hera::basic_string<CharT, Chs...>>
    : std::integral_constant<std::size_t, sizeof...(Chs)>
{};

template<std::size_t I, typename CharT, CharT... Chs>
struct tuple_element<I, hera::basic_string<CharT, Chs...>>
{
    // this works as get returns the integral constant by value
    using type = decltype(
        std::declval<hera::basic_string<CharT, Chs...>&>().template get<I>());
};
} // namespace std