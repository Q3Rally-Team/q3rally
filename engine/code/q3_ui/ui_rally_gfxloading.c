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
Q3Rally Graphics Loading Screen

Runs the UI cache stages one by one and presents them as a rally timing
sheet (SS1-SS8, real load time per stage). The layout mirrors the main menu
- rail, brand, hero panel, status chip and the player's car sit exactly
where the main menu draws them - so the hand-off at the end is a fade
instead of a cut. A start light sequence replaces the old 100% hold.

The online version check is shown as one line in the footer; a newer
release gets a card with "Update" / "Skip" in place of the drive tip.
===========================================================================
*/

#include "ui_local.h"
#include "ui_rally_theme.h"
#include "ui_rally_frontend.h"

/* -------------------------------------------------------------------------
   Timing (milliseconds)
   ------------------------------------------------------------------------- */

/* Deliberately unhurried: on a fast PC the cache work takes a few dozen
 * milliseconds, these minimums let the timing sheet and the lights play out.
 * Slower machines simply take longer per stage. */
#define GFX_STAGE_TIME          550     /* minimum time per stage */
#define GFX_GO_TIME            1200     /* from 100% until the lights turn green */
#define GFX_UPDATE_WAIT_TIME   2500     /* extra wait for a still-running version check */
#define GFX_GO_HOLD             450     /* green shown before the fade-out */
#define GFX_FADE_IN_TIME        500
#define GFX_FADE_OUT_TIME       350
#define GFX_CAR_SLIDE_TIME      450
#define GFX_MAX_DEBUG_PAUSE   15000     /* safety clamp for the test cvars */
#define GFX_SMOOTH_LERP_SPEED  4.5f     /* progress smoothing, units per second */
#define GFX_ZOOM_START         1.06f    /* backdrop push-out, ends at 1.0 */

/* Used when version.txt does not announce a download page. */
#define GFX_DEFAULT_DOWNLOAD_URL "https://www.q3rally.com/downloads"
#define GFX_GO_SOUND             "sound/rally/race/go.ogg"

/* -------------------------------------------------------------------------
   Layout - 640x480 virtual space, same grid as the main menu (ui_menu.c)
   ------------------------------------------------------------------------- */

#define GFX_RAIL_INSET          24      /* rail distance from the viewport edge */
#define GFX_PANEL_Y             32
#define GFX_PANEL_H            410
#define GFX_RAIL_W             210
#define GFX_RAIL_GAP            16      /* gap between rail and hero */
#define GFX_HERO_INSET          16

#define GFX_ROW_X               14      /* timing rows, relative to the rail */
#define GFX_ROW_W              182
#define GFX_ROW_Y               93
#define GFX_ROW_H               22
#define GFX_ROW_STEP            24
#define GFX_TOTAL_RULE_Y       289
#define GFX_TOTAL_Y            296
#define GFX_DRIVER_Y           312      /* = main menu profile card */
#define GFX_DRIVER_H            82

#define GFX_CAR_X               40      /* = main menu vehicle preview */
#define GFX_CAR_Y               92
#define GFX_CAR_W_INSET        120
#define GFX_CAR_H              228
#define GFX_CAR_SLIDE           24
#define GFX_CAR_YAW           204.0f

#define GFX_HEADER_Y            52
#define GFX_STAGE_CODE_Y       334
#define GFX_STAGE_TITLE_Y      350
#define GFX_PERCENT_Y          341
#define GFX_PERCENT_SCALE      1.7f
#define GFX_LIGHT_Y            349
#define GFX_LIGHT_SIZE          12
#define GFX_LIGHT_GAP            5
#define GFX_NUM_LIGHTS           5
#define GFX_BAR_Y              374
#define GFX_BAR_H                4
#define GFX_BAR_GAP              4
#define GFX_TIP_Y              392
#define GFX_TIP_LINE_H          13
#define GFX_TIP_INDENT          28
#define GFX_CARD_Y             386
#define GFX_CARD_H              46
#define GFX_BUTTON_W            72
#define GFX_BUTTON_H            22
#define GFX_FOOTER_Y           456

/* -------------------------------------------------------------------------
   Types
   ------------------------------------------------------------------------- */

typedef enum {
    GFX_BTN_NONE = -1,
    GFX_BTN_UPDATE,
    GFX_BTN_SKIP
} gfxButton_t;

/* Mirrors the cl_updateState values written by client/cl_update.c. */
typedef enum {
    GFX_UPD_IDLE,
    GFX_UPD_CHECKING,
    GFX_UPD_CURRENT,
    GFX_UPD_AHEAD,
    GFX_UPD_OUTDATED,
    GFX_UPD_OFFLINE,
    GFX_UPD_FAILED,
    GFX_UPD_UNAVAILABLE
} gfxUpdateState_t;

typedef struct {
    const char *code;       /* rail label */
    const char *title;      /* hero headline while the stage runs */
    void      (*exec)( void );
} gfxStage_t;

#define GFX_MAX_STAGES  16

typedef struct {
    menuframework_s  menu;

    /* the player's car, loaded by the vehicle stage */
    playerInfo_t     playerinfo;
    qboolean         carLoaded;
    int              carLoadedTime;
    char             driverName[64];
    char             carName[64];
    char             plateName[64];

    /* stages */
    int              startTime;
    int              currentStage;
    int              stageStartTime;
    qboolean         stageShown;            /* drawn at least once - run it next frame */
    qboolean         stageExecuted;
    int              stageMsec[GFX_MAX_STAGES];
    float            loadPercent;           /* raw progress [0.0 - 1.0] */
    float            smoothProgress;        /* smoothed value used for drawing */
    int              lastDrawTime;

    /* start lights and hand-off */
    qboolean         finalPhase;            /* all stages done */
    int              finalStartTime;
    int              goTime;                /* lights green, 0 until then */
    float            zoomAtGo;
    qboolean         skipRequested;
    qboolean         musicStarted;
    sfxHandle_t      goSound;

    /* drive tip */
    char             tip[256];

    /* update notice */
    gfxUpdateState_t updateState;
    qboolean         requireUpdateAck;      /* remote version newer than local */
    qboolean         updateAcked;           /* player answered the notice */
    gfxButton_t      ackedWith;
    gfxButton_t      hoveredBtn;            /* button under the mouse cursor */
    gfxButton_t      focusedBtn;            /* button activated by Enter / pad A */
    int              lastCursorX;
    int              lastCursorY;
} gfxloading_t;

static gfxloading_t s_gfxloading;

/* -------------------------------------------------------------------------
   Stages
   ------------------------------------------------------------------------- */

