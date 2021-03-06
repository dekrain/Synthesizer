#include "seq-new.hpp"

#include <map>
#include <cmath>

#include <iostream>

// 0x4000 is A4 (440Hz)
// delta 1 = 1 semitone
double note_pitch(unsigned short note) {
    return 440.0 * std::pow(2.0, (double)((short)note - 0x4000) / 12);
}

AudioBuffer Sequencer::render(Synthesizer& synth) {
    std::cout << "Tempo is " << m_tempo << std::endl;

    double time = 0.0;
    std::size_t n_samples = synth.get_sample_rate() / m_tempo * 60; // Samples per unit of time
    AudioBuffer buf = synth.step(0);
    std::map<unsigned short, Voice*> active_notes;

    std::size_t events_processed = 0;
    for (auto const& ev : m_events) {
        if (ev.dt != 0.0) { // Optimize if at the same time
			auto tmp_audio = synth.step((std::size_t)(n_samples * ev.dt));
			buf = AudioBuffer::merge_buffers(buf, tmp_audio);

			if (!buf.is_valid()) {
				std::puts("Render failed");
				std::exit(1);
			}
        }

        // Check if the note is already on
        auto n = active_notes.find(ev.note);
        if (n != active_notes.end()) {
            n->second->release();
            active_notes.erase(ev.note);
        }

        if (ev.type == SqEventType::NOTE_ON) {
            active_notes.emplace(ev.note,
                /*&synth.add_voice(std::make_unique<WaveformVoice>(m_instrument.envelope, note_pitch(ev.note), ev.volume, m_instrument.osc_shape))*/
                & synth.add_voice(std::make_unique<AdditiveVoice>(m_instrument.shape, note_pitch(ev.note), ev.volume))
                . add_articulator(m_instrument.envelope, ev.volume, static_cast<double Voice::*>(&AdditiveVoice::m_volume))
            );
        }

        time += ev.dt;
        ++events_processed;
    }

    std::cout << "Events processed: " << events_processed << '\n' << "Total length: " << (double)buf.sz_blocks / buf.i_block_rate << std::endl;
    return buf;
}
