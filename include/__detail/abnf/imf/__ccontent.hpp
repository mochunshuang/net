#pragma once

#include "../__core_types.hpp"
#include "./__comment.hpp"
#include "./__cfws.hpp"
#include "./__quoted_pair.hpp"

namespace mcs::abnf::imf
{
    constexpr simple_result::result_type ccontent(span_param_in sp) noexcept;

    // FWS             =   ([*WSP CRLF] 1*WSP) /  obs-FWS ;Folding white space
    // comment         =   "(" *([FWS] ccontent) [FWS] ")"
    constexpr abnf_result auto comment(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size < 2 || sp[0] != '(' || sp[k_size - 1] != ')')
            return simple_result::fail(0);

        // NOLINTNEXTLINE
        return simple_result::success();
    }

    // ccontent        =   ctext / quoted-pair / comment
    constexpr simple_result::result_type ccontent(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size < 1)
            return simple_result::fail(0);

        if (k_size == 1 && ctext(sp[0]))
            return simple_result::success();
        if (k_size == 2 && quoted_pair(sp[0], sp[1]))
            return simple_result::success();

        return comment(sp) ? simple_result::success() : simple_result::fail(2);
    }

} // namespace mcs::abnf::imf