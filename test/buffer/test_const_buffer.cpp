#include <cassert>
#include <cstddef> // For std::byte

// NOLINTBEGIN
#include "../test_head.hpp"

using namespace mcs::net::buffer; // NOLINT

// --------------------- 测试代码 ---------------------
// Test 1: 默认构造函数
void test_default_constructor()
{
    constexpr const_buffer buf;
    REQUIRE_TRUE(buf.data() == nullptr);
    REQUIRE_TRUE(buf.size() == 0);
}

// Test 2: 参数化构造函数
void test_parameterized_constructor()
{
    const char raw_data[5] = {};
    const_buffer buf1(raw_data, sizeof(raw_data));
    REQUIRE_TRUE(buf1.data() == raw_data);
    REQUIRE_TRUE(buf1.size() == sizeof(raw_data));

    const_buffer buf2(nullptr, 0);
    REQUIRE_TRUE(buf2.data() == nullptr);
    REQUIRE_TRUE(buf2.size() == 0);
}

// Test 3: 从 mutable_buffer 构造
void test_construction_from_mutable_buffer()
{
    char raw_data[10];
    mutable_buffer m_buf(raw_data, sizeof(raw_data));
    const_buffer c_buf(m_buf); // 显式构造

    REQUIRE_TRUE(c_buf.data() == raw_data);
    REQUIRE_TRUE(c_buf.size() == sizeof(raw_data));
}

// Test 4: 基础 += 操作
void test_basic_addition()
{
    const char raw_data[10] = {};
    const_buffer buf(raw_data, 10);

    buf += 3;
    REQUIRE_TRUE(static_cast<const char *>(buf.data()) == raw_data + 3);
    REQUIRE_TRUE(buf.size() == 7);

    buf += 5;
    REQUIRE_TRUE(static_cast<const char *>(buf.data()) == raw_data + 8);
    REQUIRE_TRUE(buf.size() == 2);
}

// Test 5: 超过当前 size 的 += 操作
void test_excessive_addition()
{
    const char raw_data[5] = {};
    const_buffer buf(raw_data, 5);

    buf += 10; // 应移动 5 字节
    REQUIRE_TRUE(static_cast<const char *>(buf.data()) == raw_data + 5);
    REQUIRE_TRUE(buf.size() == 0);
}

// Test 6: 多次 += 操作
void test_multiple_additions()
{
    const char raw_data[10] = {};
    const_buffer buf(raw_data, 10);

    buf += 0; // 无变化
    REQUIRE_TRUE(buf.data() == raw_data);
    REQUIRE_TRUE(buf.size() == 10);

    buf += 10; // 移动全部
    REQUIRE_TRUE(buf.data() == raw_data + 10);
    REQUIRE_TRUE(buf.size() == 0);

    buf += 5; // 在 size=0 时无变化
    REQUIRE_TRUE(buf.data() == raw_data + 10);
    REQUIRE_TRUE(buf.size() == 0);
}

// Test 7: 零 size 缓冲区的操作
void test_zero_size_buffer()
{
    const_buffer buf(nullptr, 0);
    buf += 5;
    REQUIRE_TRUE(buf.data() == nullptr);
    REQUIRE_TRUE(buf.size() == 0);

    const char raw_data[5] = {};
    const_buffer buf2(raw_data, 0);
    buf2 += 3;
    REQUIRE_TRUE(buf2.data() == raw_data);
    REQUIRE_TRUE(buf2.size() == 0);
}

// Test 8: 边界条件测试
void test_edge_cases()
{
    // 初始大小为 1
    const char single_byte = 0;
    const_buffer buf(&single_byte, 1);
    buf += 1;
    REQUIRE_TRUE(static_cast<const char *>(buf.data()) == &single_byte + 1);
    REQUIRE_TRUE(buf.size() == 0);

    // 使用 const void* 类型初始化
    const int values[2] = {1, 2};
    const_buffer void_buf(values, sizeof(values));
    REQUIRE_TRUE(void_buf.size() == sizeof(values));
    void_buf += sizeof(int); // 移动一个 int 的大小
    REQUIRE_TRUE(static_cast<const std::byte *>(void_buf.data()) ==
                 static_cast<const std::byte *>(static_cast<const void *>(values)) +
                     sizeof(int));
    REQUIRE_TRUE(void_buf.size() == sizeof(values) - sizeof(int));
}

int main()
{
    test_default_constructor();
    test_parameterized_constructor();
    test_construction_from_mutable_buffer();
    test_basic_addition();
    test_excessive_addition();
    test_multiple_additions();
    test_zero_size_buffer();
    test_edge_cases();

    return 0;
}
// NOLINTEND