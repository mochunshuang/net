#pragma once

#include <string_view>
#include <array>

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::iana
{
    /**
     * @see
     * https://www.rfc-editor.org/rfc/rfc9112.html#name-transfer-coding-registratio
Name	    Description
chunked	    Transfer in a series of chunks
compress	UNIX "compress" data format [Welch]
deflate	"deflate" compressed data ([RFC1951]) inside the "zlib" data format ([RFC1950])
gzip	GZIP file format [RFC1952]
trailers	(reserved)
x-compress	Deprecated (alias for compress)
x-gzip	Deprecated (alias for gzip)
     */
    struct TransferCodingRegistration
    {
        struct TransferCoding
        {
            std::string_view name;
        };
        static constexpr auto registration = // NOLINT
            [] consteval {
                return std::array{
                    TransferCoding{"chunked"},  TransferCoding{"compress"},
                    TransferCoding{"deflate"},  TransferCoding{"gzip"},
                    TransferCoding{"trailers"}, TransferCoding{"x-compress"},
                    TransferCoding{"x-gzip"}};
            }();

        constexpr static std::optional<TransferCoding> isRegistration(
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