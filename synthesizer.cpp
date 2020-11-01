#include <cstring>
#include <vector>

#include "synthesizer.hpp"

Synthesizer::~Synthesizer() = default;

AudioBuffer Synthesizer::step(std::size_t n_samples) {
	AudioBuffer buf (n_samples, m_sample_rate, 1, 1);
	std::memset(buf.p_samples.get(), 0, n_samples * sizeof(float));
	for (auto voice = m_voices.begin(); voice != m_voices.end(); ) {
		if ((*voice)->step(buf)) {
			voice = m_voices.erase(voice);
		} else
			++voice;
	}
    m_time += n_samples * m_dt;
    return buf;
}

Voice& Synthesizer::add_voice(std::unique_ptr<Voice> voice) {
	Voice& r = *voice;
	m_voices.insert(std::move(voice));
	return r;
}
