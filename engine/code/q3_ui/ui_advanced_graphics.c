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

ADVANCED GRAPHICS OPTIONS MENU

=======================================================================
*/

#include "ui_local.h"

#define ID_GRAPHICS                 10
#define ID_ADVANCED_GRAPHICS        17
#define ID_DISPLAY                  11
#define ID_SOUND                    12
#define ID_NETWORK                  13
#define ID_HDR                      14
#define ID_POSTPROCESS              15
#define ID_BACK                     16

static const char *enabled_items[] = {
	"Off", "On", NULL
};

typedef struct {
	menuframework_s menu;

	menutext_s      banner;
	menutext_s      graphics;
	menutext_s      advanced_graphics;
	menutext_s      display;
	menutext_s      sound;
	menutext_s      network;

	menulist_s      hdr;
	menulist_s      postprocess;

	menutext_s      back;
} advancedGraphicsOptionsInfo_t;

static advancedGraphicsOptionsInfo_t advancedGraphicsOptionsInfo;

static void UI_AdvancedGraphicsOptionsMenu_Event( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_GRAPHICS:
		UI_PopMenu();
		UI_GraphicsOptionsMenu();
		break;

	case ID_ADVANCED_GRAPHICS:
		break;

	case ID_DISPLAY:
		UI_PopMenu();
		UI_DisplayOptionsMenu();
		break;

	case ID_SOUND:
		UI_PopMenu();
		UI_SoundOptionsMenu();
		break;

	case ID_NETWORK:
		UI_PopMenu();
		UI_NetworkOptionsMenu();
		break;

	case ID_HDR:
		trap_Cvar_SetValue( "r_hdr", advancedGraphicsOptionsInfo.hdr.curvalue );
		break;

	case ID_POSTPROCESS:
		trap_Cvar_SetValue( "r_postProcess", advancedGraphicsOptionsInfo.postprocess.curvalue );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}


static sfxHandle_t UI_AdvancedGraphicsOptionsMenu_Key( int key ) {
	if( key == K_MOUSE2 || key == K_ESCAPE ) {
		UI_PopMenu();
		return menu_out_sound;
	}

	return Menu_DefaultKey( &advancedGraphicsOptionsInfo.menu, key );
}

static void UI_AdvancedGraphicsOptionsMenu_Init( void ) {
	int y;

	memset( &advancedGraphicsOptionsInfo, 0, sizeof(advancedGraphicsOptionsInfo) );

	UI_AdvancedGraphicsOptionsMenu_Cache();
	advancedGraphicsOptionsInfo.menu.wrapAround = qtrue;
	advancedGraphicsOptionsInfo.menu.fullscreen = qtrue;
	advancedGraphicsOptionsInfo.menu.key = UI_AdvancedGraphicsOptionsMenu_Key;

	advancedGraphicsOptionsInfo.banner.generic.type      = MTYPE_BTEXT;
	advancedGraphicsOptionsInfo.banner.generic.flags     = QMF_CENTER_JUSTIFY;
	advancedGraphicsOptionsInfo.banner.generic.x         = 320;
	advancedGraphicsOptionsInfo.banner.generic.y         = 16;
	advancedGraphicsOptionsInfo.banner.string            = "SYSTEM SETUP";
	advancedGraphicsOptionsInfo.banner.color             = color_white;
	advancedGraphicsOptionsInfo.banner.style             = UI_CENTER;

	advancedGraphicsOptionsInfo.graphics.generic.type        = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.graphics.generic.flags       = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	advancedGraphicsOptionsInfo.graphics.generic.id          = ID_GRAPHICS;
	advancedGraphicsOptionsInfo.graphics.generic.callback    = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.graphics.generic.x           = 216;
	advancedGraphicsOptionsInfo.graphics.generic.y           = 240 - 2 * PROP_HEIGHT;
	advancedGraphicsOptionsInfo.graphics.string              = "GRAPHICS";
	advancedGraphicsOptionsInfo.graphics.style               = UI_RIGHT;
	advancedGraphicsOptionsInfo.graphics.color               = text_color_normal;

	advancedGraphicsOptionsInfo.advanced_graphics.generic.type      = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.advanced_graphics.generic.flags     = QMF_RIGHT_JUSTIFY;
	advancedGraphicsOptionsInfo.advanced_graphics.generic.id        = ID_ADVANCED_GRAPHICS;
	advancedGraphicsOptionsInfo.advanced_graphics.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.advanced_graphics.generic.x         = 216;
	advancedGraphicsOptionsInfo.advanced_graphics.generic.y         = 240 - PROP_HEIGHT;
	advancedGraphicsOptionsInfo.advanced_graphics.string            = "ADVANCED GRAPHICS";
	advancedGraphicsOptionsInfo.advanced_graphics.style             = UI_RIGHT;
	advancedGraphicsOptionsInfo.advanced_graphics.color             = text_color_normal;

	advancedGraphicsOptionsInfo.display.generic.type        = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.display.generic.flags       = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	advancedGraphicsOptionsInfo.display.generic.id          = ID_DISPLAY;
	advancedGraphicsOptionsInfo.display.generic.callback    = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.display.generic.x           = 216;
	advancedGraphicsOptionsInfo.display.generic.y           = 240;
	advancedGraphicsOptionsInfo.display.string              = "DISPLAY";
	advancedGraphicsOptionsInfo.display.style               = UI_RIGHT;
	advancedGraphicsOptionsInfo.display.color               = text_color_normal;

	advancedGraphicsOptionsInfo.sound.generic.type      = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.sound.generic.flags     = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	advancedGraphicsOptionsInfo.sound.generic.id        = ID_SOUND;
	advancedGraphicsOptionsInfo.sound.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.sound.generic.x         = 216;
	advancedGraphicsOptionsInfo.sound.generic.y         = 240 + PROP_HEIGHT;
	advancedGraphicsOptionsInfo.sound.string            = "SOUND";
	advancedGraphicsOptionsInfo.sound.style             = UI_RIGHT;
	advancedGraphicsOptionsInfo.sound.color             = text_color_normal;

	advancedGraphicsOptionsInfo.network.generic.type        = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.network.generic.flags       = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	advancedGraphicsOptionsInfo.network.generic.id          = ID_NETWORK;
	advancedGraphicsOptionsInfo.network.generic.callback    = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.network.generic.x           = 216;
	advancedGraphicsOptionsInfo.network.generic.y           = 240 + 2 * PROP_HEIGHT;
	advancedGraphicsOptionsInfo.network.string              = "NETWORK";
	advancedGraphicsOptionsInfo.network.style               = UI_RIGHT;
	advancedGraphicsOptionsInfo.network.color               = text_color_normal;

	y = 240 - 1 * (BIGCHAR_HEIGHT + 2);
	advancedGraphicsOptionsInfo.hdr.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.hdr.generic.name      = "HDR:";
	advancedGraphicsOptionsInfo.hdr.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.hdr.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.hdr.generic.id        = ID_HDR;
	advancedGraphicsOptionsInfo.hdr.generic.x         = 400;
	advancedGraphicsOptionsInfo.hdr.generic.y         = y;
	advancedGraphicsOptionsInfo.hdr.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.hdr.curvalue          = trap_Cvar_VariableValue( "r_hdr" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.postprocess.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.postprocess.generic.name      = "Post Process:";
	advancedGraphicsOptionsInfo.postprocess.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.postprocess.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.postprocess.generic.id        = ID_POSTPROCESS;
	advancedGraphicsOptionsInfo.postprocess.generic.x         = 400;
	advancedGraphicsOptionsInfo.postprocess.generic.y         = y;
	advancedGraphicsOptionsInfo.postprocess.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.postprocess.curvalue          = trap_Cvar_VariableValue( "r_postProcess" ) != 0;

	advancedGraphicsOptionsInfo.back.generic.type         = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.back.generic.flags        = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	advancedGraphicsOptionsInfo.back.generic.x            = 20;
	advancedGraphicsOptionsInfo.back.generic.y            = 480 - 50;
	advancedGraphicsOptionsInfo.back.generic.id           = ID_BACK;
	advancedGraphicsOptionsInfo.back.generic.callback     = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.back.string               = "< BACK";
	advancedGraphicsOptionsInfo.back.color                = text_color_normal;
	advancedGraphicsOptionsInfo.back.style                = UI_LEFT | UI_SMALLFONT;

	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.banner );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.graphics );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.advanced_graphics );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.display );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.sound );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.network );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.hdr );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.postprocess );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.back );
}

void UI_AdvancedGraphicsOptionsMenu_Cache( void ) {
}

void UI_AdvancedGraphicsOptionsMenu( void ) {
// STONELANCE FIXME: get rid of this after proper tansitions are added
	uis.transitionIn = 0;
// END
	UI_AdvancedGraphicsOptionsMenu_Init();
	UI_PushMenu( &advancedGraphicsOptionsInfo.menu );
	Menu_SetCursorToItem( &advancedGraphicsOptionsInfo.menu, &advancedGraphicsOptionsInfo.hdr );
}