static void GFX_ReadDriver( void ) {
    char  buf[MAX_QPATH];
    char *slash;
    char *c;
    const char *profileName;

    profileName = UI_Profile_HasActiveProfile() ? UI_Profile_GetActiveName() : NULL;
    if ( profileName && profileName[0] ) {
        Q_strncpyz( s_gfxloading.driverName, profileName, sizeof( s_gfxloading.driverName ) );
    } else {
        trap_Cvar_VariableStringBuffer( "name", s_gfxloading.driverName, sizeof( s_gfxloading.driverName ) );
    }
    Q_CleanStr( s_gfxloading.driverName );

    /* "barracuda/default" -> "Barracuda" */
    trap_Cvar_VariableStringBuffer( "model", buf, sizeof( buf ) );
    if ( !buf[0] ) {
        Q_strncpyz( buf, DEFAULT_MODEL, sizeof( buf ) );
    }
    slash = strchr( buf, '/' );
    if ( slash ) {
        *slash = '\0';
    }
    buf[0] = toupper( (unsigned char)buf[0] );
    Q_strncpyz( s_gfxloading.carName, buf, sizeof( s_gfxloading.carName ) );

    /* "usa_california" -> "USA CALIFORNIA" */
    trap_Cvar_VariableStringBuffer( "plate", buf, sizeof( buf ) );
    for ( c = buf; *c; c++ ) {
        if ( *c == '_' ) {
            *c = ' ';
        }
    }
    Q_strupr( buf );
    Q_strncpyz( s_gfxloading.plateName, buf, sizeof( s_gfxloading.plateName ) );
}

/* Load the active car the same way the main menu does, so the preview can
 * stay on screen unchanged when the main menu takes over. */
static void GFXStage_Vehicle( void ) {
    char   model[MAX_QPATH];
    char   rim[MAX_QPATH];
    char   head[MAX_QPATH];
    char   plate[MAX_QPATH];
    vec3_t viewangles;
    vec3_t moveangles;

    trap_Cvar_VariableStringBuffer( "model", model, sizeof( model ) );
    if ( !model[0] ) {
        Com_sprintf( model, sizeof( model ), "%s/%s", DEFAULT_MODEL, DEFAULT_SKIN );
    } else if ( !strchr( model, '/' ) ) {
        Q_strcat( model, sizeof( model ), "/" DEFAULT_SKIN );
    }
    trap_Cvar_VariableStringBuffer( "rim",   rim,   sizeof( rim ) );
    trap_Cvar_VariableStringBuffer( "head",  head,  sizeof( head ) );
    trap_Cvar_VariableStringBuffer( "plate", plate, sizeof( plate ) );

    VectorClear( viewangles );
    VectorClear( moveangles );
    moveangles[YAW] = GFX_CAR_YAW;

    UI_PlayerInfo_SetModel( &s_gfxloading.playerinfo, model, rim, head, plate );
    UI_PlayerInfo_SetInfo( &s_gfxloading.playerinfo, LEGS_IDLE, TORSO_STAND,
                           viewangles, moveangles, WP_NONE, qfalse );

    s_gfxloading.carLoaded     = qtrue;
    s_gfxloading.carLoadedTime = trap_Milliseconds();
}

static const gfxStage_t gfxStages[] = {
    { "System",   "Starting up",              NULL                 },
    { "Vehicle",  "Loading your car",         GFXStage_Vehicle     },
    { "Setup",    "Preparing the setup menu", UI_SetupMenu_Cache   },
    { "Garage",   "Opening the garage",       PlayerModel_Cache    },
    { "Driver",   "Driver settings",          PlayerSettings_Cache },
    { "Controls", "Reading your controls",    Controls_Cache       },
    { "Servers",  "Server browser",           ArenaServers_Cache   },
    { "Tracks",   "Loading the track list",   StartServer_Cache    },
};

#define GFX_NUM_STAGES  ( (int)ARRAY_LEN( gfxStages ) )

/* compile-time check: stageMsec must hold every stage */
typedef char gfxStageCountCheck[( ARRAY_LEN( gfxStages ) <= GFX_MAX_STAGES ) ? 1 : -1];

/* -------------------------------------------------------------------------
   Loading tips - {command} is replaced with the key bound to it; a tip
   whose key is unbound is skipped.
   ------------------------------------------------------------------------- */

static const char * const loadingTips[] = {
    /* controls */
    "Tap {+button14} for the handbrake - it works best at medium speed, too fast and you'll spin out.",
    "Use the handbrake ({+button14}) to drift through tight corners.",
    "Hold {+speed} for turbo when the road opens up.",

    /* basics */
    "Keep your speed up when hitting jumps.",
    "Ramming opponents can knock them off the track.",
    "Collect power-ups to gain an edge on rivals.",
    "Watch for shortcuts to shave off lap times.",

    /* driving technique */
    "Tap the brakes before a corner, not during - you'll carry more speed.",
    "Countersteering after a drift keeps your car pointed the right way.",
    "Land jumps with a flat car to avoid losing control on impact.",

    /* tactics & racing */
    "The inside line isn't always fastest - sometimes the outside gives better exit speed.",
    "Ramming from the side is more effective than from behind.",
    "Save your power-ups for the last lap - that's when they matter most.",
    "Watch the minimap: knowing where rivals are is half the battle.",
    "Block the racing line on the final straight to deny an overtake.",

    /* tracks & shortcuts */
    "Every track has at least one shortcut - explore before you race.",
    "Wet surfaces reduce grip earlier than you'd expect - brake sooner.",
    "Jumps are faster if you hit the ramp dead center.",
    "Cutting corners too aggressively can launch you off the track entirely.",

    /* general */
    "First place isn't safe until you cross the finish line.",
    "A well-placed ram can knock two opponents off course at once.",
    "Sometimes slowing down slightly lets you set up a much faster corner exit.",
    "Race the ladder ghosts in Ghost Race to learn the fastest lines.",
};

static qboolean GFX_KeyForCommand( const char *command, char *out, int outSize ) {
    char binding[64];
    int  key;

    for ( key = 0; key < MAX_KEYS; key++ ) {
        trap_Key_GetBindingBuf( key, binding, sizeof( binding ) );
        if ( binding[0] && !Q_stricmp( binding, command ) ) {
            trap_Key_KeynumToStringBuf( key, out, outSize );
            Q_strupr( out );
            return qtrue;
        }
    }
    return qfalse;
}

static qboolean GFX_ResolveTip( const char *tip, char *out, int outSize ) {
    char        command[64];
    char        keyName[32];
    const char *s;
    int         n;

    out[0] = '\0';
    for ( s = tip; *s; s++ ) {
        if ( *s == '{' ) {
            for ( n = 0; s[n + 1] && s[n + 1] != '}' && n < (int)sizeof( command ) - 1; n++ ) {
                command[n] = s[n + 1];
            }
            command[n] = '\0';
            if ( s[n + 1] != '}' || !GFX_KeyForCommand( command, keyName, sizeof( keyName ) ) ) {
                return qfalse;
            }
            Q_strcat( out, outSize, keyName );
            s += n + 1;
        } else {
            char ch[2];
            ch[0] = *s;
            ch[1] = '\0';
            Q_strcat( out, outSize, ch );
        }
    }
    return qtrue;
}

static void GFX_PickTip( void ) {
    int count = ARRAY_LEN( loadingTips );
    int first = UI_RandomInt( count );
    int i;

    for ( i = 0; i < count; i++ ) {
        if ( GFX_ResolveTip( loadingTips[( first + i ) % count],
                             s_gfxloading.tip, sizeof( s_gfxloading.tip ) ) ) {
            return;
        }
    }
    s_gfxloading.tip[0] = '\0';
}

/* -------------------------------------------------------------------------
   Colors
   ------------------------------------------------------------------------- */

static const vec4_t gfxTextColor    = UI_FRONTEND_COLOR_TEXT;
static const vec4_t gfxMutedColor   = UI_FRONTEND_COLOR_MUTED;
static const vec4_t gfxAccentColor  = UI_FRONTEND_COLOR_ACCENT;
static const vec4_t gfxStatusColor  = UI_FRONTEND_COLOR_STATUS;
static const vec4_t gfxBorderColor  = UI_FRONTEND_COLOR_BORDER;
static const vec4_t gfxTrackColor   = UI_FRONTEND_COLOR_PROGRESS;
static const vec4_t gfxScrimColor   = UI_FRONTEND_COLOR_SCRIM;
static const vec4_t gfxSuccessColor = UI_THEME_COLOR_SUCCESS;
static const vec4_t gfxLightColor   = { 0.92f, 0.18f, 0.12f, 1.00f };
static const vec4_t gfxBlackColor   = { 0.00f, 0.00f, 0.00f, 1.00f };

static float *GFX_Color( vec4_t out, const float *base, float alpha ) {
    out[0] = base[0];
    out[1] = base[1];
    out[2] = base[2];
    out[3] = base[3] * alpha;
    return out;
}

/* -------------------------------------------------------------------------
   Layout helpers (widescreen aware, same as MainMenu_*)
   ------------------------------------------------------------------------- */

static float GFX_ViewportLeft( void ) {
    if ( uis.xscale <= 0.0f ) {
        return 0.0f;
    }
    return -uis.bias / uis.xscale;
}

static float GFX_RailX( void ) {
    return GFX_ViewportLeft() + GFX_RAIL_INSET;
}

static float GFX_HeroX( void ) {
    return GFX_RailX() + GFX_RAIL_W + GFX_RAIL_GAP;
}

static float GFX_HeroRight( void ) {
    return SCREEN_WIDTH - GFX_ViewportLeft() - GFX_RAIL_INSET;
}

/* -------------------------------------------------------------------------
   Update check state
   ------------------------------------------------------------------------- */

static gfxUpdateState_t GFX_ReadUpdateState( void ) {
    char state[32];

    trap_Cvar_VariableStringBuffer( "cl_updateState", state, sizeof( state ) );

    if ( !Q_stricmp( state, "checking" ) )    return GFX_UPD_CHECKING;
    if ( !Q_stricmp( state, "current" ) )     return GFX_UPD_CURRENT;
    if ( !Q_stricmp( state, "ahead" ) )       return GFX_UPD_AHEAD;
    if ( !Q_stricmp( state, "outdated" ) )    return GFX_UPD_OUTDATED;
    if ( !Q_stricmp( state, "offline" ) )     return GFX_UPD_OFFLINE;
    if ( !Q_stricmp( state, "failed" ) )      return GFX_UPD_FAILED;
    if ( !Q_stricmp( state, "unavailable" ) ) return GFX_UPD_UNAVAILABLE;
    return GFX_UPD_IDLE;
}

static qboolean GFX_UpdateCheckPending( void ) {
    return ( s_gfxloading.updateState == GFX_UPD_IDLE ||
             s_gfxloading.updateState == GFX_UPD_CHECKING ) ? qtrue : qfalse;
}

static qboolean GFX_UpdatePromptActive( void ) {
    return ( s_gfxloading.requireUpdateAck && !s_gfxloading.updateAcked ) ? qtrue : qfalse;
}

static void GFX_RefreshUpdateState( void ) {
    s_gfxloading.updateState = GFX_ReadUpdateState();

    if ( s_gfxloading.updateState == GFX_UPD_OUTDATED ) {
        if ( !s_gfxloading.requireUpdateAck ) {
            s_gfxloading.requireUpdateAck = qtrue;
            s_gfxloading.updateAcked      = qfalse;
            s_gfxloading.hoveredBtn       = GFX_BTN_NONE;
            s_gfxloading.focusedBtn       = GFX_BTN_UPDATE;
        }
    } else {
        s_gfxloading.requireUpdateAck = qfalse;
        s_gfxloading.updateAcked      = qfalse;
    }
}

static const char *GFX_DownloadURL( void ) {
    static char url[256];

    /* cl_updateUrl is validated by the client (official site only); the quote
     * check keeps the openURL command below well-formed regardless. */
    trap_Cvar_VariableStringBuffer( "cl_updateUrl", url, sizeof( url ) );
    if ( !url[0] || strchr( url, '"' ) ) {
        return GFX_DEFAULT_DOWNLOAD_URL;
    }
    return url;
}

static void GFX_ActivateButton( gfxButton_t button ) {
    if ( button == GFX_BTN_UPDATE ) {
        trap_Cmd_ExecuteText( EXEC_APPEND, va( "openURL \"%s\"\n", GFX_DownloadURL() ) );
    }
    /* both buttons acknowledge the notice so the game continues */
    s_gfxloading.updateAcked = qtrue;
    s_gfxloading.ackedWith   = button;
}

/* -------------------------------------------------------------------------
   Stage execution and progress
   ------------------------------------------------------------------------- */

static int GFX_GetPause( const char *cvarName, int fallback ) {
    int value = (int)trap_Cvar_VariableValue( cvarName );

    if ( value <= 0 ) {
        return fallback;
    }
    if ( value > GFX_MAX_DEBUG_PAUSE ) {
        return GFX_MAX_DEBUG_PAUSE;
    }
    return value;
}

/*
 * A stage runs in the frame after its name was first drawn, so a slow cache
 * call never freezes the screen on the previous stage. Its run time is what
 * the timing sheet shows.
 */
static void GFX_ExecuteStage( void ) {
    const gfxStage_t *stage;
    int               start;

    if ( s_gfxloading.stageExecuted || !s_gfxloading.stageShown ||
         s_gfxloading.currentStage >= GFX_NUM_STAGES ) {
        return;
    }

    stage = &gfxStages[s_gfxloading.currentStage];
    start = trap_Milliseconds();
    if ( stage->exec ) {
        stage->exec();
    }
    s_gfxloading.stageMsec[s_gfxloading.currentStage] = trap_Milliseconds() - start;
    s_gfxloading.stageExecuted = qtrue;
}

static void GFX_UpdateProgress( int currentTime ) {
    int   stageTime = GFX_GetPause( "ui_gfxLoadingStagePause", GFX_STAGE_TIME );
    int   elapsed;
    float stageProgress;

    GFX_ExecuteStage();

    if ( s_gfxloading.currentStage >= GFX_NUM_STAGES ) {
        return;
    }

    elapsed = currentTime - s_gfxloading.stageStartTime;
    if ( elapsed >= stageTime && s_gfxloading.stageExecuted ) {
        s_gfxloading.currentStage++;
        s_gfxloading.stageStartTime = currentTime;
        s_gfxloading.stageShown     = qfalse;
        s_gfxloading.stageExecuted  = qfalse;
        s_gfxloading.loadPercent    = (float)s_gfxloading.currentStage / (float)GFX_NUM_STAGES;

        if ( s_gfxloading.currentStage >= GFX_NUM_STAGES ) {
            s_gfxloading.loadPercent    = 1.0f;
            s_gfxloading.finalPhase     = qtrue;
            s_gfxloading.finalStartTime = currentTime;
        }
        return;
    }

    stageProgress = (float)elapsed / (float)stageTime;
    if ( stageProgress > 1.0f ) {
        stageProgress = 1.0f;
    }
    s_gfxloading.loadPercent =
        ( (float)s_gfxloading.currentStage + stageProgress ) / (float)GFX_NUM_STAGES;
}

static void GFX_UpdateSmoothProgress( int currentTime ) {
    float deltaTime;

    deltaTime = ( s_gfxloading.lastDrawTime > 0 )
                ? (float)( currentTime - s_gfxloading.lastDrawTime ) * 0.001f
                : 0.016f;
    if ( deltaTime > 0.1f ) {
        deltaTime = 0.1f;   /* avoid a jump after a hitch */
    }
    s_gfxloading.lastDrawTime = currentTime;

    s_gfxloading.smoothProgress +=
        ( s_gfxloading.loadPercent - s_gfxloading.smoothProgress ) * ( GFX_SMOOTH_LERP_SPEED * deltaTime );
    if ( s_gfxloading.loadPercent >= 1.0f && s_gfxloading.smoothProgress > 0.995f ) {
        s_gfxloading.smoothProgress = 1.0f;
    }
}

/* -------------------------------------------------------------------------
   Start lights and hand-off
   ------------------------------------------------------------------------- */

static int GFX_GoDelay( void ) {
    return GFX_GetPause( "ui_gfxLoadingFinalPause", GFX_GO_TIME );
}

/* Nominal run time, used to pace the backdrop push-out. */
static int GFX_NominalDuration( void ) {
    return GFX_NUM_STAGES * GFX_GetPause( "ui_gfxLoadingStagePause", GFX_STAGE_TIME ) +
           GFX_GoDelay() + GFX_GO_HOLD;
}

static float GFX_CurrentZoom( int currentTime ) {
    float k;

    if ( s_gfxloading.goTime ) {
        /* finish the push-out by the time the main menu takes over */
        k = (float)( currentTime - s_gfxloading.goTime ) / (float)( GFX_GO_HOLD + GFX_FADE_OUT_TIME );
        if ( k > 1.0f ) {
            k = 1.0f;
        }
        return s_gfxloading.zoomAtGo + ( 1.0f - s_gfxloading.zoomAtGo ) * k;
    }

    k = (float)( currentTime - s_gfxloading.startTime ) / (float)GFX_NominalDuration();
    if ( k > 1.0f ) {
        k = 1.0f;
    }
    k = 1.0f - ( 1.0f - k ) * ( 1.0f - k ) * ( 1.0f - k );   /* ease out */
    return GFX_ZOOM_START + ( 1.0f - GFX_ZOOM_START ) * k;
}

/*
 * The lights go green once 100% has been shown long enough. A pending
 * version check gets a short grace period so its notice is not lost, and an
 * open update notice always waits for an answer.
 */
static void GFX_UpdateStartLights( int currentTime ) {
    int since;
    int goDelay;

    if ( !s_gfxloading.finalPhase || GFX_UpdatePromptActive() ) {
        return;
    }

    if ( s_gfxloading.goTime ) {
        /* skipping after green jumps straight to the fade-out */
        if ( s_gfxloading.skipRequested &&
             currentTime - s_gfxloading.goTime < GFX_GO_HOLD ) {
            s_gfxloading.goTime = currentTime - GFX_GO_HOLD;
        }
        return;
    }

    since   = currentTime - s_gfxloading.finalStartTime;
    goDelay = GFX_GoDelay();

    if ( !s_gfxloading.skipRequested ) {
        if ( since < goDelay || s_gfxloading.smoothProgress < 0.98f ) {
            return;
        }
        if ( GFX_UpdateCheckPending() && since < goDelay + GFX_UPDATE_WAIT_TIME ) {
            return;
        }
    }

    s_gfxloading.zoomAtGo = GFX_CurrentZoom( currentTime );
    s_gfxloading.goTime   = currentTime > 0 ? currentTime : 1;
    if ( s_gfxloading.skipRequested ) {
        s_gfxloading.goTime -= GFX_GO_HOLD;
    } else if ( s_gfxloading.goSound ) {
        trap_S_StartLocalSound( s_gfxloading.goSound, CHAN_LOCAL_SOUND );
    }
}

static float GFX_ContentAlpha( int currentTime ) {
    int fadeStart;
    float a;

    if ( !s_gfxloading.goTime ) {
        return 1.0f;
    }
    fadeStart = s_gfxloading.goTime + GFX_GO_HOLD;
    if ( currentTime <= fadeStart ) {
        return 1.0f;
    }
    a = 1.0f - (float)( currentTime - fadeStart ) / (float)GFX_FADE_OUT_TIME;
    return a < 0.0f ? 0.0f : a;
}

static qboolean GFX_HandoffDue( int currentTime ) {
    return ( s_gfxloading.goTime &&
             currentTime >= s_gfxloading.goTime + GFX_GO_HOLD + GFX_FADE_OUT_TIME ) ? qtrue : qfalse;
}

static void GFX_HandOffToMainMenu( void ) {
    /* The main menu keeps this backdrop, the music and the car's position,
     * and only fades its own panels in. UI_MainMenu resets the menu stack. */
    Frontend_KeepBackgroundForNextMenu();
    uis.menuMusicCarryOver = s_gfxloading.musicStarted;
    uis.frontendHandoff    = s_gfxloading.carLoaded;
    UI_MainMenu();
}

/* -------------------------------------------------------------------------
   Drawing
   ------------------------------------------------------------------------- */

static void GFX_DrawTimingSheet( float railX, float a, int currentTime ) {
    vec4_t color;
    int    i;
    int    total = 0;
    char   buf[32];

    for ( i = 0; i < GFX_NUM_STAGES; i++ ) {
        int      top      = GFX_ROW_Y + i * GFX_ROW_STEP;
        int      textY    = top + ( GFX_ROW_H - UI_FRONTEND_SMALL_GLYPH_SIZE ) / 2 + 1;
        qboolean current  = ( i == s_gfxloading.currentStage ) ? qtrue : qfalse;
        qboolean pending  = ( i > s_gfxloading.currentStage ) ? qtrue : qfalse;
        qboolean executed = ( i < s_gfxloading.currentStage || ( current && s_gfxloading.stageExecuted ) ) ? qtrue : qfalse;

        if ( current ) {
            Frontend_DrawPanel( (int)railX + GFX_ROW_X, top, GFX_ROW_W, GFX_ROW_H,
                                a, UI_FRONTEND_STYLE_ACTIVE );
        }

        Frontend_DrawText( (int)railX + 24, textY, va( "SS%d", i + 1 ), UI_LEFT | UI_SMALLFONT,
                           pending ? GFX_Color( color, gfxMutedColor, a * 0.5f )
                                   : GFX_Color( color, gfxAccentColor, a ) );
        Frontend_DrawText( (int)railX + 52, textY, gfxStages[i].code, UI_LEFT | UI_SMALLFONT,
                           pending ? GFX_Color( color, gfxMutedColor, a * 0.55f )
                                   : GFX_Color( color, gfxTextColor, a ) );

        if ( executed ) {
            Com_sprintf( buf, sizeof( buf ), "%d.%03d",
                         s_gfxloading.stageMsec[i] / 1000, s_gfxloading.stageMsec[i] % 1000 );
            GFX_Color( color, current ? gfxTextColor : gfxMutedColor, a );
            total += s_gfxloading.stageMsec[i];
        } else if ( current ) {
            static const char * const dots[] = { ".  ", ".. ", "..." };
            Q_strncpyz( buf, dots[( currentTime / 160 ) % ARRAY_LEN( dots )], sizeof( buf ) );
            GFX_Color( color, gfxAccentColor, a );
        } else {
            Q_strncpyz( buf, "-.---", sizeof( buf ) );
            GFX_Color( color, gfxMutedColor, a * 0.35f );
        }
        Frontend_DrawText( (int)railX + 188, textY, buf, UI_RIGHT | UI_SMALLFONT, color );
    }

    UI_FillRect( railX + GFX_ROW_X, GFX_TOTAL_RULE_Y, GFX_ROW_W, 1,
                 GFX_Color( color, gfxBorderColor, a * 0.8f ) );
    Frontend_DrawText( (int)railX + 24, GFX_TOTAL_Y, "TOTAL", UI_LEFT | UI_SMALLFONT,
                       GFX_Color( color, gfxMutedColor, a ) );
    Com_sprintf( buf, sizeof( buf ), "%d.%03ds", total / 1000, total % 1000 );
    Frontend_DrawText( (int)railX + 188, GFX_TOTAL_Y, buf, UI_RIGHT | UI_SMALLFONT,
                       s_gfxloading.finalPhase ? GFX_Color( color, gfxSuccessColor, a )
                                               : GFX_Color( color, gfxTextColor, a ) );
}

static void GFX_DrawDriverCard( float railX, float a ) {
    vec4_t color;
    char   buf[96];
    int    x     = (int)railX + GFX_ROW_X;
    int    textW = GFX_ROW_W - 30 - 12;

    Frontend_DrawCard( x, GFX_DRIVER_Y, GFX_ROW_W, GFX_DRIVER_H, a, qfalse );
    Frontend_DrawStatusChip( x + 12, GFX_DRIVER_Y + 10, "Driver",
                             s_gfxloading.carLoaded ? gfxAccentColor : gfxStatusColor, a );

    Frontend_FitText( buf, sizeof( buf ), s_gfxloading.driverName, textW, UI_SMALLFONT );
    Frontend_DrawText( x + 30, GFX_DRIVER_Y + 27, buf, UI_LEFT | UI_SMALLFONT | UI_DROPSHADOW,
                       GFX_Color( color, gfxTextColor, a ) );

    Frontend_FitText( buf, sizeof( buf ),
                      s_gfxloading.carLoaded ? va( "Car  -  %s", s_gfxloading.carName ) : "Car  -  loading...",
                      textW, UI_SMALLFONT );
    Frontend_DrawText( x + 30, GFX_DRIVER_Y + 48, buf, UI_LEFT | UI_SMALLFONT,
                       GFX_Color( color, gfxMutedColor, a ) );

    Frontend_FitText( buf, sizeof( buf ), va( "Plate  -  %s", s_gfxloading.plateName ),
                      textW, UI_SMALLFONT );
    Frontend_DrawText( x + 30, GFX_DRIVER_Y + 63, buf, UI_LEFT | UI_SMALLFONT,
                       GFX_Color( color, gfxMutedColor, a ) );
}

static void GFX_DrawCar( float heroX, float heroWidth, int currentTime ) {
    qboolean wasMainMenu;
    float    k;
    float    slide;

    if ( !s_gfxloading.carLoaded ) {
        return;
    }

    k = (float)( currentTime - s_gfxloading.carLoadedTime ) / (float)GFX_CAR_SLIDE_TIME;
    if ( k > 1.0f ) {
        k = 1.0f;
    }
    slide = -GFX_CAR_SLIDE * ( 1.0f - k ) * ( 1.0f - k );

    /* UI_DrawPlayer only keeps a fixed yaw for the main menu preview;
     * borrow that mode so the car matches the main menu exactly. */
    wasMainMenu  = uis.mainMenu;
    uis.mainMenu = qtrue;
    UI_DrawPlayer( (int)( heroX + GFX_CAR_X ) + slide, GFX_CAR_Y,
                   (int)( heroWidth - GFX_CAR_W_INSET ), GFX_CAR_H,
                   &s_gfxloading.playerinfo, uis.realtime );
    uis.mainMenu = wasMainMenu;
}

