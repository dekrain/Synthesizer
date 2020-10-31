#include <cstring>
#include <vector>

#include "synthesizer.hpp"

Synthesizer::~Synthesizer() = default;

std::unique_ptr<AudioBuffer> Synthesizer::step(std::size_t n_samples) {
	std::unique_ptr<AudioBuffer> buf = AudioBuffer::allocate(n_samples, m_sample_rate, 1, 1);
	std::memset(buf->p_samples.get(), 0, n_samples * sizeof(float));
	for (auto voice = m_voices.begin(); voice != m_voices.end(); ) {
		if ((*voice)->step(*buf)) {
			voice = m_voices.erase(voice);
		} else
			++voice;
	}
    m_time += n_samples * m_dt;
    return buf;
}

Voice* Synthesizer::add_voice(const ADSRInfo& adsr, double pitch, double volume, WaveformVoice::OscShape shape) {
	std::unique_ptr<Voice> voice(new WaveformVoice{adsr, pitch, volume, shape});
	if (!voice)
		return nullptr;
	Voice* vp = voice.get();
	m_voices.insert(std::move(voice));
	return vp;
}

Voice* Synthesizer::add_additive_voice(double pitch, double volume, const AdditiveVoice::Shape& shape) {
	std::unique_ptr<Voice> voice(new AdditiveVoice{shape, pitch, volume});
	if (!voice)
		return nullptr;
	Voice* vp = voice.get();
	m_voices.insert(std::move(voice));
	return vp;
}
