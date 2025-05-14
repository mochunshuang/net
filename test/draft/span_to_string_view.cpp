#include <cstdint>
#include <iostream>

#include <span>
#include <string_view>
#include <array>
#include <bit>

// NOLINTBEGIN

using span_param_in = const std::span<const uint8_t> &;

constexpr auto equal_value(span_param_in a, const std::string_view &b) noexcept
{
    const auto k_size = a.size();
    if (k_size != b.size())
        return false;
    for (std::size_t i = 0; i < k_size; ++i)
    {
        if (a[i] != static_cast<uint8_t>(b[i]))
            return false;
    }
    return true;
}

int main()
{
    constexpr auto test = []() constexpr {
        unsigned char buffer[] = {72, 101, 108, 108, 111}; // "Hello"
        auto sp = std::span{buffer};
        std::string_view str = "Hello";
        return equal_value(sp, str);
    };
    static_assert(test());
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND