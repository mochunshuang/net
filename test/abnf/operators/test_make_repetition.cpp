
#include "../test_abnf.hpp"

#include <cassert>
#include <cstddef>
#include <iostream>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::operators;

int main()
{

    using A = generate::CharInsensitive<'A'>;
    auto call_A = 0;
    auto A_callback = [&](size_t begin, size_t end) noexcept {
        if (begin < end)
        {
            call_A = 1;
            std::cout << "A_callback called\n";
        }
    };

    auto rule = sequence{repetition<0, 1, watch_index<A, decltype(A_callback)>>{
                             watch_index(A{}, A_callback)},
                         generate::CharInsensitive<'c'>{}};
    {
        auto ctx = "ac"_ctx;
        auto ret = rule(ctx);
        assert(ret);
        assert(ret.value() == 2);
        assert(ctx.cur_index == 2);
        assert(ctx.done());

        assert(call_A == 1);
    }
    {

        call_A = 0;
        auto ctx = "c"_ctx;
        auto ret = rule(ctx);
        assert(ret);
        assert(ret.value() == 1);
        assert(ctx.cur_index == 1);
        assert(ctx.done());
        assert(call_A == 0); // not called
    }
    {
        auto rule = sequence{optional<A>{}, generate::CharInsensitive<'c'>{}};
        auto ctx = "c"_ctx;
        auto ret = rule(ctx);
        assert(ret);
        assert(ret.value() == 1);
        assert(ctx.cur_index == 1);
        assert(ctx.done());
    }

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND