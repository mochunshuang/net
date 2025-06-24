#include <algorithm>
#include <cassert>
#include <memory>
#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

// 保持之前的 wildcard, path_mata, http_path 实现不变
struct wildcard
{
    constexpr explicit wildcard(std::string_view input) noexcept
    {
        // 步骤1: 编译期修剪输入字符串
        constexpr auto trim = [](std::string_view s) constexpr noexcept {
            // 移除前导空白
            while (!s.empty() && is_whitespace(s.front()))
            {
                s.remove_prefix(1);
            }
            // 移除尾部空白
            while (!s.empty() && is_whitespace(s.back()))
            {
                s.remove_suffix(1);
            }
            return s;
        };

        std::string_view trimmed = trim(input);

        // 步骤2: 查找冒号位置
        size_t colon_pos = 0;
        while (colon_pos < trimmed.size() && trimmed[colon_pos] != ':')
        {
            ++colon_pos;
        }

        // 步骤3: 分割并修剪name和type
        if (colon_pos < trimmed.size())
        {
            name_ = trim(trimmed.substr(0, colon_pos));
            type_ = trim(trimmed.substr(colon_pos + 1));
        }

        assert(name_ != "");
        assert(type_ != "");
    }

    constexpr const auto &name() const noexcept
    {
        return name_;
    }
    constexpr const auto &type() const noexcept
    {
        return type_;
    }

  private:
    // 自定义constexpr的字符判断函数
    static constexpr bool is_whitespace(char c) noexcept
    {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
    }
    std::string_view name_;
    std::string_view type_;
};

struct path_mata
{
    constexpr explicit path_mata(std::string_view s) noexcept
    {
        assert(s.size() != 0);
        auto const k_size = s.size();
        if (k_size > 2 && s[0] == '{' && s[k_size - 1] == '}')
        {
            value_.emplace<wildcard>(s.substr(1, s.size() - 2));
        }
        else
            value_.emplace<std::string_view>(s);
    }

    [[nodiscard]] constexpr const auto &value() const noexcept
    {
        return value_;
    }

    [[nodiscard]] constexpr bool normal_path() const noexcept
    {
        return value_.index() == 0;
    }

    constexpr const auto &name() const noexcept
    {
        assert(not normal_path());
        return std::get<1>(value_).name();
    }
    constexpr const auto &type() const noexcept
    {
        assert(not normal_path());
        return std::get<1>(value_).type();
    }

    constexpr bool operator==(const std::string_view &s) const noexcept
    {
        if (not normal_path())
            return true;
        return s == std::get<0>(value_);
    }
    constexpr bool operator==(const std::string &s) const noexcept
    {
        return operator==(std::string_view(s));
    }
    friend constexpr bool operator==(const path_mata &a, const path_mata &b) noexcept
    {
        return a.normal_path() && b.normal_path()
                   ? std::get<0>(a.value_) == std::get<0>(b.value_)
                   : true;
    }

  private:
    std::variant<std::string_view, wildcard> value_;
};

struct http_path
{
    template <size_t N>
    constexpr http_path(const char (&str)[N]) noexcept : http_path(std::string_view(str))
    {
    }

    constexpr explicit http_path(std::string_view s) noexcept
    {
        // NOTE: '/' 需要特殊处理
        //  /a/b/c => a,b,c
        constexpr auto k_split_char = '/';
        assert(s.size() > 0 && s[0] == k_split_char);
        auto const k_size = s.size();
        if (k_size == 1)
        {
            path_.reserve(1);
            path_.emplace_back(std::string_view{"/"});
        }
        else
        {
            size_t start = 0;
            std::vector<path_mata> tmp;
            while (true)
            {
                auto end = s.find(k_split_char, start + 1);
                if (end < k_size && end - start > 1)
                {
                    tmp.emplace_back(s.substr(start + 1, end - start - 1));
                    start = end;
                    continue;
                }
                if (start + 1 < k_size)
                    tmp.emplace_back(s.substr(start + 1, k_size - 1));
                break;
            }
            path_ = std::move(tmp);
            path_.shrink_to_fit();
        }
    }

    constexpr const auto &path() const noexcept
    {
        return path_;
    }

