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

typedef struct profile_rank_def_s {
    const char *name;
    int minimumScore;
} profile_rank_def_t;

#define PROFILE_RANK_TABLE( ENTRY )       \
    ENTRY( "Rookie Driver", 0 )          \
    ENTRY( "Street Newbie", 50 )         \
    ENTRY( "Weekend Racer", 150 )        \
    ENTRY( "Track Learner", 350 )        \
    ENTRY( "Amateur Racer", 700 )        \
    ENTRY( "Street Racer", 1200 )        \
    ENTRY( "Semi-Pro Driver", 2000 )     \
    ENTRY( "Pro Racer", 3200 )           \
    ENTRY( "Elite Driver", 5000 )        \
    ENTRY( "Track Dominator", 7500 )     \
    ENTRY( "Master of Speed", 11000 )    \
    ENTRY( "Racing Legend", 16000 )      \
    ENTRY( "Asphalt King", 22000 )       \
    ENTRY( "Grand Champion", 30000 )     \
    ENTRY( "Q3Rally Icon", 40000 )

typedef struct profile_stats_s {
    double distanceKm;
    double fuelUsed;
    int bestLapMs;
    int kills;
    int deaths;
    int wins;
    int playerScore;
    int sprintWins;
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

typedef struct profile_favorite_car_s {
    char model[PROFILE_MAX_FAVORITE_FIELD];
    char skin[PROFILE_MAX_FAVORITE_FIELD];
    char rim[PROFILE_MAX_FAVORITE_FIELD];
    char head[PROFILE_MAX_FAVORITE_FIELD];
} profile_favorite_car_t;

typedef struct profile_info_s {
    char name[PROFILE_MAX_NAME];
    char gender[PROFILE_MAX_GENDER];
    char birthDate[PROFILE_MAX_BIRTHDATE];
    char avatar[PROFILE_MAX_AVATAR];
    char country[PROFILE_MAX_COUNTRY];
    int  currentRank;
    int  highestRank;
    profile_favorite_car_t favoriteCars[PROFILE_MAX_FAVORITE_CARS];
} profile_info_t;

typedef struct profile_rank_s {
    int index;
    const profile_rank_def_t *current;
    const profile_rank_def_t *next;
} profile_rank_t;

#ifndef PROFILE_SHARED_IMPLEMENTATION
qboolean Profile_GetRankForScore( const profile_stats_t *stats,
                                 const profile_rank_def_t *rankDefs,
                                 int rankDefCount,
                                 profile_rank_t *outRank );
#endif

#ifdef PROFILE_SHARED_IMPLEMENTATION
static ID_INLINE qboolean Profile_GetRankForScore( const profile_stats_t *stats,
                                                  const profile_rank_def_t *rankDefs,
                                                  int rankDefCount,
                                                  profile_rank_t *outRank ) {
    int i;
    const profile_rank_def_t *current;
    const profile_rank_def_t *next;
    int currentIndex;

    if ( !stats || !rankDefs || rankDefCount <= 0 || !outRank ) {
        return qfalse;
    }

    current = &rankDefs[0];
    next = NULL;
    currentIndex = 0;

    for ( i = 0; i < rankDefCount; ++i ) {
        if ( stats->playerScore >= rankDefs[i].minimumScore ) {
            current = &rankDefs[i];
            currentIndex = i;
        } else {
            next = &rankDefs[i];
            break;
        }
    }

    outRank->index = currentIndex;
    outRank->current = current;
    outRank->next = next;

    return qtrue;
}
#endif /* PROFILE_SHARED_IMPLEMENTATION */

#endif /* PROFILE_SHARED_H */