static void GFX_DrawStartLights( int right, float a, int currentTime ) {
    vec4_t color;
    int    since   = currentTime - s_gfxloading.finalStartTime;
    int    goDelay = GFX_GoDelay();
    int    i;

    for ( i = 0; i < GFX_NUM_LIGHTS; i++ ) {
        int x  = right - ( GFX_NUM_LIGHTS - i ) * GFX_LIGHT_SIZE - ( GFX_NUM_LIGHTS - 1 - i ) * GFX_LIGHT_GAP;
        int on = goDelay * ( 1 + i ) / ( GFX_NUM_LIGHTS + 1 );   /* evenly spaced, green follows */

        if ( s_gfxloading.goTime ) {
            GFX_Color( color, gfxSuccessColor, a );
        } else if ( since >= on ) {
            /* still waiting after the sequence: pulse */
            float pulse = ( since > goDelay ) ? 0.7f + 0.3f * sin( currentTime / 140.0f ) : 1.0f;
            GFX_Color( color, gfxLightColor, a * pulse );
        } else {
            GFX_Color( color, gfxTrackColor, a );
        }
        UI_FillRect( x, GFX_LIGHT_Y, GFX_LIGHT_SIZE, GFX_LIGHT_SIZE, color );
    }
}

static void GFX_DrawStageBlock( int innerX, int innerRight, float a, int currentTime ) {
    vec4_t      color;
    char        title[128];
    const char *headline;
    int         innerW = innerRight - innerX;
    int         rightW;
    int         i;
    float       segW;
    qboolean    final = s_gfxloading.finalPhase;

    if ( final ) {
        if ( s_gfxloading.goTime ) {
            headline = "Ready to race";
        } else if ( GFX_UpdatePromptActive() ) {
            headline = "Waiting for you";
        } else if ( GFX_UpdateCheckPending() &&
                    currentTime - s_gfxloading.finalStartTime > GFX_GoDelay() ) {
            headline = "Waiting for the start signal";
        } else {
            headline = "Ready to race";
        }
        rightW = GFX_NUM_LIGHTS * GFX_LIGHT_SIZE + ( GFX_NUM_LIGHTS - 1 ) * GFX_LIGHT_GAP;
    } else {
        headline = gfxStages[s_gfxloading.currentStage].title;
        rightW   = (int)( Frontend_TextWidth( "100%", UI_BIGFONT ) * GFX_PERCENT_SCALE );
    }

    Frontend_DrawText( innerX, GFX_STAGE_CODE_Y,
                       final ? "FINISH" : va( "SS%d", s_gfxloading.currentStage + 1 ),
                       UI_LEFT | UI_SMALLFONT, GFX_Color( color, gfxAccentColor, a ) );
    Frontend_FitText( title, sizeof( title ), headline, innerW - rightW - 16, UI_BIGFONT );
    Frontend_DrawText( innerX, GFX_STAGE_TITLE_Y, title, UI_LEFT | UI_BIGFONT | UI_DROPSHADOW,
                       GFX_Color( color, gfxTextColor, a ) );

    if ( final ) {
        GFX_DrawStartLights( innerRight, a, currentTime );
    } else {
        Frontend_DrawTextScaled( innerRight, GFX_PERCENT_Y,
                                 va( "%d%%", (int)( s_gfxloading.smoothProgress * 100.0f ) ),
                                 UI_RIGHT | UI_BIGFONT, GFX_PERCENT_SCALE,
                                 GFX_Color( color, gfxAccentColor, a ) );
    }

    /* one segment per stage */
    segW = (float)( innerW - GFX_BAR_GAP * ( GFX_NUM_STAGES - 1 ) ) / (float)GFX_NUM_STAGES;
    for ( i = 0; i < GFX_NUM_STAGES; i++ ) {
        float x    = innerX + i * ( segW + GFX_BAR_GAP );
        float fill = s_gfxloading.smoothProgress * GFX_NUM_STAGES - i;

        UI_FillRect( x, GFX_BAR_Y, segW, GFX_BAR_H, GFX_Color( color, gfxTrackColor, a ) );
        if ( fill > 0.0f ) {
            if ( fill > 1.0f ) {
                fill = 1.0f;
            }
            UI_FillRect( x, GFX_BAR_Y, segW * fill, GFX_BAR_H,
                         s_gfxloading.goTime ? GFX_Color( color, gfxSuccessColor, a )
                                             : GFX_Color( color, gfxAccentColor, a ) );
        }
    }
}

static void GFX_DrawUpdateButtons( int x, int y, int width, float a ) {
    int         buttonY = y + 14;
    int         skipX   = x + width - 10 - GFX_BUTTON_W;
    int         updateX = skipX - 6 - GFX_BUTTON_W;
    qboolean    hoverUpdate;
    qboolean    hoverSkip;
    gfxButton_t hovered;

    hoverUpdate = Frontend_DrawButtonFocused( updateX, buttonY, GFX_BUTTON_W, GFX_BUTTON_H,
                                              "Update", a,
                                              s_gfxloading.focusedBtn == GFX_BTN_UPDATE,
                                              UI_FRONTEND_TEXT_CENTER );
    hoverSkip   = Frontend_DrawButtonFocused( skipX, buttonY, GFX_BUTTON_W, GFX_BUTTON_H,
                                              "Skip", a,
                                              s_gfxloading.focusedBtn == GFX_BTN_SKIP,
                                              UI_FRONTEND_TEXT_CENTER );

    hovered = hoverUpdate ? GFX_BTN_UPDATE : ( hoverSkip ? GFX_BTN_SKIP : GFX_BTN_NONE );
    s_gfxloading.hoveredBtn = hovered;

    /* The mouse only takes over the focus when it actually moves, so a
     * resting cursor does not fight keyboard / gamepad navigation. */
    if ( hovered != GFX_BTN_NONE &&
         ( uis.cursorx != s_gfxloading.lastCursorX || uis.cursory != s_gfxloading.lastCursorY ) ) {
        s_gfxloading.focusedBtn = hovered;
    }
    s_gfxloading.lastCursorX = uis.cursorx;
    s_gfxloading.lastCursorY = uis.cursory;
}

