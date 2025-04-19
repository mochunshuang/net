#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        // 空输入
        constexpr auto s0 = ""_span;
        static_assert(parameters(s0));

        // 单个参数（无空格）
        constexpr auto s1 = ";key=value"_span;    // 允许前导分号
        constexpr auto s2 = "key=value;"_span;    // 不以 分号开头
        constexpr auto s3 = ";;key=value;;"_span; // 多个空参数段
        static_assert(parameters(s1));
        static_assert(not parameters(s2));
        {
            // parameters = *( OWS ";" OWS [ parameter ] )
            // parameter = parameter-name "=" parameter-value
            static_assert(parameters(";key=value;"_span));
        }
        static_assert(parameters(s3));

        // 多个参数（含 OWS）
        constexpr auto s4 = "  ;  name=Alice  ;  age=30  ;  "_span;
        constexpr auto s5 = "\t;\tkey1=val1\t;\tkey2=val2\t;\t"_span;
        static_assert(parameters(s4));
        static_assert(parameters(s5));

        // 混合合法和空参数段
        constexpr auto s6 = "; ; key=value; ;"_span;
        static_assert(parameters(s6));

        // 极长参数列表
        constexpr auto s7 = ";a=1;b=2;c=3;d=4;e=5;f=6;g=7;h=8"_span;
        static_assert(parameters(s7));
    }
    // 非法测试用例
    {
        // 非法分隔符（非分号）
        constexpr auto s1 = "key=value,flag=true"_span;   // 逗号分隔
        constexpr auto s2 = "key=value key2=value2"_span; // 空格分隔
        static_assert(not parameters(s1));
        static_assert(not parameters(s2));

        // 非法参数格式
        constexpr auto s3 = ";key =value;"_span; // 参数名含空格
        constexpr auto s4 = "name=val;bad@key=val"_span;
        static_assert(not parameters(s3));
        static_assert(not parameters(s4));

        // 分号后非法字符
        constexpr auto s5 = "; key=val; invalid#param=1"_span;
        static_assert(parameters(s5));
        {
            static_assert(parameter("key=val"_span));
            static_assert(parameter("invalid#param=1"_span));
        }
    }
    // 边界测试
    {
        // 纯分号和 OWS
        constexpr auto s1 = "  ; \t ;  "_span;
        static_assert(parameters(s1));

        // 参数值为空（允许）
        constexpr auto s2 = ";key=;"_span;
        static_assert(not parameters(s2)); // 不允许空值

        // 极大参数数量
        // constexpr auto s3 = "a=1;" + std::string(1000, 'b') + "=2;"_span;
        // static_assert(parameters(s3));
    }

    return 0;
}
// NOLINTEND