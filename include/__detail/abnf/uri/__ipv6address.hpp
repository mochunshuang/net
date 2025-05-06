#pragma once

#include "./__h16.hpp"
#include "./__ls32.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        namespace __detail
        {
            using double_colon = sequence<CharSensitive<':'>, CharSensitive<':'>>;
            using h16_and_colon =
                sequence<h16, CharSensitive<':'>, assert_not_is_char<':'>>;

            using IPv6_0 = sequence<times<6, h16_and_colon>, ls32>; // NOLINT
            using IPv6_1 =
                sequence<double_colon, times<5, h16_and_colon>, ls32>; // NOLINT
            using IPv6_2 =
                sequence<optional<h16>, double_colon, times<4, h16_and_colon>, ls32>;
            using IPv6_3 = sequence<optional<sequence<max_times<1, h16_and_colon>, h16>>,
                                    double_colon, times<3, h16_and_colon>, ls32>;
            using IPv6_4 = sequence<optional<sequence<max_times<2, h16_and_colon>, h16>>,
                                    double_colon, times<2, h16_and_colon>, ls32>;
            using IPv6_5 = sequence<optional<sequence<max_times<3, h16_and_colon>, h16>>,
                                    double_colon, times<1, h16_and_colon>, ls32>;
            using IPv6_6 = sequence<optional<sequence<max_times<4, h16_and_colon>, h16>>,
                                    double_colon, ls32>; // NOLINTNEXTLINE
            using IPv6_7 = sequence<optional<sequence<max_times<5, h16_and_colon>, h16>>,
                                    double_colon, h16>;
            using IPv6_8 = // NOLINTNEXTLINE
                sequence<optional<sequence<max_times<6, h16_and_colon>, h16>>,
                         double_colon>;
            using IPv6addressRule = alternative<IPv6_0, IPv6_1, IPv6_2, IPv6_3, IPv6_4,
                                                IPv6_5, IPv6_6, IPv6_7, IPv6_8>;

        }; // namespace __detail
        using IPv6address_rule = __detail::IPv6addressRule;
    }; // namespace rules

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
    struct IPv6address : SimpleRule<IPv6address, rules::IPv6address_rule>
    {
                   using SimpleRule<IPv6address, rules::IPv6address_rule>::SimpleRule;
    };

}; // namespace mcs::abnf::uri