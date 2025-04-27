#pragma once

#include "../detail/__types.hpp"
#include <string>

namespace mcs::abnf::uri
{
    /**
    IPv6address  =                            6( h16 ":" ) ls32
                /                       "::" 5( h16 ":" ) ls32
                / [               h16 ] "::" 4( h16 ":" ) ls32
                / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
                / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
                / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
                / [ *4( h16 ":" ) h16 ] "::"              ls32
                / [ *5( h16 ":" ) h16 ] "::"              h16
                / [ *6( h16 ":" ) h16 ] "::"

    NOTE: "::" can only appearing once:
            ::1
            2001:db8::8a2e:370:7334
            fe80::
    */
    struct IPv6address
    {
        struct __type
        {
            using domain = IPv6address;
            detail::absolute_span value;
        };
        using result_type = __type;

        // static constexpr auto operator()(detail::const_parser_ctx ctx) noexcept
        //     -> consumed_result
        // {
        // }
        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string IPv6address;
            return IPv6address;
        }
    };

}; // namespace mcs::abnf::uri