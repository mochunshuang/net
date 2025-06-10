#include <bitset>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>

template <uint8_t... C>
struct any_of
{
    static constexpr auto k_bitset = []() consteval {
        std::bitset<std::numeric_limits<uint8_t>::max()> bs;
        for (uint8_t ch : {C...})
            bs.set(ch);
        return bs;
    }();

    static constexpr auto operator()(uint8_t c) noexcept
    {
        return k_bitset[c];
    }
};

class BufferManager
{
  public:
    static constexpr size_t BUFFER_COUNT = 16;

    BufferManager() : available_(0xFFFF) {} // 初始全可用

    // 获取第一个可用 buffer 索引
    std::optional<size_t> acquire()
    {
        if (available_.none())
            return std::nullopt;
        return std::countr_zero(available_.to_ulong());
    }

    // 分配指定 buffer
    bool allocate(size_t index)
    {
        if (index >= BUFFER_COUNT || !available_.test(index))
            return false;
        available_.reset(index);
        return true;
    }

    // 释放 buffer
    void release(size_t index)
    {
        assert(index < BUFFER_COUNT);
        available_.set(index);
    }

    static_assert(sizeof(unsigned long) == 4);// NOLINT

  private:
    std::bitset<BUFFER_COUNT> available_;
};

int main()
{
    constexpr auto k_sub_delims = []() consteval {
        std::bitset<256> bs;
        for (uint8_t ch : {'!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='})
            bs.set(ch);
        return bs;
    }();

    static_assert(k_sub_delims['!']);
    static_assert(k_sub_delims[';']);
    static_assert(sizeof(k_sub_delims) == 32);     // NOLINT
    static_assert(sizeof(std::bitset<256>) == 32); // NOLINT

    {
        constexpr auto k_test =
            any_of<'!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='>{};
        static_assert(k_test('!'));
        static_assert(k_test(';'));
        static_assert(sizeof(k_test) == 1);            // NOLINT
        static_assert(sizeof(std::bitset<256>) == 32); // NOLINT
    }

    std::cout << "main done\n";
    return 0;
}