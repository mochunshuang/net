#pragma once

#include "../detail/__types.hpp"
#include <cstdint>

namespace mcs::abnf::uri
{
    // port          = *DIGIT
    struct port
    {
        struct __type
        {
            using domain = port;
            detail::absolute_span value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::uint16_t port{};
            return port;
        }
    };

}; // namespace mcs::abnf::uri