    friend constexpr bool operator==(const http_path &a, const http_path &b) noexcept
    {
        if (auto const k_size = a.path_.size(); k_size == b.path_.size())
        {
            for (size_t i{0}; i < k_size; ++i)
            {
                if (a.path_[i] != b.path_[i])
                    return false;
            }
            return true;
        }
        return false;
    }

  private:
    std::vector<path_mata> path_;
};

struct request_meta
{
    std::string_view method;
    http_path request_target;
    std::string_view version;
    constexpr bool operator==(const request_meta &) const = default; // since C++20
    struct request_meta_hash
    {
        std::size_t operator()(const request_meta &s) const noexcept
        {
            // 计算method的哈希
            std::size_t h1 = std::hash<std::string_view>{}(s.method);

            // 计算http_path的哈希（需要为path_mata和http_path实现哈希）
            std::size_t h2 = 0;
            for (const auto &segment : s.request_target.path())
            {
                // 关键修改：统一通配符和具体段的哈希计算
                std::size_t seg_hash;
                if (segment.normal_path())
                    seg_hash =
                        std::hash<std::string_view>{}(std::get<0>(segment.value()));
                else
                    seg_hash = std::hash<std::string_view>{}("*");

                h2 ^= seg_hash;
                // 旋转哈希位
                h2 = (h2 << 1) | (h2 >> (sizeof(std::size_t) * 8 - 1));
            }

            // 计算version的哈希
            std::size_t h3 = std::hash<std::string_view>{}(s.version);

            // 合并哈希（可以使用更复杂的组合方式）
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}; // 参数转换函数
template <typename T>
T convert_param(std::string_view sv);

template <>
int convert_param<int>(std::string_view sv)
{
    return std::stoi(std::string(sv));
}

template <>
long convert_param<long>(std::string_view sv)
{
    return std::stol(std::string(sv));
}

template <>
double convert_param<double>(std::string_view sv)
{
    return std::stod(std::string(sv));
}

template <>
std::string convert_param<std::string>(std::string_view sv)
{
    return std::string(sv);
}

template <>
std::string_view convert_param<std::string_view>(std::string_view sv)
{
    return sv;
}

// 路由处理器基类
class RouteHandlerBase
{
  public:
    virtual ~RouteHandlerBase() = default;
    virtual void invoke(const std::vector<std::string_view> &params) const = 0;
    virtual int get_handler_id() const = 0;
};

// 具体路由处理器
template <typename Func, typename... ParamTypes>
class RouteHandler : public RouteHandlerBase
{
  public:
    explicit RouteHandler(Func func, int handler_id)
        : func_(std::move(func)), handler_id_(handler_id)
    {
    }

    void invoke(const std::vector<std::string_view> &params) const override
    {
        invoke_impl(params, std::make_index_sequence<sizeof...(ParamTypes)>{});
    }

    int get_handler_id() const override
    {
        return handler_id_;
    }

  private:
    template <size_t... Is>
    void invoke_impl(const std::vector<std::string_view> &params,
                     std::index_sequence<Is...>) const
    {
        if (params.size() != sizeof...(ParamTypes))
        {
            throw std::runtime_error("Parameter count mismatch");
        }
        func_(convert_param<ParamTypes>(params[Is])...);
    }

    std::function<Func> func_;
    int handler_id_;
};

// 路由表项
struct RouteEntry
{
    http_path pattern;
    std::unique_ptr<RouteHandlerBase> handler;
    std::vector<std::string_view> param_names;
};

// 简洁路由器
class SimpleRouter
{
    using GroupKey = std::tuple<std::string_view, size_t, std::string_view>;

    struct GroupKeyHash
    {
        size_t operator()(const GroupKey &key) const noexcept
        {
            size_t h1 = std::hash<std::string_view>{}(std::get<0>(key));
            size_t h2 = std::hash<size_t>{}(std::get<1>(key));
            size_t h3 = std::hash<std::string_view>{}(std::get<2>(key));
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    std::unordered_map<GroupKey, std::vector<RouteEntry>, GroupKeyHash> routes_;
    int next_handler_id_ = 1;

  public:
    // 添加路由
    template <typename Func, typename... ParamTypes>
    int add_route(const request_meta &meta, Func &&func)
    {
        GroupKey key{meta.method, meta.request_target.path().size(), meta.version};

        // 提取参数名称
        std::vector<std::string_view> param_names;
        for (const auto &segment : meta.request_target.path())
        {
            if (!segment.normal_path())
            {
                param_names.push_back(segment.name());
            }
        }

        // 创建处理器
        int handler_id = next_handler_id_++;
        auto handler = std::make_unique<RouteHandler<Func, ParamTypes...>>(
            std::forward<Func>(func), handler_id);

        // 添加到路由表
        routes_[key].push_back(
            {meta.request_target, std::move(handler), std::move(param_names)});

        return handler_id;
    }

    // 检查路由是否存在
    bool contains(const request_meta &request) const
    {
        return find_handler(request) != nullptr;
    }

    // 获取匹配的路由信息
    struct MatchResult
    {
        int handler_id;
        std::vector<std::pair<std::string_view, std::string_view>> params;
    };

    std::optional<MatchResult> match(const request_meta &request) const
    {
        if (auto entry = find_handler(request))
        {
            MatchResult result;
            result.handler_id = entry->handler->get_handler_id();

            // 提取参数
            const auto &pattern_segments = entry->pattern.path();
            const auto &request_segments = request.request_target.path();

            for (size_t i = 0; i < pattern_segments.size(); ++i)
            {
                if (!pattern_segments[i].normal_path())
                {
                    std::string_view param_name =
                        entry->param_names.empty() ? "" : entry->param_names[i];
                    std::string_view param_value =
                        std::get<0>(request_segments[i].value());
                    result.params.emplace_back(param_name, param_value);
                }
            }

            return result;
        }
        return std::nullopt;
    }

  private:
    // 查找匹配的路由
    const RouteEntry *find_handler(const request_meta &request) const
    {
        GroupKey key{request.method, request.request_target.path().size(),
                     request.version};

        auto group_it = routes_.find(key);
        if (group_it == routes_.end())
            return nullptr;

        // 在分组内查找匹配的路由
        for (const auto &entry : group_it->second)
        {
            if (entry.pattern == request.request_target)
            {
                return &entry;
            }
        }

        return nullptr;
    }
};

// 示例处理函数
void user_handler(int id, const std::string &action)
{
    std::cout << "User ID: " << id << ", Action: " << action << "\n";
}

void product_handler(const std::string &category, int page)
{
    std::cout << "Category: " << category << ", Page: " << page << "\n";
}

int main()
{
    SimpleRouter router;

    // 添加路由
    int user_handler_id = router.add_route<decltype(user_handler), int, std::string>(
        {"GET", "/user/{id:int}/{action:string}", "HTTP/1.1"}, user_handler);

    int product_handler_id =
        router.add_route<decltype(product_handler), std::string, int>(
            {"GET", "/products/{category:string}/page/{page:int}", "HTTP/1.1"},
            product_handler);

    // 检查路由是否存在
    request_meta user_request{"GET", "/user/123/profile", "HTTP/1.1"};
    assert(router.contains(user_request));

    // 匹配路由并获取参数
    if (auto match = router.match(user_request))
    {
        std::cout << "Matched handler ID: " << match->handler_id << "\n";
        std::cout << "Parameters:\n";
        for (const auto &[name, value] : match->params)
        {
            std::cout << "  " << name << " = " << value << "\n";
        }

        // 根据 handler_id 调用相应的处理函数
        if (match->handler_id == user_handler_id)
        {
            // 实际应用中，这里会根据参数类型调用处理函数
            int id = convert_param<int>(match->params[0].second);
            std::string action = convert_param<std::string>(match->params[1].second);
            user_handler(id, action);
        }
    }

    // 处理产品请求
    request_meta product_request{"GET", "/products/books/page/5", "HTTP/1.1"};
    if (auto match = router.match(product_request))
    {
        std::cout << "Matched handler ID: " << match->handler_id << "\n";

        if (match->handler_id == product_handler_id)
        {
            std::string category = convert_param<std::string>(match->params[0].second);
            int page = convert_param<int>(match->params[1].second);
            product_handler(category, page);
        }
    }

    return 0;
}