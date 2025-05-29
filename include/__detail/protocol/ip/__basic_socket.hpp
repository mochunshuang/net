#pragma once

#include "./__detail/__sysytem_dependent.hpp"
#include <utility>

namespace mcs::protocol::ip
{
    template <typename Protocol>
    struct basic_socket
    {
      public:
        using protocol_type = Protocol;
        using endpoint_type = typename protocol_type::endpoint;

        // 构造/析构
        explicit basic_socket(protocol_type proto) noexcept
            : proto_(proto), handle_(os_invalid_socket)
        {
        }
        ~basic_socket() noexcept
        {
            if (is_open())
                close();
        }
        basic_socket(basic_socket &&other) noexcept
            : proto_(other.proto_),
              handle_(std::exchange(other.handle_, os_invalid_socket))
        {
        }
        basic_socket(const basic_socket &) = delete;
        basic_socket &operator=(const basic_socket &) = delete;
        basic_socket &operator=(basic_socket &&) = delete;

        [[nodiscard]] bool is_open() const noexcept // NOLINT
        {
            return handle_ != os_invalid_socket;
        }

        [[nodiscard]] os_socket_type native_handle() const noexcept // NOLINT
        {
            return handle_;
        }

        // 核心能力
        void open()
        {
            if (is_open())
                close();
            handle_ = os_socket_type(proto_.family(), proto_.type(), proto_.protocol());
            if (handle_ == os_invalid_socket)
                os_throw_socket_last_error("socket open() failed");
        }
        void close() noexcept
        {
            if (handle_ != os_invalid_socket)
            {
                os_closesocket(handle_);
            }
        }

      private:
        protocol_type proto_;
        os_socket_type handle_{os_invalid_socket};
    };

}; // namespace mcs::protocol::ip