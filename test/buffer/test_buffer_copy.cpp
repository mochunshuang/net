#include <cassert>
#include <cstddef> // For std::byte
#include <vector>
#if 0
// NOLINTBEGIN
#include "../test_head.hpp"

using namespace mcs::net::buffer; // NOLINT

// --------------------- 测试代码 ---------------------

// Test 1: 基础复制（完全匹配）
void test_basic_copy()
{
    char src_data[] = "HelloWorld"; // 10字节（不含'\0'）
    char dest_data[10] = {};

    // 源：单缓冲区
    const_buffer src(src_data, 10);
    // 目标：单缓冲区
    mutable_buffer dest(dest_data, 10);

    const auto copied = mcs::net::buffer::buffer_copy(dest, src);
    assert(copied == 10);
    assert(memcmp(src_data, dest_data, 10) == 0);
}

// Test 2: max_size 限制
void test_max_size_limit()
{
    char src_data[15] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    char dest_data[10] = {};

    std::array<mutable_buffer, 2> dest = {
        mutable_buffer(dest_data, 5),    // 目标缓冲区1：5字节
        mutable_buffer(dest_data + 5, 5) // 目标缓冲区2：5字节
    };

    const_buffer src(src_data, 15);
    const auto copied = mcs::net::buffer::buffer_copy(dest, src, 8); // 限制复制8字节

    assert(copied == 8);
    assert(memcmp(src_data, dest_data, 8) == 0);
    assert(dest_data[8] == 0); // 第9字节未被修改
}

// Test 3: 缓冲区大小不匹配
void test_buffer_size_mismatch()
{
    char src_data[5] = {1, 2, 3, 4, 5};
    char dest_data[3] = {};

    // 源：5字节
    const_buffer src(src_data, 5);
    // 目标：3字节
    mutable_buffer dest(dest_data, 3);

    const auto copied = mcs::net::buffer::buffer_copy(dest, src);
    assert(copied == 3);
    assert(memcmp(src_data, dest_data, 3) == 0);
}

// Test 4: 空序列处理
void test_empty_sequences()
{
    // 空目标
    std::vector<mutable_buffer> empty_dest;
    const_buffer src(nullptr, 5);
    assert(mcs::net::buffer::buffer_copy(empty_dest, src) == 0);

    // 空源
    mutable_buffer dest(nullptr, 5);
    std::vector<const_buffer> empty_src;
    assert(mcs::net::buffer::buffer_copy(dest, empty_src) == 0);
}

// Test 5: 多段缓冲区序列
void test_multi_segment_sequences()
{
    // 源：3段缓冲区 [0-1], [2-4], [5-8]
    char src_data[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::array<const_buffer, 3> src = {const_buffer(src_data, 2),
                                       const_buffer(src_data + 2, 3),
                                       const_buffer(src_data + 5, 4)};

    // 目标：2段缓冲区 [0-3], [4-6]
    char dest_data[7] = {};
    std::array<mutable_buffer, 2> dest = {mutable_buffer(dest_data, 4),
                                          mutable_buffer(dest_data + 4, 3)};

    const auto copied = mcs::net::buffer::buffer_copy(dest, src);
    assert(copied == 7); // 目标总大小7 < 源总大小9
    assert(memcmp(src_data, dest_data, 7) == 0);
}

// Test 6: 更新缓冲区状态
void test_buffer_state_update()
{
    char src_data[5] = {1, 2, 3, 4, 5};
    char dest_data[5] = {};

    // 源：两个缓冲区 [0-1] 和 [2-4]
    std::array<const_buffer, 2> src = {const_buffer(src_data, 2),
                                       const_buffer(src_data + 2, 3)};

    // 目标：两个缓冲区 [0-3] 和 [4]
    std::array<mutable_buffer, 2> dest = {mutable_buffer(dest_data, 4),
                                          mutable_buffer(dest_data + 4, 1)};

    const auto copied = mcs::net::buffer::buffer_copy(dest, src);
    assert(copied == 5);

    // 验证源缓冲区的迭代器状态
    auto src_it = src.begin();
    assert(src_it->size() == 0); // 第一个源缓冲区已耗尽
    ++src_it;
    assert(src_it->size() == 3 - (5 - 4)); // 第二个源缓冲区剩余 3 -1 = 2 字节？

    // 验证目标缓冲区的迭代器状态
    auto dest_it = dest.begin();
    assert(dest_it->size() == 0); // 第一个目标缓冲区已填满
    ++dest_it;
    assert(dest_it->size() == 0); // 第二个目标缓冲区也填满了1字节
}

// Test 7: 零长度缓冲区处理
void test_zero_length_buffers()
{
    char src_data[5] = {1, 2, 3, 4, 5};
    char dest_data[5] = {};

    // 源序列包含一个零长度缓冲区
    std::array<const_buffer, 2> src = {const_buffer(src_data, 0), // 零长度
                                       const_buffer(src_data, 5)};

    // 目标序列包含一个零长度缓冲区
    std::array<mutable_buffer, 2> dest = {mutable_buffer(dest_data, 0), // 零长度
                                          mutable_buffer(dest_data, 5)};

    const auto copied = mcs::net::buffer::buffer_copy(dest, src);
    assert(copied == 5); // 跳过零长度缓冲区
    assert(memcmp(src_data, dest_data, 5) == 0);
}

// Test 8: 编译时计算验证
constexpr bool constexpr_test()
{
    constexpr const char src_data[3] = {1, 2, 3};
    char dest_data[3] = {};

    constexpr std::array<const_buffer, 1> src = {const_buffer(src_data, 3)};
    std::array<mutable_buffer, 1> dest = {mutable_buffer(dest_data, 3)};

    constexpr auto copied = mcs::net::buffer::buffer_copy(dest, src);
    static_assert(copied == 3, "Constexpr copy failed");
    return true;
}
static_assert(constexpr_test(), "Constexpr test failed");

int main()
{
    test_basic_copy();
    test_max_size_limit();
    test_buffer_size_mismatch();
    test_empty_sequences();
    test_multi_segment_sequences();
    test_buffer_state_update();
    test_zero_length_buffers();

    return 0;
}
#else

#include <iostream>

int main()
{
    // TODO
    std::cout << "main done\n";
    return 0;
}
#endif

// NOLINTEND