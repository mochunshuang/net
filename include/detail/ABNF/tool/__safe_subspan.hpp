#pragma once

#include "../__core_rules.hpp"

namespace mcs::ABNF::tool
{
    inline constexpr auto safe_subspan = // NOLINT // NOLINTNEXTLINE
        [](default_span_t sp, std::size_t offset,
           std::size_t count = std::dynamic_extent) noexcept {
            // 优先检查offset合法性
            if (offset > sp.size())
                return empty_span_octet;

            // 修正条件：允许count == 0,k_remaining的特殊情况
            const auto k_available = sp.size() - offset;
            const auto k_actual_count =
                (count == std::dynamic_extent)
                    ? k_available
                    : (count == 0 ? 0 : std::min(count, k_available)); // NOLINT

            return sp.subspan(offset, k_actual_count);
        };
}; // namespace mcs::ABNF::tool