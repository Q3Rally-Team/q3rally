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

DISPLAY OPTIONS MENU

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
#define ID_BRIGHTNESS		14
#define ID_SCREENSIZE		15
#define ID_BACK				16

#define DISPLAY_FRAME_X             24
#define DISPLAY_FRAME_Y             20
#define DISPLAY_FRAME_WIDTH         592
#define DISPLAY_FRAME_HEIGHT        440
#define DISPLAY_NAV_X               40
#define DISPLAY_NAV_Y               104
#define DISPLAY_NAV_WIDTH           164
#define DISPLAY_NAV_HEIGHT          292
#define DISPLAY_DETAIL_X            40
#define DISPLAY_DETAIL_Y            104
#define DISPLAY_DETAIL_WIDTH        556
#define DISPLAY_DETAIL_HEIGHT       292
#define DISPLAY_ROW_HEIGHT          24
#define DISPLAY_ROW_GAP             4
#define DISPLAY_ROW_START_Y         148
#define DISPLAY_COLUMN_X            236
#define DISPLAY_COLUMN_WIDTH        344
#define DISPLAY_ACTION_Y            420
#define DISPLAY_ACTION_WIDTH        120
#define DISPLAY_ACTION_HEIGHT       24

static vec4_t displayScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t displayAccentColor = UI_FRONTEND_COLOR_ACCENT;
static vec4_t displayTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t displayMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t displayBorderColor = UI_FRONTEND_COLOR_BORDER;
static vec4_t displayFocusColor = UI_FRONTEND_COLOR_FOCUS_BG;


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

	menuslider_s	brightness;
	menuslider_s	screensize;

// STONELANCE
//	menubitmap_s	back;
	menutext_s		back;
// END
} displayOptionsInfo_t;

static displayOptionsInfo_t	displayOptionsInfo;

static void UI_DisplayOptionsMenu_Event( void *ptr, int event );

static void Display_DrawNavItem( void *self ) {
	menutext_s *text;
	menucommon_s *item;
	qboolean focus;
	qboolean active;

	text = (menutext_s *)self;
	item = &text->generic;
	focus = ( Menu_ItemAtCursor( item->parent ) == item );
	active = ( item->id == ID_DISPLAY );
	Frontend_DrawNavButton( item->left, item->top,
		item->right - item->left, item->bottom - item->top,
		text->string, 1.0f, active || focus, UI_LEFT );
}

