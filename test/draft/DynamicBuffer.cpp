#include <iostream>
#include <numeric>
#include <vector>
#include <cassert>
#include <format>
#include <algorithm>
#include <ranges>

// NOLINTBEGIN

// dynamic_vector_buffer 实现 (C++23)
template <typename T>
class dynamic_vector_buffer
{
  public:
    explicit dynamic_vector_buffer(std::vector<T> &vec,
                                   size_t max_size = std::numeric_limits<size_t>::max())
        : vec_(vec), max_size_(max_size)
    {
    }

    // 获取可写区域
    auto writable(size_t n)
    {
        // 确保有足够容量
        const size_t available = max_size_ - vec_.size();
        n = std::min(n, available);

        // 预留空间但不改变大小
        if (vec_.capacity() < vec_.size() + n)
        {
            vec_.reserve(std::min(vec_.size() + n, max_size_));
        }

        return std::span(vec_.data() + vec_.size(), n);
    }

    // 提交写入的数据
    void commit(size_t n)
    {
        if (vec_.size() + n > max_size_)
        {
            throw std::length_error("Buffer max size exceeded");
        }
        vec_.resize(vec_.size() + n);
    }

    // 获取可读数据视图
    auto readable() const
    {
        return std::span(vec_.data(), vec_.size());
    }

    // 消费已处理的数据
    void consume(size_t n)
    {
        n = std::min(n, vec_.size());

        // C++23 高效移除元素
        std::shift_left(vec_.begin(), vec_.end(), n);
        vec_.resize(vec_.size() - n);
    }

    // 当前数据大小
    size_t size() const
    {
        return vec_.size();
    }

    // 当前容量
    size_t capacity() const
    {
        return vec_.capacity();
    }

    // 最大允许大小
    size_t max_size() const
    {
        return max_size_;
    }

  private:
    std::vector<T> &vec_;
    size_t max_size_;
};

// 打印缓冲区状态
template <typename T>
void print_buffer_state(const dynamic_vector_buffer<T> &buf, std::string_view title)
{
    std::cout << std::format("\n===== {} =====\n", title);
    std::cout << std::format("大小: {}/{}\n", buf.size(), buf.capacity());
    std::cout << std::format("最大大小: {}\n", buf.max_size());

    auto data = buf.readable();
    std::cout << "内容: [";
    for (size_t i = 0; i < std::min(data.size(), size_t{10}); ++i)
    {
        if (i > 0)
            std::cout << " ";
        std::cout << data[i];
    }
    if (data.size() > 10)
        std::cout << " ...";
    std::cout << "]\n";
}

// 主测试函数
int main()
{
    // 1. 创建底层 vector 和缓冲区
    std::vector<int> storage;
    dynamic_vector_buffer<int> buf(storage, 100); // 最大100元素

    print_buffer_state(buf, "初始状态");

    // 2. 写入数据到缓冲区
    {
        auto writable = buf.writable(5); // 请求5个元素空间
        std::ranges::iota(writable, 10); // 填充10,11,12,13,14
        buf.commit(5);                   // 提交写入
    }
    print_buffer_state(buf, "写入5个元素后");

    // 3. 同时读写操作
    {
        // 读取数据
        auto readable = buf.readable();
        std::cout << "\n读取数据: ";
        for (int val : readable)
            std::cout << val << " ";
        std::cout << "\n";

        // 同时写入新数据
        auto writable = buf.writable(3);
        std::ranges::fill(writable, 99);
        buf.commit(3);
    }
    print_buffer_state(buf, "同时读写后");

    // 4. 消费部分数据
    buf.consume(3); // 消费前3个元素
    print_buffer_state(buf, "消费3个元素后");

    // 5. 继续写入数据
    {
        auto writable = buf.writable(4);
        std::ranges::iota(writable, 20); // 20,21,22,23
        buf.commit(4);
    }
    print_buffer_state(buf, "再次写入后");

    // 6. 测试内存重用
    std::cout << "\n内存重用演示:\n";
    const auto *prev_data_ptr = storage.data();
    for (int i = 0; i < 5; ++i)
    {
        // 消费部分数据
        buf.consume(2);

        // 写入新数据
        auto writable = buf.writable(2);
        std::ranges::fill(writable, i * 10);
        buf.commit(2);

        std::cout << std::format("操作 {}: 大小={} 容量={} 数据指针={}\n", i, buf.size(),
                                 buf.capacity(),
                                 static_cast<const void *>(storage.data()));

        // 验证数据指针是否变化
        if (i > 0)
            assert(storage.data() == prev_data_ptr);
        prev_data_ptr = storage.data();
    }

    // 7. 测试容量增长
    std::cout << "\n容量增长演示:\n";
    std::vector<int> grow_storage;
    dynamic_vector_buffer<int> grow_buf(grow_storage);

    for (int i = 1; i <= 5; ++i)
    {
        auto writable = grow_buf.writable(20);
        grow_buf.commit(20);

        std::cout << std::format("写入 {}: 大小={} 容量={}\n", i * 20, grow_buf.size(),
                                 grow_buf.capacity());
    }

    // 8. 测试最大大小限制
    try
    {
        std::cout << "\n测试最大大小限制:\n";
        auto writable = buf.writable(100); // 请求100个元素
        std::cout << std::format("实际可写: {} 元素\n", writable.size());

        buf.commit(writable.size() + 1); // 尝试提交过多
    }
    catch (const std::exception &e)
    {
        std::cout << "捕获异常: " << e.what() << "\n";
    }

    return 0;
}
// NOLINTEND