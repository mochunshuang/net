#pragma once

#include <string_view>
#include <array>

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::iana
{
    /**
     * @see
     * https://www.rfc-editor.org/rfc/rfc9110.html#name-authentication-scheme-regist
     */
    struct AuthenticationSchemesRegistration
    {
        struct AuthenticationScheme
        {
            std::string_view name;
        };
        static constexpr auto registration = // NOLINT
            [] consteval {
                return std::array{AuthenticationScheme{"Basic"},
                                  AuthenticationScheme{"Bearer"},
                                  AuthenticationScheme{"Concealed"},
                                  AuthenticationScheme{"Digest"},
                                  AuthenticationScheme{"DPoP"},
                                  AuthenticationScheme{"GNAP"},
                                  AuthenticationScheme{"HOBA"},
                                  AuthenticationScheme{"Mutual"},
                                  AuthenticationScheme{"Negotiate"},
                                  AuthenticationScheme{"OAuth"},
                                  AuthenticationScheme{"PrivateToken"},
                                  AuthenticationScheme{"SCRAM-SHA-1"},
                                  AuthenticationScheme{"SCRAM-SHA-256"},
                                  AuthenticationScheme{"vapid"}};
            }();

        constexpr static std::optional<AuthenticationScheme> isRegistration(
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