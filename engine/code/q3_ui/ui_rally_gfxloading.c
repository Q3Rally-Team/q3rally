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

/*
===========================================================================
Q3Rally Graphics Loading Screen - Enhanced Version

Handles step-by-step UI caching with visual feedback and artificial delay
for improved user experience and better visual feedback.
===========================================================================
*/

#include "ui_local.h"

static const int MIN_STAGE_TIME = 450;      /* minimum milliseconds per stage */
static const int FINAL_DISPLAY_TIME = 1200; /* time to display 100% before transition */

typedef struct {
    void (*exec)(void);
} gfx_stage_t;

typedef struct {
    menuframework_s menu;
    playerInfo_t playerinfo;     /* used for interpolation only */
    int currentCache;            /* current cache step */
    float loadPercent;           /* raw loading percent [0.0 - 1.0] */
    float smoothProgress;        /* smoothly interpolated value for visuals */
    int stageStartTime;          /* when current stage started */
    int finalDisplayStartTime;   /* when 100% display phase started */
    qhandle_t carShader;         /* icon for progress indicator */
    qboolean cacheExecuted;      /* whether current stage's cache has been executed */
    qboolean finalPhase;         /* whether we're in the final display phase */
} gfxloading_t;

static gfxloading_t s_gfxloading;

/*
======================
Stage Implementations
======================
*/
static void GFXStage_Init(void) {}
static void GFXStage_SetupMenu(void) { UI_SetupMenu_Cache(); }
static void GFXStage_PlayerModel(void) { PlayerModel_Cache(); }
static void GFXStage_PlayerSettings(void) { PlayerSettings_Cache(); }
static void GFXStage_Controls(void) { Controls_Cache(); }
static void GFXStage_ArenaServers(void) { ArenaServers_Cache(); }
static void GFXStage_PlayerData(void) {
    char model[MAX_QPATH];
    char rim[MAX_QPATH];
    char head[MAX_QPATH];
    char plate[MAX_QPATH];

    trap_Cvar_VariableStringBuffer("model", model, sizeof(model));
    trap_Cvar_VariableStringBuffer("rim", rim, sizeof(rim));
    trap_Cvar_VariableStringBuffer("head", head, sizeof(head));
    trap_Cvar_VariableStringBuffer("plate", plate, sizeof(plate));
    UI_PlayerInfo_SetModel(&s_gfxloading.playerinfo, model, rim, head, plate);
}
static void GFXStage_StartServer(void) { StartServer_Cache(); }

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
    {GFXStage_Init},
    {GFXStage_SetupMenu},
    {GFXStage_PlayerModel},
    {GFXStage_PlayerSettings},
    {GFXStage_Controls},
    {GFXStage_ArenaServers},
    {GFXStage_PlayerData},
    {GFXStage_StartServer},
};

/*
============================
UI_GFX_Loading_ExecuteStage
============================
Executes the actual caching operations for the current stage.
This is separated from the timing logic to allow for artificial delays.
*/
static void UI_GFX_Loading_ExecuteStage(void) {
    int totalStages = ARRAY_LEN(stages);

    if (s_gfxloading.cacheExecuted) {
        return; /* stage already executed */
    }

    if (s_gfxloading.currentCache < totalStages && stages[s_gfxloading.currentCache].exec) {
        stages[s_gfxloading.currentCache].exec();
    }

    s_gfxloading.cacheExecuted = qtrue;
}

/*
============================
UI_GFX_Loading_UpdateProgress
============================
Updates the loading progress based on timing and stage completion.
Handles artificial delays and smooth progress transitions.
*/
static void UI_GFX_Loading_UpdateProgress(void) {
    int currentTime = trap_Milliseconds();
    int stageElapsedTime;
    int totalStages = ARRAY_LEN(stages);
    float stageProgress;

    /* Execute the current stage's caching operations early */
    UI_GFX_Loading_ExecuteStage();

    stageElapsedTime = currentTime - s_gfxloading.stageStartTime;

    /* Check if minimum stage time has passed */
    if (stageElapsedTime >= MIN_STAGE_TIME && s_gfxloading.currentCache < totalStages) {
        /* Move to next stage */
        s_gfxloading.loadPercent = (float)(s_gfxloading.currentCache + 1) / (float)totalStages;
        s_gfxloading.currentCache++;
        s_gfxloading.stageStartTime = currentTime;
        s_gfxloading.cacheExecuted = qfalse;

        /* Check if we've reached 100% */
        if (s_gfxloading.loadPercent >= 1.0f) {
            s_gfxloading.loadPercent = 1.0f;
            s_gfxloading.finalPhase = qtrue;
            s_gfxloading.finalDisplayStartTime = currentTime;
        }
    } else if (s_gfxloading.currentCache < totalStages) {
        /* Calculate partial progress within current stage for smoother animation */
        stageProgress = (float)stageElapsedTime / (float)MIN_STAGE_TIME;
        if (stageProgress > 1.0f) stageProgress = 1.0f;

        /* Interpolate between current and next stage */
        s_gfxloading.loadPercent = ((float)s_gfxloading.currentCache + stageProgress) / (float)totalStages;
    }

    /* Ensure we don't exceed 100% */
    if (s_gfxloading.loadPercent > 1.0f) {
        s_gfxloading.loadPercent = 1.0f;
    }
}

