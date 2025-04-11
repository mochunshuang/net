#pragma once

#include <source_location>
#include <cstddef>

namespace mcs::ABNF::detail
{
    class ParseError
    {
        struct info
        {
            std::size_t err_index{};
            std::source_location location;
        };

        info firstError_;
        info lastError_;

      public:
        constexpr explicit ParseError(
            std::size_t err_index,
            std::source_location loc = std::source_location::current()) noexcept
            : firstError_{.err_index = err_index, .location = loc},
              lastError_{.err_index = err_index, .location = loc}
        {
        }
        constexpr void updateLastError(
            std::size_t new_consumed,
            std::source_location loc = std::source_location::current()) noexcept
        {
            lastError_.err_index = new_consumed;
            lastError_.location = loc;
        }

        [[nodiscard]] constexpr const info &firstError() const noexcept
        {
            return firstError_;
        }

        [[nodiscard]] constexpr const info &lastError() const noexcept
        {
            return lastError_;
        }

        [[nodiscard]] constexpr const auto &index() const noexcept
        {
            return firstError_.err_index;
        }
    };
}; // namespace mcs::ABNF::detail