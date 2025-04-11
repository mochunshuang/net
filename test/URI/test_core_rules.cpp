#include <iostream>

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

// NOLINTBEGIN

using OCTET = mcs::ABNF::OCTET;
using namespace mcs::ABNF;

int main()
{
    // 测试代码
    static_assert(
        [] {
            constexpr OCTET split_first1[] = {'a', ':', 'b', ':', 'c'};
            auto result = tool::split_span_first(split_first1, ':');
            return result.first.size() == 1 && result.second.size() == 3;
        }(),
        "split_span_first test failed");

    static_assert(
        [] {
            constexpr OCTET split_last1[] = {'a', ':', 'b', ':', 'c'};
            auto result = tool::split_span_last(split_last1, ':');
            return result.first.size() == 3 && result.second.size() == 1;
        }(),
        "split_span_last test failed");

    // LWSP 测试
    static_assert(
        [] {
            constexpr OCTET lwsp1[] = {' ', '\t', CR, LF, ' '};
            return LWSP(lwsp1).has_value();
        }(),
        "LWSP test 1 failed");

    static_assert(
        [] {
            constexpr OCTET lwsp2[] = {' ', 'a'};
            return !LWSP(lwsp2).has_value();
        }(),
        "LWSP test 2 failed");

    static_assert( // 不能单独  CR LF
        not[] {
            constexpr OCTET lwsp3[] = {CR, LF};
            return LWSP(lwsp3).has_value();
        }(),
        "LWSP test 3 failed");

    // CRLF 测试
    static_assert(
        [] {
            constexpr OCTET crlf1[] = {CR, LF};
            return CRLF(crlf1).has_value();
        }(),
        "CRLF test 1 failed");

    static_assert(
        [] {
            constexpr OCTET crlf2[] = {CR, 'a'};
            return !CRLF(crlf2).has_value();
        }(),
        "CRLF test 2 failed");

    {
        { // ALPHA测试
            static_assert(ALPHA('A') && ALPHA('Z'));
            static_assert(ALPHA('a') && ALPHA('z'));
            static_assert(!ALPHA('0') && !ALPHA('@'));
        }

        { // BIT测试
            static_assert(BIT('0') && BIT('1'));
            static_assert(!BIT('2') && !BIT('a'));
        }

        { // CHAR测试
            static_assert(CHAR(0x01) && CHAR(0x7F));
            static_assert(!CHAR(0x00) && !CHAR(0x80));
        }

        { // CTL测试
            static_assert(CTL(0x00) && CTL(0x1F));
            static_assert(CTL(0x7F) && !CTL(0x20));
        }

        { // DIGIT测试
            static_assert(DIGIT('0') && DIGIT('9'));
            static_assert(!DIGIT('/') && !DIGIT(':'));
        }

        { // HEXDIG测试
            static_assert(HEXDIG('0') && HEXDIG('9'));
            static_assert(HEXDIG('A') && HEXDIG('F'));
            static_assert(HEXDIG('a') && HEXDIG('f'));
            static_assert(!HEXDIG('G') && !HEXDIG('@'));
        }

        { // CRLF测试
            static constexpr OCTET validCRLF[] = {CR, LF};
            static constexpr OCTET invalidCRLF1[] = {CR, 'A'};
            static constexpr OCTET invalidCRLF2[] = {'A', LF};
            static_assert(CRLF(validCRLF).has_value());
            static_assert(!CRLF(invalidCRLF1).has_value());
            static_assert(!CRLF(invalidCRLF2).has_value());
        }

        { // WSP测试
            static_assert(WSP(SP) && WSP(HTAB));
            static_assert(!WSP('A') && !WSP('\n'));
        }

        { // LWSP测试
            static constexpr OCTET validLWSP1[] = {SP, HTAB};
            static constexpr OCTET validLWSP2[] = {CR, LF, SP};
            static constexpr OCTET invalidLWSP[] = {'A', SP};
            static_assert(LWSP(validLWSP1).has_value());
            static_assert(LWSP(validLWSP2).has_value());
            static_assert(!LWSP(invalidLWSP).has_value());
        }

        { // VCHAR测试
            static_assert(VCHAR('!') && VCHAR('~'));
            static_assert(!VCHAR(' ') && !VCHAR(0x7F));
        }
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND