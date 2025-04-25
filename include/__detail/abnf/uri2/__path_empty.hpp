#pragma once

#include "../detail/__types.hpp"
#include "./__segment.hpp"
#include "./__segment_nz.hpp"
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

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type & /*ctx*/)
        {
            constexpr std::string k_path_empty;
            return k_path_empty;
        }
    };

}; // namespace mcs::abnf::uri