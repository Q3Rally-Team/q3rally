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

REMOVE BOTS MENU

=======================================================================
*/


#include "ui_local.h"
#include "ui_rally_frontend.h"


#define ART_BACKGROUND		"menu/art/addbotframe"
#define ART_BACK0			"menu/art/back_0"
#define ART_BACK1			"menu/art/back_1"	
#define ART_DELETE0			"menu/art/delete_0"
#define ART_DELETE1			"menu/art/delete_1"
#define ART_ARROWS			"menu/art/arrows_vert_0"
#define ART_ARROWUP			"menu/art/arrows_vert_top"
#define ART_ARROWDOWN		"menu/art/arrows_vert_bot"

#define ID_UP				10
#define ID_DOWN				11
#define ID_DELETE			12
#define ID_BACK				13
#define ID_BOTNAME0			20
#define ID_BOTNAME1			21
#define ID_BOTNAME2			22
#define ID_BOTNAME3			23
#define ID_BOTNAME4			24
#define ID_BOTNAME5			25
#define ID_BOTNAME6			26

#define REMOVEBOTS_FRAME_X             48
#define REMOVEBOTS_FRAME_Y             24
#define REMOVEBOTS_FRAME_WIDTH         544
#define REMOVEBOTS_FRAME_HEIGHT        432
#define REMOVEBOTS_BOT_CARD_X          72
#define REMOVEBOTS_BOT_CARD_Y          112
#define REMOVEBOTS_BOT_CARD_WIDTH      296
#define REMOVEBOTS_BOT_CARD_HEIGHT     280
#define REMOVEBOTS_DETAIL_X            384
#define REMOVEBOTS_DETAIL_Y            112
#define REMOVEBOTS_DETAIL_WIDTH        184
#define REMOVEBOTS_DETAIL_HEIGHT       280
#define REMOVEBOTS_BOT_ROW_X           88
#define REMOVEBOTS_BOT_ROW_Y           144
#define REMOVEBOTS_BOT_ROW_WIDTH       264
#define REMOVEBOTS_BOT_ROW_HEIGHT      22
#define REMOVEBOTS_BOT_ROW_GAP         2
#define REMOVEBOTS_NAV_Y               352
#define REMOVEBOTS_NAV_WIDTH           104
#define REMOVEBOTS_NAV_HEIGHT          24
#define REMOVEBOTS_ACTION_Y            420
#define REMOVEBOTS_ACTION_WIDTH        112
#define REMOVEBOTS_ACTION_HEIGHT       24

static vec4_t removeBotsTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t removeBotsMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t removeBotsAccentColor = UI_FRONTEND_COLOR_ACCENT;


typedef struct {
	menuframework_s	menu;

	menutext_s		banner;
	menubitmap_s	background;
  menubitmap_s	arrows;
	menubitmap_s	up;
	menubitmap_s	down;
  menutext_s		bots[7];
  menutext_s	delete;
	menutext_s	back;

	int				numBots;
	int				baseBotNum;
	int				selectedBotNum;
	char			botnames[7][32];
	int				botClientNums[MAX_BOTS];
} removeBotsMenuInfo_t;

static removeBotsMenuInfo_t	removeBotsMenuInfo;

static void UI_RemoveBotsMenu_DrawAction( void *self ) {
	menucommon_s *item;
	qboolean focus;
	qboolean disabled;
	const char *label;
	const float *textColor;

	item = (menucommon_s *)self;
	focus = ( Menu_ItemAtCursor( item->parent ) == item );
	disabled = ( item->flags & QMF_GRAYED ) ? qtrue : qfalse;
	label = "";
	switch ( item->id ) {
	case ID_UP:
		label = "Prev";
		break;
	case ID_DOWN:
		label = "Next";
		break;
	case ID_DELETE:
		label = "Remove bot";
		break;
	case ID_BACK:
		label = "Back";
		break;
	}
	textColor = disabled ? removeBotsMutedColor :
		( focus ? removeBotsAccentColor : removeBotsTextColor );

	Frontend_DrawCard( item->left, item->top,
		item->right - item->left, item->bottom - item->top,
		1.0f, focus );
	Frontend_DrawText( ( item->left + item->right ) / 2,
		item->top + 5, label, UI_CENTER | UI_SMALLFONT, textColor );
}

static void UI_RemoveBotsMenu_DrawBot( void *self ) {
	menutext_s *bot;
	int index;
	int y;
	qboolean focus;
	qboolean selected;
	const char *name;
	const float *textColor;

	bot = (menutext_s *)self;
	index = bot->generic.id - ID_BOTNAME0;
	y = REMOVEBOTS_BOT_ROW_Y + index *
		( REMOVEBOTS_BOT_ROW_HEIGHT + REMOVEBOTS_BOT_ROW_GAP );
	focus = ( Menu_ItemAtCursor( bot->generic.parent ) == bot );
	selected = ( index == removeBotsMenuInfo.selectedBotNum );
	name = bot->string && bot->string[0] ? bot->string : "Unknown bot";
	textColor = focus || selected ? removeBotsAccentColor : removeBotsTextColor;

	Frontend_DrawCard( REMOVEBOTS_BOT_ROW_X, y, REMOVEBOTS_BOT_ROW_WIDTH,
		REMOVEBOTS_BOT_ROW_HEIGHT, 1.0f, focus || selected );
	Frontend_DrawText( REMOVEBOTS_BOT_ROW_X + 12, y + 4, name,
		UI_LEFT | UI_SMALLFONT, textColor );
}

static void UI_RemoveBotsMenu_Layout( int count ) {
	int n;

	removeBotsMenuInfo.background.generic.flags |= QMF_HIDDEN | QMF_INACTIVE;
	removeBotsMenuInfo.banner.generic.flags |= QMF_HIDDEN | QMF_INACTIVE;
	removeBotsMenuInfo.arrows.generic.flags |= QMF_HIDDEN | QMF_INACTIVE;

	removeBotsMenuInfo.up.generic.left = REMOVEBOTS_BOT_ROW_X;
	removeBotsMenuInfo.up.generic.top = REMOVEBOTS_NAV_Y;
	removeBotsMenuInfo.up.generic.right = REMOVEBOTS_BOT_ROW_X +
		REMOVEBOTS_NAV_WIDTH;
	removeBotsMenuInfo.up.generic.bottom = REMOVEBOTS_NAV_Y +
		REMOVEBOTS_NAV_HEIGHT;
	removeBotsMenuInfo.up.generic.x = ( removeBotsMenuInfo.up.generic.left +
		removeBotsMenuInfo.up.generic.right ) / 2;
	removeBotsMenuInfo.up.generic.y = REMOVEBOTS_NAV_Y + 4;
	removeBotsMenuInfo.up.generic.flags |= QMF_NODEFAULTINIT;
	removeBotsMenuInfo.up.generic.ownerdraw = UI_RemoveBotsMenu_DrawAction;

	removeBotsMenuInfo.down.generic.left = REMOVEBOTS_BOT_ROW_X +
		REMOVEBOTS_BOT_ROW_WIDTH - REMOVEBOTS_NAV_WIDTH;
	removeBotsMenuInfo.down.generic.top = REMOVEBOTS_NAV_Y;
	removeBotsMenuInfo.down.generic.right = REMOVEBOTS_BOT_ROW_X +
		REMOVEBOTS_BOT_ROW_WIDTH;
	removeBotsMenuInfo.down.generic.bottom = REMOVEBOTS_NAV_Y +
		REMOVEBOTS_NAV_HEIGHT;
	removeBotsMenuInfo.down.generic.x = ( removeBotsMenuInfo.down.generic.left +
		removeBotsMenuInfo.down.generic.right ) / 2;
	removeBotsMenuInfo.down.generic.y = REMOVEBOTS_NAV_Y + 4;
	removeBotsMenuInfo.down.generic.flags |= QMF_NODEFAULTINIT;
	removeBotsMenuInfo.down.generic.ownerdraw = UI_RemoveBotsMenu_DrawAction;

	for ( n = 0; n < count; n++ ) {
		removeBotsMenuInfo.bots[n].generic.left = REMOVEBOTS_BOT_ROW_X;
		removeBotsMenuInfo.bots[n].generic.top = REMOVEBOTS_BOT_ROW_Y + n *
			( REMOVEBOTS_BOT_ROW_HEIGHT + REMOVEBOTS_BOT_ROW_GAP );
		removeBotsMenuInfo.bots[n].generic.right = REMOVEBOTS_BOT_ROW_X +
			REMOVEBOTS_BOT_ROW_WIDTH;
		removeBotsMenuInfo.bots[n].generic.bottom =
			removeBotsMenuInfo.bots[n].generic.top + REMOVEBOTS_BOT_ROW_HEIGHT;
		removeBotsMenuInfo.bots[n].generic.x = REMOVEBOTS_BOT_ROW_X +
			REMOVEBOTS_BOT_ROW_WIDTH / 2;
		removeBotsMenuInfo.bots[n].generic.y =
			removeBotsMenuInfo.bots[n].generic.top + 4;
		removeBotsMenuInfo.bots[n].generic.flags |= QMF_NODEFAULTINIT;
		removeBotsMenuInfo.bots[n].generic.ownerdraw =
			UI_RemoveBotsMenu_DrawBot;
	}

	removeBotsMenuInfo.delete.generic.left = REMOVEBOTS_FRAME_X +
		REMOVEBOTS_FRAME_WIDTH - 16 - REMOVEBOTS_ACTION_WIDTH;
	removeBotsMenuInfo.delete.generic.top = REMOVEBOTS_ACTION_Y;
	removeBotsMenuInfo.delete.generic.right =
		removeBotsMenuInfo.delete.generic.left + REMOVEBOTS_ACTION_WIDTH;
	removeBotsMenuInfo.delete.generic.bottom = REMOVEBOTS_ACTION_Y +
		REMOVEBOTS_ACTION_HEIGHT;
	removeBotsMenuInfo.delete.generic.x = ( removeBotsMenuInfo.delete.generic.left +
		removeBotsMenuInfo.delete.generic.right ) / 2;
	removeBotsMenuInfo.delete.generic.y = REMOVEBOTS_ACTION_Y + 4;
	removeBotsMenuInfo.delete.generic.flags |= QMF_NODEFAULTINIT;
	removeBotsMenuInfo.delete.generic.ownerdraw = UI_RemoveBotsMenu_DrawAction;
	if ( !count ) {
		removeBotsMenuInfo.delete.generic.flags |= QMF_GRAYED;
	}

	removeBotsMenuInfo.back.generic.left = REMOVEBOTS_FRAME_X + 16;
	removeBotsMenuInfo.back.generic.top = REMOVEBOTS_ACTION_Y;
	removeBotsMenuInfo.back.generic.right =
		removeBotsMenuInfo.back.generic.left + REMOVEBOTS_ACTION_WIDTH;
	removeBotsMenuInfo.back.generic.bottom = REMOVEBOTS_ACTION_Y +
		REMOVEBOTS_ACTION_HEIGHT;
	removeBotsMenuInfo.back.generic.x = ( removeBotsMenuInfo.back.generic.left +
		removeBotsMenuInfo.back.generic.right ) / 2;
	removeBotsMenuInfo.back.generic.y = REMOVEBOTS_ACTION_Y + 4;
	removeBotsMenuInfo.back.generic.flags |= QMF_NODEFAULTINIT;
	removeBotsMenuInfo.back.generic.ownerdraw = UI_RemoveBotsMenu_DrawAction;
}

