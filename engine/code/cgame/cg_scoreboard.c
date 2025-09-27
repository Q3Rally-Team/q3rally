/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2025 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

q3rally source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with q3rally; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/* cg_scoreboard.c -- Adaptive scoreboard design for Q3Rally */
#include "cg_local.h"
#include "../client/keycodes.h"
#ifdef Q3_VM
#include "../game/bg_lib.h"
#else
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#endif

/* Modern scoreboard layout constants */
#define MODERN_SB_Y             120
#define MODERN_SB_WIDTH         650  
#define MODERN_SB_MIN_WIDTH     650  /* Minimum scoreboard width */
#define MODERN_SB_HEADER_HEIGHT 40
#define MODERN_SB_ROW_HEIGHT    36
#define MODERN_SB_COMPACT_HEIGHT 20
#define MODERN_SB_TAB_HEIGHT    28

/* Column definitions - flexible layout */
#define COL_RANK_WIDTH          64
#define COL_AVATAR_WIDTH        40
#define COL_NAME_WIDTH          180
#define COL_SCORE_WIDTH         80   /* For frags/points */
#define COL_DEATHS_WIDTH        60   /* For deaths */
#define COL_LAPTIME_WIDTH       100  /* Best lap time */
#define COL_TOTALTIME_WIDTH     120  /* Total time */
#define COL_PING_WIDTH          60   /* Ping */
#define COL_STATUS_WIDTH        80   /* Status (Ready, etc) */

/* Visual styling */
#define MODERN_SB_ALPHA         0.85f
#define MODERN_SB_BORDER_SIZE   2
#define MODERN_SB_PADDING       8
#define MODERN_SB_CORNER_RADIUS 4

/* Maximum clients display */
#define MAX_SCOREBOARD_CLIENTS  12
#define MAX_COMPACT_CLIENTS     16

/* Ladder tab / layout limits */
#define MAX_LADDER_TABS         6
#define MAX_LADDER_COLUMNS      8
#define MAX_LADDER_DISPLAY_ROWS 16
#define MAX_LOCAL_FALLBACK_ENTRIES 8

/* Scoreboard column types */
typedef enum {
    SBCOL_RANK,
    SBCOL_AVATAR,
    SBCOL_NAME,
    SBCOL_SCORE,      /* Frags/Points */
    SBCOL_DEATHS,     /* Deaths */
    SBCOL_LAPTIME,    /* Best lap time */
    SBCOL_TOTALTIME,  /* Total/Race time */
    SBCOL_PING,       /* Network ping */
    SBCOL_STATUS,     /* Ready status, etc */
    SBCOL_MAX
} sbColumn_t;

/* Column configuration structure */
typedef struct {
    sbColumn_t type;
    int width;
    int x;
    char *header;
    qboolean visible;
} columnConfig_t;

static columnConfig_t columns[SBCOL_MAX];
static int visibleColumns = 0;
static int currentScoreboardWidth = 0;
static int scoreboardX = 0; /* Dynamic X position for centering */
static int contentStartX = 0; /* Starting X position for centered content */
static qboolean localClientDrawn;

/* Tab selection state */
static int scoreboardActiveTab = 0;
static qboolean scoreboardLeftHeld = qfalse;
static qboolean scoreboardRightHeld = qfalse;
static qboolean scoreboardRetryHeld = qfalse;

/* Ladder column handling */
typedef enum {
    LADDER_COL_POSITION = 0,
    LADDER_COL_NAME,
    LADDER_COL_TEAM,
    LADDER_COL_BEST_LAP,
    LADDER_COL_TOTAL_TIME,
    LADDER_COL_LAPS,
    LADDER_COL_SCORE,
    LADDER_COL_FRAGS,
    LADDER_COL_DEATHS,
    LADDER_COL_KD,
    LADDER_COL_DAMAGE_DEALT,
    LADDER_COL_DAMAGE_TAKEN,
    LADDER_COL_CAPTURES,
    LADDER_COL_ASSISTS,
    LADDER_COL_ZONE_HOLD,
    LADDER_COL_SURVIVAL,
    LADDER_COL_ELIM_ROUND,
    LADDER_COL_ELIM_REMAIN,
    LADDER_COL_SOURCE,
    LADDER_COL__COUNT
} ladderColumnType_t;

typedef struct {
    ladderColumnType_t type;
    const char *header;
    int width;
    int align;
} ladderColumnDef_t;

typedef struct {
    char identifier[MAX_LADDER_IDENTIFIER];
    char label[32];
} ladderTabInfo_t;

typedef struct {
    int clientNum;
    int position;
    int bestLapTime;
    int totalTime;
    int score;
    int deaths;
    team_t team;
    char name[MAX_NAME_LENGTH];
} localFallbackEntry_t;

/*
=================
CG_IsRacingGametype
Helper function to determine if current gametype is racing-based
=================
*/
static qboolean CG_IsRacingGametype(void) {
    return (cgs.gametype == GT_RACING ||
            cgs.gametype == GT_TEAM_RACING ||
            cgs.gametype == GT_ELIMINATION ||
            cgs.gametype == GT_RACING_DM ||
            cgs.gametype == GT_TEAM_RACING_DM);
}

/*
=================
CG_IsTeamGametype
Helper function to determine if current gametype is team-based
=================
*/
static qboolean CG_IsTeamGametype(void) {
    return (cgs.gametype == GT_TEAM || 
            cgs.gametype == GT_TEAM_RACING ||
            cgs.gametype == GT_TEAM_RACING_DM ||
            cgs.gametype == GT_CTF ||
            cgs.gametype == GT_CTF4 ||
            cgs.gametype == GT_DOMINATION);
}

/*
=================
CG_InitScoreboardColumns
Initialize column configuration based on gametype - C89 compatible
=================
*/
static void CG_InitScoreboardColumns(void) {
    int i;
    int currentX;
    int totalContentWidth;
    qboolean showScore, showDeaths, showTimes, showLapTimes;
    qboolean isRacing, isTeam;
    
    /* Clear all columns first */
    for (i = 0; i < SBCOL_MAX; i++) {
        columns[i].visible = qfalse;
        columns[i].x = 0;
    }
    
    /* Determine gametype characteristics */
    isRacing = CG_IsRacingGametype();
    isTeam = CG_IsTeamGametype();
    
    /* Determine what to show based on gametype */
    showScore = qfalse;
    showDeaths = qfalse;
    showTimes = qfalse;
    showLapTimes = qfalse;
    
    switch (cgs.gametype) {
        case GT_RACING:
        case GT_TEAM_RACING:
        case GT_ELIMINATION:
            /* Pure racing - only positions and times matter */
            showScore = qfalse;
            showDeaths = qfalse;
            showTimes = qtrue;
            showLapTimes = qtrue;
            break;
            
        case GT_DEATHMATCH:
        case GT_TEAM:
        case GT_CTF:
		case GT_CTF4:
        case GT_DOMINATION:
            /* Pure combat - only frags/score matter */
            showScore = qtrue;
            showDeaths = qtrue;
            break;
            
        case GT_RACING_DM:
        case GT_TEAM_RACING_DM:
            /* Racing with weapons - both matter */
            showScore = qtrue;
            showTimes = qtrue;
            showLapTimes = qtrue;
            /* No deaths in racing modes typically */
            break;
            
        case GT_DERBY:
        case GT_LCS:
            /* Destruction modes - score and survival matter */
            showScore = qtrue;
            showDeaths = qtrue;
            break;
            
        default:
            /* Default fallback */
            showScore = qtrue;
            break;
    }

    /* Hide lap time column for A-to-B style races */
    if (cgs.laplimit <= 1) {
        showLapTimes = qfalse;
    }

    /* Configure base columns (always visible) */
    columns[SBCOL_RANK].type = SBCOL_RANK;
    columns[SBCOL_RANK].width = COL_RANK_WIDTH;
    columns[SBCOL_RANK].header = "POS";
    columns[SBCOL_RANK].visible = qtrue;
    
    columns[SBCOL_AVATAR].type = SBCOL_AVATAR;
    columns[SBCOL_AVATAR].width = COL_AVATAR_WIDTH;
    columns[SBCOL_AVATAR].header = "";
    columns[SBCOL_AVATAR].visible = qtrue;
    
    columns[SBCOL_NAME].type = SBCOL_NAME;
    columns[SBCOL_NAME].width = COL_NAME_WIDTH;
    columns[SBCOL_NAME].header = "PLAYER";
    columns[SBCOL_NAME].visible = qtrue;
    
    /* Configure conditional columns */
    if (showScore) {
        columns[SBCOL_SCORE].type = SBCOL_SCORE;
        columns[SBCOL_SCORE].width = COL_SCORE_WIDTH;
        
        /* Different header based on gametype */
        if (cgs.gametype == GT_CTF) {
            columns[SBCOL_SCORE].header = "CAPS";
        } else if (cgs.gametype == GT_DOMINATION) {
            columns[SBCOL_SCORE].header = "POINTS";
        } else if (isTeam && !isRacing) {
            columns[SBCOL_SCORE].header = "SCORE";
        } else {
            columns[SBCOL_SCORE].header = "FRAGS";
        }
        columns[SBCOL_SCORE].visible = qtrue;
    }
    
    if (showDeaths) {
        columns[SBCOL_DEATHS].type = SBCOL_DEATHS;
        columns[SBCOL_DEATHS].width = COL_DEATHS_WIDTH;
        columns[SBCOL_DEATHS].header = "DMG";
        columns[SBCOL_DEATHS].visible = qtrue;
    }
    
    if (showLapTimes) {
        columns[SBCOL_LAPTIME].type = SBCOL_LAPTIME;
        columns[SBCOL_LAPTIME].width = COL_LAPTIME_WIDTH;
        columns[SBCOL_LAPTIME].header = "BEST";
        columns[SBCOL_LAPTIME].visible = qtrue;
    }
    
    if (showTimes) {
        columns[SBCOL_TOTALTIME].type = SBCOL_TOTALTIME;
        columns[SBCOL_TOTALTIME].width = COL_TOTALTIME_WIDTH;
        
        /* Different header based on racing type */
        if (cgs.gametype == GT_RACING || cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_TEAM_RACING) {
            columns[SBCOL_TOTALTIME].header = "RACE TIME";
        } else {
            columns[SBCOL_TOTALTIME].header = "TOTAL";
        }
        columns[SBCOL_TOTALTIME].visible = qtrue;
    }
    
    /* Status column only in intermission - no ping column */
    if (cg.predictedPlayerState.pm_type == PM_INTERMISSION) {
        columns[SBCOL_STATUS].type = SBCOL_STATUS;
        columns[SBCOL_STATUS].width = COL_STATUS_WIDTH;
        columns[SBCOL_STATUS].header = "STATUS";
        columns[SBCOL_STATUS].visible = qtrue;
    }
    
    /* Calculate actual content width */
    totalContentWidth = 0;
    visibleColumns = 0;
    
    for (i = 0; i < SBCOL_MAX; i++) {
        if (columns[i].visible) {
            totalContentWidth += columns[i].width;
            visibleColumns++;
        }
    }
    
    /* Set scoreboard width to be at least the minimum or the content width */
    currentScoreboardWidth = totalContentWidth;
    if (currentScoreboardWidth < MODERN_SB_MIN_WIDTH) {
        currentScoreboardWidth = MODERN_SB_MIN_WIDTH;
    }
    
    /* Calculate centered X position for the entire scoreboard */
    scoreboardX = (SCREEN_WIDTH - currentScoreboardWidth) / 2;
    
    /* Calculate starting X position for centered content within the scoreboard */
    contentStartX = scoreboardX + (currentScoreboardWidth - totalContentWidth) / 2;
    
    /* Calculate column positions relative to the centered content start */
    currentX = 0;
    for (i = 0; i < SBCOL_MAX; i++) {
        if (columns[i].visible) {
            columns[i].x = contentStartX + currentX;
            currentX += columns[i].width;
        }
    }
}

