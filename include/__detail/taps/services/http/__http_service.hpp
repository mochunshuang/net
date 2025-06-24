
#pragma once

namespace mcs::net::services::http
{
    template <typename BaseService>
    struct http_service
    {
        auto accept() noexcept
        {

            return 0;
        }

        explicit http_service(BaseService &base) noexcept : up_service_{base} {}

      private:
        BaseService &up_service_; // NOLINT
    };

}; // namespace mcs::net::services::http