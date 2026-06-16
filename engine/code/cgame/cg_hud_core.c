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
        const float charWidth  = (float)TINYCHAR_WIDTH;
        const float insetWidth = HUD_TEXT_INSET * 2.0f;
        float       maxWidth   = insetWidth + charWidth * CG_DrawStrlen( "T: 00:00.000" );
        float       candidate;

#define CHECK_CANDIDATE(str) \
        candidate = insetWidth + charWidth * CG_DrawStrlen( str ); \
        if ( candidate > maxWidth ) { maxWidth = candidate; }

        CHECK_CANDIDATE( "L: 00:00.000" )
        CHECK_CANDIDATE( "B: 00:00.000" )
        CHECK_CANDIDATE( "LAP: 00/00" )
        CHECK_CANDIDATE( "POS: 00/00" )
        CHECK_CANDIDATE( "DIST: 0000m" )
        CHECK_CANDIDATE( "DIST: 100.0%" )
        CHECK_CANDIDATE( "D: +00.000" )
        CHECK_CANDIDATE( "PLAYERS LEFT: 000" )
        CHECK_CANDIDATE( "R99 LEFT63 Name (99)" )
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
#define HUDOPT_TITLE_SCALE   0.66f
#define HUDOPT_HINT_SCALE    0.40f
#define HUDOPT_SECTION_SCALE 0.46f
#define HUDOPT_ENTRY_SCALE   0.46f
#define HUDOPT_SLIDER_SCALE  0.40f
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
    { "Times Panel",         "cg_hudShowTimes",          &cg_hudShowTimes,          1, -1,       qfalse },
    { "Lap Counter",         "cg_hudShowLaps",           &cg_hudShowLaps,           1, -1,       qfalse },
    { "Race Position",       "cg_hudShowPosition",       &cg_hudShowPosition,       1, -1,       qfalse },
    { "Distance to Finish",  "cg_hudShowDistToFinish",   &cg_hudShowDistToFinish,   1, -1,       qfalse },
    { "Ghost Delta",         "cg_ghostPlayback",         &cg_ghostPlayback,         1, -1,       qfalse },
    { "Checkpoint Arrow",    "cg_checkpointArrowMode",   &cg_checkpointArrowMode,   2, -1,       qtrue  },
    { "Cars Ahead/Behind",   "cg_hudShowCarAheadBehind", &cg_hudShowCarAheadBehind, 1, -1,       qfalse },
    { "Elim. Timeline",      "cg_elimTimeline",          &cg_elimTimeline,          1, -1,       qfalse },
    { "Opponent List",       "cg_hudShowOpponentList",   &cg_hudShowOpponentList,   1, -1,       qfalse },
    { "Scores Panel",        "cg_hudShowScores",         &cg_hudShowScores,         1, -2,       qfalse },
    /* ---- Derby (right column top, indices 10-12) ---- */
    { "Derby Vehicle State", "cg_hudShowDerbyVehicle",   &cg_hudShowDerbyVehicle,   1, GT_DERBY, qfalse },
    { "Derby Scoreboard",    "cg_hudShowDerbyList",      &cg_hudShowDerbyList,      1, GT_DERBY, qfalse },
    { "Derby Hit Impact",    "cg_derbyHitFxEnable",      &cg_derbyHitFxEnable,      1, GT_DERBY, qfalse },
    /* ---- KOTH (right column middle, indices 13-14) ---- */
    { "KOTH Hill Status",   "cg_hudShowKothHillStatus",  &cg_hudShowKothHillStatus,  1, GT_KOTH,  qfalse },
    { "KOTH Respawn Wave",  "cg_hudShowKothRespawnWave", &cg_hudShowKothRespawnWave, 1, GT_KOTH,  qfalse },
    /* ---- Vehicle (right column bottom, indices 15-17) ---- */
    { "Speedometer",         "cg_hudShowSpeed",          &cg_hudShowSpeed,          1, -1,       qfalse },
    /* Fuel Gauge is part of Speedometer – hidden when Speedometer is OFF */
    { "Rear-View Mirror",    "cg_drawRearView",          &cg_drawRearView,          1, -1,       qfalse },
    { "Mini-Map",            "cg_drawMMap",              &cg_drawMMap,              1, -1,       qfalse },
};

#define HUDOPT_NUM_ENTRIES  ( (int)( sizeof(hudToggleTable) / sizeof(hudToggleTable[0]) ) )

/* Track which row the cursor hovers over (-1 = none) */
static int      g_hudOptHoverRow  = -1;
static int      g_hudOptHoverSlider = HUDOPT_SLIDER_NONE;