/*
=================
CG_DrawModernBackground
Draw a modern styled background with subtle gradients
=================
*/
static void CG_DrawModernBackground(int x, int y, int width, int height, 
                                   float alpha, qboolean isHeader) {
    vec4_t bgColor;
    vec4_t borderColor;
    
    /* Initialize colors - C89 style */
    if (isHeader) {
        /* Header background - darker */
        bgColor[0] = 0.12f; bgColor[1] = 0.15f; bgColor[2] = 0.18f; bgColor[3] = alpha;
        borderColor[0] = 0.3f; borderColor[1] = 0.4f; borderColor[2] = 0.5f; borderColor[3] = alpha;
    } else {
        /* Row background - lighter */
        bgColor[0] = 0.08f; bgColor[1] = 0.10f; bgColor[2] = 0.12f; bgColor[3] = alpha * 0.8f;
        borderColor[0] = 0.2f; borderColor[1] = 0.25f; borderColor[2] = 0.3f; borderColor[3] = alpha * 0.6f;
    }
    
    /* Main background */
    CG_FillRect(x, y, width, height, bgColor);
    
    /* Top border */
    CG_FillRect(x, y, width, MODERN_SB_BORDER_SIZE, borderColor);
    
    /* Bottom border */
    CG_FillRect(x, y + height - MODERN_SB_BORDER_SIZE, width, MODERN_SB_BORDER_SIZE, borderColor);
}

/*
=================
CG_DrawModernText
Draw text with modern styling and proper alignment
=================
*/
static void CG_DrawModernText(int x, int y, const char *text, int align, 
                             int columnWidth, float *color, qboolean isBold) {
    int textWidth, drawX;
    
    if (!text) {
        return;
    }
    
    textWidth = CG_DrawStrlen(text) * (isBold ? BIGCHAR_WIDTH : SMALLCHAR_WIDTH);
    
    switch (align) {
        case 0: /* Left align */
            drawX = x + MODERN_SB_PADDING;
            break;
        case 1: /* Center align */
            drawX = x + (columnWidth - textWidth) / 2;
            break;
        case 2: /* Right align */
            drawX = x + columnWidth - textWidth - MODERN_SB_PADDING;
            break;
        default:
            drawX = x;
            break;
    }
    
    if (isBold) {
        if (color) {
            CG_DrawBigStringColor(drawX, y, text, color);
        } else {
            CG_DrawBigString(drawX, y, text, 1.0f);
        }
    } else {
        if (color) {
            CG_DrawSmallStringColor(drawX, y, text, color);
        } else {
            CG_DrawSmallString(drawX, y, text, 1.0f);
        }
    }
}

/*
=================
CG_DrawModernHeader
Draw the adaptive scoreboard header
=================
*/
static void CG_DrawModernHeader(int y) {
    vec4_t headerTextColor;
    int i;
    
    /* Initialize header text color */
    headerTextColor[0] = 0.9f; headerTextColor[1] = 0.9f; 
    headerTextColor[2] = 0.9f; headerTextColor[3] = 1.0f;
    
    /* Draw header background */
    CG_DrawModernBackground(scoreboardX, y, currentScoreboardWidth, MODERN_SB_HEADER_HEIGHT, 
                           MODERN_SB_ALPHA, qtrue);
    
    /* Draw visible column headers */
    for (i = 0; i < SBCOL_MAX; i++) {
        if (!columns[i].visible || !columns[i].header[0]) {
            continue;
        }
        
        switch (columns[i].type) {
            case SBCOL_RANK:
            case SBCOL_SCORE:
            case SBCOL_DEATHS:
            case SBCOL_LAPTIME:
            case SBCOL_TOTALTIME:
            case SBCOL_STATUS:
                /* Center-aligned columns */
                CG_DrawModernText(columns[i].x, y + 8, columns[i].header, 1, 
                                 columns[i].width, headerTextColor, qtrue);
                break;
            case SBCOL_NAME:
                /* Left-aligned text columns */
                CG_DrawModernText(columns[i].x, y + 8, columns[i].header, 0, 
                                 columns[i].width, headerTextColor, qtrue);
                break;
            default:
                break;
        }
    }
}

/*
=================
CG_GetRankColor
Get color based on player rank for visual hierarchy
=================
*/
static void CG_GetRankColor(int rank, vec4_t color) {
    switch (rank) {
        case 1: /* Gold for 1st place */
            color[0] = 1.0f; color[1] = 0.84f; color[2] = 0.0f; color[3] = 1.0f;
            break;
        case 2: /* Silver for 2nd place */
            color[0] = 0.75f; color[1] = 0.75f; color[2] = 0.75f; color[3] = 1.0f;
            break;
        case 3: /* Bronze for 3rd place */
            color[0] = 0.8f; color[1] = 0.5f; color[2] = 0.2f; color[3] = 1.0f;
            break;
        default: /* White for others */
            color[0] = 0.9f; color[1] = 0.9f; color[2] = 0.9f; color[3] = 1.0f;
            break;
    }
}

/*
=================
CG_GetModernTeamColor
Helper function to get team-specific colors
=================
*/
static void CG_GetModernTeamColor(team_t team, vec4_t color) {
    switch (team) {
        case TEAM_RED:
            color[0] = 1.0f; color[1] = 0.2f; color[2] = 0.2f; color[3] = 1.0f;
            break;
        case TEAM_BLUE:
            color[0] = 0.2f; color[1] = 0.2f; color[2] = 1.0f; color[3] = 1.0f;
            break;
        case TEAM_GREEN:
            color[0] = 0.2f; color[1] = 1.0f; color[2] = 0.2f; color[3] = 1.0f;
            break;
        case TEAM_YELLOW:
            color[0] = 1.0f; color[1] = 1.0f; color[2] = 0.2f; color[3] = 1.0f;
            break;
        default:
            color[0] = 0.9f; color[1] = 0.9f; color[2] = 0.9f; color[3] = 1.0f;
            break;
    }
}


static void CG_FormatLadderTabLabel(const char *identifier, char *out, int outSize) {
    char working[MAX_LADDER_IDENTIFIER];
    int i, j;
    qboolean capitalize;

    if (!out || outSize <= 0) {
        return;
    }

    if (!identifier || !identifier[0]) {
        Q_strncpyz(out, "LADDER", outSize);
        return;
    }

    Q_strncpyz(working, identifier, sizeof(working));

    for (i = 0; working[i]; i++) {
        unsigned char ch = (unsigned char)working[i];
        if (ch == '_' || ch == '.' || ch == '-') {
            working[i] = ' ';
        } else {
            working[i] = (char)tolower(ch);
        }
    }

    capitalize = qtrue;
    j = 0;
    for (i = 0; working[i] && j < outSize - 1; i++) {
        char ch = working[i];

        if (ch == ' ') {
            if (!capitalize && j < outSize - 1) {
                out[j++] = ' ';
                capitalize = qtrue;
            }
            continue;
        }

        if (capitalize && ch >= 'a' && ch <= 'z') {
            ch = (char)(ch - 'a' + 'A');
        }
        out[j++] = ch;
        capitalize = qfalse;
    }

    out[j] = '\0';

    if (!out[0]) {
        Q_strncpyz(out, "LADDER", outSize);
    }
}

static int CG_BuildLadderTabs(const cg_ladder_t *ladder, ladderTabInfo_t *tabs, int maxTabs) {
    int count;
    int i, j;
    qboolean hasDefault;

    if (!ladder || !tabs || maxTabs <= 0) {
        return 0;
    }

    count = 0;
    hasDefault = qfalse;

    if (ladder->numEntries <= 0) {
        tabs[0].identifier[0] = '\0';
        CG_FormatLadderTabLabel("", tabs[0].label, sizeof(tabs[0].label));
        return 1;
    }

    for (i = 0; i < ladder->numEntries && count < maxTabs; i++) {
        const cg_ladder_entry_t *entry = &ladder->entries[i];
        const char *identifier = entry->identifier;
        qboolean exists;

        if (!identifier || !identifier[0]) {
            if (!hasDefault && count < maxTabs) {
                tabs[count].identifier[0] = '\0';
                CG_FormatLadderTabLabel("", tabs[count].label, sizeof(tabs[count].label));
                count++;
                hasDefault = qtrue;
            }
            continue;
        }

        exists = qfalse;
        for (j = 0; j < count; j++) {
            if (!Q_stricmp(identifier, tabs[j].identifier)) {
                exists = qtrue;
                break;
            }
        }

        if (exists) {
            continue;
        }

        Q_strncpyz(tabs[count].identifier, identifier, sizeof(tabs[count].identifier));
        CG_FormatLadderTabLabel(identifier, tabs[count].label, sizeof(tabs[count].label));
        count++;
    }

    if (count == 0) {
        tabs[0].identifier[0] = '\0';
        CG_FormatLadderTabLabel("", tabs[0].label, sizeof(tabs[0].label));
        count = 1;
    }

    return count;
}

static void CG_HandleScoreboardTabInput(int tabCount) {
    qboolean leftDown;
    qboolean rightDown;

    if (tabCount <= 1) {
        scoreboardActiveTab = 0;
        scoreboardLeftHeld = qfalse;
        scoreboardRightHeld = qfalse;
        return;
    }

    leftDown = trap_Key_IsDown(K_LEFTARROW) || trap_Key_IsDown(K_KP_LEFTARROW) ||
               trap_Key_IsDown(K_PGUP) || trap_Key_IsDown(K_HOME) ||
               trap_Key_IsDown(K_PAD0_DPAD_LEFT) ||
               trap_Key_IsDown(K_PAD0_LEFTSTICK_LEFT) ||
               trap_Key_IsDown(K_PAD0_RIGHTSTICK_LEFT) ||
               trap_Key_IsDown(K_PAD0_LEFTSHOULDER);
    rightDown = trap_Key_IsDown(K_RIGHTARROW) || trap_Key_IsDown(K_KP_RIGHTARROW) ||
                trap_Key_IsDown(K_PGDN) || trap_Key_IsDown(K_END) ||
                trap_Key_IsDown(K_PAD0_DPAD_RIGHT) ||
                trap_Key_IsDown(K_PAD0_LEFTSTICK_RIGHT) ||
                trap_Key_IsDown(K_PAD0_RIGHTSTICK_RIGHT) ||
                trap_Key_IsDown(K_PAD0_RIGHTSHOULDER);

    if (leftDown && !scoreboardLeftHeld) {
        scoreboardActiveTab--;
        if (scoreboardActiveTab < 0) {
            scoreboardActiveTab = tabCount - 1;
        }
    }

    if (rightDown && !scoreboardRightHeld) {
        scoreboardActiveTab++;
        if (scoreboardActiveTab >= tabCount) {
            scoreboardActiveTab = 0;
        }
    }

    scoreboardLeftHeld = leftDown;
    scoreboardRightHeld = rightDown;
}

static int CG_DrawScoreboardTabs(int y, int totalTabs, const ladderTabInfo_t *ladderTabs,
                                 int ladderTabCount, float fade) {
    int tabWidth;
    int i;
    int tabX;
    vec4_t barColor;
    vec4_t inactiveColor;
    vec4_t activeColor;
    vec4_t textColor;
    const char *label;

    if (totalTabs <= 1) {
        return y;
    }

    barColor[0] = 0.05f; barColor[1] = 0.06f; barColor[2] = 0.08f; barColor[3] = MODERN_SB_ALPHA * fade;
    inactiveColor[0] = 0.10f; inactiveColor[1] = 0.12f; inactiveColor[2] = 0.16f; inactiveColor[3] = MODERN_SB_ALPHA * fade * 0.9f;
    activeColor[0] = 0.18f; activeColor[1] = 0.22f; activeColor[2] = 0.28f; activeColor[3] = MODERN_SB_ALPHA * fade;
    textColor[0] = 0.85f; textColor[1] = 0.85f; textColor[2] = 0.85f; textColor[3] = fade;

    CG_FillRect(scoreboardX, y, currentScoreboardWidth, MODERN_SB_TAB_HEIGHT, barColor);

    if (totalTabs <= 0) {
        totalTabs = 1;
    }

    tabWidth = currentScoreboardWidth / totalTabs;
    if (tabWidth <= 0) {
        tabWidth = currentScoreboardWidth;
    }

    for (i = 0; i < totalTabs; i++) {
        tabX = scoreboardX + i * tabWidth;

        if (i == scoreboardActiveTab) {
            CG_FillRect(tabX, y, tabWidth, MODERN_SB_TAB_HEIGHT, activeColor);
        } else {
            CG_FillRect(tabX, y, tabWidth, MODERN_SB_TAB_HEIGHT, inactiveColor);
        }

        if (i == 0) {
            label = "MATCH";
        } else if (i - 1 < ladderTabCount) {
            label = ladderTabs[i - 1].label;
        } else {
            label = "LADDER";
        }

        CG_DrawModernText(tabX,
                          y + (MODERN_SB_TAB_HEIGHT - SMALLCHAR_HEIGHT) / 2,
                          label,
                          1,
                          tabWidth,
                          textColor,
                          (i == scoreboardActiveTab) ? qtrue : qfalse);
    }

    return y + MODERN_SB_TAB_HEIGHT + 12;
}

static qboolean CG_LadderCopyValue(const char *payload, const char *const *keys,
                                   int keyCount, char *out, int outSize) {
    int i;
    const char *value;

    if (!payload || !out || outSize <= 0) {
        return qfalse;
    }

    for (i = 0; i < keyCount; i++) {
        value = Info_ValueForKey(payload, keys[i]);
        if (value && value[0]) {
            Q_strncpyz(out, value, outSize);
            return qtrue;
        }
    }

    return qfalse;
}

static qboolean CG_LadderParseInt(const char *payload, const char *const *keys,
                                  int keyCount, int *out) {
    char buffer[MAX_STRING_CHARS];
    char *endPtr;
    long value;

    if (!out) {
        return qfalse;
    }

    if (!CG_LadderCopyValue(payload, keys, keyCount, buffer, sizeof(buffer))) {
        return qfalse;
    }

    value = strtol(buffer, &endPtr, 10);
    if (endPtr == buffer) {
        return qfalse;
    }

    *out = (int)value;
    return qtrue;
}

static qboolean CG_LadderParseFloat(const char *payload, const char *const *keys,
                                    int keyCount, float *out) {
    char buffer[MAX_STRING_CHARS];
    char *endPtr;
    double value;

    if (!out) {
        return qfalse;
    }

    if (!CG_LadderCopyValue(payload, keys, keyCount, buffer, sizeof(buffer))) {
        return qfalse;
    }

    value = strtod(buffer, &endPtr);
    if (endPtr == buffer) {
        return qfalse;
    }

    *out = (float)value;
    return qtrue;
}

static qboolean CG_LadderFormatTimeValue(const char *payload, const char *const *keys,
                                         int keyCount, char *out, int outSize) {
    int timeValue;

    if (CG_LadderParseInt(payload, keys, keyCount, &timeValue) && timeValue > 0) {
        const char *formatted = getStringForTime(timeValue);
        if (formatted) {
            Q_strncpyz(out, formatted, outSize);
            return qtrue;
        }
    }

    return CG_LadderCopyValue(payload, keys, keyCount, out, outSize);
}

static qboolean CG_LadderExtractName(const cg_ladder_entry_t *entry, char *out, int outSize) {
    static const char *nameKeys[] = { "name", "player", "displayName" };

    if (!entry || !out || outSize <= 0) {
        return qfalse;
    }

    if (CG_LadderCopyValue(entry->payload, nameKeys, sizeof(nameKeys) / sizeof(nameKeys[0]), out, outSize)) {
        return qtrue;
    }

    if (entry->identifier[0]) {
        Q_strncpyz(out, entry->identifier, outSize);
        return qtrue;
    }

    return qfalse;
}

