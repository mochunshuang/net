#pragma once

#include <system_error>

namespace mcs::net::io::windows
{
    struct iocp_operation_base;

    struct operation_callback_base
    {
        using callback_fun_t = void (*)(iocp_operation_base *self) noexcept;
        using callback_error_t = void (*)(iocp_operation_base *self,
                                          std::error_code err_code) noexcept;

        callback_fun_t complete{};
        callback_error_t complete_error{};
    };

}; // namespace mcs::net::io::windows