static void UI_RemoveBotsMenu_Draw( void ) {
	vec4_t overlayColor = UI_FRONTEND_COLOR_SCRIM;
	const char *selectedName;

	UI_SetColor( NULL );
	UI_FillRect( -uis.bias, 0, SCREEN_WIDTH + uis.bias * 2,
		SCREEN_HEIGHT, overlayColor );
	Frontend_DrawPanel( REMOVEBOTS_FRAME_X, REMOVEBOTS_FRAME_Y,
		REMOVEBOTS_FRAME_WIDTH, REMOVEBOTS_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( REMOVEBOTS_FRAME_X + 24, REMOVEBOTS_FRAME_Y + 24,
		"Remove bots", UI_LEFT | UI_BIGFONT, removeBotsTextColor );
	Frontend_DrawText( REMOVEBOTS_FRAME_X + 24, REMOVEBOTS_FRAME_Y + 48,
		"Select a bot currently in the race",
		UI_LEFT | UI_SMALLFONT, removeBotsMutedColor );
	Frontend_DrawStatusChip( REMOVEBOTS_FRAME_X + REMOVEBOTS_FRAME_WIDTH - 128,
		REMOVEBOTS_FRAME_Y + 26, "Bot roster", removeBotsAccentColor, 1.0f );

	Frontend_DrawCard( REMOVEBOTS_BOT_CARD_X, REMOVEBOTS_BOT_CARD_Y,
		REMOVEBOTS_BOT_CARD_WIDTH, REMOVEBOTS_BOT_CARD_HEIGHT,
		1.0f, qfalse );
	Frontend_DrawText( REMOVEBOTS_BOT_CARD_X + 16,
		REMOVEBOTS_BOT_CARD_Y + 16, "Bots in race",
		UI_LEFT | UI_SMALLFONT, removeBotsMutedColor );
	Frontend_DrawCard( REMOVEBOTS_DETAIL_X, REMOVEBOTS_DETAIL_Y,
		REMOVEBOTS_DETAIL_WIDTH, REMOVEBOTS_DETAIL_HEIGHT,
		1.0f, qfalse );
	Frontend_DrawText( REMOVEBOTS_DETAIL_X + 16,
		REMOVEBOTS_DETAIL_Y + 16, "Selection",
		UI_LEFT | UI_SMALLFONT, removeBotsMutedColor );
	selectedName = removeBotsMenuInfo.numBots > 0 ?
		removeBotsMenuInfo.botnames[removeBotsMenuInfo.selectedBotNum] :
		"No bots in race";
	Frontend_DrawText( REMOVEBOTS_DETAIL_X + 16,
		REMOVEBOTS_DETAIL_Y + 58, selectedName,
		UI_LEFT | UI_SMALLFONT, removeBotsTextColor );
	Frontend_DrawText( REMOVEBOTS_DETAIL_X + 16,
		REMOVEBOTS_DETAIL_Y + 92, "Remove the selected bot",
		UI_LEFT | UI_SMALLFONT, removeBotsMutedColor );
	Frontend_DrawText( REMOVEBOTS_FRAME_X + 24, REMOVEBOTS_FRAME_Y + 376,
		"Select a bot   Enter remove   Esc back",
		UI_LEFT | UI_SMALLFONT, removeBotsMutedColor );

	Menu_Draw( &removeBotsMenuInfo.menu );
}


/*
=================
UI_RemoveBotsMenu_SetBotNames
=================
*/
static void UI_RemoveBotsMenu_SetBotNames( void ) {
	int		n;
	char	info[MAX_INFO_STRING];

	for ( n = 0; (n < 7) && (removeBotsMenuInfo.baseBotNum + n < removeBotsMenuInfo.numBots); n++ ) {
		trap_GetConfigString( CS_PLAYERS + removeBotsMenuInfo.botClientNums[removeBotsMenuInfo.baseBotNum + n], info, MAX_INFO_STRING );
		Q_strncpyz( removeBotsMenuInfo.botnames[n], Info_ValueForKey( info, "n" ), sizeof(removeBotsMenuInfo.botnames[n]) );
		Q_CleanStr( removeBotsMenuInfo.botnames[n] );
	}

}


/*
=================
UI_RemoveBotsMenu_DeleteEvent
=================
*/
static void UI_RemoveBotsMenu_DeleteEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	trap_Cmd_ExecuteText( EXEC_APPEND, va("clientkick %i\n", removeBotsMenuInfo.botClientNums[removeBotsMenuInfo.baseBotNum + removeBotsMenuInfo.selectedBotNum]) );
}


