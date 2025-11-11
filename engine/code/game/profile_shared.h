#ifndef PROFILE_SHARED_H
#define PROFILE_SHARED_H

#include "../qcommon/q_shared.h"

#define PROFILE_MAX_NAME 32

typedef struct {
    double distanceKm;
    double fuelUsed;
    int bestLapMs;
    int kills;
    int deaths;
    int wins;
    int losses;
    int flagCaptures;
} profile_stats_t;

#endif /* PROFILE_SHARED_H */
