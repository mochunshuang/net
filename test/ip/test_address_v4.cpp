#include <iostream>
#include <sstream>

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

// NOLINTBEGIN

#if defined(_MSC_VER)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <ws2tcpip.h> // 包含 InetNtop 函数的头文件

#pragma comment(lib, "ws2_32.lib")

class WinsockRAII
{
  public:
    WinsockRAII()
    {
        WSADATA wsaData;
        if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            throw std::runtime_error("WSAStartup failed");
        }
    }
    WinsockRAII(const WinsockRAII &) = delete;
    WinsockRAII(WinsockRAII &&) = delete;
    WinsockRAII &operator=(const WinsockRAII &) = delete;
    WinsockRAII &operator=(WinsockRAII &&) = delete;
    ~WinsockRAII() noexcept
    {
        ::WSACleanup();
    }
};

int main()
{
    using namespace mcs::net::ip; // NOLINT
    TEST("test address_v4 base") = [] {
        TEST("test_constructors") = [] {
            // 默认构造函数 (0.0.0.0)
            constexpr address_v4 addr1;
            static_assert(addr1.to_uint() == 0);
            static_assert(addr1.is_unspecified());

            // 从字节数组构造
            constexpr address_v4::bytes_type bytes{192, 168, 1, 1};
            constexpr address_v4 addr2(bytes);
            static_assert(addr2.to_uint() == 0xC0A80101);

            // 从32位整数构造
            constexpr address_v4 addr3(0x7F000001); // 127.0.0.1
            static_assert(addr3.is_loopback());
        };

        TEST("test_static_factories") = [] {
            // any() 测试
            constexpr auto any_addr = address_v4::any();
            static_assert(any_addr.to_uint() == 0);
            static_assert(any_addr.is_unspecified());

            // loopback() 测试
            constexpr auto loopback_addr = address_v4::loopback();
            static_assert(loopback_addr.to_uint() == 0x7F000001);
            static_assert(loopback_addr.is_loopback());

            // broadcast() 测试
            constexpr auto broadcast_addr = address_v4::broadcast();
            static_assert(broadcast_addr.to_uint() == 0xFFFFFFFF);
        };

        TEST("test_make_address_v4") = [] {
            // 从字符串构造
            constexpr auto addr1 = make_address_v4("192.168.1.1");
            static_assert(addr1.to_uint() == 0xC0A80101);

            // 错误处理测试
            std::error_code ec;
            auto addr2 = make_address_v4("256.0.0.1", ec);
            EXPECT(ec.value());
            EXPECT(addr2.is_unspecified());

            try
            {
                auto addr3 = make_address_v4("invalid.address");
                EXPECT(false); // 不应该执行到这里
            }
            catch (const std::invalid_argument &)
            {
                // 预期异常
            }
            catch (...)
            {
                UNEXPECT("error catch");
            }
        };

        TEST("test_methods") = [] {
            // to_bytes() 测试
            constexpr address_v4 addr(0xC0A80101); // 192.168.1.1
            constexpr auto bytes = addr.to_bytes();
            static_assert(bytes[0] == 192 && bytes[1] == 168 && bytes[2] == 1 &&
                          bytes[3] == 1);

            // to_string() 测试
            EXPECT(addr.to_string() == "192.168.1.1");

            // is_multicast() 测试
            constexpr address_v4 multicast_addr(0xE0000001); // 224.0.0.1
            static_assert(multicast_addr.is_multicast());
        };

        TEST("test_comparisons") = [] {
            constexpr address_v4 addr1(0xC0A80101); // 192.168.1.1
            constexpr address_v4 addr2(0xC0A80102); // 192.168.1.2
            constexpr address_v4 addr3(0xC0A80101); // 192.168.1.1

            static_assert(addr1 == addr3);
            static_assert(addr1 != addr2);
            static_assert(addr1 < addr2);
            static_assert(addr2 > addr1);
        };

        TEST("test_stream_output") = [] {
            address_v4 addr(0xC0A80101); // 192.168.1.1
            std::ostringstream oss;
            oss << addr;
            EXPECT(oss.str() == "192.168.1.1");
        };
    };

    std::cout << "main done\n";
    return 0;
}

#else

int main()
{
    std::cout << "main done\n";
    return 0;
}

#endif
// NOLINTEND