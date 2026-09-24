/*
===========================================================================
Copyright (C) 2002-2026 Q3Rally Team
===========================================================================
*/

#ifndef UI_RALLY_THEME_H
#define UI_RALLY_THEME_H

/* Shared Q3Rally UI theme for wizard/loading style surfaces and text. */

#define UI_THEME_COLOR_PANEL_BG         { 0.07f, 0.08f, 0.12f, 0.98f }
#define UI_THEME_COLOR_PANEL_DIM        { 0.00f, 0.00f, 0.00f, 0.55f }
#define UI_THEME_COLOR_PANEL_BORDER     { 0.33f, 0.46f, 0.75f, 0.60f }
#define UI_THEME_COLOR_PANEL_SUBBG      { 0.10f, 0.12f, 0.18f, 1.00f }

#define UI_THEME_COLOR_TEXT_TITLE       { 0.76f, 0.86f, 1.00f, 1.00f }
#define UI_THEME_COLOR_TEXT_BODY        { 0.82f, 0.86f, 0.94f, 1.00f }
#define UI_THEME_COLOR_TEXT_HINT        { 0.60f, 0.66f, 0.77f, 1.00f }
#define UI_THEME_COLOR_TEXT_MUTED       { 0.52f, 0.58f, 0.68f, 1.00f }

#define UI_THEME_COLOR_ACCENT           { 0.50f, 0.70f, 1.00f, 1.00f }
#define UI_THEME_COLOR_SUCCESS          { 0.45f, 0.88f, 0.60f, 1.00f }
#define UI_THEME_COLOR_ERROR            { 1.00f, 0.45f, 0.45f, 1.00f }
#define UI_THEME_COLOR_WARNING          { 0.90f, 0.78f, 0.33f, 1.00f }

#define UI_THEME_COLOR_PROGRESS_TRACK   { 0.12f, 0.14f, 0.20f, 1.00f }
#define UI_THEME_COLOR_PROGRESS_START   { 0.26f, 0.50f, 0.96f, 1.00f }
#define UI_THEME_COLOR_PROGRESS_END     { 0.45f, 0.88f, 0.60f, 1.00f }

#define UI_THEME_COLOR_BUTTON_BG        { 0.14f, 0.18f, 0.26f, 0.95f }
#define UI_THEME_COLOR_BUTTON_BORDER    { 0.34f, 0.50f, 0.84f, 1.00f }
#define UI_THEME_COLOR_BUTTON_TEXT      { 0.85f, 0.90f, 0.98f, 1.00f }

#define UI_THEME_COLOR_BUTTON_HOVER_BG      { 0.23f, 0.36f, 0.60f, 1.00f }
#define UI_THEME_COLOR_BUTTON_HOVER_BORDER  { 0.55f, 0.76f, 1.00f, 1.00f }
#define UI_THEME_COLOR_BUTTON_HOVER_TEXT    { 1.00f, 1.00f, 1.00f, 1.00f }

#define UI_THEME_STYLE_TITLE_FONT       UI_SMALLFONT
#define UI_THEME_STYLE_BODY_FONT        UI_SMALLFONT
#define UI_THEME_STYLE_HINT_FONT        UI_SMALLFONT
#define UI_THEME_STYLE_BUTTON_FONT      UI_SMALLFONT

/* Main-menu frontend palette. Keep it separate from the blue wizard palette. */
#define UI_FRONTEND_COLOR_SCRIM        { 0.01f, 0.015f, 0.02f, 0.72f }
#define UI_FRONTEND_COLOR_PANEL        { 0.025f, 0.032f, 0.040f, 0.94f }
#define UI_FRONTEND_COLOR_PANEL_ALT    { 0.055f, 0.065f, 0.072f, 0.88f }
#define UI_FRONTEND_COLOR_HERO_OVERLAY { 0.025f, 0.032f, 0.040f, 0.60f }
#define UI_FRONTEND_COLOR_BORDER       { 0.18f, 0.24f, 0.23f, 0.36f }
#define UI_FRONTEND_COLOR_ACCENT       { 0.72f, 1.00f, 0.06f, 1.00f }
#define UI_FRONTEND_COLOR_TEXT         { 0.92f, 0.96f, 0.95f, 1.00f }
#define UI_FRONTEND_COLOR_MUTED        { 0.44f, 0.51f, 0.51f, 1.00f }
#define UI_FRONTEND_COLOR_FOCUS_BG     { 0.10f, 0.18f, 0.12f, 0.72f }
#define UI_FRONTEND_COLOR_STATUS       { 0.96f, 0.42f, 0.12f, 1.00f }
#define UI_FRONTEND_COLOR_SHADOW       { 0.00f, 0.00f, 0.00f, 0.18f }
#define UI_FRONTEND_COLOR_PROGRESS     { 0.11f, 0.14f, 0.15f, 0.82f }

/* Shared layout tokens. The UI still renders in the 640x480 virtual space. */
#define UI_FRONTEND_SPACE_XS           4
#define UI_FRONTEND_SPACE_SM           8
#define UI_FRONTEND_SPACE_MD           16
#define UI_FRONTEND_SPACE_LG           24
#define UI_FRONTEND_SPACE_XL           32
#define UI_FRONTEND_PANEL_EDGE         1
#define UI_FRONTEND_PANEL_TOPBAR       1
#define UI_FRONTEND_PANEL_SHADOW       0
#define UI_FRONTEND_BUTTON_HEIGHT      24
#define UI_FRONTEND_STATUS_DOT         6
#define UI_FRONTEND_RADIUS             0 /* legacy renderer: use quiet edge styles */

/* Component styles used by ui_rally_frontend.c. */
#define UI_FRONTEND_STYLE_SURFACE      0
#define UI_FRONTEND_STYLE_CARD         1
#define UI_FRONTEND_STYLE_ACTIVE       2
#define UI_FRONTEND_STYLE_FRAME        3

#define UI_FRONTEND_TEXT_LEFT          UI_LEFT
#define UI_FRONTEND_TEXT_CENTER        UI_CENTER

#endif
