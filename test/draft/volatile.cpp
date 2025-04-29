#include <cstdint>
#include <iostream>
#include <span>
// NOLINTBEGIN
struct A
{
    int value{0};

    // NOTE: constexpr A 将不再可能
    // volatile int value2{0};

    mutable int value3{0};

  public:
    auto update1(int v) const
    {
        // value = v; //编译器错误
    }
    auto update2(int v) const
    {
        // value2 = v; //编译器错误
    }
    auto update3(int v) const
    {
        value3 = v; // OK
    }
};

constexpr void test(const A &a) noexcept
{
    // a.value = 2; // 不能
    a.value3 = 3; // OK
}

constexpr void test2(A &a) noexcept
{
    // a.value = 2; // 不能
    a.value3 = 3; // OK
}
constexpr void test3(A &a) noexcept
{
    a.value = 3; // OK
}

using octets_view = std::span<const std::uint8_t>;
struct parser_ctx
{
    octets_view root_span;   // NOLINT
    std::size_t cur_index{}; // NOLINT
    std::size_t end_index{}; // NOLINT

    [[nodiscard]] constexpr auto size() const noexcept
    {
        return end_index - cur_index;
    }
    [[nodiscard]] constexpr auto empty() const noexcept
    {
        return end_index == cur_index;
    }
    [[nodiscard]] constexpr auto valid() const noexcept
    {
        return cur_index < end_index;
    }
};
static_assert(sizeof(octets_view) == 16);
static_assert(sizeof(parser_ctx) == 32);

/*
                    Start
                      ↓
             对象大小 ≤ 24 字节？ → Yes → 值语义
                      ↓ No
             需要多态或共享状态？ → Yes → 引用语义
                      ↓ No
             高频修改且性能敏感？ → Yes → 引用语义
                      ↓ No
             需要明确所有权？ → Yes → 值语义
                      ↓ No
                    引用语义
*/

int main()
{
    {
        constexpr A a;
        test(a);
        //  NOTE: mutable 对编译期计算不友好
        // static_assert(a.value3 == 3);
    }
    {
        //  NOTE: mutable 对编译期计算不友好
        constexpr A a;
        // test2(a); // Candidate function not viable: 1st argument ('const A') would lose
        // const qualifier

        // static_assert(a.value3 == 3);
    }
    {
        constexpr A a; //  NOTE: 引用 对编译期计算不友好
        // test3(a); // Candidate function not viable: 1st argument ('const A') would lose
        // const qualifier
    }
    {
        struct Base
        {
            using type = int;
        };

        struct A : Base
        {
        };
        using name = A::type; // OK
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND