#ifndef BG_LADDER_H
#define BG_LADDER_H

#include "../qcommon/q_shared.h"
#include "profile_shared.h"

/* Number of achievement categories. Mirrors BG_ACHIEVEMENT_CATEGORY_COUNT
 * from bg_achievements.h without pulling in that header in the server build
 * (where bg_achievements.c is not linked). Must stay in sync. */
#define LADDER_ACHIEVEMENT_CATEGORY_COUNT 11

#define LADDER_MAX_MATCH_ID             64
#define LADDER_MAX_MODE                 32
#define LADDER_MAX_TIME_STRING          32
#define LADDER_MAX_SERVER_NAME          64
#define LADDER_MAX_SERVER_HOST          96
#define LADDER_MAX_SERVER_BUILD         32
#define LADDER_MAX_PLAYER_ID            96
#define LADDER_MAX_GUID                 64
#define LADDER_MAX_MODEL                MAX_QPATH
#define LADDER_MAX_VEHICLE              MAX_QPATH
#define LADDER_MAX_LAP_TIMES            32

#ifndef RACE_MAX_RECORDED_LAPS
#define RACE_MAX_RECORDED_LAPS          LADDER_MAX_LAP_TIMES
#endif

/* Snapshot of a player's career profile, attached to the match payload.
 * Only populated for the local client – remote players don't have their
 * profile data available on the server. */
typedef struct ladderProfileSnapshot_s {
        qboolean        valid;                          /* qtrue if this snapshot was filled */
        int             playerScore;
        int             currentRank;
        int             highestRank;
        int             wins;
        int             losses;
        int             kills;
        int             deaths;
        int             flagCaptures;
        int             flagAssists;
        int             bestLapMs;
        int             accuracyAwards;
        int             excellentAwards;
        int             impressiveAwards;
        int             perfectAwards;
        int             damageDealt;
        int             damageTaken;
        /* Keep telemetry fields as float to avoid 8-byte alignment padding
         * differences between QVM and native server builds. */
        float           distanceKm;
        float           topSpeedKph;
        float           fuelUsed;
        char            mostUsedVehicle[PROFILE_MAX_VEHICLE];
        /* Achievement progress per category (unlocked tier count, 0 = none) */
        int             achievementTiers[LADDER_ACHIEVEMENT_CATEGORY_COUNT];
} ladderProfileSnapshot_t;

typedef struct ladderPlayerPayload_s {
        int                     clientNum;
        char            playerId[LADDER_MAX_PLAYER_ID];
        char            guid[LADDER_MAX_GUID];
        char            name[MAX_NAME_LENGTH];
        char            cleanName[MAX_NAME_LENGTH];
        char            model[LADDER_MAX_MODEL];
        char            vehicle[LADDER_MAX_VEHICLE];
        int                     team;
        qboolean        isBot;
        int                     score;
        int                     ping;
        int                     time;
        int                     scoreFlags;
        int                     powerUps;
        int                     accuracy;
        int                     impressiveCount;
        int                     impressiveTelefragCount;
        int                     excellentCount;
        int                     gauntletCount;
        int                     defendCount;
        int                     assistCount;
        qboolean        perfect;
        int                     captures;
        int                     damageDealt;
        int                     damageTaken;
        int                     position;
        int                     bestLapMs;
        int                     totalRaceMs;
        int                     lapCount;
        int                     lapTimes[LADDER_MAX_LAP_TIMES];
        int                     kills;
        int                     deaths;
        int                     zoneHoldMs;
        int                     zoneActiveSigil;
        int                     survivalMs;
        int                     eliminationRound;
        int                     eliminationPlayersRemaining;
        float           eliminationMetric;
        int                     finishRaceTime;
        float           kdRatio;
        ladderProfileSnapshot_t profile;   /* career snapshot, valid only for local client */
} ladderPlayerPayload_t;

typedef struct ladderMatchPayload_s {
        qboolean        valid;
        char            matchId[LADDER_MAX_MATCH_ID];
        char            mode[LADDER_MAX_MODE];
        int                     gametype;
        char            mapName[MAX_QPATH];
        char            startTimeIso[LADDER_MAX_TIME_STRING];
        char            endTimeIso[LADDER_MAX_TIME_STRING];
        char            durationIso[LADDER_MAX_TIME_STRING];
        int                     startEpoch;
        int                     endEpoch;
        int                     durationSeconds;
        char            serverName[LADDER_MAX_SERVER_NAME];
        char            serverHost[LADDER_MAX_SERVER_HOST];
        char            serverBuild[LADDER_MAX_SERVER_BUILD];
        int                     levelStartTime;
        int                     levelEndTime;
        int                     raceStartTime;
        int                     raceEndTime;
        int                     finishRaceTime;
        int                     winnerClientNum;
        int                     numberOfLaps;
        qboolean        trackReversed;
        int                     eliminationStartDelay;
        int                     eliminationInterval;
        int                     eliminationWarning;
        int                     teamScores[TEAM_NUM_TEAMS];
        int                     teamTimes[TEAM_NUM_TEAMS];
        int                     playerCount;
        qboolean        isDedicated;
        ladderPlayerPayload_t players[MAX_CLIENTS];
} ladderMatchPayload_t;

#endif // BG_LADDER_H
