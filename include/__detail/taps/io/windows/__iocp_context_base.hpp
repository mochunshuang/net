#pragma once

#include "./__iocp_operation_base.hpp"
#include <cassert>
#include <expected>
#include <print>
#include <string>

#if defined(_MSC_VER)

namespace mcs::net::io::windows
{
    using os_sockaddr_storage_in_type = ::sockaddr_in6;
    static constexpr auto SOCKADDRIN_SIZE = sizeof(os_sockaddr_storage_in_type); // NOLINT

    // https://learn.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-acceptex#:~:text=%5Bin%5D-,dwLocalAddressLength,-The%20number%20of
    static constexpr auto ADDRESS_BUFFER_SIZE = SOCKADDRIN_SIZE + 16; // NOLINT

    template <typename T>
    concept iocp_context = requires(const T &ctx) {
        { ctx.iocp_handle() } noexcept -> std::convertible_to<::HANDLE>;
        { ctx.listen_socket() } noexcept -> std::convertible_to<::SOCKET>;
        { ctx.pfn_acceptex() } noexcept -> std::convertible_to<::LPFN_ACCEPTEX>;
    };
    struct iocp_context_base
    {
        using socket_type = windows_socket_type;

        struct endpoint_info
        {
            using port_type = std::uint_least16_t;
            std::string ip_address; // NOLINT
            port_type port;         // NOLINT

            [[nodiscard]] std::string to_string() const noexcept // NOLINT
            {
                return ip_address + ":" + std::to_string(port);
            }
        };

        struct connection_info
        {
            endpoint_info local;  // NOLINT
            endpoint_info remote; // NOLINT

            [[nodiscard]] std::string to_string() const noexcept // NOLINT
            {
                return "Local: " + local.to_string() + ", Remote: " + remote.to_string();
            }
        };

        struct rawconnection
        {
            socket_type socket{INVALID_SOCKET};
            connection_info info;
        };

        struct sockaddr_storage_type
        {
            ::sockaddr_storage addr = {};
            int addr_len = 0;
        };

        static constexpr connection_info parse_connection( // NOLINT
            LPFN_GETACCEPTEXSOCKADDRS pfnGetAcceptExSockaddrs, void *buffer) noexcept
        {
            sockaddr *local_addr = nullptr;
            sockaddr *remote_addr = nullptr;
            int local_len = 0;
            int remote_len = 0;

            pfnGetAcceptExSockaddrs(buffer,
                                    0, // 无接收数据
                                    ADDRESS_BUFFER_SIZE, ADDRESS_BUFFER_SIZE, &local_addr,
                                    &local_len, &remote_addr, &remote_len);

            return {.local = parse_endpoint(local_addr),
                    .remote = parse_endpoint(remote_addr)};
        }

        static constexpr std::string ip_to_string(const sockaddr *addr) noexcept // NOLINT
        {
            char buffer[INET6_ADDRSTRLEN]{}; // NOLINT

            if (addr->sa_family == AF_INET)
            {
                const auto *a = reinterpret_cast<const sockaddr_in *>(addr);
                ::inet_ntop(AF_INET, &a->sin_addr, buffer, sizeof(buffer));
            }
            else if (addr->sa_family == AF_INET6)
            {
                const auto *a = reinterpret_cast<const sockaddr_in6 *>(addr);
                ::inet_ntop(AF_INET6, &a->sin6_addr, buffer, sizeof(buffer));
            }
            return buffer;
        }

        // NOLINTNEXTLINE
        static constexpr endpoint_info parse_endpoint(const sockaddr *addr) noexcept
        {
            if (addr == nullptr)
                return {.ip_address = "", .port = 0};

            // 处理 IPv4 映射地址
            if (addr->sa_family == AF_INET6)
            {
                const auto *addr6 = reinterpret_cast<const sockaddr_in6 *>(addr);
                if (TRUE == ::IN6_IS_ADDR_V4MAPPED(&addr6->sin6_addr))
                {
                    sockaddr_in addr4{};
                    addr4.sin_family = AF_INET;
                    addr4.sin_port = addr6->sin6_port;
                    ::memcpy(&addr4.sin_addr, &addr6->sin6_addr.s6_addr[12], 4);
                    return {.ip_address =
                                ip_to_string(reinterpret_cast<const sockaddr *>(&addr4)),
                            .port = ::ntohs(addr4.sin_port)};
                }
            }

            // 常规地址解析
            if (addr->sa_family == AF_INET)
            {
                const auto *a = reinterpret_cast<const sockaddr_in *>(addr);
                return {.ip_address = ip_to_string(addr), .port = ::ntohs(a->sin_port)};
            }
            if (addr->sa_family == AF_INET6)
            {
                const auto *a = reinterpret_cast<const sockaddr_in6 *>(addr);
                return {.ip_address = ip_to_string(addr), .port = ::ntohs(a->sin6_port)};
            }

            return {.ip_address = "Unknown", .port = 0};
        }

