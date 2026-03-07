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
Q3Rally Credits Screen

Scrolling credits combining Q3Rally team and original iD Software credits.
- Scrolls at constant speed from bottom to top
- Fades in at top and out at bottom of screen
- Music volume is raised to 0.5 if below, then restored on exit
- Pressing any key exits and quits the game (end-of-game screen)
===========================================================================
*/

#include "ui_local.h"

/* -------------------------------------------------------------------------
   Constants
   ------------------------------------------------------------------------- */

#define SCROLL_SPEED        2.80f   /* pixels per 100ms (uis.realtime units) */
#define FADE_ZONE_HEIGHT    60      /* px over which alpha fades at top/bottom edges */

#define BACKGROUND_SHADER   /* use background art; comment out for solid fill */
#define BACKGROUND_DIM      0.45f   /* 0.0 = invisible, 1.0 = original brightness */

/* -------------------------------------------------------------------------
   Colours
   ------------------------------------------------------------------------- */

#ifndef BACKGROUND_SHADER
static vec4_t color_background = { 0.00f, 0.00f, 0.00f, 1.00f };
#endif
static vec4_t color_header     = { 1.00f, 0.55f, 0.10f, 1.00f }; /* Q3Rally orange */
static vec4_t color_name       = { 0.90f, 0.90f, 0.90f, 1.00f }; /* soft white      */
static vec4_t color_id_header  = { 0.75f, 0.75f, 0.75f, 1.00f }; /* grey for iD section */
static vec4_t color_id_name    = { 0.65f, 0.65f, 0.65f, 1.00f };
static vec4_t color_title      = { 1.00f, 0.65f, 0.00f, 1.00f }; /* bright orange title */

/* -------------------------------------------------------------------------
   Credit line structure
   ------------------------------------------------------------------------- */

typedef struct {
    const char *string;     /* NULL marks end of array; "" is a spacer line */
    int         style;
    vec4_t     *colour;
} cr_line_t;

/* -------------------------------------------------------------------------
   Credits data
   ------------------------------------------------------------------------- */

static cr_line_t credits[] = {

    /* ----- Title ----- */
    { "Thank you for playing!", UI_CENTER|UI_BIGFONT|UI_DROPSHADOW,  &color_title  },
    { "",                       UI_CENTER|UI_SMALLFONT,              &color_name   },
    { "Q3Rally Team",           UI_CENTER|UI_BIGFONT|UI_DROPSHADOW,  &color_title  },
    { "",                       UI_CENTER|UI_SMALLFONT,              &color_name   },
    { "",                       UI_CENTER|UI_SMALLFONT,              &color_name   },

    /* ----- Programming ----- */
    { "Programming:",                     UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_header },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "Steven 'Stonelance' Heijsters",    UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Per 'P3rlE' Thormann",             UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Eddy Valdez aka. 'TheBigBuu'",     UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "ZTurtleMan from TurtleArena",      UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Eraser from EntityPlus",           UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },

    /* ----- Mapping ----- */
    { "Mapping:",                         UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_header },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "Jeff 'Stecki' Garstecki",          UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Jim 'gout' Bahe",                  UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Simon 'System Krash' Batty",       UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Jonathan 'Amphetamine' Garrod",    UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Michael 'Cyberdemon' Kaminsky",    UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Per 'P3rlE' Thormann",             UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "'MysteriousPoetd' aka 'Poet'",     UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "'OliverV'",                        UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Thomas aka. 'To-mos'",             UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Eddy Valdez aka. 'thebigbuu'",     UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "'ailmanki' aka. 'peyote'",         UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Denis 'insellium' Manylov",        UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },

    /* ----- Art and Models ----- */
    { "Art and Models:",                  UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_header },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "Jeff 'Stecki' Garstecki",          UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Per 'P3rlE' Thormann",             UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Denis 'insellium' Manylov",        UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "'Steel Painter'",                  UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Thomas aka. 'To-mos'",             UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },

    /* ----- Sound and Music ----- */
    { "Sound Design and Music:",          UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_header },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "P.Andersson",                      UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "F.Segerfalk",                      UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Dale Wilson",                      UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Thomas aka. 'To-mos'",             UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },

    /* ----- Texture Design ----- */
    { "Texture Design:",                  UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_header },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "Melanie aka. 'Toxicity'",          UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Eddy Valdez aka. 'thebigbuu'",     UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Denis 'insellium' Manylov",        UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },

    /* ----- Document Design ----- */
    { "Document Design and Layout:",      UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_header },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "Richard Smith",                    UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Per 'P3rlE' Thormann",             UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Mirco 'PaniC' Herrmann",           UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },

    /* ----- Beta Testing ----- */
    { "BETA Testing:",                    UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_header },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "To-Mos",                           UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Eddy Valdez aka. 'thebigbuu'",     UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Per 'P3rlE' Thormann",             UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Mirco 'PaniC' Herrmann",           UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Jeremiah aka. 'BETAMONKEY'",       UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "'Onai'",                           UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "Denis 'insellium' Manylov",        UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },

    /* ----- Special Thanks ----- */
    { "Special Thanks:",                  UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_header },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "Cyberdemon, Killaz and skw|d",     UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },

    /* ----- Footer ----- */
    { PRODUCT_VERSION " | 2002 - 2026 | www.q3rally.com",
                                          UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_header },
    { "It's damn fast baby!",             UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_title  },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name   },

    /* ----- iD Software ----- */
    { "Based on Quake III Arena",         UI_CENTER|UI_BIGFONT|UI_DROPSHADOW,   &color_id_header },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name      },
    { "John Carmack, John Cash",          UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_id_name   },
    { "Adrian Carmack, Kevin Cloud,",     UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_id_name   },
    { "Paul Steed, Kenneth Scott",        UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_id_name   },
    { "Graeme Devine, Tim Willits,",      UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_id_name   },
    { "Christian Antkow, Paul Jaquays",   UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_id_name   },
    { "Todd Hollenshead",                 UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, &color_id_name   },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name      },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name      },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name      },
    { "",                                 UI_CENTER|UI_SMALLFONT,               &color_name      },

    { NULL } /* end marker */
};

/* -------------------------------------------------------------------------
   State
   ------------------------------------------------------------------------- */

typedef struct {
    menuframework_s menu;
    int             startTime;   /* uis.realtime when credits began          */
    float           mvolume;     /* original music volume, restored on exit  */
    qhandle_t       background;  /* background shader handle (optional)      */
    int             totalHeight; /* pre-calculated total pixel height        */
} creditsmenu_t;

static creditsmenu_t s_credits;

/* -------------------------------------------------------------------------
   Helper: calculate total pixel height of all credit lines
   ------------------------------------------------------------------------- */

static int Credits_CalcTotalHeight( void ) {
    int n, h = 0;

    for ( n = 0; credits[n].string != NULL; n++ ) {
        if ( credits[n].style & UI_BIGFONT )
            h += PROP_HEIGHT;
        else if ( credits[n].style & UI_GIANTFONT )
            h += (int)( PROP_HEIGHT * ( 1.0f / PROP_SMALL_SIZE_SCALE ) );
        else
            h += (int)( PROP_HEIGHT * PROP_SMALL_SIZE_SCALE );
    }
    return h;
}

/* -------------------------------------------------------------------------
   Helper: returns an alpha [0,1] for a given screen y (top-left of line),
   based on the center of the line for smoother transitions.
   ------------------------------------------------------------------------- */

static float Credits_EdgeAlpha( int lineTop, int lineH ) {
    int center = lineTop + lineH / 2;

    if ( center < 0 || center > 480 )
        return 0.0f;
    if ( center < FADE_ZONE_HEIGHT )
        return (float)center / (float)FADE_ZONE_HEIGHT;
    if ( center > 480 - FADE_ZONE_HEIGHT )
        return (float)( 480 - center ) / (float)FADE_ZONE_HEIGHT;
    return 1.0f;
}

/* -------------------------------------------------------------------------
   Exit helper - restores music volume and quits
   ------------------------------------------------------------------------- */

static void Credits_Exit( void ) {
    trap_Cmd_ExecuteText( EXEC_APPEND,
                          va( "s_musicvolume %f; quit\n", s_credits.mvolume ) );
}

/* -------------------------------------------------------------------------
   Key handler
   ------------------------------------------------------------------------- */

static sfxHandle_t UI_CreditMenu_Key( int key ) {
    if ( key & K_CHAR_FLAG )
        return 0;

    Credits_Exit();
    return 0;
}

/* -------------------------------------------------------------------------
   Draw function
   ------------------------------------------------------------------------- */

static void UI_CreditMenu_Draw( void ) {
    int     n, y, scrollY;
    vec4_t  drawColor;
    vec4_t  dimColor = { 0.0f, 0.0f, 0.0f, 1.0f - BACKGROUND_DIM };

    /* --- background: always drawn, even after text is gone --- */
#ifdef BACKGROUND_SHADER
    UI_DrawHandlePic( -uis.bias, 0, SCREEN_WIDTH + uis.bias * 2, SCREEN_HEIGHT,
                      s_credits.background );
    /* darken the shader to the desired brightness */
    UI_FillRect( -uis.bias, 0, SCREEN_WIDTH + uis.bias * 2, SCREEN_HEIGHT, dimColor );
#else
    UI_FillRect( -uis.bias, 0, SCREEN_WIDTH + uis.bias * 2, SCREEN_HEIGHT,
                 color_background );
#endif

    /* --- scroll position: start of first line --- */
    scrollY = 480 - (int)( SCROLL_SPEED * (float)( uis.realtime - s_credits.startTime ) / 100.0f );

    /* --- draw lines --- */
    y = scrollY;
    for ( n = 0; credits[n].string != NULL; n++ ) {
        int lineTop, lineH;

        /* determine line height */
        if ( credits[n].style & UI_BIGFONT )
            lineH = PROP_HEIGHT;
        else if ( credits[n].style & UI_GIANTFONT )
            lineH = (int)( PROP_HEIGHT * ( 1.0f / PROP_SMALL_SIZE_SCALE ) );
        else
            lineH = (int)( PROP_HEIGHT * PROP_SMALL_SIZE_SCALE );

        lineTop = y;
        y += lineH;

        /* skip lines fully outside the visible screen area */
        if ( lineTop > 480 ) continue;
        if ( y < 0 )         continue;

        /* draw non-empty lines */
        if ( credits[n].string[0] != '\0' ) {
            float alpha = Credits_EdgeAlpha( lineTop, lineH );

            Vector4Copy( *credits[n].colour, drawColor );
            drawColor[3] *= alpha;

            UI_DrawProportionalString( 320, lineTop, credits[n].string,
                                       credits[n].style, drawColor );
        }
    }

    /* --- end of credits: last line has scrolled off the top --- */
    if ( y < 0 ) {
        Credits_Exit();
    }
}

/* -------------------------------------------------------------------------
   Public entry point
   ------------------------------------------------------------------------- */

void UI_CreditMenu( void ) {
    memset( &s_credits, 0, sizeof( s_credits ) );

    s_credits.menu.draw       = UI_CreditMenu_Draw;
    s_credits.menu.key        = UI_CreditMenu_Key;
    s_credits.menu.fullscreen = qtrue;

    s_credits.startTime   = uis.realtime;
    s_credits.totalHeight = Credits_CalcTotalHeight();

    s_credits.mvolume = trap_Cvar_VariableValue( "s_musicvolume" );
    if ( s_credits.mvolume < 0.5f )
        trap_Cmd_ExecuteText( EXEC_APPEND, "s_musicvolume 0.5\n" );
    trap_Cmd_ExecuteText( EXEC_APPEND, "music music/credits\n" );

#ifdef BACKGROUND_SHADER
    s_credits.background = trap_R_RegisterShaderNoMip( "menu/art/menu_back" );
#endif

    uis.transitionIn = 0;
    UI_PushMenu( &s_credits.menu );
}
