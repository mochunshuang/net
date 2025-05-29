#include <cstdint>
#include <iostream>

// NOLINTBEGIN

#ifdef __cplusplus
extern "C"
{
    struct A
    {
        int a;

        union {
            struct
            {
                int64_t Offset;
                int64_t OffsetHigh;
            } DUMMYSTRUCTNAME;
            void *Pointer;
        } DUMMYUNIONNAME;

        int hEvent;

        void *Pointer_out;
    };
}
#endif
// NOLINTEND

struct B : ::A
{
    constexpr B() noexcept : A{}
    {
        // DUMMYUNIONNAME.Pointer = nullptr;
    }
};

int main()
{
    {
        constexpr auto b = B();
        static_assert(b.a == 0);
        static_assert(b.hEvent == 0);

        static_assert(b.DUMMYUNIONNAME.DUMMYSTRUCTNAME.Offset == 0);

        static_assert(b.DUMMYUNIONNAME.DUMMYSTRUCTNAME.OffsetHigh == 0);

        static_assert(b.DUMMYUNIONNAME.DUMMYSTRUCTNAME.OffsetHigh == 0);

        // NOTE: 无法确定指针，不过也不重要
        //  static_assert(b.DUMMYUNIONNAME.Pointer == nullptr);

        static_assert(b.Pointer_out == nullptr); // 外部的没问题
    }
    {
        union U {
            int a;
            float b;
        };
        constexpr U u{}; // 内存全为 0，但 a 和 b 都不是活跃成员！
        static_assert(u.a == 0);
        // NOTE: 显然 可能 是 默认初始化 第一个成员。
        // 但是不同的编译器可能不一样。总之未定义行为
        //  static_assert(u.b == 0);
    }
    std::cout << "main done\n";
    return 0;
}
