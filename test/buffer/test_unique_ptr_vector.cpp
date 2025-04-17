#include <memory>
#include <span>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <cassert>
#include <vector>
#include <chrono>

using OCTET = std::uint8_t;
using span_param_in = const std::span<const OCTET> &;
// NOLINTBEGIN
class HighPerfPacket
{
  public:
    explicit HighPerfPacket(std::span<const OCTET> data_span)
        : size_(data_span.size()), data_(std::make_unique_for_overwrite<OCTET[]>(
                                       size_)) // C++23 优化：避免默认初始化
    {
        std::memcpy(data_.get(), data_span.data(), size_); // 直接内存拷贝（最快）
    }

    // 移动构造函数
    HighPerfPacket(HighPerfPacket &&other) noexcept
        : size_(other.size_), data_(std::move(other.data_))
    {
        other.size_ = 0;
    }

    std::span<const OCTET> get_span() const
    {
        return {data_.get(), size_};
    }

  private:
    size_t size_ = 0; // NOTE: 难以范围计算 和 buffer 的范围算法不搭配
    std::unique_ptr<OCTET[]> data_;
};
class BalancedPacket
{
  public:
    explicit BalancedPacket(std::span<const OCTET> data_span)
        : data_(data_span.begin(), data_span.end()) // 直接范围构造
    {
        data_.shrink_to_fit(); // 确保 capacity == size（可选）
    }

    // 移动构造函数
    BalancedPacket(BalancedPacket &&other) noexcept = default;

    std::span<const OCTET> get_span() const
    {
        return data_;
    }

  private:
    std::vector<OCTET> data_;
};

static_assert(sizeof(HighPerfPacket) < sizeof(BalancedPacket));
static_assert(sizeof(HighPerfPacket) == 16);
static_assert(sizeof(BalancedPacket) == 24);

using namespace std::chrono;

std::vector<OCTET> generate_test_data(size_t size)
{
    std::vector<OCTET> data(size);
    for (size_t i = 0; i < size; ++i)
    {
        data[i] = static_cast<OCTET>(i % 256);
    }
    return data;
}

// 测试 HighPerfPacket (unique_ptr)
auto test_high_perf(const std::span<const OCTET> &data_span)
{
    auto start = high_resolution_clock::now();
    HighPerfPacket packet(data_span);
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count();
}

// 测试 BalancedPacket (vector)
auto test_balanced(const std::span<const OCTET> &data_span)
{
    auto start = high_resolution_clock::now();
    BalancedPacket packet(data_span);
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count();
}

int main()
{
    constexpr size_t sizes[] = {64, 1024, 1024 * 1024}; // 64B, 1KB, 1MB
    int case_ = 0;
    for (size_t size : sizes)
    {
        auto data = generate_test_data(size);
        std::cout << "\n==== 数据大小: " << size << " B ====\n";

        // 运行 n 次取平均值（减少随机误差）
        constexpr float n = 100.0;

        long long d0 = 0;
        long long d1 = 0;
        for (int i = 0; i < n; ++i)
        {
            d0 += test_high_perf(data);
            d1 += test_balanced(data);
        }
        std::cout << "[Run " << case_++ << "] \n";
        std::cout << "HighPerfPacket 构造耗时: " << d0 / n << " μs\n";
        std::cout << "BalancedPacket 构造耗时: " << d0 / n << " μs\n";
    }
    // NOTE: 没多大区别
    return 0;
}
// NOLINTEND