static void Display_DrawSlider( void *self ) {
	menuslider_s *slider;
	menucommon_s *item;
	qboolean disabled;
	qboolean focus;
	qboolean hovered;
	qboolean highlighted;
	vec4_t fillColor;
	vec4_t lineColor;
	vec4_t labelColor;
	vec4_t valueColor;
	float range;
	int trackX;
	int trackWidth;
	int trackY;
	char value[32];

	slider = (menuslider_s *)self;
	item = &slider->generic;
	disabled = ( item->flags & ( QMF_GRAYED | QMF_INACTIVE ) ) ? qtrue : qfalse;
	focus = ( !disabled && Menu_ItemAtCursor( item->parent ) == item ) ? qtrue : qfalse;
	hovered = ( !disabled && uis.cursorx >= item->left &&
		uis.cursorx <= item->right && uis.cursory >= item->top &&
		uis.cursory <= item->bottom ) ? qtrue : qfalse;
	highlighted = ( focus || hovered ) ? qtrue : qfalse;

	Vector4Copy( displayFocusColor, fillColor );
	fillColor[3] = highlighted ? 0.72f : 0.0f;
	Vector4Copy( displayBorderColor, lineColor );
	lineColor[3] = 0.55f;
	if ( highlighted ) {
		UI_FillRect( item->left, item->top,
			item->right - item->left, item->bottom - item->top, fillColor );
		Vector4Copy( displayAccentColor, lineColor );
	}
	UI_FillRect( item->left, item->bottom - 1,
		item->right - item->left, 1, lineColor );

	if ( disabled ) {
		Vector4Copy( displayMutedColor, labelColor );
		labelColor[3] = 0.65f;
		Vector4Copy( labelColor, valueColor );
	} else if ( highlighted ) {
		Vector4Copy( displayAccentColor, labelColor );
		Vector4Copy( displayTextColor, valueColor );
	} else {
		Vector4Copy( displayMutedColor, labelColor );
		Vector4Copy( displayTextColor, valueColor );
	}

	if ( slider->maxvalue > slider->minvalue ) {
		range = ( slider->curvalue - slider->minvalue ) /
			( slider->maxvalue - slider->minvalue );
	} else {
		range = 0.0f;
	}
	if ( range < 0.0f ) range = 0.0f;
	if ( range > 1.0f ) range = 1.0f;

	if ( item->id == ID_BRIGHTNESS ) {
		Com_sprintf( value, sizeof( value ), "%.1f", slider->curvalue / 10.0f );
	} else {
		Com_sprintf( value, sizeof( value ), "%d%%", (int)( slider->curvalue * 10.0f ) );
	}

	Frontend_DrawText( item->left + UI_FRONTEND_SPACE_SM,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		item->name, UI_LEFT | UI_SMALLFONT, labelColor );

	trackX = item->left + 144;
	trackWidth = 80;
	trackY = item->top + ( item->bottom - item->top ) / 2;
	UI_FillRect( trackX, trackY, trackWidth, 2, lineColor );
	UI_FillRect( trackX, trackY, (int)( trackWidth * range ), 2,
		disabled ? displayMutedColor : displayAccentColor );
	UI_FillRect( trackX + (int)( trackWidth * range ) - 2, trackY - 3,
		4, 8, disabled ? displayMutedColor : displayAccentColor );
	Frontend_DrawText( item->right - UI_FRONTEND_SPACE_SM,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		value, UI_RIGHT | UI_SMALLFONT, valueColor );
}

