#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"
#include <expected>

namespace mcs::rfc::uri
{
    struct IPv4address
    {
        explicit IPv4address(span_param_in /*sp*/) {}
    };

    constexpr ReturnType<IPv4address> make_IPv4address(abnf::span_param_in sp) noexcept
    {
        if (parse::IPv4address(sp))
            return IPv4address{sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri