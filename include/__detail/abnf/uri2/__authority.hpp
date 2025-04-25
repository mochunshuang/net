#pragma once

#include "../detail/__types.hpp"
#include "./__userinfo.hpp"
#include "./__host.hpp"
#include "./__port.hpp"
#include <optional>
#include <string>
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

        static constexpr auto parse(detail::parser_ctx ctx) noexcept
            -> std::optional<result_type>
        {

            return std::nullopt;
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

            if (not ctx.port)
                authority.append(":").append(std::to_string(ctx.port.value()));
            return authority;
        }
    };

}; // namespace mcs::abnf::uri
