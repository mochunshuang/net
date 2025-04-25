#pragma once

#include "../detail/__types.hpp"
#include <string>

namespace mcs::abnf::uri
{
    // userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    struct userinfo
    {
        struct __type
        {
            using domain = userinfo;
            detail::absolute_span value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string userinfo;
            return userinfo;
        }
    };

}; // namespace mcs::abnf::uri