#include <cstdint>
#include <iostream>
#include <list>
#include <unordered_map>

#if fasle
struct buffer_chunk
{
    uint8_t count; // NOTE: 假设编译期能确定范围。理论上保证pool仅仅发生一次分配
};

// NOTE: 目标 0(1)的 增删查改 buffer_chunk 从而 pool的状态 0(1) 改变。保存不变性
struct manage_buffer_chunk
{
    // TODO(mcs): intrusive 无动态分配。肯定是 0(1)的删除和添加，即使在中间
    // NOTE: 直觉告诉我，可能比 unordered_multimap 好
    struct buffer_chunk_list
    {
    };
    // NOTE: key 是 count。 buffer_chunk.count的值 总能在 对应的 list 中
    std::unordered_multimap<uint8_t, buffer_chunk> pool;
    std::unordered_map<uint8_t, buffer_chunk_list> pool2;
};
#endif

#include <cstdint>
#include <array>
#include <optional>
#include <stdexcept>
#include <cassert>

// NOLINTBEGIN

// 假设COUNT_RANGE为count的最大可能值
constexpr size_t COUNT_RANGE = 256; // uint8_t范围0-255

struct buffer_chunk
{
    uint8_t count{};              // 当前chunk的计数值
    buffer_chunk *next = nullptr; // 侵入式链表：下一节点
    buffer_chunk *prev = nullptr; // 侵入式链表：前一节点
};

// 带哨兵节点的双向链表
template <typename node_type>
struct buffer_chunk_list
{
    // using node_type = buffer_chunk;
    // 哨兵节点（不存储实际数据）
    node_type dummy;

    buffer_chunk_list() noexcept
    {
        // 初始化哨兵节点（空链表状态）
        dummy.next = &dummy;
        dummy.prev = &dummy;
    }

    // 在链表尾部插入节点
    void push_back(node_type *chunk) noexcept
    {
        chunk->prev = dummy.prev;
        chunk->next = &dummy;

        dummy.prev->next = chunk;
        dummy.prev = chunk;
    }

    // 在链表头部插入节点
    void push_front(node_type *chunk) noexcept
    {
        chunk->next = dummy.next;
        chunk->prev = &dummy;

        dummy.next->prev = chunk;
        dummy.next = chunk;
    }

    // 从链表中移除节点
    void remove(node_type *chunk) noexcept
    {
        chunk->prev->next = chunk->next;
        chunk->next->prev = chunk->prev;

        // 重置指针（可选，增加安全性）
        chunk->next = nullptr;
        chunk->prev = nullptr;
    }

    // 检查链表是否为空（通过哨兵节点判断）
    bool empty() const noexcept
    {
        return dummy.next == &dummy;
    }

    // 获取链表首节点（不移除）
    node_type *front() const noexcept
    {
        return empty() ? nullptr : dummy.next;
    }

    // 弹出链表首节点
    node_type *pop_front() noexcept
    {
        if (empty())
            return nullptr;

        node_type *chunk = dummy.next;
        remove(chunk);
        return chunk;
    }
};

// NOTE: 管理 buffer_chunk 链表
//  固定大小的内存管理池
struct manage_buffer_chunk
{
    using node_type = buffer_chunk;

    // 固定大小的数组（索引对应count值）
    std::array<buffer_chunk_list<node_type>, COUNT_RANGE> pool;

    // 添加chunk到管理池
    void add(node_type *chunk)
    {
        if (chunk->count >= COUNT_RANGE)
        {
            throw std::out_of_range("count value exceeds COUNT_RANGE");
        }

        // NOTE: 位置
        pool[chunk->count].push_back(chunk);
    }

    // 从管理池中移除chunk
    void remove(node_type *chunk) noexcept
    {
        if (chunk->count >= COUNT_RANGE)
            return;

        // 确保chunk在正确的链表中
        assert(contains(chunk) && "Chunk not in expected list");
        pool[chunk->count].remove(chunk);
    }

    // 更新chunk的计数值
    void update_count(node_type *chunk, uint8_t new_count)
    {
        if (new_count >= COUNT_RANGE)
        {
            throw std::out_of_range("new_count exceeds COUNT_RANGE");
        }

        // 先移除旧位置
        remove(chunk);

        // 更新计数值
        chunk->count = new_count;

        // 添加到新位置
        add(chunk);
    }

    // 获取指定计数的chunk（不移除）
    node_type *get_chunk(uint8_t count) const noexcept
    {
        if (count >= COUNT_RANGE)
            return nullptr;
        return pool[count].front();
    }

    // 获取并移除指定计数的chunk
    std::optional<node_type *> pop_chunk(uint8_t count) noexcept
    {
        if (count >= COUNT_RANGE)
            return std::nullopt;

        node_type *chunk = pool[count].pop_front();
        if (chunk)
        {
            return chunk;
        }
        return std::nullopt;
    }

    // 检查是否存在指定计数的chunk
    bool has_count(uint8_t count) const noexcept
    {
        return count < COUNT_RANGE && !pool[count].empty();
    }

    // 验证chunk是否在正确的链表中
    bool contains(const node_type *chunk) const noexcept
    {
        if (chunk->count >= COUNT_RANGE)
            return false;

        // 遍历链表检查是否存在
        node_type *current = pool[chunk->count].dummy.next;
        while (current != &pool[chunk->count].dummy)
        {
            if (current == chunk)
                return true;
            current = current->next;
        }
        return false;
    }

    // 获取指定count的链表大小（调试用）
    size_t list_size(uint8_t count) const noexcept
    {
        if (count >= COUNT_RANGE)
            return 0;

        size_t size = 0;
        node_type *current = pool[count].dummy.next;
        while (current != &pool[count].dummy)
        {
            size++;
            current = current->next;
        }
        return size;
    }
};

int main()
{

    // 初始化管理器
    manage_buffer_chunk manager;

    // 创建测试chunks
    buffer_chunk chunks[5];
    for (int i = 0; i < 5; ++i)
    {
        chunks[i].count = static_cast<uint8_t>(i * 10); // 0,10,20,30,40
        manager.add(&chunks[i]);
    }

    // 验证状态
    assert(manager.has_count(0));
    assert(manager.has_count(40));
    assert(!manager.has_count(50));

    // 获取并移除chunk
    if (auto chunk = manager.pop_chunk(20))
    {
        // 使用 chunk
        assert(*chunk == &chunks[2]);
    }

    // 更新count值
    manager.update_count(&chunks[1], 25);
    assert(manager.has_count(25));
    assert(!manager.has_count(10));

    // 验证不变性
    for (int i = 0; i < 5; ++i)
    {
        if (i != 1 && i != 2)
        { // 已移动或移除
            assert(manager.contains(&chunks[i]));
        }
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND