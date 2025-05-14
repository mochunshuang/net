#pragma once

#include <cstdint>
#include <string_view>
#include <array>

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::iana
{
    /**
     * @see
     * https://www.rfc-editor.org/rfc/rfc9110.html#uri.schemes:~:text=4.2.-,HTTP%2DRelated%20URI%20Schemes,-IANA%20maintains%20the
        URI Scheme	    Description	                        Section
        http	        Hypertext Transfer Protocol	        4.2.1
        https	        Hypertext Transfer Protocol Secure	4.2.2
    */
    struct URISchemeRegistration
    {
        enum class URIScheme : std::uint8_t
        {
            http,  // NOLINT
            https, // NOLINT
        };
        struct URISchemeInfo
        {
            std::string_view name;
        };
        static constexpr std::array<URISchemeInfo,
                                    static_cast<size_t>(URIScheme::https) + 1>
            registration = // NOLINT
            {URISchemeInfo{"http"}, URISchemeInfo{"https"}};

        constexpr static auto isRegistration(abnf::octets_view_in span)
            -> std::optional<URISchemeInfo>
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