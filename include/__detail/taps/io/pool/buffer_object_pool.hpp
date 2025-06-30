#pragma once

#include <concepts>
#include <bit>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

namespace mcs::net::io::pool
{
    template <typename T>
    concept is_inheritable =
        !std::is_final_v<T> && !std::is_enum_v<T> && !std::is_fundamental_v<T> &&
        !std::is_union_v<T> && !std::is_function_v<T> && std::is_class_v<T>;

    template <std::default_initializable T>
        requires(is_inheritable<T>)
    struct buffer_object_pool
    {
        using object_type = T;
        // T chunk[block_count];
        struct chunk;
        struct double_link
        { // base class for intrusive doubly-linked structures
            double_link *next;
            double_link *prev;
        };

        struct chunk_storage_type : double_link
        {
            chunk item; // NOLINT
            // NOTE: 'this' make it can`t constexpr
            chunk_storage_type() noexcept : item(this) {}
        };
        struct owner_help
        {
            owner_help(const owner_help &) = delete;
            owner_help(owner_help &&other) noexcept
                : ptr_(std::exchange(other.ptr_, nullptr))
            {
            }
            owner_help &operator=(const owner_help &) = delete;
            owner_help &operator=(owner_help &&) = delete;
            owner_help() noexcept : ptr_(nullptr) {}
            explicit owner_help(chunk_storage_type *ptr) noexcept : ptr_(ptr) {}
            constexpr ~owner_help() noexcept
            {
                delete ptr_;
            }
            constexpr void release() noexcept
            {
                ptr_ = nullptr;
            }
            [[nodiscard]] constexpr auto *data() const noexcept
            {
                return ptr_;
            }
            constexpr void set_data(chunk_storage_type *ptr) noexcept // NOLINT
            {
                ptr_ = ptr;
            }

          private:
            chunk_storage_type *ptr_;
        };
        struct object_storage_type : public T
        {
            chunk *chunk_ptr{};
        };
        struct chunk
        {
            using status_type = std::size_t;
            static constexpr auto block_count = sizeof(status_type) * CHAR_BIT; // NOLINT

            explicit chunk(chunk_storage_type *parent_ptr) noexcept
                : chunk_storage_(parent_ptr), status_{~status_type{0}}
            {
                for (std::size_t i{0}; i < block_count; ++i)
                {
                    buffer_[i].chunk_ptr = this; // NOLINT
                }
            }

            [[nodiscard]] constexpr auto free_count() const noexcept // NOLINT
            {
                return std::popcount(status_);
            }
            [[nodiscard]] constexpr auto allocate_count() const noexcept // NOLINT
            {
                return chunk::block_count - free_count();
            }

            constexpr T *allocate() noexcept
            {
                return empty() ? nullptr : do_allocate();
            }

            constexpr T *do_allocate() noexcept // NOLINT
            {
                assert(status_ != 0);
                const auto k_index = std::countr_zero(status_);
                return (set_zero(k_index), static_cast<T *>(&buffer_[k_index])); // NOLINT
            }
            constexpr void deallocate(T *ptr) noexcept
            {
                set_one(as_storage(ptr) - &buffer_[0]);
            }
            [[nodiscard]] constexpr bool empty() const noexcept
            {
                return status_ == 0;
            }

            static constexpr object_storage_type *as_storage(T *ptr) noexcept // NOLINT
            {
                return static_cast<object_storage_type *>(ptr);
            }

            [[nodiscard]] constexpr auto *chunk_storage() const noexcept // NOLINT
            {
                return chunk_storage_;
            }

          private: // 添加父对象指针//NOTE: 地址偏移量只能运行时，且标准布局才安全
            chunk_storage_type *chunk_storage_; // NOLINT
            status_type status_;
            object_storage_type buffer_[block_count]; // NOLINT

            constexpr void set_one(std::uint8_t index) noexcept // NOLINT
            {
                // NOTE: status_type{1} 是必须的。否则bit位截断溢出,未定义
                status_ |= (status_type{1} << index);
            }
            constexpr void set_zero(std::uint8_t index) noexcept // NOLINT
            {
                status_ &= ~(status_type{1} << index);
            }
        };

        static constexpr auto pool_size = chunk::block_count + 1; // NOLINT
        explicit buffer_object_pool(
            std::size_t init_count) // NOTE:  new make it can`t be constexpr
        {
            const std::size_t k_chunks_needed =
                (init_count + chunk::block_count - 1) / chunk::block_count;
            std::vector<owner_help> temp_item_owner;
            temp_item_owner.reserve(k_chunks_needed);
            try
            {
                for (std::size_t i = 0; i < k_chunks_needed; ++i)
                {
                    auto *item = new chunk_storage_type();
                    // NOTE: reserve is uninitialized memory
                    temp_item_owner.emplace_back(item);
                    pool_[chunk::block_count].push_back(item);
                    ++chunk_count_;
                }
                for (auto &owner : temp_item_owner)
                {
                    owner.release();
                }
            }
            catch (...)
            {
                throw;
            }
        }
        constexpr ~buffer_object_pool() noexcept
        {
            for (std::size_t i{0}; i < pool_size; ++i)
            {
                intrusive_list<chunk_storage_type> &list = pool_[i]; // NOLINT
                while (not list.empty())
                {
                    chunk_storage_type *link = list.pop_front();
                    allocate_count_ -= link->item.allocate_count();
                    --chunk_count_;
                    delete link;
                }
            }
            assert(chunk_count_ == 0);
            assert(allocate_count_ == 0);
        }
        buffer_object_pool(const buffer_object_pool &) = delete;
        buffer_object_pool(buffer_object_pool &&) = delete;
        buffer_object_pool &operator=(const buffer_object_pool &) = delete;
        buffer_object_pool &operator=(buffer_object_pool &&) = delete;

