#include <iostream>
#include <string>
#include <map>
#include <concepts>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>
#include <algorithm>

// NOLINTBEGIN

// HTTP方法枚举
enum class HttpMethod
{
    GET,
    POST,
    PUT,
    DELETE
};

// 请求上下文
struct Request
{
    std::string path;
    HttpMethod method;
};

// 响应封装
struct Response
{
    std::string body;
};

// 定义处理函数概念
template <typename F>
concept RequestHandler = std::invocable<F, const Request &> &&
                         std::same_as<std::invoke_result_t<F, const Request &>, Response>;

// 路径冲突检测器
class RouteConflictChecker
{
    static std::set<std::pair<HttpMethod, std::string>> exact_paths;
    static std::set<std::pair<HttpMethod, std::string>> pattern_paths;

  public:
    // 添加路径并检查冲突
    static void add_route(HttpMethod method, const std::string &path)
    {
        // 检查是否参数化路径
        bool is_pattern =
            path.find('{') != std::string::npos && path.find('}') != std::string::npos;

        // 检查精确路径冲突
        if (!is_pattern && exact_paths.find({method, path}) != exact_paths.end())
        {
            throw std::runtime_error("Route conflict: exact path already exists");
        }

        // 检查模式冲突
        if (is_pattern)
        {
            // 提取模式前缀（例如 "/users/"）
            std::string prefix = path.substr(0, path.find('{'));

            // 检查是否有精确路径匹配这个前缀
            for (const auto &[m, p] : exact_paths)
            {
                if (m == method && p.starts_with(prefix) && p.size() > prefix.size())
                {
                    throw std::runtime_error(
                        "Route conflict: pattern path conflicts with exact path");
                }
            }

            // 检查模式路径冲突
            if (pattern_paths.find({method, path}) != pattern_paths.end())
            {
                throw std::runtime_error("Route conflict: pattern path already exists");
            }

            pattern_paths.insert({method, path});
        }
        else
        {
            // 检查是否有模式路径匹配这个精确路径
            for (const auto &[m, p] : pattern_paths)
            {
                std::string pattern_prefix = p.substr(0, p.find('{'));
                if (m == method && path.starts_with(pattern_prefix))
                {
                    throw std::runtime_error(
                        "Route conflict: exact path conflicts with pattern path");
                }
            }

            exact_paths.insert({method, path});
        }
    }
};

// 初始化静态成员
std::set<std::pair<HttpMethod, std::string>> RouteConflictChecker::exact_paths;
std::set<std::pair<HttpMethod, std::string>> RouteConflictChecker::pattern_paths;

// 路由注册表
class Router
{
    using HandlerFunc = Response (*)(const Request &);
    using RouteKey = std::pair<HttpMethod, std::string>;

    static std::map<RouteKey, HandlerFunc> routes;

  public:
    // 添加路由 - 编译期绑定
    template <RequestHandler Handler>
    static void add_route(HttpMethod method, const std::string &path, Handler handler)
    {
        // 检查路径冲突
        RouteConflictChecker::add_route(method, path);

        // 注册路由
        routes[{method, path}] = handler;
    }

    // 路由分发
    static std::optional<Response> dispatch(const Request &req)
    {
        // 1. 尝试精确匹配
        if (auto it = routes.find({req.method, req.path}); it != routes.end())
        {
            return it->second(req);
        }

        // 2. 尝试参数化路径匹配
        for (const auto &[key, handler] : routes)
        {
            const std::string &pattern = key.second;

            // 检查是否是参数化路径
            if (pattern.find('{') != std::string::npos &&
                pattern.find('}') != std::string::npos)
            {

                // 提取模式前缀
                std::string prefix = pattern.substr(0, pattern.find('{'));

                // 检查请求路径是否匹配前缀
                if (req.path.starts_with(prefix) && req.path.size() > prefix.size() &&
                    req.path[prefix.size()] != '/')
                {
                    return handler(req);
                }
            }
        }

        return std::nullopt;
    }
};

// 初始化静态成员
std::map<Router::RouteKey, Router::HandlerFunc> Router::routes;

// 用户控制器
class UserController
{
  public:
    // 简单处理函数
    static Response hello(const Request &)
    {
        return {"Hello from UserController!"};
    }

    // 用户状态处理函数
    static Response user_status(const Request &)
    {
        return {"User status: active"};
    }

    // 带路径参数的处理函数
    static Response get_user(const Request &req)
    {
        // 从路径中提取ID
        size_t last_slash = req.path.find_last_of('/');
        if (last_slash != std::string::npos && last_slash + 1 < req.path.size())
        {
            std::string id = req.path.substr(last_slash + 1);
            return {"User ID: " + id};
        }
        return {"Invalid user path"};
    }

    // 注册路由
    static void register_routes()
    {
        // Router::add_route(HttpMethod::GET, "/users/hello", &hello);
        // Router::add_route(HttpMethod::GET, "/users/status", &user_status);
        Router::add_route(HttpMethod::GET, "/users/{id}", &get_user);
    }
};

// 主函数
int main()
{
    try
    {
        // 注册路由
        UserController::register_routes();

        // 模拟请求1: GET /users/hello
        Request req1{"/users/hello", HttpMethod::GET};
        auto res1 = Router::dispatch(req1);
        std::cout << "GET /users/hello -> " << (res1 ? res1->body : "Not found") << "\n";

        // 模拟请求2: GET /users/status
        Request req2{"/users/status", HttpMethod::GET};
        auto res2 = Router::dispatch(req2);
        std::cout << "GET /users/status -> " << (res2 ? res2->body : "Not found") << "\n";

        // 模拟请求3: GET /users/123
        Request req3{"/users/123", HttpMethod::GET};
        auto res3 = Router::dispatch(req3);
        std::cout << "GET /users/123 -> " << (res3 ? res3->body : "Not found") << "\n";

        // 模拟请求4: GET /users/456
        Request req4{"/users/456", HttpMethod::GET};
        auto res4 = Router::dispatch(req4);
        std::cout << "GET /users/456 -> " << (res4 ? res4->body : "Not found") << "\n";

        // 模拟请求5: 无效路径
        Request req5{"/invalid", HttpMethod::GET};
        auto res5 = Router::dispatch(req5);
        std::cout << "GET /invalid -> " << (res5 ? res5->body : "Not found") << "\n";

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
}
// NOLINTEND