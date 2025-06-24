
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

struct http_meta
{
    struct request_meta
    {
        std::string method;
        std::string request_target;
        std::string version;
    };
    struct response_meta
    {
        std::string version;
        int status_code;
        std::string reason_phrase;
    };
    std::variant<std::monostate, request_meta, response_meta> start_line;
    std::unordered_map<std::string, std::string> headers;
};

struct http_request_raw
{
    http_meta::request_meta meta;
    std::unordered_map<std::string, std::string> headers;
    std::vector<std::uint8_t> raw_data;
};
struct http_response_raw
{
    http_meta::response_meta meta;
    std::unordered_map<std::string, std::string> headers;
    std::vector<std::uint8_t> raw_data;
};

struct http_connect
{
    auto get_left_data() noexcept // NOLINT
    {
        return std::vector<std::uint8_t>{'h', 'e', 'l', 'l', 'o', '!'};
    }

    auto wirte_respons(std::string str) noexcept // NOLINT
    {
        std::cout << "wirte_respons: " << str << '\n';
    }
};

template <bool fix_size_data>
struct http_data_handle;
template <>
struct http_data_handle<true>
{
    constexpr auto operator()(http_connect &conn, http_meta &&meta_info) const noexcept
        -> http_request_raw
    {
        auto raw_data = conn.get_left_data(); // TODO(mcs): 异步处理才是实践
        return http_request_raw{.meta = std::get<1>(std::move(meta_info.start_line)),
                                .headers = std::move(meta_info.headers),
                                .raw_data = std::move(raw_data)};
    }
};
template <>
struct http_data_handle<false>
{
};

struct http_request_path_mapping
{
};
struct http_request_handle
{
};

struct http_request_mapping
{
};

struct http_handle_key
{
    const char *method;
    const char *request_target;
    const char *version;
};

void http_meta_dispatch(http_meta meta) noexcept
{
    //
}

int main()
{
    std::cout << "main done\n";
    return 0;
}