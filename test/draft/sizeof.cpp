#include <iostream>

struct A
{
    char a;   // 1字节
    int b;    // 4字节（需要4字节对齐）
    double c; // 8字节（需要8字节对齐）
};

struct B // NOTE: 优先降序 大小的 排列方式
{
    double c; // 8字节
    int b;    // 4字节
    char a;   // 1字节
};

struct C
{
    char a;   // 1字节
    double b; // 8字节（需要8字节对齐）
    int c;    // 4字节
};

struct D
{
    double b; // 8字节
    char a;   // 1字节
    int c;    // 4字节
};

int main()
{
    static_assert(sizeof(A) == 16); // NOLINT
    static_assert(sizeof(A) == sizeof(B));

    // NOTE: 成员顺序直接影响填充字节的数量，从而影响类型大小。
    //  NOTE: 子类成员按对齐降序排列：尽量减少子类新增的填充。
    static_assert(sizeof(C) == 24); // NOLINT
    static_assert(sizeof(D) == 16); // NOLINT
    static_assert(sizeof(C) != sizeof(D));
}