

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

#include <cassert>

// NOLINTBEGIN

int main()
{

    using namespace mcs::ABNF;
    using namespace mcs::ABNF::URI; // NOLINT

    //================= authority 有效用例 =================
    // 完整结构：userinfo + IPv6 + port
    constexpr auto full_authority_1 =
        "user%3Apass@[v9.fe]:8080"_span; // 混合编码和IPvFuture
    EXPECT(authority(full_authority_1));

    // 仅有 userinfo 和 reg-name
    constexpr auto user_host_case = "~john:test@example%2Ecom"_span; // 带编码点的域名
    static_assert(authority(user_host_case));

    // 仅有 IPv4 和 port
    constexpr auto ipv4_port = "192.168.0.1:65535"_span; // 最大端口号
    static_assert(authority(ipv4_port));

    // 纯 reg-name 带特殊字符
    constexpr auto reg_name_special =
        "server_!$&.sub-domain"_span; // 包含sub-delims的注册名
    static_assert(authority(reg_name_special));

    // 边界用例：空port（仅冒号）
    constexpr auto empty_port = "[::1]:"_span; // 允许port为空字符串
    static_assert(authority(empty_port));

    //================= authority 无效用例 =================
    // 结构错误：多重@符号
    constexpr auto multi_at = "user@name@[::1]"_span;
    static_assert(!authority(multi_at));

    // IPv6未加方括号
    constexpr auto unbraced_ipv6 = "2001:db8::1"_span;
    static_assert(!authority(unbraced_ipv6));

    // 非法port字符
    constexpr auto non_digit_port = "localhost:80a"_span; // 字母结尾
    static_assert(!authority(non_digit_port));

    // userinfo包含非法字符（空格）
    constexpr auto space_in_user = "user name@host"_span;
    static_assert(!authority(space_in_user));

    // reg-name包含保留字符（未编码的#）
    constexpr auto invalid_reg_name = "bad#host.com"_span;
    static_assert(!authority(invalid_reg_name));

    // 端口溢出
    // NOTE: 只做类型校验，不做值限制
    constexpr auto port_overflow = "example.com:65536"_span; // 超过65535
    static_assert(authority(port_overflow));

    // 混合错误：错误IPv4格式+非法userinfo
    constexpr auto multi_error = "user@[::1:8080"_span; // IPv6缺少闭合括号
    static_assert(!authority(multi_error));

    return 0;
}

// NOLINTEND