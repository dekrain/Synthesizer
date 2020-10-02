#pragma once

struct ADSRInfo {
    double A, D, S, R;
};

class ADSREnvelope {
    double A, D, S, R;
    double time;
    bool pressed;

public:
    ADSREnvelope() {} // Must initialize afterwards

    ADSREnvelope(const ADSRInfo& adsr)
            : A{adsr.A}, D{adsr.D}, S{adsr.S}, R{adsr.R} {
        time = 0.0;
        pressed = true;
    }

    double step(double dt);
    void release() {
        if (pressed) {
            time = 0.0;
            pressed = false;
        }
    }

	bool is_killed() const {
		return (!pressed) && (time >= R);
	}
};

// EXPERIMENTAL
#include <vector>

// Articulator envelope
/** An envelope is a curve with values ranging from -1.0 to 1.0
 * consisting of control points & special markers
 */

struct EnvCPoint {
    double value;
    double length; // Length beetwen previous and this control point
    //double tension;
    //EnvCKind kind; // Default: linear
};

struct EnvelopeArt {
    double start_value; // Starting value
    double length; // Total length of the curve
    unsigned short cp_decay, cp_sustain; // Index of a special marker, 0 for unpresent
    std::vector<EnvCPoint> cpoints; // Control points

    double get_value(double pos);
};
