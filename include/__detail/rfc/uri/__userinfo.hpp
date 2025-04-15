#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct userinfo
    {
        explicit userinfo(span_param_in /*sp*/) {}
    };
    constexpr ReturnType<userinfo> make_userinfo(span_param_in sp) noexcept
    {
        if (parse::userinfo(sp))
            return userinfo{sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri