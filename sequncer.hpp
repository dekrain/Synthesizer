#pragma once

#include <vector>

#include "engine/audio_buffer.h"
#include "synthesizer.h"

struct SeqInstr {
	ADSRInfo envelope;
	WaveformVoice::OscShape osc_shape;
};

struct SeqEntry {
	unsigned short time_start; // In 1/4 beats
	unsigned short time_length;
	int instr_id; // Instrument ID in the sequencer
	double pitch;
	double volume;
};

struct SeqExtendedEntry {
	const SeqEntry& entry;
	const SeqInstr& instr;
	mutable Voice* voice;

	SeqExtendedEntry(const SeqEntry& entry, const SeqInstr& instr, Voice* voice) : entry{entry}, instr{instr}, voice{voice} {}
};

class Sequencer {
	double m_tempo;
	std::vector<SeqInstr> m_instruments;
	std::vector<SeqEntry> m_entries;
	unsigned short max_time = 0;

public:
	Sequencer(double tempo) : m_tempo{tempo} {}

	int add_instrument(const SeqInstr& instr) {
		std::size_t i = m_instruments.size();
		m_instruments.push_back(instr);
		return static_cast<int>(i);
	}

	void push_entry(const SeqEntry& entry) {
		m_entries.push_back(entry);
		unsigned short t = entry.time_start + entry.time_length;
		if (t > max_time)
			max_time = t;
	}

	std::unique_ptr<AudioBuffer> render(Synthesizer& synth);
};