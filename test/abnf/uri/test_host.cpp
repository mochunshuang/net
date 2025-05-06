
#include "../test_abnf.hpp"
#include "./test_uri.hpp"
#include <cassert>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::uri;

#include <iostream>

int main()
{
    // 有效测试用例
    {
        constexpr auto host_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = host{}(ctx);
            assert(ctx.cur_index == span.size());
            return suc;
        };
        //================= IP-literal 有效用例 =================
        constexpr auto ipv6_1 = "[2001:db8::1]"_span;          // 标准 IPv6 缩写
        constexpr auto ipv6_2 = "[::ffff:192.168.0.1]"_span;   // IPv4 嵌入格式
        constexpr auto ipv6_3 = "[1:2:3:4:5:6:7:8]"_span;      // 全展开 IPv6
        constexpr auto ip_future_1 = "[v1.a]"_span;            // 最小 IPvFuture
        constexpr auto ip_future_2 = "[vF.~!$&'()*+,;=]"_span; // 全部允许字符
        static_assert(host_rule(ipv6_1));
        static_assert(host_rule(ipv6_2));
        static_assert(host_rule(ipv6_3));
        static_assert(host_rule(ip_future_1));
        static_assert(host_rule(ip_future_2));

        //================= IPv4address 有效用例 =================
        constexpr auto ipv4_1 = "192.168.0.1"_span;     // 标准 IPv4
        constexpr auto ipv4_2 = "255.255.255.255"_span; // 最大值边界
        constexpr auto ipv4_3 = "0.0.0.0"_span;         // 最小值边界
        constexpr auto ipv4_4 = "123.45.67.89"_span;    // 混合数值
        static_assert(host_rule(ipv4_1));
        static_assert(host_rule(ipv4_2));
        static_assert(host_rule(ipv4_3));
        static_assert(host_rule(ipv4_4));

        //================= reg-name 有效用例 ===================
        constexpr auto reg_1 = "example.com"_span;    // 常规域名
        constexpr auto reg_2 = "user_name~"_span;     // 包含 unreserved
        constexpr auto reg_3 = "!$&'()*+,;=abc"_span; // 全 sub-delims 开头
        constexpr auto reg_4 = "%20%3A%2F"_span;      // 合法 pct-encoded
        constexpr auto reg_5 = "a.b_c-d~"_span;       // 混合 unreserved
        static_assert(host_rule(reg_1));
        static_assert(host_rule(reg_2));
        static_assert(host_rule(reg_3));
        static_assert(host_rule(reg_4));
        static_assert(host_rule(reg_5));
    }

    // 无效测试用例
    {
        constexpr auto host_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = host{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            return suc;
        };
        constexpr auto invalid_reg_1 = "@example.com"_span; // 错误域名
        static_assert(host_rule(invalid_reg_1));
        static_assert(host_rule(invalid_reg_1).value() == 0);
    }

    // build 测试
    {
        // IP-literal
        {
            //================= IP-literal 有效用例 =================
            constexpr auto ipv6_1 = "[2001:db8::1]"_ctx; // 标准 IPv6 缩写
            auto host_rule = mcs::abnf::uri::host{};
            auto cxt = ipv6_1;
            auto ret = host_rule.parse(cxt);
            assert(ret);
            host::result_type obj = *ret;
            assert(std::holds_alternative<host::result_type::IP_literal_t>(obj.value));
            assert(obj.value.index() == 1);

            assert(std::string_view{"[2001:db8::1]"} == host::buildString(obj));
            assert(std::string_view{"[2001:db8::1]"} ==
                   host::result_type::IP_literal_t::domain::buildString(
                       std::get<1>(obj.value)));
        }
        // IPv4address
        {
            //================= IPv4address 有效用例 =================
            constexpr auto ipv4_1 = "192.168.0.1"_ctx; // 标准 IPv4
            auto host_rule = mcs::abnf::uri::host{};
            auto cxt = ipv4_1;
            auto ret = host_rule.parse(cxt);
            assert(ret);
            host::result_type obj = *ret;
            assert(std::holds_alternative<host::result_type::IPv4address_t>(obj.value));
            assert(obj.value.index() == 2);

            assert(std::string_view{"192.168.0.1"} == host::buildString(obj));
            assert(std::string_view{"192.168.0.1"} ==
                   host::result_type::IPv4address_t::domain::buildString(
                       std::get<2>(obj.value)));
        }
        //  reg-name
        {
            //================= reg-name 有效用例 ===================
            constexpr auto reg_1 = "example.com"_ctx; // 常规域名
            auto host_rule = mcs::abnf::uri::host{};
            auto cxt = reg_1;
            auto ret = host_rule.parse(cxt);
            assert(ret);
            host::result_type obj = *ret;

            assert(std::holds_alternative<host::result_type::reg_name_t>(obj.value));
            assert(obj.value.index() == 3);

            assert(std::string_view{"example.com"} == host::buildString(obj));
            assert(std::string_view{"example.com"} ==
                   host::result_type::reg_name_t::domain::buildString(
                       std::get<3>(obj.value)));
        }
        // 错误
        {
            // NOTE: reg-name      = *( unreserved / pct-encoded / sub-delims )
            constexpr auto invalid_reg_1 = "@example.com"_ctx; // 错误域名
            auto host_rule = mcs::abnf::uri::host{};
            auto cxt = invalid_reg_1;
            auto ret = host_rule.parse(cxt);
            assert(ret);
            host::result_type obj = *ret;

            assert(cxt.cur_index == 0);

            assert(std::holds_alternative<host::result_type::reg_name_t>(obj.value));
            assert(obj.value.index() == 3);
            assert(std::string_view{""} == host::buildString(obj));
            assert(std::string_view{""} ==
                   host::result_type::reg_name_t::domain::buildString(
                       std::get<3>(obj.value)));
        }
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
