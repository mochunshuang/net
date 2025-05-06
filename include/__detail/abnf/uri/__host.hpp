#pragma once

#include "../detail/__types.hpp"
#include "./__ip_literal.hpp"
#include "./__ipv4address.hpp"
#include "./__reg_name.hpp"
#include <variant>

namespace mcs::abnf::uri
{
    //  host          = IP-literal / IPv4address / reg-name
    struct host
    {
        struct __type
        {
            using domain = host;
            using IP_literal_t = IP_literal::result_type;
            using IPv4address_t = IPv4address::result_type;
            using reg_name_t = reg_name::result_type;

            std::variant<std::monostate, IP_literal_t, IPv4address_t, reg_name_t> value;
        };
        using result_type = __type;
        using rule_concept = rule_t;

        static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
        {
            using rule = alternative<IP_literal, IPv4address, reg_name>;
            auto ret = rule{}(ctx);
            return ret ? detail::make_consumed_result(*ret) : std::nullopt;
        }

        /**
         * parse 应该增加回调。 make_sequence + rule //是从左到右 parse 的
         * parse 成功 ctx 也就转移成功。将运行时答案保存即可。 重写设计make_sequence 即可
         */
        static constexpr auto parse(parser_ctx_ref ctx) noexcept
            -> std::optional<result_type>
        {
            if (auto ret = IP_literal::parse(ctx))
                return result_type{.value = result_type::IP_literal_t{*ret}};
            if (auto ret = IPv4address::parse(ctx))
                return result_type{.value = result_type::IPv4address_t{*ret}};
            if (auto ret = reg_name::parse(ctx))
                return result_type{.value = result_type::reg_name_t{*ret}};
            return std::nullopt;
        }
        static constexpr auto buildString(const result_type &ctx) noexcept
        {
            if (ctx.value.index() == 1)
                return result_type::IP_literal_t::domain::buildString(
                    std::get<1>(ctx.value));
            if (ctx.value.index() == 2)
                return result_type::IPv4address_t::domain::buildString(
                    std::get<2>(ctx.value));
            if (ctx.value.index() == 3)
                return result_type::reg_name_t::domain::buildString(
                    std::get<3>(ctx.value));
            std::string host;
            return host;
        }
    };

}; // namespace mcs::abnf::uri