static void CG_LadderFormatColumnValue(const cg_ladder_entry_t *entry, ladderColumnType_t type,
                                       char *out, int outSize) {
    static const char *posKeys[] = { "position", "rank", "place" };
    static const char *bestLapKeys[] = { "bestLapMs", "bestLap", "bestLapTime" };
    static const char *totalTimeKeys[] = { "totalRaceMs", "totalTime", "raceTime", "durationMs" };
    static const char *lapKeys[] = { "lapCount", "laps" };
    static const char *scoreKeys[] = { "score", "points", "rawScore" };
    static const char *fragKeys[] = { "kills", "frags", "score" };
    static const char *deathKeys[] = { "deaths" };
    static const char *kdKeys[] = { "kdRatio", "kdr" };
    static const char *damageKeys[] = { "damageDealt", "damage" };
    static const char *damageTakenKeys[] = { "damageTaken" };
    static const char *captureKeys[] = { "captures", "caps" };
    static const char *assistKeys[] = { "assistCount", "assists" };
    static const char *zoneKeys[] = { "zoneHoldMs", "zoneTime" };
    static const char *survivalKeys[] = { "survivalMs", "survival" };
    static const char *elimRoundKeys[] = { "eliminationRound", "round" };
    static const char *elimRemainKeys[] = { "eliminationPlayersRemaining", "remaining", "playersLeft" };
    char buffer[MAX_STRING_CHARS];
    int value;
    float fvalue;

    if (!out || outSize <= 0) {
        return;
    }

    out[0] = '\0';

    switch (type) {
        case LADDER_COL_POSITION:
            if (CG_LadderParseInt(entry->payload, posKeys, sizeof(posKeys) / sizeof(posKeys[0]), &value) && value > 0) {
                Com_sprintf(out, outSize, "%d", value);
            } else {
                Com_sprintf(out, outSize, "%d", entry->sequence + 1);
            }
            break;

        case LADDER_COL_NAME:
            if (!CG_LadderExtractName(entry, out, outSize)) {
                Q_strncpyz(out, "--", outSize);
            }
            break;

        case LADDER_COL_TEAM: {
            static const char *teamKeys[] = { "teamName", "team" };
            if (CG_LadderCopyValue(entry->payload, teamKeys, sizeof(teamKeys) / sizeof(teamKeys[0]), buffer, sizeof(buffer))) {
                char *endPtr;
                int teamNum;

                teamNum = (int)strtol(buffer, &endPtr, 10);
                while (endPtr && *endPtr && isspace((unsigned char)*endPtr)) {
                    endPtr++;
                }

                if (endPtr && *endPtr == '\0') {
                    Q_strncpyz(out, CG_GetTeamName((team_t)teamNum), outSize);
                } else {
                    Q_strncpyz(out, buffer, outSize);
                }
            }
            break;
        }

        case LADDER_COL_BEST_LAP:
            CG_LadderFormatTimeValue(entry->payload, bestLapKeys, sizeof(bestLapKeys) / sizeof(bestLapKeys[0]), out, outSize);
            break;

        case LADDER_COL_TOTAL_TIME:
            CG_LadderFormatTimeValue(entry->payload, totalTimeKeys, sizeof(totalTimeKeys) / sizeof(totalTimeKeys[0]), out, outSize);
            break;

        case LADDER_COL_LAPS:
            if (CG_LadderParseInt(entry->payload, lapKeys, sizeof(lapKeys) / sizeof(lapKeys[0]), &value)) {
                Com_sprintf(out, outSize, "%d", value);
            }
            break;

        case LADDER_COL_SCORE:
            if (CG_LadderParseInt(entry->payload, scoreKeys, sizeof(scoreKeys) / sizeof(scoreKeys[0]), &value)) {
                Com_sprintf(out, outSize, "%d", value);
            }
            break;

        case LADDER_COL_FRAGS:
            if (CG_LadderParseInt(entry->payload, fragKeys, sizeof(fragKeys) / sizeof(fragKeys[0]), &value)) {
                Com_sprintf(out, outSize, "%d", value);
            }
            break;

        case LADDER_COL_DEATHS:
            if (CG_LadderParseInt(entry->payload, deathKeys, sizeof(deathKeys) / sizeof(deathKeys[0]), &value)) {
                Com_sprintf(out, outSize, "%d", value);
            }
            break;

        case LADDER_COL_KD:
            if (CG_LadderParseFloat(entry->payload, kdKeys, sizeof(kdKeys) / sizeof(kdKeys[0]), &fvalue)) {
                Com_sprintf(out, outSize, "%.2f", fvalue);
            }
            break;

        case LADDER_COL_DAMAGE_DEALT:
            if (CG_LadderParseInt(entry->payload, damageKeys, sizeof(damageKeys) / sizeof(damageKeys[0]), &value)) {
                Com_sprintf(out, outSize, "%d", value);
            }
            break;

        case LADDER_COL_DAMAGE_TAKEN:
            if (CG_LadderParseInt(entry->payload, damageTakenKeys, sizeof(damageTakenKeys) / sizeof(damageTakenKeys[0]), &value)) {
                Com_sprintf(out, outSize, "%d", value);
            }
            break;

        case LADDER_COL_CAPTURES:
            if (CG_LadderParseInt(entry->payload, captureKeys, sizeof(captureKeys) / sizeof(captureKeys[0]), &value)) {
                Com_sprintf(out, outSize, "%d", value);
            }
            break;

        case LADDER_COL_ASSISTS:
            if (CG_LadderParseInt(entry->payload, assistKeys, sizeof(assistKeys) / sizeof(assistKeys[0]), &value)) {
                Com_sprintf(out, outSize, "%d", value);
            }
            break;

        case LADDER_COL_ZONE_HOLD:
            CG_LadderFormatTimeValue(entry->payload, zoneKeys, sizeof(zoneKeys) / sizeof(zoneKeys[0]), out, outSize);
            break;

        case LADDER_COL_SURVIVAL:
            CG_LadderFormatTimeValue(entry->payload, survivalKeys, sizeof(survivalKeys) / sizeof(survivalKeys[0]), out, outSize);
            break;

        case LADDER_COL_ELIM_ROUND:
            if (CG_LadderParseInt(entry->payload, elimRoundKeys, sizeof(elimRoundKeys) / sizeof(elimRoundKeys[0]), &value)) {
                Com_sprintf(out, outSize, "%d", value);
            }
            break;

        case LADDER_COL_ELIM_REMAIN:
            if (CG_LadderParseInt(entry->payload, elimRemainKeys, sizeof(elimRemainKeys) / sizeof(elimRemainKeys[0]), &value)) {
                Com_sprintf(out, outSize, "%d", value);
            }
            break;

        case LADDER_COL_SOURCE:
            switch (entry->source) {
                case LADDER_SOURCE_SERVER:
                    Q_strncpyz(out, "SRV", outSize);
                    break;
                case LADDER_SOURCE_WEBSERVICE:
                    Q_strncpyz(out, "WEB", outSize);
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }

    if (!out[0]) {
        Q_strncpyz(out, "--", outSize);
    }
}

static int CG_GetLadderColumns(ladderColumnDef_t *columns, int maxColumns) {
    int count;
    qboolean isTeam;

    if (!columns || maxColumns <= 0) {
        return 0;
    }

    count = 0;
    isTeam = CG_IsTeamGametype();

    if (count < maxColumns) {
        columns[count].type = LADDER_COL_POSITION;
        columns[count].header = "POS";
        columns[count].width = 56;
        columns[count].align = 1;
        count++;
    }

    if (isTeam && count < maxColumns) {
        columns[count].type = LADDER_COL_TEAM;
        columns[count].header = "TEAM";
        columns[count].width = 72;
        columns[count].align = 1;
        count++;
    }

    if (count < maxColumns) {
        columns[count].type = LADDER_COL_NAME;
        columns[count].header = "PLAYER";
        columns[count].width = isTeam ? 188 : 220;
        columns[count].align = 0;
        count++;
    }

    switch (cgs.gametype) {
        case GT_RACING:
        case GT_TEAM_RACING:
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_BEST_LAP;
                columns[count].header = "BEST";
                columns[count].width = 104;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_TOTAL_TIME;
                columns[count].header = "TOTAL";
                columns[count].width = 112;
                columns[count].align = 1;
                count++;
            }
            if (cgs.laplimit > 1 && count < maxColumns) {
                columns[count].type = LADDER_COL_LAPS;
                columns[count].header = "LAPS";
                columns[count].width = 72;
                columns[count].align = 1;
                count++;
            }
            break;

        case GT_RACING_DM:
        case GT_TEAM_RACING_DM:
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_BEST_LAP;
                columns[count].header = "BEST";
                columns[count].width = 88;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_TOTAL_TIME;
                columns[count].header = "TOTAL";
                columns[count].width = 88;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_FRAGS;
                columns[count].header = "FRAGS";
                columns[count].width = 64;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_DAMAGE_DEALT;
                columns[count].header = "DMG";
                columns[count].width = 80;
                columns[count].align = 1;
                count++;
            }
            break;

        case GT_ELIMINATION:
        case GT_LCS:
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_SURVIVAL;
                columns[count].header = "SURVIVAL";
                columns[count].width = 112;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_ELIM_ROUND;
                columns[count].header = "ROUND";
                columns[count].width = 80;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_ELIM_REMAIN;
                columns[count].header = "LEFT";
                columns[count].width = 80;
                columns[count].align = 1;
                count++;
            }
            if (cgs.laplimit > 1 && count < maxColumns) {
                columns[count].type = LADDER_COL_LAPS;
                columns[count].header = "LAPS";
                columns[count].width = 72;
                columns[count].align = 1;
                count++;
            }
            break;

        case GT_DERBY:
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_SCORE;
                columns[count].header = "SCORE";
                columns[count].width = 88;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_DAMAGE_DEALT;
                columns[count].header = "DMG";
                columns[count].width = 96;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_DAMAGE_TAKEN;
                columns[count].header = "TAKEN";
                columns[count].width = 96;
                columns[count].align = 1;
                count++;
            }
            break;

        case GT_DOMINATION:
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_SCORE;
                columns[count].header = "POINTS";
                columns[count].width = 80;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_ZONE_HOLD;
                columns[count].header = "ZONE";
                columns[count].width = 110;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_DAMAGE_DEALT;
                columns[count].header = "DMG";
                columns[count].width = 92;
                columns[count].align = 1;
                count++;
            }
            break;

        case GT_CTF:
        case GT_CTF4:
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_SCORE;
                columns[count].header = "SCORE";
                columns[count].width = 80;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_CAPTURES;
                columns[count].header = "CAPS";
                columns[count].width = 80;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_ASSISTS;
                columns[count].header = "ASSISTS";
                columns[count].width = 80;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_DEATHS;
                columns[count].header = "DEATHS";
                columns[count].width = 72;
                columns[count].align = 1;
                count++;
            }
            break;

        default:
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_FRAGS;
                columns[count].header = isTeam ? "SCORE" : "FRAGS";
                columns[count].width = 72;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_DEATHS;
                columns[count].header = "DEATHS";
                columns[count].width = 72;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_KD;
                columns[count].header = "K/D";
                columns[count].width = 70;
                columns[count].align = 1;
                count++;
            }
            if (count < maxColumns) {
                columns[count].type = LADDER_COL_DAMAGE_DEALT;
                columns[count].header = "DMG";
                columns[count].width = 92;
                columns[count].align = 1;
                count++;
            }
            break;
    }

    return count;
}

static void CG_RequestLadderRetry(const char *identifier) {
    char cleanId[MAX_LADDER_IDENTIFIER];
    char command[MAX_STRING_CHARS];

    cleanId[0] = '\0';

    if (identifier && identifier[0]) {
        Q_strncpyz(cleanId, identifier, sizeof(cleanId));
        Q_CleanStr(cleanId);
    }

    if (cleanId[0]) {
        Com_sprintf(command, sizeof(command), "ladder_request \"%s\"", cleanId);
    } else {
        Q_strncpyz(command, "ladder_request", sizeof(command));
    }

    trap_SendClientCommand(command);

    if (cleanId[0]) {
        CG_Printf("Requesting ladder data for %s\n", cleanId);
    } else {
        CG_Printf("Requesting ladder data\n");
    }
}

static qboolean CG_HandleLadderRetryInput(qboolean *isActiveOut) {
    qboolean down;

    down = trap_Key_IsDown(K_ENTER) ||
           trap_Key_IsDown(K_KP_ENTER) ||
           trap_Key_IsDown(K_SPACE) ||
           trap_Key_IsDown(K_MOUSE1) ||
           trap_Key_IsDown(K_PAD0_A) ||
           trap_Key_IsDown(K_PAD0_X) ||
           trap_Key_IsDown(K_PAD0_Y) ||
           trap_Key_IsDown(K_PAD0_START);

    if (isActiveOut) {
        *isActiveOut = down;
    }

    if (down) {
        if (!scoreboardRetryHeld) {
            scoreboardRetryHeld = qtrue;
            return qtrue;
        }
    } else if (scoreboardRetryHeld) {
        scoreboardRetryHeld = qfalse;
    }

    return qfalse;
}

static void CG_DrawModernButton(int x, int y, int width, int height,
                                const char *label, float fade, qboolean active) {
    vec4_t baseColor;
    vec4_t borderColor;
    vec4_t textColor;

    if (active) {
        baseColor[0] = 0.24f; baseColor[1] = 0.30f; baseColor[2] = 0.36f; baseColor[3] = fade;
        borderColor[0] = 0.45f; borderColor[1] = 0.60f; borderColor[2] = 0.75f; borderColor[3] = fade;
    } else {
        baseColor[0] = 0.16f; baseColor[1] = 0.20f; baseColor[2] = 0.24f; baseColor[3] = fade * 0.85f;
        borderColor[0] = 0.32f; borderColor[1] = 0.40f; borderColor[2] = 0.48f; borderColor[3] = fade * 0.85f;
    }

    textColor[0] = 0.95f; textColor[1] = 0.95f; textColor[2] = 0.95f; textColor[3] = fade;

    CG_FillRect(x, y, width, height, baseColor);
    CG_FillRect(x, y, width, MODERN_SB_BORDER_SIZE, borderColor);
    CG_FillRect(x, y + height - MODERN_SB_BORDER_SIZE, width, MODERN_SB_BORDER_SIZE, borderColor);
    CG_FillRect(x, y, MODERN_SB_BORDER_SIZE, height, borderColor);
    CG_FillRect(x + width - MODERN_SB_BORDER_SIZE, y, MODERN_SB_BORDER_SIZE, height, borderColor);

    CG_DrawModernText(x, y + (height - BIGCHAR_HEIGHT) / 2, label, 1, width, textColor, qtrue);
}

static int CG_CalculateScoreTotalTime(const score_t *score, qboolean isRacingMode) {
    centity_t *cent;

    if (!score) {
        return 0;
    }

    if (score->client < 0 || score->client >= cgs.maxclients) {
        return 0;
    }

    cent = &cg_entities[score->client];

    if (cent->finishRaceTime) {
        if (isRacingMode && cgs.laplimit <= 1 && cent->startLapTime > 0) {
            return cent->finishRaceTime - cent->startLapTime;
        }
        if (score->time) {
            return cent->finishRaceTime - score->time;
        }
        if (cent->startRaceTime > 0) {
            return cent->finishRaceTime - cent->startRaceTime;
        }
    }

    if (isRacingMode && cgs.laplimit <= 1 && cent->startLapTime > 0) {
        return cg.time - cent->startLapTime;
    }

    if (score->time) {
        return cg.time - score->time;
    }

    return 0;
}

static qboolean CG_ShouldSwapLocalEntries(const localFallbackEntry_t *current,
                                          const localFallbackEntry_t *candidate,
                                          qboolean isRacingMode) {
    int currentPos;
    int candidatePos;

    if (!current || !candidate) {
        return qfalse;
    }

    currentPos = (current->position > 0) ? current->position : INT_MAX;
    candidatePos = (candidate->position > 0) ? candidate->position : INT_MAX;

    if (candidatePos < currentPos) {
        return qtrue;
    }
    if (candidatePos > currentPos) {
        return qfalse;
    }

    if (isRacingMode) {
        int currentBest = (current->bestLapTime > 0) ? current->bestLapTime : INT_MAX;
        int candidateBest = (candidate->bestLapTime > 0) ? candidate->bestLapTime : INT_MAX;

        if (candidateBest < currentBest) {
            return qtrue;
        }
        if (candidateBest > currentBest) {
            return qfalse;
        }

        {
            int currentTotal = (current->totalTime > 0) ? current->totalTime : INT_MAX;
            int candidateTotal = (candidate->totalTime > 0) ? candidate->totalTime : INT_MAX;

            if (candidateTotal < currentTotal) {
                return qtrue;
            }
            if (candidateTotal > currentTotal) {
                return qfalse;
            }
        }
    } else {
        if (candidate->score > current->score) {
            return qtrue;
        }
        if (candidate->score < current->score) {
            return qfalse;
        }

        if (candidate->deaths < current->deaths) {
            return qtrue;
        }
        if (candidate->deaths > current->deaths) {
            return qfalse;
        }
    }

    if (candidate->name[0] || current->name[0]) {
        if (Q_stricmp(candidate->name, current->name) < 0) {
            return qtrue;
        }
    }

    return qfalse;
}

static int CG_BuildLocalFallbackEntries(localFallbackEntry_t *entries, int maxEntries,
                                        qboolean isRacingMode) {
    int count;
    int i;

    if (!entries || maxEntries <= 0) {
        return 0;
    }

    count = 0;

    for (i = 0; i < cg.numScores && count < maxEntries; i++) {
        const score_t *score = &cg.scores[i];
        clientInfo_t *ci;

        if (score->client < 0 || score->client >= cgs.maxclients) {
            continue;
        }

        ci = &cgs.clientinfo[score->client];
        if (!ci->infoValid) {
            continue;
        }

        if (ci->team == TEAM_SPECTATOR) {
            continue;
        }

        entries[count].clientNum = score->client;
        entries[count].position = score->position;
        entries[count].bestLapTime = cg_entities[score->client].bestLapTime;
        entries[count].totalTime = CG_CalculateScoreTotalTime(score, isRacingMode);
        entries[count].score = score->score;
        entries[count].deaths = score->deaths;
        entries[count].team = ci->team;
        Q_strncpyz(entries[count].name, ci->name, sizeof(entries[count].name));
        count++;
    }

    for (i = 0; i < count; i++) {
        int j;
        int bestIndex = i;

        for (j = i + 1; j < count; j++) {
            if (CG_ShouldSwapLocalEntries(&entries[bestIndex], &entries[j], isRacingMode)) {
                bestIndex = j;
            }
        }

        if (bestIndex != i) {
            localFallbackEntry_t temp = entries[i];
            entries[i] = entries[bestIndex];
            entries[bestIndex] = temp;
        }
    }

    return count;
}

static void CG_DrawLadderOfflineFallback(int y, float fade, const cg_ladder_t *ladder,
                                         const char *tabLabel, const char *tabIdentifier,
                                         const char *overrideMessage) {
    vec4_t textColor;
    vec4_t hintColor;
    vec4_t rowHighlight;
    char cleanLabel[64];
    char cleanError[MAX_STRING_CHARS];
    char header[256];
    char infoLine[256];
    qboolean isRacing;
    qboolean retryActive = qfalse;
    qboolean retryTriggered;
    localFallbackEntry_t entries[MAX_LOCAL_FALLBACK_ENTRIES];
    int entryCount;
    int rowHeight;
    int posWidth;
    int bestWidth;
    int totalWidth;
    int nameWidth;
    int posX;
    int nameX;
    int bestX;
    int totalX;
    int localClient;
    int i;

    textColor[0] = 0.90f; textColor[1] = 0.90f; textColor[2] = 0.90f; textColor[3] = fade;
    hintColor[0] = 0.75f; hintColor[1] = 0.75f; hintColor[2] = 0.75f; hintColor[3] = fade;
    rowHighlight[0] = 0.20f; rowHighlight[1] = 0.40f; rowHighlight[2] = 0.80f; rowHighlight[3] = 0.25f * fade;

    Q_strncpyz(cleanLabel, (tabLabel && tabLabel[0]) ? tabLabel : "Ladder", sizeof(cleanLabel));
    Q_CleanStr(cleanLabel);
    if (!cleanLabel[0]) {
        Q_strncpyz(cleanLabel, "Ladder", sizeof(cleanLabel));
    }

    if (overrideMessage && overrideMessage[0]) {
        Q_strncpyz(header, overrideMessage, sizeof(header));
    } else if (ladder && ladder->status == LADDER_STATUS_ERROR) {
        if (ladder->errorMessage[0]) {
            Q_strncpyz(cleanError, ladder->errorMessage, sizeof(cleanError));
            Q_CleanStr(cleanError);
            Com_sprintf(header, sizeof(header), "%s ladder offline: %s", cleanLabel, cleanError);
        } else {
            Com_sprintf(header, sizeof(header), "%s ladder offline", cleanLabel);
        }
    } else if (ladder && ladder->status == LADDER_STATUS_EMPTY) {
        Com_sprintf(header, sizeof(header), "%s ladder currently unreachable", cleanLabel);
    } else {
        Com_sprintf(header, sizeof(header), "%s ladder data unavailable", cleanLabel);
    }

    CG_DrawModernBackground(scoreboardX, y, currentScoreboardWidth, MODERN_SB_HEADER_HEIGHT,
                            MODERN_SB_ALPHA * fade, qfalse);
    CG_DrawModernText(scoreboardX, y + (MODERN_SB_HEADER_HEIGHT - BIGCHAR_HEIGHT) / 2,
                      header, 1, currentScoreboardWidth, textColor, qtrue);

    y += MODERN_SB_HEADER_HEIGHT + 6;

    Com_sprintf(infoLine, sizeof(infoLine),
                "Showing local session results while online data is unavailable.");
    CG_DrawModernText(scoreboardX, y, infoLine, 1, currentScoreboardWidth, hintColor, qfalse);
    y += SMALLCHAR_HEIGHT + 6;

    retryTriggered = CG_HandleLadderRetryInput(&retryActive);
    CG_DrawModernButton(scoreboardX + (currentScoreboardWidth - 200) / 2, y, 200, 32,
                        "RETRY", fade, retryActive);
    if (retryTriggered) {
        CG_RequestLadderRetry((tabIdentifier) ? tabIdentifier : "");
    }
    y += 32 + 6;

    Com_sprintf(infoLine, sizeof(infoLine),
                "Press Enter, Space or A on your gamepad to retry loading online results.");
    CG_DrawModernText(scoreboardX, y, infoLine, 1, currentScoreboardWidth, hintColor, qfalse);
    y += SMALLCHAR_HEIGHT + 10;

    Com_sprintf(infoLine, sizeof(infoLine), "Local session leaderboard:");
    CG_DrawModernText(scoreboardX, y, infoLine, 0, currentScoreboardWidth, hintColor, qfalse);
    y += SMALLCHAR_HEIGHT + 4;

    isRacing = CG_IsRacingGametype();
    entryCount = CG_BuildLocalFallbackEntries(entries, MAX_LOCAL_FALLBACK_ENTRIES, isRacing);

    posWidth = 70;
    bestWidth = isRacing ? 140 : 120;
    totalWidth = 140;
    nameWidth = currentScoreboardWidth - (posWidth + bestWidth + totalWidth);
    if (nameWidth < 180) {
        nameWidth = 180;
    }
    if (posWidth + bestWidth + totalWidth + nameWidth > currentScoreboardWidth) {
        totalWidth = currentScoreboardWidth - (posWidth + bestWidth + nameWidth);
        if (totalWidth < 100) {
            totalWidth = 100;
        }
    }
    if (posWidth + bestWidth + totalWidth + nameWidth > currentScoreboardWidth) {
        bestWidth = currentScoreboardWidth - (posWidth + totalWidth + nameWidth);
        if (bestWidth < 100) {
            bestWidth = 100;
        }
    }
    if (posWidth + bestWidth + totalWidth + nameWidth > currentScoreboardWidth) {
        nameWidth = currentScoreboardWidth - (posWidth + bestWidth + totalWidth);
        if (nameWidth < 140) {
            nameWidth = 140;
        }
    }

    posX = scoreboardX;
    nameX = posX + posWidth;
    bestX = nameX + nameWidth;
    totalX = bestX + bestWidth;

    localClient = (cg.snap) ? cg.snap->ps.clientNum : -1;

    if (entryCount > 0) {
        const char *bestHeader;
        const char *totalHeader;

        bestHeader = isRacing ? "BEST LAP" : "SCORE";
        if (isRacing) {
            totalHeader = (cgs.laplimit <= 1) ? "RUN TIME" : "TOTAL TIME";
        } else {
            totalHeader = "DEATHS";
        }

        CG_DrawModernBackground(scoreboardX, y, currentScoreboardWidth, MODERN_SB_HEADER_HEIGHT,
                                MODERN_SB_ALPHA, qtrue);
        CG_DrawModernText(posX, y + 8, "POS", 1, posWidth, textColor, qtrue);
        CG_DrawModernText(nameX, y + 8, "DRIVER", 0, nameWidth, textColor, qtrue);
        CG_DrawModernText(bestX, y + 8, bestHeader, 1, bestWidth, textColor, qtrue);
        CG_DrawModernText(totalX, y + 8, totalHeader, 1, totalWidth, textColor, qtrue);

        y += MODERN_SB_HEADER_HEIGHT + 4;
        rowHeight = MODERN_SB_ROW_HEIGHT;

        for (i = 0; i < entryCount; i++) {
            int rowY = y + i * rowHeight;
            int textY = rowY + (rowHeight - SMALLCHAR_HEIGHT) / 2;
            char posText[32];
            char bestText[64];
            char totalText[64];
            const localFallbackEntry_t *entry = &entries[i];

            if (localClient >= 0 && entry->clientNum == localClient) {
                CG_FillRect(scoreboardX, rowY, currentScoreboardWidth, rowHeight, rowHighlight);
            }

            CG_DrawModernBackground(scoreboardX, rowY, currentScoreboardWidth, rowHeight,
                                    MODERN_SB_ALPHA * fade, qfalse);

            if (entry->position > 0) {
                Com_sprintf(posText, sizeof(posText), "%d", entry->position);
            } else {
                Com_sprintf(posText, sizeof(posText), "%d", i + 1);
            }

            if (isRacing) {
                if (entry->bestLapTime > 0) {
                    const char *lap = getStringForTime(entry->bestLapTime);
                    Q_strncpyz(bestText, lap, sizeof(bestText));
                } else {
                    Q_strncpyz(bestText, "--", sizeof(bestText));
                }

                if (entry->totalTime > 0) {
                    const char *total = getStringForTime(entry->totalTime);
                    Q_strncpyz(totalText, total, sizeof(totalText));
                } else {
                    Q_strncpyz(totalText, "--", sizeof(totalText));
                }
            } else {
                Com_sprintf(bestText, sizeof(bestText), "%d", entry->score);
                Com_sprintf(totalText, sizeof(totalText), "%d", entry->deaths);
            }

            CG_DrawModernText(posX, textY, posText, 1, posWidth, textColor, qfalse);
            CG_DrawModernText(nameX, textY, entry->name, 0, nameWidth, textColor, qfalse);
            CG_DrawModernText(bestX, textY, bestText, 1, bestWidth, textColor, qfalse);
            CG_DrawModernText(totalX, textY, totalText, 1, totalWidth, textColor, qfalse);
        }
    } else {
        CG_DrawModernBackground(scoreboardX, y, currentScoreboardWidth, MODERN_SB_HEADER_HEIGHT,
                                MODERN_SB_ALPHA * fade, qfalse);
        CG_DrawModernText(scoreboardX, y + (MODERN_SB_HEADER_HEIGHT - SMALLCHAR_HEIGHT) / 2,
                          "No local session data recorded yet.", 1, currentScoreboardWidth,
                          hintColor, qfalse);
    }
}

static void CG_DrawLadderStatusMessage(int x, int y, int width, float fade, const char *message) {
    vec4_t textColor;
    char buffer[128];

    if (!message || !message[0]) {
        message = "No ladder data";
    }

    Q_strncpyz(buffer, message, sizeof(buffer));

    textColor[0] = 0.85f; textColor[1] = 0.85f; textColor[2] = 0.85f; textColor[3] = fade;

    CG_DrawModernBackground(x, y, width, MODERN_SB_HEADER_HEIGHT, MODERN_SB_ALPHA * fade, qfalse);
    CG_DrawModernText(x,
                     y + (MODERN_SB_HEADER_HEIGHT - BIGCHAR_HEIGHT) / 2,
                     buffer,
                     1,
                     width,
                     textColor,
                     qtrue);
}

static void CG_DrawLadderView(int y, float fade, const cg_ladder_t *ladder,
                              int selectedTab, const ladderTabInfo_t *tabs, int tabCount) {
    ladderColumnDef_t columnDefs[MAX_LADDER_COLUMNS];
    const cg_ladder_entry_t *visibleEntries[MAX_LADDER_DISPLAY_ROWS];
    int columnCount;
    int totalWidth;
    int columnX[MAX_LADDER_COLUMNS];
    int currentX;
    int rowHeight;
    int entryCount;
    int i;
    int j;
    vec4_t textColor;
    vec4_t rowHighlight;
    char identifier[MAX_LADDER_IDENTIFIER];
    const char *activeLabel;
    char localName[MAX_NAME_LENGTH];
    char localClean[MAX_NAME_LENGTH];
    qboolean haveLocal;

    identifier[0] = '\0';
    activeLabel = "Ladder";

    if (tabs && tabCount > 0 && selectedTab >= 0 && selectedTab < tabCount) {
        activeLabel = tabs[selectedTab].label;
        Q_strncpyz(identifier, tabs[selectedTab].identifier, sizeof(identifier));
    }

    if (!ladder) {
        CG_DrawLadderOfflineFallback(y, fade, ladder, activeLabel, identifier, NULL);
        return;
    }

    if (ladder->status == LADDER_STATUS_LOADING) {
        CG_DrawLadderStatusMessage(scoreboardX, y, currentScoreboardWidth, fade, "Loading ladder data...");
        return;
    }

    if (ladder->status == LADDER_STATUS_ERROR || ladder->status == LADDER_STATUS_EMPTY) {
        CG_DrawLadderOfflineFallback(y, fade, ladder, activeLabel, identifier, NULL);
        return;
    }

    entryCount = 0;
    for (i = 0; i < ladder->numEntries && entryCount < MAX_LADDER_DISPLAY_ROWS; i++) {
        const cg_ladder_entry_t *entry = &ladder->entries[i];

        if (identifier[0]) {
            if (Q_stricmp(identifier, entry->identifier)) {
                continue;
            }
        } else if (tabs && tabCount > 1) {
            if (entry->identifier[0]) {
                continue;
            }
        }

        visibleEntries[entryCount++] = entry;
    }

    if (entryCount <= 0) {
        char cleanLabel[64];

        Q_strncpyz(cleanLabel, activeLabel, sizeof(cleanLabel));
        Q_CleanStr(cleanLabel);
        if (!cleanLabel[0]) {
            Q_strncpyz(cleanLabel, "Ladder", sizeof(cleanLabel));
        }

        CG_DrawLadderOfflineFallback(y, fade, ladder, activeLabel, identifier,
                                     va("No online %s results yet.", cleanLabel));
        return;
    }

    columnCount = CG_GetLadderColumns(columnDefs, MAX_LADDER_COLUMNS);
    if (columnCount <= 0) {
        CG_DrawLadderStatusMessage(scoreboardX, y, currentScoreboardWidth, fade, "No ladder columns");
        return;
    }

    totalWidth = 0;
    for (i = 0; i < columnCount; i++) {
        totalWidth += columnDefs[i].width;
    }

    currentX = scoreboardX + (currentScoreboardWidth - totalWidth) / 2;
    if (currentX < scoreboardX + MODERN_SB_PADDING) {
        currentX = scoreboardX + MODERN_SB_PADDING;
    }

    for (i = 0; i < columnCount; i++) {
        columnX[i] = currentX;
        currentX += columnDefs[i].width;
    }

    textColor[0] = 0.90f; textColor[1] = 0.90f; textColor[2] = 0.90f; textColor[3] = fade;
    rowHighlight[0] = 0.2f; rowHighlight[1] = 0.4f; rowHighlight[2] = 0.8f; rowHighlight[3] = 0.25f * fade;

    localName[0] = '\0';
    localClean[0] = '\0';
    haveLocal = qfalse;

    if (cg.snap) {
        int clientNum = cg.snap->ps.clientNum;
        if (clientNum >= 0 && clientNum < cgs.maxclients) {
            Q_strncpyz(localName, cgs.clientinfo[clientNum].name, sizeof(localName));
            Q_strncpyz(localClean, localName, sizeof(localClean));
            Q_CleanStr(localClean);
            if (localClean[0]) {
                haveLocal = qtrue;
            }
        }
    }

    CG_DrawModernBackground(scoreboardX, y, currentScoreboardWidth, MODERN_SB_HEADER_HEIGHT,
                           MODERN_SB_ALPHA, qtrue);

    for (i = 0; i < columnCount; i++) {
        CG_DrawModernText(columnX[i], y + 8, columnDefs[i].header, columnDefs[i].align,
                         columnDefs[i].width, textColor, qtrue);
    }

    y += MODERN_SB_HEADER_HEIGHT + 4;
    rowHeight = MODERN_SB_ROW_HEIGHT;

    for (i = 0; i < entryCount; i++) {
        const cg_ladder_entry_t *entry = visibleEntries[i];
        int rowY = y + i * rowHeight;
        int textY = rowY + (rowHeight - SMALLCHAR_HEIGHT) / 2;
        qboolean isLocal = qfalse;
        char entryName[MAX_STRING_CHARS];

        if (haveLocal && CG_LadderExtractName(entry, entryName, sizeof(entryName))) {
            char cleanName[MAX_STRING_CHARS];
            Q_strncpyz(cleanName, entryName, sizeof(cleanName));
            Q_CleanStr(cleanName);
            if (!Q_stricmp(cleanName, localClean)) {
                isLocal = qtrue;
            }
        }

        if (isLocal) {
            CG_FillRect(scoreboardX, rowY, currentScoreboardWidth, rowHeight, rowHighlight);
        }

        CG_DrawModernBackground(scoreboardX, rowY, currentScoreboardWidth, rowHeight,
                               MODERN_SB_ALPHA * fade, qfalse);

        for (j = 0; j < columnCount; j++) {
            char value[128];
            CG_LadderFormatColumnValue(entry, columnDefs[j].type, value, sizeof(value));
            CG_DrawModernText(columnX[j], textY, value, columnDefs[j].align,
                             columnDefs[j].width, textColor, qfalse);
        }
    }
}


/*
=================
CG_DrawModernTeamHeaderRow
Draws a header row for a team in the scoreboard
=================
*/
static void CG_DrawModernTeamHeaderRow(int y, team_t team, int rank, int score, int damage, float fade) {
    char teamText[128];
    char scoreText[64];
    vec4_t teamColor;
    vec4_t textColor;
    int rowHeight = MODERN_SB_COMPACT_HEIGHT; // Use compact height for team headers
    int textY = y + (rowHeight - BIGCHAR_HEIGHT) / 2;

    /* Background for the team header */
    CG_DrawModernBackground(scoreboardX, y, currentScoreboardWidth, rowHeight, MODERN_SB_ALPHA * fade, qtrue);

    /* Team Rank and Name */
    CG_GetModernTeamColor(team, teamColor);
    teamColor[3] = fade;
    Com_sprintf(teamText, sizeof(teamText), "%d. %s", rank, CG_GetTeamName(team));
    CG_DrawModernText(scoreboardX, textY, teamText, 0, currentScoreboardWidth, teamColor, qtrue);

    /* Team Score & Damage */
    textColor[0] = 0.9f; textColor[1] = 0.9f; textColor[2] = 0.9f; textColor[3] = fade;
    Com_sprintf(scoreText, sizeof(scoreText), "%d pts / %d dmg", score, damage);
    CG_DrawModernText(scoreboardX, textY, scoreText, 2, currentScoreboardWidth, textColor, qtrue);
}

