#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>

namespace mcs::net::io::windows
{
    struct io_operation_context_base
    {
        ::SOCKET socket{INVALID_SOCKET};
        ::WSABUF wsabuf{};
        ::DWORD bytes_transferred{};
    };
}; // namespace mcs::net::io::windows