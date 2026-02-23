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
#define ID_TONEMAP                  18
#define ID_AUTOEXPOSURE             19
#define ID_SSAO                     20
#define ID_SUNRAYS                  21
#define ID_DYNAMIC_REFLECTIONS      22
#define ID_PRESET                   23

#define ADV_PRESET_CUSTOM           0
#define ADV_PRESET_LOW              1
#define ADV_PRESET_MIDDLE           2
#define ADV_PRESET_HIGH             3
#define ADV_PRESET_ULTRA            4

static const char *enabled_items[] = {
	"Off", "On", NULL
};

static const char *preset_items[] = {
	"Custom", "Low", "Middle", "High", "Ultra", NULL
};

typedef struct {
	menuframework_s menu;

	menutext_s      banner;
	menutext_s      graphics;
	menutext_s      advanced_graphics;
	menutext_s      display;
	menutext_s      sound;
	menutext_s      network;

	menulist_s      preset;
	menulist_s      hdr;
	menulist_s      postprocess;
	menulist_s      tonemap;
	menulist_s      autoexposure;
	menulist_s      ssao;
	menulist_s      sunrays;
	menulist_s      dynamic_reflections;

	menutext_s      back;
} advancedGraphicsOptionsInfo_t;

static advancedGraphicsOptionsInfo_t advancedGraphicsOptionsInfo;

static void UI_AdvancedGraphicsOptionsMenu_SyncPreset( void ) {
	if (advancedGraphicsOptionsInfo.hdr.curvalue == 0 &&
		advancedGraphicsOptionsInfo.postprocess.curvalue == 0 &&
		advancedGraphicsOptionsInfo.tonemap.curvalue == 0 &&
		advancedGraphicsOptionsInfo.autoexposure.curvalue == 0 &&
		advancedGraphicsOptionsInfo.ssao.curvalue == 0 &&
		advancedGraphicsOptionsInfo.sunrays.curvalue == 0 &&
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue == 0) {
		advancedGraphicsOptionsInfo.preset.curvalue = ADV_PRESET_LOW;
		return;
	}

	if (advancedGraphicsOptionsInfo.hdr.curvalue == 1 &&
		advancedGraphicsOptionsInfo.postprocess.curvalue == 1 &&
		advancedGraphicsOptionsInfo.tonemap.curvalue == 1 &&
		advancedGraphicsOptionsInfo.autoexposure.curvalue == 1 &&
		advancedGraphicsOptionsInfo.ssao.curvalue == 0 &&
		advancedGraphicsOptionsInfo.sunrays.curvalue == 0 &&
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue == 0) {
		advancedGraphicsOptionsInfo.preset.curvalue = ADV_PRESET_MIDDLE;
		return;
	}

	if (advancedGraphicsOptionsInfo.hdr.curvalue == 1 &&
		advancedGraphicsOptionsInfo.postprocess.curvalue == 1 &&
		advancedGraphicsOptionsInfo.tonemap.curvalue == 1 &&
		advancedGraphicsOptionsInfo.autoexposure.curvalue == 1 &&
		advancedGraphicsOptionsInfo.ssao.curvalue == 1 &&
		advancedGraphicsOptionsInfo.sunrays.curvalue == 0 &&
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue == 0) {
		advancedGraphicsOptionsInfo.preset.curvalue = ADV_PRESET_HIGH;
		return;
	}

	if (advancedGraphicsOptionsInfo.hdr.curvalue == 1 &&
		advancedGraphicsOptionsInfo.postprocess.curvalue == 1 &&
		advancedGraphicsOptionsInfo.tonemap.curvalue == 1 &&
		advancedGraphicsOptionsInfo.autoexposure.curvalue == 1 &&
		advancedGraphicsOptionsInfo.ssao.curvalue == 1 &&
		advancedGraphicsOptionsInfo.sunrays.curvalue == 1 &&
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue == 1) {
		advancedGraphicsOptionsInfo.preset.curvalue = ADV_PRESET_ULTRA;
		return;
	}

	advancedGraphicsOptionsInfo.preset.curvalue = ADV_PRESET_CUSTOM;
}

static void UI_AdvancedGraphicsOptionsMenu_ApplyPreset( int preset ) {
	switch (preset) {
	case ADV_PRESET_LOW:
		advancedGraphicsOptionsInfo.hdr.curvalue = 0;
		advancedGraphicsOptionsInfo.postprocess.curvalue = 0;
		advancedGraphicsOptionsInfo.tonemap.curvalue = 0;
		advancedGraphicsOptionsInfo.autoexposure.curvalue = 0;
		advancedGraphicsOptionsInfo.ssao.curvalue = 0;
		advancedGraphicsOptionsInfo.sunrays.curvalue = 0;
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue = 0;
		break;
	case ADV_PRESET_MIDDLE:
		advancedGraphicsOptionsInfo.hdr.curvalue = 1;
		advancedGraphicsOptionsInfo.postprocess.curvalue = 1;
		advancedGraphicsOptionsInfo.tonemap.curvalue = 1;
		advancedGraphicsOptionsInfo.autoexposure.curvalue = 1;
		advancedGraphicsOptionsInfo.ssao.curvalue = 0;
		advancedGraphicsOptionsInfo.sunrays.curvalue = 0;
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue = 0;
		break;
	case ADV_PRESET_HIGH:
		advancedGraphicsOptionsInfo.hdr.curvalue = 1;
		advancedGraphicsOptionsInfo.postprocess.curvalue = 1;
		advancedGraphicsOptionsInfo.tonemap.curvalue = 1;
		advancedGraphicsOptionsInfo.autoexposure.curvalue = 1;
		advancedGraphicsOptionsInfo.ssao.curvalue = 1;
		advancedGraphicsOptionsInfo.sunrays.curvalue = 0;
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue = 0;
		break;
	case ADV_PRESET_ULTRA:
		advancedGraphicsOptionsInfo.hdr.curvalue = 1;
		advancedGraphicsOptionsInfo.postprocess.curvalue = 1;
		advancedGraphicsOptionsInfo.tonemap.curvalue = 1;
		advancedGraphicsOptionsInfo.autoexposure.curvalue = 1;
		advancedGraphicsOptionsInfo.ssao.curvalue = 1;
		advancedGraphicsOptionsInfo.sunrays.curvalue = 1;
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue = 1;
		break;
	default:
		return;
	}

	trap_Cvar_SetValue( "r_hdr", advancedGraphicsOptionsInfo.hdr.curvalue );
	trap_Cvar_SetValue( "r_postProcess", advancedGraphicsOptionsInfo.postprocess.curvalue );
	trap_Cvar_SetValue( "r_toneMap", advancedGraphicsOptionsInfo.tonemap.curvalue );
	trap_Cvar_SetValue( "r_autoExposure", advancedGraphicsOptionsInfo.autoexposure.curvalue );
	trap_Cvar_SetValue( "r_ssao", advancedGraphicsOptionsInfo.ssao.curvalue );
	trap_Cvar_SetValue( "r_drawSunRays", advancedGraphicsOptionsInfo.sunrays.curvalue );
	trap_Cvar_SetValue( "r_dynamicReflections", advancedGraphicsOptionsInfo.dynamic_reflections.curvalue );
}

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

	case ID_PRESET:
		UI_AdvancedGraphicsOptionsMenu_ApplyPreset( advancedGraphicsOptionsInfo.preset.curvalue );
		break;

	case ID_HDR:
		trap_Cvar_SetValue( "r_hdr", advancedGraphicsOptionsInfo.hdr.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_POSTPROCESS:
		trap_Cvar_SetValue( "r_postProcess", advancedGraphicsOptionsInfo.postprocess.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_TONEMAP:
		trap_Cvar_SetValue( "r_toneMap", advancedGraphicsOptionsInfo.tonemap.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_AUTOEXPOSURE:
		trap_Cvar_SetValue( "r_autoExposure", advancedGraphicsOptionsInfo.autoexposure.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_SSAO:
		trap_Cvar_SetValue( "r_ssao", advancedGraphicsOptionsInfo.ssao.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_SUNRAYS:
		trap_Cvar_SetValue( "r_drawSunRays", advancedGraphicsOptionsInfo.sunrays.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_DYNAMIC_REFLECTIONS:
		trap_Cvar_SetValue( "r_dynamicReflections", advancedGraphicsOptionsInfo.dynamic_reflections.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
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

	y = 240 - 3 * (BIGCHAR_HEIGHT + 2);
	advancedGraphicsOptionsInfo.preset.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.preset.generic.name      = "Quality Preset:";
	advancedGraphicsOptionsInfo.preset.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.preset.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.preset.generic.id        = ID_PRESET;
	advancedGraphicsOptionsInfo.preset.generic.x         = 400;
	advancedGraphicsOptionsInfo.preset.generic.y         = y;
	advancedGraphicsOptionsInfo.preset.itemnames         = preset_items;

	y += BIGCHAR_HEIGHT + 2;
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

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.tonemap.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.tonemap.generic.name      = "Tone Mapping:";
	advancedGraphicsOptionsInfo.tonemap.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.tonemap.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.tonemap.generic.id        = ID_TONEMAP;
	advancedGraphicsOptionsInfo.tonemap.generic.x         = 400;
	advancedGraphicsOptionsInfo.tonemap.generic.y         = y;
	advancedGraphicsOptionsInfo.tonemap.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.tonemap.curvalue          = trap_Cvar_VariableValue( "r_toneMap" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.autoexposure.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.autoexposure.generic.name      = "Auto Exposure:";
	advancedGraphicsOptionsInfo.autoexposure.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.autoexposure.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.autoexposure.generic.id        = ID_AUTOEXPOSURE;
	advancedGraphicsOptionsInfo.autoexposure.generic.x         = 400;
	advancedGraphicsOptionsInfo.autoexposure.generic.y         = y;
	advancedGraphicsOptionsInfo.autoexposure.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.autoexposure.curvalue          = trap_Cvar_VariableValue( "r_autoExposure" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.ssao.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.ssao.generic.name      = "SSAO:";
	advancedGraphicsOptionsInfo.ssao.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.ssao.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.ssao.generic.id        = ID_SSAO;
	advancedGraphicsOptionsInfo.ssao.generic.x         = 400;
	advancedGraphicsOptionsInfo.ssao.generic.y         = y;
	advancedGraphicsOptionsInfo.ssao.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.ssao.curvalue          = trap_Cvar_VariableValue( "r_ssao" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.sunrays.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.sunrays.generic.name      = "Sun Rays:";
	advancedGraphicsOptionsInfo.sunrays.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.sunrays.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.sunrays.generic.id        = ID_SUNRAYS;
	advancedGraphicsOptionsInfo.sunrays.generic.x         = 400;
	advancedGraphicsOptionsInfo.sunrays.generic.y         = y;
	advancedGraphicsOptionsInfo.sunrays.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.sunrays.curvalue          = trap_Cvar_VariableValue( "r_drawSunRays" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.name      = "Dynamic Reflections:";
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.id        = ID_DYNAMIC_REFLECTIONS;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.x         = 400;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.y         = y;
	advancedGraphicsOptionsInfo.dynamic_reflections.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.dynamic_reflections.curvalue          = trap_Cvar_VariableValue( "r_dynamicReflections" ) != 0;
	UI_AdvancedGraphicsOptionsMenu_SyncPreset();

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
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.preset );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.hdr );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.postprocess );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.tonemap );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.autoexposure );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.ssao );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.sunrays );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.dynamic_reflections );
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
