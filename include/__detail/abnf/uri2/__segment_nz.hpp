#pragma once

#include "../detail/__types.hpp"
#include <string>

namespace mcs::abnf::uri
{
    // segment-nz    = 1*pchar
    struct segment_nz
    {
        struct __type
        {
            using domain = segment_nz;
            detail::absolute_span value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }

        static constexpr auto build(const __type &ctx)
        {
            std::string segment_nz;
            return segment_nz;
        }
    };

}; // namespace mcs::abnf::uri