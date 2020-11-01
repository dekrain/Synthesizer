#pragma once

#include <memory>
#include <cstddef>
#include <cstdint>

// Buffer structure:
// <block: [SAMPLE_CH1] [SAMPLE_CH1] ... [SAMPLE_CH2] [SAMPLE_CH2] ...> <block> ...

struct AudioBuffer {
    std::unique_ptr<float[]> p_samples; // Pointer to the buffer
    std::size_t sz_blocks = 0; // Number of blocks in the buffer
    std::uint32_t i_block_rate = 0; // Number of blocks per second
    std::uint16_t s_block_size = 0; // Number of samples in each block per channel
    std::uint16_t s_channels = 0; // Number of audio channels

    AudioBuffer() = default;

    AudioBuffer(
            std::size_t n_blocks,
            std::uint32_t block_rate,
            std::uint16_t block_size,
            std::uint16_t channels)
            : sz_blocks{n_blocks}, i_block_rate{block_rate},
            s_block_size{block_size}, s_channels{channels} {
                p_samples.reset(new float[sz_blocks * s_block_size * s_channels]);
        }

    AudioBuffer(AudioBuffer&& other) {
        swap(other);
    };

    AudioBuffer& operator=(AudioBuffer&& other) {
        swap(other);
        return *this;
    }

    void swap(AudioBuffer& other) {
        using std::swap;
        swap(p_samples, other.p_samples);
        swap(sz_blocks, other.sz_blocks);
        swap(i_block_rate, other.i_block_rate);
        swap(s_block_size, other.s_block_size);
        swap(s_channels, other.s_channels);
    }

    bool is_valid() const { return p_samples != nullptr; }
    static AudioBuffer merge_buffers(const AudioBuffer& b1, const AudioBuffer& b2);
};
