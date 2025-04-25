#pragma once

#include "../detail/__types.hpp"

#include "./__authority.hpp"
#include "./__path_abempty.hpp"
#include "./__path_absolute.hpp"
#include "./__path_rootless.hpp"
#include "./__path_empty.hpp"
#include <variant>

namespace mcs::abnf::uri
{
    /**
     * @brief
     *  hier-part     = "//" authority path-abempty
                 / path-absolute
                 / path-rootless
                 / path-empty
     */
    struct hier_part
    {
        struct __type
        {
            using domain = hier_part;
            using authority_t = authority::result_type;
            using path_abempty_t = path_abempty::result_type;
            using path_absolute_t = path_absolute::result_type;
            using path_rootless_t = path_rootless::result_type;
            using path_empty_t = path_empty::result_type;

            struct authority_path_t // NOLINT
            {
                authority_t authority;
                path_abempty_t path_abempty;
            };

            std::variant<std::monostate, authority_path_t, path_absolute_t,
                         path_rootless_t, path_empty_t>
                value;
        };
        using result_type = __type;
        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {

            std::string hier_part;
            return hier_part;
        }
    };

}; // namespace mcs::abnf::uri