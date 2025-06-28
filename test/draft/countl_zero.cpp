
#include <cassert>
#include <iostream>
#include <cstdint>
#include <bit>

// NOLINTBEGIN

int main()
{
    // NOTE: countl_zero 无法算出具体 0 bit 的个数
    constexpr auto countl_zero = []() {
        static_assert(std::countl_zero(std::uint8_t{0}) == 8);
        static_assert(std::countl_zero(std::uint8_t{0b11111111}) == 0);
        static_assert(std::countl_zero(std::uint8_t{0b11110000}) == 0);
        static_assert(std::countl_zero(std::uint8_t{0b00011110}) == 3);
        return true;
    }();
    static_assert(countl_zero);

    // popcount 能够准确的算出 1 的个数
    constexpr auto popcount = []() {
        static_assert(std::popcount(std::uint8_t{0}) == 0);
        static_assert(std::popcount(std::uint8_t{0b00011101}) == 4);
        static_assert(std::popcount(std::uint8_t{0b11111111}) == 8);
        return true;
    }(); // NOTE: 1的个数可以表示 可用余额 。 那么0 bit 位的就是不可用
    static_assert(popcount);

    // NOTE: 找出 bit 为 1 的位置。
    constexpr auto countl_one = []() {
        static_assert(std::countl_one(std::uint8_t{0}) == 0);

        // NOTE: 如果左看，右看 都为 0. 不代表 没有 1 可用使用
        // NOTE: 因为需要设计一个算法。 快速找出 bit 为 1 的索引。
        static_assert(std::countl_one(std::uint8_t{0b00011101}) == 0);
        static_assert(std::countr_one(std::uint8_t{0b00011100}) == 0);

        return true;
    }();

    // NOTE: 可用反过来找 0 来确定 任意可选的 1
    constexpr auto find_any_one_bit = [](std::uint8_t x) noexcept -> std::uint8_t {
        constexpr auto out_index = sizeof(std::uint8_t) * 8;
        return (x == 0) ? out_index : std::countr_zero(x);
    };
    static_assert(find_any_one_bit(0b00000001) == 0); // 索引0处
    static_assert(find_any_one_bit(0b11111111) == 0);

    static_assert(find_any_one_bit(0b00011100) == 2);
    static_assert(find_any_one_bit(0b01010100) == 2);

    static_assert(find_any_one_bit(0b10000000) == 7);
    static_assert(find_any_one_bit(0b00011101) == 0);
    static_assert(find_any_one_bit(0) == 8);

    constexpr auto set_one = [](std::uint8_t status, std::uint8_t index) {
        status |= (1 << index);
        return status;
    };
    static_assert(set_one(0, 0) == 0b00000001);
    static_assert(set_one(0, 1) == 0b00000010);

    constexpr auto set_zero = [](std::uint8_t status, std::uint8_t index) {
        // return status & ~(1 << index);
        status &= ~(1 << index);
        return status;
    };
    static_assert(set_zero(0b00000001, 0) == 0);
    static_assert(set_zero(0b00000010, 1) == 0);

    // NOTE: 数组地址 和 索引的关系
    {
        struct A
        {
            int v;
        };
        A buffer[4];
        // buffer[1]-buffer[0]; //不允许
        auto dif = &buffer[1] - &buffer[0]; // NOTE: 用于计算 ptr 在数组的索引
        auto diff = (ptrdiff_t)&buffer[1] - (ptrdiff_t)&buffer[0];

        std::cout << "&buffer[1] - &buffer[0]: " << dif << '\n';
        std::cout << "(ptrdiff_t)&buffer[1] - (ptrdiff_t)&buffer[0]: " << diff << '\n';

        assert(dif == 1);
        assert(diff == sizeof(A));

        auto *ptr = &buffer[3];
        assert(ptr - &buffer[0] == 3); // NOTE: 能够反推 索引

        {
            struct A
            {
                A(int vv) : v{vv} {}
                int v;
            };
            // A buffer[4]; //将不能构造
            static_assert(!std::default_initializable<A>);
        }
        static_assert(std::default_initializable<A>);
    }

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND