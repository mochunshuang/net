#include <iostream>
#include <string>
#include <map>
#include <concepts>

// HTTP方法枚举
enum class HttpMethod
{
    GET,
    POST,
    PUT,
    DELETE
};

// 定义处理函数概念
template <typename F>
concept RequestHandler =
    std::invocable<F> && std::same_as<std::invoke_result_t<F>, std::string>;

// 路由信息（使用函数指针）
struct RouteInfo
{
    std::string full_path;
    HttpMethod method;
    std::string (*handler)(); // 编译期绑定的函数指针
};

// 路由注册表
class Router
{
    static std::map<std::string, RouteInfo> routes;

  public:
    // 添加路由（编译期绑定函数指针）
    template <RequestHandler Handler>
    static void add_route(const std::string &full_path, HttpMethod method,
                          Handler handler)
    {
        // 直接存储函数指针（无需额外包装）
        routes[full_path] = {full_path, method, handler};
    }

    static std::string dispatch(HttpMethod method, const std::string &path)
    {
        for (const auto &[key, route] : routes)
        {
            if (route.method == method && route.full_path == path)
            {
                return route.handler();
            }
        }
        return "Not found";
    }
};

// 初始化静态成员
std::map<std::string, RouteInfo> Router::routes;

// 用户控制器
class UserController
{
  public:
    static std::string hello()
    {
        return "Hello from UserController!";
    }

    static std::string status()
    {
        return "Status: OK";
    }

    static void register_routes()
    {
        // 直接绑定静态成员函数指针
        Router::add_route("/users/hello", HttpMethod::GET, &hello);
        Router::add_route("/users/status", HttpMethod::GET, &status);
    }
};

// 产品控制器
class ProductController
{
  public:
    static std::string hello()
    {
        return "Hello from ProductController!";
    }

    static std::string get_product(int id)
    {
        return "Product ID: " + std::to_string(id);
    }

    static void register_routes()
    {
        // 绑定静态成员函数
        Router::add_route("/products/hello", HttpMethod::GET, &hello);

        // 使用静态lambda（编译期绑定）
        static auto get_product_wrapper = []() -> std::string {
            return get_product(123);
        };

        Router::add_route("/products/details", HttpMethod::GET, get_product_wrapper);
    }
};

int main()
{
    // 1. 注册路由
    UserController::register_routes();
    ProductController::register_routes();

    // 2. 模拟路由分发
    std::cout << "Routing tests:\n";
    std::cout << "GET /users/hello -> "
              << Router::dispatch(HttpMethod::GET, "/users/hello") << "\n";
    std::cout << "GET /users/status -> "
              << Router::dispatch(HttpMethod::GET, "/users/status") << "\n";
    std::cout << "GET /products/hello -> "
              << Router::dispatch(HttpMethod::GET, "/products/hello") << "\n";
    std::cout << "GET /products/details -> "
              << Router::dispatch(HttpMethod::GET, "/products/details") << "\n";
    std::cout << "GET /invalid -> " << Router::dispatch(HttpMethod::GET, "/invalid")
              << "\n";

    // 3. 验证静态成员函数绑定
    std::cout << "\nStatic member function binding:\n";
    std::cout << "UserController::hello() = " << UserController::hello() << "\n";
    std::cout << "ProductController::hello() = " << ProductController::hello() << "\n";

    return 0;
}