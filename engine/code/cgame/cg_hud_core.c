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

/*
===========================================================================
  cg_hud_core.c

  Central HUD dispatcher:
    - Cvar registration for all HUD element toggles
    - CG_DrawHUD()          : main entry point called each frame
    - CG_DrawUpperRightHUD(): legacy right-side racing panels
    - CG_DrawLowerRightHUD(): speedometer area
    - CG_DrawLowerLeftHUD() : rear-weapon ammo area
    - CG_DrawHUDOptionsMenu(): in-game overlay to toggle elements
===========================================================================
*/

#include "cg_local.h"
#include "cg_hud_elements.h"

/* -----------------------------------------------------------------------
   Shared constants used across all HUD modules
   ----------------------------------------------------------------------- */
#define HUD_RIGHT_EDGE          636.0f
#define HUD_COLUMN_SPACING        4.0f
#define HUD_TEXT_INSET            6.0f
#define HUD_ROW_HEIGHT          ((float)TINYCHAR_HEIGHT + 4.0f)

/* -----------------------------------------------------------------------
   Shared colour palette (also used by racing / derby / vehicle modules)
   ----------------------------------------------------------------------- */
float colors[4][4] = {
    { 1.0f, 0.69f, 0.0f, 1.0f },   /* normal            */
    { 1.0f, 0.2f,  0.2f, 1.0f },   /* low health        */
    { 0.5f, 0.5f,  0.5f, 1.0f },   /* weapon firing     */
    { 1.0f, 1.0f,  1.0f, 1.0f }    /* health > 100      */
};

/* -----------------------------------------------------------------------
   HUD element toggle cvars  (definitions – declared extern in header)
   ----------------------------------------------------------------------- */
vmCvar_t  cg_hudShowTimes;
vmCvar_t  cg_hudShowLaps;
vmCvar_t  cg_hudShowPosition;
vmCvar_t  cg_hudShowDistToFinish;
vmCvar_t  cg_hudShowCarAheadBehind;
vmCvar_t  cg_hudShowOpponentList;
vmCvar_t  cg_hudShowScores;

vmCvar_t  cg_hudShowSpeed;
vmCvar_t  cg_hudShowFuelGauge;

vmCvar_t  cg_hudShowDerbyVehicle;
vmCvar_t  cg_hudShowDerbyList;
vmCvar_t  cg_hudShowKothHillStatus;
vmCvar_t  cg_hudShowKothRespawnWave;

/* Menu open/close toggle – registered in CG_HUD_RegisterCvars, updated each frame */
static vmCvar_t cg_hudOptionsOpen;

/* -----------------------------------------------------------------------
   CG_HUD_RegisterCvars
   Call once from CG_RegisterCvars() in cg_main.c
   ----------------------------------------------------------------------- */
void CG_HUD_RegisterCvars( void ) {
    /* Menu toggle – bind F9 "toggle cg_hudOptionsOpen" */
    trap_Cvar_Register( &cg_hudOptionsOpen,        "cg_hudOptionsOpen",        "0", CVAR_ARCHIVE );

    /* Racing */
    trap_Cvar_Register( &cg_hudShowTimes,          "cg_hudShowTimes",          "1", CVAR_ARCHIVE );
    trap_Cvar_Register( &cg_hudShowLaps,           "cg_hudShowLaps",           "1", CVAR_ARCHIVE );
    trap_Cvar_Register( &cg_hudShowPosition,       "cg_hudShowPosition",       "1", CVAR_ARCHIVE );
    trap_Cvar_Register( &cg_hudShowDistToFinish,   "cg_hudShowDistToFinish",   "1", CVAR_ARCHIVE );
    trap_Cvar_Register( &cg_hudShowCarAheadBehind, "cg_hudShowCarAheadBehind", "1", CVAR_ARCHIVE );
    trap_Cvar_Register( &cg_hudShowOpponentList,   "cg_hudShowOpponentList",   "1", CVAR_ARCHIVE );
    trap_Cvar_Register( &cg_hudShowScores,         "cg_hudShowScores",         "1", CVAR_ARCHIVE );

    /* Vehicle */
    trap_Cvar_Register( &cg_hudShowSpeed,          "cg_hudShowSpeed",          "1", CVAR_ARCHIVE );
    trap_Cvar_Register( &cg_hudShowFuelGauge,      "cg_hudShowFuelGauge",      "1", CVAR_ARCHIVE );

    /* Derby */
    trap_Cvar_Register( &cg_hudShowDerbyVehicle,   "cg_hudShowDerbyVehicle",   "1", CVAR_ARCHIVE );
    trap_Cvar_Register( &cg_hudShowDerbyList,      "cg_hudShowDerbyList",      "1", CVAR_ARCHIVE );

    /* KOTH */
    trap_Cvar_Register( &cg_hudShowKothHillStatus,  "cg_hudShowKothHillStatus",  "1", CVAR_ARCHIVE );
    trap_Cvar_Register( &cg_hudShowKothRespawnWave, "cg_hudShowKothRespawnWave", "1", CVAR_ARCHIVE );
}

/* -----------------------------------------------------------------------
   CG_GetEliminationColumnWidth  (shared utility, stays in core)
   ----------------------------------------------------------------------- */
float CG_GetEliminationColumnWidth( void ) {
    static float columnWidth = 0.0f;

    if ( columnWidth <= 0.0f ) {
        const float insetWidth = HUD_TEXT_INSET * 2.0f;
        float       maxWidth   = insetWidth +
            CG_IngameStringWidth( "T: 00:00.000", UI_SMALLFONT, 0.75f );
        float       candidate;

#define CHECK_CANDIDATE(str) \
        candidate = insetWidth + CG_IngameStringWidth( str, UI_SMALLFONT, 0.75f ); \
        if ( candidate > maxWidth ) { maxWidth = candidate; }

        CHECK_CANDIDATE( "L: 00:00.000" )
        CHECK_CANDIDATE( "B: 00:00.000" )
        CHECK_CANDIDATE( "LAP: 00/00" )
        CHECK_CANDIDATE( "POS: 00/00" )
        CHECK_CANDIDATE( "DIST: 0000m" )
        CHECK_CANDIDATE( "DIST: 100.0%" )
        CHECK_CANDIDATE( "D: +00.000" )
        CHECK_CANDIDATE( "PLAYERS LEFT: 000" )
        CHECK_CANDIDATE( "R99 LEFT63 Name (99s)" )
#undef CHECK_CANDIDATE

        columnWidth = maxWidth;
    }

    return columnWidth;
}


/* =======================================================================
   HUD OPTIONS MENU
   A lightweight in-game overlay rendered when cg.showHUDOptions is set.
   Bind a key in the .cfg:   bind F9 "toggle cg_hudOptionsOpen"
   ======================================================================= */

/*
 * Layout:  A translucent panel centred on screen.
 * Each row shows a label and a [ON]/[OFF] toggle drawn as coloured text.
 * Clicking is handled by tracking mouse position against row bounds –
 * the engine passes cursor data via CG_MouseEvent() which you hook below.
 *
 * NOTE: This uses the existing Q3 UI drawing primitives only (no new trap
 * calls needed), keeping it compatible with the ioq3 / OpenArena VM ABI.
 */

/* ---- 2-column layout, centred in 640x480 ----
 * Panel starts below the "Press FIRE or USE when ready to race" centre-print
 * which draws at y≈232-248 (SCREEN_HEIGHT/2 ± BIGCHAR_HEIGHT/2).          */
#define HUDOPT_PAD          10.0f   /* outer padding                          */
#define HUDOPT_PNL_W       560.0f   /* total panel width                      */
#define HUDOPT_PNL_X        40.0f   /* (640 - 560) / 2                        */
#define HUDOPT_PNL_Y       160.0f   /* 75px above previous position (262-75) */
#define HUDOPT_COL_W       255.0f   /* usable width per column                */
#define HUDOPT_COL_GAP      30.0f   /* gap between columns (divider lives here)*/
#define HUDOPT_COL_L_X     ( HUDOPT_PNL_X + HUDOPT_PAD )
#define HUDOPT_COL_R_X     ( HUDOPT_COL_L_X + HUDOPT_COL_W + HUDOPT_COL_GAP )
#define HUDOPT_ROW_H        19.0f
#define HUDOPT_TITLE_H      18.0f   /* BIGCHAR title row                      */
#define HUDOPT_HINT_H       12.0f   /* TINYCHAR hint row                      */
#define HUDOPT_SEC_H        16.0f   /* section header height                  */
#define HUDOPT_TITLE_SCALE   0.72f
#define HUDOPT_HINT_SCALE    0.50f
#define HUDOPT_SECTION_SCALE 0.58f
#define HUDOPT_ENTRY_SCALE   0.58f
#define HUDOPT_SLIDER_SCALE  0.50f
#define HUDOPT_SLIDER_TRACK_X ( HUDOPT_COL_R_X + 128.0f )
#define HUDOPT_SLIDER_TRACK_W ( HUDOPT_COL_W - 182.0f )
#define HUDOPT_MMAP_SLIDERS   2
#define HUDOPT_SLIDER_NONE    0
#define HUDOPT_SLIDER_SCALE_ID 1
#define HUDOPT_SLIDER_ZOOM_ID  2
/* Text sizes reuse the engine's built-in char constants:
 * Title  → BIGCHAR,  sections/entries → SMALLCHAR / TINYCHAR               */
/* Left col: Racing (10 entries, indices 0-9)
 * Right col: Derby (3, indices 10-12) + KOTH (2, indices 13-14) + Vehicle (3, indices 15-17) */
#define HUDOPT_LEFT_COUNT   10
#define HUDOPT_DERBY_START  10
#define HUDOPT_DERBY_COUNT   3
#define HUDOPT_KOTH_START   13
#define HUDOPT_KOTH_COUNT    2
#define HUDOPT_VEH_START    15
#define HUDOPT_VEH_COUNT     3

typedef struct {
    const char  *label;
    const char  *cvarName;
    vmCvar_t    *cvar;
    int          onValue;       /* value to set when toggling ON; also max cycle value */
    int          gameTypeOnly;  /* GT_* value, or -1 for always visible                */
    qboolean     isCycler;      /* qtrue: cycle 0..onValue instead of simple toggle    */
} hudToggleEntry_t;

static const hudToggleEntry_t hudToggleTable[] = {
    /* ---- Racing (left column, indices 0-9) ---- */
    { "TIMES PANEL",         "cg_hudShowTimes",          &cg_hudShowTimes,          1, -1,       qfalse },
    { "LAP COUNTER",         "cg_hudShowLaps",           &cg_hudShowLaps,           1, -1,       qfalse },
    { "RACE POSITION",       "cg_hudShowPosition",       &cg_hudShowPosition,       1, -1,       qfalse },
    { "DISTANCE TO FINISH",  "cg_hudShowDistToFinish",   &cg_hudShowDistToFinish,   1, -1,       qfalse },
    { "GHOST DELTA",         "cg_ghostPlayback",         &cg_ghostPlayback,         1, -1,       qfalse },
    { "CHECKPOINT ARROW",    "cg_checkpointArrowMode",   &cg_checkpointArrowMode,   2, -1,       qtrue  },
    { "CARS AHEAD/BEHIND",   "cg_hudShowCarAheadBehind", &cg_hudShowCarAheadBehind, 1, -1,       qfalse },
    { "ELIM. TIMELINE",      "cg_elimTimeline",          &cg_elimTimeline,          1, -1,       qfalse },
    { "OPPONENT LIST",       "cg_hudShowOpponentList",   &cg_hudShowOpponentList,   1, -1,       qfalse },
    { "SCORES PANEL",        "cg_hudShowScores",         &cg_hudShowScores,         1, -2,       qfalse },
    /* ---- Derby (right column top, indices 10-12) ---- */
    { "DERBY VEHICLE STATE", "cg_hudShowDerbyVehicle",  &cg_hudShowDerbyVehicle,   1, GT_DERBY, qfalse },
    { "DERBY SCOREBOARD",    "cg_hudShowDerbyList",      &cg_hudShowDerbyList,      1, GT_DERBY, qfalse },
    { "DERBY HIT IMPACT",    "cg_derbyHitFxEnable",      &cg_derbyHitFxEnable,      1, GT_DERBY, qfalse },
    /* ---- KOTH (right column middle, indices 13-14) ---- */
    { "KOTH HILL STATUS",   "cg_hudShowKothHillStatus",  &cg_hudShowKothHillStatus,  1, GT_KOTH,  qfalse },
    { "KOTH RESPAWN WAVE",  "cg_hudShowKothRespawnWave", &cg_hudShowKothRespawnWave, 1, GT_KOTH,  qfalse },
    /* ---- Vehicle (right column bottom, indices 15-17) ---- */
    { "SPEEDOMETER",         "cg_hudShowSpeed",          &cg_hudShowSpeed,          1, -1,       qfalse },
    /* Fuel Gauge is part of Speedometer – hidden when Speedometer is OFF */
    { "REAR-VIEW MIRROR",    "cg_drawRearView",          &cg_drawRearView,          1, -1,       qfalse },
    { "MINI-MAP",            "cg_drawMMap",              &cg_drawMMap,              1, -1,       qfalse },
};

#define HUDOPT_NUM_ENTRIES  ( (int)( sizeof(hudToggleTable) / sizeof(hudToggleTable[0]) ) )

/* Track which row the cursor hovers over (-1 = none) */
static int      g_hudOptHoverRow  = -1;
static int      g_hudOptHoverSlider = HUDOPT_SLIDER_NONE;

static void HUDOpt_DrawText( int x, int y, const char *text, vec4_t color, float scale ) {
    CG_DrawIngameString( x, y, text, UI_LEFT|UI_DROPSHADOW|UI_SMALLFONT,
                         scale, color );
}

static void HUDOpt_DrawCenteredText( float x, float width, int y, const char *text, vec4_t color, float scale ) {
    CG_DrawIngameString( (int)( x + width * 0.5f ), y, text,
                         UI_CENTER|UI_DROPSHADOW|UI_SMALLFONT, scale, color );
}

static void HUDOpt_DrawRightText( float right, int y, const char *text, vec4_t color, float scale ) {
    CG_DrawIngameString( (int)right, y, text,
                         UI_RIGHT|UI_DROPSHADOW|UI_SMALLFONT, scale, color );
}

static float HUDOpt_ClampFloat( float value, float minValue, float maxValue ) {
    if ( value < minValue ) {
        return minValue;
    }
    if ( value > maxValue ) {
        return maxValue;
    }
    return value;
}

static void HUDOpt_SetFloatCvar( const char *cvarName, vmCvar_t *cvar, float value, qboolean wholeNumber ) {
    if ( wholeNumber ) {
        trap_Cvar_Set( cvarName, va( "%.0f", value ) );
    } else {
        trap_Cvar_Set( cvarName, va( "%.2f", value ) );
    }
    trap_Cvar_Update( cvar );
}

static void HUDOpt_SetSliderFromX( int sliderId, int cx ) {
    float t = ( (float)cx - HUDOPT_SLIDER_TRACK_X ) / HUDOPT_SLIDER_TRACK_W;
    t = HUDOpt_ClampFloat( t, 0.0f, 1.0f );

    if ( sliderId == HUDOPT_SLIDER_SCALE_ID ) {
        float value = 0.40f + t * 1.60f;
        value = ( (int)( value * 20.0f + 0.5f ) ) / 20.0f;
        HUDOpt_SetFloatCvar( "cg_mmap_size", &cg_mmap_size, value, qfalse );
    } else if ( sliderId == HUDOPT_SLIDER_ZOOM_ID ) {
        float value = 30.0f + t * 90.0f;
        value = ( (int)( value / 5.0f + 0.5f ) ) * 5.0f;
        HUDOpt_SetFloatCvar( "cg_mmap_fov", &cg_mmap_fov, value, qtrue );
    }
}

static void HUDOpt_DrawSlider( float x, float y, const char *label, float value, float minValue,
                               float maxValue, int hoverId, vec4_t labelColor, vec4_t valueColor,
                               vec4_t trackColor, vec4_t fillColor, vec4_t hoverColor ) {
    char    valueText[16];
    float   t;
    float   knobX;

    t = HUDOpt_ClampFloat( ( value - minValue ) / ( maxValue - minValue ), 0.0f, 1.0f );
    knobX = HUDOPT_SLIDER_TRACK_X + t * HUDOPT_SLIDER_TRACK_W;

    if ( g_hudOptHoverSlider == hoverId ) {
        CG_FillRect( x - 2.0f, y, HUDOPT_COL_W + 4.0f, HUDOPT_ROW_H, hoverColor );
        CG_FillRect( x - 2.0f, y, 2.0f, HUDOPT_ROW_H, fillColor );
    }

    if ( hoverId == HUDOPT_SLIDER_ZOOM_ID ) {
        Com_sprintf( valueText, sizeof( valueText ), "%.0f", value );
    } else {
        Com_sprintf( valueText, sizeof( valueText ), "%.2f", value );
    }

    HUDOpt_DrawText( (int)( x + 3.0f ), (int)( y + 3.0f ), label, labelColor, HUDOPT_SLIDER_SCALE );
    CG_FillRect( HUDOPT_SLIDER_TRACK_X, y + 10.0f, HUDOPT_SLIDER_TRACK_W, 2.0f, trackColor );
    CG_FillRect( HUDOPT_SLIDER_TRACK_X, y + 10.0f, knobX - HUDOPT_SLIDER_TRACK_X, 2.0f, fillColor );
    CG_FillRect( knobX - 2.0f, y + 7.0f, 4.0f, 8.0f, fillColor );
    HUDOpt_DrawRightText( x + HUDOPT_COL_W, (int)( y + 3.0f ), valueText, valueColor, HUDOPT_SLIDER_SCALE );
}

/*
================
HUDEntry_IsUnavail
Returns qtrue when a toggle entry is not applicable in the current gametype.
  gameTypeOnly == -1  : always available
  gameTypeOnly == -2  : only in non-race DM modes (FFA, Team, etc.)
  gameTypeOnly >= 0   : only when cgs.gametype matches exactly
================
*/
static qboolean HUDEntry_IsUnavail( const hudToggleEntry_t *e ) {
    if ( e->gameTypeOnly == -1 )
        return qfalse;
    if ( e->gameTypeOnly == -2 )
        return ( isRallyNonDMRace() || cgs.gametype == GT_DERBY || cgs.gametype == GT_LCS );
    return ( cgs.gametype != e->gameTypeOnly );
}

/*
================
HUDEntry_DoBadgeLabel
Returns the badge string for a given entry, reflecting cycler states.
================
*/
static const char *HUDEntry_BadgeLabel( const hudToggleEntry_t *e ) {
    if ( e->isCycler ) {
        switch ( e->cvar->integer ) {
        case 0:  return "OFF";
        case 1:  return "ON HUD";
        case 2:  return "AB CAR";
        default: return "?";
        }
    }
    return e->cvar->integer ? "ON" : "OFF";
}

/*
================
HUDEntry_DoToggle
Advances a cycler or flips a simple toggle.
================
*/
static void HUDEntry_DoToggle( const hudToggleEntry_t *e ) {
    int next;
    if ( e->isCycler ) {
        next = e->cvar->integer + 1;
        if ( next > e->onValue ) next = 0;
    } else {
        next = e->cvar->integer ? 0 : e->onValue;
    }
    trap_Cvar_Set( e->cvarName, va("%i", next) );
    trap_Cvar_Update( e->cvar );
}


/*
================
CG_HUDOptionsIsOpen
Safe accessor for cg_main.c (cg_hudOptionsOpen is static here).
================
*/
qboolean CG_HUDOptionsIsOpen( void ) {
    return cg_hudOptionsOpen.integer ? qtrue : qfalse;
}

/*
================
CG_HUDOptions_KeyEvent
Keyboard navigation: Up/Down to move, Enter/Space to toggle, Escape to close.
================
*/
void CG_HUDOptions_KeyEvent( int key ) {
    if ( !cg_hudOptionsOpen.integer ) return;

    if ( key == 128 /* K_UPARROW */ ) {
        int next = g_hudOptHoverRow;
        do {
            next--;
            if ( next < 0 ) next = HUDOPT_NUM_ENTRIES - 1;
        } while ( HUDEntry_IsUnavail( &hudToggleTable[next] ) &&
                  next != g_hudOptHoverRow );
        g_hudOptHoverRow = next;
    } else if ( key == 129 /* K_DOWNARROW */ ) {
        int next = g_hudOptHoverRow;
        do {
            next++;
            if ( next >= HUDOPT_NUM_ENTRIES ) next = 0;
        } while ( HUDEntry_IsUnavail( &hudToggleTable[next] ) &&
                  next != g_hudOptHoverRow );
        g_hudOptHoverRow = next;
    } else if ( key == 13 /* K_ENTER */ || key == 32 /* K_SPACE */ ) {
        if ( g_hudOptHoverRow >= 0 && g_hudOptHoverRow < HUDOPT_NUM_ENTRIES ) {
            const hudToggleEntry_t *e = &hudToggleTable[g_hudOptHoverRow];
            if ( !HUDEntry_IsUnavail( e ) ) {
                HUDEntry_DoToggle( e );
            }
        }
    } else if ( key == 27 /* K_ESCAPE */ ) {
        /* cg_main.c CG_KeyEvent handles the key catcher release */
        trap_Cvar_Set( "cg_hudOptionsOpen", "0" );
        trap_Cvar_Update( &cg_hudOptionsOpen );
    }
}

/*
================
CG_HUDOptions_MouseEvent
Forward raw mouse deltas here from CG_MouseEvent().
cx/cy are *absolute* cursor coordinates in 640x480 space.
Returns qtrue if the menu consumed the click.
================
*/
qboolean CG_HUDOptions_MouseEvent( int cx, int cy, qboolean clicked ) {
    int     i;
    float   rowY, secY;

    if ( !cg_hudOptionsOpen.integer ) {
        return qfalse;
    }

    g_hudOptHoverRow = -1;
    g_hudOptHoverSlider = HUDOPT_SLIDER_NONE;

    secY = HUDOPT_PNL_Y + HUDOPT_TITLE_H + HUDOPT_HINT_H + HUDOPT_PAD;

    /* Left column: Racing (0 .. HUDOPT_LEFT_COUNT-1) */
    rowY = secY + HUDOPT_SEC_H;
    for ( i = 0; i < HUDOPT_LEFT_COUNT; i++ ) {
        if ( cy >= rowY && cy < rowY + HUDOPT_ROW_H &&
             cx >= HUDOPT_COL_L_X - 2.0f &&
             cx <  HUDOPT_COL_L_X + HUDOPT_COL_W + 4.0f ) {
            if ( !HUDEntry_IsUnavail( &hudToggleTable[i] ) ) {
                g_hudOptHoverRow = i;
                if ( clicked ) HUDEntry_DoToggle( &hudToggleTable[i] );
            }
            return qtrue;
        }
        rowY += HUDOPT_ROW_H;
    }

    /* Right column Derby section */
    rowY = secY + HUDOPT_SEC_H;
    for ( i = HUDOPT_DERBY_START; i < HUDOPT_DERBY_START + HUDOPT_DERBY_COUNT; i++ ) {
        if ( cy >= rowY && cy < rowY + HUDOPT_ROW_H &&
             cx >= HUDOPT_COL_R_X - 2.0f &&
             cx <  HUDOPT_COL_R_X + HUDOPT_COL_W + 4.0f ) {
            if ( !HUDEntry_IsUnavail( &hudToggleTable[i] ) ) {
                g_hudOptHoverRow = i;
                if ( clicked ) HUDEntry_DoToggle( &hudToggleTable[i] );
            }
            return qtrue;
        }
        rowY += HUDOPT_ROW_H;
    }

	/* Right column KOTH section (separator adds 6px) */
	rowY += 6.0f + HUDOPT_SEC_H;
	for ( i = HUDOPT_KOTH_START; i < HUDOPT_KOTH_START + HUDOPT_KOTH_COUNT; i++ ) {
		if ( cy >= rowY && cy < rowY + HUDOPT_ROW_H &&
		     cx >= HUDOPT_COL_R_X - 2.0f &&
		     cx <  HUDOPT_COL_R_X + HUDOPT_COL_W + 4.0f ) {
			if ( !HUDEntry_IsUnavail( &hudToggleTable[i] ) ) {
				g_hudOptHoverRow = i;
				if ( clicked ) HUDEntry_DoToggle( &hudToggleTable[i] );
			}
			return qtrue;
		}
		rowY += HUDOPT_ROW_H;
	}

	/* Right column Vehicle section (separator adds 6px) */
	rowY += 6.0f + HUDOPT_SEC_H;
	for ( i = HUDOPT_VEH_START; i < HUDOPT_VEH_START + HUDOPT_VEH_COUNT; i++ ) {
        if ( cy >= rowY && cy < rowY + HUDOPT_ROW_H &&
             cx >= HUDOPT_COL_R_X - 2.0f &&
             cx <  HUDOPT_COL_R_X + HUDOPT_COL_W + 4.0f ) {
            if ( !HUDEntry_IsUnavail( &hudToggleTable[i] ) ) {
                g_hudOptHoverRow = i;
                if ( clicked ) HUDEntry_DoToggle( &hudToggleTable[i] );
            }
            return qtrue;
        }
        rowY += HUDOPT_ROW_H;
    }

    if ( cy >= rowY && cy < rowY + HUDOPT_ROW_H &&
         cx >= HUDOPT_COL_R_X - 2.0f &&
         cx <  HUDOPT_COL_R_X + HUDOPT_COL_W + 4.0f ) {
        g_hudOptHoverSlider = HUDOPT_SLIDER_SCALE_ID;
        if ( clicked ) {
            HUDOpt_SetSliderFromX( HUDOPT_SLIDER_SCALE_ID, cx );
        }
        return qtrue;
    }
    rowY += HUDOPT_ROW_H;

    if ( cy >= rowY && cy < rowY + HUDOPT_ROW_H &&
         cx >= HUDOPT_COL_R_X - 2.0f &&
         cx <  HUDOPT_COL_R_X + HUDOPT_COL_W + 4.0f ) {
        g_hudOptHoverSlider = HUDOPT_SLIDER_ZOOM_ID;
        if ( clicked ) {
            HUDOpt_SetSliderFromX( HUDOPT_SLIDER_ZOOM_ID, cx );
        }
        return qtrue;
    }

    return qtrue; /* consume all mouse input while menu is open */
}

/*
================
CG_DrawHUDOptionsMenu
Renders the toggle overlay when cg_hudOptionsOpen is set.
Call from CG_DrawActive() *after* the main HUD pass.
================
*/
void CG_DrawHUDOptionsMenu( void ) {
    if ( !cg_hudOptionsOpen.integer ) {
        return;
    }

    /* Refresh all toggle cvar values from engine */
    trap_Cvar_Update( &cg_hudOptionsOpen );
    trap_Cvar_Update( &cg_hudShowTimes );
    trap_Cvar_Update( &cg_hudShowLaps );
    trap_Cvar_Update( &cg_hudShowPosition );
    trap_Cvar_Update( &cg_hudShowDistToFinish );
    trap_Cvar_Update( &cg_ghostPlayback );
    trap_Cvar_Update( &cg_checkpointArrowMode );
    trap_Cvar_Update( &cg_hudShowCarAheadBehind );
    trap_Cvar_Update( &cg_elimTimeline );
    trap_Cvar_Update( &cg_hudShowOpponentList );
    trap_Cvar_Update( &cg_hudShowScores );
    trap_Cvar_Update( &cg_hudShowSpeed );
    trap_Cvar_Update( &cg_hudShowFuelGauge );
    trap_Cvar_Update( &cg_drawRearView );
    trap_Cvar_Update( &cg_drawMMap );
    trap_Cvar_Update( &cg_mmap_size );
    trap_Cvar_Update( &cg_mmap_fov );
    trap_Cvar_Update( &cg_hudShowDerbyVehicle );
    trap_Cvar_Update( &cg_hudShowDerbyList );

    CG_SetScreenPlacement( PLACE_CENTER, PLACE_CENTER );

    /* ----------------------------------------------------------------
       Colour palette
       ---------------------------------------------------------------- */
    {
        static vec4_t bgColor     = { 0.008f, 0.012f, 0.016f, 0.88f };
        static vec4_t bandColor   = { 0.008f, 0.012f, 0.016f, 0.96f };
        static vec4_t borderColor = { 0.24f, 0.34f, 0.36f, 0.72f };
        static vec4_t accentColor = { 0.72f, 1.00f, 0.06f, 1.00f };
        static vec4_t titleColor  = { 0.90f, 0.95f, 0.94f, 1.00f };
        static vec4_t secColor    = { 0.47f, 0.62f, 0.61f, 1.00f };
        static vec4_t labelColor  = { 0.90f, 0.95f, 0.94f, 1.00f };
        static vec4_t hoverColor  = { 0.07f, 0.15f, 0.10f, 0.78f };
        static vec4_t onColor     = { 0.72f, 1.00f, 0.06f, 1.00f };
        static vec4_t offColor    = { 0.47f, 0.62f, 0.61f, 1.00f };
        static vec4_t cycColor    = { 0.30f, 0.66f, 0.96f, 1.00f };
        static vec4_t naColor     = { 0.29f, 0.37f, 0.38f, 1.00f };
        static vec4_t greyColor   = { 0.34f, 0.42f, 0.42f, 1.00f };
        static vec4_t hintColor   = { 0.47f, 0.62f, 0.61f, 1.00f };
        static vec4_t divColor    = { 0.24f, 0.34f, 0.36f, 0.62f };

		/* Panel height: title + hint + max(leftH, rightH) + padding
		 * Right col has 3 sections: Derby + KOTH + Vehicle         */
		float leftH  = HUDOPT_SEC_H + HUDOPT_LEFT_COUNT * HUDOPT_ROW_H;
		float rightH = HUDOPT_SEC_H + HUDOPT_DERBY_COUNT * HUDOPT_ROW_H
		             + HUDOPT_SEC_H + HUDOPT_KOTH_COUNT  * HUDOPT_ROW_H
		             + HUDOPT_SEC_H + HUDOPT_VEH_COUNT   * HUDOPT_ROW_H
		             + HUDOPT_MMAP_SLIDERS * HUDOPT_ROW_H
		             + 12.0f; /* two section separators (6px each) */
		float colH   = leftH > rightH ? leftH : rightH;
		float panelH = HUDOPT_TITLE_H + HUDOPT_HINT_H + HUDOPT_PAD + colH + HUDOPT_PAD + 6.0f;

        float panelX = HUDOPT_PNL_X - HUDOPT_PAD;
        float panelY = HUDOPT_PNL_Y - HUDOPT_PAD;

        int   i;
        float rowY, secY;

        /* Background */
        CG_FillRect( panelX, panelY, HUDOPT_PNL_W + HUDOPT_PAD * 2.0f, panelH, bgColor );
        CG_FillRect( panelX, panelY, HUDOPT_PNL_W + HUDOPT_PAD * 2.0f, HUDOPT_TITLE_H + 8.0f, bandColor );
        CG_FillRect( panelX, panelY, HUDOPT_PNL_W + HUDOPT_PAD * 2.0f, 2.0f, accentColor );
        CG_DrawRect( panelX, panelY, HUDOPT_PNL_W + HUDOPT_PAD * 2.0f, panelH, 1.0f, borderColor );

        /* Title – in-game charset, centred */
        {
            const char *title = "HUD ELEMENTS";
            CG_DrawIngameString( (int)( HUDOPT_PNL_X + HUDOPT_PNL_W * 0.5f ),
                                 (int)( HUDOPT_PNL_Y - 2 ), title,
                                 UI_CENTER|UI_DROPSHADOW, HUDOPT_TITLE_SCALE,
                                 titleColor );
        }

        /* Hint – in-game charset, centred */
        {
            const char *hint = "CLICK / ENTER TO TOGGLE  |  ESC TO CLOSE";
            HUDOpt_DrawCenteredText( HUDOPT_PNL_X, HUDOPT_PNL_W,
                                     (int)( HUDOPT_PNL_Y + HUDOPT_TITLE_H ),
                                     hint, hintColor, HUDOPT_HINT_SCALE );
        }

        secY = HUDOPT_PNL_Y + HUDOPT_TITLE_H + HUDOPT_HINT_H + HUDOPT_PAD;

        /* Vertical divider */
        CG_FillRect( HUDOPT_COL_L_X + HUDOPT_COL_W + HUDOPT_COL_GAP * 0.5f - 1.0f,
                     secY, 2.0f, colH, divColor );

        /* ===================== LEFT COLUMN: RACING ===================== */
        {
            const char *sec  = "RACING";
            HUDOpt_DrawCenteredText( HUDOPT_COL_L_X, HUDOPT_COL_W, (int)( secY + 2 ),
                                     sec, secColor, HUDOPT_SECTION_SCALE );
        }
        rowY = secY + HUDOPT_SEC_H;
        for ( i = 0; i < HUDOPT_LEFT_COUNT; i++ ) {
            const hudToggleEntry_t *e       = &hudToggleTable[i];
            qboolean                unavail = HUDEntry_IsUnavail( e );
            const char             *badge;
            float                  *badgeClr, *entryClr;
            int                     bx;

            if ( i == g_hudOptHoverRow && !unavail )
            {
                CG_FillRect( HUDOPT_COL_L_X - 2.0f, rowY, HUDOPT_COL_W + 4.0f, HUDOPT_ROW_H, hoverColor );
                CG_FillRect( HUDOPT_COL_L_X - 2.0f, rowY, 2.0f, HUDOPT_ROW_H, accentColor );
            }

            if ( unavail ) {
                badge = "N/A";  badgeClr = naColor;  entryClr = greyColor;
            } else if ( e->isCycler ) {
                badge    = HUDEntry_BadgeLabel( e );
                badgeClr = e->cvar->integer ? cycColor : offColor;
                entryClr = labelColor;
            } else {
                badge    = e->cvar->integer ? "ON" : "OFF";
                badgeClr = e->cvar->integer ? onColor : offColor;
                entryClr = labelColor;
            }
            bx = (int)( HUDOPT_COL_L_X + HUDOPT_COL_W );
            HUDOpt_DrawText( (int)( HUDOPT_COL_L_X + 3 ), (int)( rowY + 3 ),
                             e->label, entryClr, HUDOPT_ENTRY_SCALE );
            HUDOpt_DrawRightText( bx, (int)( rowY + 3 ),
                                  badge, badgeClr, HUDOPT_ENTRY_SCALE );
            rowY += HUDOPT_ROW_H;
        }

        /* =============== RIGHT COLUMN TOP: DERBY =============== */
        {
            const char *sec  = "DERBY";
            HUDOpt_DrawCenteredText( HUDOPT_COL_R_X, HUDOPT_COL_W, (int)( secY + 2 ),
                                     sec, secColor, HUDOPT_SECTION_SCALE );
        }
        rowY = secY + HUDOPT_SEC_H;
        for ( i = HUDOPT_DERBY_START; i < HUDOPT_DERBY_START + HUDOPT_DERBY_COUNT; i++ ) {
            const hudToggleEntry_t *e       = &hudToggleTable[i];
            qboolean                unavail = HUDEntry_IsUnavail( e );
            const char             *badge;
            float                  *badgeClr, *entryClr;
            int                     bx;

            if ( i == g_hudOptHoverRow && !unavail )
            {
                CG_FillRect( HUDOPT_COL_R_X - 2.0f, rowY, HUDOPT_COL_W + 4.0f, HUDOPT_ROW_H, hoverColor );
                CG_FillRect( HUDOPT_COL_R_X - 2.0f, rowY, 2.0f, HUDOPT_ROW_H, accentColor );
            }

            badge    = unavail ? "N/A" : ( e->cvar->integer ? "ON" : "OFF" );
            badgeClr = unavail ? naColor : ( e->cvar->integer ? onColor : offColor );
            entryClr = unavail ? greyColor : labelColor;

            bx = (int)( HUDOPT_COL_R_X + HUDOPT_COL_W );
            HUDOpt_DrawText( (int)( HUDOPT_COL_R_X + 3 ), (int)( rowY + 3 ),
                             e->label, entryClr, HUDOPT_ENTRY_SCALE );
            HUDOpt_DrawRightText( bx, (int)( rowY + 3 ),
                                  badge, badgeClr, HUDOPT_ENTRY_SCALE );
            rowY += HUDOPT_ROW_H;
        }

        /* =============== RIGHT COLUMN MIDDLE: KOTH =============== */
        {
            float sepY = rowY + 2.0f;
            CG_FillRect( HUDOPT_COL_R_X, sepY, HUDOPT_COL_W, 1.0f, divColor );
            rowY += 6.0f;
        }
        {
            const char *sec  = "KOTH";
            HUDOpt_DrawCenteredText( HUDOPT_COL_R_X, HUDOPT_COL_W, (int)( rowY + 2 ),
                                     sec, secColor, HUDOPT_SECTION_SCALE );
        }
        rowY += HUDOPT_SEC_H;
        for ( i = HUDOPT_KOTH_START; i < HUDOPT_KOTH_START + HUDOPT_KOTH_COUNT; i++ ) {
            const hudToggleEntry_t *e       = &hudToggleTable[i];
            qboolean                unavail = HUDEntry_IsUnavail( e );
            const char             *badge;
            float                  *badgeClr, *entryClr;
            int                     bx;

            if ( i == g_hudOptHoverRow && !unavail )
            {
                CG_FillRect( HUDOPT_COL_R_X - 2.0f, rowY, HUDOPT_COL_W + 4.0f, HUDOPT_ROW_H, hoverColor );
                CG_FillRect( HUDOPT_COL_R_X - 2.0f, rowY, 2.0f, HUDOPT_ROW_H, accentColor );
            }

            badge    = unavail ? "N/A" : ( e->cvar->integer ? "ON" : "OFF" );
            badgeClr = unavail ? naColor : ( e->cvar->integer ? onColor : offColor );
            entryClr = unavail ? greyColor : labelColor;

            bx = (int)( HUDOPT_COL_R_X + HUDOPT_COL_W );
            HUDOpt_DrawText( (int)( HUDOPT_COL_R_X + 3 ), (int)( rowY + 3 ),
                             e->label, entryClr, HUDOPT_ENTRY_SCALE );
            HUDOpt_DrawRightText( bx, (int)( rowY + 3 ),
                                  badge, badgeClr, HUDOPT_ENTRY_SCALE );
            rowY += HUDOPT_ROW_H;
        }

        /* =============== RIGHT COLUMN BOTTOM: VEHICLE =============== */
        {
            float sepY = rowY + 2.0f;
            CG_FillRect( HUDOPT_COL_R_X, sepY, HUDOPT_COL_W, 1.0f, divColor );
            rowY += 6.0f;
        }
        {
            const char *sec  = "VEHICLE";
            HUDOpt_DrawCenteredText( HUDOPT_COL_R_X, HUDOPT_COL_W, (int)( rowY + 2 ),
                                     sec, secColor, HUDOPT_SECTION_SCALE );
        }
        rowY += HUDOPT_SEC_H;
        for ( i = HUDOPT_VEH_START; i < HUDOPT_VEH_START + HUDOPT_VEH_COUNT; i++ ) {
            const hudToggleEntry_t *e       = &hudToggleTable[i];
            qboolean                unavail = HUDEntry_IsUnavail( e );
            const char             *badge;
            float                  *badgeClr, *entryClr;
            int                     bx;

            if ( i == g_hudOptHoverRow && !unavail )
            {
                CG_FillRect( HUDOPT_COL_R_X - 2.0f, rowY, HUDOPT_COL_W + 4.0f, HUDOPT_ROW_H, hoverColor );
                CG_FillRect( HUDOPT_COL_R_X - 2.0f, rowY, 2.0f, HUDOPT_ROW_H, accentColor );
            }

            badge    = unavail ? "N/A" : ( e->cvar->integer ? "ON" : "OFF" );
            badgeClr = unavail ? naColor : ( e->cvar->integer ? onColor : offColor );
            entryClr = unavail ? greyColor : labelColor;

            bx = (int)( HUDOPT_COL_R_X + HUDOPT_COL_W );
            HUDOpt_DrawText( (int)( HUDOPT_COL_R_X + 3 ), (int)( rowY + 3 ),
                             e->label, entryClr, HUDOPT_ENTRY_SCALE );
            HUDOpt_DrawRightText( bx, (int)( rowY + 3 ),
                                  badge, badgeClr, HUDOPT_ENTRY_SCALE );
            rowY += HUDOPT_ROW_H;
        }

        HUDOpt_DrawSlider( HUDOPT_COL_R_X, rowY, "MINI-MAP SCALE", cg_mmap_size.value,
                           0.40f, 2.00f, HUDOPT_SLIDER_SCALE_ID,
                           labelColor, offColor, divColor, accentColor, hoverColor );
        rowY += HUDOPT_ROW_H;

        HUDOpt_DrawSlider( HUDOPT_COL_R_X, rowY, "MINI-MAP ZOOM", cg_mmap_fov.value,
                           30.0f, 120.0f, HUDOPT_SLIDER_ZOOM_ID,
                           labelColor, offColor, divColor, accentColor, hoverColor );
        rowY += HUDOPT_ROW_H;

        /* Mouse cursor */
        {
            static qhandle_t s_cursorShader = 0;
            if ( !s_cursorShader )
                s_cursorShader = trap_R_RegisterShaderNoMip( "menu/art/3_cursor2" );
            trap_R_SetColor( colorWhite );
            CG_DrawPic( cgs.cursorX, cgs.cursorY, 16.0f, 16.0f, s_cursorShader );
            trap_R_SetColor( NULL );
        }
    }

    CG_PopScreenPlacement();
}


