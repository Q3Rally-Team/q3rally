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

/* -------------------------------------------------------------------------
   Constants
   ------------------------------------------------------------------------- */

#define MIN_STAGE_TIME      450     /* minimum milliseconds per stage */
#define FINAL_DISPLAY_TIME  1200    /* time to display 100% before transition */

#define Q3RALLY_DOWNLOAD_URL "https://www.q3rally.com/download-test/"

/* Layout constants - all positions in 640x480 virtual screen space */
#define SCREEN_CX       320     /* horizontal center */

/* Header block */
#define HDR_TITLE_Y      36
#define HDR_VERSION_Y    72

/* Progress block */
#define BAR_X           160
#define BAR_Y           200
#define BAR_W           320
#define BAR_H            20
#define CAR_ICON_W       48
#define CAR_ICON_H       48
#define BAR_STAGE_Y     (BAR_Y - 22)        /* stage label above the bar */
#define BAR_PCT_Y       (BAR_Y + BAR_H + 10)/* percentage below the bar  */

/* Update notice block (below progress) */
#define UPD_BASE_Y      (BAR_PCT_Y + 30)

/* Tip block (near bottom) */
#define TIP_LABEL_Y     390
#define TIP_TEXT_Y      408

/* Separator line Y positions */
#define SEP_TOP_Y        90
#define SEP_BOT_Y       380

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
    qhandle_t       carShader;             /* icon for progress indicator */
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

/* -------------------------------------------------------------------------
   Helper: draw a thin horizontal separator line
   ------------------------------------------------------------------------- */

static void DrawSeparator(int y) {
    vec4_t sepColor;
    sepColor[0] = 0.4f; sepColor[1] = 0.4f; sepColor[2] = 0.4f; sepColor[3] = 0.6f;
    UI_FillRect(80, y, 480, 1, sepColor);
}

/* -------------------------------------------------------------------------
   Helper: draw a styled button, returns qtrue if mouse is inside.
   Hit test uses uis.cursorx / uis.cursory (standard Q3 UI cursor state).
   ------------------------------------------------------------------------- */

static qboolean DrawButton(int cx, int y, int w, int h,
                            const char *label, qboolean highlighted) {
    vec4_t bgColor, borderColor, textColor, shadow;
    int    x = cx - w / 2;

    if (highlighted) {
        bgColor[0]     = 0.85f; bgColor[1]     = 0.55f; bgColor[2]     = 0.0f;  bgColor[3]     = 1.0f;
        borderColor[0] = 1.0f;  borderColor[1] = 0.75f; borderColor[2] = 0.2f;  borderColor[3] = 1.0f;
        textColor[0]   = 1.0f;  textColor[1]   = 1.0f;  textColor[2]   = 1.0f;  textColor[3]   = 1.0f;
    } else {
        bgColor[0]     = 0.15f; bgColor[1]     = 0.15f; bgColor[2]     = 0.15f; bgColor[3]     = 0.85f;
        borderColor[0] = 0.5f;  borderColor[1] = 0.5f;  borderColor[2] = 0.5f;  borderColor[3] = 1.0f;
        textColor[0]   = 0.75f; textColor[1]   = 0.75f; textColor[2]   = 0.75f; textColor[3]   = 1.0f;
    }

    shadow[0] = 0.0f; shadow[1] = 0.0f; shadow[2] = 0.0f; shadow[3] = 0.5f;
    UI_FillRect(x + 3, y + 3, w, h, shadow);
    UI_FillRect(x, y, w, h, bgColor);
    UI_DrawRect(x, y, w, h, borderColor);

    UI_DrawString(cx, y + (h - SMALLCHAR_HEIGHT) / 2,
                  label, UI_CENTER | UI_SMALLFONT, textColor);

    return (uis.cursorx >= x && uis.cursorx <= x + w &&
            uis.cursory >= y && uis.cursory <= y + h) ? qtrue : qfalse;
}

/* -------------------------------------------------------------------------
   Stage execution
   ------------------------------------------------------------------------- */

