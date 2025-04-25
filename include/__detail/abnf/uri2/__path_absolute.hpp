#pragma once

#include "../detail/__types.hpp"
#include "./__segment.hpp"
#include "./__segment_nz.hpp"
#include <optional>

namespace mcs::abnf::uri
{
    // path-absolute = "/" [ segment-nz *( "/" segment ) ]
    struct path_absolute
    {
        struct __type
        {
            using domain = path_absolute;
            using segment_t = segment::result_type;
            using segment_nz_t = segment_nz::result_type;
            struct __t
            {
                segment_nz_t segment_nz_; // NOLINT
                detail::absolute_span segments;
            };
            std::optional<__t> value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string path_absolute;
            return path_absolute;
        }
    };

}; // namespace mcs::abnf::uri