        constexpr auto try_shrink(std::size_t max_shrink_count) noexcept // NOLINT
        {
            auto &must_free_list = pool_[chunk::block_count];
            std::size_t shrink_count{0};
            while (shrink_count + chunk::block_count <= max_shrink_count &&
                   !must_free_list.empty())
            {
                chunk_storage_type *item = must_free_list.pop_back();
                --chunk_count_;
                shrink_count += chunk::block_count;
                delete item;
            }
            return shrink_count;
        }
        constexpr float usage_rate() noexcept // NOLINT
        {
            return (1.0 * allocate_count_) / (chunk_count_ * chunk::block_count);
        }

        constexpr T *allocate()
        {
            for (std::size_t i{1}; i < pool_size; ++i)
            {
                if (not pool_[i].empty())
                {
                    chunk_storage_type *link = pool_[i].pop_front(); // NOLINT
                    assert(link->item.free_count() == i);
                    T *ptr = link->item.do_allocate();
                    pool_[i - 1].push_back(link); // NOLINT
                    ++allocate_count_;
                    return ptr;
                }
            }
            // NOTE: need new chunk to allocate T* storage
            owner_help temp_owner;
            try
            {
                temp_owner.set_data(new chunk_storage_type());
            }
            catch (...)
            {
                throw;
            }
            auto *link = temp_owner.data();
            T *ptr = link->item.do_allocate();
            pool_[chunk::block_count - 1].push_back(link);
            ++chunk_count_;
            ++allocate_count_;
            temp_owner.release();
            return ptr;
        }
        constexpr void deallocate(T *ptr) noexcept
        {
            chunk *chunk_ptr = chunk::as_storage(ptr)->chunk_ptr;
            // NOTE: offsetof need standard-layout,so don`t use
            chunk_storage_type *item = chunk_ptr->chunk_storage();

            const std::size_t k_old_free_count = chunk_ptr->free_count();
            chunk_ptr->deallocate(ptr);
            --allocate_count_;

            pool_[k_old_free_count].remove(item);        // NOLINT
            pool_[k_old_free_count + 1].push_back(item); // NOLINT
        }

      private:
        template <typename Ty>
            requires(std::derived_from<Ty, double_link>)
        struct intrusive_list
        { // intrusive circular list of Ty, which must derive from _Double_link<_Tag>
            using link_type = double_link;
            constexpr intrusive_list() noexcept = default;
            intrusive_list(intrusive_list &&) = delete;
            intrusive_list &operator=(intrusive_list &&) = delete;
            intrusive_list(const intrusive_list &) = delete;
            intrusive_list &operator=(const intrusive_list &) = delete;
            ~intrusive_list() noexcept = default;

            // target: head_ -> item -> head_->next
            constexpr void push_front(Ty *const item) noexcept // NOLINT
            { // insert item at the head of this list
                insert_before(head_.next, static_cast<link_type *>(item));
            }

            // target:  head_->prev -> item -> head_
            constexpr void push_back(Ty *const item) noexcept // NOLINT
            { // insert item at the end of this list
                insert_before(&head_, static_cast<link_type *>(item));
            }

            [[nodiscard]] constexpr Ty *pop_front() noexcept // NOLINT
            {
                if (empty())
                    return nullptr;
                auto *node = head_.next;
                remove(as_item(node));
                return as_item(node);
            }
            [[nodiscard]] constexpr Ty *pop_back() noexcept // NOLINT
            {
                if (empty())
                    return nullptr;
                auto *node = head_.prev;
                remove(as_item(node));
                return as_item(node);
            }

            static constexpr void remove(Ty *const item) noexcept // NOLINT
            { // unlink item from this list
                auto *ptr = static_cast<link_type *>(item);
                ptr->next->prev = ptr->prev;
                ptr->prev->next = ptr->next;
            }

            [[nodiscard]] constexpr bool empty() const noexcept
            {
                return head_.next == &head_ && head_.prev == &head_;
            }

          private:
            link_type head_{&head_, &head_};

            // NOLINTNEXTLINE
            static constexpr void insert_before(link_type *pos, link_type *item) noexcept
            {
                item->next = pos;
                item->prev = pos->prev;
                // NOTE: first update  pos->prev->next then pos->prev
                pos->prev->next = item;
                pos->prev = item;
            }
            static constexpr link_type *as_link(Ty *const ptr) noexcept // NOLINT
            {
                // extract the link from the item denoted by _Ptr
                return static_cast<link_type *>(ptr);
            }

            static constexpr Ty *as_item(link_type *const ptr) noexcept // NOLINT
            { // get the item whose link is denoted by _Ptr
                return static_cast<Ty *>(ptr);
            }
        };

        std::size_t allocate_count_{0};                      // NOLINT
        std::size_t chunk_count_{0};                         // NOLINT
        intrusive_list<chunk_storage_type> pool_[pool_size]; // NOLINT
    };
}; // namespace mcs::net::io::pool