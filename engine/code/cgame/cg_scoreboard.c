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

/* cg_scoreboard.c -- Modern scoreboard design for Q3Rally */
#include "cg_local.h"

/* Modern scoreboard layout constants - ERWEITERT */
#define MODERN_SB_X             32
#define MODERN_SB_Y             120
#define MODERN_SB_WIDTH         650  /* Breiter fuer zusaetzliche Spalte */
#define MODERN_SB_HEADER_HEIGHT 48
#define MODERN_SB_ROW_HEIGHT    36
#define MODERN_SB_COMPACT_HEIGHT 24

/* Column definitions - streamlined layout mit LAP TIME */
#define COL_RANK_WIDTH          64
#define COL_AVATAR_WIDTH        40
#define COL_NAME_WIDTH          180  /* Etwas schmaler fuer mehr Platz */
#define COL_POINTS_WIDTH        80
#define COL_LAPTIME_WIDTH       100  /* Neue Spalte fuer Rundenzeit */
#define COL_TIME_WIDTH          120

/* Column positions */
#define COL_RANK_X              MODERN_SB_X
#define COL_AVATAR_X            (COL_RANK_X + COL_RANK_WIDTH)
#define COL_NAME_X              (COL_AVATAR_X + COL_AVATAR_WIDTH)
#define COL_POINTS_X            (COL_NAME_X + COL_NAME_WIDTH)
#define COL_LAPTIME_X           (COL_POINTS_X + COL_POINTS_WIDTH)
#define COL_TIME_X              (COL_LAPTIME_X + COL_LAPTIME_WIDTH)

/* Visual styling */
#define MODERN_SB_ALPHA         0.85f
#define MODERN_SB_BORDER_SIZE   2
#define MODERN_SB_PADDING       8
#define MODERN_SB_CORNER_RADIUS 4

/* Maximum clients display */
#define MAX_SCOREBOARD_CLIENTS  12
#define MAX_COMPACT_CLIENTS     16

static qboolean localClientDrawn;

