#pragma once

#include "./__transfer_coding.hpp"
#include "./__weight.hpp"

namespace mcs::abnf::http
{
    // t-codings = "trailers" / ( transfer-coding [ weight ] )
    using t_codings =
        alternative<String<"trailers">, sequence<transfer_coding, optional<weight>>>;
}; // namespace mcs::abnf::http