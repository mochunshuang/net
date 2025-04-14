#pragma once

#include "./__ls32.hpp"
#include "__h16.hpp"
#include <cstddef>
#include <limits>

#include "../tool/__split_span.hpp"

namespace mcs::abnf::uri
{
    namespace detail
    {
        // 1*6( h16 ":" ) ls32
        constexpr bool check_common_tail(span_param_in sp, const size_t &k_times) noexcept
        {
            if (k_times == 0)
                return false;
            // from left to right check
            auto [front, check_span] = tool::split_span_first(sp, ':');

            if (front.size() != sp.size()) // NOTE: need find ':' in sp
            {
                auto need_check_times = k_times;
                need_check_times--;
                if (not h16(front)) // check
                    return false;

                auto remaining = check_span;
                while (need_check_times-- > 0) // times( h16 ":" ) ls32
                {
                    auto [front, tail] = tool::split_span_first(remaining, ':');
                    if (not h16(front))
                        return false;
                    remaining = tail;
                }
                return ls32(remaining); // check ls32 last
            }
            return false; // NOTE: not  find ':' in sp
        }

        // [               h16 ]
        // [ *1( h16 ":" ) h16 ]
        // ......
        // [ *6( h16 ":" ) h16 ]
        constexpr bool check_common_front(span_param_in sp,
                                          const size_t k_max_colons // 最大允许的冒号数
                                          ) noexcept
        {
            if (sp.empty())
                return true;

            auto [front_part, last_h16] = tool::split_span_last(sp, ':');

            //  *k_max_colons( h16 ":" ) h16
            if (!h16(last_h16))
                return false;

            if (k_max_colons == 0) // [               h16 ]
                return front_part.empty();

            // NOTE: init state with find  ':' in sp
            size_t colons_count = 1;
            auto remaining = front_part;

            while (!remaining.empty() && colons_count <= k_max_colons)
            {
                auto [current_h16, tail] = tool::split_span_first(remaining, ':');
                if (tail.empty()) // have no find ':'
                {
                    remaining = tail;
                    break;
                }

                // nomal check
                if (h16(current_h16))
                {
                    remaining = tail;
                    ++colons_count;
                    continue;
                }
                return false;
            }
            return colons_count <= k_max_colons && remaining.empty();
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
    constexpr bool IPv6address(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size < 2) // [ *6( h16 ":" ) h16 ] "::"
            return false;

        static_assert(not ls32(empty_span_param));
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
               (detail::check_common_front(front, 0) &&
                detail::check_common_tail(last, 4)) ||
               (detail::check_common_front(front, 1) &&
                detail::check_common_tail(last, 3)) ||
               (detail::check_common_front(front, 2) &&
                detail::check_common_tail(last, 2)) ||
               (detail::check_common_front(front, 3) &&
                detail::check_common_tail(last, 1)) ||
               (detail::check_common_front(front, 4) && ls32(last)) || // NOLINT
               -                                                       // NOLINT
               (detail::check_common_front(front, 5) && h16(last)) ||  // NOLINT
               (detail::check_common_front(front, 6) && last.empty()); // NOLINT
    }
}; // namespace mcs::abnf::uri