#pragma once

#include "./__absolute_path.hpp"
#include "./__query.hpp"

namespace mcs::protocol::http::rules
{
    // origin-form = absolute-path [ "?" query ]
    using origin_form =
        abnf::sequence<absolute_path,
                       abnf::optional<abnf::sequence<abnf::Char<'?'>, query>>>;
}; // namespace mcs::protocol::http::rules