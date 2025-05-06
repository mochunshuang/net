#pragma once

#include "../detail/__types.hpp"
#include "./__ipv6address.hpp"
#include "./__ipvfuture.hpp"
#include <variant>

namespace mcs::abnf::uri
{
    // IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
    struct IP_literal
    {
        struct __type
        {
            using domain = IP_literal;
            using IPv6address_t = uri::IPv6address::result_type;
            using IPvFuture_t = uri::IPvFuture::result_type;
            std::variant<std::monostate, IPv6address_t, IPvFuture_t> value;
        };
        using result_type = __type;
        using rule_concept = rule_t;

        static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
        {
            using rule =
                sequence<CharInsensitive<'['>, alternative<IPv6address, IPvFuture>,
                         CharInsensitive<']'>>;
            constexpr auto k_rule = rule{};
            auto ret = k_rule(ctx);
            return ret ? detail::make_consumed_result(*ret) : std::nullopt;
        }
        static constexpr auto parse(parser_ctx_ref ctx) noexcept
            -> std::optional<result_type>
        {
            auto begin{ctx.cur_index};
            auto id = 0;
            auto IPv6address_callback = [&](const parser_ctx & /*ctx*/) noexcept {
                id = 1;
            };
            auto rule =
                sequence{CharInsensitive<'['>{},
                         alternative{with_callback(IPv6address{}, IPv6address_callback),
                                     IPvFuture{}},
                         CharInsensitive<']'>{}};
            if (auto ret = rule(ctx))
            {
                return id == 1
                           ? result_type{.value =
                                             result_type::IPv6address_t{
                                                 .value =
                                                     ctx.root_span.subspan(begin, *ret)}}
                           : result_type{
                                 .value = result_type::IPvFuture_t{
                                     .value = ctx.root_span.subspan(begin, *ret)}};
            }
            return std::nullopt;
        }

        static constexpr auto buildString(const result_type &ctx) noexcept
        {
            if (ctx.value.index() == 1)
                return result_type::IPv6address_t::domain::buildString(
                    std::get<1>(ctx.value));
            if (ctx.value.index() == 2)
                return result_type::IPvFuture_t::domain::buildString(
                    std::get<2>(ctx.value));
            std::string ip_literal;
            return ip_literal;
        }
    };

}; // namespace mcs::abnf::uri