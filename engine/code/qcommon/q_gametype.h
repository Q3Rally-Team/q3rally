/*
 * Q3Rally game mode identifiers shared by the game modules and engine botlib.
 * Keep explicit values: map/entity filtering and server protocols use them.
 */
#ifndef Q_GAMETYPE_H
#define Q_GAMETYPE_H

typedef enum {
        // Non-team gametypes (0-15). Values 9-15 are reserved.
        GT_RACING          =  0,    // racing
        GT_RACING_DM       =  1,    // racing with weapons
        GT_SINGLE_PLAYER   =  2,    // single player / time trial
        GT_DERBY           =  3,    // demolition derby
        GT_LCS             =  4,    // last car standing
        GT_ELIMINATION     =  5,    // elimination race
        GT_DEATHMATCH      =  6,    // free-for-all deathmatch
        GT_SPRINT          =  7,    // point-to-point sprint race
        GT_GHOST           =  8,    // solo ghost race, no bots

        // Team gametypes (16+). Values 24+ are reserved.
        GT_TEAM            = 16,    // team deathmatch
        GT_TEAM_RACING     = 17,    // team racing
        GT_TEAM_RACING_DM  = 18,    // team racing with weapons
        GT_CTF             = 19,    // capture the flag
        GT_CTF4            = 20,    // four-team capture the flag
        GT_DOMINATION      = 21,    // domination
        GT_KOTH            = 22,    // king of the hill
        GT_AUTOBALL        = 23,    // car football

        GT_MAX_GAME_TYPE
} gametype_t;

#endif
