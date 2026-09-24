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

ADD BOTS MENU

=======================================================================
*/


#include "ui_local.h"
#include "ui_rally_frontend.h"


#define ART_BACK0			"menu/art/back_0"
#define ART_BACK1			"menu/art/back_1"	
#define ART_FIGHT0			"menu/art/accept_0"
#define ART_FIGHT1			"menu/art/accept_1"
#define ART_BACKGROUND		"menu/art/addbotframe"
#define ART_ARROWS			"menu/art/arrows_vert_0"
#define ART_ARROWUP			"menu/art/arrows_vert_top"
#define ART_ARROWDOWN		"menu/art/arrows_vert_bot"

#define ID_BACK				10
#define ID_GO				11
#define ID_LIST				12
#define ID_UP				13
#define ID_DOWN				14
#define ID_SKILL			15
#define ID_TEAM				16
#define ID_BOTNAME0			20
#define ID_BOTNAME1			21
#define ID_BOTNAME2			22
#define ID_BOTNAME3			23
#define ID_BOTNAME4			24
#define ID_BOTNAME5			25
#define ID_BOTNAME6			26

#define ADDBOTS_FRAME_X             48
#define ADDBOTS_FRAME_Y             24
#define ADDBOTS_FRAME_WIDTH         544
#define ADDBOTS_FRAME_HEIGHT        432
#define ADDBOTS_BOT_CARD_X          72
#define ADDBOTS_BOT_CARD_Y          112
#define ADDBOTS_BOT_CARD_WIDTH      296
#define ADDBOTS_BOT_CARD_HEIGHT     280
#define ADDBOTS_OPTION_CARD_X       384
#define ADDBOTS_OPTION_CARD_Y       112
#define ADDBOTS_OPTION_CARD_WIDTH   184
#define ADDBOTS_OPTION_CARD_HEIGHT  280
#define ADDBOTS_BOT_ROW_X           88
#define ADDBOTS_BOT_ROW_Y           144
#define ADDBOTS_BOT_ROW_WIDTH       264
#define ADDBOTS_BOT_ROW_HEIGHT      22
#define ADDBOTS_BOT_ROW_GAP         2
#define ADDBOTS_NAV_Y               352
#define ADDBOTS_NAV_WIDTH           104
#define ADDBOTS_NAV_HEIGHT          24
#define ADDBOTS_ACTION_Y            420
#define ADDBOTS_ACTION_WIDTH        104
#define ADDBOTS_ACTION_HEIGHT       24
#define ADDBOTS_OPTION_X            400
#define ADDBOTS_OPTION_WIDTH        152
#define ADDBOTS_OPTION_HEIGHT       28

static vec4_t addBotsTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t addBotsMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t addBotsAccentColor = UI_FRONTEND_COLOR_ACCENT;


typedef struct {
	menuframework_s	menu;
	menubitmap_s	arrows;
	menubitmap_s	up;
	menubitmap_s	down;
	menutext_s		bots[7];
	menulist_s		skill;
	menulist_s		team;
	menutext_s	go;
	menutext_s	back;

	int				numBots;
	int				delay;
	int				baseBotNum;
	int				selectedBotNum;
	int				sortedBotNums[MAX_BOTS];
	char			botnames[7][32];
} addBotsMenuInfo_t;

static addBotsMenuInfo_t	addBotsMenuInfo;

static void UI_AddBotsMenu_DrawAction( void *self ) {
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
	case ID_GO:
		label = "Add bot";
		break;
	case ID_BACK:
		label = "Back";
		break;
	}
	textColor = disabled ? addBotsMutedColor :
		( focus ? addBotsAccentColor : addBotsTextColor );

	Frontend_DrawCard( item->left, item->top,
		item->right - item->left, item->bottom - item->top,
		1.0f, focus );
	Frontend_DrawText( ( item->left + item->right ) / 2,
		item->top + 5, label, UI_CENTER | UI_SMALLFONT, textColor );
}

static void UI_AddBotsMenu_DrawBot( void *self ) {
	menutext_s *bot;
	int index;
	int y;
	qboolean focus;
	qboolean selected;
	const char *name;
	const float *textColor;

	bot = (menutext_s *)self;
	index = bot->generic.id - ID_BOTNAME0;
	y = ADDBOTS_BOT_ROW_Y + index *
		( ADDBOTS_BOT_ROW_HEIGHT + ADDBOTS_BOT_ROW_GAP );
	focus = ( Menu_ItemAtCursor( bot->generic.parent ) == bot );
	selected = ( index == addBotsMenuInfo.selectedBotNum );
	name = bot->string && bot->string[0] ? bot->string : "Unknown bot";
	textColor = focus || selected ? addBotsAccentColor : addBotsTextColor;

	Frontend_DrawCard( ADDBOTS_BOT_ROW_X, y, ADDBOTS_BOT_ROW_WIDTH,
		ADDBOTS_BOT_ROW_HEIGHT, 1.0f, focus || selected );
	Frontend_DrawText( ADDBOTS_BOT_ROW_X + 12, y + 4, name,
		UI_LEFT | UI_SMALLFONT, textColor );
}

static void UI_AddBotsMenu_DrawOption( void *self ) {
	menulist_s *list;
	qboolean focus;
	qboolean disabled;
	const char *value;
	const float *textColor;

	list = (menulist_s *)self;
	focus = ( Menu_ItemAtCursor( list->generic.parent ) == &list->generic );
	disabled = ( list->generic.flags & QMF_GRAYED ) ? qtrue : qfalse;
	value = "";
	if ( list->itemnames && list->curvalue >= 0 &&
		list->itemnames[list->curvalue] ) {
		value = list->itemnames[list->curvalue];
	}
	textColor = disabled ? addBotsMutedColor :
		( focus ? addBotsAccentColor : addBotsTextColor );

	Frontend_DrawCard( list->generic.left, list->generic.top,
		list->generic.right - list->generic.left,
		list->generic.bottom - list->generic.top, 1.0f, focus );
	Frontend_DrawText( list->generic.left + 10,
		list->generic.top + 6, list->generic.name,
		UI_LEFT | UI_SMALLFONT, addBotsMutedColor );
	Frontend_DrawText( list->generic.right - 10,
		list->generic.top + 6, value,
		UI_RIGHT | UI_SMALLFONT, textColor );
}

