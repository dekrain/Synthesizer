#include "midi.hpp"

#include <cstdint>

// Utilities
inline void mid_read_u32(std::istream& s, std::uint32_t& v) {
    s.read(reinterpret_cast<char*>(&v), 4);
    v = ((0xFF000000 & v) >> 24) | ((0xFF0000 & v) >> 8) | ((0xFF00 & v) << 8) | ((0xFF & v) << 24);
}

inline void mid_read_u16(std::istream& s, std::uint16_t& v) {
    s.read(reinterpret_cast<char*>(&v), 2);
    v = ((0xFF00 & v) >> 8) | ((0xFF & v) << 8);
}

inline void mid_read_u8(std::istream& s, std::uint8_t& v) {
    s.read(reinterpret_cast<char*>(&v), 1);
}

inline void mid_read_value(std::istream& s, std::uint32_t& v) {
    std::uint8_t tmp;
    v = 0;
    do {
        tmp = s.get();
        v = (v << 7) | (tmp & 0x7F);
    } while (tmp & 0x80);
}

inline void mid_skip(std::istream& s, std::size_t l) {
    s.seekg(l, std::ios::cur);
}

inline unsigned short mid_note(std::uint8_t n) {
    // A4 (440Hz) = 0x4000
    // A4 midi = 69
    return (short)n - 69 + 0x4000;
}

inline double mid_delta(std::uint32_t d, std::uint16_t div) {
    // Translate MIDI tick to beats
    // There are {div} tick in each beat
    return (double)d / div;
}

inline void d_log(const char* s) {
    std::cout << "Debug: " << s << std::endl;
}


bool midi_load_track(std::istream& in_stream, Sequencer& seq) {
    // Header
    std::uint32_t mid_signature;
    mid_read_u32(in_stream, mid_signature);
    if (mid_signature != 0x4D546864)
        return false;

    std::uint32_t mid_hdr_length;
    std::uint16_t mid_hdr_format;
    mid_read_u32(in_stream, mid_hdr_length);
    mid_read_u16(in_stream, mid_hdr_format);

    std::uint16_t mid_tracks;
    std::uint16_t mid_division; // ticks per beat
    mid_read_u16(in_stream, mid_tracks);
    mid_read_u16(in_stream, mid_division);

    // Concat all tracks
    seq.clear();

    for (std::size_t track_id = 0; track_id < mid_tracks; ++track_id) {
        std::uint32_t track_signature;
        std::uint32_t track_length;
        mid_read_u32(in_stream, track_signature);
        mid_read_u32(in_stream, track_length);
        if (track_signature != 0x4D54726B)
            return false;

        bool reading_events = true;
        std::uint8_t ev_last_status = 0;
        while (reading_events) {
            std::uint32_t ev_dt;
            std::uint8_t ev_status;

            mid_read_value(in_stream, ev_dt);
            mid_read_u8(in_stream, ev_status);

            if (ev_status & 0x80) {
                ev_last_status = ev_status;
            } else {
                ev_status = ev_last_status;
                in_stream.unget();
            }

            if ((ev_status & 0xF0) == 0x90) { // Note ON
                [[maybe_unused]]std::uint8_t n_channel = ev_status & 0xF;
                std::uint8_t n_note = in_stream.get();
                std::uint8_t n_velocity = in_stream.get();

                seq.emplace_event(mid_delta(ev_dt, mid_division), n_velocity / 127.0, mid_note(n_note), SqEventType::NOTE_ON);
            } else if ((ev_status & 0xF0) == 0x80) { // Note OFF
                [[maybe_unused]]std::uint8_t n_channel = ev_status & 0xF;
                std::uint8_t n_note = in_stream.get();
                std::uint8_t n_velocity = in_stream.get();

                seq.emplace_event(mid_delta(ev_dt, mid_division), n_velocity / 127.0, mid_note(n_note), SqEventType::NOTE_OFF);
            } else if ((ev_status & 0xF0) == 0xA0) { // Aftertouch
                mid_skip(in_stream, 2);
            } else if ((ev_status & 0xF0) == 0xB0) { // Control Change
                mid_skip(in_stream, 2);
            } else if ((ev_status & 0xF0) == 0xC0) { // Program Change
                mid_skip(in_stream, 1);
            } else if ((ev_status & 0xF0) == 0xD0) { // Channel Pressure
                mid_skip(in_stream, 1);
            } else if ((ev_status & 0xF0) == 0xE0) { // Pitch bend
                mid_skip(in_stream, 2);
            } else if ((ev_status & 0xF0) == 0xF0) { // System exclusive
                if (ev_status == 0xF0) {
                    std::cout << "Notice: System exclusive message" << std::endl;
                    return false;
                } else if (ev_status == 0xF7) {
                    std::cout << "Notice: System exclusive message" << std::endl;
                    return false;
                } else if (ev_status == 0xFF) {
                    std::uint8_t m_type;
                    std::uint32_t m_length;
                    mid_read_u8(in_stream, m_type);
                    mid_read_value(in_stream, m_length);

                    switch (m_type) {
                        case 0x51: {
                            std::uint32_t tempo = 0;
                            tempo = (tempo << 8) | in_stream.get();
                            tempo = (tempo << 8) | in_stream.get();
                            tempo = (tempo << 8) | in_stream.get();
                            // microseconds per beat
                            seq.set_tempo(60000000.0 / tempo);
                            break;
                        }
                        case 0x2F:
                            // End the track
                            reading_events = false;
                            break;
                        default:
                            mid_skip(in_stream, m_length);
                            break;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    return true;
}
