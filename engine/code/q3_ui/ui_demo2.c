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
#define ID_PAUSE 15
#define ID_STOP 16
#define ID_RESTART 17
#define ID_REWIND 18
#define ID_FORWARD 19

/* The virtual UI is 640x480. Keep the demo screen aligned with the other
 * modern frontend surfaces and leave the image itself open behind them. */
#define DEMOS_FRAME_X           24
#define DEMOS_FRAME_Y           20
#define DEMOS_FRAME_WIDTH       592
#define DEMOS_FRAME_HEIGHT      440
#define DEMOS_LIST_X            40
#define DEMOS_LIST_Y            112
#define DEMOS_LIST_WIDTH        208
#define DEMOS_LIST_HEIGHT       256
#define DEMOS_ROW_X             ( DEMOS_LIST_X + 16 )
#define DEMOS_ROW_Y             DEMOS_LIST_Y
#define DEMOS_ROW_WIDTH         ( DEMOS_LIST_WIDTH - 32 )
#define DEMOS_ROW_HEIGHT        UI_FRONTEND_BUTTON_HEIGHT
#define DEMOS_ROW_GAP           3
#define DEMOS_VISIBLE_ITEMS     9
#define DEMOS_DETAIL_X          412
#define DEMOS_DETAIL_Y          88
#define DEMOS_DETAIL_WIDTH      188
#define DEMOS_DETAIL_HEIGHT     300
#define DEMOS_ACTION_Y          420
#define DEMOS_ACTION_HEIGHT     UI_FRONTEND_BUTTON_HEIGHT
#define DEMOS_ACTION_WIDTH      UI_FRONTEND_ACTION_WIDTH
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
    menutext_s pause, stop, restart, rewind, forward;
    char playing[MAX_QPATH];
    int infoSelection, infoBytes;

    int             numDemos;
    char            names[NAMEBUFSIZE];
    char            *demolist[MAX_DEMOS];
} demos_t;

static demos_t s_demos;

static vec4_t demosTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t demosMutedColor = UI_FRONTEND_COLOR_MUTED;

static void Demos_UpdatePaging( void );

static void Demos_PlaySelected( void ) {
    char cleanName[MAX_QPATH];

    if ( s_demos.numDemos <= 0 ||
         s_demos.list.curvalue < 0 ||
         s_demos.list.curvalue >= s_demos.numDemos ) {
        return;
    }

    COM_SanitizeFileName( s_demos.list.itemnames[s_demos.list.curvalue],
                          cleanName, sizeof( cleanName ) );
    Q_strncpyz( s_demos.playing, cleanName, sizeof(s_demos.playing) );
    trap_Cvar_Set("ui_demoPlayerName", cleanName);
    trap_Cvar_Set( "cl_freezeDemo", "0" );
    trap_Cmd_ExecuteText( EXEC_APPEND, va( "demopreview \"%s\"\n", cleanName ) );
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

    case ID_REWIND:
    case ID_FORWARD:
        if (event == QM_ACTIVATED)
            trap_Cmd_ExecuteText(EXEC_APPEND, item->id == ID_REWIND ? "demoseek -10\n" : "demoseek 10\n");
        break;
    case ID_PAUSE:
        if ( event == QM_ACTIVATED && trap_Cvar_VariableValue("cl_demoPlayer") )
            trap_Cvar_Set("cl_freezeDemo", trap_Cvar_VariableValue("cl_freezeDemo") ? "0" : "1");
        break;
    case ID_STOP:
        if ( event == QM_ACTIVATED ) trap_Cmd_ExecuteText(EXEC_APPEND, "demopreviewstop\n");
        break;
    case ID_RESTART:
        if ( event == QM_ACTIVATED && s_demos.playing[0] ) {
            trap_Cvar_Set("cl_freezeDemo", "0");
            trap_Cmd_ExecuteText(EXEC_APPEND, va("demopreview \"%s\"\n", s_demos.playing));
        }
        break;
    case ID_BACK:
        if ( event == QM_ACTIVATED ) {
            trap_Cmd_ExecuteText(EXEC_APPEND, "demopreviewstop\n");
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
        char label[24];

        index = list->top + i;
        if ( index < 0 || index >= s_demos.numDemos ) {
            break;
        }

        y = DEMOS_ROW_Y + i * ( DEMOS_ROW_HEIGHT + DEMOS_ROW_GAP );
        active = ( index == list->curvalue ) ? qtrue : qfalse;
        Q_strncpyz(label, list->itemnames[index], sizeof(label));
        Frontend_DrawNavButton( DEMOS_ROW_X, y, DEMOS_ROW_WIDTH,
                                DEMOS_ROW_HEIGHT,
                                label, 1.0f, active,
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
    qboolean playing = trap_Cvar_VariableValue("cl_demoPlayer") != 0;
    char title[40];
    Q_strncpyz(title, playing ? s_demos.playing : "Preview", sizeof(title));
    s_demos.pause.string = trap_Cvar_VariableValue("cl_freezeDemo") ? "Resume" : "Pause";
    Frontend_DrawBackground(scrimColor);
    Frontend_DrawPanel(24, 20, 592, 440, 1.0f, UI_FRONTEND_STYLE_FRAME);
    Frontend_DrawText(40, 44, "Demo player", UI_LEFT | UI_BIGFONT, demosTextColor);
    Frontend_DrawText(40, 76, "Select a recording and press Play", UI_LEFT | UI_SMALLFONT, demosMutedColor);
    Frontend_DrawCard(40, 92, 208, 290, 1.0f, qfalse);
    Frontend_DrawCard(260, 108, 336, 254, 1.0f, qfalse);
    if (trap_Cvar_VariableValue("cl_demoSeeking"))
        Frontend_DrawText(280, 220, "Seeking...", UI_LEFT | UI_SMALLFONT, demosTextColor);
    if (!playing && s_demos.numDemos > 0) {
        const char *name = s_demos.list.itemnames[s_demos.list.curvalue];
        const char *ext = strrchr(name, '.');
        char display[40];
        if (s_demos.infoSelection != s_demos.list.curvalue) {
            fileHandle_t file;
            s_demos.infoSelection = s_demos.list.curvalue;
            s_demos.infoBytes = trap_FS_FOpenFile(va("demos/%s", name), &file, FS_READ);
            if (file) trap_FS_FCloseFile(file);
        }
        Q_strncpyz(display, name, sizeof(display));
        Frontend_DrawText(280, 150, "Selected recording", UI_LEFT | UI_SMALLFONT, demosMutedColor);
        Frontend_DrawText(280, 178, display, UI_LEFT | UI_SMALLFONT, demosTextColor);
        Frontend_DrawText(280, 210, va("Size: %.1f MB", s_demos.infoBytes / 1048576.0f), UI_LEFT | UI_SMALLFONT, demosMutedColor);
        Frontend_DrawText(280, 236, va("Format: %s", ext ? ext + 1 : "unknown"), UI_LEFT | UI_SMALLFONT, demosMutedColor);
        Frontend_DrawText(280, 300, "Press Play demo to start", UI_LEFT | UI_SMALLFONT, demosMutedColor);
    }
    Frontend_DrawText(264, 94, title, UI_LEFT | UI_SMALLFONT, demosTextColor);
    Frontend_DrawText(40, 402, va("%d demos", s_demos.numDemos), UI_LEFT | UI_SMALLFONT, demosMutedColor);
    Menu_Draw(&s_demos.menu);
}

static sfxHandle_t Demos_MenuKey( int key ) {
    int row;
    int index;

    if ( key == K_ESCAPE || key == K_MOUSE2 ) trap_Cmd_ExecuteText(EXEC_APPEND, "demopreviewstop\n");
    if ((key == K_LEFTARROW || key == K_RIGHTARROW) && trap_Cvar_VariableValue("cl_demoPlayer")) {
        trap_Cmd_ExecuteText(EXEC_APPEND, key == K_LEFTARROW ? "demoseek -10\n" : "demoseek 10\n");
        return menu_null_sound;
    }
    if ( key == K_SPACE && trap_Cvar_VariableValue("cl_demoPlayer") ) {
        trap_Cvar_Set("cl_freezeDemo", trap_Cvar_VariableValue("cl_freezeDemo") ? "0" : "1");
        return menu_null_sound;
    }
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
    s_demos.previous.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
    s_demos.next.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;

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
    item->generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
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
    s_demos.infoSelection = -1;
    trap_Cvar_VariableStringBuffer("ui_demoPlayerName", s_demos.playing, sizeof(s_demos.playing));

    s_demos.menu.draw = Demos_Draw;
    s_demos.menu.key = Demos_MenuKey;
    s_demos.menu.fullscreen = qtrue;
    s_demos.menu.wrapAround = qtrue;

    s_demos.list.generic.type = MTYPE_SCROLLLIST;
    s_demos.list.generic.flags = QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
    s_demos.list.generic.id = ID_LIST;
    s_demos.list.generic.callback = Demos_MenuEvent;
    s_demos.list.generic.x = DEMOS_ROW_X;
    s_demos.list.generic.y = DEMOS_ROW_Y;
    s_demos.list.generic.left = DEMOS_ROW_X;
    s_demos.list.generic.top = DEMOS_ROW_Y;
    s_demos.list.generic.right = DEMOS_ROW_X + DEMOS_ROW_WIDTH;
    s_demos.list.generic.bottom = DEMOS_ROW_Y + DEMOS_VISIBLE_ITEMS *
                                  ( DEMOS_ROW_HEIGHT + DEMOS_ROW_GAP );
    s_demos.list.width = 22;
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

    Demos_InitAction(&s_demos.pause, ID_PAUSE, "Pause", 264);
    Demos_InitAction(&s_demos.restart, ID_RESTART, "Restart", 376);
    Demos_InitAction(&s_demos.stop, ID_STOP, "Stop", 488);
    s_demos.pause.generic.y = s_demos.restart.generic.y = s_demos.stop.generic.y = 370;
    s_demos.pause.generic.top = s_demos.restart.generic.top = s_demos.stop.generic.top = 370;
    s_demos.pause.generic.bottom = s_demos.restart.generic.bottom = s_demos.stop.generic.bottom = 398;
    Demos_InitAction(&s_demos.rewind, ID_REWIND, "-10 sec", 264);
    Demos_InitAction(&s_demos.forward, ID_FORWARD, "+10 sec", 440);
    s_demos.rewind.generic.y = s_demos.rewind.generic.top = 400;
    s_demos.forward.generic.y = s_demos.forward.generic.top = 400;
    s_demos.rewind.generic.bottom = s_demos.forward.generic.bottom = 420;
    Menu_AddItem(&s_demos.menu, &s_demos.rewind);
    Menu_AddItem(&s_demos.menu, &s_demos.forward);
    Menu_AddItem(&s_demos.menu, &s_demos.pause);
    Menu_AddItem(&s_demos.menu, &s_demos.restart);
    Menu_AddItem(&s_demos.menu, &s_demos.stop);
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

/* Naming is offered after the recording has safely been closed. Escape keeps
 * the automatically assigned name, so dismissing this menu never loses a demo. */
static struct {
    menuframework_s menu;
    menufield_s name;
    menutext_s save, keep;
    char original[MAX_QPATH];
    char error[80];
} demoSave;

static void DemoSave_Event(void *ptr, int event) {
    char clean[MAX_QPATH];
    char target[MAX_QPATH];
    const char *ext;
    fileHandle_t file;
    if (event != QM_ACTIVATED) return;
    if (((menucommon_s *)ptr)->id == 1) { UI_PopMenu(); return; }
    COM_SanitizeFileName(demoSave.name.field.buffer, clean, sizeof(clean));
    if (!clean[0] || strcmp(clean, demoSave.name.field.buffer) || strchr(clean, '/') || strchr(clean, 92) || strstr(clean, "..")) {
        Q_strncpyz(demoSave.error, "Please enter a valid filename", sizeof(demoSave.error));
        return;
    }
    ext = strrchr(demoSave.original, '.');
    Com_sprintf(target, sizeof(target), "%s%s", clean, ext ? ext : "");
    if (strcmp(target, demoSave.original)) {
        trap_FS_FOpenFile(va("demos/%s", target), &file, FS_READ);
        if (file) {
            trap_FS_FCloseFile(file);
            Q_strncpyz(demoSave.error, "That filename already exists", sizeof(demoSave.error));
            return;
        }
        trap_Cmd_ExecuteText(EXEC_APPEND, va("demosavename \"%s\"\n", clean));
    }
    UI_PopMenu();
}

static void DemoSave_DrawName(void *self) {
    menufield_s *field = (menufield_s *)self;
    char visible[33], before[33];
    int cursor = field->field.cursor - field->field.scroll;
    Q_strncpyz(visible, field->field.buffer + field->field.scroll, sizeof(visible));
    Frontend_DrawText(132, 224, visible, UI_LEFT | UI_SMALLFONT, demosTextColor);
    if (Menu_ItemAtCursor(&demoSave.menu) == field && (uis.realtime / 500) % 2 == 0) {
        if (cursor < 0) cursor = 0;
        if (cursor > 32) cursor = 32;
        Q_strncpyz(before, visible, cursor + 1);
        Frontend_DrawText(132 + Frontend_TextWidth(before, UI_SMALLFONT), 224,
            "_", UI_LEFT | UI_SMALLFONT, demosTextColor);
    }
}

static void DemoSave_Draw(void) {
    vec4_t scrim = UI_FRONTEND_COLOR_SCRIM;
    Frontend_DrawBackground(scrim);
    Frontend_DrawPanel(96, 120, 448, 240, 1, UI_FRONTEND_STYLE_FRAME);
    Frontend_DrawText(128, 144, "Save demo recording", UI_LEFT | UI_BIGFONT, demosTextColor);
    Frontend_DrawText(128, 188, "Filename (without extension)", UI_LEFT | UI_SMALLFONT, demosMutedColor);
    UI_FillRect(128, 220, 376, 24, colorBlack);
    Frontend_DrawText(128, 266, demoSave.error, UI_LEFT | UI_SMALLFONT, colorRed);
    Menu_Draw(&demoSave.menu);
}

void UI_DemoSaveMenu(void) {
    memset(&demoSave, 0, sizeof(demoSave));
    trap_Cvar_VariableStringBuffer("cl_lastDemoName", demoSave.original, sizeof(demoSave.original));
    COM_StripExtension(demoSave.original, demoSave.name.field.buffer, sizeof(demoSave.name.field.buffer));
    demoSave.menu.fullscreen = qtrue;
    demoSave.menu.wrapAround = qtrue;
    demoSave.menu.draw = DemoSave_Draw;
    demoSave.name.generic.type = MTYPE_FIELD;
    demoSave.name.generic.ownerdraw = DemoSave_DrawName;
    demoSave.name.generic.flags = QMF_NODEFAULTINIT;
    demoSave.name.generic.x = demoSave.name.generic.left = 128;
    demoSave.name.generic.y = demoSave.name.generic.top = 220;
    demoSave.name.generic.right = 504;
    demoSave.name.generic.bottom = 244;
    demoSave.name.field.widthInChars = 32;
    demoSave.name.field.maxchars = 40;
    demoSave.name.field.cursor = strlen(demoSave.name.field.buffer);
    demoSave.save.generic.type = demoSave.keep.generic.type = MTYPE_PTEXT;
    demoSave.save.generic.x = 180; demoSave.keep.generic.x = 390;
    demoSave.save.generic.y = demoSave.keep.generic.y = 308;
    demoSave.save.generic.callback = demoSave.keep.generic.callback = DemoSave_Event;
    demoSave.keep.generic.id = 1;
    demoSave.save.string = "Save"; demoSave.keep.string = "Keep name";
    demoSave.save.color = demoSave.keep.color = demosTextColor;
    demoSave.save.style = demoSave.keep.style = UI_CENTER | UI_SMALLFONT;
    demoSave.save.generic.flags = demoSave.keep.generic.flags = QMF_NODEFAULTINIT | QMF_PULSEIFFOCUS;
    demoSave.save.generic.left = 128;
    demoSave.save.generic.right = 300;
    demoSave.keep.generic.left = 324;
    demoSave.keep.generic.right = 504;
    demoSave.save.generic.top = demoSave.keep.generic.top = 302;
    demoSave.save.generic.bottom = demoSave.keep.generic.bottom = 334;
    demoSave.save.generic.ownerdraw = demoSave.keep.generic.ownerdraw = Demos_DrawAction;
    Menu_AddItem(&demoSave.menu, &demoSave.name);
    Menu_AddItem(&demoSave.menu, &demoSave.save);
    Menu_AddItem(&demoSave.menu, &demoSave.keep);
    UI_PushMenu(&demoSave.menu);
}
