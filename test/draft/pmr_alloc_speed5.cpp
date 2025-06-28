#include <memory_resource>
#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <algorithm>

constexpr size_t TEST_CYCLES = 1000;      // 测试周期数
constexpr size_t MAX_PACKET_SIZE = 16384; // 最大包大小(16KB)
constexpr size_t MIN_PACKET_SIZE = 64;    // 最小包大小(64B)

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
    auto start = std::chrono::high_resolution_clock::now();
    size_t total_allocations = 0;

    for (size_t cycle = 0; cycle < TEST_CYCLES; ++cycle)
    {
        size_t size = packet_sizes[cycle];
        std::vector<char> buffer;

        // 模拟网络数据接收：可能需要扩容
        if (buffer.capacity() < size)
        {
            buffer.reserve(size);
            total_allocations++;
        }
        buffer.resize(size);

        // 模拟数据处理
        std::fill(buffer.begin(), buffer.end(), 0);

        // 模拟数据重用：清空但保留内存
        buffer.clear();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout
        << "std::vector:\n"
        << "  总时间: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n"
        << "  总分配次数: " << total_allocations << "\n";
}

// 测试pmr::vector的重用性能
void test_pmr_vector_reuse(const std::vector<size_t> &packet_sizes)
{
    std::pmr::pool_options pool_opts;
    pool_opts.max_blocks_per_chunk = 128;
    pool_opts.largest_required_pool_block = MAX_PACKET_SIZE;
    std::pmr::unsynchronized_pool_resource pool(pool_opts);

    auto start = std::chrono::high_resolution_clock::now();
    size_t total_allocations = 0;

    for (size_t cycle = 0; cycle < TEST_CYCLES; ++cycle)
    {
        size_t size = packet_sizes[cycle];
        std::pmr::vector<char> buffer(&pool);

        // 模拟网络数据接收：可能需要扩容
        if (buffer.capacity() < size)
        {
            buffer.reserve(size);
            total_allocations++;
        }
        buffer.resize(size);

        // 模拟数据处理
        std::fill(buffer.begin(), buffer.end(), 0);

        // 内存池自动管理重用，无需手动清空
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout
        << "std::pmr::vector:\n"
        << "  总时间: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n"
        << "  总分配次数: " << total_allocations << "\n";
}

// 测试内存池重用+对象池模式
void test_object_pool_reuse(const std::vector<size_t> &packet_sizes)
{
    // 自定义对象池：管理不同大小的缓冲区
    struct BufferPool
    {
        std::pmr::unsynchronized_pool_resource pool;
        std::vector<std::pmr::vector<char>> free_buffers;

        std::pmr::vector<char> acquire(size_t size_hint)
        {
            // 查找最合适的缓冲区
            for (auto it = free_buffers.begin(); it != free_buffers.end(); ++it)
            {
                if (it->capacity() >= size_hint)
                {
                    std::pmr::vector<char> buffer = std::move(*it);
                    free_buffers.erase(it);
                    buffer.resize(size_hint);
                    return buffer;
                }
            }
            // 无合适缓冲区则新建
            return std::pmr::vector<char>(size_hint, 0, &pool);
        }

        void release(std::pmr::vector<char> &&buffer)
        {
            buffer.clear();
            free_buffers.push_back(std::move(buffer));
        }
    };

    BufferPool pool;
    auto start = std::chrono::high_resolution_clock::now();
    size_t total_allocations = 0;

    for (size_t cycle = 0; cycle < TEST_CYCLES; ++cycle)
    {
        size_t size = packet_sizes[cycle];

        // 从池中获取缓冲区
        auto buffer = pool.acquire(size);

        // 模拟数据处理
        std::fill(buffer.begin(), buffer.end(), 0);

        // 释放回池
        pool.release(std::move(buffer));
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout
        << "对象池模式:\n"
        << "  总时间: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n"
        << "  总分配次数: " << total_allocations << "\n";
}

int main()
{
    std::cout << "=== 随机大小网络数据重用测试 ===\n";

    // 生成随机包大小
    auto packet_sizes =
        generate_random_sizes(TEST_CYCLES, MIN_PACKET_SIZE, MAX_PACKET_SIZE);

    std::cout << "\n---- 第一轮测试 ----\n";
    test_std_vector_reuse(packet_sizes);
    test_pmr_vector_reuse(packet_sizes);
    test_object_pool_reuse(packet_sizes);

    std::cout << "\n---- 第二轮测试（模拟长时间运行） ----\n";
    test_std_vector_reuse(packet_sizes);
    test_pmr_vector_reuse(packet_sizes);
    test_object_pool_reuse(packet_sizes);
    // NOTE: std::pmr::vector 还是 比不了一点点 std::vector
    return 0;
}