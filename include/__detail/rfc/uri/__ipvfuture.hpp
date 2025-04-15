#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct IPvFuture
    {
        explicit IPvFuture(span_param_in sp)
        {
            // TODO(mcs): 用哪个容器存粹 OCTET。BUFFER的概念最好了
            // 表示 随时可以传送出去
        }
    };
    constexpr ReturnType<IPvFuture> make_IPvFuture(span_param_in sp) noexcept
    {
        if (parse::IPvFuture(sp))
            return IPvFuture{sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri