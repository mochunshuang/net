#include <array>
#include <cassert>
#include <cstddef> // For std::byte
#include <list>
#include <vector>

// NOLINTBEGIN
#include "../test_head.hpp"

using namespace mcs::net::buffer; // NOLINT

// --------------------- 测试代码 ---------------------

// Test 1: 空序列
void test_empty_sequence()
{

    std::vector<mutable_buffer> empty_vec;
    REQUIRE_TRUE(buffer_size(empty_vec) == 0);

    const_buffer empty_bufs[0]{};
    // NOTE: 不支持 C 数组 没有迭代器
    // REQUIRE_TRUE(buffer_size(empty_bufs) == 0);
}

// Test 2: 单元素序列
void test_single_element()
{
    char data[5];
    mutable_buffer buf(data, 5);
    REQUIRE_TRUE(buffer_size(buf) == 5); // 直接传入 mutable_buffer

    const_buffer cbuf(data, 3);
    std::array<const_buffer, 1> arr{cbuf};
    REQUIRE_TRUE(buffer_size(arr) == 3);
}

// Test 3: 多元素序列
void test_multiple_elements()
{
    mutable_buffer bufs[3] = {mutable_buffer(nullptr, 2), mutable_buffer(nullptr, 5),
                              mutable_buffer(nullptr, 3)};
    // NOTE: 不支持 C 数组 没有迭代器
    // REQUIRE_TRUE(buffer_size(bufs) == 10); // 2 + 5 + 3 = 10
    std::array<mutable_buffer, 3> arr = {mutable_buffer(nullptr, 2),
                                         mutable_buffer(nullptr, 5),
                                         mutable_buffer(nullptr, 3)};
    REQUIRE_TRUE(buffer_size(arr) == 10); // 2 + 5 + 3 = 10
}

// Test 4: 混合类型序列（mutable_buffer 和 const_buffer）
void test_mixed_buffer_types()
{
    char data1[4], data2[6];
    mutable_buffer mbuf(data1, 4);
    const_buffer cbuf(data2, 6);

    std::vector<const_buffer> mixed = {const_buffer{mbuf}, cbuf};
    REQUIRE_TRUE(buffer_size(mixed) == 10); // 4 + 6 = 10
}

// Test 5: 包含零大小缓冲区
void test_zero_size_buffers()
{
    const_buffer bufs[] = {const_buffer(nullptr, 0), const_buffer(nullptr, 3),
                           const_buffer(nullptr, 0)};
    // NOTE: 不支持 C 数组 没有迭代器
    // REQUIRE_TRUE(buffer_size(bufs) == 3); // 0 + 3 + 0 = 3

    std::array<const_buffer, 3> arr = {const_buffer(nullptr, 0), const_buffer(nullptr, 3),
                                       const_buffer(nullptr, 0)};
    REQUIRE_TRUE(buffer_size(arr) == 3);
}

// Test 6: 嵌套容器（如 vector<vector<const_buffer>>）
void test_nested_containers()
{
    std::vector<std::vector<const_buffer>> nested = {
        {const_buffer(nullptr, 2), const_buffer(nullptr, 3)}, // 5
        {const_buffer(nullptr, 5)},                           // 5
        {}                                                    // 0
    };
    // NOTE: 不支持 嵌套: 因为  const_buffer b(*i); 不能从 std::vector<const_buffer> 构造
    // REQUIRE_TRUE(buffer_size(nested) == 10); // 5 + 5 + 0 = 10
}

// Test 7: 非连续容器（如 std::list）
void test_non_contiguous_container()
{
    std::list<const_buffer> buffers;
    buffers.push_back(const_buffer(nullptr, 4));
    buffers.push_back(const_buffer(nullptr, 6));
    REQUIRE_TRUE(buffer_size(buffers) == 10); // 4 + 6 = 10
}

// Test 8: 编译时计算（constexpr 验证）
constexpr std::size_t constexpr_test()
{
    std::array<const_buffer, 2> bufs = {const_buffer(nullptr, 2),
                                        const_buffer(nullptr, 3)};
    return buffer_size(bufs);
}
static_assert(constexpr_test() == 5, "Constexpr test failed");

int main()
{
    test_empty_sequence();
    test_single_element();
    test_multiple_elements();
    test_mixed_buffer_types();
    test_zero_size_buffers();
    test_nested_containers();
    test_non_contiguous_container();

    return 0;
}
// NOLINTEND