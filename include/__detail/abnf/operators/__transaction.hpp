#pragma once

#include "../detail/__types.hpp"

namespace mcs::abnf::operators
{
    struct transaction
    {
      public:
        explicit transaction(detail::parser_ctx &ctx) noexcept
            : ctx_{ctx}, index_{ctx.cur_index}
        {
        }
        void commit() noexcept
        {
            committed_ = true;
        }
        ~transaction() noexcept
        {
            if (not committed_)
                ctx_.cur_index = index_;
        }
        transaction(const transaction &) = delete;
        transaction(transaction &&) = delete;
        transaction &operator=(const transaction &) = delete;
        transaction &operator=(transaction &&) = delete;

      private:
        detail::parser_ctx &ctx_;
        size_t index_;
        bool committed_{false};
    };
}; // namespace mcs::abnf::operators