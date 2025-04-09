#pragma once

#include "../__core_rules.hpp"

#include "./__pchar.hpp"

namespace mcs::ABNF::URI
{
    // fragment      = *( pchar / "/" / "?" ) //NOTE: 存在 递归警告：misc-no-recursion
    // constexpr bool fragment(default_span_t sp) noexcept
    // {
    //     if (sp.empty())
    //         return true;
    //     // pchar 只处理 sp.size==3 和 sp.size==1的情况
    //     // 而 sp.size==1 的时候 pchar(c) || c=="/" || c== "?" 都是满足的
    //     if (sp.size() == 1)
    //     {
    //         const auto &v = sp[0];
    //         return pchar(v) || (v == '/' || v == '?');
    //     }
    //     if (sp.size() == 3)
    //         return pchar(sp);
    //     return fragment(sp.subspan(0, 1)) || fragment(sp.subspan(0, 3));
    // }

    // fragment      = *( pchar / "/" / "?" )
    constexpr bool fragment(default_span_t sp) noexcept
    {
        size_t index = 0;
        while (index < sp.size())
        {
            bool matched = false;

            // NOTE: ABNF的顺序，一定要严格遵守
            //  优先3字节：输入"%41"（即"A"的编码），应该作为三个字符的pchar处理
            if (index + 3 <= sp.size())
            {
                const auto k_sub = sp.subspan(index, 3);
                if (pchar(k_sub))
                {
                    index += 3;
                    matched = true;
                }
            }

            // 无法匹配 3 字节时，尝试单字节（包括 "/" 和 "?"）
            if (!matched && index < sp.size())
            {
                const auto &c = sp[index];
                if (pchar(c) || c == '/' || c == '?')
                {
                    index++;
                    matched = true;
                }
            }

            // sp.size==3 和 sp.size==1 的情况都不满足
            if (!matched)
                return false;
        }
        return true;
    }
}; // namespace mcs::ABNF::URI