static void UI_AddBotsMenu_Layout( int count ) {
	int n;

	addBotsMenuInfo.arrows.generic.flags |= QMF_HIDDEN | QMF_INACTIVE;
	addBotsMenuInfo.up.generic.left = ADDBOTS_BOT_ROW_X;
	addBotsMenuInfo.up.generic.top = ADDBOTS_NAV_Y;
	addBotsMenuInfo.up.generic.right = ADDBOTS_BOT_ROW_X + ADDBOTS_NAV_WIDTH;
	addBotsMenuInfo.up.generic.bottom = ADDBOTS_NAV_Y + ADDBOTS_NAV_HEIGHT;
	addBotsMenuInfo.up.generic.x = ( addBotsMenuInfo.up.generic.left +
		addBotsMenuInfo.up.generic.right ) / 2;
	addBotsMenuInfo.up.generic.y = ADDBOTS_NAV_Y + 4;
	addBotsMenuInfo.up.generic.flags |= QMF_NODEFAULTINIT;
	addBotsMenuInfo.up.generic.ownerdraw = UI_AddBotsMenu_DrawAction;

	addBotsMenuInfo.down.generic.left = ADDBOTS_BOT_ROW_X +
		ADDBOTS_BOT_ROW_WIDTH - ADDBOTS_NAV_WIDTH;
	addBotsMenuInfo.down.generic.top = ADDBOTS_NAV_Y;
	addBotsMenuInfo.down.generic.right = ADDBOTS_BOT_ROW_X + ADDBOTS_BOT_ROW_WIDTH;
	addBotsMenuInfo.down.generic.bottom = ADDBOTS_NAV_Y + ADDBOTS_NAV_HEIGHT;
	addBotsMenuInfo.down.generic.x = ( addBotsMenuInfo.down.generic.left +
		addBotsMenuInfo.down.generic.right ) / 2;
	addBotsMenuInfo.down.generic.y = ADDBOTS_NAV_Y + 4;
	addBotsMenuInfo.down.generic.flags |= QMF_NODEFAULTINIT;
	addBotsMenuInfo.down.generic.ownerdraw = UI_AddBotsMenu_DrawAction;

	for ( n = 0; n < count; n++ ) {
		addBotsMenuInfo.bots[n].generic.left = ADDBOTS_BOT_ROW_X;
		addBotsMenuInfo.bots[n].generic.top = ADDBOTS_BOT_ROW_Y + n *
			( ADDBOTS_BOT_ROW_HEIGHT + ADDBOTS_BOT_ROW_GAP );
		addBotsMenuInfo.bots[n].generic.right = ADDBOTS_BOT_ROW_X +
			ADDBOTS_BOT_ROW_WIDTH;
		addBotsMenuInfo.bots[n].generic.bottom =
			addBotsMenuInfo.bots[n].generic.top + ADDBOTS_BOT_ROW_HEIGHT;
		addBotsMenuInfo.bots[n].generic.x = ADDBOTS_BOT_ROW_X +
			ADDBOTS_BOT_ROW_WIDTH / 2;
		addBotsMenuInfo.bots[n].generic.y =
			addBotsMenuInfo.bots[n].generic.top + 4;
		addBotsMenuInfo.bots[n].generic.flags |= QMF_NODEFAULTINIT;
		addBotsMenuInfo.bots[n].generic.ownerdraw = UI_AddBotsMenu_DrawBot;
	}

	addBotsMenuInfo.skill.generic.left = ADDBOTS_OPTION_X;
	addBotsMenuInfo.skill.generic.top = 168;
	addBotsMenuInfo.skill.generic.right = ADDBOTS_OPTION_X +
		ADDBOTS_OPTION_WIDTH;
	addBotsMenuInfo.skill.generic.bottom = 168 + ADDBOTS_OPTION_HEIGHT;
	addBotsMenuInfo.skill.generic.x = ADDBOTS_OPTION_X +
		ADDBOTS_OPTION_WIDTH / 2;
	addBotsMenuInfo.skill.generic.y = 174;
	addBotsMenuInfo.skill.generic.flags |= QMF_NODEFAULTINIT;
	addBotsMenuInfo.skill.generic.ownerdraw = UI_AddBotsMenu_DrawOption;

	addBotsMenuInfo.team.generic.left = ADDBOTS_OPTION_X;
	addBotsMenuInfo.team.generic.top = 220;
	addBotsMenuInfo.team.generic.right = ADDBOTS_OPTION_X +
		ADDBOTS_OPTION_WIDTH;
	addBotsMenuInfo.team.generic.bottom = 220 + ADDBOTS_OPTION_HEIGHT;
	addBotsMenuInfo.team.generic.x = ADDBOTS_OPTION_X +
		ADDBOTS_OPTION_WIDTH / 2;
	addBotsMenuInfo.team.generic.y = 226;
	addBotsMenuInfo.team.generic.flags |= QMF_NODEFAULTINIT;
	addBotsMenuInfo.team.generic.ownerdraw = UI_AddBotsMenu_DrawOption;

	addBotsMenuInfo.go.generic.left = ADDBOTS_FRAME_X + ADDBOTS_FRAME_WIDTH -
		16 - ADDBOTS_ACTION_WIDTH;
	addBotsMenuInfo.go.generic.top = ADDBOTS_ACTION_Y;
	addBotsMenuInfo.go.generic.right = addBotsMenuInfo.go.generic.left +
		ADDBOTS_ACTION_WIDTH;
	addBotsMenuInfo.go.generic.bottom = ADDBOTS_ACTION_Y +
		ADDBOTS_ACTION_HEIGHT;
	addBotsMenuInfo.go.generic.x = ( addBotsMenuInfo.go.generic.left +
		addBotsMenuInfo.go.generic.right ) / 2;
	addBotsMenuInfo.go.generic.y = ADDBOTS_ACTION_Y + 4;
	addBotsMenuInfo.go.generic.flags |= QMF_NODEFAULTINIT;
	addBotsMenuInfo.go.generic.ownerdraw = UI_AddBotsMenu_DrawAction;

	addBotsMenuInfo.back.generic.left = ADDBOTS_FRAME_X + 16;
	addBotsMenuInfo.back.generic.top = ADDBOTS_ACTION_Y;
	addBotsMenuInfo.back.generic.right = addBotsMenuInfo.back.generic.left +
		ADDBOTS_ACTION_WIDTH;
	addBotsMenuInfo.back.generic.bottom = ADDBOTS_ACTION_Y +
		ADDBOTS_ACTION_HEIGHT;
	addBotsMenuInfo.back.generic.x = ( addBotsMenuInfo.back.generic.left +
		addBotsMenuInfo.back.generic.right ) / 2;
	addBotsMenuInfo.back.generic.y = ADDBOTS_ACTION_Y + 4;
	addBotsMenuInfo.back.generic.flags |= QMF_NODEFAULTINIT;
	addBotsMenuInfo.back.generic.ownerdraw = UI_AddBotsMenu_DrawAction;
}


/*
=================
UI_AddBotsMenu_FightEvent
=================
*/
static void UI_AddBotsMenu_FightEvent( void* ptr, int event ) {
	const char	*team;
	int			skill;

	if (event != QM_ACTIVATED) {
		return;
	}

	team = addBotsMenuInfo.team.itemnames[addBotsMenuInfo.team.curvalue];
	skill = addBotsMenuInfo.skill.curvalue + 1;

	trap_Cmd_ExecuteText( EXEC_APPEND, va("addbot %s %i %s %i\n",
		addBotsMenuInfo.botnames[addBotsMenuInfo.selectedBotNum], skill, team, addBotsMenuInfo.delay) );

	addBotsMenuInfo.delay += 1500;
}


/*
=================
UI_AddBotsMenu_BotEvent
=================
*/
static void UI_AddBotsMenu_BotEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	addBotsMenuInfo.bots[addBotsMenuInfo.selectedBotNum].color = color_orange;
	addBotsMenuInfo.selectedBotNum = ((menucommon_s*)ptr)->id - ID_BOTNAME0;
	addBotsMenuInfo.bots[addBotsMenuInfo.selectedBotNum].color = color_white;
}


/*
=================
UI_AddBotsMenu_BackEvent
=================
*/
static void UI_AddBotsMenu_BackEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}
	UI_PopMenu();
}


/*
=================
UI_AddBotsMenu_SetBotNames
=================
*/
static void UI_AddBotsMenu_SetBotNames( void ) {
	int			n;
	const char	*info;

	for ( n = 0; n < 7; n++ ) {
		info = UI_GetBotInfoByNumber( addBotsMenuInfo.sortedBotNums[addBotsMenuInfo.baseBotNum + n] );
		Q_strncpyz( addBotsMenuInfo.botnames[n], Info_ValueForKey( info, "name" ), sizeof(addBotsMenuInfo.botnames[n]) );
	}

}


/*
=================
UI_AddBotsMenu_UpEvent
=================
*/
static void UI_AddBotsMenu_UpEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	if( addBotsMenuInfo.baseBotNum > 0 ) {
		addBotsMenuInfo.baseBotNum--;
		UI_AddBotsMenu_SetBotNames();
	}
}


/*
=================
UI_AddBotsMenu_DownEvent
=================
*/
static void UI_AddBotsMenu_DownEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	if( addBotsMenuInfo.baseBotNum + 7 < addBotsMenuInfo.numBots ) {
		addBotsMenuInfo.baseBotNum++;
		UI_AddBotsMenu_SetBotNames();
	}
}


/*
=================
UI_AddBotsMenu_GetSortedBotNums
=================
*/
static int QDECL UI_AddBotsMenu_SortCompare( const void *arg1, const void *arg2 ) {
	int			num1, num2;
	const char	*info1, *info2;
	const char	*name1, *name2;

	num1 = *(int *)arg1;
	num2 = *(int *)arg2;

	info1 = UI_GetBotInfoByNumber( num1 );
	info2 = UI_GetBotInfoByNumber( num2 );

	name1 = Info_ValueForKey( info1, "name" );
	name2 = Info_ValueForKey( info2, "name" );

	return Q_stricmp( name1, name2 );
}