/* A newer release replaces the drive tip with a card; otherwise the tip. */
static void GFX_DrawTipOrUpdate( int innerX, int innerRight, float a ) {
    vec4_t color;
    char   remoteVersion[64];
    char   remoteDate[64];
    char   buf[128];
    int    innerW = innerRight - innerX;

    s_gfxloading.hoveredBtn = GFX_BTN_NONE;

    if ( s_gfxloading.updateState != GFX_UPD_OUTDATED ) {
        if ( s_gfxloading.tip[0] ) {
            Frontend_DrawText( innerX, GFX_TIP_Y, "TIP", UI_LEFT | UI_SMALLFONT,
                               GFX_Color( color, gfxMutedColor, a ) );
            Frontend_DrawTextWrapped( innerX + GFX_TIP_INDENT, GFX_TIP_Y, innerW - GFX_TIP_INDENT,
                                      GFX_TIP_LINE_H, 2, s_gfxloading.tip, UI_LEFT | UI_SMALLFONT,
                                      GFX_Color( color, gfxTextColor, a ) );
        }
        return;
    }

    trap_Cvar_VariableStringBuffer( "cl_updateRemote", remoteVersion, sizeof( remoteVersion ) );
    trap_Cvar_VariableStringBuffer( "cl_updateDate", remoteDate, sizeof( remoteDate ) );

    Frontend_DrawCard( innerX, GFX_CARD_Y, innerW, GFX_CARD_H, a, qfalse );
    UI_FillRect( innerX, GFX_CARD_Y, 3, GFX_CARD_H, GFX_Color( color, gfxStatusColor, a ) );
    Frontend_DrawText( innerX + 14, GFX_CARD_Y + 10,
                       remoteVersion[0] ? va( "Update %s available", remoteVersion ) : "Update available",
                       UI_LEFT | UI_SMALLFONT, GFX_Color( color, gfxStatusColor, a ) );

    if ( GFX_UpdatePromptActive() ) {
        int infoW = innerW - 14 - 2 * GFX_BUTTON_W - 6 - 10 - 8;

        Com_sprintf( buf, sizeof( buf ), "You have %s  -  released %s", PRODUCT_VERSION, remoteDate );
        if ( !remoteDate[0] || Frontend_TextWidth( buf, UI_SMALLFONT ) > infoW ) {
            Com_sprintf( buf, sizeof( buf ), "You have %s", PRODUCT_VERSION );
        }
        Frontend_DrawText( innerX + 14, GFX_CARD_Y + 26, buf, UI_LEFT | UI_SMALLFONT,
                           GFX_Color( color, gfxMutedColor, a ) );
        GFX_DrawUpdateButtons( innerX, GFX_CARD_Y, innerW, a );
    } else {
        Frontend_FitText( buf, sizeof( buf ),
                          s_gfxloading.ackedWith == GFX_BTN_UPDATE
                              ? "Download page opened in your browser"
                              : "Skipped  -  you can update later from the main menu",
                          innerW - 14 - 10, UI_SMALLFONT );
        Frontend_DrawText( innerX + 14, GFX_CARD_Y + 26, buf, UI_LEFT | UI_SMALLFONT,
                           GFX_Color( color, gfxMutedColor, a ) );
    }
}

/* Footer: build plus a one-line update status, centred like the main menu's. */
static void GFX_DrawFooter( float a, int currentTime ) {
    static const char * const dots[] = { "", ".", "..", "..." };
    vec4_t       color;
    vec4_t       dotColor;
    char         remoteVersion[64];
    char         status[96];
    const float *statusColor = gfxTextColor;
    const float *statusDot   = gfxAccentColor;
    const char  *prefix;
    int          width;
    int          x;

    trap_Cvar_VariableStringBuffer( "cl_updateRemote", remoteVersion, sizeof( remoteVersion ) );

    switch ( s_gfxloading.updateState ) {
    case GFX_UPD_CURRENT:
        Q_strncpyz( status, "Up to date", sizeof( status ) );
        break;
    case GFX_UPD_AHEAD:
        Com_sprintf( status, sizeof( status ), "Development build  -  newer than %s", remoteVersion );
        break;
    case GFX_UPD_OUTDATED:
        Com_sprintf( status, sizeof( status ), "Update %s available", remoteVersion );
        statusColor = statusDot = gfxStatusColor;
        break;
    case GFX_UPD_OFFLINE:
        Q_strncpyz( status, "Update service offline", sizeof( status ) );
        statusDot = gfxStatusColor;
        break;
    case GFX_UPD_FAILED:
        Q_strncpyz( status, "Update check failed", sizeof( status ) );
        statusDot = gfxStatusColor;
        break;
    case GFX_UPD_UNAVAILABLE:
        Q_strncpyz( status, "No online update check in this build", sizeof( status ) );
        statusColor = statusDot = gfxMutedColor;
        break;
    case GFX_UPD_IDLE:
    case GFX_UPD_CHECKING:
    default:
        Com_sprintf( status, sizeof( status ), "Checking for updates%s",
                     dots[( currentTime / 400 ) % ARRAY_LEN( dots )] );
        statusColor = statusDot = gfxMutedColor;
        break;
    }

    prefix = va( "Q3Rally %s   |   ", PRODUCT_VERSION );
    /* centre on the finished text so the animated dots do not shift it */
    width  = Frontend_TextWidth( prefix, UI_SMALLFONT ) + UI_FRONTEND_STATUS_DOT + UI_FRONTEND_SPACE_SM +
             Frontend_TextWidth( s_gfxloading.updateState <= GFX_UPD_CHECKING ? "Checking for updates..." : status,
                                 UI_SMALLFONT );
    x = SCREEN_WIDTH / 2 - width / 2;

    Frontend_DrawText( x, GFX_FOOTER_Y, prefix, UI_LEFT | UI_SMALLFONT, GFX_Color( color, gfxMutedColor, a ) );
    x += Frontend_TextWidth( prefix, UI_SMALLFONT );
    UI_FillRect( x, GFX_FOOTER_Y + 1, UI_FRONTEND_STATUS_DOT, UI_FRONTEND_STATUS_DOT,
                 GFX_Color( dotColor, statusDot, a ) );
    Frontend_DrawText( x + UI_FRONTEND_STATUS_DOT + UI_FRONTEND_SPACE_SM, GFX_FOOTER_Y, status,
                       UI_LEFT | UI_SMALLFONT, GFX_Color( color, statusColor, a ) );
}

