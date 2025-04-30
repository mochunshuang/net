
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
        constexpr auto ipvfuture_rule = [](parser_ctx ctx) constexpr {
            auto suc = mcs::abnf::uri::IPvFuture{}(ctx);
            assert(ctx.empty());
            return suc;
        };
        constexpr auto valid_v1 = "v1.x"_ctx;           // 合法：'x' 是 unreserved
        constexpr auto valid_v2 = "vA.:"_ctx;           // 合法：':' 是允许字符
        constexpr auto valid_v3 = "vF.~"_ctx;           // 合法：'~' 是 unreserved
        constexpr auto valid_v4 = "v0.!$&'()*+,;="_ctx; // 合法：全 sub-delims 字符
        constexpr auto valid_v5 = "v123.a_b:z"_ctx;     // 合法：混合 unreserved 和 ':'
        static_assert(ipvfuture_rule(valid_v1));
        static_assert(ipvfuture_rule(valid_v2));
        static_assert(ipvfuture_rule(valid_v3));
        static_assert(ipvfuture_rule(valid_v4));
        static_assert(ipvfuture_rule(valid_v5));
    }

    // 无效测试用例
    {
        constexpr auto ipvfuture_rule = [](parser_ctx ctx) constexpr {
            auto suc = mcs::abnf::uri::IPvFuture{}(ctx);
            assert(ctx.cur_index == 0);
            return suc;
        };
        constexpr auto invalid_v1 = "a1.x"_ctx;    // 无效：不以 'v' 开头
        constexpr auto invalid_v2 = "v.abc"_ctx;   // 无效：'v' 后无 HEXDIG
        constexpr auto invalid_v3 = "vG.abc"_ctx;  // 无效：'G' 不是 HEXDIG
        constexpr auto invalid_v4 = "v1a"_ctx;     // 无效：缺少 '.' 和尾部
        constexpr auto invalid_v5 = "v2.#"_ctx;    // 无效：'#' 属于 gen-delims
        constexpr auto invalid_v6 = "v3."_ctx;     // 无效：尾部为空
        constexpr auto invalid_v7 = "v4a./?="_ctx; // 无效：包含 '/' 和 '?'
        static_assert(not ipvfuture_rule(invalid_v1));
        static_assert(not ipvfuture_rule(invalid_v2));
        static_assert(not ipvfuture_rule(invalid_v3));
        static_assert(not ipvfuture_rule(invalid_v4));
        static_assert(not ipvfuture_rule(invalid_v5));
        static_assert(not ipvfuture_rule(invalid_v6));
        static_assert(not ipvfuture_rule(invalid_v7));
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
