#include <iostream>

struct A
{
    int v;
    static constexpr auto operator()(int ctx) noexcept
    {
        return A{ctx};
    }
};

int main()
{
    constexpr auto k_obj = A::operator()(1);
    static_assert(k_obj.v == 1);

    {
        constexpr auto k_obj = A{}(1);
        static_assert(k_obj.v == 1);
    }

    std::cout << "main done\n";
    return 0;
}