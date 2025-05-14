#pragma once

#include <string_view>
#include <array>

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::iana
{
    /**
     * @see
     * https://www.rfc-editor.org/rfc/rfc9110.html#name-upgrade-token-registration
     */
    struct UpgradeTokenRegistration
    {
        struct UpgradeToken
        {
            std::string_view name;
        };
        static constexpr auto registration = // NOLINT
            [] consteval {
                return std::array{UpgradeToken{"HTTP"},       UpgradeToken{"TLS"},
                                  UpgradeToken{"connect-ip"}, UpgradeToken{"connect-udp"},
                                  UpgradeToken{"h2c"},        UpgradeToken{"websocket"},
                                  UpgradeToken{"WebSocket"}};
            }();

        constexpr static std::optional<UpgradeToken> isRegistration(
            abnf::octets_view_in span) noexcept
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