#include <memory_resource>
#include <vector>
#include <iostream>
#include <iomanip>
#include <memory>

// NOLINTBEGIN

// 内存操作记录结构体
struct MemOp
{
    void *ptr;
    size_t size;
    bool is_alloc;
};

// 带指纹追踪的内存资源
class TracingMemoryResource : public std::pmr::memory_resource
{
  public:
    explicit TracingMemoryResource(
        std::pmr::memory_resource *upstream = std::pmr::new_delete_resource())
        : upstream_(upstream)
    {
    }

    std::vector<MemOp> history;
    size_t high_water_mark = 0;
    size_t current_usage = 0;

    void *allocate(size_t bytes, size_t align = alignof(std::max_align_t))
    {
        return do_allocate(bytes, align);
    }

    void deallocate(void *p, size_t bytes, size_t align = alignof(std::max_align_t))
    {
        do_deallocate(p, bytes, align);
    }

  protected:
    void *do_allocate(size_t bytes, size_t align) override
    {
        void *p = upstream_->allocate(bytes, align);
        history.push_back({p, bytes, true});
        current_usage += bytes;
        high_water_mark = std::max(high_water_mark, current_usage);
        return p;
    }

    void do_deallocate(void *p, size_t bytes, size_t align) override
    {
        upstream_->deallocate(p, bytes, align);
        history.push_back({p, bytes, false});
        current_usage -= bytes;
    }

    bool do_is_equal(const memory_resource &other) const noexcept override
    {
        return this == &other;
    }

  private:
    std::pmr::memory_resource *upstream_;
};

// 打印内存操作历史
void print_mem_history(const std::vector<MemOp> &history)
{
    std::cout << "\n内存操作历史:\n";
    std::cout << "序号 | 操作   | 地址            | 大小\n";
    std::cout << "----|--------|-----------------|------\n";

    for (size_t i = 0; i < history.size(); ++i)
    {
        const auto &op = history[i];
        std::cout << std::setw(2) << i << " | " << (op.is_alloc ? "分配" : "释放")
                  << " | " << op.ptr << " | " << op.size << "字节\n";
    }
}

// 测试 pmr 分配器（带池）
void test_pmr_pool_allocator()
{
    std::cout << "\n===== 测试分配器: pmr::polymorphic_allocator (带池) =====\n";

    // 创建监控资源
    auto monitor = std::make_unique<TracingMemoryResource>();
    std::pmr::unsynchronized_pool_resource pool_res(monitor.get());
    std::pmr::polymorphic_allocator<std::byte> alloc(&pool_res);

    // 阶段1: 分配5个连续块 (A-B-C-D-E)
    std::vector<void *> blocks;
    for (int i = 0; i < 5; ++i)
    {
        blocks.push_back(alloc.allocate(256));
    }

    // 阶段2: 释放中间块 (B和D)
    alloc.deallocate(static_cast<std::byte *>(blocks[1]), 256);
    alloc.deallocate(static_cast<std::byte *>(blocks[3]), 256);

    // 阶段3: 分配2个新块
    void *new1 = alloc.allocate(256);
    void *new2 = alloc.allocate(256);

    // 阶段4: 分配超大块
    void *big_block = alloc.allocate(2048);

    // 记录所有操作
    print_mem_history(monitor->history);

    // 分析重用情况
    std::cout << "\n重用分析:\n";
    std::cout << "新块1: " << new1 << " → ";
    if (new1 == blocks[1])
        std::cout << "重用了B的位置\n";
    else if (new1 == blocks[3])
        std::cout << "重用了D的位置\n";
    else
        std::cout << "新地址 (未重用)\n";

    std::cout << "新块2: " << new2 << " → ";
    if (new2 == blocks[1])
        std::cout << "重用了B的位置\n";
    else if (new2 == blocks[3])
        std::cout << "重用了D的位置\n";
    else
        std::cout << "新地址 (未重用)\n";

    // 清理
    for (void *p : blocks)
    {
        if (p)
            alloc.deallocate(static_cast<std::byte *>(p), 256);
    }
    alloc.deallocate(static_cast<std::byte *>(new1), 256);
    alloc.deallocate(static_cast<std::byte *>(new2), 256);
    alloc.deallocate(static_cast<std::byte *>(big_block), 2048);

    std::cout << ">>> unsynchronized_pool_resource: 总操作次数："
              << monitor->history.size() << '\n';
}

// 测试标准分配器
void test_std_allocator()
{
    std::cout << "\n===== 测试分配器: std::allocator =====" << std::endl;

    std::allocator<std::byte> alloc;
    std::vector<MemOp> history;
    std::vector<void *> blocks;

    // 阶段1: 分配5个连续块 (A-B-C-D-E)
    for (int i = 0; i < 5; ++i)
    {
        void *p = alloc.allocate(256);
        blocks.push_back(p);
        history.push_back({p, 256, true});
    }

    // 阶段2: 释放中间块 (B和D)
    alloc.deallocate(static_cast<std::byte *>(blocks[1]), 256);
    history.push_back({blocks[1], 256, false});
    blocks[1] = nullptr;

    alloc.deallocate(static_cast<std::byte *>(blocks[3]), 256);
    history.push_back({blocks[3], 256, false});
    blocks[3] = nullptr;

    // 阶段3: 分配2个新块
    void *new1 = alloc.allocate(256);
    history.push_back({new1, 256, true});
    void *new2 = alloc.allocate(256);
    history.push_back({new2, 256, true});

    // 阶段4: 分配超大块
    void *big_block = alloc.allocate(2048);
    history.push_back({big_block, 2048, true});

    // 记录所有操作
    print_mem_history(history);

    // 分析重用情况
    std::cout << "\n重用分析:\n";
    std::cout << "新块1: " << new1 << " → ";
    if (new1 == blocks[1])
        std::cout << "重用了B的位置\n";
    else if (new1 == blocks[3])
        std::cout << "重用了D的位置\n";
    else
        std::cout << "新地址 (未重用)\n";

    std::cout << "新块2: " << new2 << " → ";
    if (new2 == blocks[1])
        std::cout << "重用了B的位置\n";
    else if (new2 == blocks[3])
        std::cout << "重用了D的位置\n";
    else
        std::cout << "新地址 (未重用)\n";

    // 清理
    for (void *p : blocks)
    {
        if (p)
            alloc.deallocate(static_cast<std::byte *>(p), 256);
    }
    alloc.deallocate(static_cast<std::byte *>(new1), 256);
    alloc.deallocate(static_cast<std::byte *>(new2), 256);
    alloc.deallocate(static_cast<std::byte *>(big_block), 2048);
}

// 测试 pmr 分配器（单调缓冲区）
void test_pmr_monotonic_allocator()
{
    std::cout << "\n===== 测试分配器: pmr::polymorphic_allocator (单调缓冲区) =====\n";

    // 创建缓冲区
    char buffer[4096];
    auto monitor = std::make_unique<TracingMemoryResource>();

    // 使用监控资源作为上游
    std::pmr::monotonic_buffer_resource mono_res{buffer, sizeof(buffer), monitor.get()};

    std::pmr::polymorphic_allocator<std::byte> alloc(&mono_res);

    // 阶段1: 分配5个连续块 (A-B-C-D-E)
    std::vector<void *> blocks;
    for (int i = 0; i < 5; ++i)
    {
        blocks.push_back(alloc.allocate(256));
    }

    // 阶段2: 释放中间块 (B和D)
    alloc.deallocate(static_cast<std::byte *>(blocks[1]), 256);
    alloc.deallocate(static_cast<std::byte *>(blocks[3]), 256);

    // 阶段3: 分配2个新块
    void *new1 = alloc.allocate(256);
    void *new2 = alloc.allocate(256);

    // 阶段4: 分配超大块（会超出缓冲区）
    void *big_block = alloc.allocate(2048);

    // 记录所有操作
    print_mem_history(monitor->history);

    // 分析重用情况
    std::cout << "\n重用分析:\n";
    std::cout << "新块1: " << new1 << " → ";
    if (new1 == blocks[1])
        std::cout << "重用了B的位置\n";
    else if (new1 == blocks[3])
        std::cout << "重用了D的位置\n";
    else
        std::cout << "新地址 (未重用)\n";

    std::cout << "新块2: " << new2 << " → ";
    if (new2 == blocks[1])
        std::cout << "重用了B的位置\n";
    else if (new2 == blocks[3])
        std::cout << "重用了D的位置\n";
    else
        std::cout << "新地址 (未重用)\n";

    // 清理
    for (void *p : blocks)
    {
        if (p)
            alloc.deallocate(static_cast<std::byte *>(p), 256);
    }
    alloc.deallocate(static_cast<std::byte *>(new1), 256);
    alloc.deallocate(static_cast<std::byte *>(new2), 256);
    alloc.deallocate(static_cast<std::byte *>(big_block), 2048);

    std::cout << ">>> monotonic_buffer_resource: 总操作次数：" << monitor->history.size()
              << '\n';
}

// 新增：直接使用 unsynchronized_pool_resource
void test_direct_unsync_pool()
{
    std::cout << "\n===== 测试分配器: 直接使用 unsynchronized_pool_resource =====\n";

    // 创建监控资源
    auto monitor = std::make_unique<TracingMemoryResource>();

    // 直接使用池资源，不通过 polymorphic_allocator
    std::pmr::unsynchronized_pool_resource pool_res(std::pmr::pool_options{},
                                                    monitor.get());

    // 阶段1: 分配5个连续块 (A-B-C-D-E)
    std::vector<void *> blocks;
    for (int i = 0; i < 5; ++i)
    {
        void *p = pool_res.allocate(256);
        blocks.push_back(p);
    }

    // 阶段2: 释放中间块 (B和D)
    pool_res.deallocate(blocks[1], 256);
    pool_res.deallocate(blocks[3], 256);

    // 阶段3: 分配2个新块
    void *new1 = pool_res.allocate(256);
    void *new2 = pool_res.allocate(256);

    // 阶段4: 分配超大块
    void *big_block = pool_res.allocate(2048);

    // 记录所有操作
    print_mem_history(monitor->history);

    // 分析重用情况
    std::cout << "\n重用分析:\n";
    std::cout << "新块1: " << new1 << " → ";
    if (new1 == blocks[1])
        std::cout << "重用了B的位置\n";
    else if (new1 == blocks[3])
        std::cout << "重用了D的位置\n";
    else
        std::cout << "新地址 (未重用)\n";

    std::cout << "新块2: " << new2 << " → ";
    if (new2 == blocks[1])
        std::cout << "重用了B的位置\n";
    else if (new2 == blocks[3])
        std::cout << "重用了D的位置\n";
    else
        std::cout << "新地址 (未重用)\n";

    // 清理
    for (void *p : blocks)
    {
        if (p)
            pool_res.deallocate(p, 256);
    }
    pool_res.deallocate(new1, 256);
    pool_res.deallocate(new2, 256);
    pool_res.deallocate(big_block, 2048);

    std::cout << ">>> unsynchronized_pool_resource: 总操作次数："
              << monitor->history.size() << '\n';
}

void test_pool_with_std_allocator()
{
    std::cout << "\n===== 测试: unsynchronized_pool_resource 与 std::allocator 结合 ====="
              << std::endl;

    // 创建自定义内存资源
    auto monitor = std::make_unique<TracingMemoryResource>();
    std::pmr::unsynchronized_pool_resource pool_res(monitor.get());

    // 使用标准【分配器】但自定义分配/释放函数
    struct CustomAllocator
    {
        using value_type = std::byte;

        explicit CustomAllocator(std::pmr::unsynchronized_pool_resource *p) : pool(p) {}

        std::byte *allocate(std::size_t n)
        {
            return static_cast<std::byte *>(pool->allocate(n));
        }

        void deallocate(std::byte *p, std::size_t n)
        {
            pool->deallocate(p, n);
        }

        // 添加比较运算符以满足分配器要求
        bool operator==(const CustomAllocator &other) const noexcept
        {
            return pool == other.pool;
        }

        bool operator!=(const CustomAllocator &other) const noexcept
        {
            return !(*this == other);
        }

        std::pmr::unsynchronized_pool_resource *pool;
    };

    CustomAllocator alloc(&pool_res);

    // 使用手动管理容器元素
    std::vector<std::byte *> blocks;

    // 阶段1: 分配5个连续块 (A-B-C-D-E)
    for (int i = 0; i < 5; ++i)
    {
        blocks.push_back(alloc.allocate(256));
    }

    // 阶段2: 释放中间块 (B和D)
    alloc.deallocate(blocks[1], 256);
    blocks[1] = nullptr;

    alloc.deallocate(blocks[3], 256);
    blocks[3] = nullptr;

    // 阶段3: 分配2个新块
    std::byte *new1 = alloc.allocate(256);
    std::byte *new2 = alloc.allocate(256);

    // 阶段4: 分配超大块
    std::byte *big_block = alloc.allocate(2048);

    // 记录所有操作
    print_mem_history(monitor->history);

    // 分析重用情况
    std::cout << "\n重用分析:\n";
    std::cout << "新块1: " << static_cast<void *>(new1) << " → ";
    if (new1 == blocks[1])
        std::cout << "重用了B的位置\n";
    else if (new1 == blocks[3])
        std::cout << "重用了D的位置\n";
    else
        std::cout << "新地址 (未重用)\n";

    std::cout << "新块2: " << static_cast<void *>(new2) << " → ";
    if (new2 == blocks[1])
        std::cout << "重用了B的位置\n";
    else if (new2 == blocks[3])
        std::cout << "重用了D的位置\n";
    else
        std::cout << "新地址 (未重用)\n";

    // 清理
    for (auto p : blocks)
    {
        if (p)
            alloc.deallocate(p, 256);
    }
    alloc.deallocate(new1, 256);
    alloc.deallocate(new2, 256);
    alloc.deallocate(big_block, 2048);
}

int main()
{
    test_direct_unsync_pool();
    // 测试三种分配器
    test_pmr_pool_allocator();
    test_pool_with_std_allocator(); // NOTE: 前面3个 地址意义。但是这个 无法重用
    test_std_allocator();
    test_pmr_monotonic_allocator();

    // NOTE: buffer 用 pool 还是能重用的，但是建议是 unsynchronized_pool_resource

    // NOTE: new/delete 调用系统分配的次数多于unsynchronized_pool_resource，所以性能差

    /**
     * @brief
类 std::pmr::unsynchronized_pool_resource
是一个通用的【内存资源类】，具有以下属性
它拥有已分配的内存，并在销毁时释放它，即使某些已分配的块没有调用 deallocate。
它由一系列池组成，这些池服务于不同块大小的请求。每个池管理一系列块，然后将这些块划分为大小均匀的块。
对 do_allocate 的调用被分派到服务于【可容纳请求大小的最小块的池】。
池中内存耗尽会导致该池的下一个分配请求从上游分配器分配额外的内存块来补充池。获得的块大小呈几何级数增长。
超过最大块大小的分配请求直接由上游分配器服务。
可以通过将 std::pmr::pool_options 结构体传递给其构造函数来调整最大块大小和最大块大小。
     *
     */

    return 0;
}
// NOLINTEND