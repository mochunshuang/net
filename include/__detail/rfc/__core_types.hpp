#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>

namespace mcs::rfc
{
    struct error
    {
      private:
        std::size_t errIndex_{};

      public:
        constexpr explicit error(std::size_t err_index) noexcept : errIndex_{err_index} {}

        [[nodiscard]] constexpr const auto &index() const noexcept
        {
            return errIndex_;
        }
    };

    template <typename T>
    using ReturnType = std::expected<T, error>;

    constexpr auto err_index(std::size_t err_index) noexcept
    {
        return std::unexpected(error(err_index));
    }

    using id_type = std::uint8_t;

}; // namespace mcs::rfc