static void UI_GFX_Loading_MenuDraw( void ) {
    int    currentTime = trap_Milliseconds();
    float  railX       = GFX_RailX();
    float  heroX       = GFX_HeroX();
    float  heroWidth   = GFX_HeroRight() - heroX;
    int    innerX      = (int)heroX + GFX_HERO_INSET;
    int    innerRight  = (int)GFX_HeroRight() - GFX_HERO_INSET;
    float  a;
    float  fadeIn;
    const char *chipLabel;
    vec4_t color;

    GFX_RefreshUpdateState();
    GFX_UpdateProgress( currentTime );
    GFX_UpdateSmoothProgress( currentTime );
    GFX_UpdateStartLights( currentTime );

    if ( GFX_HandoffDue( currentTime ) ) {
        GFX_HandOffToMainMenu();
        return;
    }

    a = GFX_ContentAlpha( currentTime );

    /* backdrop with a slow push-out that ends on the main menu's framing */
    Frontend_DrawBackgroundZoomed( gfxScrimColor, GFX_CurrentZoom( currentTime ) );

    /* rail: brand, timing sheet, driver */
    Frontend_DrawPanel( (int)railX, GFX_PANEL_Y, GFX_RAIL_W, GFX_PANEL_H, a, UI_FRONTEND_STYLE_SURFACE );
    UI_FillRect( railX + 22, 50, 6, 6, GFX_Color( color, gfxAccentColor, a ) );
    Frontend_DrawText( (int)railX + 38, 48, "Q3RALLY", UI_LEFT | UI_BIGFONT | UI_DROPSHADOW,
                       GFX_Color( color, gfxTextColor, a ) );
    GFX_DrawTimingSheet( railX, a, currentTime );
    GFX_DrawDriverCard( railX, a );

    /* hero: garage with the player's car */
    Frontend_DrawHeroSurface( (int)heroX, GFX_PANEL_Y, (int)heroWidth, GFX_PANEL_H, a );
    Frontend_DrawText( innerX, GFX_HEADER_Y, "Garage / active vehicle", UI_LEFT | UI_SMALLFONT,
                       GFX_Color( color, gfxMutedColor, a ) );
    chipLabel = s_gfxloading.goTime ? "Ready" : "Loading";
    Frontend_DrawStatusChip( innerRight - UI_FRONTEND_STATUS_DOT - UI_FRONTEND_SPACE_SM -
                             Frontend_TextWidth( chipLabel, UI_SMALLFONT ),
                             GFX_HEADER_Y, chipLabel,
                             s_gfxloading.goTime ? gfxAccentColor : gfxStatusColor, a );
    GFX_DrawCar( heroX, heroWidth, currentTime );

    GFX_DrawStageBlock( innerX, innerRight, a, currentTime );
    GFX_DrawTipOrUpdate( innerX, innerRight, a );
    GFX_DrawFooter( a, currentTime );

    Menu_Draw( &s_gfxloading.menu );

    fadeIn = (float)( currentTime - s_gfxloading.startTime ) / (float)GFX_FADE_IN_TIME;
    if ( fadeIn < 1.0f ) {
        if ( fadeIn < 0.0f ) {
            fadeIn = 0.0f;
        }
        UI_FillRect( GFX_ViewportLeft(), 0, SCREEN_WIDTH - 2.0f * GFX_ViewportLeft(), SCREEN_HEIGHT,
                     GFX_Color( color, gfxBlackColor, 1.0f - fadeIn ) );
    }

    s_gfxloading.stageShown = qtrue;
}

/* -------------------------------------------------------------------------
   Key / mouse handler

   Keys are never forwarded to Menu_DefaultKey: ESC there would pop the only
   menu on the stack, abort the remaining cache stages and skip the update
   notice.
   ------------------------------------------------------------------------- */

static sfxHandle_t UI_GFX_Loading_Key( int key ) {
    if ( GFX_UpdatePromptActive() ) {
        switch ( key ) {
        case K_ESCAPE:
        case K_MOUSE2:
        case K_PAD0_B:
            GFX_ActivateButton( GFX_BTN_SKIP );
            return menu_out_sound;

        case K_LEFTARROW:
        case K_KP_LEFTARROW:
        case K_PAD0_DPAD_LEFT:
        case K_PAD0_LEFTSTICK_LEFT:
            s_gfxloading.focusedBtn = GFX_BTN_UPDATE;
            return menu_move_sound;

        case K_RIGHTARROW:
        case K_KP_RIGHTARROW:
        case K_PAD0_DPAD_RIGHT:
        case K_PAD0_LEFTSTICK_RIGHT:
            s_gfxloading.focusedBtn = GFX_BTN_SKIP;
            return menu_move_sound;

        case K_TAB:
            s_gfxloading.focusedBtn = ( s_gfxloading.focusedBtn == GFX_BTN_UPDATE )
                                      ? GFX_BTN_SKIP : GFX_BTN_UPDATE;
            return menu_move_sound;

        case K_MOUSE1:
            /* only clicks on a button count - a stray click does not dismiss */
            if ( s_gfxloading.hoveredBtn == GFX_BTN_NONE ) {
                return 0;
            }
            GFX_ActivateButton( s_gfxloading.hoveredBtn );
            return menu_out_sound;

        case K_ENTER:
        case K_KP_ENTER:
        case K_JOY1:
        case K_PAD0_A:
            GFX_ActivateButton( s_gfxloading.focusedBtn == GFX_BTN_NONE
                                ? GFX_BTN_UPDATE : s_gfxloading.focusedBtn );
            return menu_out_sound;

        default:
            return 0;
        }
    }

    /* Once everything is cached, confirm / back skips the start lights. */
    if ( s_gfxloading.finalPhase ) {
        switch ( key ) {
        case K_ESCAPE:
        case K_ENTER:
        case K_KP_ENTER:
        case K_SPACE:
        case K_MOUSE1:
        case K_MOUSE2:
        case K_JOY1:
        case K_PAD0_A:
        case K_PAD0_B:
            s_gfxloading.skipRequested = qtrue;
            return 0;
        default:
            break;
        }
    }

    return 0;
}

/* -------------------------------------------------------------------------
   Public entry point
   ------------------------------------------------------------------------- */

void UI_GFX_Loading( void ) {
    memset( &s_gfxloading, 0, sizeof( s_gfxloading ) );

    s_gfxloading.menu.draw       = UI_GFX_Loading_MenuDraw;
    s_gfxloading.menu.key        = UI_GFX_Loading_Key;
    /* The draw function paints its own full-width background; fullscreen
     * only tells the engine that nothing of the game shows through. */
    s_gfxloading.menu.fullscreen = qtrue;

    uis.menusp = 0;
    UI_PushMenu( &s_gfxloading.menu );
    m_entersound = qfalse;

    s_gfxloading.startTime      = trap_Milliseconds();
    s_gfxloading.stageStartTime = s_gfxloading.startTime;
    s_gfxloading.hoveredBtn     = GFX_BTN_NONE;
    s_gfxloading.focusedBtn     = GFX_BTN_UPDATE;
    s_gfxloading.ackedWith      = GFX_BTN_NONE;
    s_gfxloading.lastCursorX    = uis.cursorx;
    s_gfxloading.lastCursorY    = uis.cursory;
    s_gfxloading.updateState    = GFX_ReadUpdateState();
    s_gfxloading.goSound        = trap_S_RegisterSound( GFX_GO_SOUND, qfalse );

    GFX_ReadDriver();
    GFX_PickTip();

    /* the main menu takes this music over instead of starting its own */
    UI_StartMenuMusic();
    s_gfxloading.musicStarted = qtrue;
}
