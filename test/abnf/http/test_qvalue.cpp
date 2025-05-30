#include "../test_head.hpp"

#include <cassert>
#include <iostream>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    constexpr auto qvalue_rule = make_pass_test<qvalue>();
    constexpr auto not_qvalue_rule = make_unpass_test<qvalue>();

    // qvalue = ( "0" [ "." *3DIGIT ] ) / ( "1" [ "." *3"0" ] )
    static_assert(not qvalue_rule(""_span));
    static_assert(not make_rule_test<qvalue>("2"_span).second.done());

    static_assert(qvalue_rule("0"_span));
    static_assert(qvalue_rule("0."_span));
    static_assert(qvalue_rule("0.1"_span));
    static_assert(qvalue_rule("0.11"_span));
    static_assert(qvalue_rule("0.111"_span));
    static_assert(qvalue_rule("0.101"_span));
    static_assert(qvalue_rule("0.999"_span));

    static_assert(not_qvalue_rule("0.1111"_span));

    static_assert(not_qvalue_rule("0.11x"_span));

    static_assert(not_qvalue_rule("1.11x"_span));
    static_assert(not_qvalue_rule("1.11"_span));

    static_assert(not_qvalue_rule("1.1"_span));

    static_assert(qvalue_rule("1."_span));
    static_assert(qvalue_rule("1"_span));
    static_assert(qvalue_rule("1.0"_span));
    static_assert(qvalue_rule("1.00"_span));
    static_assert(qvalue_rule("1.000"_span));

    static_assert(not_qvalue_rule("1.0000"_span));

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND