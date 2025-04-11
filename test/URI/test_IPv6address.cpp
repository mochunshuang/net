
#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

// NOLINTBEGIN

using OCTET = mcs::ABNF::OCTET;

// 测试工具函数 =======================================================
namespace tool_test
{
    // split_span_first 测试
    static constexpr OCTET split_first1[] = {'a', ':', 'b', ':', 'c'};
    static_assert(mcs::ABNF::tool::split_span_first(split_first1, ':').first.size() == 1);
    static_assert(mcs::ABNF::tool::split_span_first(split_first1, ':').second.size() ==
                  3);

    // split_span_last 测试
    static constexpr OCTET split_last1[] = {'a', ':', 'b', ':', 'c'};
    static_assert(mcs::ABNF::tool::split_span_last(split_last1, ':').first.size() == 3);
    static_assert(mcs::ABNF::tool::split_span_last(split_last1, ':').second.size() == 1);
}; // namespace tool_test

// 测试 check_common_tail =============================================
namespace test_tail
{
    // 有效案例：6个h16段 + ls32（IPv4格式）
    static constexpr OCTET valid_tail1[] = {'1', ':', '2', ':', '3', ':', '4',
                                            ':', '5', ':', '6', ':', '1', '2',
                                            '7', '.', '0', '.', '0', '.', '1'};
    static_assert(mcs::ABNF::URI::detail::check_common_tail(valid_tail1, 6));

    // 有效案例：6个h16段 + ls32（IPv6格式）
    static constexpr OCTET valid_tail2[] = {'a', ':', 'b', ':', 'c', ':', 'd', ':',
                                            'e', ':', 'f', ':', 'a', 'b', 'c', 'd'};
    static_assert(mcs::ABNF::URI::detail::check_common_tail(valid_tail2, 6));

    // 无效案例：h16段包含非法字符（'g'）
    static constexpr OCTET invalid_h16_tail[] = {'1', ':', '2', ':', 'g', ':', '4',
                                                 ':', '5', ':', '6', ':', '1', '.',
                                                 '2', '.', '3', '.', '4'};
    static_assert(!mcs::ABNF::URI::detail::check_common_tail(invalid_h16_tail, 6));

    // 无效案例：段数不足（只有5段）
    static constexpr OCTET insufficient_segments[] = {'1', ':', '2', ':', '3', ':', '4',
                                                      ':', '5', ':', '1', '2', '7', '.',
                                                      '0', '.', '0', '.', '1'};
    static_assert(!mcs::ABNF::URI::detail::check_common_tail(insufficient_segments, 6));

    // 无效案例：ls32格式错误（IPv4地址超范围）
    static constexpr OCTET invalid_ls32[] = {'1', ':', '2', ':', '3', ':', '4',
                                             ':', '5', ':', '6', ':', '2', '5',
                                             '6', '.', '0', '.', '0', '.', '1'};
    static_assert(!mcs::ABNF::URI::detail::check_common_tail(invalid_ls32, 6));
} // namespace test_tail

// 测试 check_common_front ============================================
namespace test_front
{
    // 有效案例：空输入（允许空字符串）
    static_assert(mcs::ABNF::URI::detail::check_common_front(mcs::ABNF::empty_span, 0,
                                                             0));

    // 有效案例：单个h16（max_segments=1, max_colons=0）
    static constexpr OCTET single_h16[] = {'a', 'b', 'c', 'd'};
    static_assert(mcs::ABNF::HEXDIG('a'));
    static_assert(mcs::ABNF::URI::h16(single_h16));
    static_assert(mcs::ABNF::URI::detail::check_common_front(single_h16, 1, 0));

    // 有效案例：两段h16（1个冒号）
    static constexpr OCTET two_segments[] = {'a', ':', 'b'};
    // static_assert(mcs::ABNF::URI::detail::check_common_front(two_segments, 2, 1));

    // 无效案例：超出最大段数（允许2段但实际3段）
    static constexpr OCTET excess_segments[] = {'1', ':', '2', ':', '3'};
    static_assert(!mcs::ABNF::URI::detail::check_common_front(excess_segments, 2, 2));

    // 无效案例：超出最大冒号数（允许1个但实际2个）
    static constexpr OCTET excess_colons[] = {'a', ':', 'b', ':', 'c'};
    static_assert(!mcs::ABNF::URI::detail::check_common_front(excess_colons, 3, 1));

    // 无效案例：末尾带冒号（h16后不能有冒号）
    static constexpr OCTET trailing_colon[] = {'a', ':', 'b', ':'};
    static_assert(!mcs::ABNF::URI::detail::check_common_front(trailing_colon, 2, 1));

    // 边界案例：最大允许6个冒号（7段h16）
    static constexpr OCTET max_colons_case[] = {'1', ':', '2', ':', '3', ':', '4',
                                                ':', '5', ':', '6', ':', '7'};
    // static_assert(mcs::ABNF::URI::detail::check_common_front(max_colons_case, 7, 6));
} // namespace test_front

int main()
{
    return 0;
}

int main()
{
    return 0;
}
// NOLINTEND