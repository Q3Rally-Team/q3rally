/*
===========================================================================
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
===========================================================================
*/
//
/*
=======================================================================

INGAME MENU

=======================================================================
*/


#include "ui_local.h"
#include "ui_rally_frontend.h"


#define INGAME_FRAME					"menu/art/addbotframe"
//#define INGAME_FRAME					"menu/art/cut_frame"
#define INGAME_MENU_VERTICAL_SPACING	28

#define ID_TEAM					10
#define ID_ADDBOTS				11
#define ID_REMOVEBOTS			12
#define ID_SETUP				13
#define ID_SERVERINFO			14
#define ID_LEAVEARENA			15
#define ID_RESTART				16
#define ID_QUIT					17
#define ID_RESUME				18
#define ID_TEAMORDERS			19

#define INGAME_FRAME_X              72
#define INGAME_FRAME_Y              32
#define INGAME_FRAME_WIDTH          496
#define INGAME_FRAME_HEIGHT         416
#define INGAME_CARD_X               96
#define INGAME_CARD_Y               112
#define INGAME_CARD_WIDTH           448
#define INGAME_CARD_HEIGHT          286
#define INGAME_ROW_X                112
#define INGAME_ROW_Y                124
#define INGAME_ROW_WIDTH            416
#define INGAME_ROW_HEIGHT           24
#define INGAME_ROW_GAP              2

static vec4_t ingameTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t ingameMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t ingameAccentColor = UI_FRONTEND_COLOR_ACCENT;


typedef struct {
	menuframework_s	menu;

	menubitmap_s	frame;
	menutext_s		team;
	menutext_s		setup;
	menutext_s		server;
	menutext_s		leave;
	menutext_s		restart;
	menutext_s		addbots;
	menutext_s		removebots;
	menutext_s		teamorders;
	menutext_s		quit;
	menutext_s		resume;
} ingamemenu_t;

static ingamemenu_t	s_ingame;

static void InGame_DrawAction( void *self ) {
	menutext_s *button;
	qboolean focus;
	qboolean disabled;
	const float *textColor;

	button = (menutext_s *)self;
	focus = ( Menu_ItemAtCursor( button->generic.parent ) == button );
	disabled = ( button->generic.flags & QMF_GRAYED ) ? qtrue : qfalse;
	textColor = disabled ? ingameMutedColor :
		( focus ? ingameAccentColor : ingameTextColor );

	Frontend_DrawCard( button->generic.left, button->generic.top,
		button->generic.right - button->generic.left,
		button->generic.bottom - button->generic.top, 1.0f, focus );
	Frontend_DrawText( button->generic.left + 16,
		button->generic.top + 5, button->string,
		UI_LEFT | UI_SMALLFONT, textColor );
}

static void InGame_LayoutAction( menutext_s *button, int row ) {
	button->generic.left = INGAME_ROW_X;
	button->generic.top = INGAME_ROW_Y + row *
		( INGAME_ROW_HEIGHT + INGAME_ROW_GAP );
	button->generic.right = INGAME_ROW_X + INGAME_ROW_WIDTH;
	button->generic.bottom = button->generic.top + INGAME_ROW_HEIGHT;
	button->generic.x = INGAME_ROW_X + INGAME_ROW_WIDTH / 2;
	button->generic.y = button->generic.top + INGAME_ROW_HEIGHT / 2;
	button->generic.flags |= QMF_NODEFAULTINIT;
	button->generic.ownerdraw = InGame_DrawAction;
}

static void InGame_Draw( void ) {
	vec4_t overlayColor = UI_FRONTEND_COLOR_SCRIM;

	UI_SetColor( NULL );
	UI_FillRect( -uis.bias, 0, SCREEN_WIDTH + uis.bias * 2,
		SCREEN_HEIGHT, overlayColor );
	Frontend_DrawPanel( INGAME_FRAME_X, INGAME_FRAME_Y,
		INGAME_FRAME_WIDTH, INGAME_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( INGAME_FRAME_X + 24, INGAME_FRAME_Y + 24,
		"Paused", UI_LEFT | UI_BIGFONT, ingameTextColor );
	Frontend_DrawText( INGAME_FRAME_X + 24, INGAME_FRAME_Y + 48,
		"Race control and session options", UI_LEFT | UI_SMALLFONT,
		ingameMutedColor );
	Frontend_DrawStatusChip( INGAME_FRAME_X + INGAME_FRAME_WIDTH - 104,
		INGAME_FRAME_Y + 26, "In race", ingameAccentColor, 1.0f );

	Frontend_DrawCard( INGAME_CARD_X, INGAME_CARD_Y,
		INGAME_CARD_WIDTH, INGAME_CARD_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( INGAME_CARD_X + 16, INGAME_CARD_Y + 16,
		"Session", UI_LEFT | UI_SMALLFONT, ingameMutedColor );
	Frontend_DrawText( INGAME_FRAME_X + 24, INGAME_FRAME_Y + 376,
		"Enter select   Esc resume", UI_LEFT | UI_SMALLFONT,
		ingameMutedColor );

	Menu_Draw( &s_ingame.menu );
}


/*
=================
InGame_RestartAction
=================
*/
static void InGame_RestartAction( qboolean result ) {
	if( !result ) {
		return;
	}

	UI_PopMenu();
	trap_Cmd_ExecuteText( EXEC_APPEND, "map_restart 0\n" );
}


/*
=================
InGame_QuitAction
=================
*/
static void InGame_QuitAction( qboolean result ) {
	if( !result ) {
		return;
	}
	UI_PopMenu();
// STONELANCE
	UI_Rally_CreditMenu();
// END
	UI_CreditMenu();
}

static void InGame_LeaveAction( qboolean result ) {
	if( !result ) {
		return;
	}

	UI_PopMenu();
	trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect\n" );
}


/*
=================
InGame_Event
=================
*/
void InGame_Event( void *ptr, int notification ) {
	if( notification != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_TEAM:
		UI_TeamMainMenu();
		break;

	case ID_SETUP:
		UI_SetupMenu();
		break;

	case ID_LEAVEARENA:
		UI_ConfirmMenu( "LEAVE RACE?", 0, InGame_LeaveAction );
		break;

	case ID_RESTART:
		UI_ConfirmMenu( "RESTART RACE?", 0, InGame_RestartAction );
		break;

	case ID_QUIT:
		UI_ConfirmMenu( "EXIT GAME?",  0, InGame_QuitAction );
		break;

	case ID_SERVERINFO:
		UI_ServerInfoMenu();
		break;

	case ID_ADDBOTS:
		UI_AddBotsMenu();
		break;

	case ID_REMOVEBOTS:
		UI_RemoveBotsMenu();
		break;

	case ID_TEAMORDERS:
		UI_TeamOrdersMenu();
		break;

	case ID_RESUME:
		UI_PopMenu();
		break;
	}
}


/*
=================
InGame_MenuInit
=================
*/
void InGame_MenuInit( void ) {
	int		y;
	uiClientState_t	cs;
	char	info[MAX_INFO_STRING];
	int		team;

	memset( &s_ingame, 0 ,sizeof(ingamemenu_t) );

	InGame_Cache();

	s_ingame.menu.wrapAround = qtrue;
	s_ingame.menu.fullscreen = qfalse;
	s_ingame.menu.draw = InGame_Draw;

	s_ingame.frame.generic.type			= MTYPE_BITMAP;
	s_ingame.frame.generic.flags		= QMF_INACTIVE;
	s_ingame.frame.generic.name			= INGAME_FRAME;
	s_ingame.frame.generic.x			= 320-233;//142;
	s_ingame.frame.generic.y			= 240-166;//118;
	s_ingame.frame.width				= 466;//359;
	s_ingame.frame.height				= 332;//256;

	//y = 96;
	y = 88;
	s_ingame.team.generic.type			= MTYPE_PTEXT;
	s_ingame.team.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_ingame.team.generic.x				= 320;
	s_ingame.team.generic.y				= y;
	s_ingame.team.generic.id			= ID_TEAM;
	s_ingame.team.generic.callback		= InGame_Event; 
	s_ingame.team.string				= "START";
	s_ingame.team.color					= color_red;
	s_ingame.team.style					= UI_CENTER|UI_SMALLFONT;

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.addbots.generic.type		= MTYPE_PTEXT;
	s_ingame.addbots.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_ingame.addbots.generic.x			= 320;
	s_ingame.addbots.generic.y			= y;
	s_ingame.addbots.generic.id			= ID_ADDBOTS;
	s_ingame.addbots.generic.callback	= InGame_Event; 
	s_ingame.addbots.string				= "ADD BOTS";
	s_ingame.addbots.color				= color_red;
	s_ingame.addbots.style				= UI_CENTER|UI_SMALLFONT;
	if( !trap_Cvar_VariableValue( "sv_running" ) || !trap_Cvar_VariableValue( "bot_enable" ) || (trap_Cvar_VariableValue( "g_gametype" ) == GT_SINGLE_PLAYER)) {
		s_ingame.addbots.generic.flags |= QMF_GRAYED;
	}

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.removebots.generic.type		= MTYPE_PTEXT;
	s_ingame.removebots.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_ingame.removebots.generic.x			= 320;
	s_ingame.removebots.generic.y			= y;
	s_ingame.removebots.generic.id			= ID_REMOVEBOTS;
	s_ingame.removebots.generic.callback	= InGame_Event; 
	s_ingame.removebots.string				= "REMOVE BOTS";
	s_ingame.removebots.color				= color_red;
	s_ingame.removebots.style				= UI_CENTER|UI_SMALLFONT;
	if( !trap_Cvar_VariableValue( "sv_running" ) || !trap_Cvar_VariableValue( "bot_enable" ) || (trap_Cvar_VariableValue( "g_gametype" ) == GT_SINGLE_PLAYER)) {
		s_ingame.removebots.generic.flags |= QMF_GRAYED;
	}

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.teamorders.generic.type		= MTYPE_PTEXT;
	s_ingame.teamorders.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_ingame.teamorders.generic.x			= 320;
	s_ingame.teamorders.generic.y			= y;
	s_ingame.teamorders.generic.id			= ID_TEAMORDERS;
	s_ingame.teamorders.generic.callback	= InGame_Event; 
	s_ingame.teamorders.string				= "TEAM ORDERS";
	s_ingame.teamorders.color				= color_red;
	s_ingame.teamorders.style				= UI_CENTER|UI_SMALLFONT;
	if( !(trap_Cvar_VariableValue( "g_gametype" ) >= GT_TEAM) ) {
		s_ingame.teamorders.generic.flags |= QMF_GRAYED;
	}
	else {
		trap_GetClientState( &cs );
		trap_GetConfigString( CS_PLAYERS + cs.clientNum, info, MAX_INFO_STRING );
		team = atoi( Info_ValueForKey( info, "t" ) );
		if( team == TEAM_SPECTATOR ) {
			s_ingame.teamorders.generic.flags |= QMF_GRAYED;
		}
	}

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.setup.generic.type			= MTYPE_PTEXT;
	s_ingame.setup.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_ingame.setup.generic.x			= 320;
	s_ingame.setup.generic.y			= y;
	s_ingame.setup.generic.id			= ID_SETUP;
	s_ingame.setup.generic.callback		= InGame_Event; 
	s_ingame.setup.string				= "SETUP";
	s_ingame.setup.color				= color_red;
	s_ingame.setup.style				= UI_CENTER|UI_SMALLFONT;

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.server.generic.type		= MTYPE_PTEXT;
	s_ingame.server.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_ingame.server.generic.x			= 320;
	s_ingame.server.generic.y			= y;
	s_ingame.server.generic.id			= ID_SERVERINFO;
	s_ingame.server.generic.callback	= InGame_Event; 
	s_ingame.server.string				= "SERVER INFO";
	s_ingame.server.color				= color_red;
	s_ingame.server.style				= UI_CENTER|UI_SMALLFONT;

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.restart.generic.type		= MTYPE_PTEXT;
	s_ingame.restart.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_ingame.restart.generic.x			= 320;
	s_ingame.restart.generic.y			= y;
	s_ingame.restart.generic.id			= ID_RESTART;
	s_ingame.restart.generic.callback	= InGame_Event; 
	s_ingame.restart.string				= "RESTART RACE";
	s_ingame.restart.color				= color_red;
	s_ingame.restart.style				= UI_CENTER|UI_SMALLFONT;
	if( !trap_Cvar_VariableValue( "sv_running" ) ) {
		s_ingame.restart.generic.flags |= QMF_GRAYED;
	}

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.resume.generic.type			= MTYPE_PTEXT;
	s_ingame.resume.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_ingame.resume.generic.x				= 320;
	s_ingame.resume.generic.y				= y;
	s_ingame.resume.generic.id				= ID_RESUME;
	s_ingame.resume.generic.callback		= InGame_Event; 
	s_ingame.resume.string					= "RESUME RACE";
	s_ingame.resume.color					= color_red;
	s_ingame.resume.style					= UI_CENTER|UI_SMALLFONT;

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.leave.generic.type			= MTYPE_PTEXT;
	s_ingame.leave.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_ingame.leave.generic.x			= 320;
	s_ingame.leave.generic.y			= y;
	s_ingame.leave.generic.id			= ID_LEAVEARENA;
	s_ingame.leave.generic.callback		= InGame_Event; 
	s_ingame.leave.string				= "LEAVE RACE";
	s_ingame.leave.color				= color_red;
	s_ingame.leave.style				= UI_CENTER|UI_SMALLFONT;

	y += INGAME_MENU_VERTICAL_SPACING;
	s_ingame.quit.generic.type			= MTYPE_PTEXT;
	s_ingame.quit.generic.flags			= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_ingame.quit.generic.x				= 320;
	s_ingame.quit.generic.y				= y;
	s_ingame.quit.generic.id			= ID_QUIT;
	s_ingame.quit.generic.callback		= InGame_Event; 
	s_ingame.quit.string				= "EXIT GAME";
	s_ingame.quit.color					= color_red;
	s_ingame.quit.style					= UI_CENTER|UI_SMALLFONT;

	s_ingame.frame.generic.flags = QMF_INACTIVE | QMF_HIDDEN;
	InGame_LayoutAction( &s_ingame.team, 0 );
	InGame_LayoutAction( &s_ingame.addbots, 1 );
	InGame_LayoutAction( &s_ingame.removebots, 2 );
	InGame_LayoutAction( &s_ingame.teamorders, 3 );
	InGame_LayoutAction( &s_ingame.setup, 4 );
	InGame_LayoutAction( &s_ingame.server, 5 );
	InGame_LayoutAction( &s_ingame.restart, 6 );
	InGame_LayoutAction( &s_ingame.resume, 7 );
	InGame_LayoutAction( &s_ingame.leave, 8 );
	InGame_LayoutAction( &s_ingame.quit, 9 );

	Menu_AddItem( &s_ingame.menu, &s_ingame.frame );
	Menu_AddItem( &s_ingame.menu, &s_ingame.team );
	Menu_AddItem( &s_ingame.menu, &s_ingame.addbots );
	Menu_AddItem( &s_ingame.menu, &s_ingame.removebots );
	Menu_AddItem( &s_ingame.menu, &s_ingame.teamorders );
	Menu_AddItem( &s_ingame.menu, &s_ingame.setup );
	Menu_AddItem( &s_ingame.menu, &s_ingame.server );
	Menu_AddItem( &s_ingame.menu, &s_ingame.restart );
	Menu_AddItem( &s_ingame.menu, &s_ingame.resume );
	Menu_AddItem( &s_ingame.menu, &s_ingame.leave );
	Menu_AddItem( &s_ingame.menu, &s_ingame.quit );
}


/*
=================
InGame_Cache
=================
*/
void InGame_Cache( void ) {
	trap_R_RegisterShaderNoMip( INGAME_FRAME );
}


/*
=================
UI_InGameMenu
=================
*/
void UI_InGameMenu( void ) {
	// force as top level menu
	uis.menusp = 0;  

	// set menu cursor to a nice location
	uis.cursorx = 319;
	uis.cursory = 80;

// STONELANCE
	uis.transitionIn = 0;
	uis.transitionOut = 0;
// END

	InGame_MenuInit();
	UI_PushMenu( &s_ingame.menu );
}
