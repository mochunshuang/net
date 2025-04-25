#pragma once

#include "../detail/__types.hpp"
#include <string>

namespace mcs::abnf::uri
{
    // scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    struct scheme
    {
        struct __type
        {
            using domain = scheme;
            detail::absolute_span value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }

        static constexpr auto build(const __type &ctx)
        {
            std::string scheme;

            return scheme;
        }
    };

}; // namespace mcs::abnf::uri