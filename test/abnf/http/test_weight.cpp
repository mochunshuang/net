#include "../test_head.hpp"

#include <cassert>
#include <string_view>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // weight = OWS ";" OWS "q=" qvalue

    constexpr auto OWS0 = "";      // NOLINT
    constexpr auto OWS1 = "     "; // NOLINT

    {
        constexpr auto ows_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = OWS{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        static_assert(ows_rule(""_span));
        static_assert(ows_rule("     "_span));
    }

    {
        constexpr auto qvalue_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = qvalue{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        static_assert(qvalue_rule("0.111"_span));
        { // error parse
            constexpr auto qvalue_rule = [](const auto &array) constexpr {
                auto span = std::span{array};
                parser_ctx ctx = make_parser_ctx(span);
                assert(ctx.cur_index == 0);
                auto suc = qvalue{}(ctx);
                assert(ctx.cur_index != span.size());
                assert(not ctx.done());
                return suc;
            };
            static_assert(qvalue_rule("0.1111"_span));
            static_assert(qvalue_rule("0.1110"_span));
        }
    }

    // weight = OWS ";" OWS "q=" qvalue
    {
        constexpr auto weight_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = weight{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };

        static_assert(weight_rule(";q=0.111"_span));
        static_assert(weight_rule(" ;Q=0.111"_span));

        static_assert(weight_rule(" ;         q=0.111"_span));
        static_assert(weight_rule("  ;         Q=0.111"_span));
    }

    constexpr auto weight_rule = [](const auto &array) constexpr {
        auto span = std::span{array};
        parser_ctx ctx = make_parser_ctx(span);
        assert(ctx.cur_index == 0);
        auto suc = weight{}(ctx);
        assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
        assert(not ctx.done());
        return suc;
    };
    static_assert(not weight_rule("0.111"_span));
    static_assert(not weight_rule("0.111"_span));
    static_assert(not weight_rule(";q= 0.111"_span));
    static_assert(not weight_rule(" ;q= 0.111"_span));
    static_assert(not weight_rule(" ;         q= 0.111"_span));
    static_assert(not weight_rule("  ;         q= 0.111"_span));

    static_assert(weight_rule(";q=0.1110"_span));
    static_assert(weight_rule(";q=0.1110"_span));
    static_assert(weight_rule(" ;q=0.1110"_span));
    static_assert(weight_rule(" ;         q=0.1110"_span));
    static_assert(weight_rule("  ;         q=0.1110"_span));

    return 0;
}
// NOLINTEND