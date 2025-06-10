#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <bit>

// NOLINTBEGIN

static_assert(sizeof(std::optional<uint8_t>) == 2);
static_assert(sizeof(unsigned long long) * 8 == 64);

template <uint8_t buffer_count, uint16_t one_buffer_size = 4 * 1024>
    requires(buffer_count <= sizeof(unsigned long long) * 8)
struct buffer_chunk
{
    using index_type = uint8_t;

    struct buffer
    {
        static constexpr auto buffer_size = one_buffer_size;
        std::array<std::uint8_t, buffer_size> data_;

        constexpr auto *data() noexcept
        {
            return data_.data();
        }
        constexpr const auto *data() const noexcept
        {
            return data_.data();
        }
    };

    constexpr buffer_chunk() noexcept : available_{~0ULL} {}

    // 获取可用缓冲区索引
    constexpr std::optional<index_type> acquire() noexcept
    {
        if (available_.none())
            return std::nullopt;
        return std::countr_zero(available_.to_ullong());
    }

    // 分配缓冲区（通过索引）
    constexpr buffer &allocate(index_type index) noexcept
    {
        assert(available_.test(index) == 1);
        available_.reset(index);
        return chunk_[index];
    }

    // 释放缓冲区（通过索引）
    constexpr void release(index_type index) noexcept
    {
        assert(available_.test(index) == 0);
        available_.set(index);
    }

    // 释放缓冲区（通过地址）
    constexpr void release(const buffer *buf_ptr) noexcept
    {
        // 计算缓冲区索引
        const auto index = buf_ptr - chunk_.data();
        assert(index >= 0 && static_cast<size_t>(index) < buffer_count);
        release(static_cast<index_type>(index));
    }

    // 获取缓冲区索引（通过地址）
    constexpr index_type get_index(const buffer *buf_ptr) const noexcept
    {
        const ptrdiff_t diff = buf_ptr - chunk_.data();
        assert(diff >= 0 && static_cast<size_t>(diff) < buffer_count);
        return static_cast<index_type>(diff);
    }

    // 访问整个数组
    constexpr auto &buffers() noexcept
    {
        return chunk_;
    }
    constexpr const auto &buffers() const noexcept
    {
        return chunk_;
    }

    // 访问单个缓冲区
    constexpr buffer &operator[](index_type index) noexcept
    {
        assert(index < buffer_count);
        return chunk_[index];
    }

    // 状态查询
    constexpr auto available_count() const noexcept
    {
        return available_.count();
    }
    constexpr bool available(index_type index) const noexcept
    {
        return available_.test(index);
    }

  private:
    std::bitset<buffer_count> available_;
    std::array<buffer, buffer_count> chunk_;
};

// ======= 测试代码 =======
void test_buffer_chunk()
{
    buffer_chunk<16> pool;
    constexpr auto buffer_count = 16;

    // 1. 测试索引访问
    auto idx = pool.acquire();
    assert(idx.has_value() && *idx == 0);

    auto &buf0 = pool.allocate(*idx);
    assert(pool.available(0) == false);

    // 2. 测试地址索引转换
    const auto *buf_ptr = &buf0;
    auto calc_idx = pool.get_index(buf_ptr);
    assert(calc_idx == 0);

    // 3. 测试通过地址释放
    pool.release(buf_ptr);
    assert(pool.available(0) == true);

    // 4. 测试数组连续性
    const auto *first = &pool.buffers()[0];
    const auto *last = &pool.buffers()[buffer_count - 1];

    {
        const auto actual_distance = last - first;
        assert(actual_distance == buffer_count - 1);

        static_assert(std::is_same_v<decltype(last - first), ptrdiff_t>);
    }

    // 计算实际内存距离
    const auto actual_distance =
        reinterpret_cast<uintptr_t>(last) - reinterpret_cast<uintptr_t>(first);

    // 计算理论内存距离
    const auto theoretical_distance =
        (buffer_count - 1) * sizeof(typename buffer_chunk<16>::buffer);

    // 验证距离
    assert(actual_distance == theoretical_distance);

    // 更简单的验证方式
    assert(last - first == buffer_count - 1);
}

int main()
{
    test_buffer_chunk();
    std::cout << "所有测试通过！\n";
    return 0;
}
// NOLINTEND