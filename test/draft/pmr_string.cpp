#include <cassert>
#include <iostream>
#include <memory_resource>
#include <string>
// NOLINTBEGIN
// 自定义内存资源跟踪器
class TrackingResource : public std::pmr::memory_resource
{
  public:
    size_t alloc_count = 0;
    size_t dealloc_count = 0;

  protected:
    void *do_allocate(size_t bytes, size_t alignment) override
    {
        alloc_count++;
        return std::pmr::new_delete_resource()->allocate(bytes, alignment);
    }

    void do_deallocate(void *p, size_t bytes, size_t alignment) override
    {
        dealloc_count++;
        std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const memory_resource &other) const noexcept override
    {
        return this == &other;
    }
};

void test_allocation_behavior()
{
    // 1. 准备测试环境
    unsigned char buffer[1024] = {};
    TrackingResource tracker;

    // 2. 设置monotonic_buffer_resource使用我们的tracker作为上游资源
    std::pmr::monotonic_buffer_resource pool(buffer, sizeof(buffer), &tracker);

    // 3. 测试不同场景
    std::cout << "=== 测试1: 小字符串（应使用缓冲区） ===\n";
    {
        std::pmr::string small_str(&pool);
        small_str = "Small string";
        std::cout << "分配次数: " << tracker.alloc_count
                  << ", 释放次数: " << tracker.dealloc_count << "\n";
    }
    std::cout << "\n上游: 分配次数: " << tracker.alloc_count
              << ", 释放次数: " << tracker.dealloc_count << "\n";

    std::cout << "\n=== 测试2: 大字符串（应触发new/delete） ===\n";
    {
        std::pmr::string large_str(&pool);
        large_str.assign(2000, 'X'); // 超过缓冲区大小
        std::cout << "分配次数: " << tracker.alloc_count
                  << ", 释放次数: " << tracker.dealloc_count << "\n";
    }
    // NOTE: 不自动调用delete 。 可能是 BUG 的来源
    std::cout << "\n上游: 最终分配次数: " << tracker.alloc_count
              << ", 最终释放次数: " << tracker.dealloc_count << "\n";

    {
        std::pmr::monotonic_buffer_resource pool(buffer, sizeof(buffer), &tracker);
        std::pmr::string large_str(&pool);
        large_str.assign(2000, 'X');
        // 手动释放超额分配的部分
        pool.release(); // 这会调用上游的deallocate
    }
    // NOTE: monotonic_buffer_resource 假定 你能自己释放内存。
    std::cout << "\n上游: 最终分配次数: " << tracker.alloc_count
              << ", 最终释放次数: " << tracker.dealloc_count << "\n";
}

void test_release_behavior()
{
    unsigned char buffer[1024] = {};
    TrackingResource tracker;
    std::pmr::monotonic_buffer_resource pool(buffer, sizeof(buffer), &tracker);

    // 分配3次（均超过缓冲区大小）
    std::pmr::string str1(&pool);
    str1.assign(2000, 'X');
    std::pmr::string str2(&pool);
    str2.assign(3000, 'Y');
    std::pmr::string str3(&pool);
    str3.assign(4000, 'Z');

    std::cout << "分配次数: " << tracker.alloc_count << "\n";               // 输出3
    std::cout << "release 前: 释放次数: " << tracker.dealloc_count << "\n"; // 输出0

    pool.release(); // 关键操作

    std::cout << "release()后释放次数: " << tracker.dealloc_count << "\n"; // 输出3
}

void prove_monotonic_behavior()
{
    TrackingResource tracker;
    std::pmr::monotonic_buffer_resource pool(&tracker); // 无初始缓冲区

    std::cout << "初始状态 - 分配次数: " << tracker.alloc_count
              << ", 释放次数: " << tracker.dealloc_count << "\n";

    void *str1_ptr = nullptr;
    void *str2_ptr = nullptr;
    // 第一次分配（RAII对象）
    {
        std::pmr::string str1(&pool);
        str1.assign(1000, 'A'); // 触发分配
        str1_ptr = str1.data();
        std::cout << "str1构造后 - 分配次数: " << tracker.alloc_count
                  << ", 释放次数: " << tracker.dealloc_count << "\n";
    } // str1析构，但内存未释放

    // 第二次分配（RAII对象）
    {
        std::pmr::string str2(&pool);
        str2.assign(2000, 'B'); // 再次触发分配
        str2_ptr = str2.data();
        std::cout << "str2构造后 - 分配次数: " << tracker.alloc_count
                  << ", 释放次数: " << tracker.dealloc_count << "\n";
    } // str2析构，内存仍未释放

    pool.release(); // 手动释放所有内存
    std::cout << "release()后 - 分配次数: " << tracker.alloc_count
              << ", 释放次数: " << tracker.dealloc_count << "\n";

    // NOTE: 地址偏移 总是增长的
    assert(str2_ptr > str1_ptr);
}

void test_initial_buffer()
{
    char initial_buffer[1024]; // 初始缓冲区（1KB）
    std::pmr::monotonic_buffer_resource pool(initial_buffer,
                                             sizeof(initial_buffer) // 显式传入初始缓冲区
    );

    std::cout << "[1] 初始缓冲区地址: " << (void *)initial_buffer << "\n";

    // 第一次分配（使用初始缓冲区）
    {
        std::pmr::vector<int> vec1(&pool);
        vec1.assign(10, 42); // 小分配，预计使用初始缓冲区
        std::cout
            << "[2] vec1.data() 地址: " << vec1.data() << " (是否在初始缓冲区范围内? "
            << (static_cast<void *>(vec1.data()) >= static_cast<void *>(initial_buffer) &&
                        static_cast<void *>(vec1.data()) <
                            static_cast<void *>(initial_buffer + sizeof(initial_buffer))
                    ? "是"
                    : "否")
            << ")\n";
    }

    // 第二次分配（耗尽初始缓冲区）
    {
        std::pmr::vector<char> vec2(&pool);
        vec2.assign(2000, 'X'); // 大分配，超过初始缓冲区大小
        std::cout << "[3] vec2.data() 地址: " << (void *)vec2.data()
                  << " (是否在初始缓冲区范围内? "
                  << (vec2.data() >= initial_buffer &&
                              vec2.data() < initial_buffer + sizeof(initial_buffer)
                          ? "是"
                          : "否")
                  << ")\n";
    }

    // 显式释放前，初始缓冲区仍被pool持有（即使未被完全使用）
    std::cout << "[4] 调用release()前，初始缓冲区仍被控制\n";
    assert(initial_buffer[0] == 42);
    pool.release(); // 此时初始缓冲区才"解除绑定"
    std::cout << "[5] release()后，初始缓冲区可安全重用\n";
    assert(initial_buffer[0] == 42); // NOTE: 不会做额外的操作，不zero 内存

    /**
     * @brief 避免频繁的内存释放/申请操作，提升性能
              初始缓冲区常用于避免小分配时的上游调用（如 new/delete）
     *
     */
    // NOTE: 适用于局部处理且明确知道最大的 数据范围设计的。 和 C++ 自带内存管理相违背
}

int main()
{
    std::cout << "===========test_allocation_behavior\n";
    test_allocation_behavior();
    std::cout << "\n===========test_release_behavior\n";
    test_release_behavior();
    std::cout << "\n===========prove_monotonic_behavior\n";
    prove_monotonic_behavior();
    std::cout << "\n===========test_initial_buffer\n";
    test_initial_buffer();
    return 0;
}
// NOLINTEND