/* =======================================================================
   MAIN HUD ENTRY POINT
   ======================================================================= */

/* -----------------------------------------------------------------------
   Shared in-game surface styling.  The frontend uses quiet dark surfaces,
   a thin lime signal colour, and a very restrained amount of text.  Keep
   the same language here so the HUD feels like part of the same product.
   ----------------------------------------------------------------------- */
#if 0 /* superseded by the flat telemetry strip */
static const vec4_t rallyHudPanelColor = { 0.018f, 0.025f, 0.030f, 0.82f };
static const vec4_t rallyHudRowColor   = { 0.050f, 0.070f, 0.070f, 0.64f };
static const vec4_t rallyHudLineColor  = { 0.250f, 0.330f, 0.320f, 0.56f };
static const vec4_t rallyHudAccent     = { 0.720f, 1.000f, 0.060f, 1.00f };
static const vec4_t rallyHudText       = { 0.900f, 0.950f, 0.940f, 1.00f };
static const vec4_t rallyHudMuted      = { 0.470f, 0.570f, 0.560f, 1.00f };
static const vec4_t rallyHudGood       = { 0.480f, 1.000f, 0.420f, 1.00f };
static const vec4_t rallyHudBad        = { 1.000f, 0.350f, 0.300f, 1.00f };

#define RALLY_HUD_PANEL_X       428
#define RALLY_HUD_PANEL_W       208
#define RALLY_HUD_HEADER_H       25
#define RALLY_HUD_ROW_H          19
#define RALLY_HUD_TEXT_STYLE     ( UI_SMALLFONT | UI_DROPSHADOW )

static void CG_RallyHUDPanel( int x, int y, int width, int height,
                              const char *title ) {
    CG_FillRect( x, y, width, height, rallyHudPanelColor );
    CG_DrawRect( x, y, width, height, 1.0f, rallyHudLineColor );
    CG_FillRect( x, y, width, 2.0f, rallyHudAccent );
    CG_FillRect( x, y, 3.0f, height, rallyHudAccent );
    CG_FillRect( x + 6, y + RALLY_HUD_HEADER_H - 2,
                 width - 12, 1.0f, rallyHudLineColor );
    CG_DrawFrontendString( x + 11, y + 5, title, RALLY_HUD_TEXT_STYLE,
                           1.0f, rallyHudAccent );
}

static void CG_RallyHUDRow( int x, int y, int width, const char *label,
                            const char *value, const float *valueColor,
                            qboolean active ) {
    vec4_t rowColor;

    Vector4Copy( rallyHudRowColor, rowColor );
    if ( active ) {
        rowColor[0] = 0.080f;
        rowColor[1] = 0.150f;
        rowColor[2] = 0.095f;
        rowColor[3] = 0.82f;
    }

    CG_FillRect( x + 6, y, width - 12, RALLY_HUD_ROW_H - 1, rowColor );
    CG_FillRect( x + 6, y + RALLY_HUD_ROW_H - 1,
                 width - 12, 1.0f, rallyHudLineColor );
    CG_DrawFrontendString( x + 13, y + 2, label, RALLY_HUD_TEXT_STYLE,
                           1.0f, rallyHudMuted );
    CG_DrawFrontendString( x + width - 13, y + 2, value,
                           UI_RIGHT | UI_SMALLFONT | UI_DROPSHADOW,
                           1.0f, valueColor ? valueColor : rallyHudText );
}

static int CG_RallyHUDRaceRowCount( void ) {
    int rows;

    rows = 0;
    if ( cg_hudShowTimes.integer ) {
        if ( cgs.laplimit > 1 ) {
            rows += 2;
        }
        rows++;
    }
    if ( cg_ghostPlayback.integer && cg.ghostSplitDeltaValid ) {
        rows++;
    }
    if ( cg_hudShowLaps.integer ) {
        rows++;
    }
    if ( cg_hudShowPosition.integer ) {
        rows++;
    }
    if ( cg_hudShowDistToFinish.integer ) {
        rows++;
    }
    return rows;
}

static float CG_DrawModernRaceHUD( float y ) {
    centity_t *cent;
    int rowCount;
    int panelHeight;
    int rowY;
    int lapTime;
    int totalTime;
    int pos;
    char value[64];
    const char *time;
    vec4_t deltaColor;

    if ( !cg.snap || CG_IntroCam_IsActive() ) {
        return y;
    }

    cent = &cg_entities[cg.snap->ps.clientNum];
    if ( cent->finishRaceTime ) {
        lapTime = cent->finishRaceTime - cent->startLapTime;
        totalTime = cent->finishRaceTime - cent->startRaceTime;
    } else if ( cent->startRaceTime ) {
        lapTime = cg.time - cent->startLapTime;
        totalTime = cg.time - cent->startRaceTime;
    } else {
        lapTime = 0;
        totalTime = 0;
    }

    rowCount = CG_RallyHUDRaceRowCount();
    if ( rowCount <= 0 ) {
        return y;
    }

    panelHeight = RALLY_HUD_HEADER_H + rowCount * RALLY_HUD_ROW_H + 7;
    CG_RallyHUDPanel( RALLY_HUD_PANEL_X, 8, RALLY_HUD_PANEL_W,
                      panelHeight, "RACE STATUS" );
    rowY = 8 + RALLY_HUD_HEADER_H + 3;

    if ( cg_hudShowTimes.integer ) {
        if ( cgs.laplimit > 1 ) {
            time = getStringForTime( cent->bestLapTime );
            Com_sprintf( value, sizeof(value), "%s", time );
            CG_RallyHUDRow( RALLY_HUD_PANEL_X, rowY, RALLY_HUD_PANEL_W,
                            "BEST LAP", value, rallyHudText, qfalse );
            rowY += RALLY_HUD_ROW_H;

            time = getStringForTime( lapTime );
            Com_sprintf( value, sizeof(value), "%s", time );
            CG_RallyHUDRow( RALLY_HUD_PANEL_X, rowY, RALLY_HUD_PANEL_W,
                            "LAP TIME", value, rallyHudText, qfalse );
            rowY += RALLY_HUD_ROW_H;
        }

        time = getStringForTime( totalTime );
        Com_sprintf( value, sizeof(value), "%s", time );
        CG_RallyHUDRow( RALLY_HUD_PANEL_X, rowY, RALLY_HUD_PANEL_W,
                        "TOTAL", value, rallyHudText, qfalse );
        rowY += RALLY_HUD_ROW_H;
    }

    if ( cg_ghostPlayback.integer && cg.ghostSplitDeltaValid ) {
        if ( cg.ghostSplitDeltaMs < 0 ) {
            Vector4Copy( rallyHudGood, deltaColor );
        } else if ( cg.ghostSplitDeltaMs > 0 ) {
            Vector4Copy( rallyHudBad, deltaColor );
        } else {
            Vector4Copy( rallyHudText, deltaColor );
        }

        Com_sprintf( value, sizeof(value), "%c%d.%03d",
                     cg.ghostSplitDeltaMs < 0 ? '-' : '+',
                     abs( cg.ghostSplitDeltaMs ) / 1000,
                     abs( cg.ghostSplitDeltaMs ) % 1000 );
        CG_RallyHUDRow( RALLY_HUD_PANEL_X, rowY, RALLY_HUD_PANEL_W,
                        "GHOST DELTA", value, deltaColor, qfalse );
        rowY += RALLY_HUD_ROW_H;
    }

    if ( cg_hudShowLaps.integer ) {
        if ( cgs.gametype == GT_SPRINT ) {
            Q_strncpyz( value, "SPRINT", sizeof(value) );
        } else if ( cgs.laplimit > 1 ) {
            Com_sprintf( value, sizeof(value), "%d / %d",
                         cent->currentLap, cgs.laplimit );
        } else {
            Com_sprintf( value, sizeof(value), "%d", cent->currentLap );
        }
        CG_RallyHUDRow( RALLY_HUD_PANEL_X, rowY, RALLY_HUD_PANEL_W,
                        "LAPS", value, rallyHudText, qfalse );
        rowY += RALLY_HUD_ROW_H;
    }

    if ( cg_hudShowPosition.integer ) {
        pos = cent->currentPosition;
        Com_sprintf( value, sizeof(value), "%d / %d", pos, cgs.numRacers );
        CG_RallyHUDRow( RALLY_HUD_PANEL_X, rowY, RALLY_HUD_PANEL_W,
                        "POSITION", value, rallyHudAccent, qtrue );
        rowY += RALLY_HUD_ROW_H;
    }

    if ( cg_hudShowDistToFinish.integer ) {
        if ( cg_distanceFormat.integer == 1 && cgs.trackLength > 0.0f ) {
            Com_sprintf( value, sizeof(value), "%.1f%%",
                         cg.snap->ps.stats[STAT_DISTANCE_REMAIN] /
                         cgs.trackLength * 100.0f );
        } else {
            Com_sprintf( value, sizeof(value), "%dm",
                         (int)cg.snap->ps.stats[STAT_DISTANCE_REMAIN] );
        }
        CG_RallyHUDRow( RALLY_HUD_PANEL_X, rowY, RALLY_HUD_PANEL_W,
                        "TO FINISH", value, rallyHudText, qfalse );
    }

    return y + panelHeight;
}
#endif

/*
================
CG_DrawUpperRightHUD
Draws the modern right-side race status card.
================
*/
float CG_DrawUpperRightHUD( float y ) {
    int i;

    cgs.numRacers = 0;
    for ( i = 0; i < cgs.maxclients; i++ ) {
        if ( !cgs.clientinfo[i].infoValid )              continue;
        if ( cgs.clientinfo[i].team == TEAM_SPECTATOR )  continue;
        if ( cg.scores[i].ping == -1 )                   continue;
        cgs.numRacers++;
    }

    if ( cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_SPECTATOR ) {
        return y;
    }

    /* Suppress all racing HUD elements during the intro camera sequence */
    if ( CG_IntroCam_IsActive() ) {
        return y;
    }

    if ( isRallyRace() ) {
        if ( cg_checkpointArrowMode.integer ) {
            CG_DrawArrowToCheckpoint( y );
        }

        CG_UpdateGhostSplitDelta();
        /* Race timing and position now live in the flat telemetry strip.
           Do not draw the former stacked card over the world view. */
	}

    if ( !isRallyNonDMRace() && cgs.gametype != GT_DERBY &&
         cgs.gametype != GT_LCS && cgs.gametype != GT_DEATHMATCH &&
         cgs.gametype != GT_TEAM && cgs.gametype != GT_CTF &&
         cgs.gametype != GT_CTF4 && cgs.gametype != GT_DOMINATION &&
         cgs.gametype != GT_KOTH ) {
        if ( cg_hudShowScores.integer ) {
            y = CG_DrawScores( 636.0f, y );
        }
    }

    return y;
}


/*
================
CG_DrawLowerRightHUD
================
*/
float CG_DrawLowerRightHUD( float y ) {
    /* The common telemetry strip owns speed, gear, fuel and RPM. */
    return y;
}


/*
================
CG_DrawLowerLeftHUD
================
*/
float CG_DrawLowerLeftHUD( float y ) {
    int i;

    y += 36;
    for ( i = RWP_SMOKE; i < WP_NUM_WEAPONS; i++ ) {
        if ( cg.snap->ps.stats[STAT_WEAPONS] & ( 1u << i ) ) {
            if ( cg.snap->ps.ammo[i] ) {
                y -= 36;
                break;
            }
        }
    }
    return y;
}


/* -----------------------------------------------------------------------
   Compact race order panel. Gaps to the leader are measured at the latest
   checkpoint passed by both drivers, rather than guessed from their speed.
   ----------------------------------------------------------------------- */
#define RACE_ORDER_MAX_ROWS   9
#define RACE_ORDER_PANEL_W    160.0f
#define TEAM_DM_ORDER_PANEL_W 192.0f
#define RACE_ORDER_ROW_H       16.0f
#define RACE_ORDER_TEXT_SCALE  0.52f

static void CG_GetRedBlueTeamColor( int teamIndex, vec4_t color ) {
    if ( teamIndex == 0 ) {
        color[0] = 0.96f; color[1] = 0.24f; color[2] = 0.22f;
    } else {
        color[0] = 0.28f; color[1] = 0.56f; color[2] = 1.00f;
    }
    color[3] = 1.00f;
}

static void CG_GetCTFTeamColor( int teamIndex, vec4_t color ) {
    switch ( teamIndex ) {
    case 0:
        color[0] = 0.96f; color[1] = 0.24f; color[2] = 0.22f;
        break;
    case 1:
        color[0] = 0.28f; color[1] = 0.56f; color[2] = 1.00f;
        break;
    case 2:
        color[0] = 0.18f; color[1] = 0.88f; color[2] = 0.32f;
        break;
    default:
        color[0] = 1.00f; color[1] = 0.82f; color[2] = 0.16f;
        break;
    }
    color[3] = 1.00f;
}

static float CG_DrawRacingOrderHUD( float top ) {
    int clientAtPosition[MAX_CLIENTS + 1];
    int teamAtPosition[MAX_CLIENTS + 1];
    int i, position, maxPosition, localPosition;
    int firstPosition, lastPosition, rowCount, clientNum, leaderClient;
    int gapMs, absGapMs, nameLength;
    int teamIndex;
    qboolean isElimination, isEliminated, isTeamRace, isTeamRaceDM;
    float panelX, panelY, panelH, panelW, rowY, chipX, chipY, chipW;
    screenPlacement_e savedHorizontalPlacement;
    screenPlacement_e savedVerticalPlacement;
    char name[32];
    char gapText[16];
    char teamValue[24];
    static const char *teamCodes[2] = { "RED", "BLU" };
    static const char *teamNames[2] = { "RED", "BLUE" };
    team_t driverTeam;
    vec4_t panelColor = { 0.008f, 0.012f, 0.016f, 0.42f };
    vec4_t headerColor = { 0.008f, 0.012f, 0.016f, 0.72f };
    vec4_t rowColor = { 0.018f, 0.027f, 0.031f, 0.28f };
    vec4_t selectedColor = { 0.060f, 0.140f, 0.088f, 0.45f };
    vec4_t borderColor = { 0.24f, 0.34f, 0.36f, 0.52f };
    vec4_t accentColor = { 0.72f, 1.00f, 0.06f, 1.00f };
    vec4_t textColor = { 0.90f, 0.95f, 0.94f, 1.00f };
    vec4_t mutedColor = { 0.47f, 0.62f, 0.61f, 1.00f };
    vec4_t gapColor = { 0.36f, 0.70f, 0.96f, 1.00f };
    vec4_t negativeGapColor = { 1.00f, 0.38f, 0.30f, 1.00f };
    vec4_t teamColor;
    vec4_t teamFillColor;

    isTeamRaceDM = cgs.gametype == GT_TEAM_RACING_DM;
    isTeamRace = cgs.gametype == GT_TEAM_RACING || isTeamRaceDM;
    if ( !cg.snap || !CG_RaceOrderIsActive() ||
         ( isTeamRace && ( cg.showScores ||
           cg.predictedPlayerState.pm_type == PM_INTERMISSION ||
           cg.snap->ps.pm_type == PM_INTERMISSION ) ) ) {
        return top;
    }

    isElimination = cgs.gametype == GT_ELIMINATION;

    for ( i = 0; i <= MAX_CLIENTS; i++ ) {
        clientAtPosition[i] = -1;
        teamAtPosition[i] = -1;
    }
    maxPosition = 0;
    localPosition = 0;

    for ( i = 0; i < cgs.maxclients && i < MAX_CLIENTS; i++ ) {
        if ( !cgs.clientinfo[i].infoValid ) {
            continue;
        }
        driverTeam = cgs.clientinfo[i].team;
        if ( isTeamRace && i == cg.snap->ps.clientNum &&
             cg.snap->ps.persistant[PERS_TEAM] >= TEAM_RED &&
             cg.snap->ps.persistant[PERS_TEAM] <= TEAM_BLUE ) {
            driverTeam = (team_t)cg.snap->ps.persistant[PERS_TEAM];
        }
        if ( isTeamRace &&
             ( driverTeam < TEAM_RED || driverTeam > TEAM_BLUE ) ) {
            continue;
        }
        isEliminated = isElimination && cg_entities[i].eliminationOut;
        if ( driverTeam == TEAM_SPECTATOR && !isEliminated ) {
            continue;
        }

        position = cg_entities[i].currentPosition;
        if ( position <= 0 ) {
            position = cgs.clientinfo[i].position;
        }
        if ( position <= 0 || position > MAX_CLIENTS ||
             clientAtPosition[position] >= 0 ) {
            continue;
        }

        clientAtPosition[position] = i;
        teamAtPosition[position] = driverTeam;
        if ( position > maxPosition ) {
            maxPosition = position;
        }
        if ( i == cg.snap->ps.clientNum ) {
            localPosition = position;
        }
    }

    if ( maxPosition <= 0 ) {
        return top;
    }

    rowCount = maxPosition < RACE_ORDER_MAX_ROWS ? maxPosition : RACE_ORDER_MAX_ROWS;
    firstPosition = 1;
    if ( maxPosition > rowCount ) {
        if ( localPosition <= 0 ) {
            localPosition = 1;
        }
        firstPosition = localPosition - rowCount / 2;
        if ( firstPosition < 1 ) {
            firstPosition = 1;
        }
        if ( firstPosition > maxPosition - rowCount + 1 ) {
            firstPosition = maxPosition - rowCount + 1;
        }
    }
    lastPosition = firstPosition + rowCount - 1;

    panelW = isTeamRace ? TEAM_DM_ORDER_PANEL_W : RACE_ORDER_PANEL_W;
    panelX = 640.0f - panelW;
    panelY = top;
    panelH = ( isTeamRace ? 57.0f : 40.0f ) +
             rowCount * RACE_ORDER_ROW_H;
    savedHorizontalPlacement = CG_GetScreenHorizontalPlacement();
    savedVerticalPlacement = CG_GetScreenVerticalPlacement();
    CG_SetScreenPlacement( PLACE_RIGHT, PLACE_TOP );

    CG_FillRect( panelX, panelY, panelW, panelH, panelColor );
    CG_FillRect( panelX, panelY, panelW, 2.0f, accentColor );
    CG_FillRect( panelX, panelY, panelW, 20.0f, headerColor );
    CG_DrawRect( panelX, panelY, panelW, panelH, 1.0f, borderColor );
    CG_FillRect( panelX + 7.0f, panelY + 19.0f, panelW - 14.0f,
                 1.0f, borderColor );

    CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( panelY + 5.0f ),
                         isTeamRaceDM ? "TEAM RACE DM" :
                         ( isTeamRace ? "TEAM RACE ORDER" : "RACE ORDER" ),
                         UI_SMALLFONT, 0.56f, accentColor );

    if ( isTeamRace ) {
        chipY = panelY + 22.0f;
        chipW = ( panelW - 18.0f ) / 2.0f;
        for ( teamIndex = 0; teamIndex < 2; teamIndex++ ) {
            CG_GetRedBlueTeamColor( teamIndex, teamColor );
            chipX = panelX + 7.0f + teamIndex * ( chipW + 4.0f );
            teamFillColor[0] = teamColor[0];
            teamFillColor[1] = teamColor[1];
            teamFillColor[2] = teamColor[2];
            teamFillColor[3] = 0.20f;
            CG_FillRect( chipX, chipY, chipW, 16.0f, teamFillColor );
            CG_FillRect( chipX, chipY, 2.0f, 16.0f, teamColor );
            CG_DrawRect( chipX, chipY, chipW, 16.0f, 1.0f, borderColor );
            CG_DrawIngameString( (int)( chipX + 5.0f ), (int)( chipY + 4.0f ),
                                 teamNames[teamIndex], UI_SMALLFONT, 0.42f, teamColor );
            if ( isTeamRaceDM ) {
                Com_sprintf( teamValue, sizeof( teamValue ), "%dF",
                             cg.teamScores[teamIndex] );
            } else if ( cg.teamTimes[teamIndex] > 0 &&
                        cg.teamTimes[teamIndex] < ( 1 << 30 ) ) {
                Q_strncpyz( teamValue, getStringForTime( cg.teamTimes[teamIndex] ),
                            sizeof( teamValue ) );
            } else {
                Q_strncpyz( teamValue, "--:--:--", sizeof( teamValue ) );
            }
            CG_DrawIngameString( (int)( chipX + chipW - 4.0f ),
                                 (int)( chipY + 4.0f ), teamValue,
                                 UI_RIGHT | UI_SMALLFONT, 0.44f, textColor );
        }
        CG_FillRect( panelX + 7.0f, panelY + 40.0f, panelW - 14.0f,
                     1.0f, borderColor );
        CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( panelY + 43.0f ),
                             "POS", UI_SMALLFONT, 0.40f, mutedColor );
        CG_DrawIngameString( (int)( panelX + 30.0f ), (int)( panelY + 43.0f ),
                             "DRIVER", UI_SMALLFONT, 0.40f, mutedColor );
        CG_DrawIngameString( (int)( panelX + 116.0f ), (int)( panelY + 43.0f ),
                             "TEAM", UI_SMALLFONT, 0.40f, mutedColor );
        CG_DrawIngameString( (int)( panelX + panelW - 8.0f ),
                             (int)( panelY + 43.0f ), "TO LEAD",
                             UI_RIGHT | UI_SMALLFONT, 0.40f, mutedColor );
        CG_FillRect( panelX + 7.0f, panelY + 51.0f, panelW - 14.0f,
                     1.0f, borderColor );
        rowY = panelY + 53.0f;
    } else {
        CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( panelY + 23.0f ),
                             "POS", UI_SMALLFONT, 0.44f, mutedColor );
        CG_DrawIngameString( (int)( panelX + 32.0f ), (int)( panelY + 23.0f ),
                             "DRIVER", UI_SMALLFONT, 0.44f, mutedColor );
        CG_DrawIngameString( (int)( panelX + panelW - 8.0f ),
                             (int)( panelY + 23.0f ),
                             isElimination ? "STATUS" : "TO LEAD",
                             UI_RIGHT | UI_SMALLFONT, 0.44f, mutedColor );
        rowY = panelY + 34.0f;
    }

    for ( position = firstPosition; position <= lastPosition; position++ ) {
        clientNum = clientAtPosition[position];
        if ( clientNum < 0 ) {
            rowY += RACE_ORDER_ROW_H;
            continue;
        }
        isEliminated = isElimination && cg_entities[clientNum].eliminationOut;

        if ( clientNum == cg.snap->ps.clientNum ) {
            CG_FillRect( panelX + 1.0f, rowY, panelW - 2.0f,
                         RACE_ORDER_ROW_H, selectedColor );
            CG_FillRect( panelX + 1.0f, rowY, 2.0f, RACE_ORDER_ROW_H, accentColor );
        } else {
            CG_FillRect( panelX + 1.0f, rowY, panelW - 2.0f,
                         RACE_ORDER_ROW_H, rowColor );
        }

        CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( rowY + 3.0f ),
                             va( "%02d", position ), UI_SMALLFONT,
                             RACE_ORDER_TEXT_SCALE,
                             position == 1 ? accentColor : mutedColor );

        Q_strncpyz( name, cgs.clientinfo[clientNum].name, sizeof( name ) );
        while ( CG_IngameStringWidth( name, UI_SMALLFONT, RACE_ORDER_TEXT_SCALE ) >
                ( isTeamRace ? 78 : 72 ) ) {
            nameLength = strlen( name );
            if ( nameLength <= 1 ) {
                break;
            }
            if ( nameLength >= 2 && name[nameLength - 2] == '^' ) {
                name[nameLength - 2] = '\0';
            } else {
                name[nameLength - 1] = '\0';
            }
        }
        CG_DrawIngameString( (int)( panelX + 32.0f ), (int)( rowY + 3.0f ),
                             name, UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                             isEliminated ? mutedColor :
                             ( clientNum == cg.snap->ps.clientNum ? accentColor : textColor ) );

        if ( isTeamRace ) {
            teamIndex = teamAtPosition[position] - TEAM_RED;
            CG_GetRedBlueTeamColor( teamIndex, teamColor );
            CG_FillRect( panelX + 116.0f, rowY + 4.0f, 2.0f, 8.0f, teamColor );
            CG_DrawIngameString( (int)( panelX + 122.0f ), (int)( rowY + 3.0f ),
                                 teamCodes[teamIndex], UI_SMALLFONT, 0.38f,
                                 teamColor );
        }

        if ( isElimination ) {
            Q_strncpyz( gapText, isEliminated ? "OUT" : "ALIVE", sizeof( gapText ) );
            CG_DrawIngameString( (int)( panelX + panelW - 8.0f ),
                                 (int)( rowY + 3.0f ), gapText,
                                 UI_RIGHT | UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                                 isEliminated ? negativeGapColor : mutedColor );
        } else if ( position == 1 ) {
            Q_strncpyz( gapText, "LEADER", sizeof( gapText ) );
            CG_DrawIngameString( (int)( panelX + panelW - 8.0f ),
                                 (int)( rowY + 3.0f ), gapText,
                                 UI_RIGHT | UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                                 accentColor );
        } else {
            leaderClient = clientAtPosition[1];
            if ( leaderClient >= 0 &&
                 CG_GetRaceSplitGap( leaderClient, clientNum, &gapMs ) ) {
                absGapMs = gapMs < 0 ? -gapMs : gapMs;
                Com_sprintf( gapText, sizeof( gapText ), "%c%d.%02d",
                             gapMs < 0 ? '-' : '+', absGapMs / 1000,
                             ( absGapMs % 1000 ) / 10 );
                CG_DrawIngameString( (int)( panelX + panelW - 8.0f ),
                                     (int)( rowY + 3.0f ), gapText,
                                     UI_RIGHT | UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                                     gapMs < 0 ? negativeGapColor : gapColor );
            } else {
                CG_DrawIngameString( (int)( panelX + panelW - 8.0f ),
                                     (int)( rowY + 3.0f ), "--",
                                     UI_RIGHT | UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                                     mutedColor );
            }
        }

        CG_FillRect( panelX + 7.0f, rowY + RACE_ORDER_ROW_H - 1.0f,
                     panelW - 14.0f, 1.0f, borderColor );
        rowY += RACE_ORDER_ROW_H;
    }

    CG_SetScreenPlacement( savedHorizontalPlacement, savedVerticalPlacement );
    return panelY + panelH;
}

