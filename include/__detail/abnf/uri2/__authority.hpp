#pragma once

#include "../detail/__types.hpp"
#include "./__userinfo.hpp"
#include "./__host.hpp"
#include "./__port.hpp"
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

namespace mcs::abnf::uri
{
    // authority     = [ userinfo "@" ] host [ ":" port ]
    struct authority
    {
      private:
        struct __type
        {
            using domain = authority;
            using userinfo_t = userinfo::result_type;
            using host_t = host::result_type;
            using port_t = port::result_type;

            std::optional<userinfo_t> userinfo;
            host_t host;
            std::optional<port_t> port;
        };
        struct authority_t
        {
            using port_type = decltype(port::build(std::declval<__type::port_t>()));
            using userinfo_type =
                decltype(userinfo::build(std::declval<__type::userinfo_t>()));

            std::optional<userinfo_type> userinfo;
            decltype(host::build(std::declval<__type::host_t>())) host;
            std::optional<port_type> port;
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
            auto rule = make_sequence{
                make_optional{make_sequence{userinfo{}, CharRule<CharSensitive<'@'>>{}}},
                host{},
                make_optional{make_sequence{CharRule<CharSensitive<':'>>{}, port{}}}};
            auto ret = rule.parse(ctx);
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
        static constexpr auto build(const result_type &ctx) noexcept
        {
            return authority_t{
                .userinfo = ctx.userinfo.has_value()
                                ? std::optional<authority_t::userinfo_type>(
                                      userinfo::build(ctx.userinfo.value()))
                                : std::nullopt,
                .host = host::build(ctx.host),
                .port = ctx.port.has_value() ? std::optional<authority_t::port_type>(
                                                   port::build(ctx.port.value()))
                                             : std::nullopt};
        }
        static constexpr auto toString(const authority_t &ctx) noexcept
        {
            std::string authority;
            if (not ctx.userinfo)
                authority.append(ctx.userinfo.value()).append("@");

            authority.append(ctx.host);

            // NOTE: 检验端口大小
            if (not ctx.port)
                authority.append(":").append(ctx.port.value());
            return authority;
        }
    };

}; // namespace mcs::abnf::uri
