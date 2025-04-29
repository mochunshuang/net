#pragma once

#include "../operators/__zero_or_more.hpp"
#include "../operators/__alternative.hpp"
#include "../operators/__sequence.hpp"
#include "./__wsp.hpp"
#include "./__crlf.hpp"

namespace mcs::abnf::core
{
    /**
     * @brief 避免在邮件头中使用，因为它可能导致解析问题。
     *
     * 在定义新协议时，建议使用更严格的空白规则（如仅 WSP）。
     *
     */
    // LWSP = *(WSP / CRLF WSP)  ;
    using LWSP = operators::zero_or_more<
        operators::alternative<WSP, operators::sequence<CRLF, WSP>>>;

}; // namespace mcs::abnf::core