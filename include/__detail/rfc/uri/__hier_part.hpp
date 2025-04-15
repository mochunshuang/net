#pragma once

#include "./__authority.hpp"
#include "./__path_abempty.hpp"
#include "./__path_absolute.hpp"
#include "./__path_rootless.hpp"
#include "./__path_empty.hpp"

namespace mcs::rfc::uri
{
    struct hier_part
    {
        enum type : id_type
        {
            AUTHORITY_PATH_ABEMPTY,
            PATH_ABSOLUTE,
            PATH_ROOTLESS,
            PATH_EMPTY
        };
        // 定义一个构造函数模板即可。变参解决
    };
    constexpr ReturnType<hier_part> make_hier_part(span_param_in sp) noexcept
    {
        if (parse::hier_part(sp))
            return hier_part{};
        return err_index(0);
    }
}; // namespace mcs::rfc::uri
