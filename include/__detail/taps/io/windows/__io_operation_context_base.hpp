#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>

namespace mcs::net::io::windows
{
    using windows_socket_type = ::SOCKET;

    struct io_operation_context_base
    {
        windows_socket_type socket{INVALID_SOCKET};
        ::WSABUF wsabuf{};
        ::DWORD bytes_transferred{};
    };

    // listen
    struct io_operation_accept_context : io_operation_context_base
    {
        io_operation_accept_context(io_operation_context_base base,
                                    windows_socket_type listen_socket) noexcept
            : io_operation_context_base{base}, listen_socket{listen_socket}
        {
        }
        windows_socket_type listen_socket{INVALID_SOCKET}; // NOLINT
    };
}; // namespace mcs::net::io::windows