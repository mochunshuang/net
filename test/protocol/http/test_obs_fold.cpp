#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{

    // NOTE: obs 开头都说过时的，其他规则 已经用不到这个规则了
    //  ==================== obs-fold 规则测试 ====================
    constexpr auto obs_fold_pass = make_pass_test<obs_fold>();
    constexpr auto obs_fold_fail = make_unpass_test<obs_fold>();

    // 有效 obs-fold 测试
    static_assert(obs_fold_pass("\r\n "_span));       // 基础格式
    static_assert(obs_fold_pass(" \r\n\t"_span));     // OWS + CRLF + RWS
    static_assert(obs_fold_pass("\t \r\n \t "_span)); // 带制表符
    static_assert(obs_fold_pass("\r\n\t \t"_span));   // 最小格式

    // 无效 obs-fold 测试
    static_assert(!obs_fold_fail("\r\n"_span));      // 缺少RWS
    static_assert(!obs_fold_fail(" \r "_span));      // 缺少LF
    static_assert(!obs_fold_fail("\n "_span));       // 缺少CR
    static_assert(!obs_fold_fail("\r \n"_span));     // 空格在CRLF之间
    static_assert(!obs_fold_fail(" \r\n\x01"_span)); // 非法字符

    return 0;
}
// NOLINTEND