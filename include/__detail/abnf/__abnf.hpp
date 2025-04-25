#pragma once

#include "./detail/__types.hpp"
#include "./detail/__concept.hpp"

namespace mcs::abnf
{
    using detail::octet;
    using detail::octets_view;
    using detail::octets_view_in;
    using detail::octet_in;
    using detail::empty_octets_view;
    using detail::absolute_span;
    using detail::consumed_result;
    using detail::parser_ctx;
    using detail::make_parser_ctx;
    using detail::const_parser_ctx;
    using detail::parser_ctx_in;
    using detail::k_max_octet_value;
    using detail::k_min_octet_value;
    using detail::k_max_size_value;
    using detail::k_min_size_value;

    using detail::rule_t;
    using detail::is_rule;
    using detail::ruleer;

}; // namespace mcs::abnf