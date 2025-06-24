#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// NOLINTBEGIN

struct RouteSegment
{
    enum Type
    {
        STATIC,
        DYNAMIC
    } type;
    std::string value;
};

struct request_meta
{
    std::string method;
    std::string request_target;
    std::string version;

    // 解析路径为段
    std::vector<RouteSegment> parse_path() const
    {
        std::vector<RouteSegment> segments;
        std::istringstream iss(request_target);
        std::string segment;

        while (std::getline(iss, segment, '/'))
        {
            if (segment.empty())
                continue;

            if (segment.front() == '{' && segment.back() == '}')
            {
                segments.push_back({RouteSegment::DYNAMIC, segment});
            }
            else
            {
                segments.push_back({RouteSegment::STATIC, segment});
            }
        }
        return segments;
    }

    // 获取路径签名（用于比较）
    std::string path_signature() const
    {
        auto segs = parse_path();
        std::string sig;
        for (const auto &seg : segs)
        {
            sig += seg.type == RouteSegment::STATIC ? seg.value : "{}";
            sig += "/";
        }
        return sig;
    }

    // 用于调试
    std::string to_string() const
    {
        return method + " " + request_target + " " + version;
    }
};

// 自定义哈希
struct request_meta_hash
{
    size_t operator()(const request_meta &meta) const
    {
        std::hash<std::string> hasher;
        return hasher(meta.method + meta.path_signature() + meta.version);
    }
};

// 自定义相等比较
struct request_meta_equal
{
    bool operator()(const request_meta &lhs, const request_meta &rhs) const
    {
        return lhs.method == rhs.method && lhs.path_signature() == rhs.path_signature() &&
               lhs.version == rhs.version;
    }
};

class RouteMap
{
  private:
    std::unordered_map<request_meta, std::string, request_meta_hash, request_meta_equal>
        map;

    // 检查路径冲突（核心逻辑）
    bool has_conflict(const request_meta &new_route) const
    {
        auto new_segs = new_route.parse_path();

        for (const auto &[existing_route, _] : map)
        {
            auto existing_segs = existing_route.parse_path();

            // 段数不同不会冲突
            if (new_segs.size() != existing_segs.size())
                continue;

            bool conflict = true;
            for (size_t i = 0; i < new_segs.size(); i++)
            {
                const auto &new_seg = new_segs[i];
                const auto &existing_seg = existing_segs[i];

                // 静态段必须完全匹配
                if (new_seg.type == RouteSegment::STATIC &&
                    existing_seg.type == RouteSegment::STATIC &&
                    new_seg.value != existing_seg.value)
                {
                    conflict = false;
                    break;
                }
            }

            if (conflict)
                return true;
        }
        return false;
    }

  public:
    void insert(const request_meta &key, const std::string &value)
    {
        if (has_conflict(key))
        {
            throw std::runtime_error("Route conflict: " + key.to_string());
        }
        map[key] = value;
    }

    void print_routes() const
    {
        std::cout << "\nRegistered Routes:\n";
        std::cout << "----------------------------------------\n";
        for (const auto &[meta, value] : map)
        {
            std::cout << "• " << meta.to_string() << " -> " << value
                      << " (Signature: " << meta.path_signature() << ")\n";
        }
        std::cout << "----------------------------------------\n";
    }
};

int main()
{
    RouteMap route_map;

    // 测试冲突
    try
    {
        request_meta r1{"GET", "/users/{id:int}", "HTTP/1.1"};
        route_map.insert(r1, "Get user by ID");

        request_meta r2{"GET", "/users/{name:string}", "HTTP/1.1"};
        route_map.insert(r2, "Get user by name"); // 应该冲突
    }
    catch (const std::exception &e)
    {
        std::cerr << "Expected error: " << e.what() << "\n";
    }

    // 测试不冲突
    try
    {
        request_meta r3{"GET", "/users/{id}/posts", "HTTP/1.1"};
        route_map.insert(r3, "User posts");

        request_meta r4{"GET", "/users/{name}/comments", "HTTP/1.1"};
        route_map.insert(r4, "User comments"); // 不冲突，因为路径不同
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unexpected error: " << e.what() << "\n";
    }

    // 测试具体路径与动态路径冲突
    try
    {
        request_meta r5{"GET", "/users/123", "HTTP/1.1"};
        route_map.insert(r5, "Concrete user"); // 应该与 r1 冲突
    }
    catch (const std::exception &e)
    {
        std::cerr << "Expected error: " << e.what() << "\n";
    }

    // 测试不同长度路径
    try
    {
        request_meta r6{"GET", "/users/{id}/profile", "HTTP/1.1"};
        route_map.insert(r6, "User profile");

        request_meta r7{"GET", "/users/{name}/profile/settings", "HTTP/1.1"};
        route_map.insert(r7, "User settings"); // 不冲突，长度不同
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unexpected error: " << e.what() << "\n";
    }

    route_map.print_routes();

    return 0;
}
// NOLINTEND