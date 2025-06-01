#pragma once

#include "./__weight.hpp"
#include "./__token.hpp"
#include "./__quoted_string.hpp"
#include "./__bws.hpp"

namespace mcs::abnf::http
{
    using transfer_parameter_no_q =
        sequence<assert_not_string<"q", token>, BWS, Char<'='>, BWS,
                 alternative<token, quoted_string>>;
    using transfer_coding_no_q =
        sequence<token,
                 zero_or_more<sequence<OWS, Char<';'>, OWS, transfer_parameter_no_q>>>;
    // t-codings = "trailers" / ( transfer-coding [ weight ] )
    using t_codings =
        alternative<String<"trailers">, sequence<transfer_coding_no_q, optional<weight>>>;
}; // namespace mcs::abnf::http