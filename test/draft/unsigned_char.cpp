#include <iostream>

// NOLINTBEGIN
int test();
struct A
{
    void doGet()
    {
        [[maybe_unused]] auto v = test();
    }
};

int main()
{
    constexpr unsigned char v = 0xFF;
    static_assert(v == 255);
    // NOTE: 警告： unsigned conversion from 'int'
    constexpr unsigned char v2 = v + 1;
    static_assert(v2 != 256); // NOTE 越界是检查不出来的
    constexpr unsigned char v3 = v2;
    static_assert(v3 == 0);
    {
        // NOTE: 无论怎么讲都是 溢出为0
        constexpr unsigned char v2 = 256;
        static_assert(v2 == 0);
    }
    {
        [[maybe_unused]] A a{}; // NOTE: 没毛病 // NOLINT
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND