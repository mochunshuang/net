#pragma once

#include "./__buffer_sequence.hpp"
#include "./__buffer.hpp"
#include "./__arithmetic.hpp"
#include <cstring>

namespace mcs::net::io::buffer
{
    namespace __detail
    {
        // Tag types used to select appropriately optimised overloads.
        struct one_buffer // NOTE: for mutable_buffer、const_buffer
        {
        };
        struct multiple_buffers
        {
        };
        template <typename BufferSequence>
        struct buffer_sequence_cardinality
            : std::conditional_t<std::is_same_v<BufferSequence, mutable_buffer> ||
                                     std::is_same_v<BufferSequence, const_buffer>,
                                 one_buffer, multiple_buffers>
        {
        };

        constexpr std::size_t buffer_copy_1(const mutable_buffer &target,
                                            const const_buffer &source)
        {
            std::size_t n = target.size() < source.size() ? target.size() : source.size();
            if (n > 0)
                std::memcpy(target.data(), source.data(), n);
            return n;
        }

        // NOTE: 函数不能偏特化
        // NOTE: 1、one_buffer <- one_buffer
        template <typename TargetIterator, typename SourceIterator>
        constexpr std::size_t buffer_copy(one_buffer /*unused*/, one_buffer /*unused*/,
                                          TargetIterator target_begin,
                                          TargetIterator /*unused*/,
                                          SourceIterator source_begin,
                                          SourceIterator /*unused*/) noexcept
        {
            return buffer_copy_1(*target_begin, *source_begin);
        }
        template <typename TargetIterator, typename SourceIterator>
        constexpr std::size_t buffer_copy(one_buffer /*unused*/, one_buffer /*unused*/,
                                          TargetIterator target_begin,
                                          TargetIterator /*unused*/,
                                          SourceIterator source_begin,
                                          SourceIterator /*unused*/,
                                          std::size_t max_bytes_to_copy) noexcept
        {
            return buffer_copy_1(*target_begin,
                                 io::buffer::buffer(*source_begin, max_bytes_to_copy));
        }

        // NOTE: 2、one_buffer <- multiple_buffers
        template <typename TargetIterator, typename SourceIterator>
        constexpr std::size_t buffer_copy(
            one_buffer /*unused*/, multiple_buffers /*unused*/,
            TargetIterator target_begin, TargetIterator /*unused*/,
            SourceIterator source_begin, SourceIterator source_end,
            const std::size_t k_max_bytes_to_copy =
                std::numeric_limits<std::size_t>::max()) noexcept
        {
            std::size_t total_bytes_copied = 0;
            SourceIterator source_iter = source_begin;

            for (mutable_buffer target_buffer(
                     io::buffer::buffer(*target_begin, k_max_bytes_to_copy));
                 target_buffer.size() && source_iter != source_end; ++source_iter)
            {
                const_buffer source_buffer{*source_iter};
                std::size_t bytes_copied = buffer_copy_1(target_buffer, source_buffer);
                total_bytes_copied += bytes_copied;
                target_buffer += bytes_copied;
            }

            return total_bytes_copied;
        }

        // NOTE: 3、multiple_buffers <- one_buffer
        template <typename TargetIterator, typename SourceIterator>
        constexpr std::size_t buffer_copy(
            multiple_buffers /*unused*/, one_buffer /*unused*/,
            TargetIterator target_begin, TargetIterator target_end,
            SourceIterator source_begin, SourceIterator /*unused*/,
            const std::size_t k_max_bytes_to_copy =
                std::numeric_limits<std::size_t>::max()) noexcept
        {
            std::size_t total_bytes_copied = 0;
            TargetIterator target_iter = target_begin;

            for (const_buffer source_buffer(
                     io::buffer::buffer(*source_begin, k_max_bytes_to_copy));
                 source_buffer.size() && target_iter != target_end; ++target_iter)
            {
                mutable_buffer target_buffer{*target_iter};
                std::size_t bytes_copied = buffer_copy_1(target_buffer, source_buffer);
                total_bytes_copied += bytes_copied;
                source_buffer += bytes_copied;
            }
            return total_bytes_copied;
        }

        // NOTE: 4、multiple_buffers <- multiple_buffers
        template <typename TargetIterator, typename SourceIterator>
        constexpr std::size_t buffer_copy(multiple_buffers /*unused*/,
                                          multiple_buffers /*unused*/,
                                          TargetIterator target_begin,
                                          TargetIterator target_end,
                                          SourceIterator source_begin,
                                          SourceIterator source_end) noexcept
        {
            std::size_t total_bytes_copied = 0;

            TargetIterator target_iter = target_begin;
            std::size_t target_buffer_offset = 0;

            SourceIterator source_iter = source_begin;
            std::size_t source_buffer_offset = 0;

            while (target_iter != target_end && source_iter != source_end)
            {
                auto target_buffer = mutable_buffer(*target_iter) + target_buffer_offset;

                auto source_buffer = const_buffer(*source_iter) + source_buffer_offset;

                std::size_t bytes_copied = buffer_copy_1(target_buffer, source_buffer);
                total_bytes_copied += bytes_copied;

                if (bytes_copied == target_buffer.size())
                {
                    ++target_iter;
                    target_buffer_offset = 0;
                }
                else
                    target_buffer_offset += bytes_copied;

                if (bytes_copied == source_buffer.size())
                {
                    ++source_iter;
                    source_buffer_offset = 0;
                }
                else
                    source_buffer_offset += bytes_copied;
            }

            return total_bytes_copied;
        }
        template <typename TargetIterator, typename SourceIterator>
        constexpr std::size_t buffer_copy(multiple_buffers /*unused*/,
                                          multiple_buffers /*unused*/,
                                          TargetIterator target_begin,
                                          TargetIterator target_end,
                                          SourceIterator source_begin,
                                          SourceIterator source_end,
                                          const std::size_t k_max_bytes_to_copy) noexcept
        {
            std::size_t total_bytes_copied = 0;

            TargetIterator target_iter = target_begin;
            std::size_t target_buffer_offset = 0;

            SourceIterator source_iter = source_begin;
            std::size_t source_buffer_offset = 0;

            while (total_bytes_copied != k_max_bytes_to_copy &&
                   target_iter != target_end && source_iter != source_end)
            {
                auto target_buffer = mutable_buffer(*target_iter) + target_buffer_offset;

                auto source_buffer = const_buffer(*source_iter) + source_buffer_offset;

                std::size_t bytes_copied = buffer_copy_1(
                    target_buffer,
                    io::buffer::buffer(source_buffer,
                                       k_max_bytes_to_copy - total_bytes_copied));
                total_bytes_copied += bytes_copied;

                if (bytes_copied == target_buffer.size())
                {
                    ++target_iter;
                    target_buffer_offset = 0;
                }
                else
                    target_buffer_offset += bytes_copied;

                if (bytes_copied == source_buffer.size())
                {
                    ++source_iter;
                    source_buffer_offset = 0;
                }
                else
                    source_buffer_offset += bytes_copied;
            }
            return total_bytes_copied;
        }
    }; // namespace __detail
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
        return __detail::buffer_copy(
            __detail::buffer_sequence_cardinality<MutableBufferSequence>(),
            __detail::buffer_sequence_cardinality<ConstBufferSequence>(),
            buffer_sequence_begin(dest), buffer_sequence_end(dest),
            buffer_sequence_begin(source), buffer_sequence_end(source), max_size);
    }

    template <class MutableBufferSequence, class ConstBufferSequence>
    constexpr std::size_t buffer_copy(const MutableBufferSequence &dest,
                                      const ConstBufferSequence &source) noexcept
    {
        return __detail::buffer_copy(
            __detail::buffer_sequence_cardinality<MutableBufferSequence>(),
            __detail::buffer_sequence_cardinality<ConstBufferSequence>(),
            buffer_sequence_begin(dest), buffer_sequence_end(dest),
            buffer_sequence_begin(source), buffer_sequence_end(source));
    }
}; // namespace mcs::net::io::buffer