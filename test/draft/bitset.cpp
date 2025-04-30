#include <bitset>
#include <cstdint>
#include <iostream>
#include <limits>

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