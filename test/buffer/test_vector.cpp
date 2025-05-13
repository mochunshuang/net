#include <vector>
#include <span>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <cassert>

using octet = std::uint8_t;
using span_param_in = const std::span<const octet> &;

// NOLINTBEGIN

class NetworkPacket
{
  public:
    // 从 span 构造，直接拷贝数据
    explicit NetworkPacket(span_param_in data_span)
        : data_(data_span.begin(), data_span.end())
    {
    }

    // 数据访问接口
    std::span<const octet> get_span() const
    {
        return data_;
    }
    bool is_empty() const
    {
        return data_.empty();
    }

  private:
    std::vector<octet> data_;
};

// 生成测试数据
std::vector<octet> generate_test_data(size_t size)
{
    std::vector<octet> data(size);
    for (size_t i = 0; i < size; ++i)
    {
        data[i] = static_cast<octet>(i % 256);
    }
    return data;
}

void test_basic_functionality()
{
    std::vector<octet> src_data = {0x01, 0x02, 0x03};
    NetworkPacket packet(src_data);

    // 验证数据一致性
    assert(packet.get_span().size() == 3);
    assert(memcmp(packet.get_span().data(), src_data.data(), 3) == 0);
    std::cout << "基础功能测试通过！\n";
}

void test_copy_semantics()
{
    auto src_data = generate_test_data(1024);
    NetworkPacket packet1(src_data);
    NetworkPacket packet2 = packet1; // 触发拷贝构造函数

    // 验证拷贝后的数据独立
    assert(packet2.get_span().size() == 1024);
    assert(packet1.get_span().data() != packet2.get_span().data());
    assert(memcmp(packet1.get_span().data(), packet2.get_span().data(), 1024) == 0);
    std::cout << "拷贝语义测试通过！\n";
}

void test_move_semantics()
{
    auto src_data = generate_test_data(1024);
    NetworkPacket packet1(src_data);

    // 移动构造新对象
    NetworkPacket packet2 = std::move(packet1);

    // 验证新对象数据正确
    assert(packet2.get_span().size() == 1024);
    assert(std::memcmp(packet2.get_span().data(), src_data.data(), 1024) == 0);

    // 验证原对象状态（根据标准，不强制为空，但实际实现中 vector 移动后为空）
    assert(packet1.is_empty());
    std::cout << "移动语义测试通过！\n";
}

void test_memory_safety()
{
    // 使用 Valgrind 或 AddressSanitizer 运行时验证
    {
        auto large_data = generate_test_data(10'000'000);
        NetworkPacket packet(large_data);
    } // 此处应自动释放内存
    std::cout << "内存安全测试通过！\n";
}
int main()
{
    test_basic_functionality();
    test_copy_semantics();
    test_move_semantics();
    test_memory_safety();
    std::cout << "所有测试通过！\n";
    return 0;
}
// NOLINTEND