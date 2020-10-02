#pragma once

#include <cstdio>
#include "engine/audio_buffer.h"

#define WAV_FMT_PCM 0x0001
#define WAV_FMT_FLOAT 0x0003

int save_to_wav(const void *buffer, std::size_t buf_len, std::FILE *ofile,
        std::uint32_t block_rate, std::uint32_t byterate, std::uint16_t block_size,
        std::uint16_t bit_depth=16, std::uint16_t n_channels=1, std::uint16_t fmt=WAV_FMT_PCM);

// Save Audio Buffer using 16-bit integer sample format
int save_buffer_to_wav(const AudioBuffer& buffer, std::FILE *ofile);

// TODO: Create WAV file stream to send multiple packets

