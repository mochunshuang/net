#pragma once

#include <cstdint>
#include <bitset>
#include <ranges>

namespace mcs::protocol::http::iana
{
    /**
     * @see
    https://www.rfc-editor.org/rfc/rfc9110.html#name-iana-considerations:~:text=18.3.-,Status%20Code%20Registration,-IANA%20has%20updated
        Value	Description	                    Section
        100	    Continue	                    15.2.1
        101	    Switching Protocols	            15.2.2
        200 	OK	                            15.3.1
        201	    Created	                        15.3.2
        202	    Accepted	                    15.3.3
        203	    Non-Authoritative Information	15.3.4
        204	    No Content	                    15.3.5
        205	    Reset Content	                15.3.6
        206	    Partial Content	                15.3.7
        300	    Multiple Choices	            15.4.1
        301	    Moved Permanently	            15.4.2
        302	    Found	                        15.4.3
        303	    See Other	                    15.4.4
        304	    Not Modified	                15.4.5
        305	    Use Proxy	                    15.4.6
        306	    (Unused)	                    15.4.7
        307	    Temporary Redirect	            15.4.8
        308	    Permanent Redirect	            15.4.9
        400	    Bad Request	                    15.5.1
        401	    Unauthorized	                15.5.2
        402	    Payment Required	            15.5.3
        403	    Forbidden	                    15.5.4
        404	    Not Found	                    15.5.5
        405	    Method Not Allowed	            15.5.6
        406	    Not Acceptable	                15.5.7
        407	    Proxy Authentication Required	15.5.8
        408	    Request Timeout	                15.5.9
        409	    Conflict	                    15.5.10
        410	    Gone	                        15.5.11
        411	    Length Required	                15.5.12
        412	    Precondition Failed	            15.5.13
        413	    Content Too Large	            15.5.14
        414	    URI Too Long	                15.5.15
        415	    Unsupported Media Type	        15.5.16
        416	    Range Not Satisfiable	        15.5.17
        417	    Expectation Failed	            15.5.18
        418	    (Unused)	                    15.5.19
        421	    Misdirected Request	            15.5.20
        422	    Unprocessable Content	        15.5.21
        426	    Upgrade Required	            15.5.22
        500	    Internal Server Error	        15.6.1
        501	    Not Implemented	                15.6.2
        502	    Bad Gateway	                    15.6.3
        503	    Service Unavailable	            15.6.4
        504	    Gateway Timeout	                15.6.5
        505	    HTTP Version Not Supported	    15.6.6
     */
    struct StatusCodeRegistration
    {
        enum class StatusCode : std::uint16_t
        {
            Continue = 100,            // NOLINT
            Switching_Protocols = 101, // NOLINT
            OK = 200,
            Created = 201,                       // NOLINT
            Accepted = 202,                      // NOLINT
            Non_Authoritative_Information = 203, // NOLINT
            No_Content = 204,                    // NOLINT
            Reset_Content = 205,                 // NOLINT
            Partial_Content = 206,               // NOLINT
            Multiple_Choices = 300,              // NOLINT
            Moved_Permanently = 301,             // NOLINT
            Found = 302,                         // NOLINT
            See_Other = 303,                     // NOLINT
            Not_Modified = 304,                  // NOLINT
            Use_Proxy = 305,                     // NOLINT
            // Unused,
            Temporary_Redirect = 307,            // NOLINT
            Permanent_Redirect = 308,            // NOLINT
            Bad_Request = 400,                   // NOLINT
            Unauthorized = 401,                  // NOLINT
            Payment_Required = 402,              // NOLINT
            Forbidden = 403,                     // NOLINT
            Not_Found = 404,                     // NOLINT
            Method_Not_Allowed = 405,            // NOLINT
            Not_Acceptable = 406,                // NOLINT
            Proxy_Authentication_Required = 407, // NOLINT
            Request_Timeout = 408,               // NOLINT
            Conflict = 409,                      // NOLINT
            Gone = 410,                          // NOLINT
            Length_Required = 411,               // NOLINT
            Precondition_Failed = 412,           // NOLINT
            Content_Too_Large = 413,             // NOLINT
            URI_Too_Long = 414,                  // NOLINT
            Unsupported_Media_Type = 415,        // NOLINT
            Range_Not_Satisfiable = 416,         // NOLINT
            Expectation_Failed = 417,            // NOLINT
            // Unused,
            Misdirected_Request = 421,        // NOLINT
            Unprocessable_Content = 422,      // NOLINT
            Upgrade_Required = 426,           // NOLINT
            Internal_Server_Error = 500,      // NOLINT
            Not_Implemented = 501,            // NOLINT
            Bad_Gateway = 502,                // NOLINT
            Service_Unavailable = 503,        // NOLINT
            Gateway_Timeout = 504,            // NOLINT
            HTTP_Version_Not_Supported = 505, // NOLINT
        };

        static constexpr auto registration = []() consteval { // NOLINT
            using enum StatusCode;
            std::bitset<static_cast<uint16_t>(HTTP_Version_Not_Supported) + 1> bs;
            // 100-101
            for (auto ch : {Continue, Switching_Protocols})
                bs.set(static_cast<uint16_t>(ch));
            // 200-206
            for (uint16_t i : std::views::iota(static_cast<uint16_t>(OK),
                                               static_cast<uint16_t>(Partial_Content)))
                bs.set(i);
            // 300-305
            for (uint16_t i : std::views::iota(static_cast<uint16_t>(Multiple_Choices),
                                               static_cast<uint16_t>(Use_Proxy)))
                bs.set(i);
            // 307-308
            bs.set(static_cast<uint16_t>(Temporary_Redirect));
            bs.set(static_cast<uint16_t>(Permanent_Redirect));

            // 400-417
            for (uint16_t i : std::views::iota(static_cast<uint16_t>(Bad_Request),
                                               static_cast<uint16_t>(Expectation_Failed)))
                bs.set(i);
            // 2421,422,426
            bs.set(static_cast<uint16_t>(Misdirected_Request));
            bs.set(static_cast<uint16_t>(Unprocessable_Content));
            bs.set(static_cast<uint16_t>(Upgrade_Required));
            // 500-505
            for (uint16_t i :
                 std::views::iota(static_cast<uint16_t>(Internal_Server_Error),
                                  static_cast<uint16_t>(HTTP_Version_Not_Supported)))
                bs.set(i);
            return bs;
        }();

        constexpr static bool isRegistration(std::uint16_t span) noexcept
        {
            return registration[span];
        }
    };

}; // namespace mcs::protocol::http::iana