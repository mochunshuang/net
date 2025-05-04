#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
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
        {
            // NOTE: std::variant 和普通类型一样
            auto v2 = std::move(v);
            assert(v2.index() == 1);
        }
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
    {

        struct Rule
        {
            using T [[maybe_unused]] = B;
            struct __type
            {
                using value_type = std::variant<std::monostate, A, B>;

                value_type value;

                __type() = default;
                explicit __type(value_type &&v) noexcept : value(std::move(v)) {}
                __type &operator=(value_type &&v) noexcept
                {
                    this->value = std::move(v);
                    return *this;
                }
            };
            using result_type = __type;
        };
        std::tuple<Rule::result_type> ret;
        auto get_A = [] {
            return std::variant<std::monostate, A, B>{A{1}};
        };
        auto get_B = [] {
            return std::variant<std::monostate, A, B>{B{1}};
        };
        Rule::result_type v{get_A()};
        {
            Rule::result_type v;
            v = Rule::result_type{get_A()};
        }
        assert(std::get<0>(ret).value.index() == 0);

        std::get<0>(ret) = Rule::result_type{get_A()};
        assert(std::get<0>(ret).value.index() == 1);

        std::get<0>(ret) = Rule::result_type{get_B()};
        assert(std::get<0>(ret).value.index() == 2);

        std::get<0>(ret) = Rule::result_type{std::variant<std::monostate, A, B>{A{1}}};
        assert(std::get<0>(ret).value.index() == 1);

        std::get<0>(ret) = Rule::result_type{B{1}};
        assert(std::get<0>(ret).value.index() == 2);
        {
            Rule::result_type ret;
            assert(ret.value.index() == 0);
            ret = Rule::result_type{B{1}};
            assert(ret.value.index() == 2);
        }
        {
            auto get_A = [](int v) -> std::optional<A> {
                return v != 0 ? std::make_optional(A{1}) : std ::nullopt;
            };
            auto get_B = [](int v) -> std::optional<B> {
                return v != 0 ? std::make_optional(B{1}) : std ::nullopt;
            };
            auto get_ret = [&](int v0, int v1) -> std::optional<Rule::result_type> {
                {
                    Rule::result_type ret;
                    using value_type = Rule::result_type::value_type;
                    if (get_A(v0))
                        ret = Rule::result_type{value_type{get_A(v0).value()}};
                    // NOTE: 没有开启隐式转换
                    //  ret = Rule::result_type::value_type{get_A(v0).value()};
                    //  ret = V{get_A(v0).value()};
                }
                if (auto r = get_A(v0))
                    return Rule::result_type{*r};
                if (auto r = get_B(v1))
                    return Rule::result_type{*r};
                return std::nullopt;
            };
            {
                std::optional<Rule::result_type> ret = get_ret(0, 0);
                assert(not ret);
            }
            {
                std::optional<Rule::result_type> ret = get_ret(1, 0);
                assert(ret);
                assert(ret.value().value.index() == 1);
                assert(std::holds_alternative<A>(ret.value().value));
                {
                    std::optional<Rule::result_type> ret;
                    // NOTE: 需要定义赋值操作
                    ret = Rule::result_type::value_type{*get_A(1)};
                    assert(ret.value().value.index() == 1);
                    assert(std::holds_alternative<A>(ret.value().value));
                }
            }
            {
                std::optional<Rule::result_type> ret = get_ret(0, 1);
                assert(ret);
                assert((*ret).value.index() == 2);
                assert(std::holds_alternative<B>(ret.value().value));

                // 等价 (*ret).value.index() == 2
                assert(ret->value.index() == 2);
            }
        }
    }
    {
        std::optional<A> ret1;
        std::optional<B> ret2;
        bool ok [[maybe_unused]] = ret1 || ret2;
    }
    {
    }
    std::cout << "done\n";
    return 0;
}
// NOLINTEND