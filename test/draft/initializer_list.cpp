#include <array>
#include <cassert>
#include <iostream>

#include <vector>

static void f(std::initializer_list<double> il) {}; // NOLINT

struct Point
{
    int x;
    int y;
};

void fun(std::initializer_list<Point> il) {}; // NOLINT

void fun2(int count, std::initializer_list<Point> il) {}; // NOLINT

void q(std::initializer_list<Point>){};

void r()
{
    q({Point{.x = 1}, Point{.x = 2}, Point{.x = 3}});
}

struct Foo
{
    std::vector<int> mem = {1, 2, 3}; // 非静态成员的列表初始化
    std::vector<int> mem2;
    Foo() : mem2{-1, -2, -3} {} // 构造函数中的成员列表初始化
};

struct Foo2
{
    std::array<int, 3> mem2;
    Foo2() : mem2{-1, -2, -3} {} // 构造函数中的成员列表初始化
};

struct Foo3
{
    std::array<Point, 3> mem2;
    Foo3() : mem2{-1, -2, -3, -3}
    {
        assert(mem2[0].x == -1);
        assert(mem2[0].y == -2);
    } // 构造函数中的成员列表初始化
};

struct Foo4
{
    int id{0};
    std::array<Point, 3> mem2;
};

struct Foo5
{
    int id{0};
    std::array<Point, 3> mem2;
};

int main()
{
    {
        f({1, 2, 3});
    }
    {
        // fun({1, 2});
        fun({{.x = 1, .y = 2}});
    }
    {

        fun2(2, {{.x = 1, .y = 2}, {.x = 2, .y = 3}});
    }
    {
        Foo3 f3{};
    }
    {
        Foo4 f4{.id = 2, .mem2 = {{1, 2, 2, 3}}};
    }

    {
        // TODO(mcs): 如何让下面的语法成立？目前是做不到的
        // Foo5 f5{2, {1, 2}, {2, 3}};
        // Foo5 f5{2, {{1, 2}, {2, 3}, {3, 4}}}; //一样做不到
    }
    std::cout << "main done\n";
    return 0;
}
