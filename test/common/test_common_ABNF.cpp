#include "../../include/net.hpp"

#include "../test_common/test_macro.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <string_view>

int main()
{
    using namespace mcs::ABNF; // NOLINT

    TEST("OCTET") = [] {
        static_assert(std::numeric_limits<OCTET>::min() == 0x00);
        static_assert(std::numeric_limits<OCTET>::max() == 0xFF); // NOLINT
    };
    TEST("ALPHA") = [] {
        static_assert(ALPHA('A')); // 大写字母
        static_assert(ALPHA('M'));
        static_assert(ALPHA('Z'));
        static_assert(ALPHA('a')); // 小写字母
        static_assert(ALPHA('n'));
        static_assert(ALPHA('z'));

        static_assert(!ALPHA(' ')); // 非字母字符
        static_assert(!ALPHA('@'));
        static_assert(!ALPHA('['));
        static_assert(!ALPHA('0'));
        static_assert(!ALPHA('\t'));
    };
    TEST("BIT") = [] {
        static_assert(BIT('0'));
        static_assert(BIT('1'));

        static_assert(not BIT('A')); // 大写字母
    };

    TEST("CHAR") = [] {
        static_assert(CHAR('0'));
        static_assert(CHAR('1'));

        static_assert(CHAR('A')); // 大写字母

        static_assert(CHAR(' '));

        constexpr char k_nul = '\0'; // 或 '\x00'
        static_assert(not CHAR(k_nul));
        static_assert(not CHAR('\x00'));

        // Character too large for enclosing character literal
        // typeclang(character_too_large)
        // static_assert(!CHAR('€')); // 非 ASCII

        // char c = '€'; // 编译期失败
        // NOTE: ASCII 是 0-127 即0x00 - 0x7F
        constexpr auto k_max = std::numeric_limits<char>::max();
        static_assert(k_max == 127); // NOLINT
        constexpr auto k_min = std::numeric_limits<char>::min();
        static_assert(k_min == -128); // NOLINT

        constexpr auto k_min2 = std::numeric_limits<unsigned char>::min();
        static_assert(k_min2 == 0);                                      // NOLINT
        static_assert(std::numeric_limits<unsigned char>::max() == 255); // NOLINT
        {
            constexpr char c = -42; // NOLINT
            // 合法，但值可能出乎意料！
            constexpr unsigned char uc = c; // NOLINT
            // unsigned char 的值为 256 + c（例如 -42 → 214）
            static_assert(uc == 214); // NOLINT
        }
    };

    TEST("CRLF") = [] {
        constexpr const auto *k_crlf = "\r\n"; // 序列
        constexpr std::string_view k_v = k_crlf;
        static_assert(k_v.length() == 2);
        static_assert(k_v.size() == 2);

        assert(k_v == "\r\n");
        {
            constexpr std::string_view k_v = "\r\n";
            static_assert(k_v.length() == 2);
            static_assert(k_v.size() == 2);
            assert(k_v == "\r\n");
        }
        {
            // NOTE: 运行时
            std::string k_s = "\r\n";
            std::string_view k_v = k_s;
            assert(k_v.length() == 2);
            assert(k_v.size() == 2);
            assert(k_v == "\r\n");
        }
    };
    TEST("CRLF 2") = [] {
        const auto *k_crlf = "\r\n";  // 序列
        const auto *k_crlf2 = "\r\n"; // 序列
        assert(k_crlf == k_crlf2);
        // static_assert(CRLF<k_crlf>); // is not a constant expressionc
        // static_assert(CRLF<"\r\n">); // 失败
    };

    TEST("CRLF 3 ") = [] {
        // NOLINTBEGIN
        constexpr OCTET valid[] = {CR, LF};
        static_assert(CRLF(valid));

        constexpr OCTET invalid1[] = {CR};
        static_assert(not CRLF(invalid1));

        constexpr OCTET invalid2[] = {LF, CR};
        static_assert(not CRLF(invalid2));

        constexpr OCTET invalid3[] = {0x00, LF};
        static_assert(not CRLF(invalid3));

        constexpr OCTET invalid4[] = {CR, LF, LF};
        static_assert(not CRLF(invalid4));
        // NOLINTEND
    };

    TEST("string_view") = [] {
        // std::string_view c = 'a'; // 不可以
        std::string_view c = "a"; // NOTE: string_view 是字符序列
        assert(c[0] == 'a');
    };

    TEST("CTL") = [] {
        // NOLINTBEGIN
        static_assert(CTL(0x00));
        static_assert(CTL(0x1F));
        static_assert(CTL(0x7F));

        static_assert(not CTL(0x20));
        static_assert(not CTL('A'));
        static_assert(not CTL(0x80));
        // NOLINTEND
    };

    TEST("DIGIT") = [] {
        static_assert(DIGIT('0'));
        static_assert(DIGIT('9'));

        static_assert(not DIGIT('/'));
        static_assert(not DIGIT(':'));
        static_assert(not DIGIT('A'));
    };

    TEST("Constants") = [] {
        static_assert(DQUOTE == '"');
        static_assert(HTAB == '\t');
        static_assert(LF == '\n');
        static_assert(CR == '\r');
        static_assert(SP == ' ');
    };

    TEST("HEXDIG") = [] {
        static_assert(HEXDIG('0'));
        static_assert(HEXDIG('9'));
        static_assert(HEXDIG('A'));
        static_assert(HEXDIG('F'));

        static_assert(not HEXDIG('G'));
        static_assert(not HEXDIG('a'));
        static_assert(not HEXDIG(0xFF)); // NOLINT
    };

    TEST("WSP") = [] {
        static_assert(WSP(SP));
        static_assert(WSP(HTAB));

        static_assert(not WSP('a'));
        static_assert(not WSP(0x00));
    };

    TEST("LWSP") = [] {
        // NOLINTBEGIN
        // 有效案例
        static_assert(LWSP(std::span<const OCTET>{})); // 空

        constexpr OCTET wsp1[] = {SP};
        static_assert(LWSP(wsp1));

        constexpr OCTET wsp2[] = {HTAB, SP, HTAB};
        static_assert(LWSP(wsp2));

        constexpr OCTET crlf_wsp[] = {CR, LF, SP};
        static_assert(LWSP(crlf_wsp));

        constexpr OCTET complex[] = {CR, LF, SP, HTAB, CR, LF, HTAB};
        static_assert(LWSP(complex));

        // 无效案例
        constexpr const OCTET crlf[] = {CR, LF};
        static_assert(not LWSP(crlf));

        constexpr OCTET bad_ending[] = {CR, LF, 'A'};
        static_assert(not LWSP(bad_ending));

        constexpr OCTET mixed[] = {SP, 'a'};
        static_assert(not LWSP(mixed));
        constexpr OCTET mixed2[] = {'a', SP};
        static_assert(not LWSP(mixed2));
        // NOLINTEND
    };

    TEST("VCHAR") = [] {
        // NOLINTBEGIN
        static_assert(VCHAR(0x21)); // '!'
        static_assert(VCHAR(0x7E)); // '~'
        static_assert(VCHAR('A'));

        static_assert(not VCHAR(0x20)); // SP
        static_assert(not VCHAR(0x7F));
        static_assert(not VCHAR(0x00));
        constexpr char k_nul = '\0'; // 或 '\x00'
        static_assert(not VCHAR(k_nul));
        // NOLINTEND
        static_assert(HTAB == '\t');
        static_assert(LF == '\n');
        static_assert(CR == '\r');
        static_assert(SP == ' ');
        static_assert(not VCHAR(HTAB));
        static_assert(not VCHAR(LF));
        static_assert(not VCHAR(CR));
        static_assert(not VCHAR(SP));
    };

    std::cout << "main done\n";
    return 0;
}