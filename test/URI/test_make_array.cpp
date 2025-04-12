
#include <array>
#include <cassert>
#include <span>
#include <string_view>
#include <cstddef>
#include <cstdint>
#include <iostream>

using OCTET = std::uint8_t;
using default_span_t = const std::span<const OCTET> &;
using octet_t = const OCTET &;

// HTAB           =  %x09; horizontal tab
inline constexpr OCTET HTAB = 0x09; // NOLINT

// LF             =  %x0A
inline constexpr OCTET LF = 0x0A; // NOLINT

// CR             =  %x0D
inline constexpr OCTET CR = 0x0D; // NOLINT
// CRLF           =  CR LF
constexpr bool CRLF(default_span_t pair) noexcept // NOLINT
{
    if (pair.size() != 2)
        return false;
    return pair[0] == CR && pair[1] == LF;
}

// SP             =  %x20
inline constexpr OCTET SP = 0x20; // NOLINT

// WSP            =  SP / HTAB; white space
constexpr bool WSP(octet_t c) noexcept // NOLINT
{
    return c == SP || c == HTAB;
}

// NOLINTBEGIN
template <std::size_t N>
consteval auto char_array_size(const char (&str)[N]) noexcept
{
    return N;
}
static_assert(4 == char_array_size("123"));

consteval auto operator"" _get_array_size(const char *, std::size_t len)
{
    return len;
}
static_assert(3 == "123"_get_array_size);

// NOTE: 指针是不能变成数组的
//  consteval auto operator"" _get_array_size2(const char *str, std::size_t len)
//  {
//      return char_array_size(str);
//  }
//  static_assert(4 == "123"_get_array_size2);

consteval auto operator"" _strv(const char *str, std::size_t len)
{
    return std::string_view{str, len};
}

// 如何使用

// TODO
consteval auto length(std::size_t len)
{
    return len;
}

constexpr auto v = "123"_strv;
static_assert(v == "123");

static_assert(sizeof(v) > 0);
constexpr const char *str = "123";

static_assert(sizeof(str) > 0);

consteval auto operator"" _strv2(const char *str, std::size_t len)
{
    return sizeof(str);
}

static_assert("123"_strv2 > 0);

static_assert("123"_strv2 == 8);

// NOTE: 无法使用 函数参数俩构建模板

#include <algorithm>
template <std::size_t N>
struct DoubleString
{
    char p[N * 2 - 1]{};

    constexpr DoubleString(char const (&pp)[N])
    {
        std::ranges::copy(pp, p);
        std::ranges::copy(pp, p + N - 1);
    };
};
template <DoubleString A>
constexpr auto operator""_x2()
{
    return A.p;
}
constexpr auto vv = "abc"_x2;
static_assert(std::string_view(vv) == "abcabc");

template <std::size_t N>
struct make_span
{
    using OCTET = std::uint8_t;
    OCTET octets[N - 1]{};
    constexpr make_span(char const (&s)[N]) noexcept
    {
        for (size_t i = 0; i < N - 1; ++i)
        {
            octets[i] = static_cast<OCTET>(s[i]);
        }
    };

    constexpr auto size() const noexcept
    {
        return N - 1;
    }
};

template <make_span S>
constexpr auto operator""_octs()
{
    return S.octets;
}
constexpr auto c = "abc"_octs;
static_assert(c[2] == 'c');
static_assert(std::is_same_v<decltype(c), const OCTET *const>);

template <make_span S>
consteval auto operator""_span()
{
    return std::span{S.octets, S.size()};
}

struct make_oct
{
    using OCTET = std::uint8_t;
    OCTET oct[1]{};
    constexpr make_oct(char const (&s)[2]) noexcept
    {
        oct[0] = static_cast<OCTET>(s[0]);
    };
};

template <make_oct a>
consteval auto operator""_oct()
{
    return a.oct[0];
}

int main()
{
    // static_assert(CRLF("\r\n")); // 不行
    static_assert(CRLF(std::array<OCTET, 2>{'\r', '\n'}));
    constexpr auto v = "\r\n"_octs;
    constexpr std::span sp(v, 2);

    static_assert(CRLF(sp));
    static_assert(CRLF("\r\n"_span));
    static_assert("\r\n"_span.size() == 2);

    static_assert(WSP(' '));
    constexpr auto v2 = " "_oct;
    static_assert(WSP(v2));
    static_assert(WSP(" "_oct));

    {
        constexpr OCTET octes[1000]{};
        static_assert(octes[999] == 0);
    }
    {
        constexpr char chars[63] =
            "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
        // 如何生成 std::array
    }

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND