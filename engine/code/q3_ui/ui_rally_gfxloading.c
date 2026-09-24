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
Q3Rally Graphics Loading Screen - Enhanced Version

Handles step-by-step UI caching with visual feedback and artificial delay
for improved user experience and better visual feedback.

Update dialog: integrated into loading screen with "Update Now" / "Skip" buttons.
"Update Now" opens the Q3Rally download page in the system browser.
===========================================================================
*/

#include "ui_local.h"
#include "ui_rally_theme.h"
#include "ui_rally_frontend.h"

/* -------------------------------------------------------------------------
   Constants
   ------------------------------------------------------------------------- */

#define MIN_STAGE_TIME      450     /* minimum milliseconds per stage */
#define FINAL_DISPLAY_TIME  1200    /* time to display 100% before transition */
#define MAX_DEBUG_PAUSE     15000   /* safety clamp for test-only loading pauses */

#define NUM_STAGES          8       /* must match ARRAY_LEN(stages) */
#define SMOOTH_LERP_SPEED   4.5f    /* units per second for framerate-independent smoothing */

#define Q3RALLY_DOWNLOAD_URL "https://www.q3rally.com/download-test/"

/* Layout constants - all positions in 640x480 virtual screen space */
#define GFX_RAIL_Y             32
#define GFX_RAIL_W            210
#define GFX_RAIL_H            410
#define GFX_CONTENT_Y          32
#define GFX_CONTENT_H         410
#define GFX_CONTENT_INSET      16
#define GFX_PROGRESS_Y        176
#define GFX_PROGRESS_H         14
#define GFX_PROGRESS_SEG_Y    198
#define GFX_PROGRESS_SEG_H      5
#define GFX_STATUS_Y          226
#define GFX_UPDATE_Y          250
#define GFX_TIP_SEPARATOR_Y   324
#define GFX_TIP_LABEL_Y       340
#define GFX_TIP_TEXT_Y        358

/* Header block */
#define HDR_TITLE_Y      36
#define HDR_VERSION_Y    68

/* Progress block */
#define PANEL_X          82
#define PANEL_Y         126
#define PANEL_W         476
#define PANEL_H         160
#define BAR_X           118
#define BAR_Y           210
#define BAR_W           404
#define BAR_H            14
#define BAR_STAGE_Y     172                 /* stage label above the bar */
#define BAR_PCT_Y       236                 /* percentage below the bar  */
#define BAR_SEG_Y       255
#define BAR_SEG_H         5

/* Update notice block (below progress) */
#define UPD_BASE_Y      304

/* Tip block (near bottom) */
#define TIP_LABEL_Y     390
#define TIP_TEXT_Y      408

/* Separator line Y positions */
#define SEP_TOP_Y        90
#define SEP_BOT_Y       374

/* -------------------------------------------------------------------------
   Update-dialog button state
   ------------------------------------------------------------------------- */

typedef enum {
    UPD_BTN_NONE = -1,
    UPD_BTN_NOW  =  0,
    UPD_BTN_SKIP =  1
} updBtn_t;

/* -------------------------------------------------------------------------
   Main state struct - declared early so stage functions can reference it
   ------------------------------------------------------------------------- */

typedef struct {
    menuframework_s menu;
    playerInfo_t    playerinfo;            /* used for interpolation only */
    int             currentCache;          /* current cache step */
    float           loadPercent;           /* raw loading percent [0.0 - 1.0] */
    float           smoothProgress;        /* smoothly interpolated value for visuals */
    int             stageStartTime;        /* when current stage started */
    int             finalDisplayStartTime; /* when 100% display phase started */
    int             tipIndex;              /* index into loading tips */
    qboolean        cacheExecuted;         /* whether current stage's cache has been executed */
    qboolean        finalPhase;            /* whether we're in the final display phase */

    /* update notice */
    qboolean        requireUpdateAck;      /* remote version newer than local */
    qboolean        updateAcked;           /* player confirmed the update notice */
    updBtn_t        hoveredBtn;            /* which button the mouse is over */
} gfxloading_t;

static gfxloading_t s_gfxloading;

/* -------------------------------------------------------------------------
   Stage definitions - placed after s_gfxloading so GFXStage_PlayerData
   can reference it without a forward declaration.
   ------------------------------------------------------------------------- */

typedef struct {
    void (*exec)(void);
} gfx_stage_t;

static void GFXStage_Init(void)           {}
static void GFXStage_SetupMenu(void)      { UI_SetupMenu_Cache(); }
static void GFXStage_PlayerModel(void)    { PlayerModel_Cache(); }
static void GFXStage_PlayerSettings(void) { PlayerSettings_Cache(); }
static void GFXStage_Controls(void)       { Controls_Cache(); }
static void GFXStage_ArenaServers(void)   { ArenaServers_Cache(); }
static void GFXStage_PlayerData(void) {
    char model[MAX_QPATH];
    char rim[MAX_QPATH];
    char head[MAX_QPATH];
    char plate[MAX_QPATH];

    trap_Cvar_VariableStringBuffer("model", model, sizeof(model));
    trap_Cvar_VariableStringBuffer("rim",   rim,   sizeof(rim));
    trap_Cvar_VariableStringBuffer("head",  head,  sizeof(head));
    trap_Cvar_VariableStringBuffer("plate", plate, sizeof(plate));
    UI_PlayerInfo_SetModel(&s_gfxloading.playerinfo, model, rim, head, plate);
}
static void GFXStage_StartServer(void)    { StartServer_Cache(); }

static const char * const stageNames[] = {
    "Initializing System...",
    "Loading Setup Menu...",
    "Caching Player Models...",
    "Applying Player Settings...",
    "Loading Control Bindings...",
    "Building Arena Server List...",
    "Configuring Player Data...",
    "Finalizing User Interface...",
    "Ready to Start!"
};

static const gfx_stage_t stages[] = {
    { GFXStage_Init           },
    { GFXStage_SetupMenu      },
    { GFXStage_PlayerModel    },
    { GFXStage_PlayerSettings },
    { GFXStage_Controls       },
    { GFXStage_ArenaServers   },
    { GFXStage_PlayerData     },
    { GFXStage_StartServer    },
};

/* -------------------------------------------------------------------------
   Loading tips  (23 total: 5 original + 18 new)
   ------------------------------------------------------------------------- */

static const char *loadingTips[] = {
    /* --- original 5 --- */
    "Use the handbrake to drift through tight corners.",
    "Keep your speed up when hitting jumps.",
    "Ramming opponents can knock them off the track.",
    "Collect power-ups to gain an edge on rivals.",
    "Watch for shortcuts to shave off lap times.",

    /* --- driving technique --- */
    "Tap the brakes before a corner, not during - you'll carry more speed.",
    "Handbrake turns work best at medium speed - too fast and you'll spin out.",
    "Countersteering after a drift keeps your car pointed the right way.",
    "Drafting behind opponents reduces drag - slingshot past them on straights.",
    "Land jumps with a flat car to avoid losing control on impact.",

    /* --- tactics & racing --- */
    "The inside line isn't always fastest - sometimes the outside gives better exit speed.",
    "Ramming from the side is more effective than from behind.",
    "Save your power-ups for the last lap - that's when they matter most.",
    "Watch the minimap: knowing where rivals are is half the battle.",
    "Block the racing line on the final straight to deny an overtake.",

    /* --- tracks & shortcuts --- */
    "Every track has at least one shortcut - explore before you race.",
    "Wet surfaces reduce grip earlier than you'd expect - brake sooner.",
    "Jumps are faster if you hit the ramp dead center.",
    "Cutting corners too aggressively can launch you off the track entirely.",

    /* --- general / fun --- */
    "First place isn't safe until you cross the finish line.",
    "A well-placed ram can knock two opponents off course at once.",
    "Sometimes slowing down slightly lets you set up a much faster corner exit.",
    "Practice a track in free roam before racing - knowing the layout pays off.",
};


static vec4_t gfxSeparatorColor     = UI_FRONTEND_COLOR_BORDER;
static vec4_t gfxHeaderColor        = UI_FRONTEND_COLOR_TEXT;
static vec4_t gfxBodyTextColor      = UI_FRONTEND_COLOR_TEXT;
static vec4_t gfxMutedTextColor     = UI_FRONTEND_COLOR_MUTED;
static vec4_t gfxAccentColor        = UI_FRONTEND_COLOR_ACCENT;
static vec4_t gfxSuccessColor       = UI_FRONTEND_COLOR_ACCENT;
static vec4_t gfxErrorColor         = { 0.96f, 0.30f, 0.16f, 1.00f };
static vec4_t gfxWarningColor       = UI_FRONTEND_COLOR_STATUS;
static vec4_t gfxProgressTrackColor = UI_FRONTEND_COLOR_PROGRESS;
static vec4_t gfxBackdropColor          = UI_FRONTEND_COLOR_SCRIM;
static vec4_t gfxPanelColor             = UI_FRONTEND_COLOR_PANEL;
static vec4_t gfxHeroOverlayColor       = UI_FRONTEND_COLOR_HERO_OVERLAY;

static float GFX_ViewportLeft( void ) {
    if ( uis.xscale <= 0.0f ) {
        return 0.0f;
    }
    return -uis.bias / uis.xscale;
}

static float GFX_ViewportRight( void ) {
    return SCREEN_WIDTH - GFX_ViewportLeft();
}

static float GFX_RailX( void ) {
    return GFX_ViewportLeft() + 24.0f;
}

static float GFX_ContentX( void ) {
    return GFX_RailX() + 210.0f + 16.0f;
}

static float GFX_ContentRight( void ) {
    return GFX_ViewportRight() - 24.0f;
}

static float GFX_ContentWidth( void ) {
    return GFX_ContentRight() - GFX_ContentX();
}

/* -------------------------------------------------------------------------
   Helper: draw a thin horizontal separator line
   ------------------------------------------------------------------------- */

static void DrawSeparator(float x, float width, int y) {
    UI_FillRect(x, y, width, 1, gfxSeparatorColor);
}

static void DrawProgressSegments(int x, int y, int width, float progress) {
    int segments = 18;
    int gap = 3;
    int usableW = width - (segments - 1) * gap;
    int i;
    int segW;
    float threshold;
    vec4_t color;

    for (i = 0; i < segments; i++) {
        segW = usableW / segments;
        if (i < usableW % segments) {
            segW++;
        }
        threshold = (float)(i + 1) / (float)segments;
        if (progress >= threshold) {
            color[0] = gfxAccentColor[0] + (gfxSuccessColor[0] - gfxAccentColor[0]) * threshold;
            color[1] = gfxAccentColor[1] + (gfxSuccessColor[1] - gfxAccentColor[1]) * threshold;
            color[2] = gfxAccentColor[2] + (gfxSuccessColor[2] - gfxAccentColor[2]) * threshold;
            color[3] = 0.95f;
        } else {
            Vector4Copy(gfxProgressTrackColor, color);
            color[3] = 0.58f;
        }
        UI_FillRect(x, y, segW, GFX_PROGRESS_SEG_H, color);
        x += segW + gap;
    }
}

/* -------------------------------------------------------------------------
   Stage execution
   ------------------------------------------------------------------------- */

static void UI_GFX_Loading_ExecuteStage(void) {
    int totalStages = NUM_STAGES;

    if (s_gfxloading.cacheExecuted) {
        return;
    }
    if (s_gfxloading.currentCache < totalStages && stages[s_gfxloading.currentCache].exec) {
        stages[s_gfxloading.currentCache].exec();
    }
    s_gfxloading.cacheExecuted = qtrue;
}

static int UI_GFX_Loading_GetPause( const char *cvarName, int fallback ) {
    int value = (int)trap_Cvar_VariableValue(cvarName);

    if (value <= 0) {
        return fallback;
    }
    if (value > MAX_DEBUG_PAUSE) {
        return MAX_DEBUG_PAUSE;
    }
    return value;
}

/* -------------------------------------------------------------------------
   Progress update (timing + smooth interpolation)
   ------------------------------------------------------------------------- */

static void UI_GFX_Loading_UpdateProgress(void) {
    int   currentTime  = trap_Milliseconds();
    int   stageElapsed = currentTime - s_gfxloading.stageStartTime;
    int   stageTime    = UI_GFX_Loading_GetPause("ui_gfxLoadingStagePause", MIN_STAGE_TIME);
    int   totalStages  = NUM_STAGES;
    float stageProgress;

    UI_GFX_Loading_ExecuteStage();

    if (stageElapsed >= stageTime && s_gfxloading.currentCache < totalStages) {
        s_gfxloading.loadPercent = (float)(s_gfxloading.currentCache + 1) / (float)totalStages;
        s_gfxloading.currentCache++;
        s_gfxloading.stageStartTime = currentTime;
        s_gfxloading.cacheExecuted  = qfalse;

        if (s_gfxloading.loadPercent >= 1.0f) {
            s_gfxloading.loadPercent           = 1.0f;
            s_gfxloading.finalPhase            = qtrue;
            s_gfxloading.finalDisplayStartTime = currentTime;
        }
    } else if (s_gfxloading.currentCache < totalStages) {
        stageProgress = (float)stageElapsed / (float)stageTime;
        if (stageProgress > 1.0f) stageProgress = 1.0f;
        s_gfxloading.loadPercent =
            ((float)s_gfxloading.currentCache + stageProgress) / (float)totalStages;
    }

    if (s_gfxloading.loadPercent > 1.0f) {
        s_gfxloading.loadPercent = 1.0f;
    }
}

/* -------------------------------------------------------------------------
   Main draw function
   ------------------------------------------------------------------------- */

#if 0
static void UI_GFX_Loading_MenuDraw_Legacy(void) {
    int         totalStages = NUM_STAGES;
    int         stage_index;
    const char *stageName;
    int         textY;
    char        buf[256];
    char        updateState[32];
    int         currentTime;
    float       deltaTime;
    vec4_t      color;

    static int  lastDrawTime = 0;

    Menu_Draw(&s_gfxloading.menu);
    gfxBackdropColor[3] = 1.0f;
    UI_FillRect(-uis.bias, 0, SCREEN_WIDTH + uis.bias * 2, SCREEN_HEIGHT, gfxBackdropColor);

    currentTime = trap_Milliseconds();
    deltaTime   = (lastDrawTime > 0)
                  ? (float)(currentTime - lastDrawTime) * 0.001f
                  : 0.016f;                             /* assume 60 fps on first frame */
    if (deltaTime > 0.1f) deltaTime = 0.1f;            /* clamp: avoid huge jump after pause */
    lastDrawTime = currentTime;

    UI_GFX_Loading_UpdateProgress();

    s_gfxloading.smoothProgress +=
        (s_gfxloading.loadPercent - s_gfxloading.smoothProgress)
        * (SMOOTH_LERP_SPEED * deltaTime);
    if (s_gfxloading.loadPercent >= 1.0f && s_gfxloading.smoothProgress > 0.995f) {
        s_gfxloading.smoothProgress = 1.0f;
    }

    /* -----------------------------------------------------------------------
       HEADER
       ----------------------------------------------------------------------- */

    Vector4Copy(gfxHeaderColor, color);
    UI_DrawProportionalString(SCREEN_CX, HDR_TITLE_Y,
                              "Q3Rally", UI_CENTER | UI_THEME_STYLE_TITLE_FONT, color);

    Vector4Copy(gfxSecondaryTextColor, color);
    UI_DrawString(SCREEN_CX, HDR_VERSION_Y,
                  va("Version %s  /  Loading Resources", PRODUCT_VERSION),
                  UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);

    DrawSeparator(SEP_TOP_Y);
    DrawPanel(PANEL_X, PANEL_Y, PANEL_W, PANEL_H);

    /* -----------------------------------------------------------------------
       PROGRESS BAR
       ----------------------------------------------------------------------- */

    stage_index = s_gfxloading.currentCache;
    stageName   = stageNames[(stage_index < totalStages) ? stage_index : totalStages];

    Vector4Copy(gfxMutedTextColor, color);
    UI_DrawString(SCREEN_CX, PANEL_Y + 8, "RESOURCE CACHE",
                  UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);

    Vector4Copy(gfxBodyTextColor, color);
    UI_DrawString(SCREEN_CX, BAR_STAGE_Y, stageName, UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);

    /* shadow frame */
    color[0] = 0.0f; color[1] = 0.0f; color[2] = 0.0f; color[3] = 0.75f;
    UI_FillRect(BAR_X - 2, BAR_Y - 2, BAR_W + 4, BAR_H + 4, color);

    /* track */
    Vector4Copy(gfxProgressTrackColor, color);
    UI_FillRect(BAR_X, BAR_Y, BAR_W, BAR_H, color);

    /* fill - red to green */
    if (s_gfxloading.smoothProgress > 0.0f) {
        vec4_t fillColor;
        int    fillW = (int)(BAR_W * s_gfxloading.smoothProgress);

        fillColor[0] = gfxAccentColor[0] + (gfxSuccessColor[0] - gfxAccentColor[0]) * s_gfxloading.smoothProgress;
        fillColor[1] = gfxAccentColor[1] + (gfxSuccessColor[1] - gfxAccentColor[1]) * s_gfxloading.smoothProgress;
        fillColor[2] = gfxAccentColor[2] + (gfxSuccessColor[2] - gfxAccentColor[2]) * s_gfxloading.smoothProgress;
        fillColor[3] = 1.0f;

        UI_FillRect(BAR_X, BAR_Y, fillW, BAR_H, fillColor);
        UI_FillRect(BAR_X, BAR_Y, fillW, 2, gfxProgressGlowColor);
    }

    /* border */
    Vector4Copy(gfxSeparatorColor, color);
    UI_DrawRect(BAR_X, BAR_Y, BAR_W, BAR_H, color);

    DrawProgressSegments(s_gfxloading.smoothProgress);

    /* percentage */
    Vector4Copy(gfxMutedTextColor, color);
    UI_DrawString(SCREEN_CX, BAR_PCT_Y,
                  va("%.0f%%", s_gfxloading.smoothProgress * 100.0f),
                  UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);

    /* -----------------------------------------------------------------------
       UPDATE NOTICE
       ----------------------------------------------------------------------- */

    textY = UPD_BASE_Y;

    trap_Cvar_VariableStringBuffer("cl_updateState", updateState, sizeof(updateState));

    if (!Q_stricmp(updateState, "outdated")) {
        char remoteVersion[64];
        char remoteDate[64];

        if (!s_gfxloading.requireUpdateAck) {
            s_gfxloading.requireUpdateAck = qtrue;
            s_gfxloading.updateAcked      = qfalse;
            s_gfxloading.hoveredBtn       = UPD_BTN_NONE;
        }

        trap_Cvar_VariableStringBuffer("cl_updateRemote", remoteVersion, sizeof(remoteVersion));
        trap_Cvar_VariableStringBuffer("cl_updateDate",   remoteDate,   sizeof(remoteDate));

        Vector4Copy(gfxErrorColor, color);
        UI_DrawString(SCREEN_CX, textY,
                      "A new version of Q3Rally is available!",
                      UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);
        textY += 22;

        if (remoteVersion[0]) {
            if (remoteDate[0]) {
                Com_sprintf(buf, sizeof(buf),
                            "Installed: %s     Latest: %s  (%s)",
                            PRODUCT_VERSION, remoteVersion, remoteDate);
            } else {
                Com_sprintf(buf, sizeof(buf),
                            "Installed: %s     Latest: %s",
                            PRODUCT_VERSION, remoteVersion);
            }
        } else {
            Com_sprintf(buf, sizeof(buf),
                        "Installed: %s     Latest: unknown", PRODUCT_VERSION);
        }

        Vector4Copy(gfxWarningColor, color);
        UI_DrawString(SCREEN_CX, textY, buf, UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);
        textY += 28;

        if (!s_gfxloading.updateAcked) {
            qboolean hoverNow  = DrawButton(SCREEN_CX - 70, textY, 120, 28,
                                             "Update Now", qtrue);
            qboolean hoverSkip = DrawButton(SCREEN_CX + 70, textY,  80, 28,
                                             "Skip",       qfalse);

            if (hoverNow)       s_gfxloading.hoveredBtn = UPD_BTN_NOW;
            else if (hoverSkip) s_gfxloading.hoveredBtn = UPD_BTN_SKIP;
            else                s_gfxloading.hoveredBtn = UPD_BTN_NONE;

            textY += 40;
        } else {
            Vector4Copy(gfxSuccessColor, color);
            UI_DrawString(SCREEN_CX, textY, "Update acknowledged - continuing...",
                          UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);
            textY += 24;
        }

    } else if (!Q_stricmp(updateState, "current")) {
        char remoteVersion[64];

        s_gfxloading.requireUpdateAck = qfalse;
        s_gfxloading.updateAcked      = qfalse;

        trap_Cvar_VariableStringBuffer("cl_updateRemote", remoteVersion, sizeof(remoteVersion));

        Vector4Copy(gfxSuccessColor, color);
        UI_DrawString(SCREEN_CX, textY,
                      "Q3Rally is up to date.",
                      UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);
        textY += 22;

        if (remoteVersion[0]) {
            Com_sprintf(buf, sizeof(buf),
                        "Installed: %s     Latest: %s",
                        PRODUCT_VERSION, remoteVersion);
            Vector4Copy(gfxBodyTextColor, color);
            UI_DrawString(SCREEN_CX, textY, buf, UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);
            textY += 24;
        }

    } else if (!Q_stricmp(updateState, "offline") ||
               !Q_stricmp(updateState, "failed")) {
        char errorMsg[128];

        s_gfxloading.requireUpdateAck = qfalse;
        s_gfxloading.updateAcked      = qfalse;

        trap_Cvar_VariableStringBuffer("cl_updateError", errorMsg, sizeof(errorMsg));
        if (!errorMsg[0]) {
            if (!Q_stricmp(updateState, "offline")) {
                Q_strncpyz(errorMsg,
                           "Update server offline - version check unavailable",
                           sizeof(errorMsg));
            } else {
                Q_strncpyz(errorMsg, "Unable to check for updates", sizeof(errorMsg));
            }
        }

        Vector4Copy(gfxWarningColor, color);
        UI_DrawString(SCREEN_CX, textY, errorMsg,
                      UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);
        textY += 24;

    } else {
        s_gfxloading.requireUpdateAck = qfalse;
        s_gfxloading.updateAcked      = qfalse;
    }

    /* -----------------------------------------------------------------------
       TIP
       ----------------------------------------------------------------------- */

    DrawSeparator(SEP_BOT_Y);

    Vector4Copy(gfxSeparatorColor, color);
    UI_DrawString(SCREEN_CX, TIP_LABEL_Y, "TIP",
                  UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);

    Vector4Copy(gfxBodyTextColor, color);
    UI_DrawString(SCREEN_CX, TIP_TEXT_Y,
                  loadingTips[s_gfxloading.tipIndex],
                  UI_CENTER | UI_THEME_STYLE_BODY_FONT, color);

    /* -----------------------------------------------------------------------
       TRANSITION
       ----------------------------------------------------------------------- */

    if (s_gfxloading.finalPhase) {
        int finalDisplayTime = UI_GFX_Loading_GetPause("ui_gfxLoadingFinalPause", FINAL_DISPLAY_TIME);
        currentTime = trap_Milliseconds();
        if (currentTime - s_gfxloading.finalDisplayStartTime >= finalDisplayTime &&
            s_gfxloading.smoothProgress >= 0.98f) {
            if (s_gfxloading.requireUpdateAck && !s_gfxloading.updateAcked) {
                return;
            }
            UI_PopMenu();
            UI_MainMenu();
        }
    }
}

#endif

static void UI_GFX_Loading_MenuDraw(void) {
    int         totalStages = NUM_STAGES;
    int         stage_index;
    int         stageNumber;
    int         progressX;
    int         progressW;
    int         updateX;
    int         updateW;
    int         currentTime;
    int         textY;
    const char *stageName;
    float       deltaTime;
    float       railX;
    float       contentX;
    float       contentRight;
    float       contentWidth;
    char        buf[256];
    char        updateState[32];
    vec4_t      color;
    static int  lastDrawTime = 0;

    railX         = GFX_RailX();
    contentX      = GFX_ContentX();
    contentRight  = GFX_ContentRight();
    contentWidth  = GFX_ContentWidth();
    progressX     = (int)contentX + GFX_CONTENT_INSET;
    progressW     = (int)contentWidth - GFX_CONTENT_INSET * 2;
    updateX       = progressX;
    updateW       = progressW;

    currentTime = trap_Milliseconds();
    deltaTime   = (lastDrawTime > 0)
                  ? (float)(currentTime - lastDrawTime) * 0.001f
                  : 0.016f;
    if (deltaTime > 0.1f) deltaTime = 0.1f;
    lastDrawTime = currentTime;

    UI_GFX_Loading_UpdateProgress();
    s_gfxloading.smoothProgress +=
        (s_gfxloading.loadPercent - s_gfxloading.smoothProgress)
        * (SMOOTH_LERP_SPEED * deltaTime);
    if (s_gfxloading.loadPercent >= 1.0f && s_gfxloading.smoothProgress > 0.995f) {
        s_gfxloading.smoothProgress = 1.0f;
    }

    stage_index = s_gfxloading.currentCache;
    stageNumber = (stage_index < totalStages) ? stage_index + 1 : totalStages;
    stageName = stageNames[(stage_index < totalStages) ? stage_index : totalStages];

    /* Use the same full-width background, rail, and hero-panel language as
     * the main menu. */
    Vector4Copy(gfxBackdropColor, color);
    Frontend_DrawBackground(color);

    Frontend_DrawPanel((int)railX, GFX_RAIL_Y, 210, GFX_RAIL_H,
                       1.0f, UI_FRONTEND_STYLE_SURFACE);

    UI_FillRect(contentX, GFX_CONTENT_Y, contentWidth, GFX_CONTENT_H, gfxPanelColor);
    UI_SetColor(NULL);
    UI_DrawHandlePic(contentX, GFX_CONTENT_Y, contentWidth, GFX_CONTENT_H,
                     Frontend_BackgroundShader());
    UI_FillRect(contentX, GFX_CONTENT_Y, contentWidth, GFX_CONTENT_H, gfxHeroOverlayColor);
    Frontend_DrawPanel((int)contentX, GFX_CONTENT_Y, (int)contentWidth,
                       GFX_CONTENT_H, 1.0f, UI_FRONTEND_STYLE_FRAME);

    /* Brand and status rail. */
    UI_FillRect(railX + 22, 50, 6, 6, gfxAccentColor);
    Frontend_DrawText((int)railX + 38, 48, "Q3RALLY",
                      UI_LEFT | UI_BIGFONT | UI_DROPSHADOW, gfxHeaderColor);
    Frontend_DrawText((int)railX + 22, 94, "System boot",
                      UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);

    Frontend_DrawCard((int)railX + 14, 112, 182, 76, 1.0f, qfalse);
    Frontend_DrawText((int)railX + 28, 124, "Startup sequence",
                      UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);
    Frontend_DrawText((int)railX + 28, 145,
                      va("Stage %02d / %02d", stageNumber, totalStages),
                      UI_LEFT | UI_SMALLFONT, gfxBodyTextColor);
    Frontend_DrawText((int)railX + 28, 165,
                      va("%.0f%% ready", s_gfxloading.smoothProgress * 100.0f),
                      UI_LEFT | UI_SMALLFONT, gfxAccentColor);

    Frontend_DrawCard((int)railX + 14, 348, 182, 64, 1.0f, qfalse);
    UI_FillRect(railX + 28, 363, 6, 6,
                s_gfxloading.smoothProgress >= 1.0f ? gfxAccentColor : gfxWarningColor);
    Frontend_DrawText((int)railX + 44, 358, "Frontend",
                      UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);
    Frontend_DrawText((int)railX + 44, 376,
                      s_gfxloading.smoothProgress >= 1.0f ? "System ready" : "Boot sequence",
                      UI_LEFT | UI_SMALLFONT, gfxBodyTextColor);
    Frontend_DrawText((int)railX + 44, 394, "Q3Rally  -  2002-2026",
                      UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);

    /* Workspace header and progress. */
    Frontend_DrawText((int)contentX + GFX_CONTENT_INSET, 52,
                      "System / GFX loading",
                      UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);
    Frontend_DrawText((int)contentRight - GFX_CONTENT_INSET, 52,
                      s_gfxloading.smoothProgress >= 1.0f ? "Ready" : "Loading",
                      UI_RIGHT | UI_SMALLFONT, gfxAccentColor);
    UI_FillRect(contentRight - 10, 48, 6, 6, gfxAccentColor);

    Frontend_DrawText(progressX, 100, "Resource cache",
                      UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);
    Frontend_DrawText(progressX, 122, stageName,
                      UI_LEFT | UI_SMALLFONT | UI_DROPSHADOW, gfxBodyTextColor);
    Frontend_DrawText((int)contentRight - GFX_CONTENT_INSET, 122,
                      va("%02d / %02d", stageNumber, totalStages),
                      UI_RIGHT | UI_SMALLFONT, gfxMutedTextColor);
    Frontend_DrawText(progressX, GFX_STATUS_Y, "Cache progress",
                      UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);
    Frontend_DrawText((int)contentRight - GFX_CONTENT_INSET, GFX_STATUS_Y,
                      va("%.0f%%", s_gfxloading.smoothProgress * 100.0f),
                      UI_RIGHT | UI_SMALLFONT, gfxAccentColor);

    Frontend_DrawProgress(progressX, GFX_PROGRESS_Y, progressW, GFX_PROGRESS_H,
                          s_gfxloading.smoothProgress, 1.0f);
    DrawProgressSegments(progressX, GFX_PROGRESS_SEG_Y, progressW, s_gfxloading.smoothProgress);

    /* Compact update status card. */
    textY = GFX_UPDATE_Y;
    trap_Cvar_VariableStringBuffer("cl_updateState", updateState, sizeof(updateState));

    if (!Q_stricmp(updateState, "outdated")) {
        char remoteVersion[64];
        char remoteDate[64];

        if (!s_gfxloading.requireUpdateAck) {
            s_gfxloading.requireUpdateAck = qtrue;
            s_gfxloading.updateAcked      = qfalse;
            s_gfxloading.hoveredBtn       = UPD_BTN_NONE;
        }

        trap_Cvar_VariableStringBuffer("cl_updateRemote", remoteVersion, sizeof(remoteVersion));
        trap_Cvar_VariableStringBuffer("cl_updateDate", remoteDate, sizeof(remoteDate));
        Frontend_DrawCard(updateX, textY - 10, updateW, 78, 1.0f, qfalse);
        UI_FillRect(updateX, textY - 10, 3, 78, gfxErrorColor);
        Frontend_DrawText(updateX + 14, textY, "Update available",
                          UI_LEFT | UI_SMALLFONT, gfxErrorColor);

        if (remoteVersion[0]) {
            if (remoteDate[0]) {
                Com_sprintf(buf, sizeof(buf), "Installed %s  /  latest %s (%s)",
                            PRODUCT_VERSION, remoteVersion, remoteDate);
            } else {
                Com_sprintf(buf, sizeof(buf), "Installed %s  /  latest %s",
                            PRODUCT_VERSION, remoteVersion);
            }
        } else {
            Com_sprintf(buf, sizeof(buf), "Installed %s  /  latest unknown", PRODUCT_VERSION);
        }
        Frontend_DrawText(updateX + 14, textY + 18, buf,
                          UI_LEFT | UI_SMALLFONT, gfxWarningColor);

        if (!s_gfxloading.updateAcked) {
            qboolean hoverNow = Frontend_DrawButton(updateX + updateW - 185,
                                                     textY + 43, 102, 24,
                                                     "Update", 1.0f, qfalse,
                                                     UI_FRONTEND_TEXT_CENTER);
            qboolean hoverSkip = Frontend_DrawButton(updateX + updateW - 80,
                                                      textY + 43, 64, 24,
                                                      "Skip", 1.0f, qfalse,
                                                      UI_FRONTEND_TEXT_CENTER);
            if (hoverNow)       s_gfxloading.hoveredBtn = UPD_BTN_NOW;
            else if (hoverSkip) s_gfxloading.hoveredBtn = UPD_BTN_SKIP;
            else                s_gfxloading.hoveredBtn = UPD_BTN_NONE;
        } else {
            Frontend_DrawText(updateX + 14, textY + 43, "Update acknowledged / continuing",
                              UI_LEFT | UI_SMALLFONT, gfxSuccessColor);
        }
    } else if (!Q_stricmp(updateState, "current")) {
        char remoteVersion[64];

        s_gfxloading.requireUpdateAck = qfalse;
        s_gfxloading.updateAcked      = qfalse;
        trap_Cvar_VariableStringBuffer("cl_updateRemote", remoteVersion, sizeof(remoteVersion));
        Frontend_DrawCard(updateX, textY - 10, updateW, 46, 1.0f, qfalse);
        UI_FillRect(updateX, textY - 10, 3, 46, gfxAccentColor);
        Frontend_DrawText(updateX + 14, textY, "System check / up to date",
                          UI_LEFT | UI_SMALLFONT, gfxSuccessColor);
        Com_sprintf(buf, sizeof(buf), "BUILD %s%s%s", PRODUCT_VERSION,
                    remoteVersion[0] ? "  /  LATEST " : "", remoteVersion);
        Frontend_DrawText(updateX + 14, textY + 18, buf,
                          UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);
    } else if (!Q_stricmp(updateState, "offline") ||
               !Q_stricmp(updateState, "failed")) {
        char errorMsg[128];

        s_gfxloading.requireUpdateAck = qfalse;
        s_gfxloading.updateAcked      = qfalse;
        trap_Cvar_VariableStringBuffer("cl_updateError", errorMsg, sizeof(errorMsg));
        if (!errorMsg[0]) {
            Q_strncpyz(errorMsg,
                       !Q_stricmp(updateState, "offline") ? "UPDATE SERVICE OFFLINE" : "UPDATE CHECK FAILED",
                       sizeof(errorMsg));
        }
        Frontend_DrawCard(updateX, textY - 10, updateW, 46, 1.0f, qfalse);
        UI_FillRect(updateX, textY - 10, 3, 46, gfxWarningColor);
        Frontend_DrawText(updateX + 14, textY, errorMsg,
                          UI_LEFT | UI_SMALLFONT, gfxWarningColor);
        Frontend_DrawText(updateX + 14, textY + 18, "Continuing without update data",
                          UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);
    } else {
        s_gfxloading.requireUpdateAck = qfalse;
        s_gfxloading.updateAcked      = qfalse;
        Frontend_DrawCard(updateX, textY - 10, updateW, 46, 1.0f, qfalse);
        UI_FillRect(updateX, textY - 10, 3, 46, gfxMutedTextColor);
        Frontend_DrawText(updateX + 14, textY, "Update check",
                          UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);
        Frontend_DrawText(updateX + 14, textY + 18, "Waiting for version status",
                          UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);
    }

    DrawSeparator(contentX + GFX_CONTENT_INSET, progressW, GFX_TIP_SEPARATOR_Y);
    Frontend_DrawText(progressX, GFX_TIP_LABEL_Y, "Drive tip",
                      UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);
    Frontend_DrawText(progressX, GFX_TIP_TEXT_Y,
                      loadingTips[s_gfxloading.tipIndex],
                      UI_LEFT | UI_SMALLFONT, gfxBodyTextColor);

    Frontend_DrawText(progressX, 458, "Q3Rally  -  system initialization",
                      UI_LEFT | UI_SMALLFONT, gfxMutedTextColor);
    Frontend_DrawText((int)contentRight - GFX_CONTENT_INSET, 458,
                      va("BUILD %s", PRODUCT_VERSION),
                      UI_RIGHT | UI_SMALLFONT, gfxMutedTextColor);

    if (s_gfxloading.finalPhase) {
        int finalDisplayTime = UI_GFX_Loading_GetPause("ui_gfxLoadingFinalPause", FINAL_DISPLAY_TIME);

        if (currentTime - s_gfxloading.finalDisplayStartTime >= finalDisplayTime &&
            s_gfxloading.smoothProgress >= 0.98f) {
            if (s_gfxloading.requireUpdateAck && !s_gfxloading.updateAcked) {
                return;
            }
            UI_PopMenu();
            UI_MainMenu();
        }
    }

    Menu_Draw(&s_gfxloading.menu);
}