/*
=================
CG_DrawColumnData
Draw data for a specific column type - C89 compatible
=================
*/
static void CG_DrawColumnData(sbColumn_t colType, int x, int y, int width, 
                             score_t *score, int rank, float fade, 
                             qboolean isCompact) {
    clientInfo_t *ci;
    centity_t *cent;
    char buffer[32];
    int totalTime, lapTime, avatarSize, rowHeight;
    char *timeStr, *lapTimeStr;
    vec4_t textColor;
    vec4_t rankColor;
    vec4_t teamColor;
    vec4_t botColor;
    vec4_t readyColor;
    qboolean isRacingMode;
    
    if (score->client < 0 || score->client >= cgs.maxclients) {
        return;
    }
    
    /* Initialize colors - C89 style */
    textColor[0] = 0.9f; textColor[1] = 0.9f; textColor[2] = 0.9f; textColor[3] = fade;
    rankColor[0] = 0.9f; rankColor[1] = 0.9f; rankColor[2] = 0.9f; rankColor[3] = fade;
    teamColor[0] = 0.9f; teamColor[1] = 0.9f; teamColor[2] = 0.9f; teamColor[3] = fade;
    botColor[0] = 0.5f; botColor[1] = 0.8f; botColor[2] = 0.5f; botColor[3] = fade;
    readyColor[0] = 0.3f; readyColor[1] = 0.8f; readyColor[2] = 0.3f; readyColor[3] = fade;
    
    ci = &cgs.clientinfo[score->client];
    cent = &cg_entities[score->client];
    rowHeight = isCompact ? MODERN_SB_COMPACT_HEIGHT : MODERN_SB_ROW_HEIGHT;
    avatarSize = rowHeight - 8;
    isRacingMode = CG_IsRacingGametype();
    
    switch (colType) {
        case SBCOL_RANK:
            if (ci->team == TEAM_SPECTATOR) {
                Com_sprintf(buffer, sizeof(buffer), "SPEC");
                CG_DrawModernText(x, y, buffer, 1, width, textColor, qfalse);
            } else if (isRacingMode && score->position > 0) {
                Com_sprintf(buffer, sizeof(buffer), "%d", score->position);
                CG_GetRankColor(score->position, rankColor);
                rankColor[3] = fade;
                CG_DrawModernText(x, y, buffer, 1, width, rankColor, 
                                 (score->position <= 3) ? qtrue : qfalse);
            } else if (!isRacingMode && rank > 0) {
                Com_sprintf(buffer, sizeof(buffer), "%d", rank);
                CG_GetRankColor(rank, rankColor);
                rankColor[3] = fade;
                CG_DrawModernText(x, y, buffer, 1, width, rankColor, (rank <= 3) ? qtrue : qfalse);
            } else {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            }
            break;
            
        case SBCOL_AVATAR:
            if (ci->modelIcon) {
                CG_DrawPic(x + (width - avatarSize) / 2, 
                          y - (avatarSize - SMALLCHAR_HEIGHT) / 2, 
                          avatarSize, avatarSize, ci->modelIcon);
            }
            break;
            
        case SBCOL_NAME:
            if (CG_IsTeamGametype() && ci->team != TEAM_SPECTATOR) {
                CG_GetModernTeamColor(ci->team, teamColor);
                teamColor[3] = fade;
                CG_DrawModernText(x, y, ci->name, 0, width, teamColor, qfalse);
            } else {
                CG_DrawModernText(x, y, ci->name, 0, width, textColor, qfalse);
            }
            
            /* Bot indicator */
            if (ci->botSkill > 0 && ci->botSkill <= 5) {
                CG_DrawSmallStringColor(x + width - 32, y, "BOT", botColor);
            }
            break;
            
        case SBCOL_SCORE:
            if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            } else {
                Com_sprintf(buffer, sizeof(buffer), "%d", score->score);
                CG_DrawModernText(x, y, buffer, 1, width, textColor, qfalse);
            }
            break;
            
        case SBCOL_DEATHS:
            if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            } else {
                /* Use damageDealt as damage indicator for Q3Rally */
                Com_sprintf(buffer, sizeof(buffer), "%d", score->damageTaken);
                CG_DrawModernText(x, y, buffer, 1, width, textColor, qfalse);
            }
            break;
            
        case SBCOL_LAPTIME:
            if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            } else {
                lapTime = cg_entities[score->client].bestLapTime;
                if (lapTime > 0) {
                    lapTimeStr = getStringForTime(lapTime);
                    CG_DrawModernText(x, y, lapTimeStr, 1, width, textColor, qfalse);
                } else {
                    CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
                }
            }
            break;
            
        case SBCOL_TOTALTIME:
            if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            } else {
                if (cent->finishRaceTime) {
                    if (isRacingMode && cgs.laplimit <= 1 && cent->startLapTime > 0) {
                        totalTime = cent->finishRaceTime - cent->startLapTime;
                    } else {
                        totalTime = cent->finishRaceTime - score->time;
                    }
                } else if (isRacingMode && cgs.laplimit <= 1 && cent->startLapTime > 0) {
                    totalTime = cg.time - cent->startLapTime;
                } else if (score->time) {
                    totalTime = cg.time - score->time;
                } else {
                    totalTime = 0;
                }
                
                if (totalTime > 0) {
                    timeStr = getStringForTime(totalTime);
                    CG_DrawModernText(x, y, timeStr, 1, width, textColor, qfalse);
                } else {
                    CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
                }
            }
            break;
            
        case SBCOL_STATUS:
            if (cg.snap->ps.stats[STAT_CLIENTS_READY] & (1 << score->client)) {
                CG_DrawModernText(x, y, "READY", 1, width, readyColor, qfalse);
            } else if (ci->team != TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "WAIT", 1, width, textColor, qfalse);
            } else {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            }
            break;
            
        default:
            break;
    }
}

/*
=================
CG_DrawModernPlayerRow
Draw a single player row with adaptive columns
=================
*/
static void CG_DrawModernPlayerRow(int y, score_t *score, int rank, 
                                  qboolean isCompact, float fade) {
    int rowHeight, textY, i;
    vec4_t localHighlight;
    qboolean isLocalPlayer;
    
    if (score->client < 0 || score->client >= cgs.maxclients) {
        return;
    }
    
    /* Initialize highlight color */
    localHighlight[0] = 0.2f; localHighlight[1] = 0.4f; 
    localHighlight[2] = 0.8f; localHighlight[3] = 0.3f * fade;
    
    isLocalPlayer = (score->client == cg.snap->ps.clientNum);
    rowHeight = isCompact ? MODERN_SB_COMPACT_HEIGHT : MODERN_SB_ROW_HEIGHT;
    textY = y + (rowHeight - SMALLCHAR_HEIGHT) / 2;
    
    /* Highlight local player */
    if (isLocalPlayer) {
        localClientDrawn = qtrue;
        CG_FillRect(scoreboardX, y, currentScoreboardWidth, rowHeight, localHighlight);
    }
    
    /* Draw row background */
    CG_DrawModernBackground(scoreboardX, y, currentScoreboardWidth, rowHeight, 
                           MODERN_SB_ALPHA * fade, qfalse);
    
    /* Draw all visible columns */
    for (i = 0; i < SBCOL_MAX; i++) {
        if (!columns[i].visible) {
            continue;
        }
        
        CG_DrawColumnData(columns[i].type, columns[i].x, textY, columns[i].width,
                         score, rank, fade, isCompact);
    }
}

/*
=================
CG_DrawModernGameInfo
Draw game information header with gametype-specific info
=================
*/
static void CG_DrawModernGameInfo(int y, float fade) {
    char mainInfo[128];
    char eliminationInfo[128];
    char combinedInfo[256];
    char driversText[8];
    char roundText[8];
    int x, w;
    int leadingTeam;
    const char *teamName;
    vec4_t titleColor;
    qboolean isRacing;
    int drivers;
    int displayRound;
    int msLeft;
    qboolean showCountdown;

    /* Initialize title color */
    titleColor[0] = 1.0f; titleColor[1] = 1.0f; titleColor[2] = 1.0f; titleColor[3] = fade;

    isRacing = CG_IsRacingGametype();

    mainInfo[0] = '\0';
    eliminationInfo[0] = '\0';

    /* Draw current rank/status */
    if (!CG_IsTeamGametype()) {
        if (cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR) {
            if (isRacing) {
                Com_sprintf(mainInfo, sizeof(mainInfo), "Position %s",
                            CG_PlaceString(cg.snap->ps.persistant[PERS_RANK] + 1));
            } else {
                Com_sprintf(mainInfo, sizeof(mainInfo), "%s place with %d frags",
                            CG_PlaceString(cg.snap->ps.persistant[PERS_RANK] + 1),
                            cg.snap->ps.persistant[PERS_SCORE]);
            }
        } else {
            Q_strncpyz(mainInfo, "Spectating", sizeof(mainInfo));
        }
    } else {
        /* Team game info */
        leadingTeam = GetTeamAtRank(1);

        switch (leadingTeam) {
            case TEAM_RED:    teamName = "Red Team"; break;
            case TEAM_BLUE:   teamName = "Blue Team"; break;
            case TEAM_GREEN:  teamName = "Green Team"; break;
            case TEAM_YELLOW: teamName = "Yellow Team"; break;
            default:          teamName = "Unknown Team"; break;
        }

        if (TiedWinner()) {
            Q_strncpyz(mainInfo, "Teams tied", sizeof(mainInfo));
        } else {
            Com_sprintf(mainInfo, sizeof(mainInfo), "%s in lead", teamName);
        }
    }

    if (cgs.gametype == GT_ELIMINATION) {
        drivers = cgs.eliminationRemainingPlayers;
        if (drivers < 0) {
            drivers = 0;
        }

        displayRound = CG_EliminationDisplayRound();
        msLeft = CG_EliminationMsLeft();
        showCountdown = (cgs.eliminationActive && drivers > 1 && msLeft > 0);

        if (drivers > 0) {
            Com_sprintf(driversText, sizeof(driversText), "%i", drivers);
        } else {
            Q_strncpyz(driversText, "--", sizeof(driversText));
        }

        if (displayRound > 0) {
            Com_sprintf(roundText, sizeof(roundText), "%i", displayRound);
        } else {
            Q_strncpyz(roundText, "--", sizeof(roundText));
        }

        if (showCountdown) {
            int secondsLeft = (msLeft + 999) / 1000;
            Com_sprintf(eliminationInfo, sizeof(eliminationInfo),
                        "Drivers: %s  Round: %s  Elim in %is",
                        driversText, roundText, secondsLeft);
        } else {
            Com_sprintf(eliminationInfo, sizeof(eliminationInfo),
                        "Drivers: %s  Round: %s",
                        driversText, roundText);
        }
    }

    if (eliminationInfo[0]) {
        if (mainInfo[0]) {
            Com_sprintf(combinedInfo, sizeof(combinedInfo), "%s - %s",
                        mainInfo, eliminationInfo);
        } else {
            Q_strncpyz(combinedInfo, eliminationInfo, sizeof(combinedInfo));
        }
    } else {
        Q_strncpyz(combinedInfo, mainInfo, sizeof(combinedInfo));
    }

    if (!combinedInfo[0]) {
        Q_strncpyz(combinedInfo, " ", sizeof(combinedInfo));
    }

    w = CG_DrawStrlen(combinedInfo) * BIGCHAR_WIDTH;
    x = (SCREEN_WIDTH - w) / 2;
    CG_DrawBigStringColor(x, y, combinedInfo, titleColor);
}

