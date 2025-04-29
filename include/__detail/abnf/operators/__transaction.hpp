#pragma once

#include "../detail/__types.hpp"

namespace mcs::abnf::operators
{
    struct transaction
    {
      public:
        constexpr explicit transaction(detail::parser_ctx_ref ctx) noexcept
            : ctx_{ctx}, index_{ctx.cur_index}
        {
        }
        constexpr void commit() noexcept
        {
            committed_ = true;
        }
        constexpr ~transaction() noexcept
        {
            if (not committed_)
                ctx_.cur_index = index_;
        }
        transaction(const transaction &) = delete;
        transaction(transaction &&) = delete;
        transaction &operator=(const transaction &) = delete;
        transaction &operator=(transaction &&) = delete;

      private:
        detail::parser_ctx_ref ctx_;
        size_t index_;
        bool committed_{false};
    };
}; // namespace mcs::abnf::operators