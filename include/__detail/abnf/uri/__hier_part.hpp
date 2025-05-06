#pragma once

#include "../detail/__types.hpp"

#include "./__authority.hpp"
#include "./__path_abempty.hpp"
#include "./__path_absolute.hpp"
#include "./__path_rootless.hpp"
#include "./__path_empty.hpp"
#include <cassert>
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
            using authority_t = uri::authority::result_type;
            using path_abempty_t = uri::path_abempty::result_type;
            using path_absolute_t = uri::path_absolute::result_type;
            using path_rootless_t = uri::path_rootless::result_type;
            using path_empty_t = uri::path_empty::result_type;

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
        using rule_concept = rule_t;

        static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
        {
            using rule = alternative<sequence<CharInsensitive<'/'>, CharInsensitive<'/'>,
                                              authority, path_abempty>,
                                     path_absolute, path_rootless, path_empty>;
            auto ret = rule{}(ctx);
            return ret ? detail::make_consumed_result(*ret) : std::nullopt;
        }

        static constexpr auto parse(parser_ctx_ref ctx) noexcept
            -> std::optional<result_type>
        {
            constexpr auto k_rule =
                make_alternative{make_sequence{CharRule<CharInsensitive<'/'>>{},
                                               CharRule<CharInsensitive<'/'>>{},
                                               authority{}, path_abempty{}},
                                 path_absolute{}, path_rootless{}, path_empty{}};
            auto ret = k_rule.parse(ctx);
            if (not ret)
                return std::nullopt;
            result_type result;
            assert(ret->value.index() != 0);
            if (ret->value.index() == 1)
            {
                result_type::authority_path_t authority;
                auto &sequence = std::get<1>(ret->value);
                authority.authority = sequence.value.get<2>();
                authority.path_abempty = sequence.value.get<3>();
                result.value = authority;
            }
            else if (ret->value.index() == 2)
                result.value = std::get<2>(ret->value);
            else if (ret->value.index() == 3)
                result.value = std::get<3>(ret->value);
            else if (ret->value.index() == 4)
                result.value = std::get<4>(ret->value);
            return result;
        }
        static constexpr auto buildString(const result_type &ctx) noexcept
        {
            if (std::holds_alternative<result_type::authority_path_t>(ctx.value))
            {
                std::string build("//");
                result_type::authority_path_t authority_path = std::get<1>(ctx.value);
                build
                    .append(result_type::authority_t::domain::buildString(
                        authority_path.authority))
                    .append(result_type::path_abempty_t::domain::buildString(
                        authority_path.path_abempty));
                return build;
            }
            std::string hier_part;
            if (ctx.value.index() == 2)
                hier_part.append(result_type::path_absolute_t::domain::buildString(
                    std::get<2>(ctx.value)));
            else if (ctx.value.index() == 3)
                hier_part.append(result_type::path_rootless_t::domain::buildString(
                    std::get<3>(ctx.value)));
            else if (ctx.value.index() == 4)
                hier_part.append(result_type::path_empty_t::domain::buildString(
                    std::get<4>(ctx.value)));
            return hier_part;
        }
    };

}; // namespace mcs::abnf::uri