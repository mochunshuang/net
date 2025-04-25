#pragma once

#include "../detail/__types.hpp"
#include <string>

namespace mcs::abnf::uri
{
    // segment       = *pchar
    struct segment
    {
        struct __type
        {
            using domain = segment;
            detail::absolute_span value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const __type &ctx)
        {
            std::string segment;
            return segment;
        }
    };

}; // namespace mcs::abnf::uri