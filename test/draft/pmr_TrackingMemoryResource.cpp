#include <algorithm>
#include <memory_resource>
#include <iostream>
#include <vector>
#include <iomanip>
#include <cassert>

// NOLINTBEGIN

// 监控上游分配器
class TrackingUpstreamResource : public std::pmr::memory_resource
{
  public:
    explicit TrackingUpstreamResource(
        std::pmr::memory_resource *upstream = std::pmr::new_delete_resource())
        : upstream_(upstream)
    {
    }

    // 获取操作历史
    const auto &history() const noexcept
    {
        return history_;
    }

    // 获取上游分配次数
    size_t allocation_count() const noexcept
    {
        return std::count_if(history_.begin(), history_.end(),
                             [](const auto &r) { return r.type == "allocate"; });
    }

    struct Record
    {
        std::string type;
        void *address;
        size_t size;
    };

  protected:
    void *do_allocate(size_t bytes, size_t align) override
    {
        void *p = upstream_->allocate(bytes, align);
        history_.push_back({"allocate", p, bytes});
        return p;
    }

    void do_deallocate(void *p, size_t bytes, size_t align) override
    {
        upstream_->deallocate(p, bytes, align);
        history_.push_back({"deallocate", p, bytes});
    }

    bool do_is_equal(const memory_resource &other) const noexcept override
    {
        return this == &other;
    }

  private:
    std::pmr::memory_resource *upstream_;
    std::vector<Record> history_;
};

// 打印内存历史
void print_history(const std::vector<TrackingUpstreamResource::Record> &history)
{
    std::cout << "\n内存操作历史:\n";
    std::cout << "序号 | 操作       | 地址            | 大小\n";
    std::cout << "----|------------|-----------------|--------\n";

    for (size_t i = 0; i < history.size(); ++i)
    {
        const auto &r = history[i];
        std::cout << std::setw(4) << i << " | " << std::setw(10) << r.type << " | "
                  << r.address << " | " << r.size << " 字节\n";
    }
}

// 测试 unsynchronized_pool_resource 的内存行为
void test_pool_behavior()
{
    std::cout << "===== 测试 unsynchronized_pool_resource 内存行为 =====\n";

    // 创建监控上游资源
    TrackingUpstreamResource tracking_upstream;

    // 配置池选项：每块最多5个对象
    std::pmr::pool_options options;
    options.max_blocks_per_chunk = 5;
    options.largest_required_pool_block = 64; // 假设对象大小不超过64字节

    // 创建池资源（使用监控上游）
    std::pmr::unsynchronized_pool_resource pool(options, &tracking_upstream);

    // 存储分配的内存地址
    std::vector<void *> addresses;

    std::cout << "\n=== 阶段1: 分配5个对象 ===\n";
    for (int i = 0; i < 5; i++)
    {
        void *addr = pool.allocate(32); // 假设对象大小为32字节
        addresses.push_back(addr);
        std::cout << "分配对象 " << i << " 地址: " << addr << "\n";
    }
    std::cout << "上游分配次数: " << tracking_upstream.allocation_count() << "\n";

    // NOTE: deallocate 不会释放 unsynchronized_pool_resource 的内存
    std::cout << "\n=== 阶段2: 释放对象1和3 ===\n";
    pool.deallocate(addresses[1], 32);
    pool.deallocate(addresses[3], 32);
    addresses[1] = nullptr;
    addresses[3] = nullptr;
    std::cout << "已释放2个对象\n";
    std::cout << "上游释放次数: "
              << tracking_upstream.history().size() - tracking_upstream.allocation_count()
              << "\n";

    std::cout << "\n=== 阶段3: 分配2个新对象（应重用内存） ===\n";
    void *new1 = pool.allocate(32);
    void *new2 = pool.allocate(32);
    addresses.push_back(new1);
    addresses.push_back(new2);
    std::cout << "新对象1 地址: " << new1 << "\n";
    std::cout << "新对象2 地址: " << new2 << "\n";
    std::cout << "上游分配次数: " << tracking_upstream.allocation_count()
              << " (应无新增)\n";

    std::cout << "\n=== 阶段4: 分配第6个对象===\n";
    void *obj6 = pool.allocate(32);
    addresses.push_back(obj6);
    std::cout << "新对象6 地址: " << obj6 << "\n";
    std::cout << "上游分配次数: " << tracking_upstream.allocation_count() << "\n";

    std::cout << "\n=== 阶段5: 分配更多对象 ===\n";
    for (int i = 0; i < 4; i++)
    {
        void *addr = pool.allocate(32);
        addresses.push_back(addr);
        std::cout << "分配对象 地址: " << addr << "\n";
    }
    std::cout << "上游分配次数: " << tracking_upstream.allocation_count() << "\n";

    std::cout << "\n=== 阶段6: 分配第11个对象 ===\n";
    void *obj11 = pool.allocate(32);
    addresses.push_back(obj11);
    std::cout << "新对象11 地址: " << obj11 << "\n";
    std::cout << "上游分配次数: " << tracking_upstream.allocation_count() << "\n";

    // 打印完整历史
    print_history(tracking_upstream.history());

    // 清理
    for (void *addr : addresses)
    {
        if (addr)
            pool.deallocate(addr, 32);
    }
    std::cout << "[未析构前]上游总分配次数: " << tracking_upstream.allocation_count()
              << "\n";
    std::cout << "[未析构前]上游总操作次数: " << tracking_upstream.history().size()
              << "\n";
}

int main()
{
    test_pool_behavior();
    return 0;
}
// NOLINTEND