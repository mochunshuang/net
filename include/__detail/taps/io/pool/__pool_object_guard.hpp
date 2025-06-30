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

        explicit pool_object_guard(Pool &pool) : buffer_{pool.allocate()}, pool_{pool} {}
        ~pool_object_guard()
        {
            pool_.deallocate(static_cast<object_type *>(buffer_));
        }
        object_type *data() const noexcept
        {
            return static_cast<object_type *>(buffer_);
        }

      private:
        void *buffer_;
        Pool &pool_;
    };
}; // namespace mcs::net::io::pool