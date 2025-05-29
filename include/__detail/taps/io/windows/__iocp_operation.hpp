#pragma once

#include "./__iocp_operation_base.hpp"

namespace mcs::net::io::windows
{

    // NOTE: 不能约束 impl_type。 因为 impl_type 是 模板iocp_operation的子类。循环约束
    template <typename impl_type>
    struct iocp_operation : iocp_operation_base
    {
        explicit constexpr iocp_operation(io_operation_context_base &context) noexcept
            : iocp_operation_base{impl_type::io_type,
                                  operation_callback_base{
                                      .complete = &iocp_operation::invoke,
                                      .complete_error = &iocp_operation::invoke_error},
                                  context}

        {
            assert(this->context.socket != INVALID_SOCKET);
        }

      private:
        constexpr static void invoke(iocp_operation_base *self) noexcept
        {
            /**
            NOTE: Undefined behavior may be

               struct B {};
               struct D : B { B b };
               D d;
               B& br1 = d;
               B& br2 = d.b;

               static_cast<D&>(br1); // OK
               static_cast<D&>(br2); // Undefined behavior
            */
            impl_type::complete(static_cast<impl_type *>(self));
        }
        // NOLINTNEXTLINE
        constexpr static void invoke_error(iocp_operation_base *self,
                                           std::error_code code) noexcept
        {
            impl_type::complete_error(static_cast<impl_type *>(self), code);
        }
    };
}; // namespace mcs::net::io::windows