static void UI_AddBotsMenu_GetSortedBotNums( void ) {
	int		n;

	// initialize the array
	for( n = 0; n < addBotsMenuInfo.numBots; n++ ) {
		addBotsMenuInfo.sortedBotNums[n] = n;
	}

	qsort( addBotsMenuInfo.sortedBotNums, addBotsMenuInfo.numBots, sizeof(addBotsMenuInfo.sortedBotNums[0]), UI_AddBotsMenu_SortCompare );
}


/*
=================
UI_AddBotsMenu_Draw
=================
*/
static void UI_AddBotsMenu_Draw( void ) {
	vec4_t overlayColor = UI_FRONTEND_COLOR_SCRIM;

	UI_SetColor( NULL );
	UI_FillRect( -uis.bias, 0, SCREEN_WIDTH + uis.bias * 2,
		SCREEN_HEIGHT, overlayColor );
	Frontend_DrawPanel( ADDBOTS_FRAME_X, ADDBOTS_FRAME_Y,
		ADDBOTS_FRAME_WIDTH, ADDBOTS_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( ADDBOTS_FRAME_X + 24, ADDBOTS_FRAME_Y + 24,
		"Add bots", UI_LEFT | UI_BIGFONT, addBotsTextColor );
	Frontend_DrawText( ADDBOTS_FRAME_X + 24, ADDBOTS_FRAME_Y + 48,
		"Choose a bot and configure its starting settings",
		UI_LEFT | UI_SMALLFONT, addBotsMutedColor );
	Frontend_DrawStatusChip( ADDBOTS_FRAME_X + ADDBOTS_FRAME_WIDTH - 128,
		ADDBOTS_FRAME_Y + 26, "Bot roster", addBotsAccentColor, 1.0f );

	Frontend_DrawCard( ADDBOTS_BOT_CARD_X, ADDBOTS_BOT_CARD_Y,
		ADDBOTS_BOT_CARD_WIDTH, ADDBOTS_BOT_CARD_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( ADDBOTS_BOT_CARD_X + 16, ADDBOTS_BOT_CARD_Y + 16,
		"Available bots", UI_LEFT | UI_SMALLFONT, addBotsMutedColor );
	Frontend_DrawCard( ADDBOTS_OPTION_CARD_X, ADDBOTS_OPTION_CARD_Y,
		ADDBOTS_OPTION_CARD_WIDTH, ADDBOTS_OPTION_CARD_HEIGHT,
		1.0f, qfalse );
	Frontend_DrawText( ADDBOTS_OPTION_CARD_X + 16,
		ADDBOTS_OPTION_CARD_Y + 16, "Bot settings",
		UI_LEFT | UI_SMALLFONT, addBotsMutedColor );
	Frontend_DrawText( ADDBOTS_FRAME_X + 24, ADDBOTS_FRAME_Y + 376,
		"Select a bot   Left / right adjust", UI_LEFT | UI_SMALLFONT,
		addBotsMutedColor );

	Menu_Draw( &addBotsMenuInfo.menu );
}

	
/*
=================
UI_AddBotsMenu_Init
=================
*/
static const char *skillNames[] = {
	"I Can Win",
	"Bring It On",
	"Hurt Me Plenty",
	"Hardcore",
	"Nightmare!",
	0
};

static const char *teamNames1[] = {
	"Free",
	0
};

static const char *teamNames2[] = {
	"Red",
	"Blue",
	0
};

// STONELANCE
static const char *teamNames4[] = {
	"Red",
	"Blue",
	"Green",
	"Yellow",
	0
};
// END

static void UI_AddBotsMenu_Init( void ) {
	int		n;
	int		y;
	int		gametype;
	int		count;
	char	info[MAX_INFO_STRING];

	trap_GetConfigString(CS_SERVERINFO, info, MAX_INFO_STRING);   
	gametype = atoi( Info_ValueForKey( info,"g_gametype" ) );

	memset( &addBotsMenuInfo, 0 ,sizeof(addBotsMenuInfo) );
	addBotsMenuInfo.menu.draw = UI_AddBotsMenu_Draw;
	addBotsMenuInfo.menu.fullscreen = qfalse;
	addBotsMenuInfo.menu.wrapAround = qtrue;
	addBotsMenuInfo.delay = 1000;

	UI_AddBots_Cache();

	addBotsMenuInfo.numBots = UI_GetNumBots();
	count = addBotsMenuInfo.numBots < 7 ? addBotsMenuInfo.numBots : 7;

	addBotsMenuInfo.arrows.generic.type  = MTYPE_BITMAP;
	addBotsMenuInfo.arrows.generic.name  = ART_ARROWS;
	addBotsMenuInfo.arrows.generic.flags = QMF_INACTIVE;
	addBotsMenuInfo.arrows.generic.x	 = 200;
	addBotsMenuInfo.arrows.generic.y	 = 128;
	addBotsMenuInfo.arrows.width  	     = 64;
	addBotsMenuInfo.arrows.height  	     = 128;

	addBotsMenuInfo.up.generic.type	    = MTYPE_BITMAP;
	addBotsMenuInfo.up.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	addBotsMenuInfo.up.generic.x		= 200;
	addBotsMenuInfo.up.generic.y		= 128;
	addBotsMenuInfo.up.generic.id	    = ID_UP;
	addBotsMenuInfo.up.generic.callback = UI_AddBotsMenu_UpEvent;
	addBotsMenuInfo.up.width  		    = 64;
	addBotsMenuInfo.up.height  		    = 64;
	addBotsMenuInfo.up.focuspic         = ART_ARROWUP;

	addBotsMenuInfo.down.generic.type	  = MTYPE_BITMAP;
	addBotsMenuInfo.down.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	addBotsMenuInfo.down.generic.x		  = 200;
	addBotsMenuInfo.down.generic.y		  = 128+64;
	addBotsMenuInfo.down.generic.id	      = ID_DOWN;
	addBotsMenuInfo.down.generic.callback = UI_AddBotsMenu_DownEvent;
	addBotsMenuInfo.down.width  		  = 64;
	addBotsMenuInfo.down.height  		  = 64;
	addBotsMenuInfo.down.focuspic         = ART_ARROWDOWN;

	for( n = 0, y = 120; n < count; n++, y += 20 ) {
		addBotsMenuInfo.bots[n].generic.type		= MTYPE_PTEXT;
		addBotsMenuInfo.bots[n].generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
		addBotsMenuInfo.bots[n].generic.id			= ID_BOTNAME0 + n;
		addBotsMenuInfo.bots[n].generic.x			= 320 - 56;
		addBotsMenuInfo.bots[n].generic.y			= y;
		addBotsMenuInfo.bots[n].generic.callback	= UI_AddBotsMenu_BotEvent;
		addBotsMenuInfo.bots[n].string				= addBotsMenuInfo.botnames[n];
		addBotsMenuInfo.bots[n].color				= color_orange;
		addBotsMenuInfo.bots[n].style				= UI_LEFT|UI_SMALLFONT;
	}

	y += 12;
	addBotsMenuInfo.skill.generic.type		= MTYPE_SPINCONTROL;
	addBotsMenuInfo.skill.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	addBotsMenuInfo.skill.generic.x			= 320;
	addBotsMenuInfo.skill.generic.y			= y;
	addBotsMenuInfo.skill.generic.name		= "Skill:";
	addBotsMenuInfo.skill.generic.id		= ID_SKILL;
	addBotsMenuInfo.skill.itemnames			= skillNames;
	addBotsMenuInfo.skill.curvalue			= Com_Clamp( 0, 4, (int)trap_Cvar_VariableValue( "g_spSkill" ) - 1 );

	y += SMALLCHAR_HEIGHT;
	addBotsMenuInfo.team.generic.type		= MTYPE_SPINCONTROL;
	addBotsMenuInfo.team.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	addBotsMenuInfo.team.generic.x			= 320;
	addBotsMenuInfo.team.generic.y			= y;
	addBotsMenuInfo.team.generic.name		= "Team: ";
	addBotsMenuInfo.team.generic.id			= ID_TEAM;
// STONELANCE
	if( gametype == GT_CTF ) {
		addBotsMenuInfo.team.itemnames		= teamNames2;
	}
// END
	if( gametype >= GT_TEAM ) {
// STONELANCE
//		addBotsMenuInfo.team.itemnames		= teamNames2;
		addBotsMenuInfo.team.itemnames		= teamNames4;
// END
	}
	else {
		addBotsMenuInfo.team.itemnames		= teamNames1;
		addBotsMenuInfo.team.generic.flags	= QMF_GRAYED;
	}

	addBotsMenuInfo.go.generic.type			   = MTYPE_PTEXT;
	addBotsMenuInfo.go.generic.flags		   = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	addBotsMenuInfo.go.generic.id			     = ID_GO;
	addBotsMenuInfo.go.generic.callback		 = UI_AddBotsMenu_FightEvent;
	addBotsMenuInfo.go.generic.x			     = 320+128;
	addBotsMenuInfo.go.generic.y			     = 256+128-64;
  addBotsMenuInfo.go.string					     = "GO";
	addBotsMenuInfo.go.color					     = text_color_normal;
	addBotsMenuInfo.go.style				       = UI_RIGHT | UI_SMALLFONT;

/*         to remove

  addBotsMenuInfo.go.generic.type			= MTYPE_BITMAP;
	addBotsMenuInfo.go.generic.name			= ART_FIGHT0;
	addBotsMenuInfo.go.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	addBotsMenuInfo.go.generic.id			= ID_GO;
	addBotsMenuInfo.go.generic.callback		= UI_AddBotsMenu_FightEvent;
	addBotsMenuInfo.go.generic.x			= 320+128-128;
	addBotsMenuInfo.go.generic.y			= 256+128-64;
	addBotsMenuInfo.go.width  				= 128;
	addBotsMenuInfo.go.height  				= 64;
	addBotsMenuInfo.go.focuspic				= ART_FIGHT1;

*/

	addBotsMenuInfo.back.generic.type		= MTYPE_PTEXT;
	addBotsMenuInfo.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	addBotsMenuInfo.back.generic.id			= ID_BACK;
	addBotsMenuInfo.back.generic.callback	= UI_AddBotsMenu_BackEvent;
	addBotsMenuInfo.back.generic.x			= 320-128;
	addBotsMenuInfo.back.generic.y			= 256+128-64;
  addBotsMenuInfo.back.string					= "< BACK";
	addBotsMenuInfo.back.color					= text_color_normal;
	addBotsMenuInfo.back.style					= UI_LEFT | UI_SMALLFONT;
	addBotsMenuInfo.baseBotNum = 0;
	addBotsMenuInfo.selectedBotNum = 0;
	addBotsMenuInfo.bots[0].color = color_white;

	UI_AddBotsMenu_GetSortedBotNums();
	UI_AddBotsMenu_SetBotNames();
	UI_AddBotsMenu_Layout( count );

	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.arrows );

	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.up );
	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.down );
	for( n = 0; n < count; n++ ) {
		Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.bots[n] );
	}
	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.skill );
	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.team );
	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.go );
	Menu_AddItem( &addBotsMenuInfo.menu, &addBotsMenuInfo.back );
}


/*
=================
UI_AddBots_Cache
=================
*/
void UI_AddBots_Cache( void ) {
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
	trap_R_RegisterShaderNoMip( ART_FIGHT0 );
	trap_R_RegisterShaderNoMip( ART_FIGHT1 );
	trap_R_RegisterShaderNoMip( ART_BACKGROUND );
	trap_R_RegisterShaderNoMip( ART_ARROWS );
	trap_R_RegisterShaderNoMip( ART_ARROWUP );
	trap_R_RegisterShaderNoMip( ART_ARROWDOWN );
}


/*
=================
UI_AddBotsMenu
=================
*/
void UI_AddBotsMenu( void ) {
	UI_AddBotsMenu_Init();
	UI_PushMenu( &addBotsMenuInfo.menu );
}
