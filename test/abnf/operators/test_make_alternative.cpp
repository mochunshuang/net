
#include "../test_abnf.hpp"

#include <cassert>
#include <iostream>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::operators;

int main()
{

    using A = generate::CharInsensitive<'A'>;
    using B = generate::CharInsensitive<'B'>;
    auto id = 0;
    auto A_callback = [&](const detail::parser_ctx & /*ctx*/) noexcept {
        id = 1;
        std::cout << "A_callback called \n";
    };
    auto B_callback = [&](const detail::parser_ctx & /*ctx*/) noexcept {
        id = 2;
        std::cout << "B_callback called\n";
    };

    auto rule = make_sequence{
        make_alternative{with_callback(A{}, A_callback), with_callback(B{}, B_callback)},
        generate::CharInsensitive<'c'>{}};
    {
        auto ctx = "ac"_ctx;
        auto ret = rule(ctx);
        assert(ret);
        assert(ret.value() == 2);
        assert(ctx.cur_index == 2);
        assert(ctx.empty());

        assert(id == 1);
    }
    {
        auto ctx = "bc"_ctx;
        auto ret = rule(ctx);
        assert(ret);
        assert(ret.value() == 2);
        assert(ctx.cur_index == 2);
        assert(ctx.empty());

        assert(id == 2);
    }

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND