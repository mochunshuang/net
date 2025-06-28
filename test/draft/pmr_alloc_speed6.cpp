#include <memory_resource>
#include <vector>
#include <deque>
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>

constexpr size_t TEST_CYCLES = 10000;     // 增加测试周期数
constexpr size_t MAX_PACKET_SIZE = 16384; // 最大包大小(16KB)
constexpr size_t MIN_PACKET_SIZE = 64;    // 最小包大小(64B)
constexpr size_t WARMUP_CYCLES = 1000;    // 预热周期

// 生成随机大小的数据包
std::vector<size_t> generate_random_sizes(size_t cycles, size_t min, size_t max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(min, max);

    std::vector<size_t> sizes(cycles);
    for (size_t i = 0; i < cycles; ++i)
    {
        sizes[i] = dist(gen);
    }
    return sizes;
}

// 测试std::vector的重用性能
void test_std_vector_reuse(const std::vector<size_t> &packet_sizes)
{
    // 预热
    std::vector<char> warmup_buffer;
    for (size_t i = 0; i < WARMUP_CYCLES; ++i)
    {
        size_t size = packet_sizes[i % packet_sizes.size()];
        if (warmup_buffer.capacity() < size)
        {
            warmup_buffer.reserve(size);
        }
        warmup_buffer.resize(size);
        std::fill(warmup_buffer.begin(), warmup_buffer.end(), 0);
        warmup_buffer.clear();
    }

    auto start = std::chrono::high_resolution_clock::now();
    size_t total_allocations = 0;
    size_t max_capacity = 0;

    std::vector<char> buffer;
    for (size_t cycle = 0; cycle < TEST_CYCLES; ++cycle)
    {
        size_t size = packet_sizes[cycle % packet_sizes.size()];

        if (buffer.capacity() < size)
        {
            buffer.reserve(size);
            total_allocations++;
            max_capacity = std::max(max_capacity, buffer.capacity());
        }
        buffer.resize(size);

        // 模拟数据处理
        std::fill(buffer.begin(), buffer.end(), static_cast<char>(cycle % 256));

        // 保留内存，仅清空数据
        buffer.clear();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout
        << "std::vector:\n"
        << "  总时间: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n"
        << "  总分配次数: " << total_allocations << "\n"
        << "  最大容量: " << max_capacity << " 字节\n";
}

// 测试pmr::vector的重用性能
void test_pmr_vector_reuse(const std::vector<size_t> &packet_sizes)
{
    // 预热
    std::pmr::pool_options pool_opts;
    pool_opts.max_blocks_per_chunk = 128;
    pool_opts.largest_required_pool_block = MAX_PACKET_SIZE;
    std::pmr::unsynchronized_pool_resource pool(pool_opts);

    std::pmr::vector<char> warmup_buffer(&pool);
    for (size_t i = 0; i < WARMUP_CYCLES; ++i)
    {
        size_t size = packet_sizes[i % packet_sizes.size()];
        if (warmup_buffer.capacity() < size)
        {
            warmup_buffer.reserve(size);
        }
        warmup_buffer.resize(size);
        std::fill(warmup_buffer.begin(), warmup_buffer.end(), 0);
    }
    pool.release(); // 释放预热分配的内存

    auto start = std::chrono::high_resolution_clock::now();
    size_t total_allocations = 0;
    size_t max_capacity = 0;

    for (size_t cycle = 0; cycle < TEST_CYCLES; ++cycle)
    {
        size_t size = packet_sizes[cycle % packet_sizes.size()];
        std::pmr::vector<char> buffer(&pool);

        if (buffer.capacity() < size)
        {
            buffer.reserve(size);
            total_allocations++;
            max_capacity = std::max(max_capacity, buffer.capacity());
        }
        buffer.resize(size);

        // 模拟数据处理
        std::fill(buffer.begin(), buffer.end(), static_cast<char>(cycle % 256));

        // 析构时内存回归池中
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout
        << "std::pmr::vector:\n"
        << "  总时间: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n"
        << "  总分配次数: " << total_allocations << "\n"
        << "  最大容量: " << max_capacity << " 字节\n";
}

// 改进的对象池实现
class BufferPool
{
  private:
    std::pmr::unsynchronized_pool_resource pool;
    std::deque<std::pmr::vector<char>> free_buffers;
    size_t max_capacity = 0;

  public:
    std::pmr::vector<char> acquire(size_t size_hint)
    {
        // 查找容量最接近且不小于size_hint的缓冲区
        auto it = std::lower_bound(
            free_buffers.begin(), free_buffers.end(), size_hint,
            [](const auto &buf, size_t size) { return buf.capacity() < size; });

        if (it != free_buffers.end())
        {
            std::pmr::vector<char> buffer = std::move(*it);
            free_buffers.erase(it);
            buffer.resize(size_hint);
            return buffer;
        }

        // 无合适缓冲区则新建
        std::pmr::vector<char> new_buffer(&pool);
        new_buffer.reserve(size_hint);
        new_buffer.resize(size_hint);
        max_capacity = std::max(max_capacity, new_buffer.capacity());
        return new_buffer;
    }

    void release(std::pmr::vector<char> &&buffer)
    {
        buffer.clear();
        // 插入排序以保持容量有序
        auto it = std::lower_bound(
            free_buffers.begin(), free_buffers.end(), buffer.capacity(),
            [](const auto &buf, size_t cap) { return buf.capacity() < cap; });
        free_buffers.insert(it, std::move(buffer));
    }

    size_t get_max_capacity() const
    {
        return max_capacity;
    }
};

// 测试改进的对象池重用
void test_object_pool_reuse(const std::vector<size_t> &packet_sizes)
{
    // 预热
    BufferPool warmup_pool;
    for (size_t i = 0; i < WARMUP_CYCLES; ++i)
    {
        size_t size = packet_sizes[i % packet_sizes.size()];
        auto buffer = warmup_pool.acquire(size);
        std::fill(buffer.begin(), buffer.end(), 0);
        warmup_pool.release(std::move(buffer));
    }

    auto start = std::chrono::high_resolution_clock::now();
    size_t total_allocations = 0;

    BufferPool pool;
    for (size_t cycle = 0; cycle < TEST_CYCLES; ++cycle)
    {
        size_t size = packet_sizes[cycle % packet_sizes.size()];

        // 从池中获取缓冲区
        auto buffer = pool.acquire(size);

        // 模拟数据处理
        std::fill(buffer.begin(), buffer.end(), static_cast<char>(cycle % 256));

        // 释放回池
        pool.release(std::move(buffer));
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout
        << "改进的对象池模式:\n"
        << "  总时间: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n"
        << "  总分配次数: " << total_allocations << "\n"
        << "  最大容量: " << pool.get_max_capacity() << " 字节\n";
}

// 测试固定大小对象池
void test_fixed_size_pool(const std::vector<size_t> &packet_sizes)
{
    // 按大小类分组的对象池（64B, 128B, 256B, ..., 16KB）
    constexpr size_t SIZE_CLASSES = 9;
    std::array<BufferPool, SIZE_CLASSES> size_pools;
    std::array<size_t, SIZE_CLASSES> size_thresholds = {64,   128,  256,  512,  1024,
                                                        2048, 4096, 8192, 16384};

    // 查找适合的大小类
    auto get_size_class = [&](size_t size) {
        for (size_t i = 0; i < SIZE_CLASSES; ++i)
        {
            if (size <= size_thresholds[i])
                return i;
        }
        return SIZE_CLASSES - 1;
    };

    // 预热
    for (size_t i = 0; i < WARMUP_CYCLES; ++i)
    {
        size_t size = packet_sizes[i % packet_sizes.size()];
        size_t cls = get_size_class(size);
        auto buffer = size_pools[cls].acquire(size_thresholds[cls]);
        std::fill(buffer.begin(), buffer.end(), 0);
        size_pools[cls].release(std::move(buffer));
    }

    auto start = std::chrono::high_resolution_clock::now();
    size_t total_allocations = 0;

    for (size_t cycle = 0; cycle < TEST_CYCLES; ++cycle)
    {
        size_t size = packet_sizes[cycle % packet_sizes.size()];
        size_t cls = get_size_class(size);

        // 从对应大小类的池中获取缓冲区
        auto buffer = size_pools[cls].acquire(size_thresholds[cls]);

        // 调整到实际需要的大小
        buffer.resize(size);

        // 模拟数据处理
        std::fill(buffer.begin(), buffer.end(), static_cast<char>(cycle % 256));

        // 释放回池
        size_pools[cls].release(std::move(buffer));
    }

    auto end = std::chrono::high_resolution_clock::now();

    // 计算总最大容量
    size_t max_capacity = 0;
    for (const auto &pool : size_pools)
    {
        max_capacity += pool.get_max_capacity();
    }

    std::cout
        << "固定大小对象池:\n"
        << "  总时间: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n"
        << "  总分配次数: " << total_allocations << "\n"
        << "  总最大容量: " << max_capacity << " 字节\n";
}

int main()
{
    std::cout << "=== 优化后的随机大小网络数据重用测试 ===\n";

    // 生成随机包大小
    auto packet_sizes =
        generate_random_sizes(TEST_CYCLES, MIN_PACKET_SIZE, MAX_PACKET_SIZE);

    std::cout << "\n---- 测试结果 ----\n";
    test_std_vector_reuse(packet_sizes);
    test_pmr_vector_reuse(packet_sizes);
    test_object_pool_reuse(packet_sizes);
    test_fixed_size_pool(packet_sizes);

    /*
 内存池的设计权衡
优势场景：内存池擅长管理大量相同大小的小对象（如网络连接、消息头），减少系统调用。
劣势场景：对于随机大小的大块数据（如你的测试中的 64B~16KB
随机数据包），内存池的块管理开销（如查找、分割、合并）可能超过直接分配的成本。
    */
    // NOTE: 内存池的块管理开销（如查找、分割、合并）可能超过直接分配的成本
    return 0;
}