static void HUDOpt_DrawText( int x, int y, const char *text, vec4_t color, float scale ) {
    UI_DrawProportionalStringScaled( x, y, text, UI_LEFT|UI_DROPSHADOW, color, scale );
}

static void HUDOpt_DrawCenteredText( float x, float width, int y, const char *text, vec4_t color, float scale ) {
    UI_DrawProportionalStringScaled( (int)( x + width * 0.5f ), y, text,
        UI_CENTER|UI_DROPSHADOW, color, scale );
}

static void HUDOpt_DrawRightText( float right, int y, const char *text, vec4_t color, float scale ) {
    UI_DrawProportionalStringScaled( (int)right, y, text,
        UI_RIGHT|UI_DROPSHADOW, color, scale );
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
        case 0:  return "[ OFF ]";
        case 1:  return "[ON HUD]";
        case 2:  return "[ABCAR]";
        default: return "[ ??? ]";
        }
    }
    return e->cvar->integer ? "[ ON ]" : "[ OFF]";
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
        static vec4_t bgColor     = { 0.04f, 0.05f, 0.08f, 0.78f };
        static vec4_t bandColor   = { 0.08f, 0.10f, 0.16f, 0.86f };
        static vec4_t borderColor = { 0.34f, 0.48f, 0.76f, 0.72f };
        static vec4_t accentColor = { 0.50f, 0.70f, 1.00f, 1.00f };
        static vec4_t titleColor  = { 0.78f, 0.84f, 0.95f, 1.00f };
        static vec4_t secColor    = { 0.50f, 0.70f, 1.00f, 1.00f };
        static vec4_t labelColor  = { 1.00f, 1.00f, 1.00f, 1.00f };
        static vec4_t hoverColor  = { 0.18f, 0.28f, 0.44f, 0.62f };
        static vec4_t onColor     = { 0.40f, 1.00f, 0.58f, 1.00f };
        static vec4_t offColor    = { 0.60f, 0.66f, 0.77f, 1.00f };
        static vec4_t cycColor    = { 0.95f, 0.82f, 0.35f, 1.00f };
        static vec4_t naColor     = { 0.34f, 0.38f, 0.46f, 1.00f };
        static vec4_t greyColor   = { 0.34f, 0.38f, 0.46f, 1.00f };
        static vec4_t hintColor   = { 0.60f, 0.66f, 0.77f, 1.00f };
        static vec4_t divColor    = { 0.34f, 0.48f, 0.76f, 0.55f };

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

        /* Title – BIGCHAR, centred */
        {
            const char *title = "HUD ELEMENTS";
            HUDOpt_DrawCenteredText( HUDOPT_PNL_X, HUDOPT_PNL_W, (int)( HUDOPT_PNL_Y - 2 ),
                                     title, titleColor, HUDOPT_TITLE_SCALE );
        }

        /* Hint – TINYCHAR, centred */
        {
            const char *hint = "Click to toggle  |  Shift+H to open/close";
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
                CG_FillRect( HUDOPT_COL_L_X - 2.0f, rowY, HUDOPT_COL_W + 4.0f, HUDOPT_ROW_H, hoverColor );

            if ( unavail ) {
                badge = "[ n/a]";  badgeClr = naColor;  entryClr = greyColor;
            } else if ( e->isCycler ) {
                badge    = HUDEntry_BadgeLabel( e );
                badgeClr = e->cvar->integer ? cycColor : offColor;
                entryClr = labelColor;
            } else {
                badge    = e->cvar->integer ? "[ ON ]" : "[ OFF]";
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
                CG_FillRect( HUDOPT_COL_R_X - 2.0f, rowY, HUDOPT_COL_W + 4.0f, HUDOPT_ROW_H, hoverColor );

            badge    = unavail ? "[ n/a]" : ( e->cvar->integer ? "[ ON ]" : "[ OFF]" );
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
                CG_FillRect( HUDOPT_COL_R_X - 2.0f, rowY, HUDOPT_COL_W + 4.0f, HUDOPT_ROW_H, hoverColor );

            badge    = unavail ? "[ n/a]" : ( e->cvar->integer ? "[ ON ]" : "[ OFF]" );
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
                CG_FillRect( HUDOPT_COL_R_X - 2.0f, rowY, HUDOPT_COL_W + 4.0f, HUDOPT_ROW_H, hoverColor );

            badge    = unavail ? "[ n/a]" : ( e->cvar->integer ? "[ ON ]" : "[ OFF]" );
            badgeClr = unavail ? naColor : ( e->cvar->integer ? onColor : offColor );
            entryClr = unavail ? greyColor : labelColor;

            bx = (int)( HUDOPT_COL_R_X + HUDOPT_COL_W );
            HUDOpt_DrawText( (int)( HUDOPT_COL_R_X + 3 ), (int)( rowY + 3 ),
                             e->label, entryClr, HUDOPT_ENTRY_SCALE );
            HUDOpt_DrawRightText( bx, (int)( rowY + 3 ),
                                  badge, badgeClr, HUDOPT_ENTRY_SCALE );
            rowY += HUDOPT_ROW_H;
        }

        HUDOpt_DrawSlider( HUDOPT_COL_R_X, rowY, "Minimap Scale", cg_mmap_size.value,
                           0.40f, 2.00f, HUDOPT_SLIDER_SCALE_ID,
                           labelColor, offColor, divColor, accentColor, hoverColor );
        rowY += HUDOPT_ROW_H;

        HUDOpt_DrawSlider( HUDOPT_COL_R_X, rowY, "Minimap Zoom", cg_mmap_fov.value,
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

/*
================
CG_DrawUpperRightHUD
Draws the legacy right-side racing info stack.
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
        float timesStart = y;
        float timesY     = y;

        if ( cg_checkpointArrowMode.integer ) {
            y = CG_DrawArrowToCheckpoint( y );
            timesStart = y;
            timesY     = y;
        }

        CG_UpdateGhostSplitDelta();

        if ( cg_hudShowTimes.integer )        timesY = CG_DrawTimes( timesY );
        if ( cg_ghostPlayback.integer )       timesY = CG_DrawGhostSplitDelta( timesY );
        if ( cg_hudShowLaps.integer )         timesY = CG_DrawLaps( timesY );
        if ( cg_hudShowDistToFinish.integer ) timesY = CG_DrawDistanceToFinish( timesY );
        if ( cg_elimTimeline.integer )        timesY = CG_DrawEliminationTimeline( timesY );

        if ( cg_hudShowPosition.integer ) {
            CG_DrawCurrentPosition( timesStart );
        }

        if ( cg_hudShowCarAheadBehind.integer ) {
            y = CG_DrawCarAheadAndBehind( timesY );
        } else {
            y = timesY;
        }
    } else if ( cgs.gametype == GT_DERBY || cgs.gametype == GT_LCS ) {
        float timesStart = y;

        if ( cg_hudShowTimes.integer ) {
            y = CG_DrawTimes( y );
        }

        if ( cgs.gametype == GT_LCS && cg_hudShowPosition.integer ) {
            CG_DrawCurrentPosition( timesStart );
        }
    }

    if ( !isRallyNonDMRace() && cgs.gametype != GT_DERBY && cgs.gametype != GT_LCS ) {
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
    if ( cgs.clientinfo[cg.snap->ps.clientNum].team != TEAM_SPECTATOR ) {
        if ( cg_hudShowSpeed.integer ) {
            y = CG_DrawSpeed( y );
        }
    }
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

    switch ( cgs.gametype ) {

    default:
    case GT_RACING:
    case GT_SPRINT:
    case GT_TEAM_RACING:
        /* Rendered via legacy upper-right stack (CG_DrawUpperRightHUD). */
        break;

    case GT_ELIMINATION:
        /* Rendered via legacy upper-right stack (CG_DrawUpperRightHUD). */
        break;

    case GT_RACING_DM:
    case GT_TEAM_RACING_DM:
        /* Rendered via legacy upper-right stack (CG_DrawUpperRightHUD). */
        break;

    case GT_DEATHMATCH:
    case GT_TEAM:
    case GT_CTF:
    case GT_DOMINATION:
        /* Rendered via legacy upper-right stack (CG_DrawUpperRightHUD). */
        break;

    // Q3Rally Code Start - KOTH
    case GT_KOTH:
        // KOTH uses the modular top-right scoreboard; avoid duplicate legacy FRAGS/TEAM panel.
        if ( cg_hudShowKothHillStatus.integer ) {
            CG_DrawKOTH_HillStatus();
        }
        break;
    // Q3Rally Code END - KOTH

    case GT_DERBY:
        if ( cg_hudShowDerbyVehicle.integer )   CG_DrawHUD_DerbyVehicleState();
        if ( cg_hudShowDerbyList.integer )      CG_DrawHUD_DerbyList( 440, 16 );
        if ( cg_derbyHitFxEnable.integer )     CG_DrawHUD_DerbyHitImpact();
        break;

    case GT_LCS:
        if ( cg_hudShowOpponentList.integer )   CG_DrawCarAheadAndBehind( 130 );
        break;
    }

    // CG_DrawHUD draws overlays but is not the scoreboard visibility gate.
    return qfalse;
}
