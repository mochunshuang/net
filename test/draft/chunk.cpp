#include <cstddef>
#include <iostream>
#include <array>
#include <cassert>

// NOLINTBEGIN

class Chunk
{
    // 使用 std::byte 保证原始内存语义
    alignas(std::max_align_t) std::array<std::byte, 1024> buffer;
    bool used = false;

  public:
    // 类型安全的访问接口
    template <typename T>
    T *as() noexcept
    {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
        return reinterpret_cast<T *>(buffer.data());
    }

    template <typename T>
    const T *as() const noexcept
    {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
        return reinterpret_cast<const T *>(buffer.data());
    }

    // 启用/禁用使用状态
    void mark_used() noexcept
    {
        used = true;
    }
    void mark_unused() noexcept
    {
        used = false;
    }
    bool is_used() const noexcept
    {
        return used;
    }
};

// 使用示例
struct DataHeader
{
    int version;
    std::byte flags[4];
};

int main()
{
    {
        std::byte *ptr = nullptr;
        // auto *p = static_cast<unsigned char *>(ptr); // NOTE: 不允许
    }
    Chunk chunk;

    // 安全访问为 DataHeader
    auto *header = chunk.as<DataHeader>();
    header->version = 1;

    // 标记为已使用
    chunk.mark_used();

    // 需要 unsigned char* 时（如C接口）
    unsigned char *uchar_ptr = chunk.as<unsigned char>();

    // 需要 std::byte* 时（如C++23算法）
    std::byte *byte_ptr = chunk.as<std::byte>();

    assert(chunk.is_used());
    Chunk *ptr = chunk.as<Chunk>();
    ptr->mark_unused();
    assert(not chunk.is_used());
    {
        struct s1 final
        {
        };
        struct alignas(2) s2 final
        {
        };
        static_assert(alignof(s1) == 1);
        static_assert(alignof(s2) == 2);
        std::cout << "max_align: " << alignof(std::max_align_t) << '\n';

        // 64 位。
        std::cout << "size_t 位数: " << (sizeof(size_t) * 8) << " 位" << std::endl;
    }
    // NOTE: 避免不了 reinterpret_cast
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND