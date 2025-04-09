#pragma once

#include <array>
#include <cstdint>
#include <bit>
#include <mutex>

template <typename T>
struct SafeHeapManager
{
    struct Chunk;
    struct BlockHeader
    {
        Chunk *chunk;
        uint8_t index;
    };

    // 侵入式链表节点
    struct ListNode
    {
        ListNode *prev{};
        ListNode *next{};
    };
    enum ListType : std::uint8_t
    {
        FREE,
        TMP,
        WAIT_DELETE,
        NONE
    };
    // NOLINTBEGIN
    struct Chunk
    {
        ListNode list_node;

        static Chunk *from_list_node(ListNode *node) noexcept
        {
            return static_cast<Chunk *>(static_cast<void *>(node));
        }

        using status_type = std::uint64_t;
        using index_type = std::uint8_t;
        static constexpr size_t BIT_COUNT = sizeof(status_type) * CHAR_BIT;
        static constexpr status_type INIT_STATUS = ~status_type{0};
        static constexpr size_t BLOCK_ALIGN =
            (alignof(BlockHeader) > alignof(T)) ? alignof(BlockHeader) : alignof(T);

        status_type status{INIT_STATUS};
        ListType current_list{NONE};
        alignas(BLOCK_ALIGN)
            std::array<std::byte, (sizeof(T) + sizeof(BlockHeader)) * BIT_COUNT> block;

        static BlockHeader *get_header(T *ptr) noexcept
        {
            return reinterpret_cast<BlockHeader *>(reinterpret_cast<std::byte *>(ptr) +
                                                   sizeof(T));
        }

        [[nodiscard]] index_type find_free_slot() const noexcept
        {
            assert(status != 0);
            return std::countl_zero(status);
        }

        T *allocate(index_type index) noexcept
        {
            const status_type mask = status_type{1} << (BIT_COUNT - 1 - index);
            status &= ~mask;
            size_t offset = index * (sizeof(T) + sizeof(BlockHeader));
            new (&block[offset]) T();
            new (&block[offset + sizeof(T)]) BlockHeader{this, index};
            return reinterpret_cast<T *>(&block[offset]);
        }

        void deallocate(index_type index) noexcept
        {
            const status_type mask = status_type{1} << (BIT_COUNT - 1 - index);
            status |= mask;
        }

        [[nodiscard]] bool no_available() const noexcept
        {
            return status == 0;
        }
        [[nodiscard]] bool all_available() const noexcept
        {
            return status == INIT_STATUS;
        }
    };
    // NOLINTEND

    static_assert(offsetof(Chunk, list_node) == 0, "list_node must be the first member");
    static_assert(std::is_standard_layout_v<Chunk>, "Chunk must have standard layout");

    struct ChunkList
    {
        ListNode head;      // NOLINT
        ListType list_type; // NOLINT

        explicit ChunkList(ListType type = NONE) : list_type(type)
        {
            head.prev = &head;
            head.next = &head;
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return head.next == &head;
        }

        void push_front(Chunk *chunk) noexcept // NOLINT
        {
            assert(chunk->current_list == NONE);
            ListNode *node = &chunk->list_node;
            node->prev = &head;
            node->next = head.next;
            head.next->prev = node;
            head.next = node;
            chunk->current_list = list_type;
        }

        void push_back(Chunk *chunk) noexcept // NOLINT
        {
            assert(chunk->current_list == ListType::NONE);
            ListNode *node = &chunk->list_node;
            node->next = &head;
            node->prev = head.prev;
            head.prev->next = node;
            head.prev = node;
            chunk->current_list = list_type;
        }

        [[nodiscard]] Chunk *header() const noexcept
        {
            if (empty())
                return nullptr;
            return Chunk::from_list_node(head.next);
        }

        Chunk *pop_front() noexcept // NOLINT
        {
            if (empty())
                return nullptr;
            ListNode *node = head.next;
            node->prev->next = node->next;
            node->next->prev = node->prev;
            node->prev = node->next = nullptr;
            Chunk *chunk = Chunk::from_list_node(node);
            chunk->current_list = ListType::NONE;
            return chunk;
        }

        [[nodiscard]] bool contains(const Chunk *chunk) const noexcept
        {
            return chunk->current_list == list_type;
        }

        Chunk *remove(Chunk *chunk) noexcept
        {
            assert(chunk->current_list == list_type);

            ListNode *node = &chunk->list_node;
            node->prev->next = node->next;
            node->next->prev = node->prev;
            node->prev = node->next = nullptr;

            chunk->current_list = ListType::NONE;
            return chunk;
        }
    };
    // NOLINTBEGIN
    ChunkList free_list{ListType::FREE};
    ChunkList tmp_list{ListType::TMP};
    ChunkList wait_delete_list{ListType::WAIT_DELETE};
    uint64_t chunk_count{0};
    std::mutex mutex;
    // NOLINTEND
    T *allocate()
    {
        std::lock_guard<std::mutex> lock(mutex);
        // free_list 为空则 new / 保证一直有值
        if (free_list.header() == nullptr)
        {
            auto *new_chunk = new Chunk();
            free_list.push_front(new_chunk);
            chunk_count++;
            return new_chunk->allocate(0);
        }

        auto *chunk = free_list.header();
        T *obj = chunk->allocate(chunk->find_free_slot());
        if (chunk->no_available())
        {
            tmp_list.push_front(free_list.pop_front());
        }
        return obj;
    }

    void deallocate(T *ptr) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex);

        BlockHeader *header = Chunk::get_header(ptr);
        Chunk *chunk = header->chunk;
        ptr->~T();
        chunk->deallocate(header->index);

        switch (chunk->current_list)
        {
        case ListType::FREE:
            // free_list -> wait_delete_list
            if (chunk_count > 1 && chunk->all_available())
                wait_delete_list.push_back(free_list.remove(chunk));
            break;
        case ListType::TMP:
            // tmp_list -> wait_delete_list || free_list
            if (chunk_count > 1 && chunk->all_available())
                wait_delete_list.push_back(tmp_list.remove(chunk));
            else
                free_list.push_back(tmp_list.remove(chunk));
            break;
        case ListType::WAIT_DELETE:
        case ListType::NONE:
            std::abort();
            break;
        default:
            break;
        }
        if (not wait_delete_list.empty())
        {
            chunk_count--;
            delete wait_delete_list.pop_front();
        }
    }
    SafeHeapManager() = default;
    SafeHeapManager(const SafeHeapManager &) = delete;
    SafeHeapManager &operator=(const SafeHeapManager &) = delete;
    SafeHeapManager(SafeHeapManager &&) = delete;
    SafeHeapManager &operator=(SafeHeapManager &&) = delete;
    ~SafeHeapManager() noexcept
    {
        auto delete_chunks = [](ChunkList &list) noexcept {
            while (auto *chunk = list.pop_front())
            {
                delete chunk;
            }
        };

        delete_chunks(free_list);
        delete_chunks(tmp_list);
        delete_chunks(wait_delete_list);
    }
};