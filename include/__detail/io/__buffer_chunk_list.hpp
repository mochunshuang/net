#pragma once

#include <cassert>
#include <cstddef>
#include <type_traits>

namespace mcs::net::io
{
    template <typename node_type>
        requires(requires(node_type &node) {
            node.prev;
            node.next;
            requires std::is_same_v<decltype(node.prev), node_type *>;
            requires std::is_same_v<decltype(node.next), node_type *>;
        })
    struct buffer_chunk_list
    {
        constexpr buffer_chunk_list() noexcept
        {
            dummy_.next = &dummy_;
            dummy_.prev = &dummy_;
        }

        constexpr void push_back(node_type *chunk) noexcept // NOLINT
        {
            chunk->prev = dummy_.prev;
            chunk->next = &dummy_;

            dummy_.prev->next = chunk;
            dummy_.prev = chunk;

            ++size_;
        }

        // add from front
        constexpr void push_front(node_type *chunk) noexcept // NOLINT
        {
            chunk->next = dummy_.next;
            chunk->prev = &dummy_;

            dummy_.next->prev = chunk;
            dummy_.next = chunk;

            ++size_;
        }

        // NOTE: no safe check
        constexpr void remove(node_type *chunk) noexcept
        {
            assert(chunk->prev != nullptr);
            assert(chunk->next != nullptr);

            chunk->prev->next = chunk->next;
            chunk->next->prev = chunk->prev;
            // safe
            chunk->next = nullptr;
            chunk->prev = nullptr;

            --size_;
        }

        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return dummy_.next == &dummy_;
        }

        constexpr node_type *front() const noexcept
        {
            return empty() ? nullptr : dummy_.next;
        }

        constexpr node_type *pop_front() noexcept // NOLINT
        {
            if (empty())
                return nullptr;

            node_type *chunk = dummy_.next;
            remove(chunk);
            return chunk;
        }

        [[nodiscard]] std::size_t size() const
        {
            return size_;
        }

      private:
        node_type dummy_;
        std::size_t size_{};
    };
}; // namespace mcs::net::io