/* Deathmatch uses the same compact top-right visual language as the racing
 * order panel, but ranks players by frags and puts the frag limit in the
 * header. */
static void CG_DrawDeathmatchOrderHUD( float top ) {
    int clients[MAX_CLIENTS];
    int fragScores[MAX_CLIENTS];
    int count, i, j, clientNum, localIndex;
    int rowCount, firstRow, row, position, nameLength;
    float panelX, panelY, panelH, rowY;
    screenPlacement_e savedHorizontalPlacement;
    screenPlacement_e savedVerticalPlacement;
    char name[32];
    char limitText[24];
    vec4_t panelColor = { 0.008f, 0.012f, 0.016f, 0.42f };
    vec4_t headerColor = { 0.008f, 0.012f, 0.016f, 0.72f };
    vec4_t rowColor = { 0.018f, 0.027f, 0.031f, 0.28f };
    vec4_t selectedColor = { 0.060f, 0.140f, 0.088f, 0.45f };
    vec4_t borderColor = { 0.24f, 0.34f, 0.36f, 0.52f };
    vec4_t accentColor = { 0.72f, 1.00f, 0.06f, 1.00f };
    vec4_t textColor = { 0.90f, 0.95f, 0.94f, 1.00f };
    vec4_t mutedColor = { 0.47f, 0.62f, 0.61f, 1.00f };

    if ( !cg.snap || cgs.gametype != GT_DEATHMATCH ) {
        return;
    }

    count = 0;
    localIndex = -1;
    for ( i = 0; i < cgs.maxclients && i < MAX_CLIENTS; i++ ) {
        if ( !cgs.clientinfo[i].infoValid ||
             cgs.clientinfo[i].team == TEAM_SPECTATOR ) {
            continue;
        }
        clients[count] = i;
        fragScores[count] = cgs.clientinfo[i].score;
        if ( i == cg.snap->ps.clientNum ) {
            fragScores[count] = cg.snap->ps.persistant[PERS_SCORE];
            localIndex = count;
        }
        count++;
    }

    /* Keep the local player visible even while client info is catching up. */
    if ( localIndex < 0 && count < MAX_CLIENTS &&
         cg.snap->ps.clientNum >= 0 &&
         cg.snap->ps.clientNum < MAX_CLIENTS &&
         cgs.clientinfo[cg.snap->ps.clientNum].team != TEAM_SPECTATOR ) {
        localIndex = count;
        clients[count] = cg.snap->ps.clientNum;
        fragScores[count] = cg.snap->ps.persistant[PERS_SCORE];
        count++;
    }
    if ( count <= 0 ) {
        return;
    }

    /* Sort by frags, keeping the server order for ties. */
    for ( i = 1; i < count; i++ ) {
        int savedClient = clients[i];
        int savedScore = fragScores[i];
        j = i;
        while ( j > 0 && fragScores[j - 1] < savedScore ) {
            clients[j] = clients[j - 1];
            fragScores[j] = fragScores[j - 1];
            j--;
        }
        clients[j] = savedClient;
        fragScores[j] = savedScore;
    }

    localIndex = -1;
    for ( i = 0; i < count; i++ ) {
        if ( clients[i] == cg.snap->ps.clientNum ) {
            localIndex = i;
            break;
        }
    }

    rowCount = count < RACE_ORDER_MAX_ROWS ? count : RACE_ORDER_MAX_ROWS;
    firstRow = 0;
    if ( count > rowCount && localIndex >= 0 ) {
        firstRow = localIndex - rowCount / 2;
        if ( firstRow < 0 ) {
            firstRow = 0;
        }
        if ( firstRow > count - rowCount ) {
            firstRow = count - rowCount;
        }
    }

    panelX = 640.0f - RACE_ORDER_PANEL_W;
    panelY = top;
    panelH = 34.0f + rowCount * RACE_ORDER_ROW_H + 6.0f;
    savedHorizontalPlacement = CG_GetScreenHorizontalPlacement();
    savedVerticalPlacement = CG_GetScreenVerticalPlacement();
    CG_SetScreenPlacement( PLACE_RIGHT, PLACE_TOP );

    CG_FillRect( panelX, panelY, RACE_ORDER_PANEL_W, panelH, panelColor );
    CG_FillRect( panelX, panelY, RACE_ORDER_PANEL_W, 2.0f, accentColor );
    CG_FillRect( panelX, panelY, RACE_ORDER_PANEL_W, 20.0f, headerColor );
    CG_DrawRect( panelX, panelY, RACE_ORDER_PANEL_W, panelH, 1.0f, borderColor );
    CG_FillRect( panelX + 7.0f, panelY + 19.0f,
                 RACE_ORDER_PANEL_W - 14.0f, 1.0f, borderColor );

    CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( panelY + 5.0f ),
                         "FRAG ORDER", UI_SMALLFONT, 0.56f, accentColor );
    if ( cgs.fraglimit > 0 ) {
        Com_sprintf( limitText, sizeof( limitText ), "LIMIT %d", cgs.fraglimit );
    } else {
        Q_strncpyz( limitText, "NO LIMIT", sizeof( limitText ) );
    }
    CG_DrawIngameString( (int)( panelX + RACE_ORDER_PANEL_W - 8.0f ),
                         (int)( panelY + 5.0f ), limitText,
                         UI_RIGHT | UI_SMALLFONT, 0.44f, mutedColor );
    CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( panelY + 23.0f ),
                         "POS", UI_SMALLFONT, 0.44f, mutedColor );
    CG_DrawIngameString( (int)( panelX + 32.0f ), (int)( panelY + 23.0f ),
                         "DRIVER", UI_SMALLFONT, 0.44f, mutedColor );
    CG_DrawIngameString( (int)( panelX + RACE_ORDER_PANEL_W - 8.0f ),
                         (int)( panelY + 23.0f ), "FRAGS",
                         UI_RIGHT | UI_SMALLFONT, 0.44f, mutedColor );

    rowY = panelY + 34.0f;
    for ( row = 0; row < rowCount; row++ ) {
        i = firstRow + row;
        clientNum = clients[i];
        position = i + 1;
        if ( cgs.clientinfo[clientNum].position > 0 ) {
            position = cgs.clientinfo[clientNum].position;
        }

        if ( clientNum == cg.snap->ps.clientNum ) {
            CG_FillRect( panelX + 1.0f, rowY, RACE_ORDER_PANEL_W - 2.0f,
                         RACE_ORDER_ROW_H, selectedColor );
            CG_FillRect( panelX + 1.0f, rowY, 2.0f, RACE_ORDER_ROW_H, accentColor );
        } else {
            CG_FillRect( panelX + 1.0f, rowY, RACE_ORDER_PANEL_W - 2.0f,
                         RACE_ORDER_ROW_H, rowColor );
        }

        CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( rowY + 3.0f ),
                             va( "%02d", position ), UI_SMALLFONT,
                             RACE_ORDER_TEXT_SCALE,
                             row == 0 ? accentColor : mutedColor );
        Q_strncpyz( name, cgs.clientinfo[clientNum].name, sizeof( name ) );
        while ( CG_IngameStringWidth( name, UI_SMALLFONT,
                                      RACE_ORDER_TEXT_SCALE ) > 72 ) {
            nameLength = strlen( name );
            if ( nameLength <= 1 ) {
                break;
            }
            if ( nameLength >= 2 && name[nameLength - 2] == '^' ) {
                name[nameLength - 2] = '\0';
            } else {
                name[nameLength - 1] = '\0';
            }
        }
        CG_DrawIngameString( (int)( panelX + 32.0f ), (int)( rowY + 3.0f ),
                             name, UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                             clientNum == cg.snap->ps.clientNum ? accentColor : textColor );
        CG_DrawIngameString( (int)( panelX + RACE_ORDER_PANEL_W - 8.0f ),
                             (int)( rowY + 3.0f ), va( "%d", fragScores[i] ),
                             UI_RIGHT | UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                             textColor );
        CG_FillRect( panelX + 7.0f, rowY + RACE_ORDER_ROW_H - 1.0f,
                     RACE_ORDER_PANEL_W - 14.0f, 1.0f, borderColor );
        rowY += RACE_ORDER_ROW_H;
    }

    CG_SetScreenPlacement( savedHorizontalPlacement, savedVerticalPlacement );
}

/* Two-team modes keep both totals above a player-score-ranked list. */
static void CG_DrawTeamDeathmatchOrderHUD( float top ) {
    static const char *teamCodes[2] = { "RED", "BLU" };
    static const char *teamNames[2] = { "RED", "BLUE" };
    int clients[MAX_CLIENTS];
    int fragScores[MAX_CLIENTS];
    int playerTeams[MAX_CLIENTS];
    int count, i, j, clientNum, localIndex, rowCount, firstRow, row;
    int teamIndex, nameLength;
    qboolean isKoth;
    team_t playerTeam, localTeam;
    float panelX, panelY, panelH, rowY, chipX, chipY, chipW;
    screenPlacement_e savedHorizontalPlacement;
    screenPlacement_e savedVerticalPlacement;
    char name[32];
    char limitText[24];
    vec4_t panelColor = { 0.008f, 0.012f, 0.016f, 0.42f };
    vec4_t headerColor = { 0.008f, 0.012f, 0.016f, 0.72f };
    vec4_t rowColor = { 0.018f, 0.027f, 0.031f, 0.28f };
    vec4_t selectedColor = { 0.060f, 0.140f, 0.088f, 0.45f };
    vec4_t borderColor = { 0.24f, 0.34f, 0.36f, 0.52f };
    vec4_t accentColor = { 0.72f, 1.00f, 0.06f, 1.00f };
    vec4_t textColor = { 0.90f, 0.95f, 0.94f, 1.00f };
    vec4_t mutedColor = { 0.47f, 0.62f, 0.61f, 1.00f };
    vec4_t teamColor;
    vec4_t teamFillColor;

    if ( !cg.snap || ( cgs.gametype != GT_TEAM && cgs.gametype != GT_KOTH ) ) {
        return;
    }
    isKoth = ( cgs.gametype == GT_KOTH );

    count = 0;
    localIndex = -1;
    localTeam = (team_t)cg.snap->ps.persistant[PERS_TEAM];
    for ( i = 0; i < cgs.maxclients && i < MAX_CLIENTS; i++ ) {
        if ( !cgs.clientinfo[i].infoValid ) {
            continue;
        }
        playerTeam = cgs.clientinfo[i].team;
        if ( i == cg.snap->ps.clientNum &&
             localTeam >= TEAM_RED && localTeam <= TEAM_BLUE ) {
            playerTeam = localTeam;
        }
        if ( playerTeam < TEAM_RED || playerTeam > TEAM_BLUE ) {
            continue;
        }

        clients[count] = i;
        playerTeams[count] = playerTeam;
        fragScores[count] = cgs.clientinfo[i].score;
        if ( i == cg.snap->ps.clientNum ) {
            fragScores[count] = cg.snap->ps.persistant[PERS_SCORE];
            localIndex = count;
        }
        count++;
    }

    /* Keep the local player visible while client info is catching up. */
    if ( localIndex < 0 && count < MAX_CLIENTS &&
         cg.snap->ps.clientNum >= 0 && cg.snap->ps.clientNum < MAX_CLIENTS &&
         localTeam >= TEAM_RED && localTeam <= TEAM_BLUE ) {
        localIndex = count;
        clients[count] = cg.snap->ps.clientNum;
        playerTeams[count] = localTeam;
        fragScores[count] = cg.snap->ps.persistant[PERS_SCORE];
        count++;
    }
    if ( count <= 0 ) {
        return;
    }

    /* Rank every playable team together; retain server order for ties. */
    for ( i = 1; i < count; i++ ) {
        int savedClient = clients[i];
        int savedScore = fragScores[i];
        int savedTeam = playerTeams[i];
        j = i;
        while ( j > 0 && fragScores[j - 1] < savedScore ) {
            clients[j] = clients[j - 1];
            fragScores[j] = fragScores[j - 1];
            playerTeams[j] = playerTeams[j - 1];
            j--;
        }
        clients[j] = savedClient;
        fragScores[j] = savedScore;
        playerTeams[j] = savedTeam;
    }

    localIndex = -1;
    for ( i = 0; i < count; i++ ) {
        if ( clients[i] == cg.snap->ps.clientNum ) {
            localIndex = i;
            break;
        }
    }

    rowCount = count < RACE_ORDER_MAX_ROWS ? count : RACE_ORDER_MAX_ROWS;
    firstRow = 0;
    if ( count > rowCount && localIndex >= 0 ) {
        firstRow = localIndex - rowCount / 2;
        if ( firstRow < 0 ) {
            firstRow = 0;
        }
        if ( firstRow > count - rowCount ) {
            firstRow = count - rowCount;
        }
    }

    panelX = 640.0f - TEAM_DM_ORDER_PANEL_W;
    panelY = top;
    panelH = 57.0f + rowCount * RACE_ORDER_ROW_H;
    savedHorizontalPlacement = CG_GetScreenHorizontalPlacement();
    savedVerticalPlacement = CG_GetScreenVerticalPlacement();
    CG_SetScreenPlacement( PLACE_RIGHT, PLACE_TOP );

    CG_FillRect( panelX, panelY, TEAM_DM_ORDER_PANEL_W, panelH, panelColor );
    CG_FillRect( panelX, panelY, TEAM_DM_ORDER_PANEL_W, 2.0f, accentColor );
    CG_FillRect( panelX, panelY, TEAM_DM_ORDER_PANEL_W, 20.0f, headerColor );
    CG_DrawRect( panelX, panelY, TEAM_DM_ORDER_PANEL_W, panelH, 1.0f, borderColor );
    CG_FillRect( panelX + 7.0f, panelY + 20.0f, TEAM_DM_ORDER_PANEL_W - 14.0f,
                 1.0f, borderColor );

    CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( panelY + 5.0f ),
                         isKoth ? "KOTH DRIVER ORDER" : "TEAM FRAG ORDER",
                         UI_SMALLFONT, isKoth ? 0.48f : 0.56f, accentColor );
    if ( isKoth ) {
        Q_strncpyz( limitText, "HILL MODE", sizeof( limitText ) );
    } else if ( cgs.fraglimit > 0 ) {
        Com_sprintf( limitText, sizeof( limitText ), "LIMIT %d", cgs.fraglimit );
    } else {
        Q_strncpyz( limitText, "NO LIMIT", sizeof( limitText ) );
    }
    CG_DrawIngameString( (int)( panelX + TEAM_DM_ORDER_PANEL_W - 8.0f ), (int)( panelY + 5.0f ),
                         limitText, UI_RIGHT | UI_SMALLFONT, 0.42f, mutedColor );

    chipY = panelY + 22.0f;
    chipW = ( TEAM_DM_ORDER_PANEL_W - 18.0f ) / 2.0f;
    for ( teamIndex = 0; teamIndex < 2; teamIndex++ ) {
        CG_GetRedBlueTeamColor( teamIndex, teamColor );
        chipX = panelX + 7.0f + teamIndex * ( chipW + 4.0f );
        teamFillColor[0] = teamColor[0];
        teamFillColor[1] = teamColor[1];
        teamFillColor[2] = teamColor[2];
        teamFillColor[3] = 0.20f;
        CG_FillRect( chipX, chipY, chipW, 16.0f, teamFillColor );
        CG_FillRect( chipX, chipY, 2.0f, 16.0f, teamColor );
        CG_DrawRect( chipX, chipY, chipW, 16.0f, 1.0f, borderColor );
        CG_DrawIngameString( (int)( chipX + 5.0f ), (int)( chipY + 4.0f ),
                             teamNames[teamIndex], UI_SMALLFONT, 0.42f, teamColor );
        CG_DrawIngameString( (int)( chipX + chipW - 4.0f ),
                             (int)( chipY + 4.0f ),
                             va( "%d", cg.teamScores[teamIndex] ),
                             UI_RIGHT | UI_SMALLFONT, 0.44f, textColor );
    }

    CG_FillRect( panelX + 7.0f, panelY + 40.0f, TEAM_DM_ORDER_PANEL_W - 14.0f,
                 1.0f, borderColor );
    CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( panelY + 43.0f ),
                         "POS", UI_SMALLFONT, 0.40f, mutedColor );
    CG_DrawIngameString( (int)( panelX + 30.0f ), (int)( panelY + 43.0f ),
                         "DRIVER", UI_SMALLFONT, 0.40f, mutedColor );
    CG_DrawIngameString( (int)( panelX + 116.0f ), (int)( panelY + 43.0f ),
                         "TEAM", UI_SMALLFONT, 0.40f, mutedColor );
    CG_DrawIngameString( (int)( panelX + TEAM_DM_ORDER_PANEL_W - 8.0f ), (int)( panelY + 43.0f ),
                         isKoth ? "PTS" : "FRAGS",
                         UI_RIGHT | UI_SMALLFONT, 0.40f, mutedColor );
    CG_FillRect( panelX + 7.0f, panelY + 51.0f, TEAM_DM_ORDER_PANEL_W - 14.0f,
                 1.0f, borderColor );

    rowY = panelY + 53.0f;
    for ( row = 0; row < rowCount; row++ ) {
        i = firstRow + row;
        clientNum = clients[i];
        teamIndex = playerTeams[i] - TEAM_RED;

        if ( clientNum == cg.snap->ps.clientNum ) {
            CG_FillRect( panelX + 1.0f, rowY, TEAM_DM_ORDER_PANEL_W - 2.0f,
                         RACE_ORDER_ROW_H, selectedColor );
            CG_FillRect( panelX + 1.0f, rowY, 2.0f,
                         RACE_ORDER_ROW_H, accentColor );
        } else {
            CG_FillRect( panelX + 1.0f, rowY, TEAM_DM_ORDER_PANEL_W - 2.0f,
                         RACE_ORDER_ROW_H, rowColor );
        }

        CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( rowY + 3.0f ),
                             va( "%02d", i + 1 ), UI_SMALLFONT,
                             RACE_ORDER_TEXT_SCALE,
                             row == 0 ? accentColor : mutedColor );
        Q_strncpyz( name, cgs.clientinfo[clientNum].name, sizeof( name ) );
        while ( CG_IngameStringWidth( name, UI_SMALLFONT,
                                      RACE_ORDER_TEXT_SCALE ) > 78 ) {
            nameLength = strlen( name );
            if ( nameLength <= 1 ) {
                break;
            }
            if ( nameLength >= 2 && name[nameLength - 2] == '^' ) {
                name[nameLength - 2] = '\0';
            } else {
                name[nameLength - 1] = '\0';
            }
        }
        CG_DrawIngameString( (int)( panelX + 30.0f ), (int)( rowY + 3.0f ),
                             name, UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                             clientNum == cg.snap->ps.clientNum ? accentColor : textColor );

        CG_GetRedBlueTeamColor( teamIndex, teamColor );
        CG_FillRect( panelX + 116.0f, rowY + 4.0f, 2.0f, 8.0f, teamColor );
        CG_DrawIngameString( (int)( panelX + 122.0f ), (int)( rowY + 3.0f ),
                             teamCodes[teamIndex], UI_SMALLFONT, 0.38f,
                             teamColor );
        CG_DrawIngameString( (int)( panelX + 216.0f ), (int)( rowY + 3.0f ),
                             va( "%d", fragScores[i] ),
                             UI_RIGHT | UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                             textColor );
        CG_FillRect( panelX + 7.0f, rowY + RACE_ORDER_ROW_H - 1.0f,
                     TEAM_DM_ORDER_PANEL_W - 14.0f, 1.0f, borderColor );
        rowY += RACE_ORDER_ROW_H;
    }

    CG_SetScreenPlacement( savedHorizontalPlacement, savedVerticalPlacement );
}

/* CTF keeps team totals and flag state in compact header chips; each driver
 * row reports the carried flag, using icons for the four-team variant. */
static void CG_DrawCTFOrderHUD( float top ) {
    int clients[MAX_CLIENTS];
    int playerScores[MAX_CLIENTS];
    int playerTeams[MAX_CLIENTS];
    int count, i, j, clientNum, localIndex, rowCount, firstRow, row;
    int nameLength, teamIndex, carriedFlag, flagStatus;
    int teamCount;
    gitem_t *flagItem;
    team_t playerTeam, localTeam;
    float panelX, panelY, panelH, rowY, chipX, chipY, chipW;
    qhandle_t flagShader, carriedFlagIcon;
    screenPlacement_e savedHorizontalPlacement;
    screenPlacement_e savedVerticalPlacement;
    char name[32];
    char flagText[12];
    vec4_t panelColor = { 0.008f, 0.012f, 0.016f, 0.42f };
    vec4_t headerColor = { 0.008f, 0.012f, 0.016f, 0.72f };
    vec4_t rowColor = { 0.018f, 0.027f, 0.031f, 0.28f };
    vec4_t selectedColor = { 0.060f, 0.140f, 0.088f, 0.45f };
    vec4_t borderColor = { 0.24f, 0.34f, 0.36f, 0.52f };
    vec4_t accentColor = { 0.72f, 1.00f, 0.06f, 1.00f };
    vec4_t textColor = { 0.90f, 0.95f, 0.94f, 1.00f };
    vec4_t mutedColor = { 0.47f, 0.62f, 0.61f, 1.00f };
    vec4_t flagColor;
    vec4_t teamColor;
    vec4_t teamFillColor;
    static const char *teamCodes[4] = { "RED", "BLU", "GRN", "YLW" };
    static const char *teamShortCodes[4] = { "R", "B", "G", "Y" };
    static const char *teamNames[4] = { "RED", "BLUE", "GREEN", "YELLOW" };
    qboolean isFourTeam;
    qboolean isDomination;

    if ( !cg.snap || ( cgs.gametype != GT_CTF && cgs.gametype != GT_CTF4 &&
                       cgs.gametype != GT_DOMINATION ) ) {
        return;
    }

    isDomination = cgs.gametype == GT_DOMINATION;
    isFourTeam = cgs.gametype == GT_CTF4 || isDomination;
    teamCount = isFourTeam ? 4 : 2;
    count = 0;
    localIndex = -1;
    localTeam = (team_t)cg.snap->ps.persistant[PERS_TEAM];
    for ( i = 0; i < cgs.maxclients && i < MAX_CLIENTS; i++ ) {
        if ( !cgs.clientinfo[i].infoValid ) {
            continue;
        }
        playerTeam = cgs.clientinfo[i].team;
        if ( i == cg.snap->ps.clientNum &&
             localTeam >= TEAM_RED &&
             localTeam <= ( isFourTeam ? TEAM_YELLOW : TEAM_BLUE ) ) {
            playerTeam = localTeam;
        }
        if ( playerTeam < TEAM_RED ||
             playerTeam > ( isFourTeam ? TEAM_YELLOW : TEAM_BLUE ) ) {
            continue;
        }

        clients[count] = i;
        playerTeams[count] = playerTeam;
        playerScores[count] = cgs.clientinfo[i].score;
        if ( i == cg.snap->ps.clientNum ) {
            playerScores[count] = cg.snap->ps.persistant[PERS_SCORE];
            localIndex = count;
        }
        count++;
    }

    if ( localIndex < 0 && count < MAX_CLIENTS &&
         cg.snap->ps.clientNum >= 0 && cg.snap->ps.clientNum < MAX_CLIENTS &&
         localTeam >= TEAM_RED &&
         localTeam <= ( isFourTeam ? TEAM_YELLOW : TEAM_BLUE ) ) {
        localIndex = count;
        clients[count] = cg.snap->ps.clientNum;
        playerTeams[count] = localTeam;
        playerScores[count] = cg.snap->ps.persistant[PERS_SCORE];
        count++;
    }
    if ( count <= 0 ) {
        return;
    }

    /* Rank by score and keep server order stable for ties. */
    for ( i = 1; i < count; i++ ) {
        int savedClient = clients[i];
        int savedScore = playerScores[i];
        int savedTeam = playerTeams[i];
        j = i;
        while ( j > 0 && playerScores[j - 1] < savedScore ) {
            clients[j] = clients[j - 1];
            playerScores[j] = playerScores[j - 1];
            playerTeams[j] = playerTeams[j - 1];
            j--;
        }
        clients[j] = savedClient;
        playerScores[j] = savedScore;
        playerTeams[j] = savedTeam;
    }

    localIndex = -1;
    for ( i = 0; i < count; i++ ) {
        if ( clients[i] == cg.snap->ps.clientNum ) {
            localIndex = i;
            break;
        }
    }

    rowCount = count < RACE_ORDER_MAX_ROWS ? count : RACE_ORDER_MAX_ROWS;
    firstRow = 0;
    if ( count > rowCount && localIndex >= 0 ) {
        firstRow = localIndex - rowCount / 2;
        if ( firstRow < 0 ) {
            firstRow = 0;
        }
        if ( firstRow > count - rowCount ) {
            firstRow = count - rowCount;
        }
    }

    panelX = 640.0f - TEAM_DM_ORDER_PANEL_W;
    panelY = top;
    panelH = 57.0f + rowCount * RACE_ORDER_ROW_H;
    savedHorizontalPlacement = CG_GetScreenHorizontalPlacement();
    savedVerticalPlacement = CG_GetScreenVerticalPlacement();
    CG_SetScreenPlacement( PLACE_RIGHT, PLACE_TOP );

    CG_FillRect( panelX, panelY, TEAM_DM_ORDER_PANEL_W, panelH, panelColor );
    CG_FillRect( panelX, panelY, TEAM_DM_ORDER_PANEL_W, 2.0f, accentColor );
    CG_FillRect( panelX, panelY + 2.0f, TEAM_DM_ORDER_PANEL_W, 18.0f, headerColor );
    CG_DrawRect( panelX, panelY, TEAM_DM_ORDER_PANEL_W, panelH, 1.0f, borderColor );
    CG_FillRect( panelX + 7.0f, panelY + 20.0f,
                 TEAM_DM_ORDER_PANEL_W - 14.0f, 1.0f, borderColor );
    CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( panelY + 5.0f ),
                         isDomination ? "DOMINATION ORDER" :
                         ( isFourTeam ? "CTF4 DRIVER ORDER" : "CTF DRIVER ORDER" ),
                         UI_SMALLFONT, 0.56f, accentColor );

    chipY = panelY + 22.0f;
    chipW = ( TEAM_DM_ORDER_PANEL_W - 14.0f - ( teamCount - 1 ) * 3.0f ) /
            teamCount;
    for ( teamIndex = 0; teamIndex < teamCount; teamIndex++ ) {
        CG_GetCTFTeamColor( teamIndex, teamColor );
        chipX = panelX + 7.0f + teamIndex * ( chipW + 3.0f );
        teamFillColor[0] = teamColor[0];
        teamFillColor[1] = teamColor[1];
        teamFillColor[2] = teamColor[2];
        teamFillColor[3] = 0.20f;
        CG_FillRect( chipX, chipY, chipW, 16.0f, teamFillColor );
        CG_FillRect( chipX, chipY, 2.0f, 16.0f, teamColor );
        CG_DrawRect( chipX, chipY, chipW, 16.0f, 1.0f, borderColor );
        if ( isFourTeam ) {
            CG_DrawIngameString( (int)( chipX + 4.0f ), (int)( chipY + 4.0f ),
                                 teamShortCodes[teamIndex], UI_SMALLFONT,
                                 0.40f, teamColor );
        } else {
            CG_DrawIngameString( (int)( chipX + 5.0f ), (int)( chipY + 4.0f ),
                                 teamNames[teamIndex], UI_SMALLFONT,
                                 0.40f, teamColor );
        }
        flagShader = 0;
        if ( !isDomination ) {
            switch ( teamIndex ) {
            case 0: flagStatus = cgs.redflag; break;
            case 1: flagStatus = cgs.blueflag; break;
            case 2: flagStatus = cgs.greenflag; break;
            default: flagStatus = cgs.yellowflag; break;
            }
            if ( flagStatus >= 0 && flagStatus <= 2 ) {
                switch ( teamIndex ) {
                case 0: flagShader = cgs.media.redFlagShader[flagStatus]; break;
                case 1: flagShader = cgs.media.blueFlagShader[flagStatus]; break;
                case 2: flagShader = cgs.media.greenFlagShader[flagStatus]; break;
                default: flagShader = cgs.media.yellowFlagShader[flagStatus]; break;
                }
            }
        }
        if ( flagShader ) {
            trap_R_SetColor( NULL );
            CG_DrawPic( chipX + ( isFourTeam ? 15.0f : 39.0f ),
                        chipY + 2.0f, isFourTeam ? 10.0f : 13.0f,
                        12.0f, flagShader );
        }
        CG_DrawIngameString( (int)( chipX + chipW - 4.0f ),
                             (int)( chipY + 4.0f ),
                             va( "%d", cg.teamScores[teamIndex] ),
                             UI_RIGHT | UI_SMALLFONT, 0.42f, textColor );
    }

    CG_FillRect( panelX + 7.0f, panelY + 40.0f, TEAM_DM_ORDER_PANEL_W - 14.0f,
                 1.0f, borderColor );
    CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( panelY + 43.0f ),
                         "POS", UI_SMALLFONT, 0.40f, mutedColor );
    CG_DrawIngameString( (int)( panelX + 30.0f ), (int)( panelY + 43.0f ),
                         "DRIVER", UI_SMALLFONT, 0.40f, mutedColor );
    CG_DrawIngameString( (int)( panelX + 116.0f ), (int)( panelY + 43.0f ),
                         "TEAM", UI_SMALLFONT, 0.40f, mutedColor );
    CG_DrawIngameString( (int)( panelX + TEAM_DM_ORDER_PANEL_W - 8.0f ),
                         (int)( panelY + 43.0f ), isDomination ? "SCORE" : "FLAG",
                         UI_RIGHT | UI_SMALLFONT, 0.40f, mutedColor );
    CG_FillRect( panelX + 7.0f, panelY + 51.0f, TEAM_DM_ORDER_PANEL_W - 14.0f,
                 1.0f, borderColor );

    rowY = panelY + 53.0f;
    for ( row = 0; row < rowCount; row++ ) {
        i = firstRow + row;
        clientNum = clients[i];
        teamIndex = playerTeams[i] - TEAM_RED;
        if ( clientNum == cg.snap->ps.clientNum ) {
            CG_FillRect( panelX + 1.0f, rowY, TEAM_DM_ORDER_PANEL_W - 2.0f,
                         RACE_ORDER_ROW_H, selectedColor );
            CG_FillRect( panelX + 1.0f, rowY, 2.0f, RACE_ORDER_ROW_H, accentColor );
        } else {
            CG_FillRect( panelX + 1.0f, rowY, TEAM_DM_ORDER_PANEL_W - 2.0f,
                         RACE_ORDER_ROW_H, rowColor );
        }
        CG_DrawIngameString( (int)( panelX + 8.0f ), (int)( rowY + 3.0f ),
                             va( "%02d", i + 1 ), UI_SMALLFONT,
                             RACE_ORDER_TEXT_SCALE,
                             row == 0 ? accentColor : mutedColor );

        Q_strncpyz( name, cgs.clientinfo[clientNum].name, sizeof( name ) );
        while ( CG_IngameStringWidth( name, UI_SMALLFONT,
                                      RACE_ORDER_TEXT_SCALE ) > 78 ) {
            nameLength = strlen( name );
            if ( nameLength <= 1 ) {
                break;
            }
            if ( nameLength >= 2 && name[nameLength - 2] == '^' ) {
                name[nameLength - 2] = '\0';
            } else {
                name[nameLength - 1] = '\0';
            }
        }
        CG_DrawIngameString( (int)( panelX + 30.0f ), (int)( rowY + 3.0f ),
                             name, UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                             clientNum == cg.snap->ps.clientNum ? accentColor : textColor );
        CG_GetCTFTeamColor( teamIndex, teamColor );
        CG_FillRect( panelX + 116.0f, rowY + 4.0f, 2.0f, 8.0f, teamColor );
        CG_DrawIngameString( (int)( panelX + 122.0f ), (int)( rowY + 3.0f ),
                             isFourTeam ? teamShortCodes[teamIndex] : teamCodes[teamIndex],
                             UI_SMALLFONT, isFourTeam ? 0.42f : 0.38f,
                             teamColor );

        if ( isDomination ) {
            CG_DrawIngameString( (int)( panelX + TEAM_DM_ORDER_PANEL_W - 8.0f ),
                                 (int)( rowY + 3.0f ),
                                 va( "%d", playerScores[i] ),
                                 UI_RIGHT | UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                                 textColor );
        } else {
            carriedFlag = 0;
            if ( cgs.clientinfo[clientNum].powerups & ( 1 << PW_REDFLAG ) ) {
                carriedFlag = PW_REDFLAG;
            } else if ( cgs.clientinfo[clientNum].powerups & ( 1 << PW_BLUEFLAG ) ) {
                carriedFlag = PW_BLUEFLAG;
            } else if ( isFourTeam &&
                        ( cgs.clientinfo[clientNum].powerups & ( 1 << PW_GREENFLAG ) ) ) {
                carriedFlag = PW_GREENFLAG;
            } else if ( isFourTeam &&
                        ( cgs.clientinfo[clientNum].powerups & ( 1 << PW_YELLOWFLAG ) ) ) {
                carriedFlag = PW_YELLOWFLAG;
            }
            carriedFlagIcon = 0;
            if ( isFourTeam && carriedFlag ) {
                flagItem = BG_FindItemForPowerup( carriedFlag );
                if ( flagItem && flagItem->icon ) {
                    carriedFlagIcon = trap_R_RegisterShader( flagItem->icon );
                }
                if ( carriedFlagIcon ) {
                    trap_R_SetColor( NULL );
                    CG_DrawPic( panelX + TEAM_DM_ORDER_PANEL_W - 22.0f,
                                rowY + 2.0f, 12.0f, 12.0f, carriedFlagIcon );
                }
            } else if ( carriedFlag == PW_REDFLAG ) {
                Q_strncpyz( flagText, "RED", sizeof( flagText ) );
                Vector4Copy( colorRed, flagColor );
            } else if ( carriedFlag == PW_BLUEFLAG ) {
                Q_strncpyz( flagText, "BLUE", sizeof( flagText ) );
                Vector4Copy( colorBlue, flagColor );
            } else {
                Q_strncpyz( flagText, "--", sizeof( flagText ) );
                Vector4Copy( mutedColor, flagColor );
            }
            if ( !isFourTeam || !carriedFlagIcon ) {
                CG_DrawIngameString( (int)( panelX + TEAM_DM_ORDER_PANEL_W - 8.0f ),
                                     (int)( rowY + 3.0f ),
                                     isFourTeam ? "--" : flagText,
                                     UI_RIGHT | UI_SMALLFONT, RACE_ORDER_TEXT_SCALE,
                                     isFourTeam ? mutedColor : flagColor );
            }
        }
        CG_FillRect( panelX + 7.0f, rowY + RACE_ORDER_ROW_H - 1.0f,
                     TEAM_DM_ORDER_PANEL_W - 14.0f, 1.0f, borderColor );
        rowY += RACE_ORDER_ROW_H;
    }

    CG_SetScreenPlacement( savedHorizontalPlacement, savedVerticalPlacement );
}

/*
================================
CG_DrawHUD
Main HUD dispatcher, called each frame from CG_DrawActive().
================================
*/
qboolean CG_DrawHUD( void ) {
    /* Update all HUD toggle cvars from engine each frame */
    trap_Cvar_Update( &cg_hudOptionsOpen );
    trap_Cvar_Update( &cg_hudShowTimes );
    trap_Cvar_Update( &cg_hudShowLaps );
    trap_Cvar_Update( &cg_hudShowPosition );
    trap_Cvar_Update( &cg_hudShowDistToFinish );
    trap_Cvar_Update( &cg_ghostPlayback );
    trap_Cvar_Update( &cg_checkpointArrowMode );
    trap_Cvar_Update( &cg_hudShowCarAheadBehind );
    trap_Cvar_Update( &cg_elimTimeline );

    trap_Cvar_Update( &cg_hudShowOpponentList );
    trap_Cvar_Update( &cg_hudShowScores );
    trap_Cvar_Update( &cg_hudShowSpeed );
    trap_Cvar_Update( &cg_hudShowFuelGauge );
    trap_Cvar_Update( &cg_drawRearView );
    trap_Cvar_Update( &cg_drawMMap );
    trap_Cvar_Update( &cg_hudShowDerbyVehicle );
    trap_Cvar_Update( &cg_hudShowDerbyList );
    trap_Cvar_Update( &cg_hudShowKothHillStatus );
    trap_Cvar_Update( &cg_hudShowKothRespawnWave );

    if ( cg_paused.integer ) {
        return qfalse;
    }

    if ( !cg.showHUD ) {
        if ( cgs.gametype == GT_DERBY
             && cg_hudShowDerbyVehicle.integer ) {
            CG_DrawHUD_DerbyVehicleState();
        }
        return qfalse;
    }

    /* Keep scores fresh for accurate team DM times */
    if ( cg.scoresRequestTime + 2000 < cg.time ) {
        cg.scoresRequestTime = cg.time;
        trap_SendClientCommand( "score" );
    }

    if ( isRallyRace() && cg_hudShowOpponentList.integer ) {
        CG_DrawRacingOrderHUD( 10.0f );
    }
    if ( cgs.gametype == GT_DEATHMATCH && !cg.showScores &&
         cg.predictedPlayerState.pm_type != PM_INTERMISSION ) {
        CG_DrawDeathmatchOrderHUD( 10.0f );
    }
    if ( ( cgs.gametype == GT_TEAM || cgs.gametype == GT_KOTH ) && !cg.showScores &&
         cg.predictedPlayerState.pm_type != PM_INTERMISSION ) {
        CG_DrawTeamDeathmatchOrderHUD( 10.0f );
    }
    if ( ( cgs.gametype == GT_CTF || cgs.gametype == GT_CTF4 ||
           cgs.gametype == GT_DOMINATION ) &&
         !cg.showScores &&
         cg.predictedPlayerState.pm_type != PM_INTERMISSION ) {
        CG_DrawCTFOrderHUD( 10.0f );
    }

    switch ( cgs.gametype ) {

    default:
    case GT_RACING:
    case GT_SPRINT:
    case GT_TEAM_RACING:
        /* Rendered via legacy upper-right stack (CG_DrawUpperRightHUD). */
        break;

    case GT_ELIMINATION:
        /* Eliminations are shown in the driver list status column. */
        break;

    case GT_RACING_DM:
    case GT_TEAM_RACING_DM:
        /* Rendered via legacy upper-right stack (CG_DrawUpperRightHUD). */
        break;

    case GT_DEATHMATCH:
    case GT_TEAM:
    case GT_CTF:
    case GT_CTF4:
        /* Rendered via legacy upper-right stack (CG_DrawUpperRightHUD). */
        break;

    case GT_DOMINATION:
        /* Dedicated four-team driver and zone panels are drawn above. */
        break;

    // Q3Rally Code Start - KOTH
    case GT_KOTH:
        // KOTH uses the modular top-right scoreboard; avoid duplicate legacy FRAGS/TEAM panel.
        if ( cg_hudShowKothHillStatus.integer && !cg.showScores &&
             cg.predictedPlayerState.pm_type != PM_INTERMISSION ) {
            CG_DrawKOTH_HillStatus();
        }
        break;
    // Q3Rally Code END - KOTH

    case GT_DERBY:
        /* The results scoreboard takes over this corner at match end. */
        if ( cg_hudShowDerbyList.integer && !cg.showScores &&
             cg.predictedPlayerState.pm_type != PM_INTERMISSION ) {
            CG_DrawHUD_DerbyList( 440, 16 );
        }
        if ( cg_derbyHitFxEnable.integer )     CG_DrawHUD_DerbyHitImpact();
        break;

    case GT_LCS:
	{
		float y = 130.0f;
		qboolean showLcsOverview = ( !cg.showScores &&
			cg.predictedPlayerState.pm_type != PM_DEAD &&
			cg.predictedPlayerState.pm_type != PM_INTERMISSION );
		if ( showLcsOverview && cg_hudShowOpponentList.integer ) {
			y = CG_DrawHUD_LCSList( 440, 16 );
		}
		if ( showLcsOverview && cg_elimTimeline.integer ) {
			CG_DrawEliminationTimeline( y + 4.0f );
		}
        break;
	}
    }

    // CG_DrawHUD draws overlays but is not the scoreboard visibility gate.
    return qfalse;
}
