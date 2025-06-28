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
    std::cout << "operator new\n";
    g_new_count++;
    return std::malloc(size);
}

void operator delete(void *ptr) noexcept
{
    std::cout << "operator delete\n";
    g_delete_count++;
    std::free(ptr);
}
// NOLINTEND

int main()
{
    // NOTE: 我宣布 mingw 就是比不上 clang。 在windows 不要用 mingw
    // NOTE: mingw 这里直接崩溃
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

    static constexpr auto k_obj_size = 64;
    auto *ptr1 = pool.allocate(k_obj_size);
    std::cout << "pool.allocate first: " << ptr1 << '\n';

    auto *ptr2 = pool.allocate(k_obj_size);
    std::cout << "pool.allocate second: " << ptr2 << '\n';

    assert(ptr1 != ptr2);

    std::cout << "ptr2 - ptr1: " << (ptrdiff_t)ptr2 - (ptrdiff_t)ptr1 << '\n';

    // NOTE: 线性的地址。 不会走 new / delete
    assert(2 * k_obj_size == ((ptrdiff_t)ptr2 - (ptrdiff_t)ptr1));

    pool.release();

    assert(g_new_count == 1);
    assert(g_delete_count == 1);
    std::cout << "g_new_count = " << g_new_count << "\n";
    std::cout << "g_delete_count = " << g_delete_count << "\n";
    std::cout << "main done\n";
    return 0;
}