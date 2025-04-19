#pragma once

#include "./__detail/__types.hpp"
#include "./__detail/__result_builder.hpp"
#include "./__detail/__result.hpp"
#include "./__detail/__error.hpp"
#include "./__detail/__span.hpp"

#include "./tool/__make_stdspan.hpp"
#include "./tool/__equal_value.hpp"
#include "./tool/__equal_span.hpp"
#include "./tool/__is_valid_span.hpp"
#include "./tool/__is_empty_span.hpp"
#include "./tool/__find_index.hpp"
#include "./tool/__split_span.hpp"

namespace mcs::abnf
{
    using __detail::result_builder;
    using __detail::result;
    using __detail::error;
    using __detail::span;
    using __detail::invalid_span;
    using simple_result = result_builder<void>;

    /**
     * @brief OCTET          =  %x00-FF
     *                          ; 8 bits of data
     */
    using __detail::OCTET;

    using __detail::span_param_in;
    using __detail::octet_param_in;
    using __detail::empty_span_param;

    // help
    using tool::make_stdspan;
    using tool::equal_value;
    using tool::equal_span;
    using tool::is_valid_span;
    using tool::is_empty_span;
    using tool::find_index;
    using tool::split_span_first;
    using tool::split_span_last;

}; // namespace mcs::abnf