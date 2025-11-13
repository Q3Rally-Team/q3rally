#ifndef PROFILE_SHARED_H
#define PROFILE_SHARED_H

#include "../qcommon/q_shared.h"

#define PROFILE_MAX_NAME 32
#define PROFILE_MAX_GENDER 16
#define PROFILE_MAX_BIRTHDATE 16
#define PROFILE_MAX_AVATAR 64
#define PROFILE_MAX_COUNTRY 32

typedef struct {
    double distanceKm;
    double fuelUsed;
    int bestLapMs;
    int kills;
    int deaths;
    int wins;
    int losses;
    int flagCaptures;
    int flagAssists;
} profile_stats_t;

typedef struct {
    char gender[PROFILE_MAX_GENDER];
    char birthDate[PROFILE_MAX_BIRTHDATE];
    char avatar[PROFILE_MAX_AVATAR];
    char country[PROFILE_MAX_COUNTRY];
} profile_info_t;

#endif /* PROFILE_SHARED_H */
