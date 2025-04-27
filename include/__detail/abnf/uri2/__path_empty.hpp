#pragma once

#include "../detail/__types.hpp"
#include "./__segment.hpp"
#include "./__segment_nz.hpp"
#include "__pchar.hpp"
#include <optional>

namespace mcs::abnf::uri
{
    // path-empty    = 0<pchar> ; path-empty      ; zero characters
    struct path_empty
    {
        struct __type
        {
        };
        using result_type = __type;
        using rule = times<0, pchar>;
        using rule_concept = rule_t;

        static constexpr auto operator()(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (auto ret = rule{}(ctx))
                return make_consumed_result(*ret);
            return std::nullopt;
        }
        static constexpr auto parse(parser_ctx &ctx) noexcept
            -> std::optional<result_type>
        {
            if (auto ret = operator()(ctx))
            {
                std::exchange(ctx.cur_index, ctx.cur_index + *ret);
                return {};
            }
            return std::nullopt;
        }
        static constexpr auto build(const result_type & /*ctx*/) noexcept
        {
            constexpr std::string k_path_empty;
            return k_path_empty;
        }
    };

}; // namespace mcs::abnf::uri