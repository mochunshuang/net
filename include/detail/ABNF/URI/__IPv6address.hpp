#pragma once

#include "./__ls32.hpp"
#include "__h16.hpp"
#include <cstddef>
#include <limits>

#include "../tool/__split_span.hpp"

namespace mcs::ABNF::URI
{
    namespace detail
    {
        // 1*6( h16 ":" ) ls32
        constexpr bool check_common_tail(default_span_t sp,
                                         const size_t &k_times) noexcept
        {
            if (k_times == 0)
                return false;

            auto [front, check_span] = tool::split_span_first(sp, ':');
            auto need_check_times = k_times;
            if (front.size() != sp.size()) // NOTE: need find ':' in sp
            {
                need_check_times--;
                if (not h16(front)) // check
                    return false;

                auto tail = check_span;
                while (need_check_times-- > 0) // times( h16 ":" ) ls32
                {
                    auto [front, t] = tool::split_span_first(tail, ':');
                    if (not h16(front))
                        return false;
                    tail = t;
                }
                return ls32(tail); // check ls32 last
            }
            return false;
        }

        // [               h16 ] → 0冒号，1段
        // [ *1( h16 ":" ) h16 ] → 1冒号，2段
        // ......
        // [ *6( h16 ":" ) h16 ] → 6冒号，7段
        constexpr bool check_common_front(
            default_span_t sp,
            size_t max_segments, // 最大允许的h16段数 (冒号数+1)
            size_t max_colons    // 最大允许的冒号数
            ) noexcept
        {
            // NOTE: [ *n( h16 ":" ) h16 ] 允许空字符串
            if (sp.empty())
                return true;

            // 第一步：检查最后一个h16
            auto [front_part, last_h16] = tool::split_span_last(sp, ':');
            if (!h16(last_h16))
                return false;

            // 特殊情况：只有最后一段h16（无冒号）
            if (front_part.empty())
                return max_segments >= 1 && max_colons >= 0;

            // 第二步：解析前面的 h16 ":" 结构
            size_t colon_count = 0;
            size_t segment_count = 1; // 已包含最后的h16
            auto remaining = front_part;

            while (!remaining.empty())
            {
                // 分割冒号
                auto [current_h16, tail] = tool::split_span_first(remaining, ':');

                // 冒号必须存在于字符串中
                if (current_h16.size() == remaining.size())
                    return false;

                // 验证h16有效性
                if (!h16(current_h16))
                    return false;

                // 更新计数
                ++colon_count;
                ++segment_count;
                remaining = tail;

                // 超过限制立即返回
                if (colon_count > max_colons || segment_count > max_segments)
                    return false;
            }

            // 最终验证（必须精确匹配限制）
            return colon_count <= max_colons && segment_count <= max_segments;
        }
    }; // namespace detail

    /**
    IPv6address  =                            6( h16 ":" ) ls32
                 /                       "::" 5( h16 ":" ) ls32
                 / [               h16 ] "::" 4( h16 ":" ) ls32
                 / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
                 / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
                 / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
                 / [ *4( h16 ":" ) h16 ] "::"              ls32
                 / [ *5( h16 ":" ) h16 ] "::"              h16
                 / [ *6( h16 ":" ) h16 ] "::"

    NOTE: "::" can only appearing once:
            ::1
            2001:db8::8a2e:370:7334
            fe80::
     */
    constexpr bool IPv6address(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        constexpr auto k_min_size = 7; // 6( h16 ":" ) ls32
        if (k_size < k_min_size)
            return false;

        static_assert(not ls32(empty_span));
        // h16           = 1*4HEXDIG
        // HEXDIG         =  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
        // DIGIT          =  %x30-39; 0-9
        static_assert(not h16(std::array<OCTET, 2>{':', ':'}));
        static_assert(DIGIT('0'));

        // find "::" index
        constexpr auto k_max = std::numeric_limits<size_t>::max();
        size_t index = k_max;
        for (size_t i = 1; i < k_size; i++)
        {
            if (sp[i] == ':' && sp[i - 1] == ':')
            {
                index = i - 1;
                break;
            }
        }
        // NOTE: [               h16 ] mean is optional
        // NOTE: ["a"] == *1"a" then match: "", "a"
        // NOTE: split by "::"
        auto front = sp.first(index);
        auto last = sp.subspan(index + 2);

        //                            6( h16 ":" ) ls32
        //                       "::" 5( h16 ":" ) ls32
        // [               h16 ] "::" 4( h16 ":" ) ls32
        // [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
        // [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
        // [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
        // [ *4( h16 ":" ) h16 ] "::"              ls32
        // [ *5( h16 ":" ) h16 ] "::"              h16
        // [ *6( h16 ":" ) h16 ] "::"
        return (index == k_max && detail::check_common_tail(sp, 6)) ||  // NOLINT: case 1
               (front.empty() && detail::check_common_tail(last, 5)) || // NOLINT : case 2
               (detail::check_common_front(front, 1, 0) &&
                detail::check_common_tail(last, 4)) ||
               (detail::check_common_front(front, 2, 1) &&
                detail::check_common_tail(last, 3)) ||
               (detail::check_common_front(front, 3, 2) &&
                detail::check_common_tail(last, 2)) ||
               (detail::check_common_front(front, 4, 3) &&
                detail::check_common_tail(last, 1)) ||
               (detail::check_common_front(front, 5, 4) && ls32(last)) || // NOLINT
               -                                                          // NOLINT
               (detail::check_common_front(front, 6, 5) && h16(last)) ||  // NOLINT
               (detail::check_common_front(front, 7, 6) && last.empty()); // NOLINT
    }
}; // namespace mcs::ABNF::URI