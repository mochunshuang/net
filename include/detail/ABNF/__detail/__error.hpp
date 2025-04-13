#pragma once

#include <source_location>
#include <cstddef>

namespace mcs::abnf::__detail
{
    struct error
    {
      private:
        struct info
        {
            std::size_t err_index{};
            std::source_location location;
        };

        info e_;

      public:
        constexpr explicit error(
            std::size_t err_index,
            std::source_location loc = std::source_location::current()) noexcept
            : e_{.err_index = err_index, .location = loc}
        {
        }

        [[nodiscard]] constexpr const auto &index() const noexcept
        {
            return e_.err_index;
        }
    };
}; // namespace mcs::abnf::__detail