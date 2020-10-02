#pragma once

#include <vector>

#include "synthesizer.h"

// New MIDI-like sequencer!

enum class SqEventType {
    NOTE_ON,
    NOTE_OFF
};

struct SqEvent {
    double dt;
    double volume; // velocity
    unsigned short note;
    SqEventType type;
};

struct SqInstr {
    /*ADSRInfo envelope;
	WaveformVoice::OscShape osc_shape;*/
    AdditiveVoice::Shape shape;
};

// For now, monotrack
class Sequencer {
    double m_tempo;
    double m_length;
    std::vector<SqEvent> m_events;
    SqInstr m_instrument;

public:
	Sequencer(double tempo) : m_tempo{tempo}, m_length{0.0} {}

    void set_tempo(double new_tempo) {m_tempo = new_tempo; }

    void set_instrument(const SqInstr& instr) {
        m_instrument = instr;
    }

    template <class It>
    void load_sequence(It start, It end) {
        m_length = 0.0;
        m_events.clear();
        while (start != end) {
            const SqEvent& ev = *start;
            m_length += ev.dt;
            m_events.push_back(ev);
            ++start;
        }
    }

    void clear() {
        m_length = 0.0;
        m_events.clear();
    }

    void add_event(SqEvent ev) {
        m_events.push_back(ev);
    }

    void emplace_event(double dt, double volume, unsigned short note, SqEventType type) {
        m_events.push_back(SqEvent{dt, volume, note, type});
    }

    std::unique_ptr<AudioBuffer> render(Synthesizer& synth);
};
