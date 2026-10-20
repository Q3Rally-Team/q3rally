#include "g_profile_speed.h"

/* Return the minimum reading in a continuous 250 ms window. The first sample
 * establishes the start; elapsed time before that sample is never credited.
 * NaN/infinity and physics spikes are rejected without becoming records. */
double G_Profile_SustainedSpeed( profile_speed_window_t *window,
    double speedKph, int now, int valid ) {
    int delta;
    double result;
    if ( !valid || !( speedKph >= 0.0 && speedKph <= 400.0 ) ) {
        window->active = 0;
        return 0.0;
    }
    delta = now - window->lastTime;
    if ( !window->active || delta < 0 || delta > 250 ) {
        window->active = 1;
        window->lastTime = now;
        window->elapsed = 0;
        window->minimum = speedKph;
        return 0.0;
    }
    window->lastTime = now;
    if ( speedKph < window->minimum ) {
        window->minimum = speedKph;
    }
    window->elapsed += delta;
    if ( window->elapsed < 250 ) {
        return 0.0;
    }
    result = window->minimum;
    window->elapsed = 0;
    window->minimum = speedKph;
    return result;
}
