

#include <cassert>
#include <iostream>
#include <string>
#include <type_traits>

// NOLINTBEGIN

// 非 Windows 平台定义兼容类型
#if defined(_LP64) || defined(__LP64__) || defined(__64BIT__) || defined(__x86_64__) || \
    defined(__ppc64__)
typedef uint64_t ULONG_PTR; // 64 位系统
#else
typedef uint32_t ULONG_PTR; // 32 位系统
#endif
using PVOID = void *;
using DWORD = uint32_t;
using HANDLE = void *;

struct OVERLAPPED
{
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union {
        struct
        {
            DWORD Offset;
            DWORD OffsetHigh;
        } DUMMYSTRUCTNAME;
        PVOID Pointer;
    } DUMMYUNIONNAME;

    HANDLE hEvent;
};

// 场景1：继承自OVERLAPPED，无虚函数
struct DerivedNoVirtual : OVERLAPPED
{
    int data;
};

// 场景2：继承自OVERLAPPED，有虚函数
struct DerivedWithVirtual : OVERLAPPED
{
    virtual ~DerivedWithVirtual() {}
    int data{};
};

// 场景3：组合结构体，OVERLAPPED为首成员，无虚函数
struct ComposedNoVirtual
{
    OVERLAPPED ov;
    int data;
};

// 场景4：多层组合嵌套
struct NestedComposed
{
    struct Inner
    {
        OVERLAPPED ov;
        int inner_data;
    } inner;
    int outer_data;
};

// 场景5：组合结构体，有虚函数
struct ComposedWithVirtual
{
    virtual ~ComposedWithVirtual() {}
    OVERLAPPED ov;
    int data;
};

// 场景6：多层继承，最底层无虚函数
struct BaseWithVirtual : OVERLAPPED
{
    int data;
};
struct MultiDerived : BaseWithVirtual
{
    std::string name;
};
struct MultiDerived2 : MultiDerived
{
    double age;

    auto get_age()
    {
        return age;
    }
};

// 验证指针转换是否安全的辅助函数
template <typename T>
bool TestConversion(T *obj, const char *case_name)
{
    // 变成二级指针再转化 一级指针
    ::OVERLAPPED **ppOv = reinterpret_cast<OVERLAPPED **>(&obj);    // NOLINT
    ::OVERLAPPED *expected = reinterpret_cast<OVERLAPPED *>(*ppOv); // NOLINT
    {
        // NOTE: reinterpret_cast 等价于 括号强转
        auto *expected2 = (::OVERLAPPED *)(*ppOv);
        assert(expected == expected2);

        assert(ppOv == (::OVERLAPPED **)(&obj));
    }
    ::OVERLAPPED *actual = nullptr;
    // NOTE:继承 太方便了: static_cast
    if constexpr (std::is_base_of_v<OVERLAPPED, T>)
    {
        actual = static_cast<OVERLAPPED *>(obj);
    }
    else
    {
        if constexpr (requires { &(obj->ov); })
            actual = &(obj->ov);
        else
        {
            actual = &(obj->inner.ov);
        }
    }

    std::cout << "测试案例 [" << case_name << "]: ";
    if (expected == actual)
    {
        std::cout << "通过 (地址匹配)\n";
    }
    else
    {
        std::cout << "失败! 预期地址: " << actual << ", 实际转换地址: " << expected
                  << "\n";
    }
    return expected == actual;
}

int main()
{

    DerivedNoVirtual d1{};
    assert(TestConversion(&d1, "继承无虚函数"));

    DerivedWithVirtual d2;
    assert(not TestConversion(&d2, "继承有虚函数"));

    // 场景3测试
    ComposedNoVirtual c1;
    assert(TestConversion(&c1, "组合无虚函数"));

    NestedComposed n1;
    assert(TestConversion(&n1, "多层组合嵌套,无虚函数"));

    ComposedWithVirtual c2;
    assert(not TestConversion(&c2, "组合有虚函数"));

    MultiDerived m1;
    assert(TestConversion(&m1, "多层继承无虚函数"));

    MultiDerived2 m2;
    assert(TestConversion(&m2, "3层继承无虚函数"));

    // NOTE: 多次 static_cast。 失败，因此 reinterpret_cast 不要和继承一起使用
    // 避免依赖 reinterpret_cast 进行类层次转换
    // NOTE: reinterpret_cast 错误地假设 OVERLAPPED 在 0 偏移处，但 static_cast
    // 会正确计算偏移量。
    {
        // NOTE: reinterpret_cast<OVERLAPPED **>,
        // 要求OVERLAPPED必须是第一个成员，且内存布局是平凡的标准。多继承+虚函数未定义
        auto fun = []() {
            MultiDerived2 m2;
            ::OVERLAPPED **ppOv = reinterpret_cast<OVERLAPPED **>(&m2);     // NOLINT
            ::OVERLAPPED *expected = reinterpret_cast<OVERLAPPED *>(*ppOv); // NOLINT

            auto *v = static_cast<MultiDerived *>(&m2);
            auto *v2 = static_cast<BaseWithVirtual *>(v);
            auto *v3 = static_cast<OVERLAPPED *>(v2);

            return v3 == expected;
        };
        assert(not fun()); // 这里断言失败了为什么？

        // NOTE: 继承关系 通通使用 static_cast
        auto fun2 = []() {
            MultiDerived2 m2;
            ::OVERLAPPED *expected =
                static_cast<::OVERLAPPED *>(&m2); // 正确的单次转换作为基准

            // 正确的多次static_cast路径
            auto *v = static_cast<MultiDerived *>(&m2);
            auto *v2 = static_cast<BaseWithVirtual *>(v);
            auto *v3 = static_cast<::OVERLAPPED *>(
                static_cast<MultiDerived2 *>(v2)); // 关键：需要先转回最派生类

            return v3 == expected;
        };
        assert(fun2());

        auto fun3 = []() {
            MultiDerived2 m2;
            ::OVERLAPPED *expected = static_cast<::OVERLAPPED *>(&m2);

            // 更直接的转换路径
            auto *v = static_cast<MultiDerived2 *>(&m2);
            auto *v3 = static_cast<::OVERLAPPED *>(v);

            return v3 == expected;
        };
        assert(fun3());

        auto fun4 = []() {
            MultiDerived2 m2;
            ::OVERLAPPED *expected = static_cast<::OVERLAPPED *>(&m2);

            // 更直接的转换路径
            auto *v3 = static_cast<::OVERLAPPED *>(&m2);

            return v3 == expected;
        };
        assert(fun3());
    }
    {
        class A
        {
        };
        class B
        {
        };

        A a;
        // B *b = static_cast<B *>(&a); // ❌ 编译错误：A 和 B 无继承关系
        B *b = reinterpret_cast<B *>(&a); // ✅ 编译通过，但行为未定义（UB）
    }

    // NOTE: 总结，不能有虚函数。请优先使用无需函数的继承。
    // NOTE: 避免使用 reinterpret_cast
    /**
     * @brief
     * static_cast 主要用于以下情况：
        继承体系内的向上/向下转换（基类 ↔ 派生类）
        基本类型的显式转换（如 int → double）
        void* 与其他指针类型的互转
        自定义转换运算符（如 operator T()）
     */

    return 0;
}
// NOLINTEND