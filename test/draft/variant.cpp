#include <cassert>
#include <cstdint>
#include <string>
#include <variant>
#include <type_traits>
#include <vector>

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
    {
        std::variant<int, std::string> v = "abc";
        assert(v.index() == 1);
        v = 0;
        assert(v.index() == 0);
        v = "abc";
        assert(v.index() == 1);
    }

    {
        struct A
        {
            int v;
        };
        struct B
        {
            int v;
        };

        std::variant<std::monostate, A, B> v;
        v = A{};
        assert(v.index() == 1);
        v = B{};

        assert(v.index() == 2);
        auto b [[maybe_unused]] = std::get<2>(v);

        v.emplace<A>(2);
        assert(v.index() == 1);
        v.emplace<B>(B{});
        assert(v.index() == 2);
        {
            // std::variant<std::monostate, A, A> v;
            // v = A{}; // 编译期失败
        }
    }
    return 0;
}
// NOLINTEND