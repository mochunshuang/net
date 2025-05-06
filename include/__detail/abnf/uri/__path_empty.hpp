#pragma once

#include "__pchar.hpp"
#include <optional>

namespace mcs::abnf::uri
{

    // path-empty    = 0<pchar> ; path-empty      ; zero characters
    struct path_empty
    {
        struct __type
        {
            using domain = path_empty;
        };
        using result_type = __type;
        using rule = times<0, pchar>;
        using rule_concept = rule_t;

        static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
        {
            auto ret = rule{}(ctx);
            return ret ? detail::make_consumed_result(*ret) : std::nullopt;
        }
        static constexpr auto parse(parser_ctx_ref ctx) noexcept
            -> std::optional<result_type>
        {
            auto ret = operator()(ctx);
            return ret ? std::optional<result_type>{__type{}} : std::nullopt;
        }
        static constexpr auto buildString(const result_type & /*ctx*/) noexcept
        {
            std::string path_empty;
            return path_empty;
        }
    };

}; // namespace mcs::abnf::uri