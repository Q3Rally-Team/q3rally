#ifndef PROFILE_SHARED_H
#define PROFILE_SHARED_H

#include "../qcommon/q_shared.h"

#define PROFILE_MAX_NAME 32
#define PROFILE_MAX_GENDER 16
#define PROFILE_MAX_BIRTHDATE 16
#define PROFILE_MAX_AVATAR 64
#define PROFILE_MAX_COUNTRY 32
#define PROFILE_MAX_VEHICLE 64
#define PROFILE_MAX_FAVORITE_CARS 4
#define PROFILE_MAX_FAVORITE_FIELD MAX_QPATH

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
    int accuracyAwards;
    int excellentAwards;
    int impressiveAwards;
    int perfectAwards;
    double topSpeedKph;
    int damageDealt;
    int damageTaken;
    char mostUsedVehicle[PROFILE_MAX_VEHICLE];
    int mostUsedVehicleTimeMs;
} profile_stats_t;

typedef struct {
    char model[PROFILE_MAX_FAVORITE_FIELD];
    char skin[PROFILE_MAX_FAVORITE_FIELD];
    char rim[PROFILE_MAX_FAVORITE_FIELD];
    char head[PROFILE_MAX_FAVORITE_FIELD];
} profile_favorite_car_t;

typedef struct {
    char name[PROFILE_MAX_NAME];
    char gender[PROFILE_MAX_GENDER];
    char birthDate[PROFILE_MAX_BIRTHDATE];
    char avatar[PROFILE_MAX_AVATAR];
    char country[PROFILE_MAX_COUNTRY];
    profile_favorite_car_t favoriteCars[PROFILE_MAX_FAVORITE_CARS];
} profile_info_t;

#endif /* PROFILE_SHARED_H */
