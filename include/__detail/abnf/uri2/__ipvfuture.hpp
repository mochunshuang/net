#pragma once

#include "../detail/__types.hpp"
#include <string>

namespace mcs::abnf::uri
{
    // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    struct IPvFuture
    {
        struct __type
        {
            using domain = IPvFuture;
            detail::absolute_span value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string IPvFuture;
            return IPvFuture;
        }
    };

}; // namespace mcs::abnf::uri