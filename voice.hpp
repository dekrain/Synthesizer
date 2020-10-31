#pragma once

#include <cstddef>
#include <array>

#include "envelope.hpp"
#include "engine/audio_buffer.hpp"

class Voice {
public:
    virtual ~Voice() = default;
    // Every sample is added to that being in the buffer instead of replacing it
	// @returns Should the voice be killed
    virtual bool step(AudioBuffer& buf) = 0;

	// Doesn't kill the voice, but make it fade out
    virtual void release() = 0;
};

class ADSRVoice : public Voice {
protected:
    ADSREnvelope m_env;

public:
    virtual void release() override {
        m_env.release();
    }
};

class WaveformVoice : public ADSRVoice {
public:
    enum OscShape {
        SAW_OSC,
        SINE_OSC,
        SQUARE_OSC,
        TRIANGLE_OSC
    };

private:
    double m_phase; // Phase is periodic in [0.0, 1.0)
    double m_pitch;
    double m_volume;

    OscShape m_shape;

public:
    WaveformVoice(const ADSRInfo& adsr, double pitch, double volume, OscShape osc_shape) {
        m_phase = 0.0;
        m_pitch = pitch;
        m_volume = volume;
        m_env = ADSREnvelope{adsr};
        m_shape = osc_shape;
    }

    bool step(AudioBuffer& buf) override;
};

class AdditiveVoice : public Voice {
    public:
        constexpr static std::size_t num_harmonics = 32;

        struct Harmonic {
            //std::size_t order; // Harmonic number; Frequency is f_base * order; order is implicit
            double volume;
            double phase;
        };

        using Shape = std::array<Harmonic, num_harmonics>;
    private:

        std::array<Harmonic, num_harmonics> m_harmonics;

        double m_pitch;
        double m_volume;

        bool alive = true;
    
    public:
        AdditiveVoice(const Shape& shape, double pitch, double volume)
            : m_harmonics{shape}, m_pitch{pitch}, m_volume{volume} {}
        
        bool step(AudioBuffer& buf) override;
        void release() override {
            alive = false;
        }

        // Create sawtooth shape
        static void shape_sawtooth(Shape& shape);
};
