#include "synthesizer.hpp"
#include "wav-file.hpp"
#include "seq-new.hpp"
#include "midi.hpp"

#include <fstream>

#define SAMPLE_RATE 44100
#define TIME2SAMPLE(t) ((std::size_t)(t*SAMPLE_RATE))

int main() {
    Synthesizer synth{SAMPLE_RATE};
	/*Voice *v1 = synth.add_voice({.25, .7, .5, .2}, 400.0, .6, BasicVoice::SAW_OSC);
    AudioBufferContainer b1 = synth.step(TIME2SAMPLE(1.5));
	v1->release();
	v1 = nullptr;
	Voice* v2 = synth.add_voice({.25, .8, .5, .1}, 600.0, .3, BasicVoice::SAW_OSC);
    AudioBufferContainer b2 = synth.step(TIME2SAMPLE(2));
	v2 = nullptr;

    AudioBufferContainer cat = AudioBuffer::merge_buffers(b1.buf, b2.buf);
    if (cat.buf.p_samples == NULL) {
        std::puts("Buffer merging failed");
        return 1;
    }*/

	Sequencer sequencer{125.0};

	/*int instr_saw = sequencer.add_instrument(SeqInstr{ADSRInfo{0.0, .2, 0.7, 0.0}, WaveformVoice::SAW_OSC});

	sequencer.push_entry(SeqEntry{0, 2, instr_saw, 261.63/2, .7}); // C3
	sequencer.push_entry(SeqEntry{2, 2, instr_saw, 261.63/2, .7});
	sequencer.push_entry(SeqEntry{4, 2, instr_saw, 523.25/2, .7}); // C4
	sequencer.push_entry(SeqEntry{6, 2, instr_saw, 523.25/2, .7});
	sequencer.push_entry(SeqEntry{8, 2, instr_saw, 622.25/2, .7}); // Eb4
	sequencer.push_entry(SeqEntry{10, 2, instr_saw, 622.25/2, .7});
	sequencer.push_entry(SeqEntry{12, 2, instr_saw, 523.25/2, .7}); // C4
	sequencer.push_entry(SeqEntry{14, 2, instr_saw, 523.25/2, .7});
	sequencer.push_entry(SeqEntry{16, 2, instr_saw, 415.30/2, .7}); // Ab3
	sequencer.push_entry(SeqEntry{18, 2, instr_saw, 415.30/2, .7});
	sequencer.push_entry(SeqEntry{20, 2, instr_saw, 415.30/2, .7});
	sequencer.push_entry(SeqEntry{22, 2, instr_saw, 523.25/2, .7}); // C4
	sequencer.push_entry(SeqEntry{24, 2, instr_saw, 392.00/2, .7}); // G3
	sequencer.push_entry(SeqEntry{26, 2, instr_saw, 392.00/2, .7});
	sequencer.push_entry(SeqEntry{28, 2, instr_saw, 392.00/2, .7});
	sequencer.push_entry(SeqEntry{30, 2, instr_saw, 523.25/2, .7}); // C4*/

	//sequencer.set_instrument(SqInstr{ADSRInfo{0.0, .2, 0.2, 0.0}, WaveformVoice::SAW_OSC});
	SqInstr saw_instr;
	saw_instr.envelope = {.01, .3, .0, .0};
	AdditiveVoice::shape_sawtooth(saw_instr.shape);
	sequencer.set_instrument(saw_instr);

	// Reference:
	// C3 = 3FEB
	// C4 = 3FF7

	#if 0
	static const SqEvent test_sequence[] = {
	#define NOTE(t, v, p) {0.0, v, p, SqEventType::NOTE_ON}, {t, v, p, SqEventType::NOTE_OFF},
		NOTE(.5, .3, 0x3FEB) // C3
		NOTE(.5, .7, 0x3FEB)
		NOTE(.5, .3, 0x3FF7) // C4
		NOTE(.5, .7, 0x3FF7)
		NOTE(.5, .3, 0x3FFA) // Eb4
		NOTE(.5, .7, 0x3FFA)
		NOTE(.5, .3, 0x3FF7) // C4
		NOTE(.5, .7, 0x3FF7)
		NOTE(.5, .3, 0x3FF3) // Ab3
		NOTE(.5, .7, 0x3FF3)
		NOTE(.5, .3, 0x3FF3)
		NOTE(.5, .7, 0x3FF7) // C4
		NOTE(.5, .3, 0x3FF2) // G3
		NOTE(.5, .7, 0x3FF2)
		NOTE(.5, .3, 0x3FF2)
		NOTE(.5, .7, 0x3FF7) // C4
	#undef NOTE
	};
	sequencer.load_sequence(std::begin(test_sequence), std::end(test_sequence));
	#endif

	std::ifstream seq_file{"test.mid"};
	if (!seq_file.good()) {
		std::cerr << "Error opening the sequence file" << std::endl;
		return 1;
	}

	if (!midi_load_track(seq_file, sequencer)) {
		std::cerr << "Error loading the sequence track" << std::endl;
		return 1;
	}

	seq_file.close();

	auto buffer = sequencer.render(synth);

    std::FILE *f = std::fopen("synth-out.wav", "wb");
    if (!f) {
        std::perror("Error while opening out file");
        return 1;
    }

    //if (save_to_wav(buf, TIME2SAMPLE(3.5), f, SAMPLE_RATE, 2*SAMPLE_RATE, 2))
	if (save_buffer_to_wav(buffer, f)) {
		return 1;
	}

    std::fclose(f);
    return 0;
}

