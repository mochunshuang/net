#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include "./__product_type.hpp"

// NOLINTBEGIN

void construction()
{
    struct A
    {
        draft::__detail::product_type<int, std::string, double> impl;
        explicit A(int i, const char *s, double d) : impl{i, s, d} {}
    };
    A a{42, "test", 3.14};

    A b(42, "test", 3.14); // 允许
}

template <typename... T>
struct product
{
  private:
    draft::__detail::product_type<T...> impl;

  public:
    explicit product(T &&...t) : impl{std::forward<T>(t)...} {}
};
template <typename... T> // NOLINTNEXTLINE
product(T &&...r) -> product<std::remove_cvref_t<T>...>;

void construction2()
{
    /*
 error: cannot bind rvalue reference of type 'char (&&)[5]' to lvalue of type 'const char
[5]' 35 |     product b(42, "test", 3.14);               // 涓                   ^~~~~~
e:\0_github_project\net\net\test\draft\product_type.cpp:28:17: note:   initializing
argument 2 of 'product<T>::product(T&& ...) [with T = {int, char [5], double}]' 28 |
product(T &&...t) : impl{std::forward<T>(t)...} {} |             ~~~~^~~~
*/
    // product b(42, "test", 3.14);               // 不允许字面量字符串
    product b1(42, std::string("test"), 3.14); // 允许
    static_assert(not std::is_class<decltype("test")>::value);
    static_assert(not std::is_class<decltype(1)>::value);
}

void test_construction_and_access()
{
    draft::__detail::product_type<int, std::string, double> pt{42, "test", 3.14};
    assert(std::get<0>(pt) == 42);
    assert(pt.get<1>() == "test");
    assert(std::get<2>(pt) == 3.14);
}

void test_structured_binding()
{
    draft::__detail::product_type<int, std::string> pt{10, "hello"};
    auto &[a, b] = pt;
    assert(a == 10);
    assert(b == "hello");
    a = 20;
    assert(std::get<0>(pt) == 20);
}

void test_comparison()
{
    draft::__detail::product_type<int, char> pt1{1, 'a'};
    draft::__detail::product_type<int, char> pt2{1, 'a'};
    draft::__detail::product_type<int, char> pt3{2, 'b'};
    assert(pt1 == pt2);
    assert(pt1 != pt3);
}

void test_apply()
{
    draft::__detail::product_type<int, float> pt{5, 3.0f};
    auto sum = pt.apply([](int x, float y) { return x + y; });
    assert(sum == 8.0f);
}

void test_move_semantics()
{
    draft::__detail::product_type<std::unique_ptr<int>> pt{std::make_unique<int>(42)};
    auto pt2 = std::move(pt);
    assert(*std::get<0>(pt2) == 42);
    assert(std::get<0>(pt) == nullptr); // NOLINT: moved-from state
}

void test_copy()
{
    draft::__detail::product_type<int, std::string> pt1{5, "test"};
    auto pt2 = pt1;
    assert(pt1 == pt2);
    pt2.get<0>() = 10;
    assert(std::get<0>(pt1) == 5); // Ensure deep copy
}

void test_deduction_guide()
{
    draft::__detail::product_type pt{1, 2.0, std::string{"three"}};
    static_assert(
        std::is_same_v<decltype(pt),
                       draft::__detail::product_type<int, double, std::string>>);
}

void test_const_access()
{
    const draft::__detail::product_type<int, double> pt{1, 2.0};
    assert(pt.get<0>() == 1);
    assert(std::get<1>(pt) == 2.0);
}

void test_apply_move()
{
    draft::__detail::product_type<std::unique_ptr<int>> pt{std::make_unique<int>(42)};
    auto moved = pt.apply([](std::unique_ptr<int> &p) { return std::move(p); });
    assert(*moved == 42);
    assert(pt.get<0>() == nullptr);
}

void test_empty_product_type()
{
    draft::__detail::product_type<> pt;
    assert(pt.size() == 0);
}

int main()
{
    test_construction_and_access();
    test_structured_binding();
    test_comparison();
    test_apply();
    test_move_semantics();
    test_copy();
    test_deduction_guide();
    test_const_access();
    test_apply_move();
    test_empty_product_type();
    return 0;
}
// NOLINTEND