/*
=================
UI_RemoveBotsMenu_BotEvent
=================
*/
static void UI_RemoveBotsMenu_BotEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	removeBotsMenuInfo.bots[removeBotsMenuInfo.selectedBotNum].color = color_orange;
	removeBotsMenuInfo.selectedBotNum = ((menucommon_s*)ptr)->id - ID_BOTNAME0;
	removeBotsMenuInfo.bots[removeBotsMenuInfo.selectedBotNum].color = color_white;
}


/*
=================
UI_RemoveAddBotsMenu_BackEvent
=================
*/
static void UI_RemoveBotsMenu_BackEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}
	UI_PopMenu();
}


/*
=================
UI_RemoveBotsMenu_UpEvent
=================
*/
static void UI_RemoveBotsMenu_UpEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	if( removeBotsMenuInfo.baseBotNum > 0 ) {
		removeBotsMenuInfo.baseBotNum--;
		UI_RemoveBotsMenu_SetBotNames();
	}
}


/*
=================
UI_RemoveBotsMenu_DownEvent
=================
*/
static void UI_RemoveBotsMenu_DownEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	if( removeBotsMenuInfo.baseBotNum + 7 < removeBotsMenuInfo.numBots ) {
		removeBotsMenuInfo.baseBotNum++;
		UI_RemoveBotsMenu_SetBotNames();
	}
}


