#include <cassert>
#include <cstddef> // For std::byte
#include <cstdint>
#include <limits>

// NOLINTBEGIN

#include "../test_head.hpp"

using namespace mcs::net::buffer; // NOLINT

// Test 1: 默认构造函数
void test_default_constructor()
{
    constexpr mutable_buffer buf;
    REQUIRE_TRUE(buf.data() == nullptr);
    REQUIRE_TRUE(buf.size() == 0);
}

// Test 2: 参数化构造函数
void test_parameterized_constructor()
{

    static_assert(std::numeric_limits<char>::min() == -128);
    static_assert(std::numeric_limits<char>::max() == 127);

    static_assert(std::numeric_limits<unsigned char>::min() == 0);
    static_assert(std::numeric_limits<unsigned char>::max() == 255);
    static_assert(std::numeric_limits<uint8_t>::min() == 0);
    static_assert(std::numeric_limits<uint8_t>::max() == 255);

    char raw_data[10];
    mutable_buffer buf1(raw_data, sizeof(raw_data));
    REQUIRE_TRUE(buf1.data() == raw_data);
    REQUIRE_TRUE(buf1.size() == sizeof(raw_data));

    mutable_buffer buf2(nullptr, 0);
    REQUIRE_TRUE(buf2.data() == nullptr);
    REQUIRE_TRUE(buf2.size() == 0);
}

// Test 3: 基础 += 操作
void test_basic_addition()
{
    char raw_data[10];
    mutable_buffer buf(raw_data, 10);

    buf += 3;
    REQUIRE_TRUE(static_cast<char *>(buf.data()) == raw_data + 3);
    REQUIRE_TRUE(buf.size() == 7);

    buf += 5;
    REQUIRE_TRUE(static_cast<char *>(buf.data()) == raw_data + 8);
    REQUIRE_TRUE(buf.size() == 2);
}

// Test 4: 超过当前 size 的 += 操作
void test_excessive_addition()
{
    char raw_data[5];
    mutable_buffer buf(raw_data, 5);

    buf += 10; // 应移动 5 字节
    REQUIRE_TRUE(static_cast<char *>(buf.data()) == raw_data + 5);
    REQUIRE_TRUE(buf.size() == 0);
}

// Test 5: 多次 += 操作
void test_multiple_additions()
{
    char raw_data[10];
    mutable_buffer buf(raw_data, 10);

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

// Test 6: 零 size 缓冲区的操作
void test_zero_size_buffer()
{
    mutable_buffer buf(nullptr, 0);
    buf += 5;
    REQUIRE_TRUE(buf.data() == nullptr);
    REQUIRE_TRUE(buf.size() == 0);

    char raw_data[5];
    mutable_buffer buf2(raw_data, 0);
    buf2 += 3;
    REQUIRE_TRUE(buf2.data() == raw_data);
    REQUIRE_TRUE(buf2.size() == 0);
}

// Test 7: 边界条件测试
void test_edge_cases()
{
    // 初始大小为 1
    char single_byte;
    mutable_buffer buf(&single_byte, 1);
    buf += 1;
    REQUIRE_TRUE(buf.data() == &single_byte + 1);
    REQUIRE_TRUE(buf.size() == 0);

    // 使用 void* 类型初始化
    int values[2] = {1, 2};
    mutable_buffer void_buf(values, sizeof(values));
    REQUIRE_TRUE(void_buf.size() == sizeof(values));
    void_buf += sizeof(int); // 移动一个 int 的大小

    REQUIRE_TRUE(static_cast<std::byte *>(void_buf.data()) ==
                 static_cast<std::byte *>(static_cast<void *>(values)) + sizeof(int));
    REQUIRE_TRUE(void_buf.size() == sizeof(values) - sizeof(int));
}

int main()
{
    test_default_constructor();
    test_parameterized_constructor();
    test_basic_addition();
    test_excessive_addition();
    test_multiple_additions();
    test_zero_size_buffer();
    test_edge_cases();

    return 0;
}
// NOLINTEND