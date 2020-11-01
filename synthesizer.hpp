#pragma once

#include <unordered_set>
#include <memory>

#include "voice.hpp"
#include "envelope.hpp"

class Synthesizer {
    std::size_t m_sample_rate;
    double m_dt;
    double m_time;
    std::unordered_set<std::unique_ptr<Voice>> m_voices;

public:
    Synthesizer(std::size_t sample_rate) {
        m_sample_rate = sample_rate;
        m_dt = 1.0/sample_rate;
        m_time = 0.0;
    }

    ~Synthesizer();

	std::size_t get_sample_rate() { return m_sample_rate; }

    Voice& add_voice(std::unique_ptr<Voice> voice);

	//Voice* add_voice(const ADSRInfo& adsr, double pitch, double volume, WaveformVoice::OscShape shape);
    //Voice* add_additive_voice(double pitch, double volume, const AdditiveVoice::Shape& shape);

    AudioBuffer step(std::size_t n_samples);
};