static void Display_DrawAction( void *self ) {
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

static void Display_SetBounds( menucommon_s *item, int id,
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

static void Display_SetNavBounds( menutext_s *item, int id,
	const char *label, int y ) {
	item->generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
	item->generic.callback = UI_DisplayOptionsMenu_Event;
	item->string = (char *)label;
	item->style = UI_LEFT | UI_SMALLFONT;
	item->generic.ownerdraw = Display_DrawNavItem;
	Display_SetBounds( &item->generic, id, DISPLAY_NAV_X + 16, y,
		DISPLAY_NAV_WIDTH - 32, DISPLAY_ROW_HEIGHT, NULL );
}

static void Display_SetSliderBounds( menuslider_s *item, int id,
	const char *label, int y ) {
	item->generic.ownerdraw = Display_DrawSlider;
	Display_SetBounds( &item->generic, id, DISPLAY_COLUMN_X, y,
		DISPLAY_COLUMN_WIDTH, DISPLAY_ROW_HEIGHT, label );
	/* Slider_Key uses x as the start of its legacy ten-step input range. */
	item->generic.x = DISPLAY_COLUMN_X + 128;
}

static void UI_DisplayOptionsMenu_Draw( void ) {
	Frontend_DrawBackground( displayScrimColor );
	Frontend_DrawPanel( DISPLAY_FRAME_X, DISPLAY_FRAME_Y,
		DISPLAY_FRAME_WIDTH, DISPLAY_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( DISPLAY_FRAME_X + 24, DISPLAY_FRAME_Y + 24,
		"Display", UI_LEFT | UI_BIGFONT, displayTextColor );
	Frontend_DrawText( DISPLAY_FRAME_X + 24, DISPLAY_FRAME_Y + 48,
		"Tune brightness, screen size and framing",
		UI_LEFT | UI_SMALLFONT, displayMutedColor );
	Frontend_DrawStatusChip( DISPLAY_FRAME_X + DISPLAY_FRAME_WIDTH - 104,
		DISPLAY_FRAME_Y + 26, "Settings", displayAccentColor, 1.0f );

	Frontend_DrawCard( DISPLAY_DETAIL_X, DISPLAY_DETAIL_Y,
		DISPLAY_DETAIL_WIDTH, DISPLAY_DETAIL_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( DISPLAY_DETAIL_X + 16, DISPLAY_DETAIL_Y + 22,
		"Display calibration", UI_LEFT | UI_SMALLFONT, displayMutedColor );

	Menu_Draw( &displayOptionsInfo.menu );

	Frontend_DrawText( DISPLAY_FRAME_X + 24, DISPLAY_FRAME_Y + 384,
		"Select an option   Left / right adjust   Esc back",
		UI_LEFT | UI_SMALLFONT, displayMutedColor );
}


/*
=================
UI_DisplayOptionsMenu_Event
=================
*/
static void UI_DisplayOptionsMenu_Event( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_BRIGHTNESS:
		trap_Cvar_SetValue( "r_gamma", displayOptionsInfo.brightness.curvalue / 10.0f );
		break;
	
	case ID_SCREENSIZE:
		trap_Cvar_SetValue( "cg_viewsize", displayOptionsInfo.screensize.curvalue * 10 );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}


/*
===============
UI_DisplayOptionsMenu_Init
===============
*/
static void UI_DisplayOptionsMenu_Init( void ) {
	int		y;

	memset( &displayOptionsInfo, 0, sizeof(displayOptionsInfo) );

	UI_DisplayOptionsMenu_Cache();
	displayOptionsInfo.menu.wrapAround = qtrue;
	displayOptionsInfo.menu.fullscreen = qtrue;
	displayOptionsInfo.menu.draw = UI_DisplayOptionsMenu_Draw;

	displayOptionsInfo.banner.generic.type		= MTYPE_BTEXT;
	displayOptionsInfo.banner.generic.flags		= QMF_CENTER_JUSTIFY;
	displayOptionsInfo.banner.generic.x			= 320;
	displayOptionsInfo.banner.generic.y			= 16;
	displayOptionsInfo.banner.string			= "SYSTEM SETUP";
	displayOptionsInfo.banner.color				= color_white;
	displayOptionsInfo.banner.style				= UI_CENTER;

// STONELANCE
/*
	displayOptionsInfo.framel.generic.type		= MTYPE_BITMAP;
	displayOptionsInfo.framel.generic.name		= ART_FRAMEL;
	displayOptionsInfo.framel.generic.flags		= QMF_INACTIVE;
	displayOptionsInfo.framel.generic.x			= 0;  
	displayOptionsInfo.framel.generic.y			= 78;
	displayOptionsInfo.framel.width				= 256;
	displayOptionsInfo.framel.height			= 329;

	displayOptionsInfo.framer.generic.type		= MTYPE_BITMAP;
	displayOptionsInfo.framer.generic.name		= ART_FRAMER;
	displayOptionsInfo.framer.generic.flags		= QMF_INACTIVE;
	displayOptionsInfo.framer.generic.x			= 376;
	displayOptionsInfo.framer.generic.y			= 76;
	displayOptionsInfo.framer.width				= 256;
	displayOptionsInfo.framer.height			= 334;
*/
// END

	displayOptionsInfo.graphics.generic.type		= MTYPE_PTEXT;
	displayOptionsInfo.graphics.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	displayOptionsInfo.graphics.generic.id			= ID_GRAPHICS;
	displayOptionsInfo.graphics.generic.callback	= UI_DisplayOptionsMenu_Event;
	displayOptionsInfo.graphics.generic.x			= 216;
	displayOptionsInfo.graphics.generic.y			= 240 - 2 * PROP_HEIGHT;
	displayOptionsInfo.graphics.string				= "GRAPHICS";
	displayOptionsInfo.graphics.style				= UI_RIGHT;
// BAGPUSS
//	displayOptionsInfo.graphics.color				= color_red;
	displayOptionsInfo.graphics.color				= text_color_normal;
// END


	displayOptionsInfo.advanced_graphics.generic.type			= MTYPE_PTEXT;
	displayOptionsInfo.advanced_graphics.generic.flags			= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	displayOptionsInfo.advanced_graphics.generic.id			= ID_ADVANCED_GRAPHICS;
	displayOptionsInfo.advanced_graphics.generic.callback		= UI_DisplayOptionsMenu_Event;
	displayOptionsInfo.advanced_graphics.generic.x				= 216;
	displayOptionsInfo.advanced_graphics.generic.y				= 240 - PROP_HEIGHT;
	displayOptionsInfo.advanced_graphics.string					= "ADVANCED GRAPHICS";
	displayOptionsInfo.advanced_graphics.style					= UI_RIGHT;
	displayOptionsInfo.advanced_graphics.color					= text_color_normal;

	displayOptionsInfo.display.generic.type			= MTYPE_PTEXT;
	displayOptionsInfo.display.generic.flags		= QMF_RIGHT_JUSTIFY;
	displayOptionsInfo.display.generic.id			= ID_DISPLAY;
	displayOptionsInfo.display.generic.callback		= UI_DisplayOptionsMenu_Event;
	displayOptionsInfo.display.generic.x			= 216;
	displayOptionsInfo.display.generic.y			= 240;
	displayOptionsInfo.display.string				= "DISPLAY";
	displayOptionsInfo.display.style				= UI_RIGHT;
// BAGPUSS
//	displayOptionsInfo.display.color				= color_red;
	displayOptionsInfo.display.color				= text_color_normal;
// END

	displayOptionsInfo.sound.generic.type			= MTYPE_PTEXT;
	displayOptionsInfo.sound.generic.flags			= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	displayOptionsInfo.sound.generic.id				= ID_SOUND;
	displayOptionsInfo.sound.generic.callback		= UI_DisplayOptionsMenu_Event;
	displayOptionsInfo.sound.generic.x				= 216;
	displayOptionsInfo.sound.generic.y				= 240 + PROP_HEIGHT;
	displayOptionsInfo.sound.string					= "SOUND";
	displayOptionsInfo.sound.style					= UI_RIGHT;
// BAGPUSS
//	displayOptionsInfo.sound.color					= color_red;
	displayOptionsInfo.sound.color					= text_color_normal;
// END

	displayOptionsInfo.network.generic.type			= MTYPE_PTEXT;
	displayOptionsInfo.network.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	displayOptionsInfo.network.generic.id			= ID_NETWORK;
	displayOptionsInfo.network.generic.callback		= UI_DisplayOptionsMenu_Event;
	displayOptionsInfo.network.generic.x			= 216;
	displayOptionsInfo.network.generic.y			= 240 + 2 * PROP_HEIGHT;
	displayOptionsInfo.network.string				= "NETWORK";
	displayOptionsInfo.network.style				= UI_RIGHT;
// BAGPUSS
//	displayOptionsInfo.network.color				= color_red;
	displayOptionsInfo.network.color				= text_color_normal;
// END

	y = 240 - 1 * (BIGCHAR_HEIGHT+2);
	displayOptionsInfo.brightness.generic.type		= MTYPE_SLIDER;
	displayOptionsInfo.brightness.generic.name		= "Brightness:";
	displayOptionsInfo.brightness.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	displayOptionsInfo.brightness.generic.callback	= UI_DisplayOptionsMenu_Event;
	displayOptionsInfo.brightness.generic.ownerdraw	= UI_RallySlider_Draw;
	displayOptionsInfo.brightness.generic.id		= ID_BRIGHTNESS;
	displayOptionsInfo.brightness.generic.x			= 400;
	displayOptionsInfo.brightness.generic.y			= y;
	displayOptionsInfo.brightness.minvalue			= 5;
	displayOptionsInfo.brightness.maxvalue			= 20;
	if( !uis.glconfig.deviceSupportsGamma ) {
		displayOptionsInfo.brightness.generic.flags |= QMF_GRAYED;
	}

	y += BIGCHAR_HEIGHT+2;
	displayOptionsInfo.screensize.generic.type		= MTYPE_SLIDER;
	displayOptionsInfo.screensize.generic.name		= "Screen Size:";
	displayOptionsInfo.screensize.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	displayOptionsInfo.screensize.generic.callback	= UI_DisplayOptionsMenu_Event;
	displayOptionsInfo.screensize.generic.ownerdraw	= UI_RallySlider_Draw;
	displayOptionsInfo.screensize.generic.id		= ID_SCREENSIZE;
	displayOptionsInfo.screensize.generic.x			= 400;
	displayOptionsInfo.screensize.generic.y			= y;
	displayOptionsInfo.screensize.minvalue			= 3;
    displayOptionsInfo.screensize.maxvalue			= 10;

// STONELANCE
/*
	displayOptionsInfo.back.generic.type		= MTYPE_BITMAP;
	displayOptionsInfo.back.generic.name		= ART_BACK0;
	displayOptionsInfo.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	displayOptionsInfo.back.generic.callback	= UI_DisplayOptionsMenu_Event;
	displayOptionsInfo.back.generic.id			= ID_BACK;
	displayOptionsInfo.back.generic.x			= 0;
	displayOptionsInfo.back.generic.y			= 480-64;
	displayOptionsInfo.back.width				= 128;
	displayOptionsInfo.back.height				= 64;
	displayOptionsInfo.back.focuspic			= ART_BACK1;
*/
	displayOptionsInfo.back.generic.type			= MTYPE_PTEXT;
	displayOptionsInfo.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	displayOptionsInfo.back.generic.x				= 20;
	displayOptionsInfo.back.generic.y				= 480 - 50;
	displayOptionsInfo.back.generic.id				= ID_BACK;
	displayOptionsInfo.back.generic.callback		= UI_DisplayOptionsMenu_Event; 
	displayOptionsInfo.back.string					= "< BACK";
	displayOptionsInfo.back.color					= text_color_normal;
	displayOptionsInfo.back.style					= UI_LEFT | UI_SMALLFONT;
// END

// STONELANCE
/*
	Menu_AddItem( &displayOptionsInfo.menu, ( void * ) &displayOptionsInfo.framel );
	Menu_AddItem( &displayOptionsInfo.menu, ( void * ) &displayOptionsInfo.framer );
*/
// END
	Menu_AddItem( &displayOptionsInfo.menu, ( void * ) &displayOptionsInfo.brightness );
	Menu_AddItem( &displayOptionsInfo.menu, ( void * ) &displayOptionsInfo.screensize );
	Menu_AddItem( &displayOptionsInfo.menu, ( void * ) &displayOptionsInfo.back );

	displayOptionsInfo.brightness.curvalue  = trap_Cvar_VariableValue("r_gamma") * 10;
	displayOptionsInfo.screensize.curvalue  = trap_Cvar_VariableValue( "cg_viewsize")/10;

	/* Menu_AddItem initializes the legacy widgets and overwrites their
	 * default bounds. Apply the frontend layout after that initialization. */
	Display_SetSliderBounds( &displayOptionsInfo.brightness,
		ID_BRIGHTNESS, "Brightness", DISPLAY_ROW_START_Y );
	Display_SetSliderBounds( &displayOptionsInfo.screensize,
		ID_SCREENSIZE, "Screen size",
		DISPLAY_ROW_START_Y + DISPLAY_ROW_HEIGHT + DISPLAY_ROW_GAP );

	Display_SetBounds( &displayOptionsInfo.back.generic, ID_BACK,
		DISPLAY_NAV_X, DISPLAY_ACTION_Y, DISPLAY_ACTION_WIDTH,
		DISPLAY_ACTION_HEIGHT, NULL );
	displayOptionsInfo.back.generic.ownerdraw = Display_DrawAction;
}


/*
===============
UI_DisplayOptionsMenu_Cache
===============
*/
void UI_DisplayOptionsMenu_Cache( void ) {
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
UI_DisplayOptionsMenu
===============
*/
void UI_DisplayOptionsMenu( void ) {
// STONELANCE FIXME: get rid of this after proper tansitions are added
	uis.transitionIn = 0;
// END

	UI_DisplayOptionsMenu_Init();
	UI_PushMenu( &displayOptionsInfo.menu );
	Menu_SetCursorToItem( &displayOptionsInfo.menu, &displayOptionsInfo.brightness );
}
