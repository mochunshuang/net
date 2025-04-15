#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct port
    {
    };

    constexpr ReturnType<port> make_port(span_param_in sp) noexcept
    {
        if (parse::port(sp))
            return port{};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri