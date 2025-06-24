#include <iostream>
#include <string>
#include <map>
#include <concepts>
#include <functional>
#include <unordered_map>
#include <sstream>
#include <cctype>
#include <optional>
#include <any> // 使用 any 作为通用响应类型

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
struct RequestContext
{
    std::string path;
    HttpMethod method;
    std::unordered_map<std::string, std::string> path_params;
    std::unordered_map<std::string, std::string> query_params;
    std::string body;
};

// 响应封装
struct Response
{
    int status = 200;
    std::any body;
    std::string content_type = "text/plain";
};

// 定义处理函数概念
template <typename F>
concept RequestHandler =
    std::invocable<F, const RequestContext &> &&
    std::same_as<std::invoke_result_t<F, const RequestContext &>, Response>;

// 路由信息
struct RouteInfo
{
    std::string full_path;
    HttpMethod method;
    Response (*handler)(const RequestContext &);
};

// 路径解析器
class PathParser
{
  public:
    static std::unordered_map<std::string, std::string> parse_path(
        const std::string &pattern, const std::string &actual_path)
    {
        std::unordered_map<std::string, std::string> params;

        std::vector<std::string> pattern_segments = split(pattern, '/');
        std::vector<std::string> path_segments = split(actual_path, '/');

        if (pattern_segments.size() != path_segments.size())
        {
            return {};
        }

        for (size_t i = 0; i < pattern_segments.size(); ++i)
        {
            if (pattern_segments[i].size() > 2 && pattern_segments[i].front() == '{' &&
                pattern_segments[i].back() == '}')
            {
                // 提取参数名
                std::string param_name =
                    pattern_segments[i].substr(1, pattern_segments[i].size() - 2);
                params[param_name] = path_segments[i];
            }
            else if (pattern_segments[i] != path_segments[i])
            {
                return {};
            }
        }

        return params;
    }

  private:
    static std::vector<std::string> split(const std::string &s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
            if (!token.empty())
                tokens.push_back(token);
        }
        return tokens;
    }
};

// 路由注册表
class Router
{
    static std::map<std::string, RouteInfo> routes;

  public:
    // 添加路由 - 完全避免lambda捕获问题
    template <RequestHandler Handler>
    static void add_route(const std::string &full_path, HttpMethod method,
                          Handler handler)
    {
        // 存储原始处理函数指针
        static Handler static_handler = handler;

        routes[full_path] = {full_path, method,
                             [](const RequestContext &ctx) -> Response {
                                 return static_handler(ctx);
                             }};
    }

    static std::optional<Response> dispatch(const RequestContext &ctx)
    {
        // 尝试精确匹配
        if (auto it = routes.find(ctx.path); it != routes.end())
        {
            if (it->second.method == ctx.method)
            {
                return it->second.handler(ctx);
            }
        }

        // 尝试路径参数匹配
        for (const auto &[pattern, route] : routes)
        {
            auto params = PathParser::parse_path(pattern, ctx.path);
            if (!params.empty() && route.method == ctx.method)
            {
                // 创建新的上下文包含路径参数
                RequestContext new_ctx = ctx;
                new_ctx.path_params = params;
                return route.handler(new_ctx);
            }
        }

        return std::nullopt;
    }
};

// 初始化静态成员
std::map<std::string, RouteInfo> Router::routes;

// 自动参数绑定
template <typename T>
T bind_param(const RequestContext &ctx, const std::string &name)
{
    // 尝试从路径参数获取
    if (auto it = ctx.path_params.find(name); it != ctx.path_params.end())
    {
        std::istringstream iss(it->second);
        T value;
        iss >> value;
        return value;
    }

    // 尝试从查询参数获取
    if (auto it = ctx.query_params.find(name); it != ctx.query_params.end())
    {
        std::istringstream iss(it->second);
        T value;
        iss >> value;
        return value;
    }

    // 返回默认值
    return T{};
}

// 用户服务（模拟依赖注入）
class UserService
{
  public:
    static std::string get_user_name(int id)
    {
        return "User_" + std::to_string(id);
    }
};

// 用户控制器
class UserController
{
  public:
    // 简单处理函数
    static Response hello(const RequestContext &)
    {
        return {200, std::string("Hello from UserController!")};
    }

    // 带路径参数的处理函数
    static Response get_user(const RequestContext &ctx)
    {
        int id = bind_param<int>(ctx, "id");
        std::string result =
            "User ID: " + std::to_string(id) + ", Name: User_" + std::to_string(id);
        return {200, result};
    }

    // 带查询参数的处理函数
    static Response search_users(const RequestContext &ctx)
    {
        std::string query = bind_param<std::string>(ctx, "q");
        int page = bind_param<int>(ctx, "page");
        int page_size = bind_param<int>(ctx, "page_size");

        std::ostringstream oss;
        oss << "Search Results:\n"
            << "Query: " << query << "\n"
            << "Page: " << page << "\n"
            << "Page Size: " << page_size << "\n"
            << "Results:\n";

        // 添加一些模拟结果
        for (int i = 0; i < 3; ++i)
        {
            oss << "- ID: " << i << ", Name: " << query << "_user_" << i << "\n";
        }

        return {200, oss.str()};
    }

    // 带请求体的处理函数
    static Response create_user(const RequestContext &ctx)
    {
        std::ostringstream oss;
        oss << "Created user with data: " << ctx.body;
        return {201, oss.str()};
    }

    // 带依赖的处理函数
    static Response get_user_status(const RequestContext &ctx)
    {
        int id = bind_param<int>(ctx, "id");
        std::string name = UserService::get_user_name(id);

        std::ostringstream oss;
        oss << "User Status:\n"
            << "ID: " << id << "\n"
            << "Name: " << name << "\n"
            << "Status: active";

        return {200, oss.str()};
    }

    // 注册路由
    static void register_routes()
    {
        Router::add_route("/users/hello", HttpMethod::GET, &hello);
        Router::add_route("/users/{id}", HttpMethod::GET, &get_user);
        Router::add_route("/users/search", HttpMethod::GET, &search_users);
        Router::add_route("/users", HttpMethod::POST, &create_user);
        Router::add_route("/users/{id}/status", HttpMethod::GET, &get_user_status);
    }
};

// 主函数 - 模拟服务器请求处理
int main()
{
    // 注册路由
    UserController::register_routes();

    // 模拟请求1: GET /users/hello
    RequestContext req1{"/users/hello", HttpMethod::GET, {}, {}, ""};
    auto res1 = Router::dispatch(req1);
    std::cout << "GET /users/hello -> \n"
              << std::any_cast<std::string>(res1->body) << "\n\n";

    // 模拟请求2: GET /users/123
    RequestContext req2{"/users/123", HttpMethod::GET, {}, {}, ""};
    auto res2 = Router::dispatch(req2);
    std::cout << "GET /users/123 -> \n"
              << std::any_cast<std::string>(res2->body) << "\n\n";

    // 模拟请求3: GET /users/search?q=john&page=2&page_size=20
    RequestContext req3{"/users/search",
                        HttpMethod::GET,
                        {},
                        {{"q", "john"}, {"page", "2"}, {"page_size", "20"}},
                        ""};
    auto res3 = Router::dispatch(req3);
    std::cout << "GET /users/search?q=john&page=2&page_size=20 -> \n"
              << std::any_cast<std::string>(res3->body) << "\n\n";

    // 模拟请求4: POST /users
    RequestContext req4{"/users", HttpMethod::POST, {}, {}, "name=Alice&age=30"};
    auto res4 = Router::dispatch(req4);
    std::cout << "POST /users -> \n" << std::any_cast<std::string>(res4->body) << "\n\n";

    // 模拟请求5: GET /users/456/status
    RequestContext req5{"/users/456/status", HttpMethod::GET, {}, {}, ""};
    auto res5 = Router::dispatch(req5);
    std::cout << "GET /users/456/status -> \n"
              << std::any_cast<std::string>(res5->body) << "\n\n";

    // 模拟请求6: 无效路径
    RequestContext req6{"/invalid", HttpMethod::GET, {}, {}, ""};
    auto res6 = Router::dispatch(req6);
    if (!res6)
    {
        std::cout << "GET /invalid -> 404 Not Found\n";
    }

    return 0;
}
// NOLINTEND