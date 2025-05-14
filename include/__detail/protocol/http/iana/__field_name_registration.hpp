#pragma once

#include <cstdint>
#include <string_view>
#include <array>

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::iana
{
    /**
     * @see
     * https://www.rfc-editor.org/rfc/rfc9110.html#name-field-name-registration
     */
    struct FieldNameRegistration
    {
        struct FieldName
        {
            std::string_view name;
            enum class Status : std::uint8_t
            {
                permanent, // NOLINT
                deprecated // NOLINT
            } status;
        };
        static constexpr auto registration = // NOLINT
            [] consteval {
                using enum FieldName::Status;
                return std::array{
                    FieldName{
                        .name = "Accept",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Accept-Charset",
                        .status = deprecated,
                    },
                    FieldName{
                        .name = "Accept-Encoding",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Accept-Language",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Accept-Ranges",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Allow",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Authentication-Info",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Authorization",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Connection",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Content-Encoding",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Content-Language",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Content-Length",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Content-Location",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Content-Range",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Content-Type",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Date",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "ETag",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Expect",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "From",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Host",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "If-Match",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "If-Modified-Since",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "If-None-Match",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "If-Range",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "If-Unmodified-Since",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Last-Modified",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Location",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Max-Forwards",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Proxy-Authenticate",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Proxy-Authentication-Info",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Proxy-Authorization",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Range",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Referer",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Retry-After",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Server",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "TE",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Trailer",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Upgrade",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "User-Agent",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Vary",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Via",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "WWW-Authenticate",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "*",
                        .status = permanent,
                    },
                    // https://www.rfc-editor.org/rfc/rfc9112.html#name-field-name-registration
                    FieldName{
                        .name = "Close",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "MIME-Version",
                        .status = permanent,
                    },
                    FieldName{
                        .name = "Transfer-Encoding",
                        .status = permanent,
                    }};
            }();

        constexpr static std::optional<FieldName> isRegistration(
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