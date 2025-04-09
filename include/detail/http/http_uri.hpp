
#pragma once

namespace mcs::net::http
{
    /**
     * @brief https://www.rfc-editor.org/rfc/rfc9110.html#name-http-uri-scheme
     * define
     * http-URI = "http" "://" authority path-abempty [ "?" query ]
     * authority = <authority, see [URI], Section 3.2>
     * path-abempty = <path-abempty, see [URI], Section 3.3>
     * @see[URI] https://www.rfc-editor.org/rfc/rfc3986.html
     */
    struct http_uri
    {
    };

} // namespace mcs::net::http