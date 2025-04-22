#pragma once

namespace mcs::abnf::operators
{
    template <typename... R>
        requires(sizeof...(R) > 0)
    struct concatenation;

    template <typename R>
    struct concatenation<R>
    {
    };

}; // namespace mcs::abnf::operators