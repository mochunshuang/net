#pragma once

#include "../detail/__types.hpp"
#include <string>

namespace mcs::abnf::uri
{
    //  reg-name      = *( unreserved / pct-encoded / sub-delims )
    struct reg_name
    {
        struct __type
        {
            using domain = reg_name;
            detail::absolute_span value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string reg_name;
            return reg_name;
        }
    };

}; // namespace mcs::abnf::uri