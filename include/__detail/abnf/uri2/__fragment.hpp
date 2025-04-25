#pragma once

#include "../detail/__types.hpp"
#include <string>

namespace mcs::abnf::uri
{
    // fragment      = *( pchar / "/" / "?" )
    struct fragment
    {
        struct __type
        {
            using domain = fragment;
            detail::absolute_span value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string fragment;
            return fragment;
        }
    };

}; // namespace mcs::abnf::uri