/*
=================
UI_RemoveBotsMenu_GetBots
=================
*/
static void UI_RemoveBotsMenu_GetBots( void ) {
	int		numPlayers;
	int		isBot;
	int		n;
	char	info[MAX_INFO_STRING];

	trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) );
	numPlayers = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
	removeBotsMenuInfo.numBots = 0;

	for( n = 0; n < numPlayers; n++ ) {
		trap_GetConfigString( CS_PLAYERS + n, info, MAX_INFO_STRING );

		isBot = atoi( Info_ValueForKey( info, "skill" ) );
		if( !isBot ) {
			continue;
		}

		removeBotsMenuInfo.botClientNums[removeBotsMenuInfo.numBots] = n;
		removeBotsMenuInfo.numBots++;
	}
}


/*
=================
UI_RemoveBots_Cache
=================
*/
void UI_RemoveBots_Cache( void ) {
	trap_R_RegisterShaderNoMip( ART_BACKGROUND );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
	trap_R_RegisterShaderNoMip( ART_DELETE0 );
	trap_R_RegisterShaderNoMip( ART_DELETE1 );
}


/*
=================
UI_RemoveBotsMenu_Init
=================
*/
static void UI_RemoveBotsMenu_Init( void ) {
	int		n;
	int		count;
	int		y;

	memset( &removeBotsMenuInfo, 0 ,sizeof(removeBotsMenuInfo) );
	removeBotsMenuInfo.menu.fullscreen = qfalse;
	removeBotsMenuInfo.menu.wrapAround = qtrue;
	removeBotsMenuInfo.menu.draw = UI_RemoveBotsMenu_Draw;

	UI_RemoveBots_Cache();

	UI_RemoveBotsMenu_GetBots();
	UI_RemoveBotsMenu_SetBotNames();
	count = removeBotsMenuInfo.numBots < 7 ? removeBotsMenuInfo.numBots : 7;

	removeBotsMenuInfo.banner.generic.type		= MTYPE_BTEXT;
	removeBotsMenuInfo.banner.generic.x			= 320;
	removeBotsMenuInfo.banner.generic.y			= 16;
	removeBotsMenuInfo.banner.string			= "REMOVE BOTS";
	removeBotsMenuInfo.banner.color				= color_white;
	removeBotsMenuInfo.banner.style				= UI_CENTER;

	removeBotsMenuInfo.background.generic.type	= MTYPE_BITMAP;
	removeBotsMenuInfo.background.generic.name	= ART_BACKGROUND;
	removeBotsMenuInfo.background.generic.flags	= QMF_INACTIVE;
	removeBotsMenuInfo.background.generic.x		= 320-233;
	removeBotsMenuInfo.background.generic.y		= 240-166;
	removeBotsMenuInfo.background.width			= 466;
	removeBotsMenuInfo.background.height		= 332;

	removeBotsMenuInfo.arrows.generic.type		= MTYPE_BITMAP;
	removeBotsMenuInfo.arrows.generic.name		= ART_ARROWS;
	removeBotsMenuInfo.arrows.generic.flags		= QMF_INACTIVE;
	removeBotsMenuInfo.arrows.generic.x			= 200;
	removeBotsMenuInfo.arrows.generic.y			= 128;
	removeBotsMenuInfo.arrows.width				= 64;
	removeBotsMenuInfo.arrows.height			= 128;

	removeBotsMenuInfo.up.generic.type			= MTYPE_BITMAP;
	removeBotsMenuInfo.up.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	removeBotsMenuInfo.up.generic.x				= 200;
	removeBotsMenuInfo.up.generic.y				= 128;
	removeBotsMenuInfo.up.generic.id			= ID_UP;
	removeBotsMenuInfo.up.generic.callback		= UI_RemoveBotsMenu_UpEvent;
	removeBotsMenuInfo.up.width					= 64;
	removeBotsMenuInfo.up.height				= 64;
	removeBotsMenuInfo.up.focuspic				= ART_ARROWUP;

	removeBotsMenuInfo.down.generic.type		= MTYPE_BITMAP;
	removeBotsMenuInfo.down.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	removeBotsMenuInfo.down.generic.x			= 200;
	removeBotsMenuInfo.down.generic.y			= 128+64;
	removeBotsMenuInfo.down.generic.id			= ID_DOWN;
	removeBotsMenuInfo.down.generic.callback	= UI_RemoveBotsMenu_DownEvent;
	removeBotsMenuInfo.down.width				= 64;
	removeBotsMenuInfo.down.height				= 64;
	removeBotsMenuInfo.down.focuspic			= ART_ARROWDOWN;

	for( n = 0, y = 120; n < count; n++, y += 20 ) {
		removeBotsMenuInfo.bots[n].generic.type		= MTYPE_PTEXT;
		removeBotsMenuInfo.bots[n].generic.flags	= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
		removeBotsMenuInfo.bots[n].generic.id		= ID_BOTNAME0 + n;
		removeBotsMenuInfo.bots[n].generic.x		= 320 - 56;
		removeBotsMenuInfo.bots[n].generic.y		= y;
		removeBotsMenuInfo.bots[n].generic.callback	= UI_RemoveBotsMenu_BotEvent;
		removeBotsMenuInfo.bots[n].string			= removeBotsMenuInfo.botnames[n];
		removeBotsMenuInfo.bots[n].color			= color_orange;
		removeBotsMenuInfo.bots[n].style			= UI_LEFT|UI_SMALLFONT;
	}

	removeBotsMenuInfo.delete.generic.type		= MTYPE_PTEXT;
	removeBotsMenuInfo.delete.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	removeBotsMenuInfo.delete.generic.id		= ID_DELETE;
	removeBotsMenuInfo.delete.generic.callback	= UI_RemoveBotsMenu_DeleteEvent;
	removeBotsMenuInfo.delete.generic.x			= 320+64;
	removeBotsMenuInfo.delete.generic.y			= 256+128-64;
  removeBotsMenuInfo.delete.string			= "DELETE";
	removeBotsMenuInfo.delete.color			= color_orange;
	removeBotsMenuInfo.delete.style			= UI_LEFT|UI_SMALLFONT;

	removeBotsMenuInfo.back.generic.type		= MTYPE_PTEXT;
	removeBotsMenuInfo.back.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	removeBotsMenuInfo.back.generic.id			= ID_BACK;
	removeBotsMenuInfo.back.generic.callback	= UI_RemoveBotsMenu_BackEvent;
	removeBotsMenuInfo.back.generic.x			= 320-64;
	removeBotsMenuInfo.back.generic.y			= 256+128-64;
  removeBotsMenuInfo.back.string			= "< BACK";
	removeBotsMenuInfo.back.color			= color_orange;
	removeBotsMenuInfo.back.style			= UI_RIGHT|UI_SMALLFONT;
	UI_RemoveBotsMenu_Layout( count );

	Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.background );
	Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.banner );
	Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.arrows );
	Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.up );
	Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.down );
	for( n = 0; n < count; n++ ) {
		Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.bots[n] );
	}
	Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.delete );
	Menu_AddItem( &removeBotsMenuInfo.menu, &removeBotsMenuInfo.back );

	removeBotsMenuInfo.baseBotNum = 0;
	removeBotsMenuInfo.selectedBotNum = 0;
	removeBotsMenuInfo.bots[0].color = color_white;
}


/*
=================
UI_RemoveBotsMenu
=================
*/
void UI_RemoveBotsMenu( void ) {
	UI_RemoveBotsMenu_Init();
	UI_PushMenu( &removeBotsMenuInfo.menu );
}