/*
=================
CG_DrawModernScoreboard
Main function to draw the adaptive modern scoreboard
=================
*/
qboolean CG_DrawModernScoreboard(void) {
    int y, maxClients, rowHeight;
    int i, drawnClients;
    int team, teamClients;
    int j;
    float fade, *fadeColor;
    qboolean isCompact;
    score_t *score;
    clientInfo_t *ci;
    const cg_ladder_t *ladder;
    ladderTabInfo_t ladderTabs[MAX_LADDER_TABS];
    int ladderTabCount;
    int totalTabs;
    qboolean ladderEnabled;

    
    CG_SetScreenPlacement(PLACE_CENTER, PLACE_CENTER);
    
    /* Don't draw if paused or in single player intermission */
    if (cg_paused.integer) {
        cg.deferredPlayerLoading = 0;
        return qfalse;
    }
    
    if (cgs.gametype == GT_SINGLE_PLAYER && 
        cg.predictedPlayerState.pm_type == PM_INTERMISSION) {
        cg.deferredPlayerLoading = 0;
        return qfalse;
    }
    
    /* Don't draw during warmup unless scores are forced */
    if (cg.warmup && !cg.showScores) {
        return qfalse;
    }
    
    /* Initialize columns for current gametype */
    CG_InitScoreboardColumns();

    ladder = CG_LadderState();
    ladderTabCount = 0;
    ladderEnabled = qfalse;

    if (ladder && (ladder->status != LADDER_STATUS_EMPTY || ladder->numEntries > 0)) {
        ladderTabCount = CG_BuildLadderTabs(ladder, ladderTabs, MAX_LADDER_TABS);
        if (ladderTabCount > 0) {
            ladderEnabled = qtrue;
        }
    }

    totalTabs = 1 + (ladderEnabled ? ladderTabCount : 0);
    if (scoreboardActiveTab >= totalTabs) {
        scoreboardActiveTab = totalTabs - 1;
    }
    if (scoreboardActiveTab < 0) {
        scoreboardActiveTab = 0;
    }

    CG_HandleScoreboardTabInput(totalTabs);

    if (scoreboardActiveTab >= totalTabs) {
        scoreboardActiveTab = totalTabs - 1;
    }
    if (scoreboardActiveTab < 0) {
        scoreboardActiveTab = 0;
    }
    
    /* Calculate fade */
    if (cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD ||
        cg.predictedPlayerState.pm_type == PM_INTERMISSION) {
        fade = 1.0f;
        fadeColor = colorWhite;
    } else {
        fadeColor = CG_FadeColor(cg.scoreFadeTime, FADE_TIME);
        if (!fadeColor) {
            cg.deferredPlayerLoading = 0;
            cg.killerName[0] = 0;
            return qfalse;
        }
        fade = *fadeColor;
    }
    
    /* Request scores update */
    if (cg.scoresRequestTime + 2000 < cg.time) {
        cg.scoresRequestTime = cg.time;
        trap_SendClientCommand("score");
    }
    
    y = 80;

    if (totalTabs > 1) {
        y = CG_DrawScoreboardTabs(y, totalTabs, ladderTabs, ladderTabCount, fade);
    }

    if (ladderEnabled && scoreboardActiveTab > 0) {
        CG_DrawLadderView(y, fade, ladder, scoreboardActiveTab - 1, ladderTabs, ladderTabCount);

        if (++cg.deferredPlayerLoading > 10) {
            CG_LoadDeferredPlayers();
        }

        return qtrue;
    }

    /* Draw "Fragged by" message */
    if (cg.killerName[0]) {
        char *fragMsg;
        int w, x;
        vec4_t fragColor;

        /* Different message based on gametype */
        if (cgs.gametype == GT_DERBY) {
            fragMsg = va("Wrecked by %s", cg.killerName);
        } else if (cgs.gametype == GT_RACING || cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_TEAM_RACING) {
            fragMsg = va("Crashed by %s", cg.killerName);
        } else {
            fragMsg = va("Eliminated by %s", cg.killerName);
        }

        w = CG_DrawStrlen(fragMsg) * BIGCHAR_WIDTH;
        x = (SCREEN_WIDTH - w) / 2;

        fragColor[0] = 1.0f; fragColor[1] = 0.3f; fragColor[2] = 0.3f; fragColor[3] = fade;
        CG_DrawBigStringColor(x, y, fragMsg, fragColor);
        y += BIGCHAR_HEIGHT + 16;
    }

    /* Draw game info */
    CG_DrawModernGameInfo(y, fade);
    y += BIGCHAR_HEIGHT + 24;

    /* Determine layout mode */
    isCompact = (cg.numScores > MAX_SCOREBOARD_CLIENTS) || CG_IsTeamGametype();
    maxClients = isCompact ? MAX_COMPACT_CLIENTS : MAX_SCOREBOARD_CLIENTS;
    rowHeight = isCompact ? MODERN_SB_COMPACT_HEIGHT : MODERN_SB_ROW_HEIGHT;

    /* Draw header */
    CG_DrawModernHeader(y);
    y += MODERN_SB_HEADER_HEIGHT + 4;

    localClientDrawn = qfalse;
    drawnClients = 0;

    if (CG_IsTeamGametype()) {
        /* Team-based scoreboard */
        for (i = 0; i < 4 && drawnClients < maxClients; i++) {
            qboolean hasPlayers = qfalse;
            int teamDamage = 0;
            team = GetTeamAtRank(i + 1);
            if (team == -1) {
                continue;
            }
            
            // Check if team has players and calculate team damage
            for (j = 0; j < cg.numScores; j++) {
                score = &cg.scores[j];
                if (cgs.clientinfo[score->client].team == team) {
                    hasPlayers = qtrue;
                    teamDamage += score->damageDealt;
                }
            }

            if (!hasPlayers) {
                continue;
            }

            // Draw team header
            CG_DrawModernTeamHeaderRow(y, team, i + 1, cg.teamScores[team - TEAM_RED], teamDamage, fade);
            y += MODERN_SB_COMPACT_HEIGHT + 2;

            teamClients = 0;
            for (j = 0; j < cg.numScores && drawnClients < maxClients; j++) {
                score = &cg.scores[j];
                ci = &cgs.clientinfo[score->client];
                
                if (ci->team != team) {
                    continue;
                }
                
                CG_DrawModernPlayerRow(y, score, teamClients + 1, isCompact, fade);
                y += rowHeight + 2;
                drawnClients++;
                teamClients++;
            }
            
            if (teamClients > 0) {
                y += 8; /* Space between teams */
            }
        }
        
        /* Draw spectators */
        for (i = 0; i < cg.numScores && drawnClients < maxClients; i++) {
            score = &cg.scores[i];
            ci = &cgs.clientinfo[score->client];
            
            if (ci->team != TEAM_SPECTATOR) {
                continue;
            }
            
            CG_DrawModernPlayerRow(y, score, 0, isCompact, fade);
            y += rowHeight + 2;
            drawnClients++;
        }
    } else {
        /* Free-for-all scoreboard */
        for (i = 0; i < cg.numScores && drawnClients < maxClients; i++) {
            score = &cg.scores[i];
            ci = &cgs.clientinfo[score->client];
            
            CG_DrawModernPlayerRow(y, score, i + 1, isCompact, fade);
            y += rowHeight + 2;
            drawnClients++;
        }
    }
    
    /* Draw local client at bottom if not shown */
    if (!localClientDrawn) {
        for (i = 0; i < cg.numScores; i++) {
            if (cg.scores[i].client == cg.snap->ps.clientNum) {
                y += 16;
                CG_DrawModernPlayerRow(y, &cg.scores[i], 0, isCompact, fade);
                break;
            }
        }
    }
    
    /* Load deferred models */
    if (++cg.deferredPlayerLoading > 10) {
        CG_LoadDeferredPlayers();
    }
    
    return qtrue;
}

/*
=================
CG_DrawOldScoreboard
Wrapper function to maintain compatibility
=================
*/
qboolean CG_DrawOldScoreboard(void) {
    return CG_DrawModernScoreboard();
}

/*
=================
CG_DrawScoreboardGameModeInfo
Debug function to show current gametype and visible columns (C89 compatible)
=================
*/
void CG_DrawScoreboardGameModeInfo(void) {
    char *gametypeName;
    char columnInfo[256];
    int i, len;
    vec4_t debugColor;
    
    /* Initialize debug color */
    debugColor[0] = 0.7f; debugColor[1] = 0.7f; debugColor[2] = 0.7f; debugColor[3] = 1.0f;
    
    /* Get gametype name */
    switch (cgs.gametype) {
        case GT_RACING:           gametypeName = "Racing"; break;
        case GT_RACING_DM:        gametypeName = "Racing Deathmatch"; break;
        case GT_ELIMINATION:      gametypeName = "Elimination"; break;
        case GT_DERBY:            gametypeName = "Demolition Derby"; break;
        case GT_DEATHMATCH:       gametypeName = "Deathmatch"; break;
        case GT_LCS:              gametypeName = "Last Car Standing"; break;
        case GT_TEAM:             gametypeName = "Team Deathmatch"; break;
        case GT_TEAM_RACING:      gametypeName = "Team Racing"; break;
        case GT_TEAM_RACING_DM:   gametypeName = "Team Racing DM"; break;
        case GT_CTF:              gametypeName = "Capture The Flag"; break;
        case GT_DOMINATION:       gametypeName = "Domination"; break;
        default:                  gametypeName = "Unknown"; break;
    }
    
    /* Build column list */
    columnInfo[0] = '\0';
    len = 0;
    
    for (i = 0; i < SBCOL_MAX; i++) {
        if (!columns[i].visible) {
            continue;
        }
        
        if (len > 0) {
            Q_strcat(columnInfo, sizeof(columnInfo), ", ");
        }
        Q_strcat(columnInfo, sizeof(columnInfo), columns[i].header);
        len = strlen(columnInfo);
    }
    
    /* Draw debug info */
    CG_DrawSmallStringColor(8, SCREEN_HEIGHT - 40, 
                           va("Gametype: %s", gametypeName), debugColor);
    CG_DrawSmallStringColor(8, SCREEN_HEIGHT - 24, 
                           va("Columns: %s", columnInfo), debugColor);
} 
/*
=================
CG_GetGametypeScoreLabel
Get appropriate score label for current gametype
=================
*/
const char* CG_GetGametypeScoreLabel(void) {
    switch (cgs.gametype) {
        case GT_CTF:
            return "CAPS";
        case GT_DOMINATION:
            return "POINTS";
        case GT_DERBY:
        case GT_LCS:
            return "SCORE";
        case GT_RACING:
        case GT_ELIMINATION:
        case GT_TEAM_RACING:
            return "TIME";
        case GT_RACING_DM:
        case GT_TEAM_RACING_DM:
            return "FRAGS";
        default:
            if (CG_IsTeamGametype()) {
                return "SCORE";
            } else {
                return "FRAGS";
            }
    }
}

/*
=================
CG_GetGametypeDeathLabel
Get appropriate damage label for current gametype
=================
*/
const char* CG_GetGametypeDeathLabel(void) {
    /* Always return DMG for consistency */
    return "DMG";
}
