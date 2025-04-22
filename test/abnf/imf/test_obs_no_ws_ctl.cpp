#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效值测试
    static_assert(obs_NO_WS_CTL(0x01), "1 should be valid");   // 1
    static_assert(obs_NO_WS_CTL(0x08), "8 should be valid");   // 8
    static_assert(obs_NO_WS_CTL(0x0B), "11 should be valid");  // 11
    static_assert(obs_NO_WS_CTL(0x0C), "12 should be valid");  // 12
    static_assert(obs_NO_WS_CTL(0x0E), "14 should be valid");  // 14
    static_assert(obs_NO_WS_CTL(0x1F), "31 should be valid");  // 31
    static_assert(obs_NO_WS_CTL(0x7F), "127 should be valid"); // 127
    static_assert(obs_NO_WS_CTL(0x05), "5 should be valid");   // 中间值
    static_assert(obs_NO_WS_CTL(0x14), "20 should be valid");  // 中间值

    // 无效值测试
    static_assert(!obs_NO_WS_CTL(0x00), "0 should be invalid");
    static_assert(!obs_NO_WS_CTL(0x09), "9 should be invalid");
    static_assert(!obs_NO_WS_CTL(0x0A), "10 (LF) should be invalid");
    static_assert(!obs_NO_WS_CTL(0x0D), "13 (CR) should be invalid");
    static_assert(!obs_NO_WS_CTL(0x20), "32 (space) should be invalid");
    static_assert(!obs_NO_WS_CTL(0x40), "64 (@) should be invalid");
    static_assert(!obs_NO_WS_CTL(0x7E), "126 (~) should be invalid");
    static_assert(!obs_NO_WS_CTL(0x80), "128 should be invalid");
    static_assert(!obs_NO_WS_CTL(0xFF), "255 should be invalid");

    return 0;
}
// NOLINTEND