/* -------------------------------------------------------------------------
   Key / mouse handler
   ------------------------------------------------------------------------- */

static sfxHandle_t UI_GFX_Loading_Key(int key) {
    if (s_gfxloading.requireUpdateAck && !s_gfxloading.updateAcked) {
        if (key == K_ESCAPE || key == K_MOUSE2 || key == K_PAD0_B) {
            s_gfxloading.updateAcked = qtrue;
            trap_S_StartLocalSound(menu_out_sound, CHAN_LOCAL_SOUND);
            return menu_out_sound;
        }

        if (key == K_MOUSE1 || key == K_ENTER || key == K_KP_ENTER || key == K_JOY1 || key == K_PAD0_A) {
            if (s_gfxloading.hoveredBtn == UPD_BTN_NONE) {
                s_gfxloading.hoveredBtn = UPD_BTN_SKIP;
            }

            switch (s_gfxloading.hoveredBtn) {
                case UPD_BTN_NOW:
                    trap_Cmd_ExecuteText(EXEC_APPEND,
                                        "openURL \"" Q3RALLY_DOWNLOAD_URL "\"\n");
                    /* fall through - also ack so the game continues */
                case UPD_BTN_SKIP:
                    s_gfxloading.updateAcked = qtrue;
                    trap_S_StartLocalSound(menu_out_sound, CHAN_LOCAL_SOUND);
                    return menu_out_sound;
                default:
                    return 0;
            }
        }
        return 0;
    }

    return Menu_DefaultKey(&s_gfxloading.menu, key);
}

/* -------------------------------------------------------------------------
   Public entry point
   ------------------------------------------------------------------------- */

void UI_GFX_Loading(void) {
    memset(&s_gfxloading, 0, sizeof(gfxloading_t));

    s_gfxloading.menu.draw       = UI_GFX_Loading_MenuDraw;
    /*
     * Draw our own full-width theme background in UI_GFX_Loading_MenuDraw.
     * Keep fullscreen background disabled here so UI_DrawMenu won't paint
     * menuBackShader underneath.
     */
    s_gfxloading.menu.fullscreen = qtrue;
    s_gfxloading.menu.key        = UI_GFX_Loading_Key;

    uis.menusp = 0;

    UI_PushMenu(&s_gfxloading.menu);
    m_entersound = qfalse;

    s_gfxloading.currentCache          = 0;
    s_gfxloading.loadPercent           = 0.0f;
    s_gfxloading.smoothProgress        = 0.0f;
    s_gfxloading.stageStartTime        = trap_Milliseconds();
    s_gfxloading.cacheExecuted         = qfalse;
    s_gfxloading.finalPhase            = qfalse;
    s_gfxloading.finalDisplayStartTime = 0;
    s_gfxloading.tipIndex              = UI_RandomInt(ARRAY_LEN(loadingTips));
    s_gfxloading.requireUpdateAck      = qfalse;
    s_gfxloading.updateAcked           = qfalse;
    s_gfxloading.hoveredBtn            = UPD_BTN_NONE;
}
