#include <iostream>
#include <string_view>
#include <utility>
#include <tuple>
#include <type_traits>
#include <optional>

// 编译期字符串包装
template <size_t N>
struct ConstString
{
    char value[N] = {};
    constexpr ConstString(const char (&str)[N])
    {
        for (size_t i = 0; i < N; ++i)
            value[i] = str[i];
    }
    constexpr operator std::string_view() const
    {
        return std::string_view(value, N - 1);
    }
};

// HTTP方法枚举
enum class HttpMethod
{
    GET
};

// 路径规则元信息
template <HttpMethod Method, ConstString Path, auto Func>
struct PathRule
{
    static constexpr HttpMethod method = Method;
    static constexpr std::string_view path = Path;
    static constexpr auto function = Func;

    // 编译期路径匹配
    static constexpr bool match(std::string_view request_path)
    {
        return request_path == path;
    }

    // 调用处理函数
    static auto call()
    {
        return function();
    }
};

// 函数特征萃取
template <typename>
struct FunctionTraits;

template <typename Ret, typename... Args>
struct FunctionTraits<Ret (*)(Args...)>
{
    static constexpr size_t arg_count = sizeof...(Args);
};

// 路由绑定器
template <auto Func, ConstString Path>
struct RouteBinder
{
    static constexpr auto function = Func;
    static constexpr std::string_view path = Path;

    template <HttpMethod Method>
    using RuleType = PathRule<Method, Path, Func>;
};

// 测试处理函数
static std::string hello()
{
    return "Hello, World!";
}

static std::string goodbye()
{
    return "Goodbye!";
}

// 编译期路由绑定
constexpr auto hello_route = RouteBinder<&hello, "/hello">{};
constexpr auto goodbye_route = RouteBinder<&goodbye, "/goodbye">{};

// 路由调用器
template <HttpMethod Method, typename Route>
auto dispatch_route()
{
    using Rule = typename Route::template RuleType<Method>;

    // 验证函数绑定
    static_assert(Rule::function == Route::function, "Function binding mismatch");

    // 验证路径绑定
    static_assert(Rule::path == Route::path, "Path binding mismatch");

    return Rule::call();
}

int main()
{
    // 调用hello路由
    auto result1 = dispatch_route<HttpMethod::GET, decltype(hello_route)>();
    std::cout << result1 << "\n";

    // 调用goodbye路由
    auto result2 = dispatch_route<HttpMethod::GET, decltype(goodbye_route)>();
    std::cout << result2 << "\n";

    // 编译期验证绑定
    static_assert(hello_route.function == &hello, "Hello function binding failed");
    static_assert(hello_route.path == "/hello", "Hello path binding failed");

    static_assert(goodbye_route.function == &goodbye, "Goodbye function binding failed");
    static_assert(goodbye_route.path == "/goodbye", "Goodbye path binding failed");

    // 运行时验证
    if (hello_route.function() != "Hello, World!")
    {
        std::cerr << "Runtime hello function binding failed\n";
        return 1;
    }

    if (goodbye_route.function() != "Goodbye!")
    {
        std::cerr << "Runtime goodbye function binding failed\n";
        return 1;
    }

    std::cout << "All bindings verified successfully!\n";
    return 0;
}