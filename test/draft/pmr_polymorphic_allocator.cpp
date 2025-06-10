#include <iostream>
#include <memory_resource>
#include <vector>
// NOLINTBEGIN

int main()
{
    struct person
    {
        person(int a) noexcept : age(a)
        {
            std::cout << "person(int a)\n";
        }
        ~person() noexcept
        {
            std::cout << " ~person()\n";
        }
        int age;
    };
    // 1. 创建内存池（默认上游为 new/delete）
    std::pmr::unsynchronized_pool_resource pool;

    // NOTE: unsynchronized_pool_resource 需要 polymorphic_allocator 才能与标准容器
    // NOTE: （如pmr::vector）无缝协作。
    //  2. 通过 polymorphic_allocator 绑定容器
    std::pmr::polymorphic_allocator<person> alloc(&pool);
    std::pmr::vector<person> vec(alloc);

    // NOTE: std::pmr::vector 比 std::vector 性能差

    const auto k_count = 5;
    vec.reserve(k_count);

    std::cout << "emplace_back \n";

    // 3. 容器自动使用 pool 分配内存
    for (int i = 0; i < k_count; ++i)
    {
        vec.emplace_back(i); // 内存请求通过 pool 处理
    }

    std::cout << "release start \n";
    // 4. 批量释放（可选）
    pool.release(); // NOTE: 不会调用析构
    std::cout << "release end \n";
    return 0;
}
// NOLINTEND