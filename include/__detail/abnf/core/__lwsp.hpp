#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::core
{
    struct LWSP_TYPE
    {
    };

    /**
     * @brief 避免在邮件头中使用，因为它可能导致解析问题。
     *
     * 在定义新协议时，建议使用更严格的空白规则（如仅 WSP）。
     *
     */
    // LWSP = *(WSP / CRLF WSP)  ;
    struct LWSP
    {
        using rule_concept = rule_t;

        static constexpr auto parse(octets_view_in input) noexcept -> consumed_result
        {
            (void)input;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core