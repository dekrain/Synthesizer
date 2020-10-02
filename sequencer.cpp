#include "sequncer.h"

namespace std {
	template<> struct hash<SeqExtendedEntry> {
		std::size_t operator()(const SeqExtendedEntry& entry) const noexcept {
			return std::hash<const SeqEntry*>{}(&entry.entry);
		}
	};
}

bool operator==(const SeqExtendedEntry& l, const SeqExtendedEntry& r) {
	return &l.entry == &r.entry && l.voice == r.voice;
}

/*std::unique_ptr<AudioBuffer> Sequencer::render(Synthesizer& synth) {
	unsigned short time = 0; // In 1/4 beats
	std::size_t n_samples = synth.get_sample_rate() / m_tempo * 60/4; // TODO: Fix quantization
	std::unordered_set<SeqExtendedEntry> active_notes;
	std::unique_ptr<AudioBuffer> curr_audio = synth.step(0);
	while (time < max_time) {
		for (auto note = active_notes.begin(); note != active_notes.end(); ) {
			if (time >= note->entry.time_start + note->entry.time_length) {
				note->voice->release();
				note = active_notes.erase(note);
			} else
				note++;
		}

		for (SeqEntry& note : m_entries) {
			if (note.time_start == time) {
				const SeqInstr& instr = m_instruments[note.instr_id];
				active_notes.emplace(note, instr, synth.add_voice(instr.envelope, note.pitch, note.volume, instr.osc_shape));
			}
		}

		{
			std::unique_ptr<AudioBuffer> tmp_audio = synth.step(n_samples);
			curr_audio = std::make_unique<AudioBuffer>(std::move(AudioBuffer::merge_buffers(*curr_audio, *tmp_audio)));

			if (!curr_audio->is_valid()) {
				std::puts("Render failed");
				return nullptr;
			}
		}

		time++;
		if ((time & 3) == 0)
			std::putchar('X');
		else
			std::putchar('O');
	}
	return curr_audio;
}*/
