/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team
===========================================================================
*/
//
// ui_team.c
//

#include "ui_local.h"
#include "ui_rally_frontend.h"

#define ID_JOINRED      100
#define ID_JOINBLUE     101
#define ID_JOINGAME     102
#define ID_SPECTATE     103
#define ID_JOINGREEN    104
#define ID_JOINYELLOW   105

#define TEAMMAIN_FRAME_X       72
#define TEAMMAIN_FRAME_Y       32
#define TEAMMAIN_FRAME_WIDTH   496
#define TEAMMAIN_FRAME_HEIGHT  416
#define TEAMMAIN_CARD_X        96
#define TEAMMAIN_CARD_Y        112
#define TEAMMAIN_CARD_WIDTH    448
#define TEAMMAIN_CARD_HEIGHT   254
#define TEAMMAIN_ROW_X         112
#define TEAMMAIN_ROW_Y         148
#define TEAMMAIN_ROW_WIDTH     416
#define TEAMMAIN_ROW_HEIGHT    26
#define TEAMMAIN_ROW_GAP       4

static vec4_t teamMainTextColor   = UI_FRONTEND_COLOR_TEXT;
static vec4_t teamMainMutedColor  = UI_FRONTEND_COLOR_MUTED;
static vec4_t teamMainAccentColor = UI_FRONTEND_COLOR_ACCENT;
static vec4_t teamMainRedColor    = { 1.0f, 0.34f, 0.34f, 1.0f };
static vec4_t teamMainBlueColor   = { 0.42f, 0.64f, 1.0f, 1.0f };
static vec4_t teamMainGreenColor  = { 0.42f, 0.92f, 0.56f, 1.0f };
static vec4_t teamMainYellowColor = { 1.0f, 0.82f, 0.28f, 1.0f };

typedef struct {
    menuframework_s menu;
    menutext_s      joinred;
    menutext_s      joinblue;
    menutext_s      joingreen;
    menutext_s      joinyellow;
    menutext_s      joingame;
    menutext_s      spectate;
} teammain_t;

static teammain_t s_teammain;

static void TeamMain_MenuEvent( void *ptr, int event );

static void TeamMain_DrawAction( void *self ) {
    menutext_s *button;
    qboolean focus;
    qboolean disabled;
    const float *baseColor;
    const float *textColor;

    button = (menutext_s *)self;
    focus = ( Menu_ItemAtCursor( button->generic.parent ) == button );
    disabled = ( button->generic.flags & ( QMF_GRAYED | QMF_INACTIVE ) ) ?
        qtrue : qfalse;
    baseColor = teamMainTextColor;

    switch ( button->generic.id ) {
    case ID_JOINRED:
        baseColor = teamMainRedColor;
        break;
    case ID_JOINBLUE:
        baseColor = teamMainBlueColor;
        break;
    case ID_JOINGREEN:
        baseColor = teamMainGreenColor;
        break;
    case ID_JOINYELLOW:
        baseColor = teamMainYellowColor;
        break;
    }

    textColor = disabled ? teamMainMutedColor :
        ( focus ? teamMainAccentColor : baseColor );

    Frontend_DrawCard( button->generic.left, button->generic.top,
        button->generic.right - button->generic.left,
        button->generic.bottom - button->generic.top, 1.0f,
        focus && !disabled );
    Frontend_DrawText( button->generic.left + 16,
        button->generic.top + 6, button->string,
        UI_LEFT | UI_SMALLFONT, textColor );
}

static void TeamMain_LayoutAction( menutext_s *button, int id,
                                    char *label, int row ) {
    button->generic.type = MTYPE_PTEXT;
    button->generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS |
        QMF_NODEFAULTINIT;
    button->generic.id = id;
    button->generic.callback = TeamMain_MenuEvent;
    button->generic.left = TEAMMAIN_ROW_X;
    button->generic.top = TEAMMAIN_ROW_Y + row *
        ( TEAMMAIN_ROW_HEIGHT + TEAMMAIN_ROW_GAP );
    button->generic.right = TEAMMAIN_ROW_X + TEAMMAIN_ROW_WIDTH;
    button->generic.bottom = button->generic.top + TEAMMAIN_ROW_HEIGHT;
    button->generic.x = TEAMMAIN_ROW_X + TEAMMAIN_ROW_WIDTH / 2;
    button->generic.y = button->generic.top + 6;
    button->generic.ownerdraw = TeamMain_DrawAction;
    button->string = label;
    button->style = UI_LEFT | UI_SMALLFONT;
}

static void TeamMain_Draw( void ) {
    vec4_t overlayColor = UI_FRONTEND_COLOR_SCRIM;

    UI_SetColor( NULL );
    UI_FillRect( -uis.bias, 0, SCREEN_WIDTH + uis.bias * 2,
        SCREEN_HEIGHT, overlayColor );
    Frontend_DrawPanel( TEAMMAIN_FRAME_X, TEAMMAIN_FRAME_Y,
        TEAMMAIN_FRAME_WIDTH, TEAMMAIN_FRAME_HEIGHT, 1.0f,
        UI_FRONTEND_STYLE_FRAME );
    Frontend_DrawText( TEAMMAIN_FRAME_X + 24, TEAMMAIN_FRAME_Y + 24,
        "Choose team", UI_LEFT | UI_BIGFONT, teamMainTextColor );
    Frontend_DrawText( TEAMMAIN_FRAME_X + 24, TEAMMAIN_FRAME_Y + 48,
        "Choose a team or spectate the race", UI_LEFT | UI_SMALLFONT,
        teamMainMutedColor );
    Frontend_DrawStatusChip( TEAMMAIN_FRAME_X + TEAMMAIN_FRAME_WIDTH - 104,
        TEAMMAIN_FRAME_Y + 26, "In race", teamMainAccentColor, 1.0f );

    Frontend_DrawCard( TEAMMAIN_CARD_X, TEAMMAIN_CARD_Y,
        TEAMMAIN_CARD_WIDTH, TEAMMAIN_CARD_HEIGHT, 1.0f, qfalse );
    Frontend_DrawText( TEAMMAIN_CARD_X + 16, TEAMMAIN_CARD_Y + 16,
        "Available choices", UI_LEFT | UI_SMALLFONT, teamMainMutedColor );
    Frontend_DrawText( TEAMMAIN_FRAME_X + 24,
        TEAMMAIN_FRAME_Y + TEAMMAIN_FRAME_HEIGHT - 24,
        "Enter select    Esc back", UI_LEFT | UI_SMALLFONT,
        teamMainMutedColor );

    Menu_Draw( &s_teammain.menu );
}

static void TeamMain_MenuEvent( void *ptr, int event ) {
    if ( event != QM_ACTIVATED ) {
        return;
    }

    switch ( ( (menucommon_s *)ptr )->id ) {
    case ID_JOINRED:
        trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team red\n" );
        UI_ForceMenuOff();
        break;
    case ID_JOINBLUE:
        trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team blue\n" );
        UI_ForceMenuOff();
        break;
    case ID_JOINGREEN:
        trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team green\n" );
        UI_ForceMenuOff();
        break;
    case ID_JOINYELLOW:
        trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team yellow\n" );
        UI_ForceMenuOff();
        break;
    case ID_JOINGAME:
        trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team free\n" );
        UI_ForceMenuOff();
        break;
    case ID_SPECTATE:
        trap_Cmd_ExecuteText( EXEC_APPEND, "cmd team spectator\n" );
        UI_ForceMenuOff();
        break;
    }
}

void TeamMain_MenuInit( void ) {
    int gametype;
    char info[MAX_INFO_STRING];

    memset( &s_teammain, 0, sizeof( s_teammain ) );

    s_teammain.menu.wrapAround = qtrue;
    s_teammain.menu.fullscreen = qfalse;
    s_teammain.menu.draw = TeamMain_Draw;

    TeamMain_LayoutAction( &s_teammain.joinred, ID_JOINRED,
        "Join red", 0 );
    TeamMain_LayoutAction( &s_teammain.joinblue, ID_JOINBLUE,
        "Join blue", 1 );
    TeamMain_LayoutAction( &s_teammain.joingreen, ID_JOINGREEN,
        "Join green", 2 );
    TeamMain_LayoutAction( &s_teammain.joinyellow, ID_JOINYELLOW,
        "Join yellow", 3 );
    TeamMain_LayoutAction( &s_teammain.joingame, ID_JOINGAME,
        "Join game", 4 );
    TeamMain_LayoutAction( &s_teammain.spectate, ID_SPECTATE,
        "Spectate", 5 );

    trap_GetConfigString( CS_SERVERINFO, info, MAX_INFO_STRING );
    gametype = atoi( Info_ValueForKey( info, "g_gametype" ) );

    switch ( gametype ) {
    case GT_SINGLE_PLAYER:
    case GT_RACING:
    case GT_RACING_DM:
    case GT_SPRINT:
    case GT_DEATHMATCH:
    case GT_DERBY:
    case GT_LCS:
        s_teammain.joinred.generic.flags  |= QMF_GRAYED;
        s_teammain.joinblue.generic.flags |= QMF_GRAYED;
        s_teammain.joingreen.generic.flags |= QMF_GRAYED;
        s_teammain.joinyellow.generic.flags |= QMF_GRAYED;
        break;
    case GT_TEAM:
    case GT_CTF:
        s_teammain.joingreen.generic.flags |= QMF_GRAYED;
        s_teammain.joinyellow.generic.flags |= QMF_GRAYED;
        s_teammain.joingame.generic.flags |= QMF_GRAYED;
        break;
    case GT_CTF4:
    default:
        s_teammain.joingame.generic.flags |= QMF_GRAYED;
        break;
    }

    Menu_AddItem( &s_teammain.menu, &s_teammain.joinred );
    Menu_AddItem( &s_teammain.menu, &s_teammain.joinblue );
    Menu_AddItem( &s_teammain.menu, &s_teammain.joingreen );
    Menu_AddItem( &s_teammain.menu, &s_teammain.joinyellow );
    Menu_AddItem( &s_teammain.menu, &s_teammain.joingame );
    Menu_AddItem( &s_teammain.menu, &s_teammain.spectate );
}

void TeamMain_Cache( void ) {
}

void UI_TeamMainMenu( void ) {
    TeamMain_MenuInit();
    UI_PushMenu( &s_teammain.menu );
}
