/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

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

/* Modern scoreboard layout constants */
#define MODERN_SB_Y             120
#define MODERN_SB_WIDTH         650  
#define MODERN_SB_MIN_WIDTH     620  /* Keep the frame inside a 4:3 viewport */
#define MODERN_SB_HEADER_HEIGHT 40
#define MODERN_SB_ROW_HEIGHT    36
#define MODERN_SB_COMPACT_HEIGHT 20

/* Column definitions - flexible layout */
#define COL_RANK_WIDTH          64
#define COL_AVATAR_WIDTH        40
#define COL_NAME_WIDTH          180
#define COL_SCORE_WIDTH         80   /* For frags/points */
#define COL_DEATHS_WIDTH        60   /* For deaths */
#define COL_LAPTIME_WIDTH       100  /* Best lap time */
#define COL_DELTA_WIDTH         90   /* Local ghost split delta */
#define COL_TOTALTIME_WIDTH     120  /* Total time */
#define COL_PING_WIDTH          60   /* Ping */
#define COL_STATUS_WIDTH        96   /* Status (race result / ready) */
#define COL_KOTH_CAPTURE_WIDTH  64
#define COL_KOTH_DEFEND_WIDTH   64
#define COL_KOTH_HILLKILLS_WIDTH 64
#define KOTH_SCOREBOARD_WIDTH   552

/* Visual styling */
#define MODERN_SB_ALPHA         0.68f
#define MODERN_SB_BORDER_SIZE   2
#define MODERN_SB_PADDING       8
#define MODERN_SB_CORNER_RADIUS 4
#define MODERN_SB_HEADING_SCALE 0.52f
#define MODERN_SB_TEXT_SCALE    0.74f
#define MODERN_SB_TITLE_SCALE   0.72f

/* Maximum clients display */
#define MAX_SCOREBOARD_CLIENTS  12
#define MAX_COMPACT_CLIENTS     16

