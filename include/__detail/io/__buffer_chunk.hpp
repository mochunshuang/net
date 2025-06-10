#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <optional>
#include <bit>

#include <cassert>

namespace mcs::net::io
{

    // NOTE: buffer_count_max == 64
    // NOTE: buffer_count 或许可用运行时？
    template <uint8_t buffer_count, uint16_t one_buffer_size = 4 * 1024> // NOLINT
        requires(buffer_count <= sizeof(unsigned long long) * 8)         // NOLINT
    struct buffer_chunk
    {
        using index_type = uint8_t;
        using data_type = uint8_t;

        struct buffer_type
        {
            static constexpr auto buffer_size = one_buffer_size; // NOLINT

            constexpr auto *data() noexcept
            {
                return data_.data();
            }
            constexpr const auto *data() const noexcept
            {
                return data_.data();
            }

          private:
            std::array<data_type, buffer_size> data_; // NOTE: 是否惰性
            static_assert(sizeof(std::array<data_type, 1>) == 1, "check data_type");
        };

        constexpr buffer_chunk() noexcept = default;

        // get available_index from buffer_chunk
        constexpr std::optional<index_type> available_index() noexcept // NOLINT
        {
            if (available_.none())
                return std::nullopt;
            return std::countr_zero(available_.to_ullong());
        }

        // allocate buffer from available_index
        constexpr buffer_type &allocate(index_type index) noexcept
        {
            assert(available_.test(index) == 1);
            available_.reset(index);
            return chunk_[index];
        }
        constexpr buffer_type *allocate() noexcept
        {
            if (available_.none())
                return nullptr;
            return &(chunk_[std::countr_zero(available_.to_ullong())]);
        }

        // release from buffer pointer
        constexpr void release(const buffer_type *buf_ptr) noexcept
        {
            // if (buf_ptr == nullptr) // safe
            //     return;
            const ptrdiff_t k_index = buf_ptr - chunk_.data();
            assert(k_index >= 0 && static_cast<size_t>(k_index) < buffer_count);
            release(static_cast<index_type>(k_index));
        }

        // get index from buf_ptr
        constexpr auto get_index(const buffer_type *buf_ptr) const noexcept // NOLINT
        {
            const ptrdiff_t k_diff = buf_ptr - chunk_.data();
            assert(k_diff >= 0 && static_cast<size_t>(k_diff) < buffer_count);
            return static_cast<index_type>(k_diff);
        }

        // state
        constexpr auto available_count() const noexcept // NOLINT
        {
            return available_.count();
        }
        [[nodiscard]] constexpr bool available(index_type index) const noexcept
        {
            return available_.test(index);
        }

      private:
        std::bitset<buffer_count> available_{~0ULL};
        std::array<buffer_type, buffer_count> chunk_;

        // release buffer from the allocated index
        constexpr void release(index_type index) noexcept
        {
            assert(available_.test(index) == 0);
            available_.set(index);
            // NOTE: it is need to memory clean to 0 ?
        }
    };
}; // namespace mcs::net::io