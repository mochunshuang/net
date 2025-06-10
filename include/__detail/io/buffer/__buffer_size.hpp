#pragma once

#include "./__buffer_sequence.hpp"
#include "./__const_buffer.hpp"
#include "./__mutable_buffer.hpp"
#include <type_traits>

namespace mcs::net::io::buffer
{
    //  Get the total number of bytes in a buffer sequence.
    //  Returns: The total size of all buffers in the sequence, as if computed as follows:
    template <class ConstBufferSequence>
        requires(requires(ConstBufferSequence buf) {
            buffer_sequence_begin(buf);
            buffer_sequence_end(buf);
            const_buffer(*buffer_sequence_begin(buf));
        })
    constexpr std::size_t buffer_size(const ConstBufferSequence &buffers) noexcept
    {
        if constexpr (std::is_same_v<std::decay_t<decltype(buffers)>, const_buffer> ||
                      std::is_same_v<std::decay_t<decltype(buffers)>, mutable_buffer>)
        {
            return buffers.size(); // NOTE: one_buffer
        }
        else
        {
            std::size_t total_size = 0;
            auto i = buffer_sequence_begin(buffers);
            auto end = buffer_sequence_end(buffers);
            for (; i != end; ++i)
            {
                const_buffer b(*i);
                total_size += b.size();
            }
            return total_size;
        }
    }

}; // namespace mcs::net::io::buffer
