#include <memory>
#include <span>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <cassert>
#include <vector>

using OCTET = std::uint8_t;
using span_param_in = const std::span<const OCTET> &;

// NOLINTBEGIN
class NetworkPacketRaw
{
  public:
    // 从 span 构造，拷贝数据到 unique_ptr 数组
    explicit NetworkPacketRaw(span_param_in data_span)
        : size_(data_span.size()), data_(std::make_unique<OCTET[]>(size_))
    {
        std::copy(data_span.begin(), data_span.end(), data_.get());
    }

    // 移动构造函数
    NetworkPacketRaw(NetworkPacketRaw &&other) noexcept
        : size_(other.size_), data_(std::move(other.data_))
    {
        other.size_ = 0; // 必须显式重置原对象大小
    }

    // 禁用拷贝构造函数
    NetworkPacketRaw(const NetworkPacketRaw &) = delete;
    NetworkPacketRaw &operator=(const NetworkPacketRaw &) = delete;

    // 数据访问接口
    std::span<const OCTET> get_span() const
    {
        return std::span(data_.get(), size_);
    }

    bool is_valid() const
    {
        return data_ != nullptr && size_ > 0;
    }

    size_t size() const
    {
        return size_;
    }

  private:
    size_t size_ = 0;
    std::unique_ptr<OCTET[]> data_;
};

void test_unique_ptr_basic()
{

    std::vector<OCTET> src_data = {0x01, 0x02, 0x03};
    NetworkPacketRaw packet(src_data);

    // 验证数据一致性
    assert(packet.size() == 3);
    assert(memcmp(packet.get_span().data(), src_data.data(), 3) == 0);
    std::cout << "[基础功能] 测试通过\n";
}
void test_unique_ptr_move_semantics()
{
    std::vector<OCTET> src_data = {0x04, 0x05, 0x06};
    NetworkPacketRaw packet1(src_data);

    // 移动构造新对象
    NetworkPacketRaw packet2 = std::move(packet1);

    // 验证新对象
    assert(packet2.size() == 3);
    assert(memcmp(packet2.get_span().data(), src_data.data(), 3) == 0);

    // 验证原对象失效
    assert(packet1.size() == 0); // 关键点：size_ 被重置为 0
    assert(!packet1.is_valid()); // data_ 指针为 nullptr

    std::cout << "[移动语义] 测试通过\n";
}
void test_unique_ptr_copy_semantics()
{
    // 编译时验证拷贝构造函数被删除
    static_assert(!std::is_copy_constructible_v<NetworkPacketRaw>, "应禁用拷贝构造");

    // 运行时示例（实际无法编译）
    // NetworkPacketRaw packet1(...);
    // NetworkPacketRaw packet2 = packet1; // 应触发编译错误

    std::cout << "[拷贝禁用] 测试通过\n";
}
void test_unique_ptr_memory_safety()
{
    // 使用 Valgrind 或 AddressSanitizer 验证
    {
        std::vector<OCTET> large_data(10'000'000, 0xFF);
        NetworkPacketRaw packet(large_data);
        auto moved_packet = std::move(packet);
    } // 此处应自动释放内存

    std::cout << "[内存安全] 测试通过\n";
}
int main()
{
    test_unique_ptr_basic();
    test_unique_ptr_move_semantics();
    test_unique_ptr_copy_semantics();
    test_unique_ptr_memory_safety();
    std::cout << "所有 unique_ptr 测试通过！\n";
    return 0;
}
// NOLINTEND