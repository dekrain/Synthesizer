#include "envelope.hpp"

inline double lerp(double x0, double x1, double y0, double y1, double x) {
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}


double ADSREnvelope::step(double dt) {
    double t = time;
    time += dt;
    if (!pressed) {
        if (t < R)
            return S - t / R * S;
        else return 0.0;
    }
    if (t < A)
        return t / A;
    t -= A;
    if (t < D)
        return 1.0 - t / D + t / D * S;
    return S;
}


double EnvelopeArt::get_value(double pos) {
    // ASSUME. this.total is valid total length

    // 1. Find the point corresponding to the input
    if (pos == 0.0) {
        return start_value;
    }

    // <errors>
    if (pos < 0.0) {
        return 0.0;
    }

    if (pos > length) {
        return 0.0;
    }
    // </errors>

    double cur = 0.0;
    std::size_t i;
    for (i = 0; i < cpoints.size(); ++i) {
        if (cur >= pos)
            break;
        cur += cpoints[i].length;
    }

    // 2. Interpolate for value
    double aval, bval, apos, bpos;
    aval = (i == 0) ? start_value : cpoints[i-1].value;
    bval = cpoints[i].value;

    apos = (i == 0) ? 0.0 : cur - cpoints[i].length;
    bpos = cur;

    // Linear interpolation
    return lerp(apos, bpos, aval, bval, pos);
}
