#pragma once

#include <cassert>
#if defined(_MSC_VER)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// WIN32_LEAN_AND_MEAN宏阻止Windows.h头文件包含Winsock.h。

#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <ws2tcpip.h> // 包含 InetNtop 函数的头文件

#pragma comment(lib, "ws2_32.lib")

#include "../__io_type.hpp"
#include "./__operation_callback_base.hpp"
#include "./__io_operation_context_base.hpp"

#include <type_traits>

namespace mcs::net::io::windows
{
    struct iocp_operation_base : ::OVERLAPPED
    {
        explicit constexpr iocp_operation_base(io::io_type type,
                                               operation_callback_base callback,
                                               io_operation_context_base &ctx) noexcept
            : OVERLAPPED{}, op_type{type}, callbacks{callback}, context{ctx}
        {
            assert(Internal == 0);
            assert(Offset == 0);     // NOLINT
            assert(OffsetHigh == 0); // NOLINT
            assert(hEvent == nullptr);
        }
        io_type op_type;                    // NOLINT
        operation_callback_base callbacks;  // NOLINT
        io_operation_context_base &context; // NOLINT
    };

    // NOTE: 继承破坏标准布局。 但是更简洁
    static_assert(not std::is_standard_layout_v<iocp_operation_base>,
                  "Must be standardlayout");
}; // namespace mcs::net::io::windows

#endif