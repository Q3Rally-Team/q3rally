/*
=============================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2021 Q3Rally Team (Per Thormann - q3rally@gmail.com)

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
=============================================================================
*/
//
// ui_ladder.c
//

#include "ui_local.h"

#define LADDER_FRAME                            "menu/art/cut_frame"

static const vec4_t ladderBackdropColor = { 0.f, 0.f, 0.f, 0.6f };

#define ID_MODE                                 100
#define ID_TIMEFRAME                            101
#define ID_REGION                               102
#define ID_RESULTS                              103
#define ID_REFRESH                              104
#define ID_BACK                                 105

#define LADDER_LIST_WIDTH                       80
#define LADDER_LIST_HEIGHT                      12
#define LADDER_STATUS_TEXTLEN                   128

#define LADDER_MODE_COUNT                       5
#define LADDER_TIMEFRAME_COUNT                  4
#define LADDER_REGION_COUNT                     5

#define LADDER_DISPLAY_COUNT                    32

static const char *ladderModeLabels[LADDER_MODE_COUNT + 1] = {
        "All Modes",
        "Racing",
        "Team Racing",
        "Derby",
        "Deathmatch",
        NULL
};

static const char *ladderModeKeys[LADDER_MODE_COUNT] = {
        "all",
        "racing",
        "team_racing",
        "derby",
        "deathmatch"
};

static const char *ladderTimeframeLabels[LADDER_TIMEFRAME_COUNT + 1] = {
        "All Time",
        "Monthly",
        "Weekly",
        "Daily",
        NULL
};

static const char *ladderTimeframeKeys[LADDER_TIMEFRAME_COUNT] = {
        "all_time",
        "monthly",
        "weekly",
        "daily"
};

static const char *ladderRegionLabels[LADDER_REGION_COUNT + 1] = {
        "Global",
        "North America",
        "Europe",
        "South America",
        "Asia-Pacific",
        NULL
};

static const char *ladderRegionKeys[LADDER_REGION_COUNT] = {
        "global",
        "na",
        "eu",
        "sa",
        "apac"
};

typedef struct {
        menuframework_s         menu;

        menubitmap_s            frame;
        menutext_s              title;
        menutext_s              columns;

        menulist_s              modeFilter;
        menulist_s              timeframeFilter;
        menulist_s              regionFilter;
        menulist_s              results;

        menutext_s              status;
        menutext_s              refresh;
        menutext_s              back;

        char                    statusText[LADDER_STATUS_TEXTLEN];
        char                    resultBuffer[LADDER_DISPLAY_COUNT][LADDER_LIST_WIDTH];
        const char              *resultItems[LADDER_DISPLAY_COUNT + 1];

        uiLadderStatus_t        snapshot;
} ladderMenu_t;

static ladderMenu_t        s_ladderMenu;

static const char *LadderMenu_CurrentModeKey( void ) {
        int index;

        index = s_ladderMenu.modeFilter.curvalue;
        if ( index < 0 || index >= LADDER_MODE_COUNT ) {
                index = 0;
        }
        return ladderModeKeys[index];
}

static const char *LadderMenu_CurrentTimeframeKey( void ) {
        int index;

        index = s_ladderMenu.timeframeFilter.curvalue;
        if ( index < 0 || index >= LADDER_TIMEFRAME_COUNT ) {
                index = 0;
        }
        return ladderTimeframeKeys[index];
}

static const char *LadderMenu_CurrentRegionKey( void ) {
        int index;

        index = s_ladderMenu.regionFilter.curvalue;
        if ( index < 0 || index >= LADDER_REGION_COUNT ) {
                index = 0;
        }
        return ladderRegionKeys[index];
}

static void LadderMenu_ClearResults( void ) {
        int i;

        s_ladderMenu.results.numitems = 0;
        s_ladderMenu.results.curvalue = 0;
        s_ladderMenu.results.top = 0;
        for ( i = 0; i < LADDER_DISPLAY_COUNT + 1; i++ ) {
                if ( i < LADDER_DISPLAY_COUNT ) {
                        s_ladderMenu.resultBuffer[i][0] = '\0';
                        s_ladderMenu.resultItems[i] = s_ladderMenu.resultBuffer[i];
                } else {
                        s_ladderMenu.resultItems[i] = NULL;
                }
        }
}

static void LadderMenu_SetStatusText( const char *text, const float *color ) {
        Q_strncpyz( s_ladderMenu.statusText, text, sizeof( s_ladderMenu.statusText ) );
        s_ladderMenu.status.color = (float*)color;
}

static void LadderMenu_BuildResults( const uiLadderStatus_t *status ) {
        int                     i;
        int                     displayCount;

        displayCount = status->entryCount;
        if ( displayCount > LADDER_DISPLAY_COUNT ) {
                displayCount = LADDER_DISPLAY_COUNT;
        }

        s_ladderMenu.results.numitems = displayCount;
        for ( i = 0; i < displayCount; i++ ) {
                const uiLadderEntry_t *entry;
                int                     rank;
                const char              *mode;
                const char              *region;
                const char              *metric;
                char                    playerColumn[64];

                entry = &status->entries[i];
                rank = entry->rank > 0 ? entry->rank : (i + 1);

                if ( entry->player[0] ) {
                        Q_strncpyz( playerColumn, entry->player, sizeof( playerColumn ) );
                } else {
                        Q_strncpyz( playerColumn, "Unknown", sizeof( playerColumn ) );
                }

                if ( entry->vehicle[0] ) {
                        Q_strcat( playerColumn, sizeof( playerColumn ), " (" );
                        Q_strcat( playerColumn, sizeof( playerColumn ), entry->vehicle );
                        Q_strcat( playerColumn, sizeof( playerColumn ), ")" );
                }

                mode = entry->mode[0] ? entry->mode : "-";
                region = entry->region[0] ? entry->region : "-";
                metric = entry->metric[0] ? entry->metric : "-";

                Com_sprintf( s_ladderMenu.resultBuffer[i], sizeof( s_ladderMenu.resultBuffer[i] ),
                        "%2d %-26.26s %-12.12s %-12.12s %-16.16s",
                        rank, playerColumn, mode, region, metric );
        }

        s_ladderMenu.resultItems[displayCount] = NULL;
}

static void LadderMenu_RequestData( void ) {
        trap_CancelLadderRequest();
        LadderMenu_ClearResults();
        LadderMenu_SetStatusText( "Loading ladder data...", text_color_normal );
        trap_RequestLadderData( LadderMenu_CurrentModeKey(),
                LadderMenu_CurrentTimeframeKey(),
                LadderMenu_CurrentRegionKey() );
}

static void LadderMenu_UpdateFromBackend( void ) {
        uiLadderStatus_t        status;
        int                     total;
        int                     shown;

        memset( &status, 0, sizeof( status ) );
        trap_GetLadderStatus( &status );

        s_ladderMenu.snapshot = status;

        switch ( status.status ) {
        case UI_LADDER_STATUS_LOADING:
                LadderMenu_SetStatusText( "Loading ladder data...", text_color_normal );
                break;

        case UI_LADDER_STATUS_ERROR:
                if ( status.errorMessage[0] ) {
                        Com_sprintf( s_ladderMenu.statusText, sizeof( s_ladderMenu.statusText ),
                                "Error: %s", status.errorMessage );
                } else {
                        Q_strncpyz( s_ladderMenu.statusText, "Ladder error.", sizeof( s_ladderMenu.statusText ) );
                }
                s_ladderMenu.status.color = colorRed;
                LadderMenu_ClearResults();
                break;

        case UI_LADDER_STATUS_EMPTY:
                LadderMenu_ClearResults();
                LadderMenu_SetStatusText( "No ladder data available.", text_color_normal );
                break;

        case UI_LADDER_STATUS_READY:
        default:
                LadderMenu_BuildResults( &status );
                total = status.entryCount;
                shown = s_ladderMenu.results.numitems;

                if ( shown <= 0 ) {
                        LadderMenu_SetStatusText( "No ladder results for current filters.", text_color_normal );
                } else if ( shown < total ) {
                        Com_sprintf( s_ladderMenu.statusText, sizeof( s_ladderMenu.statusText ),
                                "Showing %d of %d results", shown, total );
                        s_ladderMenu.status.color = text_color_normal;
                } else {
                        Com_sprintf( s_ladderMenu.statusText, sizeof( s_ladderMenu.statusText ),
                                "Showing %d result%s", shown, (shown == 1) ? "" : "s" );
                        s_ladderMenu.status.color = text_color_normal;
                }
                break;
        }
}

static void LadderMenu_MenuEvent( void *ptr, int event ) {
        if ( event != QM_ACTIVATED ) {
                return;
        }

        switch( ( (menucommon_s*)ptr )->id ) {
        case ID_BACK:
                trap_CancelLadderRequest();
                UI_PopMenu();
                break;

        case ID_REFRESH:
        case ID_MODE:
        case ID_TIMEFRAME:
        case ID_REGION:
                LadderMenu_RequestData();
                break;
        }
}

static void LadderMenu_MenuDraw( void ) {
        UI_FillRect( -uis.bias, 0, SCREEN_WIDTH + 2 * uis.bias, SCREEN_HEIGHT, ladderBackdropColor );
        LadderMenu_UpdateFromBackend();
        Menu_Draw( &s_ladderMenu.menu );
}

static void LadderMenu_MenuInit( void ) {
        int i;

        memset( &s_ladderMenu, 0, sizeof( s_ladderMenu ) );

        UI_LadderMenu_Cache();

        s_ladderMenu.menu.wrapAround = qtrue;
        s_ladderMenu.menu.fullscreen = qfalse;
        s_ladderMenu.menu.draw = LadderMenu_MenuDraw;

        s_ladderMenu.frame.generic.type = MTYPE_BITMAP;
        s_ladderMenu.frame.generic.flags = QMF_INACTIVE;
        s_ladderMenu.frame.generic.name = LADDER_FRAME;
        s_ladderMenu.frame.generic.x = 142;
        s_ladderMenu.frame.generic.y = 118;
        s_ladderMenu.frame.width = 359;
        s_ladderMenu.frame.height = 256;

        s_ladderMenu.title.generic.type = MTYPE_PTEXT;
        s_ladderMenu.title.generic.flags = QMF_CENTER_JUSTIFY|QMF_INACTIVE;
        s_ladderMenu.title.generic.x = 320;
        s_ladderMenu.title.generic.y = 96;
        s_ladderMenu.title.string = "GLOBAL LADDER";
        s_ladderMenu.title.style = UI_CENTER|UI_SMALLFONT;
        s_ladderMenu.title.color = color_white;

        s_ladderMenu.modeFilter.generic.type = MTYPE_SPINCONTROL;
        s_ladderMenu.modeFilter.generic.name = "Mode:";
        s_ladderMenu.modeFilter.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
        s_ladderMenu.modeFilter.generic.callback = LadderMenu_MenuEvent;
        s_ladderMenu.modeFilter.generic.id = ID_MODE;
        s_ladderMenu.modeFilter.generic.x = 200;
        s_ladderMenu.modeFilter.generic.y = 152;
        s_ladderMenu.modeFilter.itemnames = ladderModeLabels;
        s_ladderMenu.modeFilter.numitems = LADDER_MODE_COUNT;

        s_ladderMenu.timeframeFilter.generic.type = MTYPE_SPINCONTROL;
        s_ladderMenu.timeframeFilter.generic.name = "Range:";
        s_ladderMenu.timeframeFilter.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
        s_ladderMenu.timeframeFilter.generic.callback = LadderMenu_MenuEvent;
        s_ladderMenu.timeframeFilter.generic.id = ID_TIMEFRAME;
        s_ladderMenu.timeframeFilter.generic.x = 320;
        s_ladderMenu.timeframeFilter.generic.y = 152;
        s_ladderMenu.timeframeFilter.itemnames = ladderTimeframeLabels;
        s_ladderMenu.timeframeFilter.numitems = LADDER_TIMEFRAME_COUNT;

        s_ladderMenu.regionFilter.generic.type = MTYPE_SPINCONTROL;
        s_ladderMenu.regionFilter.generic.name = "Region:";
        s_ladderMenu.regionFilter.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
        s_ladderMenu.regionFilter.generic.callback = LadderMenu_MenuEvent;
        s_ladderMenu.regionFilter.generic.id = ID_REGION;
        s_ladderMenu.regionFilter.generic.x = 440;
        s_ladderMenu.regionFilter.generic.y = 152;
        s_ladderMenu.regionFilter.itemnames = ladderRegionLabels;
        s_ladderMenu.regionFilter.numitems = LADDER_REGION_COUNT;

        s_ladderMenu.columns.generic.type = MTYPE_PTEXT;
        s_ladderMenu.columns.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
        s_ladderMenu.columns.generic.x = 168;
        s_ladderMenu.columns.generic.y = 188;
        s_ladderMenu.columns.string = "#  PLAYER & VEHICLE           MODE         REGION       METRIC";
        s_ladderMenu.columns.style = UI_LEFT|UI_SMALLFONT;
        s_ladderMenu.columns.color = text_color_normal;

        s_ladderMenu.results.generic.type = MTYPE_SCROLLLIST;
        s_ladderMenu.results.generic.flags = QMF_PULSEIFFOCUS|QMF_CENTER_JUSTIFY;
        s_ladderMenu.results.generic.callback = LadderMenu_MenuEvent;
        s_ladderMenu.results.generic.id = ID_RESULTS;
        s_ladderMenu.results.generic.x = 320;
        s_ladderMenu.results.generic.y = 208;
        s_ladderMenu.results.width = LADDER_LIST_WIDTH;
        s_ladderMenu.results.height = LADDER_LIST_HEIGHT;
        s_ladderMenu.results.itemnames = s_ladderMenu.resultItems;
        s_ladderMenu.results.columns = 1;
        s_ladderMenu.results.separation = 0;

        s_ladderMenu.status.generic.type = MTYPE_PTEXT;
        s_ladderMenu.status.generic.flags = QMF_CENTER_JUSTIFY|QMF_INACTIVE;
        s_ladderMenu.status.generic.x = 320;
        s_ladderMenu.status.generic.y = 360;
        s_ladderMenu.status.string = s_ladderMenu.statusText;
        s_ladderMenu.status.style = UI_CENTER|UI_SMALLFONT;
        s_ladderMenu.status.color = text_color_normal;

        s_ladderMenu.refresh.generic.type = MTYPE_PTEXT;
        s_ladderMenu.refresh.generic.flags = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
        s_ladderMenu.refresh.generic.callback = LadderMenu_MenuEvent;
        s_ladderMenu.refresh.generic.id = ID_REFRESH;
        s_ladderMenu.refresh.generic.x = 320;
        s_ladderMenu.refresh.generic.y = 384;
        s_ladderMenu.refresh.string = "REFRESH";
        s_ladderMenu.refresh.style = UI_CENTER|UI_SMALLFONT;
        s_ladderMenu.refresh.color = colorRed;

        s_ladderMenu.back.generic.type = MTYPE_PTEXT;
        s_ladderMenu.back.generic.flags = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
        s_ladderMenu.back.generic.callback = LadderMenu_MenuEvent;
        s_ladderMenu.back.generic.id = ID_BACK;
        s_ladderMenu.back.generic.x = 20;
        s_ladderMenu.back.generic.y = 410;
        s_ladderMenu.back.string = "< BACK";
        s_ladderMenu.back.style = UI_LEFT|UI_SMALLFONT;
        s_ladderMenu.back.color = text_color_normal;

        for ( i = 0; i < LADDER_DISPLAY_COUNT; i++ ) {
                s_ladderMenu.resultItems[i] = s_ladderMenu.resultBuffer[i];
                s_ladderMenu.resultBuffer[i][0] = '\0';
        }
        s_ladderMenu.resultItems[LADDER_DISPLAY_COUNT] = NULL;

        Menu_AddItem( &s_ladderMenu.menu, &s_ladderMenu.frame );
        Menu_AddItem( &s_ladderMenu.menu, &s_ladderMenu.title );
        Menu_AddItem( &s_ladderMenu.menu, &s_ladderMenu.modeFilter );
        Menu_AddItem( &s_ladderMenu.menu, &s_ladderMenu.timeframeFilter );
        Menu_AddItem( &s_ladderMenu.menu, &s_ladderMenu.regionFilter );
        Menu_AddItem( &s_ladderMenu.menu, &s_ladderMenu.columns );
        Menu_AddItem( &s_ladderMenu.menu, &s_ladderMenu.results );
        Menu_AddItem( &s_ladderMenu.menu, &s_ladderMenu.status );
        Menu_AddItem( &s_ladderMenu.menu, &s_ladderMenu.refresh );
        Menu_AddItem( &s_ladderMenu.menu, &s_ladderMenu.back );

        LadderMenu_RequestData();
}

void UI_LadderMenu_Cache( void ) {
        trap_R_RegisterShaderNoMip( LADDER_FRAME );
}

void UI_LadderMenu( void ) {
        LadderMenu_MenuInit();
        UI_PushMenu( &s_ladderMenu.menu );
}
