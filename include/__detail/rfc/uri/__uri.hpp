#pragma once

#include "./__scheme.hpp"
#include "./__hier_part.hpp"
#include "./__query.hpp"
#include "./__fragment.hpp"
#include <optional>

namespace mcs::rfc::uri
{
    // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    class URI
    {
        path_absolute s_;
        hier_part hP_;
        std::optional<query> q_;
        std::optional<fragment> f_;

      public:
        decltype(auto) scheme(this auto &&self) noexcept
        {
            std::forward_like<decltype(self)>(self.s_);
        }
        decltype(auto) hier_part(this auto &&self) noexcept // NOLINT
        {
            std::forward_like<decltype(self)>(self.hP_);
        }
        decltype(auto) query(this auto &&self) noexcept
        {
            std::forward_like<decltype(self)>(self.q_);
        }
        decltype(auto) fragment(this auto &&self) noexcept
        {
            std::forward_like<decltype(self)>(self.f_);
        }
    };

    constexpr ReturnType<URI> make_reg_name(span_param_in sp) noexcept
    {
        if (parse::URI(sp))
            return URI{};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri
