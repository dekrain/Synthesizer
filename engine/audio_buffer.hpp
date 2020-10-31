#pragma once

#include <memory>
#include <cstddef>
#include <cstdint>

// Buffer structure:
// <block: [SAMPLE_CH1] [SAMPLE_CH1] ... [SAMPLE_CH2] [SAMPLE_CH2] ...> <block> ...

struct AudioBuffer {
    std::unique_ptr<float[]> p_samples; // Pointer to the buffer
    std::size_t sz_blocks; // Number of blocks in the buffer
    std::uint32_t i_block_rate; // Number of blocks per second
    std::uint16_t s_block_size; // Number of samples in each block per channel
    std::uint16_t s_channels; // Number of audio channels

    AudioBuffer(
            std::size_t n_blocks,
            std::uint32_t block_rate,
            std::uint16_t block_size,
            std::uint16_t channels)
            : sz_blocks{n_blocks}, i_block_rate{block_rate},
            s_block_size{block_size}, s_channels{channels} {
                p_samples.reset(new float[sz_blocks * s_block_size * s_channels]);
        }

    AudioBuffer(AudioBuffer&& obj) :
            p_samples{std::exchange(obj.p_samples, nullptr)},
            sz_blocks{std::exchange(obj.sz_blocks, 0)},
            i_block_rate{std::exchange(obj.i_block_rate, 0)},
            s_block_size{std::exchange(obj.s_block_size, 0)},
            s_channels{std::exchange(obj.s_channels, 0)} {};

    AudioBuffer()
            : p_samples{nullptr}, sz_blocks{0}, i_block_rate{0}, s_block_size{0}, s_channels{0} {}

    bool is_valid() const { return p_samples != nullptr; }
    static AudioBuffer merge_buffers(const AudioBuffer& b1, const AudioBuffer& b2);

    static inline std::unique_ptr<AudioBuffer> allocate(
            std::size_t n_blocks,
            std::uint32_t block_rate,
            std::uint16_t block_size,
            std::uint16_t channels) {
                    return std::make_unique<AudioBuffer>(n_blocks, block_rate, block_size, channels);
            }
};
