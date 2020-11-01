#pragma once

#include <cstddef>
#include <array>
#include <vector>

#include "envelope.hpp"
#include "engine/audio_buffer.hpp"

class Voice {
protected:
    std::vector<ADSRArticulator> articulators;

public:
    virtual ~Voice();
    // Every sample is added to that being in the buffer instead of replacing it
	// @returns Should the voice be killed
    virtual bool step(AudioBuffer& buf) = 0;

    // Steps the articulators
    // Call this after every sample if this is desired
    void step_art(double dt);

	// Doesn't kill the voice, but make it fade out
    // By default releases the articulators;
    // derived classes overwriting this methos should call the base method if this is desired
    virtual void release();

    bool is_killed() const;

    Voice& add_articulator(const ADSRInfo& adsr, double scale, double Voice::* param) & {
        articulators.emplace_back(adsr, scale, param);
        return *this;
    }
};

class WaveformVoice : public Voice {
public:
    enum OscShape {
        SAW_OSC,
        SINE_OSC,
        SQUARE_OSC,
        TRIANGLE_OSC
    };

    double m_phase = 0.0; // Phase is periodic in [0.0, 1.0)
    double m_pitch;
    double m_volume;
private:

    OscShape m_shape;

public:
    WaveformVoice(double pitch, double volume, OscShape osc_shape) {
        m_pitch = pitch;
        m_volume = volume;
        m_shape = osc_shape;
    }

    bool step(AudioBuffer& buf) override;
};

class AdditiveVoice : public Voice {
    public:
        constexpr static std::size_t num_harmonics = 128;

        struct Harmonic {
            //std::size_t order; // Harmonic number; Frequency is f_base * order; order is implicit
            double volume;
            double phase;
        };

        using Shape = std::array<Harmonic, num_harmonics>;
    private:
        std::array<Harmonic, num_harmonics> m_harmonics;

        bool alive = true;
    
    public:
        double m_pitch;
        double m_volume;

        AdditiveVoice(const Shape& shape, double pitch, double volume)
            : m_harmonics{shape}, m_pitch{pitch}, m_volume{volume} {}
        
        bool step(AudioBuffer& buf) override;
        void release() override {
            alive = false;
            Voice::release();
        }

        // Create sawtooth shape
        static void shape_sawtooth(Shape& shape);
};
