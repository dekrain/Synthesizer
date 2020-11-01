#include <cmath>

#include "voice.hpp"

static const double TAU = 2 * std::acos(-1.0);

Voice::~Voice() {}

void Voice::step_art(double dt) {
    for (auto& art : articulators) {
        art.step(*this, dt);
    }
}

void Voice::release() {
    for (auto& art : articulators) {
        art.release();
    }
}

bool Voice::is_killed() const {
    // TODO: remove_if, erase
    for (auto const& art : articulators) {
        if (!art.is_killed())
            return false;
    }
    return true;
}


bool WaveformVoice::step(AudioBuffer& buf) {
	std::size_t n_samples = buf.sz_blocks;
    float *buffer = buf.p_samples.get();
	double dt = 1.0 / buf.i_block_rate;
    while (n_samples-- > 0) {
        double sample;
        switch (m_shape) {
            case SAW_OSC:
                sample = std::fmod(2 * m_phase, 2.0);
                if (sample >= 1.0)
                    sample -= 2.0;
                break;
            case SINE_OSC:
                sample = std::sin(TAU * m_phase);
                break;
            case SQUARE_OSC:
                sample = std::fmod(m_phase, 1.0) > .5 ? -1.0 : 1.0;
                break;
            case TRIANGLE_OSC:
                sample = 2*std::fmod(2*m_phase, 2.0);
                if (sample >= 1.0) {
                    if (sample < 3.0)
                        sample = 2.0 - sample;
                    else
                        sample -= 4.0;
                }
                break;
            default:
                sample = 0.0;
        }
        sample *= m_volume;
        *buffer++ += sample;
        m_phase += dt * m_pitch;
        step_art(dt);
    }
	return is_killed();
}


bool AdditiveVoice::step(AudioBuffer& buf) {
    if (!alive)
        return true;
    std::size_t n_samples = buf.sz_blocks;
    float *buffer = buf.p_samples.get();
    double dt = 1.0 / buf.i_block_rate;
    while (n_samples-- > 0) {
        double sample = 0.0;
        for (std::size_t n = 0; n < num_harmonics; ++n) {
            // bandlimit
            if (2*n * m_pitch < buf.i_block_rate)
                sample += m_harmonics[n].volume * std::sin(TAU * m_harmonics[n].phase);
            m_harmonics[n].phase = std::fmod(m_harmonics[n].phase + dt * m_pitch * n, 1.0);
        }
        sample *= m_volume;
        *buffer++ += sample;
        step_art(dt);
    }
    return false;
}

void AdditiveVoice::shape_sawtooth(AdditiveVoice::Shape& shape) {
    // 0 - D/C offset
    // 1 - fundamental
    // [2..num_voices) - overtones...
    for (std::size_t i = 0; i < AdditiveVoice::num_harmonics; ++i) {
        shape[i] = Harmonic{i == 0 ? 0.0 : 0.8/i, 0.0};
    }
}
