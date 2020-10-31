#include <cstring>
#include "audio_buffer.hpp"

AudioBuffer AudioBuffer::merge_buffers(const AudioBuffer& b1, const AudioBuffer& b2) {
    if (b1.i_block_rate != b2.i_block_rate
    ||  b1.s_block_size != b2.s_block_size
    ||  b1.s_channels != b2.s_channels) {
        return AudioBuffer{}; // Error!
    }
    AudioBuffer buf{b1.sz_blocks + b2.sz_blocks, b1.i_block_rate, b1.s_block_size, b1.s_channels};
    std::size_t s1 = b1.sz_blocks * b1.s_block_size * b1.s_channels;
    std::size_t s2 = b2.sz_blocks * b2.s_block_size * b2.s_channels;
    std::memcpy(buf.p_samples.get(), b1.p_samples.get(), s1 * sizeof(float));
    std::memcpy(&buf.p_samples[s1], b2.p_samples.get(), s2 * sizeof(float));
    return buf;
}

