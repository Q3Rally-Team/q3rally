/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

q3rally source code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with q3rally; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
===========================================================================
*/

/*
=======================================================================

DEMOS MENU

=======================================================================
*/

#include "ui_local.h"
#include "ui_rally_frontend.h"

#define NAMEBUFSIZE             ( MAX_DEMOS * 32 )

#define ID_BACK                 10
#define ID_GO                   11
#define ID_LIST                 12
#define ID_RIGHT                13
#define ID_LEFT                 14

/* The virtual UI is 640x480. Keep the demo screen aligned with the other
 * modern frontend surfaces and leave the image itself open behind them. */
#define DEMOS_FRAME_X           24
#define DEMOS_FRAME_Y           20
#define DEMOS_FRAME_WIDTH       592
#define DEMOS_FRAME_HEIGHT      440
#define DEMOS_LIST_X            40
#define DEMOS_LIST_Y            132
#define DEMOS_LIST_WIDTH        356
#define DEMOS_LIST_HEIGHT       256
#define DEMOS_ROW_X             ( DEMOS_LIST_X + 16 )
#define DEMOS_ROW_Y             DEMOS_LIST_Y
#define DEMOS_ROW_WIDTH         ( DEMOS_LIST_WIDTH - 32 )
#define DEMOS_ROW_HEIGHT        22
#define DEMOS_ROW_GAP           3
#define DEMOS_VISIBLE_ITEMS     9
#define DEMOS_DETAIL_X          412
#define DEMOS_DETAIL_Y          88
#define DEMOS_DETAIL_WIDTH      188
#define DEMOS_DETAIL_HEIGHT     300
#define DEMOS_ACTION_Y          420
#define DEMOS_ACTION_HEIGHT     24
#define DEMOS_ACTION_WIDTH      112
#define DEMOS_BACK_X            40
#define DEMOS_PREVIOUS_X        168
#define DEMOS_NEXT_X            296
#define DEMOS_GO_X              424

typedef struct {
    menuframework_s menu;
    menulist_s      list;
    menutext_s      back;
    menutext_s      previous;
    menutext_s      next;
    menutext_s      go;

    int             numDemos;
    char            names[NAMEBUFSIZE];
    char            *demolist[MAX_DEMOS];
} demos_t;

static demos_t s_demos;

static vec4_t demosTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t demosMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t demosAccentColor = UI_FRONTEND_COLOR_ACCENT;

static void Demos_UpdatePaging( void );

static void Demos_PlaySelected( void ) {
    char cleanName[MAX_QPATH];

    if ( s_demos.numDemos <= 0 ||
         s_demos.list.curvalue < 0 ||
         s_demos.list.curvalue >= s_demos.numDemos ) {
        return;
    }

    UI_ForceMenuOff();
    COM_SanitizeFileName( s_demos.list.itemnames[s_demos.list.curvalue],
                          cleanName, sizeof( cleanName ) );
    trap_Cmd_ExecuteText( EXEC_APPEND, va( "demo \"%s\"\n", cleanName ) );
}

static void Demos_MenuEvent( void *ptr, int event ) {
    menucommon_s *item;

    if ( event != QM_ACTIVATED && event != QM_GOTFOCUS ) {
        return;
    }

    item = (menucommon_s *)ptr;
    switch ( item->id ) {
    case ID_GO:
        if ( event == QM_ACTIVATED ) {
            Demos_PlaySelected();
        }
        break;

    case ID_BACK:
        if ( event == QM_ACTIVATED ) {
            UI_PopMenu();
        }
        break;

    case ID_LEFT:
        if ( event == QM_ACTIVATED ) {
            ScrollList_Key( &s_demos.list, K_PGUP );
            Demos_UpdatePaging();
        }
        break;

    case ID_RIGHT:
        if ( event == QM_ACTIVATED ) {
            ScrollList_Key( &s_demos.list, K_PGDN );
            Demos_UpdatePaging();
        }
        break;

    case ID_LIST:
        Demos_UpdatePaging();
        break;
    }
}

static void Demos_FitText( char *out, int outSize, const char *text,
                           int maxWidth ) {
    int len;

    if ( !out || outSize <= 0 ) {
        return;
    }

    Q_strncpyz( out, text ? text : "", outSize );
    if ( Frontend_TextWidth( out, UI_SMALLFONT ) <= maxWidth ) {
        return;
    }

    len = (int)strlen( out );
    while ( len > 3 && Frontend_TextWidth( out, UI_SMALLFONT ) > maxWidth ) {
        len--;
        out[len] = '\0';
    }

    if ( len >= 3 ) {
        out[len - 3] = '.';
        out[len - 2] = '.';
        out[len - 1] = '.';
    }
}

static void Demos_DrawList( void *self ) {
    menulist_s *list;
    int i;

    list = (menulist_s *)self;
    if ( s_demos.numDemos <= 0 ) {
        Frontend_DrawText( DEMOS_ROW_X, DEMOS_ROW_Y + 10,
                           "No demos found", UI_LEFT | UI_SMALLFONT,
                           demosMutedColor );
        return;
    }

    for ( i = 0; i < DEMOS_VISIBLE_ITEMS; i++ ) {
        int index;
        int y;
        qboolean active;

        index = list->top + i;
        if ( index < 0 || index >= s_demos.numDemos ) {
            break;
        }

        y = DEMOS_ROW_Y + i * ( DEMOS_ROW_HEIGHT + DEMOS_ROW_GAP );
        active = ( index == list->curvalue ) ? qtrue : qfalse;
        Frontend_DrawNavButton( DEMOS_ROW_X, y, DEMOS_ROW_WIDTH,
                                DEMOS_ROW_HEIGHT,
                                list->itemnames[index], 1.0f, active,
                                UI_FRONTEND_TEXT_LEFT );
    }
}

static void Demos_DrawAction( void *self ) {
    menutext_s *button;
    qboolean focus;
    qboolean disabled;
    vec4_t disabledColor;
    int x;
    int y;
    int width;
    int height;

    button = (menutext_s *)self;
    focus = ( Menu_ItemAtCursor( button->generic.parent ) == button );
    disabled = ( button->generic.flags & QMF_GRAYED ) ? qtrue : qfalse;
    x = button->generic.left;
    y = button->generic.top;
    width = button->generic.right - button->generic.left;
    height = button->generic.bottom - button->generic.top;

    if ( disabled ) {
        Vector4Copy( demosMutedColor, disabledColor );
        disabledColor[3] = 0.35f;
        Frontend_DrawText( x + width / 2, y + 4, button->string,
                           UI_CENTER | UI_SMALLFONT, disabledColor );
        return;
    }

    Frontend_DrawButton( x, y, width, height, button->string, 1.0f,
                         focus, UI_FRONTEND_TEXT_CENTER );
}

static void Demos_Draw( void ) {
    vec4_t scrimColor = UI_FRONTEND_COLOR_SCRIM;
    char pageText[64];
    char selectedName[64];
    int visibleEnd;

    Frontend_DrawBackground( scrimColor );

    Frontend_DrawPanel( DEMOS_FRAME_X, DEMOS_FRAME_Y,
                        DEMOS_FRAME_WIDTH, DEMOS_FRAME_HEIGHT, 1.0f,
                        UI_FRONTEND_STYLE_FRAME );
    Frontend_DrawText( DEMOS_FRAME_X + 24, DEMOS_FRAME_Y + 24,
                       "Demo browser", UI_LEFT | UI_BIGFONT,
                       demosTextColor );
    Frontend_DrawText( DEMOS_FRAME_X + 24, DEMOS_FRAME_Y + 48,
                       "Replay a run from your local archive",
                       UI_LEFT | UI_SMALLFONT, demosMutedColor );
    Frontend_DrawStatusChip( DEMOS_FRAME_X + DEMOS_FRAME_WIDTH - 100,
                             DEMOS_FRAME_Y + 26, "Archive",
                             demosAccentColor, 1.0f );

    Frontend_DrawCard( DEMOS_LIST_X, DEMOS_LIST_Y - 28,
                       DEMOS_LIST_WIDTH, DEMOS_LIST_HEIGHT, 1.0f, qfalse );
    Frontend_DrawCard( DEMOS_DETAIL_X, DEMOS_DETAIL_Y,
                       DEMOS_DETAIL_WIDTH, DEMOS_DETAIL_HEIGHT, 1.0f,
                       qfalse );
    Frontend_DrawText( DEMOS_LIST_X + 16, DEMOS_LIST_Y - 10,
                       "Available demos", UI_LEFT | UI_SMALLFONT,
                       demosMutedColor );
    Frontend_DrawText( DEMOS_DETAIL_X + 16, DEMOS_DETAIL_Y + 16,
                       "Selected demo", UI_LEFT | UI_SMALLFONT,
                       demosMutedColor );

    Menu_Draw( &s_demos.menu );

    if ( s_demos.numDemos > 0 &&
         s_demos.list.curvalue >= 0 &&
         s_demos.list.curvalue < s_demos.numDemos ) {
        Demos_FitText( selectedName, sizeof( selectedName ),
                       s_demos.list.itemnames[s_demos.list.curvalue],
                       DEMOS_DETAIL_WIDTH - 32 );
        Frontend_DrawStatusChip( DEMOS_DETAIL_X + 16, DEMOS_DETAIL_Y + 50,
                                 "Replay ready", demosAccentColor, 1.0f );
        Frontend_DrawText( DEMOS_DETAIL_X + 16, DEMOS_DETAIL_Y + 96,
                           selectedName, UI_LEFT | UI_BIGFONT,
                           demosTextColor );
        Frontend_DrawText( DEMOS_DETAIL_X + 16, DEMOS_DETAIL_Y + 132,
                           "Press Play demo", UI_LEFT | UI_SMALLFONT,
                           demosMutedColor );
        Frontend_DrawText( DEMOS_DETAIL_X + 16, DEMOS_DETAIL_Y + 154,
                           "to start this run.", UI_LEFT | UI_SMALLFONT,
                           demosMutedColor );
    } else {
        Frontend_DrawText( DEMOS_DETAIL_X + 16, DEMOS_DETAIL_Y + 86,
                           "Nothing to replay", UI_LEFT | UI_SMALLFONT,
                           demosMutedColor );
    }

    if ( s_demos.numDemos > 0 ) {
        visibleEnd = s_demos.list.top + DEMOS_VISIBLE_ITEMS;
        if ( visibleEnd > s_demos.numDemos ) {
            visibleEnd = s_demos.numDemos;
        }
        Com_sprintf( pageText, sizeof( pageText ), "%d-%d of %d",
                     s_demos.list.top + 1, visibleEnd, s_demos.numDemos );
    } else {
        Q_strncpyz( pageText, "0 demos", sizeof( pageText ) );
    }

    Frontend_DrawText( DEMOS_LIST_X + DEMOS_LIST_WIDTH - 16,
                       DEMOS_LIST_Y + DEMOS_LIST_HEIGHT - 20, pageText,
                       UI_RIGHT | UI_SMALLFONT, demosMutedColor );
    Frontend_DrawText( DEMOS_FRAME_X + 24, DEMOS_FRAME_Y + 384,
                       s_demos.numDemos == 1 ? "1 demo available" :
                       va( "%d demos available", s_demos.numDemos ),
                       UI_LEFT | UI_SMALLFONT, demosMutedColor );
    Frontend_DrawText( DEMOS_FRAME_X + DEMOS_FRAME_WIDTH - 24,
                       DEMOS_FRAME_Y + 384,
                       "Enter play   Esc back", UI_RIGHT | UI_SMALLFONT,
                       demosMutedColor );
}

static sfxHandle_t Demos_MenuKey( int key ) {
    int row;
    int index;

    if ( key & K_CHAR_FLAG ) {
        return 0;
    }

    /* The list is drawn with custom row spacing, so resolve its mouse hit
     * area explicitly before the generic menu router sees the click. */
    if ( key == K_MOUSE1 &&
         uis.cursorx >= DEMOS_ROW_X &&
         uis.cursorx <= DEMOS_ROW_X + DEMOS_ROW_WIDTH &&
         uis.cursory >= DEMOS_ROW_Y &&
         uis.cursory < DEMOS_ROW_Y + DEMOS_VISIBLE_ITEMS *
             ( DEMOS_ROW_HEIGHT + DEMOS_ROW_GAP ) ) {
        row = ( uis.cursory - DEMOS_ROW_Y ) /
              ( DEMOS_ROW_HEIGHT + DEMOS_ROW_GAP );
        if ( uis.cursory >= DEMOS_ROW_Y + row *
             ( DEMOS_ROW_HEIGHT + DEMOS_ROW_GAP ) + DEMOS_ROW_HEIGHT ) {
            return menu_null_sound;
        }

        index = s_demos.list.top + row;
        if ( index >= 0 && index < s_demos.numDemos ) {
            s_demos.list.oldvalue = s_demos.list.curvalue;
            s_demos.list.curvalue = index;
            return s_demos.list.oldvalue == index ? menu_null_sound :
                                                     menu_move_sound;
        }
        return menu_null_sound;
    }

    if ( ( key == K_ENTER || key == K_KP_ENTER ) &&
         Menu_ItemAtCursor( &s_demos.menu ) ==
             (menucommon_s *)&s_demos.list ) {
        Demos_PlaySelected();
        return menu_in_sound;
    }

    return Menu_DefaultKey( &s_demos.menu, key );
}

static void Demos_UpdatePaging( void ) {
    s_demos.previous.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    s_demos.next.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;

    if ( s_demos.numDemos <= 0 || s_demos.list.top <= 0 ) {
        s_demos.previous.generic.flags |= QMF_GRAYED;
    }
    if ( s_demos.numDemos <= 0 ||
         s_demos.list.top + DEMOS_VISIBLE_ITEMS >= s_demos.numDemos ) {
        s_demos.next.generic.flags |= QMF_GRAYED;
    }
}

static void Demos_InitAction( menutext_s *item, int id, const char *label,
                              int x ) {
    item->generic.type = MTYPE_PTEXT;
    item->generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    item->generic.id = id;
    item->generic.callback = Demos_MenuEvent;
    item->generic.x = x + DEMOS_ACTION_WIDTH / 2;
    item->generic.y = DEMOS_ACTION_Y;
    item->generic.left = x;
    item->generic.top = DEMOS_ACTION_Y;
    item->generic.right = x + DEMOS_ACTION_WIDTH;
    item->generic.bottom = DEMOS_ACTION_Y + DEMOS_ACTION_HEIGHT;
    item->string = (char *)label;
    item->color = demosTextColor;
    item->style = UI_CENTER | UI_SMALLFONT;
    item->generic.ownerdraw = Demos_DrawAction;
}

static void Demos_MenuInit( void ) {
    memset( &s_demos, 0, sizeof( s_demos ) );

    s_demos.menu.draw = Demos_Draw;
    s_demos.menu.key = Demos_MenuKey;
    s_demos.menu.fullscreen = qtrue;
    s_demos.menu.wrapAround = qtrue;

    s_demos.list.generic.type = MTYPE_SCROLLLIST;
    s_demos.list.generic.flags = QMF_PULSEIFFOCUS;
    s_demos.list.generic.id = ID_LIST;
    s_demos.list.generic.callback = Demos_MenuEvent;
    s_demos.list.generic.x = DEMOS_ROW_X;
    s_demos.list.generic.y = DEMOS_ROW_Y;
    s_demos.list.generic.left = DEMOS_ROW_X;
    s_demos.list.generic.top = DEMOS_ROW_Y;
    s_demos.list.generic.right = DEMOS_ROW_X + DEMOS_ROW_WIDTH;
    s_demos.list.generic.bottom = DEMOS_ROW_Y + DEMOS_VISIBLE_ITEMS *
                                  ( DEMOS_ROW_HEIGHT + DEMOS_ROW_GAP );
    s_demos.list.width = 48;
    s_demos.list.height = DEMOS_VISIBLE_ITEMS;
    s_demos.list.columns = 1;
    s_demos.list.itemnames = (const char **)s_demos.demolist;
    s_demos.list.generic.ownerdraw = Demos_DrawList;

    s_demos.numDemos = UI_FetchDemoList( s_demos.names, sizeof( s_demos.names ),
                                         (const char **)s_demos.demolist,
                                         MAX_DEMOS, NULL );
    s_demos.list.numitems = s_demos.numDemos;

    if ( !s_demos.numDemos ) {
        s_demos.list.itemnames[0] = "No demos found";
        s_demos.list.numitems = 1;
        s_demos.list.generic.flags |= QMF_INACTIVE;
    }

    Demos_InitAction( &s_demos.back, ID_BACK, "Back", DEMOS_BACK_X );
    Demos_InitAction( &s_demos.previous, ID_LEFT, "Previous",
                      DEMOS_PREVIOUS_X );
    Demos_InitAction( &s_demos.next, ID_RIGHT, "Next", DEMOS_NEXT_X );
    Demos_InitAction( &s_demos.go, ID_GO, "Play demo", DEMOS_GO_X );

    Menu_AddItem( &s_demos.menu, &s_demos.list );
    Menu_AddItem( &s_demos.menu, &s_demos.back );
    Menu_AddItem( &s_demos.menu, &s_demos.previous );
    Menu_AddItem( &s_demos.menu, &s_demos.next );
    Menu_AddItem( &s_demos.menu, &s_demos.go );

    if ( !s_demos.numDemos ) {
        s_demos.go.generic.flags |= QMF_GRAYED;
    }
    Demos_UpdatePaging();
}

void Demos_Cache( void ) {
    /* Keep this legacy entry point for existing callers. */
}

void UI_DemosMenu( void ) {
    Demos_MenuInit();
    UI_PushMenu( &s_demos.menu );
}
