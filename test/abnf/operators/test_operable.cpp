
#include "../test_abnf.hpp"

#include <iostream>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::operators;

int main()
{

    using Rule = generate::CharInsensitive<'A'>;

    constexpr auto rule = [](detail::parser_ctx ctx) constexpr {
        return Rule{}(ctx);
    };
    static_assert(rule("a"_ctx).has_value());
    static_assert(rule("A"_ctx).has_value());

    {
        using Rule = generate::CharSensitive<'A'>;
        constexpr auto rule = [](detail::parser_ctx ctx) constexpr {
            return Rule{}(ctx);
        };
        static_assert(not rule("a"_ctx).has_value());
        static_assert(rule("A"_ctx).has_value());
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND