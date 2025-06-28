#include <iostream>
#include <memory_resource>
#include <vector>
#include <atomic>
#include <algorithm>
#include <cassert>

// NOLINTBEGIN

// 自定义内存资源，用于统计分配和释放次数
class TrackingResource : public std::pmr::memory_resource
{
  public:
    std::atomic<size_t> allocation_count{0};
    std::atomic<size_t> deallocation_count{0};
    std::atomic<size_t> total_bytes_allocated{0};
    std::atomic<size_t> total_bytes_deallocated{0};

    TrackingResource(
        std::pmr::memory_resource *upstream = std::pmr::get_default_resource())
        : upstream_(upstream)
    {
    }

    ~TrackingResource() override
    {
        // NOTE: 验证所有分配的内存都被释放
        assert(total_bytes_allocated == total_bytes_deallocated);
    }

  protected:
    void *do_allocate(size_t bytes, size_t alignment) override
    {
        ++allocation_count;
        total_bytes_allocated += bytes;
        return upstream_->allocate(bytes, alignment);
    }

    void do_deallocate(void *p, size_t bytes, size_t alignment) override
    {
        ++deallocation_count;
        total_bytes_deallocated += bytes;
        upstream_->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const memory_resource &other) const noexcept override
    {
        return this == &other || upstream_->is_equal(other);
    }

  private:
    std::pmr::memory_resource *upstream_;
};

// 可观察的测试对象，记录内存地址和生命周期
class ObservableObject
{
  public:
    static inline std::atomic<size_t> next_id = 0;

    ObservableObject() : id_(next_id++)
    {
        // 记录对象的内存地址
        address_ = reinterpret_cast<uintptr_t>(this);
    }

    ~ObservableObject() = default;

    size_t id() const
    {
        return id_;
    }
    uintptr_t address() const
    {
        return address_;
    }

  private:
    size_t id_;
    uintptr_t address_;
};

// 测试内存复用策略：释放的内存是否被下一次分配立即复用
void test_memory_reuse_immediate()
{
    std::cout << "\n=== 测试立即内存复用策略 ===\n";

    TrackingResource tracking_resource;
    std::pmr::unsynchronized_pool_resource pool(&tracking_resource);
    std::pmr::polymorphic_allocator<ObservableObject> alloc(&pool);

    const size_t batch_size = 10;

    // 第一批次：分配对象A1-A10
    std::vector<ObservableObject *, std::pmr::polymorphic_allocator<ObservableObject *>>
        batch(alloc);
    batch.reserve(batch_size);

    for (size_t i = 0; i < batch_size; ++i)
    {
        batch.push_back(alloc.new_object<ObservableObject>());
    }

    std::vector<std::pair<uintptr_t, uintptr_t>> reuse_pairs;

    // 逐个释放A1-A10，并立即分配新对象，检查内存地址是否复用
    for (size_t i = 0; i < batch_size; ++i)
    {
        uintptr_t released_address = batch[i]->address();
        std::cout << "释放对象 ID=" << batch[i]->id() << ", 地址=" << released_address
                  << "\n";

        // 释放当前对象
        alloc.delete_object(batch[i]);

        // 立即分配一个新对象
        ObservableObject *new_obj = alloc.new_object<ObservableObject>();
        uintptr_t new_address = new_obj->address();
        std::cout << "分配新对象 ID=" << new_obj->id() << ", 地址=" << new_address
                  << "\n";

        // 记录释放的地址和新分配的地址
        reuse_pairs.push_back({released_address, new_address});

        // 保存新对象的指针，以便后续释放
        batch[i] = new_obj;
    }

    // 分析复用情况
    size_t immediate_reuse_count = 0;
    for (const auto &pair : reuse_pairs)
    {
        if (pair.first == pair.second)
        {
            ++immediate_reuse_count;
        }
    }

    std::cout << "\n内存复用分析:\n";
    std::cout << "- 立即复用次数: " << immediate_reuse_count << " / " << batch_size
              << "\n";
    std::cout << "- 立即复用率: " << (immediate_reuse_count * 100.0 / batch_size)
              << "%\n";
    std::cout << "- 向上游分配器请求次数: " << tracking_resource.allocation_count << "\n";
    std::cout << "- 向上游分配器释放次数: " << tracking_resource.deallocation_count
              << "\n";

    if (immediate_reuse_count == batch_size)
    {
        std::cout << "- 结论: 对象池总是立即复用最近释放的内存\n";
    }
    else if (immediate_reuse_count > 0)
    {
        std::cout << "- 结论: 对象池部分复用最近释放的内存\n";
    }
    else
    {
        std::cout << "- 结论: 对象池不使用最近释放的内存，可能使用其他策略\n";
    }

    // 释放所有对象
    for (auto obj : batch)
    {
        alloc.delete_object(obj);
    }
}

// 测试内存复用策略：是否使用"最老"的释放内存
void test_memory_reuse_oldest()
{
    std::cout << "\n=== 测试最老内存复用策略 ===\n";

    TrackingResource tracking_resource;
    std::pmr::unsynchronized_pool_resource pool(&tracking_resource);
    std::pmr::polymorphic_allocator<ObservableObject> alloc(&pool);

    const size_t batch_size = 10;

    // 第一批次：分配对象A1-A10
    std::vector<ObservableObject *, std::pmr::polymorphic_allocator<ObservableObject *>>
        batch(alloc);
    batch.reserve(batch_size);

    for (size_t i = 0; i < batch_size; ++i)
    {
        batch.push_back(alloc.new_object<ObservableObject>());
    }

    // 记录所有对象的地址
    std::vector<uintptr_t> released_addresses;

    // 释放A1-A10，但不立即分配新对象
    for (size_t i = 0; i < batch_size; ++i)
    {
        released_addresses.push_back(batch[i]->address());
        std::cout << "释放对象 ID=" << batch[i]->id()
                  << ", 地址=" << released_addresses.back() << "\n";
        alloc.delete_object(batch[i]);
    }

    // 清空批次
    batch.clear();

    // 分配新对象B1-B10，检查是否按释放顺序复用内存
    std::vector<ObservableObject *, std::pmr::polymorphic_allocator<ObservableObject *>>
        new_batch(alloc);
    new_batch.reserve(batch_size);

    for (size_t i = 0; i < batch_size; ++i)
    {
        ObservableObject *new_obj = alloc.new_object<ObservableObject>();
        uintptr_t new_address = new_obj->address();
        std::cout << "分配新对象 ID=" << new_obj->id() << ", 地址=" << new_address;

        if (i < released_addresses.size() && new_address == released_addresses[i])
        {
            std::cout << " (复用了最老释放的内存)\n";
        }
        else
        {
            std::cout << "\n";
        }

        new_batch.push_back(new_obj);
    }

    // 分析复用情况
    size_t oldest_reuse_count = 0;
    for (size_t i = 0; i < std::min(batch_size, released_addresses.size()); ++i)
    {
        if (new_batch[i]->address() == released_addresses[i])
        {
            ++oldest_reuse_count;
        }
    }

    std::cout << "\n内存复用分析:\n";
    std::cout << "- 最老内存复用次数: " << oldest_reuse_count << " / " << batch_size
              << "\n";
    std::cout << "- 最老内存复用率: " << (oldest_reuse_count * 100.0 / batch_size)
              << "%\n";
    std::cout << "- 向上游分配器请求次数: " << tracking_resource.allocation_count << "\n";
    std::cout << "- 向上游分配器释放次数: " << tracking_resource.deallocation_count
              << "\n";

    if (oldest_reuse_count == batch_size)
    {
        std::cout << "- 结论: 对象池使用FIFO策略，总是复用最老释放的内存\n";
    }
    else if (oldest_reuse_count > 0)
    {
        std::cout << "- 结论: 对象池部分使用FIFO策略\n";
    }
    else
    {
        std::cout << "- 结论: 对象池不使用FIFO策略，可能使用LIFO或其他策略\n";
    }

    // 释放所有对象
    for (auto obj : new_batch)
    {
        alloc.delete_object(obj);
    }
}

int main()
{
    // 测试立即复用策略
    test_memory_reuse_immediate();

    // 测试最老内存复用策略
    test_memory_reuse_oldest();

    // NOTE: 不会主动释放 向上游释放的内存
    // 立即复用测试显示对象池总是立即复用最近释放的内存（LIFO 策略）
    // 最老内存复用测试显示对象池总是复用最老释放的内存（FIFO 策略）

    // NOTE: 二合一结论是 总是按照 释放内存的顺序 复用内存
    return 0;
}
// NOLINTEND