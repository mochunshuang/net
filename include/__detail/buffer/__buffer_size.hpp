#pragma once

#include "./__buffer_sequence.hpp"

namespace mcs::net::buffer
{
    // Returns: The total size of all buffers in the sequence, as if computed as follows:
    template <class ConstBufferSequence> // TODO(mcs): 概念约束ConstBufferSequence
    constexpr std::size_t buffer_size(const ConstBufferSequence &buffers) noexcept
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

}; // namespace mcs::net::buffer