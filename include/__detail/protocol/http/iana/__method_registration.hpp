#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <array>

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::iana
{
    /**
     * @see
     https://www.rfc-editor.org/rfc/rfc9110.html#name-iana-considerations:~:text=18.2.-,Method%20Registration,-IANA%20has%20updated
        Method	Safe	Idempotent	Section
        CONNECT	no	    no	        9.3.6
        DELETE	no	    yes	        9.3.5
        GET	    yes	    yes	        9.3.1
        HEAD	yes	    yes	        9.3.2
        OPTIONS	yes	    yes	        9.3.7
        POST	no	    no	        9.3.3
        PUT	    no	    yes	        9.3.4
        TRACE	yes	    yes	        9.3.8
        *	    no	    no	        18.2
     */
    struct MethodRegistration
    {
        enum class Method : std::uint8_t
        {
            CONNECT,
            DELETE,
            GET,
            HEAD,
            OPTIONS,
            POST,
            PUT,
            TRACE,
            ANY
        };
        struct MethodInfo
        {
            std::string_view name;
            bool is_safe;
            bool is_idempotent;
        };

        static constexpr std::array<MethodInfo, static_cast<size_t>(Method::ANY) + 1>
            registration = // NOLINT
            {{
                {.name = "CONNECT", .is_safe = false, .is_idempotent = false},
                {.name = "DELETE", .is_safe = false, .is_idempotent = true},
                {.name = "GET", .is_safe = true, .is_idempotent = true},
                {.name = "HEAD", .is_safe = true, .is_idempotent = true},
                {.name = "OPTIONS", .is_safe = true, .is_idempotent = true},
                {.name = "POST", .is_safe = false, .is_idempotent = false},
                {.name = "PUT", .is_safe = false, .is_idempotent = true},
                {.name = "TRACE", .is_safe = true, .is_idempotent = true},
                {.name = "*", .is_safe = false, .is_idempotent = false},
            }};
        constexpr static auto isRegistration(abnf::octets_view_in span) noexcept
            -> std::optional<MethodInfo>
        {
            for (const auto &item : registration)
            {
                if (abnf::tool::equal_value(span, item.name))
                    return item;
            }
            return std::nullopt;
        }
    };
}; // namespace mcs::protocol::http::iana