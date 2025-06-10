#pragma once

#include "./__buffer_chunk_list.hpp"
#include "./__buffer_chunk.hpp"
#include <cassert>

namespace mcs::net::io
{

    template <uint8_t buffer_count>
    struct buffer_chunk_list_pool
    {
        struct chunk_list_node
        {
            using list_type = buffer_chunk<buffer_count>;

            list_type node;            // NOLINT
            list_type *next = nullptr; // NOLINT
            list_type *prev = nullptr; // NOLINT

            [[nodiscard]] constexpr auto available_count() const noexcept // NOLINT
            {
                return node.available_count();
            }

            constexpr auto &buffer_chunk() noexcept // NOLINT
            {
                return node;
            }
            constexpr const auto &buffer_chunk() const noexcept // NOLINT
            {
                return node;
            }
        };
        using buffer_chunk_type = chunk_list_node;
        using node_type = buffer_chunk_list<buffer_chunk_type>;

        // add chunk to right buffer_chunk_list
        constexpr void add(buffer_chunk_type *chunk)
        {
            assert(chunk->available_count() < buffer_count);
            pool_[chunk->available_count()].push_back(chunk);
        }

        // remove chunk from right buffer_chunk_list
        constexpr void remove(buffer_chunk_type *chunk) noexcept
        {
            if (chunk->available_count() >= buffer_count)
                return;
            assert(contains(chunk) && "Chunk not in expected list");
            pool_[chunk->available_count()].remove(chunk);
        }

        constexpr node_type &chunk_list(uint8_t count) const noexcept // NOLINT
        {
            if (count >= buffer_count)
                return nullptr;
            return pool_[count];
        }

        // NOLINTNEXTLINE
        constexpr std::optional<buffer_chunk_type *> pop_buffer_chunk(
            uint8_t count) noexcept
        {
            if (count >= buffer_count)
                return std::nullopt;

            buffer_chunk_type *chunk = pool_[count].pop_front();
            if (chunk)
            {
                return chunk;
            }
            return std::nullopt;
        }

        //  if pool_[chunk->available_count()] has chunk
        constexpr bool contains(const buffer_chunk_type *chunk) const noexcept // NOLINT
        {
            if (chunk->count >= buffer_count)
                return false;

            buffer_chunk_type *current = pool_[chunk->available_count()].dummy.next;
            while (current != &pool_[chunk->available_count()].dummy)
            {
                if (current == chunk)
                    return true;
                current = current->next;
            }
            return false;
        }

        // NOLINTNEXTLINE
        [[nodiscard]] constexpr size_t chunk_list_size(uint8_t count) const noexcept
        {
            if (count >= buffer_count)
                return 0;
            return pool_[count].size();
        }
        // NOLINTNEXTLINE
        [[nodiscard]] constexpr bool empty_chunk_list(uint8_t count) const noexcept
        {
            assert(count < buffer_count);
            return pool_[count].empty();
        }

      private:
        std::array<node_type, buffer_count> pool_;
    };

}; // namespace mcs::net::io