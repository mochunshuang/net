#pragma once

#include "./__ip_literal.hpp"
#include "./__ipv4address.hpp"
#include "./__reg_name.hpp"

namespace mcs::rfc::uri
{
    struct host
    {
        enum type : id_type
        {
            IP_LITERAL,
            IPV4ADDRESS,
            REG_NAME
        };
        host(id_type id, span_param_in sp, id_type cid = 0) noexcept
        {
            switch (id)
            {
            case IP_LITERAL:
                impl.emplace<IP_literal>(cid, sp);
                break;
            case IPV4ADDRESS:
                impl.emplace<IPv4address>(sp);
                break;
            case REG_NAME:
                impl.emplace<reg_name>(sp);
                break;
            default:
                impl = std::monostate{};
                break;
            }
        }

        // NOLINTNEXTLINE
        std::variant<std::monostate, IP_literal, IPv4address, reg_name> impl;
    };

    constexpr ReturnType<IP_literal> make_host(span_param_in sp) noexcept
    {
        // TODO(mcs): variant 需要id
        // 简化接口，可以从 std::uint8_t cid = 0 得到。 或者从 variant 自身得到
        if (parse::IPvFuture(sp))
            return IP_literal{0, sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri