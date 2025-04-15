#include <cassert>
#include <variant>
#include <type_traits>

// NOLINTBEGIN

struct A
{
    A(int) {}
};
struct B
{
    B(double) {}
};

class MyClass
{
  public:
    // 构造函数模板：直接接受 A 或 B
    template <typename T>
    explicit MyClass(T &&arg) : var_(std::forward<T>(arg))
    {
        static_assert(std::is_same_v<std::decay_t<T>, A> ||
                          std::is_same_v<std::decay_t<T>, B>,
                      "T must be A or B");
    }

    // 显式构造版本（处理 explicit 构造函数）
    template <typename T, typename... Args>
    explicit MyClass(std::in_place_type_t<T>, Args &&...args)
        : var_(std::in_place_type<T>, std::forward<Args>(args)...)
    {
        static_assert(std::is_same_v<T, A> || std::is_same_v<T, B>, "T must be A or B");
    }

    auto getVariant() const
    {
        return var_;
    }

  private:
    std::variant<std::monostate, A, B> var_;
};

// 验证初始化是否成功
int main()
{
    MyClass obj1(A(42));                     // var_ 存储 A
    MyClass obj2(B(3.14));                   // var_ 存储 B
    MyClass obj3(std::in_place_type<A>, 42); // 显式构造 A

    // 检查 var_ 的实际类型
    assert(std::holds_alternative<A>(obj1.getVariant()));
    assert(std::holds_alternative<B>(obj2.getVariant()));
    return 0;
}
// NOLINTEND