/* Scoreboard column types */
typedef enum {
    SBCOL_RANK,
    SBCOL_AVATAR,
    SBCOL_NAME,
    SBCOL_SCORE,      /* Frags/Points */
    SBCOL_DEATHS,     /* Deaths */
    SBCOL_LAPTIME,    /* Best lap time */
    SBCOL_DELTA,      /* Local ghost split delta */
    SBCOL_TOTALTIME,  /* Total/Race time */
    SBCOL_PING,       /* Network ping */
    SBCOL_STATUS,     /* Ready status, etc */
    SBCOL_KOTH_CAPS,  /* Q3Rally KOTH: captures */
    SBCOL_KOTH_DEF,   /* Q3Rally KOTH: defends */
    SBCOL_KOTH_HILL_KILLS,
    SBCOL_KOTH_CONTEST_TIME,
    SBCOL_KOTH_HOLD_TIME,
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

/*
=================
CG_IsRacingGametype
Helper function to determine if current gametype is racing-based
=================
*/
static qboolean CG_IsRacingGametype(void) {
    return (cgs.gametype == GT_RACING ||
            cgs.gametype == GT_SPRINT ||
            cgs.gametype == GT_TEAM_RACING ||
            cgs.gametype == GT_RACING_DM ||
            cgs.gametype == GT_TEAM_RACING_DM ||
            cgs.gametype == GT_ELIMINATION ||
            cgs.gametype == GT_LCS ||
            cgs.gametype == GT_DERBY ||
            cgs.gametype == GT_SINGLE_PLAYER);
}

/* Gametypes where finishRaceTime means a race finish, not an elimination. */
static qboolean CG_HasRaceFinishStatus(void) {
    return (cgs.gametype == GT_RACING ||
            cgs.gametype == GT_SPRINT ||
            cgs.gametype == GT_TEAM_RACING ||
            cgs.gametype == GT_RACING_DM ||
            cgs.gametype == GT_TEAM_RACING_DM ||
            cgs.gametype == GT_SINGLE_PLAYER);
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
            cgs.gametype == GT_DOMINATION ||
            cgs.gametype == GT_KOTH); /* Q3Rally KOTH */
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
    int minimumScoreboardWidth;
    qboolean showScore, showDeaths, showTimes, showLapTimes, showDelta;
    qboolean isRacing, isTeam;
    qboolean ghostDeltaEnabled;
    
    /* Clear all columns first */
    for (i = 0; i < SBCOL_MAX; i++) {
        columns[i].visible = qfalse;
        columns[i].x = 0;
    }
    
    /* Determine gametype characteristics */
    isRacing = CG_IsRacingGametype();
    isTeam = CG_IsTeamGametype();
    ghostDeltaEnabled = (cg_ghostPlayback.integer != 0);
    
    /* Determine what to show based on gametype */
    showScore = qfalse;
    showDeaths = qfalse;
    showTimes = qfalse;
    showLapTimes = qfalse;
    showDelta = qfalse;
    
    switch (cgs.gametype) {
        case GT_RACING:
        case GT_SPRINT:
        case GT_TEAM_RACING:
        case GT_SINGLE_PLAYER:
            /* Pure racing - only times matter */
            showTimes = qtrue;
            showLapTimes = qtrue;
            showDelta = ghostDeltaEnabled;
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

        case GT_KOTH: /* Q3Rally KOTH */
            showScore = qtrue;
            showDeaths = qfalse;
            break;
            
        case GT_RACING_DM:
        case GT_TEAM_RACING_DM:
            /* Racing with weapons - both matter */
            showScore = qtrue;
            showTimes = qtrue;
            showLapTimes = qtrue;
            showDelta = ghostDeltaEnabled;
            /* No deaths in racing modes typically */
            break;
            
        case GT_DERBY:
        case GT_LCS:
        case GT_ELIMINATION:
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
        if (cgs.gametype == GT_CTF || cgs.gametype == GT_CTF4) {
            columns[SBCOL_SCORE].header = "CAPS";
        } else if (cgs.gametype == GT_KOTH) {
            columns[SBCOL_SCORE].header = "PTS";
        } else if (cgs.gametype == GT_DOMINATION) {
            columns[SBCOL_SCORE].header = "POINTS";
        } else if (cgs.gametype == GT_DERBY || cgs.gametype == GT_LCS || cgs.gametype == GT_ELIMINATION) {
            columns[SBCOL_SCORE].header = "SCORE";
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

    if (showDelta) {
        columns[SBCOL_DELTA].type = SBCOL_DELTA;
        columns[SBCOL_DELTA].width = COL_DELTA_WIDTH;
        columns[SBCOL_DELTA].header = "DELTA";
        columns[SBCOL_DELTA].visible = qtrue;
    }
    
    if (showTimes) {
        columns[SBCOL_TOTALTIME].type = SBCOL_TOTALTIME;
        columns[SBCOL_TOTALTIME].width = COL_TOTALTIME_WIDTH;
        
        /* Different header based on racing type */
        if (cgs.gametype == GT_RACING || cgs.gametype == GT_SPRINT || cgs.gametype == GT_TEAM_RACING || cgs.gametype == GT_SINGLE_PLAYER) {
            columns[SBCOL_TOTALTIME].header = "RACE TIME";
        } else {
            columns[SBCOL_TOTALTIME].header = "TOTAL";
        }
        columns[SBCOL_TOTALTIME].visible = qtrue;
    }
    
    /* Q3Rally KOTH result stats - only shown in intermission. */
    if (cgs.gametype == GT_KOTH &&
        cg.predictedPlayerState.pm_type == PM_INTERMISSION) {
        /* KOTH has several useful result stats; use compact cells so the
         * complete table stays comfortably inside the 640px HUD canvas. */
        columns[SBCOL_RANK].width = 42;
        columns[SBCOL_AVATAR].width = 34;
        columns[SBCOL_NAME].width = 126;
        columns[SBCOL_SCORE].width = 50;
        columns[SBCOL_KOTH_CAPS].type    = SBCOL_KOTH_CAPS;
        columns[SBCOL_KOTH_CAPS].width   = 38;
        columns[SBCOL_KOTH_CAPS].header  = "CAP";
        columns[SBCOL_KOTH_CAPS].visible = qtrue;

        columns[SBCOL_KOTH_DEF].type    = SBCOL_KOTH_DEF;
        columns[SBCOL_KOTH_DEF].width   = 38;
        columns[SBCOL_KOTH_DEF].header  = "DEF";
        columns[SBCOL_KOTH_DEF].visible = qtrue;

        columns[SBCOL_KOTH_HILL_KILLS].type    = SBCOL_KOTH_HILL_KILLS;
        columns[SBCOL_KOTH_HILL_KILLS].width   = 38;
        columns[SBCOL_KOTH_HILL_KILLS].header  = "HK";
        columns[SBCOL_KOTH_HILL_KILLS].visible = qtrue;

        columns[SBCOL_KOTH_CONTEST_TIME].type    = SBCOL_KOTH_CONTEST_TIME;
        columns[SBCOL_KOTH_CONTEST_TIME].width   = 68;
        columns[SBCOL_KOTH_CONTEST_TIME].header  = "CONT.";
        columns[SBCOL_KOTH_CONTEST_TIME].visible = qtrue;

        columns[SBCOL_KOTH_HOLD_TIME].type    = SBCOL_KOTH_HOLD_TIME;
        columns[SBCOL_KOTH_HOLD_TIME].width   = 68;
        columns[SBCOL_KOTH_HOLD_TIME].header  = "HOLD";
        columns[SBCOL_KOTH_HOLD_TIME].visible = qtrue;
    }

    /* Status column only in intermission - no ping column */
    if (cg.predictedPlayerState.pm_type == PM_INTERMISSION &&
        cgs.gametype != GT_KOTH) {
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
    minimumScoreboardWidth = ( cgs.gametype == GT_KOTH &&
                               cg.predictedPlayerState.pm_type == PM_INTERMISSION ) ?
                              KOTH_SCOREBOARD_WIDTH : MODERN_SB_MIN_WIDTH;
    currentScoreboardWidth = totalContentWidth;
    if (currentScoreboardWidth < minimumScoreboardWidth) {
        currentScoreboardWidth = minimumScoreboardWidth;
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
    vec4_t accentColor;
    float fillAlpha;
    
    accentColor[0] = 0.72f; accentColor[1] = 1.0f;
    accentColor[2] = 0.06f; accentColor[3] = alpha;
    if (isHeader) {
        bgColor[0] = 0.008f; bgColor[1] = 0.012f;
        bgColor[2] = 0.016f; bgColor[3] = alpha;
        borderColor[0] = 0.24f; borderColor[1] = 0.34f;
        borderColor[2] = 0.36f; borderColor[3] = alpha * 0.72f;
        fillAlpha = alpha;
    } else {
        bgColor[0] = 0.018f; bgColor[1] = 0.027f;
        bgColor[2] = 0.031f; bgColor[3] = alpha * 0.62f;
        borderColor[0] = 0.24f; borderColor[1] = 0.34f;
        borderColor[2] = 0.36f; borderColor[3] = alpha * 0.42f;
        fillAlpha = alpha * 0.62f;
    }
    bgColor[3] = fillAlpha;
    
    CG_FillRect(x, y, width, height, bgColor);
    if (isHeader) {
        CG_FillRect(x, y, width, MODERN_SB_BORDER_SIZE, accentColor);
    }
    CG_FillRect(x, y + height - 1, width, 1, borderColor);
}

/*
=================
CG_DrawModernText
Draw text with modern styling and proper alignment
=================
*/
static void CG_DrawModernText(int x, int y, const char *text, int align, 
                             int columnWidth, float *color, qboolean isBold) {
    int textWidth, drawX, fontStyle;
    float scale;
    
    if (!text) {
        return;
    }
    
    fontStyle = isBold ? 0 : UI_SMALLFONT;
    scale = isBold ? MODERN_SB_HEADING_SCALE : MODERN_SB_TEXT_SCALE;
    textWidth = CG_IngameStringWidth(text, fontStyle, scale);
    
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
    
    CG_DrawIngameString(drawX, y, text, fontStyle | UI_DROPSHADOW,
                        scale, color ? color : colorWhite);
}

/*
=================
CG_DrawModernHeader
Draw the adaptive scoreboard header
=================
*/
static void CG_DrawModernHeader(int y, float fade) {
    vec4_t headerTextColor;
    int i;
    
    /* Initialize header text color */
    headerTextColor[0] = 0.47f; headerTextColor[1] = 0.62f;
    headerTextColor[2] = 0.61f; headerTextColor[3] = fade;
    
    /* Draw header background */
    CG_DrawModernBackground(scoreboardX, y, currentScoreboardWidth, MODERN_SB_HEADER_HEIGHT,
                           MODERN_SB_ALPHA * fade, qtrue);
    
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
            case SBCOL_DELTA:
            case SBCOL_TOTALTIME:
            case SBCOL_STATUS:
            case SBCOL_KOTH_CAPS:
            case SBCOL_KOTH_DEF:
            case SBCOL_KOTH_HILL_KILLS:
            case SBCOL_KOTH_CONTEST_TIME:
            case SBCOL_KOTH_HOLD_TIME:
                /* Center-aligned columns */
                CG_DrawModernText(columns[i].x,
                                 y + (MODERN_SB_HEADER_HEIGHT - (int)(24 * MODERN_SB_HEADING_SCALE)) / 2,
                                 columns[i].header, 1,
                                 columns[i].width, headerTextColor, qtrue);
                break;
            case SBCOL_NAME:
                /* Left-aligned text columns */
                CG_DrawModernText(columns[i].x,
                                 y + (MODERN_SB_HEADER_HEIGHT - (int)(24 * MODERN_SB_HEADING_SCALE)) / 2,
                                 columns[i].header, 0,
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


/*
=================
CG_DrawModernTeamHeaderRow
Draws a header row for a team in the scoreboard
=================
*/
static void CG_DrawModernTeamHeaderRow(int y, team_t team, int rank, int score, int damage, int teamTime, int kothHoldTimeMs, float fade) {
    char teamText[128];
    char scoreText[64];
    char hillText[32];
    vec4_t teamColor;
    vec4_t textColor;
    vec4_t metricDividerColor;
    int rowHeight = MODERN_SB_COMPACT_HEIGHT; // Use compact height for team headers
    int textY = y + (rowHeight - BIGCHAR_HEIGHT) / 2;

    /* Background for the team header */
    CG_DrawModernBackground(scoreboardX, y, currentScoreboardWidth, rowHeight, MODERN_SB_ALPHA * fade, qtrue);

    /* Team Rank and Name */
    CG_GetModernTeamColor(team, teamColor);
    teamColor[3] = fade;
    Com_sprintf(teamText, sizeof(teamText), "%d. %s", rank, CG_GetTeamName(team));
    CG_DrawModernText(scoreboardX, textY, teamText, 0, currentScoreboardWidth, teamColor, qtrue);

    /* Team score — show average race time for team racing and objective points for KOTH. */
    textColor[0] = 0.9f; textColor[1] = 0.9f; textColor[2] = 0.9f; textColor[3] = fade;
    metricDividerColor[0] = 0.24f;
    metricDividerColor[1] = 0.34f;
    metricDividerColor[2] = 0.36f;
    metricDividerColor[3] = fade * 0.7f;
    if ( cgs.gametype == GT_TEAM_RACING || cgs.gametype == GT_TEAM_RACING_DM ) {
        if ( teamTime > 0 ) {
            if ( cgs.gametype == GT_TEAM_RACING_DM && score > 0 ) {
                /* Show average time and frag bonus so players can see both */
                Com_sprintf(scoreText, sizeof(scoreText), "avg %s  +%d frags", getStringForTimePrecise(teamTime), score);
            } else {
                Com_sprintf(scoreText, sizeof(scoreText), "avg %s", getStringForTimePrecise(teamTime));
            }
        } else {
            Com_sprintf(scoreText, sizeof(scoreText), "avg --:--:---");
        }
    } else if ( cgs.gametype == GT_KOTH ) {
        int holdSecs = kothHoldTimeMs / 1000;
        int metricRight = scoreboardX + currentScoreboardWidth - 8;
        int hillWidth = 132;
        int scoreWidth = 100;
        int dividerX = metricRight - hillWidth - 9;
        int scoreX = dividerX - scoreWidth - 9;

        Com_sprintf(scoreText, sizeof(scoreText), "%d PTS", score);
        Com_sprintf(hillText, sizeof(hillText), "HILL %d:%02d",
                    holdSecs / 60, holdSecs % 60);
        CG_DrawModernText(scoreX, textY, scoreText, 2, scoreWidth, textColor, qtrue);
        CG_FillRect(dividerX, y + 5, 1, rowHeight - 10, metricDividerColor);
        CG_DrawModernText(dividerX + 9, textY, hillText, 2, hillWidth, textColor, qtrue);
    } else {
        Com_sprintf(scoreText, sizeof(scoreText), "%d pts / %d dmg", score, damage);
    }
    if ( cgs.gametype != GT_KOTH ) {
        CG_DrawModernText(scoreboardX, textY, scoreText, 2, currentScoreboardWidth, textColor, qtrue);
    }
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
    char buffer[32];
    int totalTime, lapTime, avatarSize, rowHeight;
    int deltaMs, absMs;
    char sign;
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
    rowHeight = isCompact ? MODERN_SB_COMPACT_HEIGHT : MODERN_SB_ROW_HEIGHT;
    avatarSize = rowHeight - 8;
    isRacingMode = CG_IsRacingGametype();
    
    switch (colType) {
        case SBCOL_RANK:
            if (ci->team == TEAM_SPECTATOR) {
                Com_sprintf(buffer, sizeof(buffer), "SPEC");
                CG_DrawModernText(x, y, buffer, 1, width, textColor, qfalse);
            } else if (isRacingMode &&
                       cgs.gametype != GT_DERBY && cgs.gametype != GT_LCS &&
                       score->position > 0) {
                /* Pure racing modes: use STAT_POSITION set by server */
                Com_sprintf(buffer, sizeof(buffer), "%d", score->position);
                CG_GetRankColor(score->position, rankColor);
                rankColor[3] = fade;
                CG_DrawModernText(x, y, buffer, 1, width, rankColor,
                                 (score->position <= 3) ? qtrue : qfalse);
            } else if (rank > 0) {
                /* Derby, LCS, and all non-racing modes: use sorted rank from server */
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
                /* Show damageTaken (damage received by this player) */
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
//                    lapTimeStr = getStringForTime(lapTime);
                      lapTimeStr = getStringForTimePrecise(lapTime);
                    CG_DrawModernText(x, y, lapTimeStr, 1, width, textColor, qfalse);
                } else {
                    CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
                }
            }
            break;

        case SBCOL_DELTA:
            if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            } else if (cg.snap && score->client == cg.snap->ps.clientNum && cg.ghostSplitDeltaValid) {
                deltaMs = cg.ghostSplitDeltaMs;
                sign = deltaMs < 0 ? '-' : '+';
                absMs = deltaMs < 0 ? -deltaMs : deltaMs;
                Com_sprintf(buffer, sizeof(buffer), "%c%d.%03d", sign, absMs / 1000, absMs % 1000);
                CG_DrawModernText(x, y, buffer, 1, width, textColor, qfalse);
            } else if (cg.snap && score->client == cg.snap->ps.clientNum) {
                CG_DrawModernText(x, y, "- - -", 1, width, textColor, qfalse);
            } else {
                CG_DrawModernText(x, y, "- - -", 1, width, textColor, qfalse);
            }
            break;
            
        case SBCOL_TOTALTIME:
            if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            } else {
                if (cg_entities[score->client].finishRaceTime) {
                    totalTime = cg_entities[score->client].finishRaceTime - score->time;
                } else if (score->time) {
                    totalTime = cg.time - score->time;
                } else {
                    totalTime = 0;
                }
                
                if (totalTime > 0) {
//                    timeStr = getStringForTime(totalTime);
                      timeStr = getStringForTimePrecise(totalTime);
                    CG_DrawModernText(x, y, timeStr, 1, width, textColor, qfalse);
                } else {
                    CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
                }
            }
            break;
            
        case SBCOL_STATUS:
            if (cgs.gametype == GT_ELIMINATION) {
                if (cg_entities[score->client].eliminationOut) {
                    vec4_t outColor;
                    outColor[0] = 1.0f;
                    outColor[1] = 0.25f;
                    outColor[2] = 0.20f;
                    outColor[3] = fade;
                    CG_DrawModernText(x, y, "OUT", 1, width, outColor, qfalse);
                } else if (ci->team == TEAM_SPECTATOR) {
                    CG_DrawModernText(x, y, "SPEC", 1, width, textColor, qfalse);
                } else if (cg.predictedPlayerState.pm_type == PM_INTERMISSION && rank == 1) {
                    CG_DrawModernText(x, y, "WINNER", 1, width, readyColor, qfalse);
                } else {
                    CG_DrawModernText(x, y, "ALIVE", 1, width, readyColor, qfalse);
                }
            } else if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "SPEC", 1, width, textColor, qfalse);
            } else if (cgs.gametype == GT_DERBY) {
                qboolean wrecked = (score->integrity == 0 ||
                                    (cg_entities[score->client].currentState.eFlags & EF_DEAD));
                if (cg.predictedPlayerState.pm_type == PM_INTERMISSION && rank == 1 && !wrecked) {
                    CG_DrawModernText(x, y, "WINNER", 1, width, readyColor, qfalse);
                } else if (wrecked) {
                    vec4_t wreckedColor;
                    wreckedColor[0] = 1.0f;
                    wreckedColor[1] = 0.38f;
                    wreckedColor[2] = 0.20f;
                    wreckedColor[3] = fade;
                    CG_DrawModernText(x, y, "WRECKED", 1, width, wreckedColor, qfalse);
                } else {
                    CG_DrawModernText(x, y, "ALIVE", 1, width, readyColor, qfalse);
                }
            } else if (CG_HasRaceFinishStatus() &&
                       cg_entities[score->client].startRaceTime > 0) {
                if (cg_entities[score->client].finishRaceTime > 0) {
                    CG_DrawModernText(x, y, "FINISHED", 1, width, readyColor, qfalse);
                } else {
                    vec4_t dnfColor;
                    dnfColor[0] = 1.0f;
                    dnfColor[1] = 0.45f;
                    dnfColor[2] = 0.20f;
                    dnfColor[3] = fade;
                    CG_DrawModernText(x, y, "DNF", 1, width, dnfColor, qfalse);
                }
            } else if (cg.snap && (cg.snap->ps.stats[STAT_CLIENTS_READY] & (1 << score->client))) {
                CG_DrawModernText(x, y, "READY", 1, width, readyColor, qfalse);
            } else if (ci->team != TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "WAIT", 1, width, textColor, qfalse);
            } else {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            }
            break;

        case SBCOL_KOTH_CAPS:
            if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            } else {
                Com_sprintf(buffer, sizeof(buffer), "%d", score->captures);
                CG_DrawModernText(x, y, buffer, 1, width, textColor, qfalse);
            }
            break;

        case SBCOL_KOTH_DEF:
            if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            } else {
                Com_sprintf(buffer, sizeof(buffer), "%d", score->defendCount);
                CG_DrawModernText(x, y, buffer, 1, width, textColor, qfalse);
            }
            break;

        case SBCOL_KOTH_HILL_KILLS:
            if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            } else {
                Com_sprintf(buffer, sizeof(buffer), "%d", score->kothHillKills);
                CG_DrawModernText(x, y, buffer, 1, width, textColor, qfalse);
            }
            break;

        case SBCOL_KOTH_CONTEST_TIME:
            if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            } else {
                int contestSecs = score->kothContestTimeMs / 1000;
                Com_sprintf(buffer, sizeof(buffer), "%d:%02d", contestSecs / 60, contestSecs % 60);
                CG_DrawModernText(x, y, buffer, 1, width, textColor, qfalse);
            }
            break;

        case SBCOL_KOTH_HOLD_TIME:
            if (ci->team == TEAM_SPECTATOR) {
                CG_DrawModernText(x, y, "-", 1, width, textColor, qfalse);
            } else {
                int holdSecs = score->kothHoldTimeMs / 1000;
                Com_sprintf(buffer, sizeof(buffer), "%d:%02d", holdSecs / 60, holdSecs % 60);
                CG_DrawModernText(x, y, buffer, 1, width, textColor, qfalse);
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
                                  qboolean isCompact, float fade,
                                  int lastPlaceClient, int lastPlacePosition,
                                  int playersRemaining) {
    int rowHeight, textY, i;
    vec4_t localHighlight;
    vec4_t localAccent;
    qboolean isLocalPlayer;
    
    if (score->client < 0 || score->client >= cgs.maxclients) {
        return;
    }
    
    /* Initialize highlight color */
    localHighlight[0] = 0.06f; localHighlight[1] = 0.14f;
    localHighlight[2] = 0.08f; localHighlight[3] = 0.32f * fade;
    localAccent[0] = 0.72f; localAccent[1] = 1.0f;
    localAccent[2] = 0.06f; localAccent[3] = fade;

    isLocalPlayer = (score->client == cg.snap->ps.clientNum);
    rowHeight = isCompact ? MODERN_SB_COMPACT_HEIGHT : MODERN_SB_ROW_HEIGHT;
    textY = y + (rowHeight - (int)(14 * MODERN_SB_TEXT_SCALE)) / 2;

    {
        vec4_t lastHighlight;
        qboolean isLastPlace = qfalse;

        lastHighlight[0] = 0.8f; lastHighlight[1] = 0.2f;
        lastHighlight[2] = 0.2f; lastHighlight[3] = 0.35f * fade;

        if (CG_IsRacingGametype() && playersRemaining > 1 && lastPlacePosition > 0) {
            if ((score->client == lastPlaceClient || score->position == lastPlacePosition) &&
                CG_IsActiveCompetitor(score->client)) {
                isLastPlace = qtrue;
            }
        }

        if (isLocalPlayer) {
            localClientDrawn = qtrue;
        }

        CG_DrawModernBackground(scoreboardX, y, currentScoreboardWidth, rowHeight,
                               MODERN_SB_ALPHA * fade, qfalse);

        if (isLastPlace) {
            CG_FillRect(scoreboardX, y, currentScoreboardWidth, rowHeight, lastHighlight);
        }

        if (isLocalPlayer) {
            CG_FillRect(scoreboardX, y, currentScoreboardWidth, rowHeight, localHighlight);
            CG_FillRect(scoreboardX, y, 2, rowHeight, localAccent);
        }
    }

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
static void CG_DrawModernGameInfo(int y, float fade,
                                  int playersRemaining,
                                  int lastPlaceClient) {
    char *gameInfo;
    int x, w;
    int leadingTeam;
    char *teamName;
    vec4_t titleColor;
    qboolean isRacing;
    qboolean isEliminationMode;
    clientInfo_t *lastPlaceInfo;
    char remainingText[64];
    int remainingY;
    
    /* Initialize title color */
    titleColor[0] = 0.72f; titleColor[1] = 1.0f;
    titleColor[2] = 0.06f; titleColor[3] = fade;
    
    isRacing = CG_IsRacingGametype();
    isEliminationMode = (cgs.gametype == GT_ELIMINATION ||
                         cgs.gametype == GT_LCS);
    
    /* Draw current rank/status */
    if (!CG_IsTeamGametype()) {
        if (cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR) {
            if (isRacing) {
                gameInfo = va("Position %s",
                             CG_PlaceString(cg.snap->ps.persistant[PERS_RANK] + 1));
            } else {
                gameInfo = va("%s place with %d frags",
                             CG_PlaceString(cg.snap->ps.persistant[PERS_RANK] + 1),
                             cg.snap->ps.persistant[PERS_SCORE]);
            }
        } else {
            gameInfo = "Spectating";
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
            gameInfo = va("Teams tied");
        } else {
            gameInfo = va("%s in lead", teamName);
        }
    }
    
    w = CG_IngameStringWidth(gameInfo, 0, MODERN_SB_TITLE_SCALE);
    x = (SCREEN_WIDTH - w) / 2;
    CG_DrawIngameString(x, y, gameInfo, UI_DROPSHADOW,
                        MODERN_SB_TITLE_SCALE, titleColor);

    if (isEliminationMode && playersRemaining > 0) {
        remainingY = y + BIGCHAR_HEIGHT + 4;

        Com_sprintf(remainingText, sizeof(remainingText),
                    "Players Remaining: %d", playersRemaining);

        if (lastPlaceClient >= 0 && lastPlaceClient < cgs.maxclients) {
            lastPlaceInfo = &cgs.clientinfo[lastPlaceClient];
            if (lastPlaceInfo->infoValid && lastPlaceInfo->name[0]) {
                Q_strcat(remainingText, sizeof(remainingText),
                         va("  |  Last: %s", lastPlaceInfo->name));
            }
        }

        w = CG_IngameStringWidth(remainingText, UI_SMALLFONT, MODERN_SB_TEXT_SCALE);
        x = (SCREEN_WIDTH - w) / 2;
        CG_DrawIngameString(x, remainingY, remainingText,
                            UI_SMALLFONT | UI_DROPSHADOW,
                            MODERN_SB_TEXT_SCALE, titleColor);
    }
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
    int playersRemaining;
    int lastPlaceClient;
    int lastPlacePosition;
    qboolean isRacingGametype;
    int topMargin, bottomMargin, headerHeight, availableHeight;
    int rowSpacing, maxRows, baseMaxClients;
    int maxRowsStandard;
    qboolean showOverflowRow;

    
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
    
    /* Draw "Fragged by" message */
    if (cg.killerName[0]) {
        char *fragMsg;
        int w, x;
        vec4_t fragColor;
        
        /* Different message based on gametype */
        if (cgs.gametype == GT_DERBY) {
            fragMsg = va("Wrecked by %s", cg.killerName);
        } else if (cgs.gametype == GT_RACING || cgs.gametype == GT_SPRINT || cgs.gametype == GT_TEAM_RACING) {
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
    
    /* Determine elimination status info */
    playersRemaining = 0;
    lastPlaceClient = -1;
    lastPlacePosition = -1;
    isRacingGametype = CG_IsRacingGametype();

    if (isRacingGametype) {
        playersRemaining = CG_GetPlayersRemaining(&lastPlaceClient);
        if (lastPlaceClient >= 0 && lastPlaceClient < cgs.maxclients) {
            lastPlacePosition = cg_entities[lastPlaceClient].currentPosition;
            if (lastPlacePosition <= 0) {
                lastPlacePosition = cgs.clientinfo[lastPlaceClient].position;
            }
        }

        if (lastPlacePosition <= 0 && playersRemaining > 0) {
            lastPlacePosition = playersRemaining;
        }
    }

    /* Draw game info */
    CG_DrawModernGameInfo(y, fade, playersRemaining, lastPlaceClient);
    y += BIGCHAR_HEIGHT + 24;
    
    /* Determine layout mode */
    topMargin = y;
    bottomMargin = 32;
    headerHeight = MODERN_SB_HEADER_HEIGHT + 4;
    rowSpacing = 2;
    availableHeight = SCREEN_HEIGHT - topMargin - bottomMargin - headerHeight;
    if (availableHeight < MODERN_SB_COMPACT_HEIGHT) {
        availableHeight = MODERN_SB_COMPACT_HEIGHT;
    }

    maxRowsStandard = availableHeight / (MODERN_SB_ROW_HEIGHT + rowSpacing);
    isCompact = (cg.numScores > MAX_SCOREBOARD_CLIENTS) ||
                CG_IsTeamGametype() ||
                (cg.numScores > maxRowsStandard);

    rowHeight = isCompact ? MODERN_SB_COMPACT_HEIGHT : MODERN_SB_ROW_HEIGHT;
    maxRows = availableHeight / (rowHeight + rowSpacing);

    if (isCompact && cg.numScores > maxRows && cg.numScores > 0) {
        int adjustedHeight;
        int minRowHeight = 14;

        adjustedHeight = (availableHeight / cg.numScores) - rowSpacing;
        if (adjustedHeight < minRowHeight) {
            adjustedHeight = minRowHeight;
        }
        if (adjustedHeight < rowHeight) {
            rowHeight = adjustedHeight;
            maxRows = availableHeight / (rowHeight + rowSpacing);
        }
    }

    if (maxRows < 1) {
        maxRows = 1;
    }

    baseMaxClients = isCompact ? MAX_COMPACT_CLIENTS : MAX_SCOREBOARD_CLIENTS;
    maxClients = baseMaxClients;
    if (maxClients > maxRows) {
        maxClients = maxRows;
    }
    if (maxClients < 1) {
        maxClients = 1;
    }

    showOverflowRow = qfalse;
    if (cg.numScores > maxClients && maxClients > 1) {
        maxClients--;
        showOverflowRow = qtrue;
    } else if (cg.numScores > maxClients && maxRows > maxClients) {
        showOverflowRow = qtrue;
    }
    
    /* Draw header */
    CG_DrawModernHeader(y, fade);
    y += MODERN_SB_HEADER_HEIGHT + 4;
    
    localClientDrawn = qfalse;
    drawnClients = 0;
    
    if (CG_IsTeamGametype()) {
        /* Team-based scoreboard */
        for (i = 0; i < 4 && drawnClients < maxClients; i++) {
            qboolean hasPlayers = qfalse;
            int teamDamage = 0;
            int teamKothHoldTimeMs = 0;
            team = GetTeamAtRank(i + 1);
            if (team == -1) {
                continue;
            }
            
            // Check if team has players and calculate team damage
            for (j = 0; j < cg.numScores; j++) {
                score = &cg.scores[j];
                if (cgs.clientinfo[score->client].team == team) {
                    hasPlayers = qtrue;
                    if ( cgs.gametype != GT_KOTH ) {
                        teamDamage += score->damageDealt;
                    } else {
                        teamKothHoldTimeMs = score->kothTeamHoldTimeMs;
                    }
                }
            }

            if (!hasPlayers) {
                continue;
            }

            // Draw team header
            CG_DrawModernTeamHeaderRow(y, team, i + 1, cg.teamScores[team - TEAM_RED], teamDamage, cg.teamTimes[team - TEAM_RED], teamKothHoldTimeMs, fade);
            y += MODERN_SB_COMPACT_HEIGHT + 2;

            teamClients = 0;
            for (j = 0; j < cg.numScores && drawnClients < maxClients; j++) {
                score = &cg.scores[j];
                ci = &cgs.clientinfo[score->client];
                
                if (ci->team != team) {
                    continue;
                }
                
                CG_DrawModernPlayerRow(y, score, teamClients + 1, isCompact, fade,
                                       lastPlaceClient, lastPlacePosition,
                                       playersRemaining);
                y += rowHeight + rowSpacing;
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
            
            CG_DrawModernPlayerRow(y, score, 0, isCompact, fade,
                                   lastPlaceClient, lastPlacePosition,
                                   playersRemaining);
            y += rowHeight + rowSpacing;
            drawnClients++;
        }
    } else {
        /* Free-for-all scoreboard */
        for (i = 0; i < cg.numScores && drawnClients < maxClients; i++) {
            score = &cg.scores[i];
            ci = &cgs.clientinfo[score->client];

            CG_DrawModernPlayerRow(y, score, i + 1, isCompact, fade,
                                   lastPlaceClient, lastPlacePosition,
                                   playersRemaining);
            y += rowHeight + rowSpacing;
            drawnClients++;
        }
    }

    if (showOverflowRow && drawnClients < maxRows) {
        vec4_t overflowColor;
        int textY;

        overflowColor[0] = 0.8f; overflowColor[1] = 0.8f;
        overflowColor[2] = 0.8f; overflowColor[3] = fade;
        textY = y + (rowHeight - SMALLCHAR_HEIGHT) / 2;

        CG_DrawModernBackground(scoreboardX, y, currentScoreboardWidth, rowHeight,
                                MODERN_SB_ALPHA * fade, qfalse);
        CG_DrawModernText(scoreboardX, textY, "Weitere Spieler...",
                          1, currentScoreboardWidth, overflowColor, qfalse);
        y += rowHeight + rowSpacing;
    }
    
    /* Draw local client at bottom if not shown */
    if (!localClientDrawn) {
        for (i = 0; i < cg.numScores; i++) {
            if (cg.scores[i].client == cg.snap->ps.clientNum) {
                y += 16;
                CG_DrawModernPlayerRow(y, &cg.scores[i], 0, isCompact, fade,
                                       lastPlaceClient, lastPlacePosition,
                                       playersRemaining);
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
        case GT_SPRINT:           gametypeName = "Sprint"; break;
        case GT_DERBY:            gametypeName = "Demolition Derby"; break;
        case GT_DEATHMATCH:       gametypeName = "Deathmatch"; break;
        case GT_LCS:              gametypeName = "Last Car Standing"; break;
        case GT_ELIMINATION:      gametypeName = "Elimination"; break;
        case GT_TEAM:             gametypeName = "Team Deathmatch"; break;
        case GT_TEAM_RACING:      gametypeName = "Team Racing"; break;
        case GT_TEAM_RACING_DM:   gametypeName = "Team Racing DM"; break;
        case GT_CTF:              gametypeName = "Capture The Flag"; break;
        case GT_CTF4:             gametypeName = "4-Team CTF"; break;
        case GT_DOMINATION:       gametypeName = "Domination"; break;
        case GT_KOTH:              gametypeName = "King of the Hill"; break; /* Q3Rally KOTH */
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
        case GT_CTF4:
            return "CAPS";
        case GT_DOMINATION:
            return "POINTS";
        case GT_KOTH:
            return "PTS"; /* Q3Rally KOTH */
        case GT_DERBY:
        case GT_LCS:
        case GT_ELIMINATION:
            return "SCORE";
        case GT_RACING:
        case GT_SPRINT:
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
