#include <cassert>
#include <memory_resource>
#include <iostream>
#include <vector>

// NOLINTBEGIN

// 自定义监控内存资源
class monitoring_memory_resource : public std::pmr::memory_resource
{
  public:
    explicit monitoring_memory_resource(
        std::pmr::memory_resource *upstream = std::pmr::get_default_resource())
        : upstream_(upstream)
    {
    }

    size_t total_allocated() const noexcept
    {
        return total_allocated_;
    }
    size_t total_deallocated() const noexcept
    {
        return total_deallocated_;
    }
    size_t current_usage() const noexcept
    {
        return total_allocated_ - total_deallocated_;
    }

  protected:
    void *do_allocate(size_t bytes, size_t alignment) override
    {
        void *p = upstream_->allocate(bytes, alignment);
        total_allocated_ += bytes;
        allocations_++;
        return p;
    }

    void do_deallocate(void *p, size_t bytes, size_t alignment) override
    {
        upstream_->deallocate(p, bytes, alignment);
        total_deallocated_ += bytes;
        deallocations_++;
    }

    bool do_is_equal(const memory_resource &other) const noexcept override
    {
        return this == &other;
    }

  private:
    std::pmr::memory_resource *upstream_;
    size_t total_allocated_ = 0;
    size_t total_deallocated_ = 0;
    size_t allocations_ = 0;
    size_t deallocations_ = 0;
};

void print_stats(const monitoring_memory_resource &monitor, const std::string &title)
{
    std::cout << "=== " << title << " ===\n"
              << "监控资源当前状态:\n"
              << "  总分配数: " << monitor.total_allocated() << " bytes\n"
              << "  总释放数: " << monitor.total_deallocated() << " bytes\n"
              << "  净内存使用: " << monitor.current_usage() << " bytes\n"
              << std::endl;
}

int main()
{
    // NOTE: unsynchronized_pool_resource不会自动释放内存.
    //  创建监控资源作为上游资源
    monitoring_memory_resource monitor;

    // 使用监控资源作为上游创建池资源
    std::pmr::unsynchronized_pool_resource pool(&monitor);

    std::cout << "实验: std::pmr::unsynchronized_pool_resource 内存行为验证\n"
              << "===================================================\n";

    // 阶段1: 初始分配
    std::pmr::vector<void *> blocks;
    for (int i = 0; i < 1000; ++i)
    {
        blocks.push_back(pool.allocate(256));
    }
    print_stats(monitor, "初始分配后");

    // 阶段2: 释放所有内存块
    for (void *ptr : blocks)
    {
        pool.deallocate(ptr, 256);
    }
    blocks.clear();
    print_stats(monitor, "释放所有块后");

    // NOTE: 不会调用上游的 delete
    assert(monitor.total_deallocated() == 0);

    // 阶段3: 再次分配相同大小的块
    for (int i = 0; i < 1000; ++i)
    {
        blocks.push_back(pool.allocate(256));
    }
    print_stats(monitor, "重新分配相同块后");

    // NOTE: 不会调用上游的 delete
    assert(monitor.total_deallocated() == 0);

    // 阶段4: 释放所有块并调用release()
    for (void *ptr : blocks)
    {
        pool.deallocate(ptr, 256);
    }
    blocks.clear();
    pool.release();
    print_stats(monitor, "调用release()后");

    // NOTE:  pool.release(); 才释放前面的内存
    assert(monitor.total_deallocated() != 0);

    // 阶段5: 验证内存重用
    for (int i = 0; i < 1000; ++i)
    {
        blocks.push_back(pool.allocate(256));
    }
    print_stats(monitor, "再次分配后");

    std::cout << "===================================================\n"
              << "结论分析:\n"
              << "1. 释放后未调用release()时，净内存使用量保持不变\n"
              << "2. 重新分配时，监控资源无新分配记录\n"
              << "3. 调用release()后，净内存使用量降为零\n"
              << "4. 证明unsynchronized_pool_resource不会自动释放内存\n";
}
// NOLINTEND