#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct reg_name
    {
        explicit reg_name(span_param_in /*sp*/) {}
    };
    constexpr ReturnType<reg_name> make_reg_name(span_param_in sp) noexcept
    {
        if (parse::reg_name(sp))
            return reg_name{sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri