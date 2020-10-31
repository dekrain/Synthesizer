#include "sampler.hpp"

bool SamplerVoice::step(AudioBuffer& buf) {
    size_t n_samples = buf.sz_blocks;
    float* p_out = buf.p_samples.get();
    const float* p_in = m_sample->p_samples.get();
    if (buf.i_block_rate != m_sample->i_block_rate) {
        // XXX
        return false;
    }
    while (n_samples-- > 0) {
        if (++m_time >= m_sample->sz_blocks)
            *p_out = 0.0f;
        else
            *p_out++ = m_volume * *p_in++;
    }
    return m_time >= m_sample->sz_blocks;
}
