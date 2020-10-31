#pragma once

#include "voice.hpp"

class SamplerVoice : public Voice {
    size_t m_time;
    double m_volume;

    std::unique_ptr<AudioBuffer> m_sample;

public:
    SamplerVoice(std::unique_ptr<AudioBuffer>&& sample, double volume = 1.0) : m_sample{std::move(sample)} {
        m_time = 0;
        m_volume = volume;
    }

    bool step(AudioBuffer& buf) override;
};
