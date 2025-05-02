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
            using scheme_t = scheme::result_type;
            using hier_part_t = hier_part::result_type;
            using query_t = query::result_type;
            using fragment_t = fragment::result_type;

            scheme_t scheme;
            hier_part_t hier_part;
            std::optional<query_t> query;
            std::optional<fragment_t> fragment;
        };
        using result_type = __type;

        static constexpr auto parse(parser_ctx_ref ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }

        static constexpr auto build(const __type &ctx)
        {
            struct URI
            {
                std::string scheme;
                std::string hier_part;
                std::string query;
                std::string fragment;
            };

            return std::nullopt;
        }
    };
}; // namespace mcs::abnf::uri