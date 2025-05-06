#pragma once

#include "../detail/__types.hpp"
#include "./__userinfo.hpp"
#include "./__host.hpp"
#include "./__port.hpp"
#include <optional>
#include <string>
#include <type_traits>

namespace mcs::abnf::uri
{
    // authority     = [ userinfo "@" ] host [ ":" port ]
    struct authority
    {
      private:
        struct __type
        {
            using domain = authority;
            using userinfo_t = uri::userinfo::result_type;
            using host_t = uri::host::result_type;
            using port_t = uri::port::result_type;

            std::optional<userinfo_t> userinfo;
            host_t host;
            std::optional<port_t> port;
        };

      public:
        using result_type = __type;
        using rule_concept = rule_t;

        static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
        {
            using rule = sequence<optional<sequence<userinfo, CharSensitive<'@'>>>, host,
                                  optional<sequence<CharSensitive<':'>, port>>>;
            auto ret = rule{}(ctx);
            return ret ? detail::make_consumed_result(*ret) : std::nullopt;
        }

        static constexpr auto parse(parser_ctx_ref ctx) noexcept
            -> std::optional<result_type>
        {
            constexpr auto k_rule = sequence{
                make_optional{sequence{userinfo{}, CharRule<CharSensitive<'@'>>{}}},
                host{}, make_optional{sequence{CharRule<CharSensitive<':'>>{}, port{}}}};
            auto ret = k_rule.parse(ctx);
            if (not ret)
                return std::nullopt;
            result_type result;
            if (auto &userinfo = ret->value.get<0>(); userinfo.index() != 0)
            {
                auto &ret = std::get<1>(userinfo);
                static_assert(std::is_same_v<std::decay_t<decltype(ret.value.get<0>())>,
                                             result_type::userinfo_t>);
                result.userinfo = ret.value.get<0>();
            }
            result.host = ret->value.get<1>();
            if (auto &port = ret->value.get<2>(); port.index() != 0)
                result.port = std::get<1>(port).value.get<1>();
            return result;
        }
        static constexpr auto buildString(const result_type &ctx) noexcept
        {
            std::string authority;
            if (ctx.userinfo)
                authority
                    .append(result_type::userinfo_t::domain::buildString(*(ctx.userinfo)))
                    .append("@");

            authority.append(result_type::host_t::domain::buildString(ctx.host));

            // NOTE: 检验端口大小
            if (ctx.port)
                authority.append(":").append(
                    result_type::port_t::domain::buildString(*(ctx.port)));
            return authority;
        }
    };

}; // namespace mcs::abnf::uri
