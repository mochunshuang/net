#pragma once

#include "../detail/__types.hpp"
#include "./__scheme.hpp"
#include "./__hier_part.hpp"
#include "./__query.hpp"
#include "./__fragment.hpp"
#include <optional>
#include <string>

namespace mcs::abnf::uri
{
    // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    struct URI
    {
        struct __type
        {
            using domain = URI;
            using scheme_t = uri::scheme::result_type;
            using hier_part_t = uri::hier_part::result_type;
            using query_t = uri::query::result_type;
            using fragment_t = uri::fragment::result_type;

            scheme_t scheme;
            hier_part_t hier_part;
            std::optional<query_t> query;
            std::optional<fragment_t> fragment;
        };
        using result_type = __type;
        using rule_concept = rule_t;

        static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
        {
            constexpr auto k_rule =
                sequence<scheme, CharInsensitive<':'>, hier_part,
                         optional<sequence<CharInsensitive<'?'>, query>>,
                         optional<sequence<CharInsensitive<'#'>, fragment>>>{};
            auto ret = k_rule(ctx);
            return ret ? make_consumed_result(*ret) : std::nullopt;
        }

        static constexpr auto parse(parser_ctx_ref ctx) -> std::optional<result_type>
        {
            constexpr auto k_rule =
                sequence<scheme, CharInsensitive<':'>, hier_part,
                         make_optional<sequence<CharInsensitive<'?'>, query>>,
                         make_optional<sequence<CharInsensitive<'#'>, fragment>>>{};
            auto ret = k_rule.parse(ctx);
            if (not ret)
                return std::nullopt;
            result_type result;
            auto &ref = (*ret).value;
            result.scheme = ref.get<0>();
            result.hier_part = ref.get<2>();
            if (ref.get<3>().index() == 1)
            {
                auto &sequence = std::get<1>(ref.get<3>()).value;
                result.query = sequence.get<1>();
            }
            if (ref.get<4>().index() == 1)
                result.fragment = std::get<1>(ref.get<4>()).value.get<1>();
            return result;
        }

        static constexpr auto buildString(const result_type &ctx) noexcept
        {
            std::string build;
            build.append(result_type::scheme_t::domain::buildString(ctx.scheme))
                .append(":")
                .append(result_type::hier_part_t::domain::buildString(ctx.hier_part));
            if (ctx.query)
                build.append("?").append(
                    result_type::query_t::domain::buildString(*ctx.query));
            if (ctx.fragment)
                build.append("#").append(
                    result_type::fragment_t::domain::buildString(*ctx.fragment));
            return build;
        }
    };
}; // namespace mcs::abnf::uri