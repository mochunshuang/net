#include <cassert>
#include <iostream>
#include <unordered_map>

// NOLINTBEGIN

// 测试1: 修改全局静态变量
static int global_value = 0;

consteval void modify_global()
{
    // 以下代码会导致编译错误
    global_value = 42; // 错误：constexpr函数中不允许修改具有静态存储期的变量
}

// 测试2: 修改函数内的静态变量
consteval void modify_static_local()
{
    // 以下代码也会导致编译错误
    static int local_value = 0;
    local_value = 42; // 错误：constexpr函数中不允许修改具有静态存储期的变量
}

// 测试3: 修改类静态成员
struct TestClass
{
    static int class_value;

    consteval static void modify_class_static()
    {
        // 以下代码会导致编译错误
        class_value = 42; // 错误：constexpr函数中不允许修改具有静态存储期的变量
    }
};
int TestClass::class_value = 0;

struct Record
{
    std::unordered_map<int, int> map{};
};
struct A
{
    static constexpr void add(Record &r) noexcept
    {
        r.map.insert({0, 0});
    }
};

constexpr auto get_record()
{
    static Record r{};
    A::add(r);
    return r;
}

int main()
{
    { // modify_global(); // NOTE: 编译期错误
      // modify_static_local(); // NOTE: 编译期错误
    } // TestClass::modify_class_static(); // NOTE: 编译期错误
    std::cout << "测试结果：consteval " << TestClass::class_value << "\n";

    {

        auto r = get_record();     // NOTE: constexpr 不了 map 内部是非 constexpr的
        assert(r.map.size() == 1); // NOTE: 只能运行时了
    }
    {
        auto *a = []() {
            return new A{}; // NOTE: 只有 new / 没用 delete 也是不可以的
        }();
        delete a;
    }
    {
        constexpr auto a = []() consteval {
            auto *a = new A{};
            delete a;
            return 0;
        }();
        static_assert(a == 0);
    }
    // NOTE: c++26 才能 互相类型转化

    {
        struct Record
        {
            std::unordered_map<int, int> map{};

            static auto &get_instance() noexcept
            {
                static Record r{};
                return r;
            }
        };
        struct A
        {
            static auto add_map() noexcept
            {
                Record::get_instance().map.insert({0, 0});
            }
        };
        struct B
        {
            static auto add_map() noexcept
            {
                Record::get_instance().map.insert({1, 1});
            }
        };
        A::add_map();
        B::add_map();
        assert(Record::get_instance().map.size() == 2);
    }
    return 0;
}
// NOLINTEND