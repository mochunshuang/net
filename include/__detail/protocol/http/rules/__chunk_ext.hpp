#pragma once

#include "./__chunk_ext_name.hpp"
#include "./__chunk_ext_val.hpp"
#include "./__bws.hpp"

namespace mcs::protocol::http::rules
{
    // chunk-ext = *( BWS ";" BWS chunk-ext-name [ BWS "=" BWS chunk-ext-val ] )
    using chunk_ext = abnf::zero_or_more<abnf::sequence<
        BWS, abnf::Char<';'>, BWS, chunk_ext_name,
        abnf::optional<abnf::sequence<BWS, abnf::Char<'='>, BWS, chunk_ext_val>>>>;
}; // namespace mcs::protocol::http::rules