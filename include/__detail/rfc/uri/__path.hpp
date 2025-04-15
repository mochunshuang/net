#pragma once

#include "./__path_abempty.hpp"
#include "./__path_absolute.hpp"
#include "./__path_noscheme.hpp"
#include "./__path_rootless.hpp"
#include "./__path_empty.hpp"

namespace mcs::rfc::uri
{
    /**
     * @brief
     *    path   = path-abempty    ; begins with "/" or is empty
     *           / path-absolute   ; begins with "/" but not "//"
     *           / path-noscheme   ; begins with a non-colon segment
     *           / path-rootless   ; begins with a segment
     *           / path-empty      ; zero characters
     */
    struct path
    {
        enum type : id_type
        {
            PATH_ABEMPTY,
            PATH_ABSOLUTE,
            PATH_NOSCHEME,
            PATH_ROOTLESS,
            PATH_EMPTY
        };
        // 定义一个构造函数模板即可。变参解决
    };

    constexpr ReturnType<path> make_path(span_param_in sp) noexcept
    {
        if (parse::path(sp))
            return path{};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri
