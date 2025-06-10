#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>

static_assert(sizeof(std::optional<uint8_t>) == 2);
static_assert(sizeof(int) == 4);                     // NOTE: 因此 负数，不代表更好
static_assert(sizeof(unsigned long long) * 8 == 64); // NOLINT

template <uint8_t buffer_count, uint16_t one_buffer_size = 4 * 1024> // NOLINT
    requires(buffer_count <= sizeof(unsigned long long) * 8)         // NOLINT
struct buffer_chunk
{
    using index_type = uint8_t;

    struct buffer
    {
        static constexpr auto buffer_size = one_buffer_size; // NOLINT

        [[nodiscard]] constexpr auto id() const noexcept
        {
            return id_;
        }
        constexpr auto &impl() noexcept
        {
            return data_;
        }
        [[nodiscard]] constexpr const auto &impl() const noexcept
        {
            return data_;
        }
        constexpr auto *data() noexcept
        {
            return data_.data();
        }
        [[nodiscard]] constexpr const auto *data() const noexcept
        {
            return data_.data();
        }

      private:
        index_type id_;
        std::array<std::uint8_t, buffer_size> data_;

        constexpr explicit buffer(index_type id) noexcept : id_{id} {}
        friend struct buffer_chunk;
    };

    constexpr buffer_chunk() noexcept
        : available_{~0ULL},
          chunk_{create_chunk(std::make_index_sequence<buffer_count>{})}
    {
    }

    constexpr std::optional<index_type> acquire() noexcept
    {
        if (available_.none())
            return std::nullopt;
        return std::countr_zero(available_.to_ullong());
    }
    constexpr buffer &allocate(index_type index) noexcept
    {
        assert(available_.test(index) == 1);
        available_.reset(index);
        return chunk_[index]; // NOLINT
    }

    constexpr void release(index_type index) noexcept
    {
        assert(available_.test(index) == 0);
        available_.set(index);
    }

    [[nodiscard]] constexpr const auto &data() const noexcept
    {
        return chunk_;
    }

    [[nodiscard]] constexpr auto available_count() const noexcept // NOLINT
    {
        return available_.count();
    }

    [[nodiscard]] constexpr auto available(index_type index) const noexcept
    {
        return available_.test(index);
    }

  private:
    std::bitset<buffer_count> available_;
    std::array<buffer, buffer_count> chunk_;

    template <size_t... Is>
    static constexpr std::array<buffer, buffer_count> create_chunk( // NOLINT
        std::index_sequence<Is...> /*unused*/) noexcept
    {
        return {buffer(static_cast<index_type>(Is))...};
    }
};

// 编译时验证初始化
static_assert(
    [] consteval {
        {
            buffer_chunk<0> chunk;
            std::array<int, 0> arr; // NOTE: 允许这么做
        }
        buffer_chunk<3> chunk;
        return chunk.data()[0].id() == 0 && chunk.data()[1].id() == 1 &&
               chunk.data()[2].id() == 2;
    }(),
    "Compile-time initialization failed");

// 运行时测试
void test_buffer_chunk() // NOLINT
{
    buffer_chunk<16> pool;

    // 验证ID顺序
    for (uint8_t i = 0; i < 16; ++i) // NOLINT
    {
        assert(pool.data()[i].id() == i); // NOLINT
    }

    // 验证初始可用位
    auto first = pool.acquire();
    assert(first.has_value() && *first == 0);

    assert(pool.available_count() == 16);
    assert(pool.available(0) == true);

    pool.allocate(*first);

    assert(pool.available_count() == 15);
    assert(pool.available(0) == false);
}

int main()
{
    test_buffer_chunk();
    {
        struct buffer
        {
            uint8_t id{0};
        };
        // 情况1: 栈上对象
        buffer stack_buf;
        std::cout << "栈上对象地址: " << &stack_buf << "\n";
        std::cout << "栈上id地址:   " << static_cast<void *>(&stack_buf.id) << "\n\n";

        assert(&stack_buf == static_cast<void *>(&stack_buf.id));

        // 情况2: 堆上对象
        auto *heap_buf = new buffer();
        std::cout << "堆上对象地址: " << heap_buf << "\n";
        std::cout << "堆上id地址:   " << static_cast<void *>(&(heap_buf->id)) << "\n";

        assert(heap_buf == static_cast<void *>(&heap_buf->id));

        delete heap_buf;
    }
    std::cout << "main done\n";
    return 0;
}