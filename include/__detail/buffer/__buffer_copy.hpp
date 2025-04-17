#pragma once

#include "./__buffer_sequence.hpp"
#include "./__buffer_size.hpp"
#include "./__arithmetic.hpp"

#include <cstring>

namespace mcs::net::buffer
{
    /**
     * @brief
     * 1、Effects: Copies bytes from the buffer sequence source to the buffer sequence
     * dest, as if by calls to memcpy.
     * 2、The number of bytes copied is the lesser of:
     * —(2.1) buffer_size(dest);
     * —(2.2) buffer_size(source); and
     * —(2.3) max_size, if specified.
     * 3、The mutable buffer sequence dest specifies memory where the data should be
     * placed. The operation always fills a buffer in the sequence completely before
     * proceeding to the next.
     * 4、The constant buffer sequence source specifies memory where the data to be
     * written is located. The operation always copies a buffer in the sequence completely
     * before proceeding to the next
     * 5、Returns: The number of bytes copied from source to dest.
     */
    template <class MutableBufferSequence, class ConstBufferSequence>
    constexpr std::size_t buffer_copy(const MutableBufferSequence &dest,
                                      const ConstBufferSequence &source,
                                      std::size_t max_size) noexcept
    {
        std::size_t total_copied{0};

        auto src_it = buffer_sequence_begin(source);
        auto src_end = buffer_sequence_end(source);

        auto dest_it = buffer_sequence_begin(dest);
        auto dest_end = buffer_sequence_end(dest);

        // 处理每个缓冲区直到达到任意限制
        while (src_it != src_end && dest_it != dest_end && total_copied < max_size)
        {
            // 解引用当前缓冲区
            const_buffer src_buf(*src_it);
            mutable_buffer dest_buf(*dest_it);

            std::size_t src_avail = src_buf.size();
            std::size_t dest_avail = dest_buf.size();
            std::size_t remaining = max_size - total_copied;
            std::size_t copy_size = std::min({src_avail, dest_avail, remaining});

            if (copy_size > 0)
            {

                std::memcpy(dest_buf.data(), src_buf.data(), copy_size);
                total_copied += copy_size;

                // 更新缓冲区状态
                *src_it += copy_size;
                *dest_it += copy_size;

                // 如果当前缓冲区耗尽则移动迭代器
                if (src_buf.size() == copy_size)
                    ++src_it;
                if (dest_buf.size() == copy_size)
                    ++dest_it;
                continue;
            }
            break;
        }

        return total_copied;
    }
    template <class MutableBufferSequence, class ConstBufferSequence>
    constexpr std::size_t buffer_copy(const MutableBufferSequence &dest,
                                      const ConstBufferSequence &source) noexcept
    {
        auto dest_size = buffer_size(dest);
        auto src_size = buffer_size(source);
        return buffer_copy(dest, source, std::min(dest_size, src_size));
    }

}; // namespace mcs::net::buffer