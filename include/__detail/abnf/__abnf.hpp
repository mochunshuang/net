#pragma once

#include "./detail/__types.hpp"
#include "./detail/__concept.hpp"
#include "./operators/__alternative.hpp"
#include "./operators/__one_or_more.hpp"
#include "./operators/__operable_rule.hpp"
#include "./operators/__optional.hpp"
#include "./operators/__repetition.hpp"
#include "./operators/__sequence.hpp"
#include "./operators/__times.hpp"
#include "./operators/__zero_or_more.hpp"
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

namespace mcs::abnf
{
    using detail::octet;
    using detail::octets_view;
    using detail::octets_view_in;
    using detail::octet_in;
    using detail::empty_octets_view;
    using detail::absolute_span;
    using detail::consumed_result;
    using detail::make_consumed_result;
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

    using operators::alternative;
    using operators::one_or_more;
    using operators::operable_rule;
    using operators::optional;
    using operators::repetition;
    using operators::sequence;
    using operators::times;
    using operators::zero_or_more;

    using core::ALPHA;
    using core::alpha;
    using core::BIT;
    using core::bit;
    using core::CHAR;
    using core::Char;
    using core::CR;
    using core::cr;
    using core::LF;
    using core::lf;
    using core::CRLF;
    using core::crlf;
    using core::CTL;
    using core::ctl;
    using core::DIGIT;
    using core::digit;
    using core::DQUOTE;
    using core::dquote;
    using core::HEXDIG;
    using core::hexdig;
    using core::HTAB;
    using core::htab;
    using core::LWSP;
    using core::lwsp;
    using core::SP;
    using core::sp;
    using core::VCHAR;
    using core::vchar;
    using core::WSP;
    using core::wsp;

    using core::cr_value;
    using core::lf_value;
    using core::htab_value;
    using core::sp_value;
    using core::dquote_value;

    using generate::InsensitiveChar;
    using generate::SensitiveChar;
    using generate::Range;
    using generate::any_of;

}; // namespace mcs::abnf