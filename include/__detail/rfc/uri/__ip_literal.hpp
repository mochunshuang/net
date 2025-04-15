#pragma once

#include "./__ipv6address.hpp"
#include "./__ipvfuture.hpp"
#include <cstdint>
#include <variant>

namespace mcs::rfc::uri
{
    struct IP_literal
    {
        enum type : id_type
        {
            IPV6ADDRESS,
            IPVFUTURE
        };
        // TODO(mcs): 或许3个构造更符合 move 是比较轻量级的操作
        // 或许使用初始化列表
        // 或许使用模板 是最好的。 或许是初始化列表？
        IP_literal(id_type id, span_param_in sp, id_type cid = 0) noexcept
        {
            switch (id)
            {
            case IPV6ADDRESS:
                impl.emplace<IPv6address>(cid, sp);
                break;
            case IPVFUTURE:
                impl.emplace<IPvFuture>(sp);
                break;
            default:
                impl = std::monostate{};
                break;
            }
        }
        std::variant<std::monostate, IPv6address, IPvFuture> impl; // NOLINT
    };

    constexpr ReturnType<IP_literal> make_IP_literal(span_param_in sp) noexcept
    {
        // TODO(mcs): 多种高级语义化的过程时，给个id是自然的
        if (parse::IPvFuture(sp))
            return IP_literal{0, sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri