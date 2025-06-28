#include <array>
#include <atomic>
#include <cassert>
#include <iostream>
#include <memory_resource>
#include <cstdlib>

// NOLINTBEGIN
static std::atomic<int> g_new_count = 0;
static std::atomic<int> g_delete_count = 0;

struct A
{
    int a{0};
};

void *operator new(size_t size)
{
    g_new_count++;
    return std::malloc(size);
}

void operator delete(void *ptr) noexcept
{
    g_delete_count++;
    std::free(ptr);
}
// NOLINTEND

int main()
{
    {
        assert(g_new_count == 0);
        assert(g_delete_count == 0);

        auto *ptr = new A;
        assert(g_new_count == 1);
        delete ptr;
        assert(g_delete_count == 1);
    }

    assert(g_new_count == 1);
    assert(g_delete_count == 1);
    std::pmr::unsynchronized_pool_resource pool(std::pmr::new_delete_resource());

    auto *ptr1 = pool.allocate(256);
    std::cout << "pool.allocate first: " << ptr1 << '\n';

    auto *ptr2 = pool.allocate(256);
    std::cout << "pool.allocate second: " << ptr2 << '\n';

    pool.release();
    std::cout << "g_new_count = " << g_new_count << "\n";
    std::cout << "g_delete_count = " << g_delete_count << "\n";

    {
        struct chunk
        {
            char buffer[16]; // NOLINT
        };
        auto *ptr = new chunk;
        delete ptr;
        assert(g_new_count == 2);
        assert(g_delete_count == 2);
    }
    {
        struct chunk
        {
            std::array<char, 16> buffer; // NOLINT
        };
        auto *ptr = new chunk;
        delete ptr;
        assert(g_new_count == 3);
        assert(g_delete_count == 3);

        // NOTE: 内存分堆内存 和 栈内存
        // NOTE: 编译期 和 运行时 互不干扰. 平行世界
        constexpr auto res = []() {
            auto *ptr = new chunk;
            delete ptr;
            return true;
        }();
        static_assert(res);
        assert(g_new_count == 3);
        assert(g_delete_count == 3);
    }
    std::cout << "main done\n";
    return 0;
}