        static constexpr auto get_acceptex_ptr(::SOCKET listen_socket) // NOLINT

        {
            ::LPFN_ACCEPTEX pfn = nullptr;
            ::GUID guidAcceptEx = WSAID_ACCEPTEX;
            // https://learn.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-acceptex#:~:text=lib%20library.%0A%20%20%20%20iResult%20%3D-,WSAIoctl,-(ListenSocket%2C%20SIO_GET_EXTENSION_FUNCTION_POINTER%2C%0A%20%20%20%20%20%20%20%20%20%20%20%20%20%26GuidAcceptEx
            if (::DWORD bytes = 0;
                SOCKET_ERROR == ::WSAIoctl(listen_socket,
                                           SIO_GET_EXTENSION_FUNCTION_POINTER,
                                           &guidAcceptEx, sizeof(guidAcceptEx),
                                           reinterpret_cast<::LPVOID>(&pfn), sizeof(pfn),
                                           &bytes, nullptr, nullptr))
                throw std::system_error(::WSAGetLastError(), std::system_category());
            return pfn;
        }

        static constexpr auto get_accpetex_sockaddrs_ptr(::SOCKET listen_socket) // NOLINT
        {
            LPFN_GETACCEPTEXSOCKADDRS pfnGetAcceptExSockaddrs{};
            GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
            if (DWORD bytes = 0;
                SOCKET_ERROR ==
                ::WSAIoctl(listen_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                           &guidGetAcceptExSockaddrs, sizeof(guidGetAcceptExSockaddrs),
                           &pfnGetAcceptExSockaddrs, sizeof(pfnGetAcceptExSockaddrs),
                           &bytes, nullptr, nullptr))
                throw std::system_error(::WSAGetLastError(), std::system_category());
            return pfnGetAcceptExSockaddrs;
        }

        // NOLINTNEXTLINE
        static constexpr auto post_accept(LPFN_ACCEPTEX pfn_acceptex,
                                          iocp_operation_base *op) noexcept
            -> std::expected<void, std::error_code>
        {
            DWORD bytes = 0;
            auto &accept_context =
                static_cast<io::windows::io_operation_accept_context &>(op->context);
            // https://learn.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-acceptex
            // https://learn.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-acceptex#:~:text=be%20NULL.-,Return%20value,-If%20no%20error
            if (FALSE == pfn_acceptex(accept_context.listen_socket, accept_context.socket,
                                      accept_context.wsabuf.buf, 0, ADDRESS_BUFFER_SIZE,
                                      ADDRESS_BUFFER_SIZE, &bytes,
                                      static_cast<::OVERLAPPED *>(op)))
            {
                if (const ::DWORD k_error = ::WSAGetLastError();
                    k_error != ERROR_IO_PENDING)
                    return std::unexpected(std::error_code(static_cast<int>(k_error),
                                                           std::system_category()));
            }
            return {};
        }

        static constexpr auto post_read(iocp_operation_base *op) noexcept // NOLINT
            -> std::expected<void, std::error_code>
        {
            auto &ctx = op->context;
            DWORD flags = 0;
            // https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsarecv#:~:text=for%20nonoverlapped%20sockets).-,Return%20value,-If%20no%20error
            if (SOCKET_ERROR == ::WSARecv(ctx.socket, &ctx.wsabuf, 1, nullptr, &flags,
                                          static_cast<::OVERLAPPED *>(op), nullptr))
            {
                if (const ::DWORD k_error = ::WSAGetLastError();
                    k_error != ERROR_IO_PENDING)
                    return std::unexpected(std::error_code(static_cast<int>(k_error),
                                                           std::system_category()));
            }
            return {};
        }

        static constexpr auto post_write(iocp_operation_base *op) noexcept // NOLINT
            -> std::expected<void, std::error_code>
        {
            auto &ctx = op->context;
            // https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsasend#:~:text=for%20nonoverlapped%20sockets.-,Return%20value,-If%20no%20error
            if (SOCKET_ERROR == ::WSASend(ctx.socket, &ctx.wsabuf, 1, nullptr, 0,
                                          static_cast<::OVERLAPPED *>(op), nullptr))
            {
                if (const ::DWORD k_error = ::WSAGetLastError();
                    k_error != ERROR_IO_PENDING)
                    return std::unexpected(std::error_code(static_cast<int>(k_error),
                                                           std::system_category()));
            }
            return {};
        }
    };

}; // namespace mcs::net::io::windows

#endif