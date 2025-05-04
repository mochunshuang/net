
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
        constexpr auto authority_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = authority{}(ctx);
            assert(ctx.cur_index == span.size());
            return suc;
        };
        // 完整结构：userinfo + IPv6 + port
        constexpr auto full_authority_1 =
            "user%3Apass@[v9.fe]:8080"_span; // 混合编码和IPvFuture
        static_assert(authority_rule(full_authority_1));
    }

    // 无效测试用例
    {
        constexpr auto authority_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = authority{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            return suc;
        };
    }

    // build 测试
    {
        constexpr auto authority_rule = authority{};
        // 完整结构：userinfo + IPv6 + port
        constexpr auto full_authority_1 =
            "user%3Apass@[v9.fe]:8080"_span; // 混合编码和IPvFuture
        auto ctx = make_parser_ctx(full_authority_1);
        auto ret = authority_rule.parse(ctx);
        assert(ret);
        assert(authority::result_type::userinfo_t::domain::build(
                   (*ret).userinfo.value()) == std::string("user%3Apass"));
        assert(authority::result_type::host_t::domain::build((*ret).host) ==
               std::string("[v9.fe]"));
        std::cout << "ctx.cur_index: " << ctx.cur_index << '\n';
        assert(ctx.cur_index == full_authority_1.size());
        assert((*ret).port.has_value());
        assert(authority::result_type::port_t::domain::build((*ret).port.value()) ==
               std::string("8080"));
        {
            auto rule =
                make_optional{make_sequence{CharRule<CharSensitive<':'>>{}, port{}}};
            constexpr auto full_authority_1 = ":8080"_span;
            auto ctx = make_parser_ctx(full_authority_1);
            rule.parse(ctx);
            assert(ctx.cur_index == full_authority_1.size());
        }
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
