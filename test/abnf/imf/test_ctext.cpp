#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    /*
  ctext           =   %d33-39 /          ; Printable US-ASCII
                      %d42-91 /          ;  characters not including
                      %d93-126 /         ;  "(", ")", or "\"
                      obs-ctext
  */
    // 有效边界值
    {
        constexpr auto ctext_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::imf::ctext{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        static_assert(ctext_rule("!"_span)); // 33

        static_assert(ctext_rule("\x01"_span)); // obs_ctext
        static_assert(ctext_rule("\""_span));   // " 34
    }

    // 无效值
    {
        constexpr auto ctext_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::imf::ctext{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        static_assert(!ctext_rule(")"_span)); // 41 应无效

        static_assert(!ctext_rule("\x20"_span)); // 32 空格
        static_assert(!ctext_rule(" "_span));    // 空格 32
    }

    return 0;
}
// NOLINTEND