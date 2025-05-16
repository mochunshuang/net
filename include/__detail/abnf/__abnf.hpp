#pragma once

#include "./detail/__types.hpp"
#include "./detail/__concept.hpp"
#include "./operators/__alternative.hpp"
#include "./operators/__one_or_more.hpp"
#include "./operators/__optional.hpp"
#include "./operators/__repetition.hpp"
#include "./operators/__sequence.hpp"
#include "./operators/__times.hpp"
#include "./operators/__zero_or_more.hpp"
#include "./operators/__max_times.hpp"
#include "./operators/__with_callback.hpp"
#include "./operators/__watch_index.hpp"
#include "./core/__octet.hpp"
#include "./core/__alpha.hpp"
#include "./core/__bit.hpp"
#include "./core/__char.hpp"
#include "./core/__cr.hpp"
#include "./core/__lf.hpp"
#include "./core/__crlf.hpp"
#include "./core/__ctl.hpp"
#include "./core/__digit.hpp"
#include "./core/__dquote.hpp"
#include "./core/__hexdig.hpp"
#include "./core/__htab.hpp"
#include "./core/__lwsp.hpp"
#include "./core/__sp.hpp"
#include "./core/__vchar.hpp"
#include "./core/__wsp.hpp"
#include "./generate/__char.hpp"
#include "./generate/__range.hpp"
#include "./generate/__any_of.hpp"
#include "./generate/__check_char.hpp"
#include "./generate/__simple_rule.hpp"
#include "./generate/__string.hpp"
#include "./tool/__to_lower.hpp"
#include "./tool/__to_upper.hpp"
#include "./tool/__equal_value.hpp"
#include "./tool/__equal_span.hpp"

namespace mcs::abnf
{
    using detail::octet;
    using detail::octets_view;
    using detail::octets_view_in;
    using detail::octet_in;
    using detail::consumed_result;
    using detail::make_consumed_result;
    using detail::parser_ctx;
    using detail::make_parser_ctx;
    using detail::parser_ctx_ref;
    using detail::k_max_octet_value;
    using detail::k_min_octet_value;
    using detail::k_max_size_value;
    using detail::k_min_size_value;

    using detail::rule_t;
    using detail::rule;

    using operators::alternative;
    using operators::optional;
    using operators::repetition;
    using operators::sequence;
    using operators::times;
    using operators::max_times;
    using operators::zero_or_more;
    using operators::one_or_more;
    using operators::with_callback;
    using operators::watch_index;

    using core::OCTET;
    using core::ALPHA;
    using core::BIT;
    using core::CHAR;
    using core::CR;
    using core::LF;
    using core::CRLF;
    using core::CTL;
    using core::DIGIT;
    using core::DQUOTE;
    using core::HEXDIG;
    using core::HTAB;
    using core::LWSP;
    using core::SP;
    using core::VCHAR;
    using core::WSP;

    using core::cr_value;
    using core::lf_value;
    using core::htab_value;
    using core::sp_value;
    using core::dquote_value;

    using generate::CharInsensitive;
    using generate::Char;
    using generate::Range;
    using generate::any_of;
    using generate::assert_not_is_char;
    using generate::assert_not_string;
    using generate::SimpleRule;
    using generate::String;
    using generate::StringSensitive;
    using generate::ctx_done_after;

    using tool::to_lower;
    using tool::to_upper;
    using tool::equal_value;
    using tool::equal_span;

}; // namespace mcs::abnf