/*
=======================
UI_GFX_Loading_MenuDraw
=======================
Draws the graphics loading menu and updates progress with enhanced visual feedback.
*/
static void UI_GFX_Loading_MenuDraw(void) {
    float blendRate;
    int bar_x, bar_y, bar_w, bar_h;
    int stage_index;
    int currentTime;
    int totalStages = ARRAY_LEN(stages);
    const char *stageName;

    /* Draw base menu (background, etc.) */
    Menu_Draw(&s_gfxloading.menu);

    /* Update loading progress with timing */
    UI_GFX_Loading_UpdateProgress();

    /* Smooth interpolation toward target percentage */
    blendRate = 0.08f;  /* lower = smoother, adjusted for better visual feel */
    s_gfxloading.smoothProgress += (s_gfxloading.loadPercent - s_gfxloading.smoothProgress) * blendRate;

    /* Draw loading header */
    UI_DrawProportionalString(320, 160, "Q3Rally - Loading Graphics", UI_CENTER | UI_BIGFONT, text_color_normal);

    /* Draw current stage message */
    stage_index = s_gfxloading.currentCache;
    if (stage_index >= totalStages) {
        stageName = stageNames[totalStages];
    } else {
        stageName = stageNames[stage_index];
    }
    UI_DrawString(320, 200, stageName, UI_CENTER | UI_SMALLFONT, text_color_highlight);

    /* Draw progress bar with enhanced visuals */
    bar_x = 200;
    bar_y = 240 + 50;
    bar_w = 240;
    bar_h = 24;

    /* Outer frame with shadow effect */
    UI_FillRect(bar_x - 3, bar_y - 3, bar_w + 6, bar_h + 6, colorBlack);
    UI_FillRect(bar_x - 2, bar_y - 2, bar_w + 4, bar_h + 4, menu_back_color);
    
    /* Progress bar background */
    UI_FillRect(bar_x, bar_y, bar_w, bar_h, colorDkGrey);
    
    /* Actual progress fill */
    if (s_gfxloading.smoothProgress > 0.0f) {
        UI_FillRect(bar_x, bar_y, (int)(bar_w * s_gfxloading.smoothProgress), bar_h, text_color_highlight);
    }

    /* Moving car icon */
    {
        const int iconWidth = 64;
        const int iconHeight = 64;
        float progress = s_gfxloading.smoothProgress;
        int car_x;

        /* Clamp progress to [0,1] to keep icon on bar */
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        car_x = bar_x + (int)(bar_w * progress) - iconWidth / 2;
        UI_DrawHandlePic(car_x, bar_y - iconHeight, iconWidth, iconHeight, s_gfxloading.carShader);
    }

    /* Progress bar border */
    UI_DrawRect(bar_x, bar_y, bar_w, bar_h, colorWhite);

    /* Draw percentage text with better formatting */
    UI_DrawString(320, bar_y + bar_h + 16, 
                  va("Loading Progress: %.1f%%", s_gfxloading.smoothProgress * 100.0f), 
                  UI_CENTER | UI_SMALLFONT, text_color_normal);

    /* Draw additional info during loading */
    if (s_gfxloading.smoothProgress < 1.0f) {
        UI_DrawString(320, bar_y + bar_h + 36, "Please wait...", 
                      UI_CENTER | UI_SMALLFONT, text_color_disabled);
    }

    /* Handle transition when loading is complete */
    if (s_gfxloading.finalPhase) {
        currentTime = trap_Milliseconds();
        if (currentTime - s_gfxloading.finalDisplayStartTime >= FINAL_DISPLAY_TIME && 
            s_gfxloading.smoothProgress >= 0.98f) {
            UI_PopMenu();
            UI_MainMenu();
        }
    }
}

/*
===============
UI_GFX_Loading
===============
Initializes and shows the enhanced graphics loading screen.
*/
void UI_GFX_Loading(void) {
    /* Clear all fields */
    memset(&s_gfxloading, 0, sizeof(gfxloading_t));

    /* Load progress icon */
    s_gfxloading.carShader = trap_R_RegisterShaderNoMip("menu/art/loading_car");

    /* Setup menu structure */
    s_gfxloading.menu.draw = UI_GFX_Loading_MenuDraw;
    s_gfxloading.menu.fullscreen = qtrue;

    /* Reset menu stack */
    uis.menusp = 0;

    /* Push menu and disable enter sound */
    UI_PushMenu(&s_gfxloading.menu);
    m_entersound = qfalse;

    /* Initialize loading state */
    s_gfxloading.currentCache = 0;
    s_gfxloading.loadPercent = 0.0f;
    s_gfxloading.smoothProgress = 0.0f;
    s_gfxloading.stageStartTime = trap_Milliseconds();
    s_gfxloading.cacheExecuted = qfalse;
    s_gfxloading.finalPhase = qfalse;
    s_gfxloading.finalDisplayStartTime = 0;
}

