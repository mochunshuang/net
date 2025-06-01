#pragma once

#include <system_error>

#if defined(_MSC_VER)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <ws2tcpip.h> // 包含 InetNtop 函数的头文件

#pragma comment(lib, "ws2_32.lib")

namespace mcs::protocol::ip
{

#define OS_DEF_AF_INET AF_INET
#define OS_DEF_AF_INET6 AF_INET6
#define OS_DEF_SOCK_STREAM SOCK_STREAM
#define OS_DEF_IPPROTO_TCP IPPROTO_TCP
#define OS_DEF_SOCK_DGRAM SOCK_DGRAM
#define OS_DEF_IPPROTO_UDP IPPROTO_UDP

    using os_socket_type = ::SOCKET;
    constexpr inline auto os_invalid_socket = INVALID_SOCKET; // NOLINT

    constexpr auto os_closesocket(os_socket_type s) noexcept
    {
        return ::closesocket(s);
    }

    [[noreturn]] constexpr void os_throw_socket_last_error(const char *msg)
    {
        throw std::system_error(::WSAGetLastError(), std::system_category(), msg);
    }
}; // namespace mcs::protocol::ip

#else

namespace mcs::protocol::ip
{
#ifndef AF_INET
#define AF_INET 2
#endif
#ifndef AF_INET6
#define AF_INET6 23
#endif
#ifndef SOCK_STREAM
#define SOCK_STREAM 1
#endif
#ifndef IPPROTO_TCP
#define IPPROTO_TCP 6
#endif
#ifndef SOCK_DGRAM
#define SOCK_DGRAM 2
#endif
#ifndef IPPROTO_UDP
#define IPPROTO_UDP 17
#endif

// 使用与Windows相同的宏定义
#define OS_DEF_AF_INET AF_INET
#define OS_DEF_AF_INET6 AF_INET6
#define OS_DEF_SOCK_STREAM SOCK_STREAM
#define OS_DEF_IPPROTO_TCP IPPROTO_TCP
#define OS_DEF_SOCK_DGRAM SOCK_DGRAM
#define OS_DEF_IPPROTO_UDP IPPROTO_UDP

    // Linux使用int作为socket类型
    using os_socket_type = int;
    constexpr inline auto os_invalid_socket = -1; // Linux中无效socket通常用-1表示

    constexpr auto os_closesocket(os_socket_type)
    {
        throw;
    }

    [[noreturn]] constexpr void os_throw_socket_last_error(const char *msg)
    {
        throw std::system_error(errno, std::system_category(), msg);
    }
} // namespace mcs::protocol::ip

#endif