/*
=================
CG_DrawModernBackground
Draw a modern styled background with subtle gradients
=================
*/
static void CG_DrawModernBackground(int x, int y, int width, int height, 
                                   float alpha, qboolean isHeader) {
    vec4_t bgColor = {0.08f, 0.10f, 0.12f, 0.8f};
    vec4_t borderColor = {0.2f, 0.25f, 0.3f, 0.6f};
    
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
    
    if (!text) return;
    
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
Draw the modern scoreboard header
=================
*/
static void CG_DrawModernHeader(int y) {
    vec4_t headerTextColor = {0.9f, 0.9f, 0.9f, 1.0f};
    
    /* Draw header background */
    CG_DrawModernBackground(MODERN_SB_X, y, MODERN_SB_WIDTH, MODERN_SB_HEADER_HEIGHT, 
                           MODERN_SB_ALPHA, qtrue);
    
    /* Draw column headers */
    CG_DrawModernText(COL_RANK_X, y + 12, "POS", 1, COL_RANK_WIDTH, headerTextColor, qtrue);
    CG_DrawModernText(COL_NAME_X, y + 12, "PLAYER", 0, COL_NAME_WIDTH, headerTextColor, qtrue);
    CG_DrawModernText(COL_POINTS_X, y + 12, "PTS", 1, COL_POINTS_WIDTH, headerTextColor, qtrue);
    CG_DrawModernText(COL_LAPTIME_X, y + 12, "BEST", 1, COL_LAPTIME_WIDTH, headerTextColor, qtrue);
    CG_DrawModernText(COL_TIME_X, y + 12, "TOTAL", 1, COL_TIME_WIDTH, headerTextColor, qtrue);
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
CG_DrawModernPlayerRow
Draw a single player row with modern styling
=================
*/
static void CG_DrawModernPlayerRow(int y, score_t *score, int rank, 
                                  qboolean isCompact, float fade) {
    clientInfo_t *ci;
    char rankStr[16], pointsStr[16];
    char *timeStr, *lapTimeStr;
    int totalTime, lapTime, rowHeight;
    vec4_t rankColor = {0.9f, 0.9f, 0.9f, 1.0f};
    vec4_t playerColor = {0.9f, 0.9f, 0.9f, 1.0f}; 
    vec4_t localHighlight = {0.2f, 0.4f, 0.8f, 0.3f};
    vec4_t teamColor = {0.9f, 0.9f, 0.9f, 1.0f};
    vec4_t botColor = {0.5f, 0.8f, 0.5f, 1.0f};
    vec4_t readyColor = {0.3f, 0.8f, 0.3f, 1.0f};
    qboolean isLocalPlayer;
    int avatarSize, textY;
    
    if (score->client < 0 || score->client >= cgs.maxclients) {
        return;
    }
    
    ci = &cgs.clientinfo[score->client];
    isLocalPlayer = (score->client == cg.snap->ps.clientNum);
    rowHeight = isCompact ? MODERN_SB_COMPACT_HEIGHT : MODERN_SB_ROW_HEIGHT;
    avatarSize = rowHeight - 8;
    textY = y + (rowHeight - SMALLCHAR_HEIGHT) / 2;
    
    /* Calculate time display */
    if (cg_entities[score->client].finishRaceTime) {
        totalTime = cg_entities[score->client].finishRaceTime - score->time;
    } else if (score->time) {
        totalTime = cg.time - score->time;
    } else {
        totalTime = 0;
    }
    
    if (totalTime < 0) totalTime = 0;
    timeStr = getStringForTime(totalTime);
    
    /* Calculate lap time - nutzt Q3Rally-spezifische Felder */
    if (cg_entities[score->client].bestLapTime) {
        lapTime = cg_entities[score->client].bestLapTime;
    } else {
        lapTime = 0;
    }
    
    if (lapTime < 0) lapTime = 0;
    lapTimeStr = getStringForTime(lapTime);
    
    /* Prepare display strings */
    if (ci->team == TEAM_SPECTATOR) {
        Com_sprintf(rankStr, sizeof(rankStr), "SPEC");
        Com_sprintf(pointsStr, sizeof(pointsStr), "-");
    } else if (cg_entities[score->client].currentPosition > 0) {
        Com_sprintf(rankStr, sizeof(rankStr), "%d", cg_entities[score->client].currentPosition);
        Com_sprintf(pointsStr, sizeof(pointsStr), "%d", score->score);
    } else {
        Com_sprintf(rankStr, sizeof(rankStr), "-");
        Com_sprintf(pointsStr, sizeof(pointsStr), "%d", score->score);
    }
    
    /* Highlight local player */
    if (isLocalPlayer) {
        localClientDrawn = qtrue;
        localHighlight[0] = 0.2f; localHighlight[1] = 0.4f; 
        localHighlight[2] = 0.8f; localHighlight[3] = 0.3f * fade;
        CG_FillRect(MODERN_SB_X, y, MODERN_SB_WIDTH, rowHeight, localHighlight);
    }
    
    /* Draw row background */
    CG_DrawModernBackground(MODERN_SB_X, y, MODERN_SB_WIDTH, rowHeight, 
                           MODERN_SB_ALPHA * fade, qfalse);
    
    /* Get colors */
    if (ci->team == TEAM_SPECTATOR) {
        playerColor[0] = 0.6f; playerColor[1] = 0.6f; 
        playerColor[2] = 0.6f; playerColor[3] = fade;
        rankColor[0] = 0.6f; rankColor[1] = 0.6f; 
        rankColor[2] = 0.6f; rankColor[3] = fade;
    } else {
        CG_GetRankColor(cg_entities[score->client].currentPosition, rankColor);
        rankColor[3] = fade;
        playerColor[0] = 0.9f; playerColor[1] = 0.9f; 
        playerColor[2] = 0.9f; playerColor[3] = fade;
    }
    
    /* Draw player avatar */
    if (ci->modelIcon) {
        CG_DrawPic(COL_AVATAR_X + (COL_AVATAR_WIDTH - avatarSize) / 2, 
                   y + (rowHeight - avatarSize) / 2, 
                   avatarSize, avatarSize, ci->modelIcon);
    }
    
    /* Draw rank with special styling for top 3 */
    if (cg_entities[score->client].currentPosition <= 3 && 
        cg_entities[score->client].currentPosition > 0) {
        CG_DrawModernText(COL_RANK_X, textY, rankStr, 1, COL_RANK_WIDTH, 
                         rankColor, qtrue);
    } else {
        CG_DrawModernText(COL_RANK_X, textY, rankStr, 1, COL_RANK_WIDTH, 
                         playerColor, qfalse);
    }
    
    /* Draw player name with team color if applicable */
    if (cgs.gametype >= GT_TEAM && ci->team != TEAM_SPECTATOR) {
        CG_GetModernTeamColor(ci->team, teamColor);
        teamColor[3] = fade;
        CG_DrawModernText(COL_NAME_X, textY, ci->name, 0, COL_NAME_WIDTH, 
                         teamColor, qfalse);
    } else {
        CG_DrawModernText(COL_NAME_X, textY, ci->name, 0, COL_NAME_WIDTH, 
                         playerColor, qfalse);
    }
    
    /* Draw points */
    CG_DrawModernText(COL_POINTS_X, textY, pointsStr, 1, COL_POINTS_WIDTH, 
                     playerColor, qfalse);
    
    /* Draw lap time */
    if (lapTimeStr && strcmp(lapTimeStr, "0:00:00") != 0) {
        CG_DrawModernText(COL_LAPTIME_X, textY, lapTimeStr, 1, COL_LAPTIME_WIDTH, 
                         playerColor, qfalse);
    } else {
        CG_DrawModernText(COL_LAPTIME_X, textY, "-", 1, COL_LAPTIME_WIDTH, 
                         playerColor, qfalse);
    }
    
    /* Draw total time */
    if (timeStr && strcmp(timeStr, "0:00:00") != 0) {
        CG_DrawModernText(COL_TIME_X, textY, timeStr, 1, COL_TIME_WIDTH, 
                         playerColor, qfalse);
    } else {
        CG_DrawModernText(COL_TIME_X, textY, "-", 1, COL_TIME_WIDTH, 
                         playerColor, qfalse);
    }
    
    /* Draw connection status indicators */
    if (ci->botSkill > 0 && ci->botSkill <= 5) {
        /* Bot indicator */
        botColor[3] = fade;
        CG_DrawSmallStringColor(COL_NAME_X + COL_NAME_WIDTH - 32, textY, "BOT", botColor);
    }
    
    /* Draw ready status in intermission */
    if (cg.snap->ps.stats[STAT_CLIENTS_READY] & (1 << score->client)) {
        readyColor[3] = fade;
        CG_DrawSmallStringColor(COL_TIME_X + COL_TIME_WIDTH + 8, textY, "RDY", readyColor);
    }
}

/*
=================
CG_DrawModernGameInfo
Draw game information header
=================
*/
static void CG_DrawModernGameInfo(int y, float fade) {
    char *gameInfo;
    int x, w;
    int leadingTeam;
    char *teamName;
    vec4_t titleColor = {1.0f, 1.0f, 1.0f, 1.0f};
    
    titleColor[3] = fade;
    
    /* Draw current rank/status */
    if (cgs.gametype < GT_TEAM) {
        if (cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR) {
            gameInfo = va("%s place",
                         CG_PlaceString(cg.snap->ps.persistant[PERS_RANK] + 1));
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
    
    w = CG_DrawStrlen(gameInfo) * BIGCHAR_WIDTH;
    x = (SCREEN_WIDTH - w) / 2;
    CG_DrawBigStringColor(x, y, gameInfo, titleColor);
}

/*
=================
CG_DrawModernScoreboard
Main function to draw the modern scoreboard
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
        char *fragMsg = va("Eliminated by %s", cg.killerName);
        int w = CG_DrawStrlen(fragMsg) * BIGCHAR_WIDTH;
        int x = (SCREEN_WIDTH - w) / 2;
        vec4_t fragColor = {1.0f, 0.3f, 0.3f, 1.0f};
        fragColor[3] = fade;
        CG_DrawBigStringColor(x, y, fragMsg, fragColor);
        y += BIGCHAR_HEIGHT + 16;
    }
    
    /* Draw game info */
    CG_DrawModernGameInfo(y, fade);
    y += BIGCHAR_HEIGHT + 24;
    
    /* Determine layout mode */
    isCompact = (cg.numScores > MAX_SCOREBOARD_CLIENTS);
    maxClients = isCompact ? MAX_COMPACT_CLIENTS : MAX_SCOREBOARD_CLIENTS;
    rowHeight = isCompact ? MODERN_SB_COMPACT_HEIGHT : MODERN_SB_ROW_HEIGHT;
    
    /* Draw header */
    CG_DrawModernHeader(y);
    y += MODERN_SB_HEADER_HEIGHT + 4;
    
    localClientDrawn = qfalse;
    drawnClients = 0;
    
    if (cgs.gametype >= GT_TEAM) {
        /* Team-based scoreboard */
        for (i = 0; i < 4 && drawnClients < maxClients; i++) {
            team = GetTeamAtRank(i + 1);
            if (team == -1) continue;
            
            teamClients = 0;
            for (j = 0; j < cg.numScores && drawnClients < maxClients; j++) {
                score = &cg.scores[j];
                ci = &cgs.clientinfo[score->client];
                
                if (ci->team != team) continue;
                
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
            
            if (ci->team != TEAM_SPECTATOR) continue;
            
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
