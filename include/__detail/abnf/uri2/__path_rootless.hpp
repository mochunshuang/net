#pragma once

#include "../detail/__types.hpp"
#include "./__segment.hpp"
#include "./__segment_nz.hpp"
#include <optional>

namespace mcs::abnf::uri
{
    // path-rootless = segment-nz *( "/" segment )
    struct path_rootless
    {
        struct __type
        {
            using domain = path_rootless;
            using segment_t = segment::result_type;
            using segment_nz_t = segment_nz::result_type;

            segment_nz_t segment_nz_; // NOLINT
            detail::absolute_span segments;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string path_rootless;
            return path_rootless;
        }
    };

}; // namespace mcs::abnf::uri