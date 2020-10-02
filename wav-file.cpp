#include "wav-file.h"

int save_to_wav(const void *buffer, std::size_t buf_len, std::FILE *ofile,
        std::uint32_t block_rate, std::uint32_t byterate, std::uint16_t block_size,
        std::uint16_t bit_depth, std::uint16_t n_channels, std::uint16_t fmt) {
    std::uint32_t buf_size = buf_len * bit_depth/8;
    std::uint32_t total_size = 8 + 4 + 16 + 8 + buf_size;
    std::uint32_t tmp;
    long DBG_start = std::ftell(ofile) + 8;
    std::fwrite("RIFF", 1, 4, ofile);
    std::fwrite(&total_size, 4, 1, ofile);
    std::fwrite("WAVEfmt ", 1, 8, ofile);
    tmp = 16;
    std::fwrite(&tmp, 4, 1, ofile);
    std::fwrite(&fmt, 2, 1, ofile);
    std::fwrite(&n_channels, 2, 1, ofile);
    std::fwrite(&block_rate, 4, 1, ofile);
    std::fwrite(&byterate, 4, 1, ofile);
    std::fwrite(&block_size, 2, 1, ofile);
    std::fwrite(&bit_depth, 2, 1, ofile);
    std::fwrite("data", 1, 4, ofile);
    std::fwrite(&buf_size, 4, 1, ofile);
    std::fwrite(buffer, bit_depth/8, buf_len, ofile);
    long DBG_end = std::ftell(ofile);
    if (DBG_end - DBG_start != (long)total_size) {
        printf("[DEBUG] Encoded wrong total size (%d), should be (%ld)\n", total_size, DBG_end - DBG_start);
        return -1;
    }
    return 0;
}

void convert_buffer(const float *bin, short *bout, std::size_t count) {
    for (std::size_t i = 0; i < count; i++)
        bout[i] = (short)(bin[i]*0x7FFF);
}

int save_buffer_to_wav(const AudioBuffer& buffer, std::FILE *ofile) {
    std::size_t n_samples = buffer.sz_blocks * buffer.s_block_size * buffer.s_channels;
    short *buf = new short[n_samples];
    if (buf == nullptr) {
        std::puts("[DEBUG] Out of memory on allocating sample buffer");
        return -1;
    }
    convert_buffer(buffer.p_samples.get(), buf, n_samples);
    int out = save_to_wav(buf, n_samples, ofile, buffer.i_block_rate,
            2*buffer.i_block_rate*buffer.s_block_size*buffer.s_channels,
            2*buffer.s_block_size*buffer.s_channels, 16, buffer.s_channels, WAV_FMT_PCM);
    delete[] buf;
    return out;
}

