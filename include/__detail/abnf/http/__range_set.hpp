#pragma once

#include "./__other_range.hpp"
#include "./__suffix_range.hpp"
#include "./__int_range.hpp"
#include "__range_spec.hpp"
#include <array>

namespace mcs::abnf::http
{
    // suffix-range = "-" suffix-length
    // int-range = first-pos "-" [ last-pos ]
    // range-spec = int-range / suffix-range / other-range
    // range-set = range-spec *( OWS "," OWS range-spec )
    constexpr abnf_result auto range_set(span_param_in sp) noexcept
    {
        static_assert(not int_range(std::array<OCTET, 1>{','}));
        static_assert(not suffix_range(std::array<OCTET, 1>{','}));
        static_assert(not other_range(std::array<OCTET, 1>{','}));
        const auto k_size = sp.size();
        if (k_size == 0)
            return simple_result::fail(0);

        auto span = sp;
        while (not span.empty())
        {
            const auto [first, tail] = tool::split_span_first(span, ',');

            // remove empty
            std::size_t left{0};
            std::size_t right{first.size() - 1};
            while (left < right && (first[left] == SP || first[left] == HTAB))
                ++left;
            while (left < right && (first[right] == SP || first[right] == HTAB))
                --right;

            if (not range_spec(first.subspan(left, right - left + 1)))
                return simple_result::fail(0);

            span = tail;
        }
        return simple_result::success();
    }

}; // namespace mcs::abnf::http