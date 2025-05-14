#pragma once

#include <cstdint>
#include <string_view>
#include <array>

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::iana
{
    /**
     * @see
     * https://www.rfc-editor.org/rfc/rfc9110.html#name-port-registration
            http	80	tcp World Wide Web HTTP
            https	443	tcp	http protocol over TLS/SSL
     */
    struct PortRegistration
    {
        struct PortInfo
        {
            std::string_view name;
            uint16_t port;
        };
        static constexpr auto registration = // NOLINT
            [] consteval {
                return std::array{PortInfo{.name = "http", .port = 80},    // NOLINT
                                  PortInfo{.name = "https", .port = 443}}; // NOLINT
            }();

        constexpr static auto isRegistration(abnf::octets_view_in span) noexcept
            -> std::optional<PortInfo>
        {
            if (span.size() == 4 && abnf::tool::to_lower(span[0]) == 'h' &&
                abnf::tool::to_lower(span[1]) == 't' &&
                abnf::tool::to_lower(span[2]) == 't' &&
                abnf::tool::to_lower(span[3]) == 'p')
                return registration[0];

            if (span.size() == 5 && abnf::tool::to_lower(span[0]) == 'h' && // NOLINT
                abnf::tool::to_lower(span[1]) == 't' &&
                abnf::tool::to_lower(span[2]) == 't' &&
                abnf::tool::to_lower(span[3]) == 'p' &&
                abnf::tool::to_lower(span[4]) == 's')
                return registration[1];
            return std::nullopt;
        }
    };
}; // namespace mcs::protocol::http::iana