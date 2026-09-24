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

NETWORK OPTIONS MENU

=======================================================================
*/

#include "ui_local.h"
#include "ui_rally_frontend.h"


// STONELANCE
/*
#define ART_FRAMEL			"menu/art/frame2_l"
#define ART_FRAMER			"menu/art/frame1_r"
#define ART_BACK0			"menu/art/back_0"
#define ART_BACK1			"menu/art/back_1"
*/
// END

#define ID_GRAPHICS			10
#define ID_ADVANCED_GRAPHICS	17
#define ID_DISPLAY			11
#define ID_SOUND			12
#define ID_NETWORK			13
#define ID_RATE				14
#define ID_BACK				15

#define NETWORK_FRAME_X             24
#define NETWORK_FRAME_Y             20
#define NETWORK_FRAME_WIDTH         592
#define NETWORK_FRAME_HEIGHT        440
#define NETWORK_NAV_X               40
#define NETWORK_NAV_Y               104
#define NETWORK_NAV_WIDTH           164
#define NETWORK_NAV_HEIGHT          292
#define NETWORK_DETAIL_X            40
#define NETWORK_DETAIL_Y            104
#define NETWORK_DETAIL_WIDTH        556
#define NETWORK_DETAIL_HEIGHT       292
#define NETWORK_ROW_HEIGHT          24
#define NETWORK_ROW_GAP             4
#define NETWORK_ROW_START_Y         148
#define NETWORK_COLUMN_X            236
#define NETWORK_COLUMN_WIDTH        344
#define NETWORK_ACTION_Y            420
#define NETWORK_ACTION_WIDTH        120
#define NETWORK_ACTION_HEIGHT       24

static vec4_t networkScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t networkAccentColor = UI_FRONTEND_COLOR_ACCENT;
static vec4_t networkTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t networkMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t networkBorderColor = UI_FRONTEND_COLOR_BORDER;
static vec4_t networkFocusColor = UI_FRONTEND_COLOR_FOCUS_BG;


static const char *rate_items[] = {
	"<= 28.8K",
	"33.6K",
	"56K",
	"ISDN",
	"LAN/Cable/xDSL",
	0
};

typedef struct {
	menuframework_s	menu;

	menutext_s		banner;
// STONELANCE
/*
	menubitmap_s	framel;
	menubitmap_s	framer;
*/
// END

	menutext_s		graphics;
	menutext_s		advanced_graphics;
	menutext_s		display;
	menutext_s		sound;
	menutext_s		network;

	menulist_s		rate;

// STONELANCE
//	menubitmap_s	back;
	menutext_s		back;
// END
} networkOptionsInfo_t;

static networkOptionsInfo_t	networkOptionsInfo;

static void UI_NetworkOptionsMenu_Event( void *ptr, int event );

static const char *Network_ListValue( menulist_s *item ) {
	int i;

	if ( !item->itemnames || item->curvalue < 0 ) {
		return "-";
	}

	for ( i = 0; item->itemnames[i]; i++ ) {
		if ( i == item->curvalue ) {
			return item->itemnames[i];
		}
	}

	return "-";
}

static void Network_DrawNavItem( void *self ) {
	menutext_s *text;
	menucommon_s *item;
	qboolean focus;
	qboolean active;

	text = (menutext_s *)self;
	item = &text->generic;
	focus = ( Menu_ItemAtCursor( item->parent ) == item );
	active = ( item->id == ID_NETWORK );
	Frontend_DrawNavButton( item->left, item->top,
		item->right - item->left, item->bottom - item->top,
		text->string, 1.0f, active || focus, UI_LEFT );
}

static void Network_DrawSetting( void *self ) {
	menucommon_s *item;
	const char *value;
	qboolean disabled;
	qboolean focus;
	qboolean hovered;
	qboolean highlighted;
	vec4_t fillColor;
	vec4_t lineColor;
	vec4_t labelColor;
	vec4_t valueColor;

	item = (menucommon_s *)self;
	disabled = ( item->flags & ( QMF_GRAYED | QMF_INACTIVE ) ) ? qtrue : qfalse;
	focus = ( !disabled && Menu_ItemAtCursor( item->parent ) == item ) ? qtrue : qfalse;
	hovered = ( !disabled && uis.cursorx >= item->left &&
		uis.cursorx <= item->right && uis.cursory >= item->top &&
		uis.cursory <= item->bottom ) ? qtrue : qfalse;
	highlighted = ( focus || hovered ) ? qtrue : qfalse;
	value = Network_ListValue( (menulist_s *)item );

	Vector4Copy( networkFocusColor, fillColor );
	fillColor[3] = highlighted ? 0.72f : 0.0f;
	Vector4Copy( networkBorderColor, lineColor );
	lineColor[3] = 0.55f;
	if ( highlighted ) {
		UI_FillRect( item->left, item->top,
			item->right - item->left, item->bottom - item->top, fillColor );
		Vector4Copy( networkAccentColor, lineColor );
	}
	UI_FillRect( item->left, item->bottom - 1,
		item->right - item->left, 1, lineColor );

	if ( disabled ) {
		Vector4Copy( networkMutedColor, labelColor );
		labelColor[3] = 0.65f;
		Vector4Copy( labelColor, valueColor );
	} else if ( highlighted ) {
		Vector4Copy( networkAccentColor, labelColor );
		Vector4Copy( networkTextColor, valueColor );
	} else {
		Vector4Copy( networkMutedColor, labelColor );
		Vector4Copy( networkTextColor, valueColor );
	}

	Frontend_DrawText( item->left + UI_FRONTEND_SPACE_SM,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		item->name, UI_LEFT | UI_SMALLFONT, labelColor );
	Frontend_DrawText( item->right - UI_FRONTEND_SPACE_SM,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		value, UI_RIGHT | UI_SMALLFONT, valueColor );
}

static void Network_DrawAction( void *self ) {
	menutext_s *text;
	menucommon_s *item;
	qboolean focus;

	text = (menutext_s *)self;
	item = &text->generic;
	focus = ( Menu_ItemAtCursor( item->parent ) == item );
	Frontend_DrawButton( item->left, item->top,
		item->right - item->left, item->bottom - item->top,
		text->string, 1.0f, focus, UI_CENTER );
}

static void Network_SetBounds( menucommon_s *item, int id,
	int x, int y, int width, int height, const char *label ) {
	item->id = id;
	item->x = x;
	item->y = y;
	item->left = x;
	item->top = y;
	item->right = x + width;
	item->bottom = y + height;
	if ( label ) {
		item->name = (char *)label;
	}
}

static void Network_SetNavBounds( menutext_s *item, int id,
	const char *label, int y ) {
	item->generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
	item->generic.callback = UI_NetworkOptionsMenu_Event;
	item->string = (char *)label;
	item->style = UI_LEFT | UI_SMALLFONT;
	item->generic.ownerdraw = Network_DrawNavItem;
	Network_SetBounds( &item->generic, id, NETWORK_NAV_X + 16, y,
		NETWORK_NAV_WIDTH - 32, NETWORK_ROW_HEIGHT, NULL );
}

static void Network_SetSettingBounds( menulist_s *item, int id,
	const char *label, int y ) {
	item->generic.ownerdraw = Network_DrawSetting;
	Network_SetBounds( &item->generic, id, NETWORK_COLUMN_X, y,
		NETWORK_COLUMN_WIDTH, NETWORK_ROW_HEIGHT, label );
}

static void UI_NetworkOptionsMenu_Draw( void ) {
	Frontend_DrawBackground( networkScrimColor );
	Frontend_DrawPanel( NETWORK_FRAME_X, NETWORK_FRAME_Y,
		NETWORK_FRAME_WIDTH, NETWORK_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( NETWORK_FRAME_X + 24, NETWORK_FRAME_Y + 24,
		"Network", UI_LEFT | UI_BIGFONT, networkTextColor );
	Frontend_DrawText( NETWORK_FRAME_X + 24, NETWORK_FRAME_Y + 48,
		"Tune connection rate and server behavior",
		UI_LEFT | UI_SMALLFONT, networkMutedColor );
	Frontend_DrawStatusChip( NETWORK_FRAME_X + NETWORK_FRAME_WIDTH - 104,
		NETWORK_FRAME_Y + 26, "Settings", networkAccentColor, 1.0f );

	Frontend_DrawCard( NETWORK_DETAIL_X, NETWORK_DETAIL_Y,
		NETWORK_DETAIL_WIDTH, NETWORK_DETAIL_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( NETWORK_DETAIL_X + 16, NETWORK_DETAIL_Y + 22,
		"Connection", UI_LEFT | UI_SMALLFONT, networkMutedColor );

	Menu_Draw( &networkOptionsInfo.menu );

	Frontend_DrawText( NETWORK_FRAME_X + 24, NETWORK_FRAME_Y + 384,
		"Select an option   Left / right adjust   Esc back",
		UI_LEFT | UI_SMALLFONT, networkMutedColor );
}


/*
=================
UI_NetworkOptionsMenu_Event
=================
*/
static void UI_NetworkOptionsMenu_Event( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_RATE:
		if( networkOptionsInfo.rate.curvalue == 0 ) {
			trap_Cvar_SetValue( "rate", 2500 );
		}
		else if( networkOptionsInfo.rate.curvalue == 1 ) {
			trap_Cvar_SetValue( "rate", 3000 );
		}
		else if( networkOptionsInfo.rate.curvalue == 2 ) {
			trap_Cvar_SetValue( "rate", 4000 );
		}
		else if( networkOptionsInfo.rate.curvalue == 3 ) {
			trap_Cvar_SetValue( "rate", 5000 );
		}
		else if( networkOptionsInfo.rate.curvalue == 4 ) {
			trap_Cvar_SetValue( "rate", 25000 );
		}
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}


/*
===============
UI_NetworkOptionsMenu_Init
===============
*/
static void UI_NetworkOptionsMenu_Init( void ) {
	int		y;
	int		rate;

	memset( &networkOptionsInfo, 0, sizeof(networkOptionsInfo) );

	UI_NetworkOptionsMenu_Cache();
	networkOptionsInfo.menu.wrapAround = qtrue;
	networkOptionsInfo.menu.fullscreen = qtrue;
	networkOptionsInfo.menu.draw = UI_NetworkOptionsMenu_Draw;

	networkOptionsInfo.banner.generic.type		= MTYPE_BTEXT;
	networkOptionsInfo.banner.generic.flags		= QMF_CENTER_JUSTIFY;
	networkOptionsInfo.banner.generic.x			= 320;
	networkOptionsInfo.banner.generic.y			= 16;
	networkOptionsInfo.banner.string			= "SYSTEM SETUP";
	networkOptionsInfo.banner.color				= color_white;
	networkOptionsInfo.banner.style				= UI_CENTER;

// STONELANCE
/*
	networkOptionsInfo.framel.generic.type		= MTYPE_BITMAP;
	networkOptionsInfo.framel.generic.name		= ART_FRAMEL;
	networkOptionsInfo.framel.generic.flags		= QMF_INACTIVE;
	networkOptionsInfo.framel.generic.x			= 0;  
	networkOptionsInfo.framel.generic.y			= 78;
	networkOptionsInfo.framel.width				= 256;
	networkOptionsInfo.framel.height			= 329;

	networkOptionsInfo.framer.generic.type		= MTYPE_BITMAP;
	networkOptionsInfo.framer.generic.name		= ART_FRAMER;
	networkOptionsInfo.framer.generic.flags		= QMF_INACTIVE;
	networkOptionsInfo.framer.generic.x			= 376;
	networkOptionsInfo.framer.generic.y			= 76;
	networkOptionsInfo.framer.width				= 256;
	networkOptionsInfo.framer.height			= 334;
*/
// END

	networkOptionsInfo.graphics.generic.type		= MTYPE_PTEXT;
	networkOptionsInfo.graphics.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	networkOptionsInfo.graphics.generic.id			= ID_GRAPHICS;
	networkOptionsInfo.graphics.generic.callback	= UI_NetworkOptionsMenu_Event;
	networkOptionsInfo.graphics.generic.x			= 216;
	networkOptionsInfo.graphics.generic.y			= 240 - 2 * PROP_HEIGHT;
	networkOptionsInfo.graphics.string				= "GRAPHICS";
	networkOptionsInfo.graphics.style				= UI_RIGHT;
// BAGPUSS
//	networkOptionsInfo.graphics.color				= color_red;
	networkOptionsInfo.graphics.color				= text_color_normal;
// END


	networkOptionsInfo.advanced_graphics.generic.type			= MTYPE_PTEXT;
	networkOptionsInfo.advanced_graphics.generic.flags			= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	networkOptionsInfo.advanced_graphics.generic.id			= ID_ADVANCED_GRAPHICS;
	networkOptionsInfo.advanced_graphics.generic.callback		= UI_NetworkOptionsMenu_Event;
	networkOptionsInfo.advanced_graphics.generic.x				= 216;
	networkOptionsInfo.advanced_graphics.generic.y				= 240 - PROP_HEIGHT;
	networkOptionsInfo.advanced_graphics.string					= "ADVANCED GRAPHICS";
	networkOptionsInfo.advanced_graphics.style					= UI_RIGHT;
	networkOptionsInfo.advanced_graphics.color					= text_color_normal;

	networkOptionsInfo.display.generic.type			= MTYPE_PTEXT;
	networkOptionsInfo.display.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	networkOptionsInfo.display.generic.id			= ID_DISPLAY;
	networkOptionsInfo.display.generic.callback		= UI_NetworkOptionsMenu_Event;
	networkOptionsInfo.display.generic.x			= 216;
	networkOptionsInfo.display.generic.y			= 240;
	networkOptionsInfo.display.string				= "DISPLAY";
	networkOptionsInfo.display.style				= UI_RIGHT;
// BAGPUSS
//	networkOptionsInfo.display.color				= color_red;
	networkOptionsInfo.display.color				= text_color_normal;
// END

	networkOptionsInfo.sound.generic.type			= MTYPE_PTEXT;
	networkOptionsInfo.sound.generic.flags			= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	networkOptionsInfo.sound.generic.id				= ID_SOUND;
	networkOptionsInfo.sound.generic.callback		= UI_NetworkOptionsMenu_Event;
	networkOptionsInfo.sound.generic.x				= 216;
	networkOptionsInfo.sound.generic.y				= 240 + PROP_HEIGHT;
	networkOptionsInfo.sound.string					= "SOUND";
	networkOptionsInfo.sound.style					= UI_RIGHT;
	networkOptionsInfo.sound.color					= color_red;
// BAGPUSS
//	networkOptionsInfo.sound.color					= color_red;
	networkOptionsInfo.sound.color					= text_color_normal;
// END

	networkOptionsInfo.network.generic.type			= MTYPE_PTEXT;
	networkOptionsInfo.network.generic.flags		= QMF_RIGHT_JUSTIFY;
	networkOptionsInfo.network.generic.id			= ID_NETWORK;
	networkOptionsInfo.network.generic.callback		= UI_NetworkOptionsMenu_Event;
	networkOptionsInfo.network.generic.x			= 216;
	networkOptionsInfo.network.generic.y			= 240 + 2 * PROP_HEIGHT;
	networkOptionsInfo.network.string				= "NETWORK";
	networkOptionsInfo.network.style				= UI_RIGHT;
// BAGPUSS
//	networkOptionsInfo.network.color				= color_red;
	networkOptionsInfo.network.color				= text_color_normal;
// END

	y = 240 - 1 * (BIGCHAR_HEIGHT+2);
	networkOptionsInfo.rate.generic.type		= MTYPE_SPINCONTROL;
	networkOptionsInfo.rate.generic.name		= "Data Rate:";
	networkOptionsInfo.rate.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	networkOptionsInfo.rate.generic.callback	= UI_NetworkOptionsMenu_Event;
	networkOptionsInfo.rate.generic.id			= ID_RATE;
	networkOptionsInfo.rate.generic.x			= 400;
	networkOptionsInfo.rate.generic.y			= y;
	networkOptionsInfo.rate.itemnames			= rate_items;

// STONELANCE
/*
	networkOptionsInfo.back.generic.type		= MTYPE_BITMAP;
	networkOptionsInfo.back.generic.name		= ART_BACK0;
	networkOptionsInfo.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	networkOptionsInfo.back.generic.callback	= UI_NetworkOptionsMenu_Event;
	networkOptionsInfo.back.generic.id			= ID_BACK;
	networkOptionsInfo.back.generic.x			= 0;
	networkOptionsInfo.back.generic.y			= 480-64;
	networkOptionsInfo.back.width				= 128;
	networkOptionsInfo.back.height				= 64;
	networkOptionsInfo.back.focuspic			= ART_BACK1;
*/
	networkOptionsInfo.back.generic.type			= MTYPE_PTEXT;
	networkOptionsInfo.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	networkOptionsInfo.back.generic.x				= 20;
	networkOptionsInfo.back.generic.y				= 480 - 50;
	networkOptionsInfo.back.generic.id				= ID_BACK;
	networkOptionsInfo.back.generic.callback		= UI_NetworkOptionsMenu_Event; 
	networkOptionsInfo.back.string					= "< BACK";
	networkOptionsInfo.back.color					= text_color_normal;
	networkOptionsInfo.back.style					= UI_LEFT | UI_SMALLFONT;
// END

// STONELANCE
/*
	Menu_AddItem( &networkOptionsInfo.menu, ( void * ) &networkOptionsInfo.framel );
	Menu_AddItem( &networkOptionsInfo.menu, ( void * ) &networkOptionsInfo.framer );
*/
// END
	Menu_AddItem( &networkOptionsInfo.menu, ( void * ) &networkOptionsInfo.rate );
	Menu_AddItem( &networkOptionsInfo.menu, ( void * ) &networkOptionsInfo.back );

	rate = trap_Cvar_VariableValue( "rate" );
	if( rate <= 2500 ) {
		networkOptionsInfo.rate.curvalue = 0;
	}
	else if( rate <= 3000 ) {
		networkOptionsInfo.rate.curvalue = 1;
	}
	else if( rate <= 4000 ) {
		networkOptionsInfo.rate.curvalue = 2;
	}
	else if( rate <= 5000 ) {
		networkOptionsInfo.rate.curvalue = 3;
	}
	else {
		networkOptionsInfo.rate.curvalue = 4;
	}

	/* Menu_AddItem initializes the legacy widgets and overwrites their
	 * default bounds. Apply the frontend layout after that initialization. */
	Network_SetSettingBounds( &networkOptionsInfo.rate,
		ID_RATE, "Data rate", NETWORK_ROW_START_Y );
	Network_SetBounds( &networkOptionsInfo.back.generic, ID_BACK,
		NETWORK_NAV_X, NETWORK_ACTION_Y, NETWORK_ACTION_WIDTH,
		NETWORK_ACTION_HEIGHT, NULL );
	networkOptionsInfo.back.generic.ownerdraw = Network_DrawAction;
}


/*
===============
UI_NetworkOptionsMenu_Cache
===============
*/
void UI_NetworkOptionsMenu_Cache( void ) {
// STONELANCE
/*
	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
*/
// END
}


/*
===============
UI_NetworkOptionsMenu
===============
*/
void UI_NetworkOptionsMenu( void ) {
// STONELANCE FIXME: get rid of this after proper tansitions are added
	uis.transitionIn = 0;
// END

	UI_NetworkOptionsMenu_Init();
	UI_PushMenu( &networkOptionsInfo.menu );
	Menu_SetCursorToItem( &networkOptionsInfo.menu, &networkOptionsInfo.rate );
}
