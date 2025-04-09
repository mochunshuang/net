#pragma once

#include "__pchar.hpp"

namespace mcs::ABNF::URI
{
    // path-rootless = segment-nz *( "/" segment )
    // segment-nz    = 1*pchar
    // segment       = *pchar
    constexpr bool path_rootless(default_span_t sp) noexcept
    {

        if (sp.empty()) // segment-nz 至少需要一个 pchar，空输入非法
            return false;

        static_assert(not pchar('/')); // NOTE: '/' 不属于 pchar

        // Step 1: 解析第一个 segment-nz
        size_t pos = 0;
        bool has_pchar = false; // 标记是否至少有一个 pchar

        // 逐个字符（或 pct-encoded 三元组）匹配 pchar
        while (pos < sp.size())
        {
            bool matched = false;

            if (pos + 3 <= sp.size())
            {
                const auto k_sub = sp.subspan(pos, 3);
                if (pchar(k_sub))
                {
                    pos += 3;
                    matched = true;
                    has_pchar = true;
                }
            }

            if (!matched && pos < sp.size())
            {
                const auto &c = sp[pos];
                if (pchar(c))
                {
                    pos++;
                    matched = true;
                    has_pchar = true;
                }
            }

            if (!matched) // 匹配失败时退出循环
            {
                break;
            }
        }

        if (!has_pchar) // segment-nz 必须包含至少一个 pchar
            return false;

        while (pos < sp.size()) // Step 2: 解析后续的 *( "/" segment )
        {

            if (sp[pos] != '/') // 必须从 '/' 开始
            {
                return false;
            }
            pos++; // 跳过 '/'

            // 解析 segment（允许空字符串）
            while (pos < sp.size())
            {
                bool matched = false;

                // 优先检查 pct-encoded（3字节）
                if (pos + 3 <= sp.size())
                {
                    const auto sub = sp.subspan(pos, 3);
                    if (pchar(sub))
                    { // 检查 pct-encoded
                        pos += 3;
                        matched = true;
                    }
                }

                // 若未匹配 pct-encoded，检查单字符 pchar
                if (!matched && pos < sp.size())
                {
                    const auto c = sp[pos];
                    if (pchar(c))
                    { // 检查单字符 pchar
                        pos++;
                        matched = true;
                    }
                }

                // 无法匹配时退出当前 segment 解析
                if (!matched)
                {
                    break;
                }
            }
        }

        return true;
    }
}; // namespace mcs::ABNF::URI
