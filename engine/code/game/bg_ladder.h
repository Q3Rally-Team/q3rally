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
#define LADDER_MAX_VALIDATION_REASON    128

typedef enum ladderPayloadIssue_e {
        LADDER_PAYLOAD_WARN_FORBIDDEN_MODE_FIELDS = 1 << 0,
        LADDER_PAYLOAD_WARN_KD_RATIO_REPAIRED     = 1 << 1,
        LADDER_PAYLOAD_WARN_LAPCOUNT_REPAIRED     = 1 << 2,

        LADDER_PAYLOAD_ERR_MISSING_REQUIRED       = 1 << 8,
        LADDER_PAYLOAD_ERR_VALUE_RANGE            = 1 << 9,
        LADDER_PAYLOAD_ERR_INTERNAL_CONSISTENCY   = 1 << 10
} ladderPayloadIssue_t;

#ifndef RACE_MAX_RECORDED_LAPS
#define RACE_MAX_RECORDED_LAPS          LADDER_MAX_LAP_TIMES
#endif

/* Snapshot of a player's career profile, attached to the match payload.
 * Only populated for the local client – remote players don't have their
 * profile data available on the server. */
typedef struct ladderProfileSnapshot_s {
        qboolean        valid;
        int             snapshotEpoch;
        int             snapshotRevision;
        /* ── Allgemein ─────────────────────────────────────────────────── */
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
        float           distanceKm;
        float           topSpeedKph;
        float           fuelUsed;
        char            mostUsedVehicle[PROFILE_MAX_VEHICLE];
        int             gamesPlayed;
        int             achievementTiers[LADDER_ACHIEVEMENT_CATEGORY_COUNT];

        /* ── GT_RACING ──────────────────────────────────────────────────── */
        int             racingWins;
        int             racingPodiums;
        int             racingCompleted;
        int             racingTotalMs;

        /* ── GT_RACING_DM ───────────────────────────────────────────────── */
        int             racingDmWins;
        int             racingDmPodiums;
        int             racingDmCompleted;
        int             racingDmTotalMs;

        /* ── GT_SPRINT ──────────────────────────────────────────────────── */
        int             sprintWins;
        int             sprintCompleted;
        int             sprintBestMs;

        /* ── GT_ELIMINATION ─────────────────────────────────────────────── */
        int             eliminationWins;
        int             eliminationCompleted;
        int             eliminationTotalRoundsLasted;

        /* ── GT_LCS ─────────────────────────────────────────────────────── */
        int             lcsWins;
        int             lcsCompleted;
        int             lcsTotalSurvivalMs;

        /* ── GT_DERBY ───────────────────────────────────────────────────── */
        int             derbyWins;
        int             derbyCompleted;
        int             derbyKills;

        /* ── GT_DEATHMATCH ──────────────────────────────────────────────── */
        int             dmWins;
        int             dmCompleted;
        int             dmKills;

        /* ── GT_CTF ─────────────────────────────────────────────────────── */
        int             ctfWins;
        int             ctfCompleted;
        int             ctfCaptures;

        /* ── GT_CTF4 ────────────────────────────────────────────────────── */
        int             ctf4Wins;
        int             ctf4Completed;
        int             ctf4Captures;

        /* ── GT_TEAM ────────────────────────────────────────────────────── */
        int             teamWins;
        int             teamCompleted;
        int             teamKills;

        /* ── GT_TEAM_RACING ─────────────────────────────────────────────── */
        int             teamRacingWins;
        int             teamRacingCompleted;
        int             teamRacingPodiums;

        /* ── GT_TEAM_RACING_DM ──────────────────────────────────────────── */
        int             teamRacingDmWins;
        int             teamRacingDmCompleted;
        int             teamRacingDmPodiums;

        /* ── GT_DOMINATION ──────────────────────────────────────────────── */
        int             dominationWins;
        int             dominationCompleted;
        int             dominationZoneHoldMs;

        /* ── GT_KOTH ────────────────────────────────────────────────────── */
        int             kothWins;
        int             kothCompleted;
        int             kothZoneHoldMs;
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
        qboolean        profileAttached; /* profile block explicitly attached (valid true/false) */
        ladderProfileSnapshot_t profile;   /* career snapshot, valid only for local client */
} ladderPlayerPayload_t;

typedef struct ladderMatchPayload_s {
        qboolean        valid;
        int                     validationWarnings;
        int                     validationErrors;
        char            validationReason[LADDER_MAX_VALIDATION_REASON];
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
