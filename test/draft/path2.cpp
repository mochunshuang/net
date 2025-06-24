#include <iostream>
#include <string>
#include <functional>
#include <map>

// HTTP方法枚举
enum class HttpMethod
{
    GET,
    POST,
    PUT,
    DELETE
};

// 控制器基类
class Controller
{
  public:
    virtual ~Controller() = default;
};

// 路由信息
struct RouteInfo
{
    std::string full_path;
    HttpMethod method;
    std::function<std::string()> handler;
};

// 路由注册表
class Router
{
    static std::map<std::string, RouteInfo> routes;

  public:
    static void add_route(const std::string &full_path, HttpMethod method,
                          std::function<std::string()> handler)
    {
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
class UserController : public Controller
{
  public:
    static std::string hello()
    {
        return "Hello from UserController!";
    }

    static void register_routes()
    {
        Router::add_route("/users/hello", HttpMethod::GET, &UserController::hello);
    }
};

// 产品控制器
class ProductController : public Controller
{
  public:
    static std::string hello()
    {
        return "Hello from ProductController!";
    }

    static void register_routes()
    {
        Router::add_route("/products/hello", HttpMethod::GET, &ProductController::hello);
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
    std::cout << "GET /products/hello -> "
              << Router::dispatch(HttpMethod::GET, "/products/hello") << "\n";
    std::cout << "GET /invalid -> " << Router::dispatch(HttpMethod::GET, "/invalid")
              << "\n";

    // 3. 验证静态成员函数绑定
    std::cout << "\nStatic member function binding:\n";
    std::cout << "UserController::hello() = " << UserController::hello() << "\n";
    std::cout << "ProductController::hello() = " << ProductController::hello() << "\n";

    return 0;
}