static void UI_GFX_Loading_ExecuteStage(void) {
    int totalStages = ARRAY_LEN(stages);

    if (s_gfxloading.cacheExecuted) {
        return;
    }
    if (s_gfxloading.currentCache < totalStages && stages[s_gfxloading.currentCache].exec) {
        stages[s_gfxloading.currentCache].exec();
    }
    s_gfxloading.cacheExecuted = qtrue;
}

/* -------------------------------------------------------------------------
   Progress update (timing + smooth interpolation)
   ------------------------------------------------------------------------- */

static void UI_GFX_Loading_UpdateProgress(void) {
    int   currentTime  = trap_Milliseconds();
    int   stageElapsed = currentTime - s_gfxloading.stageStartTime;
    int   totalStages  = ARRAY_LEN(stages);
    float stageProgress;

    UI_GFX_Loading_ExecuteStage();

    if (stageElapsed >= MIN_STAGE_TIME && s_gfxloading.currentCache < totalStages) {
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
        stageProgress = (float)stageElapsed / (float)MIN_STAGE_TIME;
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

static void UI_GFX_Loading_MenuDraw(void) {
    int         totalStages = ARRAY_LEN(stages);
    int         stage_index;
    const char *stageName;
    int         textY;
    char        buf[256];
    char        updateState[32];
    int         currentTime;
    vec4_t      color;

    Menu_Draw(&s_gfxloading.menu);

    UI_GFX_Loading_UpdateProgress();

    s_gfxloading.smoothProgress +=
        (s_gfxloading.loadPercent - s_gfxloading.smoothProgress) * 0.08f;

    /* -----------------------------------------------------------------------
       HEADER
       ----------------------------------------------------------------------- */

    color[0] = 1.0f; color[1] = 0.65f; color[2] = 0.0f; color[3] = 1.0f;
    UI_DrawProportionalString(SCREEN_CX, HDR_TITLE_Y,
                              "Q3Rally", UI_CENTER | UI_BIGFONT, color);

    color[0] = 0.65f; color[1] = 0.65f; color[2] = 0.65f; color[3] = 1.0f;
    UI_DrawString(SCREEN_CX, HDR_VERSION_Y,
                  va("Version %s - Loading Resources", PRODUCT_VERSION),
                  UI_CENTER | UI_SMALLFONT, color);

    DrawSeparator(SEP_TOP_Y);

    /* -----------------------------------------------------------------------
       PROGRESS BAR
       ----------------------------------------------------------------------- */

    stage_index = s_gfxloading.currentCache;
    stageName   = stageNames[(stage_index < totalStages) ? stage_index : totalStages];
    color[0] = 0.9f; color[1] = 0.9f; color[2] = 0.9f; color[3] = 1.0f;
    UI_DrawString(SCREEN_CX, BAR_STAGE_Y, stageName, UI_CENTER | UI_SMALLFONT, color);

    /* shadow frame */
    color[0] = 0.0f; color[1] = 0.0f; color[2] = 0.0f; color[3] = 1.0f;
    UI_FillRect(BAR_X - 2, BAR_Y - 2, BAR_W + 4, BAR_H + 4, color);

    /* track */
    color[0] = 0.12f; color[1] = 0.12f; color[2] = 0.12f; color[3] = 1.0f;
    UI_FillRect(BAR_X, BAR_Y, BAR_W, BAR_H, color);

    /* fill - red to green */
    if (s_gfxloading.smoothProgress > 0.0f) {
        vec4_t fillColor;
        int    fillW = (int)(BAR_W * s_gfxloading.smoothProgress);

        fillColor[0] = 0.85f + (0.1f  - 0.85f) * s_gfxloading.smoothProgress;
        fillColor[1] = 0.15f + (0.85f - 0.15f) * s_gfxloading.smoothProgress;
        fillColor[2] = 0.05f + (0.2f  - 0.05f) * s_gfxloading.smoothProgress;
        fillColor[3] = 1.0f;

        UI_FillRect(BAR_X, BAR_Y, fillW, BAR_H, fillColor);
    }

    /* border */
    color[0] = 0.55f; color[1] = 0.55f; color[2] = 0.55f; color[3] = 1.0f;
    UI_DrawRect(BAR_X, BAR_Y, BAR_W, BAR_H, color);

    /* car icon */
    {
        float progress = s_gfxloading.smoothProgress;
        int   car_x;

        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        car_x = BAR_X + (int)(BAR_W * progress) - CAR_ICON_W / 2;
        UI_DrawHandlePic(car_x, BAR_Y - CAR_ICON_H,
                         CAR_ICON_W, CAR_ICON_H, s_gfxloading.carShader);
    }

    /* percentage */
    color[0] = 0.75f; color[1] = 0.75f; color[2] = 0.75f; color[3] = 1.0f;
    UI_DrawString(SCREEN_CX, BAR_PCT_Y,
                  va("%.0f%%", s_gfxloading.smoothProgress * 100.0f),
                  UI_CENTER | UI_SMALLFONT, color);

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

        color[0] = 1.0f; color[1] = 0.35f; color[2] = 0.1f; color[3] = 1.0f;
        UI_DrawString(SCREEN_CX, textY,
                      "A new version of Q3Rally is available!",
                      UI_CENTER | UI_SMALLFONT, color);
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

        color[0] = 0.85f; color[1] = 0.85f; color[2] = 0.2f; color[3] = 1.0f;
        UI_DrawString(SCREEN_CX, textY, buf, UI_CENTER | UI_SMALLFONT, color);
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
            color[0] = 0.5f; color[1] = 0.85f; color[2] = 0.5f; color[3] = 1.0f;
            UI_DrawString(SCREEN_CX, textY, "Update acknowledged - continuing...",
                          UI_CENTER | UI_SMALLFONT, color);
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

        color[0] = 0.7f; color[1] = 0.7f; color[2] = 0.3f; color[3] = 1.0f;
        UI_DrawString(SCREEN_CX, textY, errorMsg,
                      UI_CENTER | UI_SMALLFONT, color);
        textY += 24;

    } else {
        s_gfxloading.requireUpdateAck = qfalse;
        s_gfxloading.updateAcked      = qfalse;
    }

    /* -----------------------------------------------------------------------
       TIP
       ----------------------------------------------------------------------- */

    DrawSeparator(SEP_BOT_Y);

    color[0] = 0.55f; color[1] = 0.55f; color[2] = 0.55f; color[3] = 1.0f;
    UI_DrawString(SCREEN_CX, TIP_LABEL_Y, "TIP",
                  UI_CENTER | UI_SMALLFONT, color);

    color[0] = 0.85f; color[1] = 0.85f; color[2] = 0.85f; color[3] = 1.0f;
    UI_DrawString(SCREEN_CX, TIP_TEXT_Y,
                  loadingTips[s_gfxloading.tipIndex],
                  UI_CENTER | UI_SMALLFONT, color);

    /* -----------------------------------------------------------------------
       TRANSITION
       ----------------------------------------------------------------------- */

    if (s_gfxloading.finalPhase) {
        currentTime = trap_Milliseconds();
        if (currentTime - s_gfxloading.finalDisplayStartTime >= FINAL_DISPLAY_TIME &&
            s_gfxloading.smoothProgress >= 0.98f) {
            if (s_gfxloading.requireUpdateAck && !s_gfxloading.updateAcked) {
                return;
            }
            UI_PopMenu();
            UI_MainMenu();
        }
    }
}

/* -------------------------------------------------------------------------
   Key / mouse handler
   ------------------------------------------------------------------------- */

static sfxHandle_t UI_GFX_Loading_Key(int key) {
    if (s_gfxloading.requireUpdateAck && !s_gfxloading.updateAcked) {
        if (key == K_MOUSE1) {
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

    s_gfxloading.carShader = trap_R_RegisterShaderNoMip("menu/art/loading_car");

    s_gfxloading.menu.draw       = UI_GFX_Loading_MenuDraw;
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
