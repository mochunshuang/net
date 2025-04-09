#pragma once

#include "../../queries/__get_domain.hpp"
#include "../../queries/__get_scheduler.hpp"

#include "./__query_or_default.hpp"

namespace mcs::execution::snd::general
{
    template <typename Scheduler>
    class SCHED_ENV // NOLINT
    {
        Scheduler sched; // NOLINT

      public:
        explicit SCHED_ENV(Scheduler &&s) : sched(::std::forward<Scheduler>(s)) {}

        constexpr auto query(queries::get_scheduler_t const & /*unused*/) const noexcept
        {
            return sched;
        }

        constexpr auto query(queries::get_domain_t const &q /*unused*/) const noexcept
        {
            // TODO(mcs): just()也没有，或许是
            // return sched.query(q);
            return query_or_default(q, sched, default_domain());
        }
    };

    template <typename T>
    SCHED_ENV(T &&) -> SCHED_ENV<::std::remove_cvref_t<T>>;

}; // namespace mcs::execution::snd::general