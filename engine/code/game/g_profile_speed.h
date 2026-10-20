/* Server-only filter for lifetime speed records; no network layout changes. */
#ifndef G_PROFILE_SPEED_H
#define G_PROFILE_SPEED_H

typedef struct {
    int active;
    int lastTime;
    int elapsed;
    double minimum;
} profile_speed_window_t;

double G_Profile_SustainedSpeed( profile_speed_window_t *window,
    double speedKph, int now, int valid );
#endif
