#pragma once

namespace mcs::net::io::pool
{
    template <typename Pool>
    struct pool_object_guard
    {
        using object_type = typename Pool::object_type;

        pool_object_guard(const pool_object_guard &) = delete;
        pool_object_guard(pool_object_guard &&) = delete;
        pool_object_guard &operator=(const pool_object_guard &) = delete;
        pool_object_guard &operator=(pool_object_guard &&) = delete;

        constexpr explicit pool_object_guard(Pool &pool)
            : buffer_{pool.allocate()}, pool_{pool}
        {
        }
        constexpr ~pool_object_guard() noexcept
        {
            pool_.deallocate(buffer_);
        }
        constexpr object_type *data() const noexcept
        {
            return buffer_;
        }

      private:
        object_type *buffer_;
        Pool &pool_;
    };
}; // namespace mcs::net::io::pool