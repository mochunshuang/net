#pragma once

#include "../detail/__types.hpp"
#include "./__segment.hpp"

namespace mcs::abnf::uri
{
    // path-abempty  = *( "/" segment )
    struct path_abempty
    {
        struct __type
        {
            using domain = path_abempty;
            using segment_t = segment::result_type;
            segment_t segment;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string path_abempty;
            return path_abempty;
        }
    };

}; // namespace mcs::abnf::uri