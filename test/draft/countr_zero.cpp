
#include <bit>
#include <bitset>
#include <iostream>
#include <climits>
#include <cassert>

// NOLINTBEGIN

#include <bit>
#include <bitset>
#include <optional>
#include <climits>

template <size_t N>
class BufferAllocator
{
    static_assert(N <= 64, "Maximum 64 buffers supported");

  public:
    // 获取第一个可用 buffer 索引（O(1)复杂度）
    std::optional<size_t> acquire() noexcept
    {
        if (available_.none())
            return std::nullopt;

        // 关键优化：直接使用硬件指令
        // const auto val = available_.to_ullong();
        // const size_t pos = std::countr_zero(val);

        return std::countr_zero(available_.to_ullong());
    }

    // 标记 buffer 为已使用
    void allocate(size_t index) noexcept
    {
        available_.reset(index);
    }

    // 标记 buffer 为可用
    void release(size_t index) noexcept
    {
        available_.set(index);
    }

  private:
    std::bitset<N> available_{~0ULL}; // 初始全可用
};

#include <cassert>

// 直接测试 acquire + allocate + release 操作序列
void test_core_operations()
{
    constexpr size_t N = 8;  // 使用小位宽便于观察
    std::bitset<N> bs(0xFF); // 初始全1 (所有buffer可用)

    // bs.count();
    assert(bs.count() == 8); // NOTE: 1 的个数

    std::cout << "===== Core Operations Test =====" << "\n";
    std::cout << "Initial state: " << bs << "\n\n";

    // 1. 第一次 acquire + allocate
    auto pos1 = std::countr_zero(bs.to_ullong());
    std::cout << "Acquire 1: " << pos1 << " (should be 0)\n";
    assert(pos1 == 0);
    bs.reset(pos1); // allocate
    std::cout << "After allocate 0: " << bs << " (should be 11111110)\n\n";
    assert(bs == 0b11111110);

    // 2. 第二次 acquire + allocate
    auto pos2 = std::countr_zero(bs.to_ullong());
    std::cout << "Acquire 2: " << pos2 << " (should be 1)\n";
    assert(pos2 == 1);
    bs.reset(pos2); // allocate
    std::cout << "After allocate 1: " << bs << " (should be 11111100)\n\n";
    assert(bs == 0b11111100);

    // 3. 释放位置0
    bs.set(0); // release
    std::cout << "After release 0: " << bs << " (should be 11111101)\n\n";
    assert(bs == 0b11111101);

    // 4. 第三次 acquire (应获取刚释放的位置0)
    auto pos3 = std::countr_zero(bs.to_ullong());
    std::cout << "Acquire 3: " << pos3 << " (should be 0)\n";
    assert(pos3 == 0);
    bs.reset(pos3); // allocate
    std::cout << "After allocate 0: " << bs << " (should be 11111100)\n\n";
    assert(bs == 0b11111100);

    // 5. 第四次 acquire (应获取位置2)
    auto pos4 = std::countr_zero(bs.to_ullong());
    std::cout << "Acquire 4: " << pos4 << " (should be 2)\n";
    assert(pos4 == 2);

    // 6.  全部 获取
    for (int i = 0; i < 8; i++)
    {
        bs.reset(i);
    }
    assert(bs == 0b00000000);
    std::cout << "when full: " << std::countr_zero(bs.to_ullong()) << " (should be 64)\n";
    assert(bs.none()); // NOTE: 通过 none 来表示是可以的
    for (int i = 1; i < 8; i++)
    {
        bs.set(i);
    }
    assert(bs == 0b11111110); // NOTE: 是从左往右的 顺序
    bs.set(0);

    assert(bs == 0b11111111); // NOTE: 1 全部表示可用
    assert(std::countr_zero(bs.to_ullong()) == 0);

    bs.reset(3);
    assert(bs == 0b11110111);
    // NOTE: countr 是从右开始数
    assert(std::countr_zero(bs.to_ullong()) == 0); // NOTE: 总是从左往右

    {
        std::bitset<N> bs(0b00000000); // NOTE: 全部不可以
        assert(std::countr_zero(bs.to_ullong()) == 64);
        assert(bs.none());

        assert(bs.count() == 0);
    }
    {
        std::bitset<N> bs(0b00000001);
        assert(std::countr_zero(bs.to_ullong()) == 0);

        assert(bs.count() == 1);
        {
            std::bitset<N> bs(0b00000011);
            assert(std::countr_zero(bs.to_ullong()) == 0);

            assert(bs.count() == 2);
        }
        assert(bs.test(0) == true);  // NOTE: 未占用
        assert(bs.test(1) == false); // NOTE: 占用
    }
    {
        std::bitset<N> bs(0b00000010);
        assert(std::countr_zero(bs.to_ullong()) == 1);
        {
            std::bitset<N> bs(0b00010010);
            assert(std::countr_zero(bs.to_ullong()) == 1);
        }
    }
    {
        std::bitset<N> bs(0b00010000);
        assert(std::countr_zero(bs.to_ullong()) == 4);
    }
    {
        std::bitset<N> bs(0b10000000);
        assert(std::countr_zero(bs.to_ullong()) == 7);

        // 修改状态
        bs.set(0); // NOTE: 释放
        assert(std::countr_zero(bs.to_ullong()) == 0);
        assert(bs == 0b10000001);
    }

    std::cout << "===== All assertions passed! =====" << "\n";
}

void test_core_operations2()
{
    std::cout << "\n===== test_core_operations2 start! =====" << "\n";
    std::bitset<8> bs(0);
    assert(bs == 0b00000000);
    std::cout << "when init: " << std::countl_one(bs.to_ullong()) << " (should be 0)\n";
    std::cout << "===== test_core_operations2 end! =====" << "\n";
    {
        // NOTE: bug 的来源
        std::bitset<8> bs(0b10000000);
        std::cout << "when 0b10000000: " << std::countl_one(bs.to_ullong())
                  << " (should be 0)\n";

        assert(std::countl_one(bs.to_ullong()) == 0);
    }
}

int main()
{
    test_core_operations();
    test_core_operations2();
    BufferAllocator<16> alloc;

    // 测试1: 初始状态获取
    assert(alloc.acquire() == 0);

    // 测试2: 分配后状态
    alloc.allocate(0);
    assert(alloc.acquire() == 1);

    // 测试3: 全部分配
    for (size_t i = 0; i < 16; ++i)
        alloc.allocate(i);
    assert(!alloc.acquire().has_value());

    // 测试4: 释放后获取
    alloc.release(7);
    assert(alloc.acquire() == 7);

    // 测试5: 最高位获取
    BufferAllocator<3> small;
    small.allocate(0);
    small.allocate(1);
    assert(small.acquire() == 2);

    return 0;
}

// NOLINTEND