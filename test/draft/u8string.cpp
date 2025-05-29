#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <ranges>
#include <algorithm>
#include <string_view>

#include "../test_common/test_macro.hpp"

int main()
{

    // array<uint8_t> -> u8string
    std::array<std::uint8_t, 5> arr = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"
    std::u8string u8str(reinterpret_cast<const char8_t *>(arr.data()), arr.size());

    std::cout << (std::string &)u8str << '\n';

    // u8string -> array<uint8_t>
    std::array<uint8_t, 5> arr2;
    std::ranges::copy(u8str, arr2.begin());

    std::string str(reinterpret_cast<char *>(arr2.data()), 5);
    std::cout << str << '\n';

    {
        constexpr auto str = "你好！"_span;
        std::u8string u8str(reinterpret_cast<const char8_t *>(str.data()), str.size());
        std::cout << (std::string &)u8str << '\n';
    }

    {
        constexpr std::string_view str = "你好！";
        std::u8string u8str(reinterpret_cast<const char8_t *>(str.data()), str.size());
        std::cout << (std::string &)u8str << '\n';
    }

    // 不是同一类型
    static_assert(not std::is_same_v<char8_t, uint8_t>);
    static_assert(sizeof(char8_t) == sizeof(uint8_t));

    return 0;
}