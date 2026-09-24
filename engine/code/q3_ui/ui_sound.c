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

SOUND OPTIONS MENU

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
#define ART_ACCEPT0			"menu/art/accept_0"
#define ART_ACCEPT1			"menu/art/accept_1"
*/
// END

#define ID_GRAPHICS			10
#define ID_ADVANCED_GRAPHICS	21
#define ID_DISPLAY			11
#define ID_SOUND			12
#define ID_NETWORK			13
#define ID_EFFECTSVOLUME	14
#define ID_MUSICVOLUME		15
#define ID_QUALITY			16
#define ID_SOUNDSYSTEM		17
//#define ID_A3D				18
#define ID_BACK				19
#define ID_APPLY			20

#define SOUND_FRAME_X               24
#define SOUND_FRAME_Y               20
#define SOUND_FRAME_WIDTH           592
#define SOUND_FRAME_HEIGHT          440
#define SOUND_NAV_X                 40
#define SOUND_NAV_Y                 104
#define SOUND_NAV_WIDTH             164
#define SOUND_NAV_HEIGHT            292
#define SOUND_DETAIL_X              40
#define SOUND_DETAIL_Y              104
#define SOUND_DETAIL_WIDTH          556
#define SOUND_DETAIL_HEIGHT         292
#define SOUND_ROW_HEIGHT            24
#define SOUND_ROW_GAP               4
#define SOUND_ROW_START_Y           148
#define SOUND_COLUMN_X              236
#define SOUND_COLUMN_WIDTH          344
#define SOUND_ACTION_Y              420
#define SOUND_ACTION_WIDTH          120
#define SOUND_ACTION_HEIGHT         24

static vec4_t soundScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t soundAccentColor = UI_FRONTEND_COLOR_ACCENT;
static vec4_t soundTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t soundMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t soundBorderColor = UI_FRONTEND_COLOR_BORDER;
static vec4_t soundFocusColor = UI_FRONTEND_COLOR_FOCUS_BG;

/*
 * Keep the UI's implicit-default mapping in sync with the SDL backend:
 * s_sdlSpeed = 0 falls back to 22050 Hz.
 */
#define DEFAULT_SDL_SND_SPEED 22050

static const char *quality_items[] = {
	"Low", "Medium", "High", NULL
};

#define UISND_SDL 0
#define UISND_OPENAL 1

static const char *soundSystem_items[] = {
	"SDL", "OpenAL", NULL
};

typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
// STONELANCE
/*
	menubitmap_s		framel;
	menubitmap_s		framer;
*/
// END

	menutext_s			graphics;
	menutext_s			advanced_graphics;
	menutext_s			display;
	menutext_s			sound;
	menutext_s			network;

	menuslider_s		sfxvolume;
	menuslider_s		musicvolume;
	menulist_s  		soundSystem;
	menulist_s			quality;
//	menuradiobutton_s	a3d;

// STONELANCE
//	menubitmap_s		back;
//	menubitmap_s		apply;
	menutext_s			back;
	menutext_s			apply;
// END

	float				sfxvolume_original;
	float				musicvolume_original;
	int					soundSystem_original;
	int					quality_original;
} soundOptionsInfo_t;

static soundOptionsInfo_t	soundOptionsInfo;

static void UI_SoundOptionsMenu_Event( void *ptr, int event );

static const char *Sound_ListValue( menulist_s *item ) {
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

static void Sound_DrawNavItem( void *self ) {
	menutext_s *text;
	menucommon_s *item;
	qboolean focus;
	qboolean active;

	text = (menutext_s *)self;
	item = &text->generic;
	focus = ( Menu_ItemAtCursor( item->parent ) == item );
	active = ( item->id == ID_SOUND );
	Frontend_DrawNavButton( item->left, item->top,
		item->right - item->left, item->bottom - item->top,
		text->string, 1.0f, active || focus, UI_LEFT );
}

static void Sound_DrawSetting( void *self ) {
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
	value = Sound_ListValue( (menulist_s *)item );

	Vector4Copy( soundFocusColor, fillColor );
	fillColor[3] = highlighted ? 0.72f : 0.0f;
	Vector4Copy( soundBorderColor, lineColor );
	lineColor[3] = 0.55f;
	if ( highlighted ) {
		UI_FillRect( item->left, item->top,
			item->right - item->left, item->bottom - item->top, fillColor );
		Vector4Copy( soundAccentColor, lineColor );
	}
	UI_FillRect( item->left, item->bottom - 1,
		item->right - item->left, 1, lineColor );

	if ( disabled ) {
		Vector4Copy( soundMutedColor, labelColor );
		labelColor[3] = 0.65f;
		Vector4Copy( labelColor, valueColor );
	} else if ( highlighted ) {
		Vector4Copy( soundAccentColor, labelColor );
		Vector4Copy( soundTextColor, valueColor );
	} else {
		Vector4Copy( soundMutedColor, labelColor );
		Vector4Copy( soundTextColor, valueColor );
	}

	Frontend_DrawText( item->left + UI_FRONTEND_SPACE_SM,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		item->name, UI_LEFT | UI_SMALLFONT, labelColor );
	Frontend_DrawText( item->right - UI_FRONTEND_SPACE_SM,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		value, UI_RIGHT | UI_SMALLFONT, valueColor );
}

static void Sound_DrawSlider( void *self ) {
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

	Vector4Copy( soundFocusColor, fillColor );
	fillColor[3] = highlighted ? 0.72f : 0.0f;
	Vector4Copy( soundBorderColor, lineColor );
	lineColor[3] = 0.55f;
	if ( highlighted ) {
		UI_FillRect( item->left, item->top,
			item->right - item->left, item->bottom - item->top, fillColor );
		Vector4Copy( soundAccentColor, lineColor );
	}
	UI_FillRect( item->left, item->bottom - 1,
		item->right - item->left, 1, lineColor );

	if ( disabled ) {
		Vector4Copy( soundMutedColor, labelColor );
		labelColor[3] = 0.65f;
		Vector4Copy( labelColor, valueColor );
	} else if ( highlighted ) {
		Vector4Copy( soundAccentColor, labelColor );
		Vector4Copy( soundTextColor, valueColor );
	} else {
		Vector4Copy( soundMutedColor, labelColor );
		Vector4Copy( soundTextColor, valueColor );
	}

	if ( slider->maxvalue > slider->minvalue ) {
		range = ( slider->curvalue - slider->minvalue ) /
			( slider->maxvalue - slider->minvalue );
	} else {
		range = 0.0f;
	}
	if ( range < 0.0f ) range = 0.0f;
	if ( range > 1.0f ) range = 1.0f;
	Com_sprintf( value, sizeof( value ), "%d%%", (int)( slider->curvalue * 10.0f ) );

	Frontend_DrawText( item->left + UI_FRONTEND_SPACE_SM,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		item->name, UI_LEFT | UI_SMALLFONT, labelColor );
	trackX = item->left + 144;
	trackWidth = 80;
	trackY = item->top + ( item->bottom - item->top ) / 2;
	UI_FillRect( trackX, trackY, trackWidth, 2, lineColor );
	UI_FillRect( trackX, trackY, (int)( trackWidth * range ), 2,
		disabled ? soundMutedColor : soundAccentColor );
	UI_FillRect( trackX + (int)( trackWidth * range ) - 2, trackY - 3,
		4, 8, disabled ? soundMutedColor : soundAccentColor );
	Frontend_DrawText( item->right - UI_FRONTEND_SPACE_SM,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		value, UI_RIGHT | UI_SMALLFONT, valueColor );
}

static void Sound_DrawAction( void *self ) {
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

static void Sound_SetBounds( menucommon_s *item, int id,
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

static void Sound_SetNavBounds( menutext_s *item, int id,
	const char *label, int y ) {
	item->generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
	item->generic.callback = UI_SoundOptionsMenu_Event;
	item->string = (char *)label;
	item->style = UI_LEFT | UI_SMALLFONT;
	item->generic.ownerdraw = Sound_DrawNavItem;
	Sound_SetBounds( &item->generic, id, SOUND_NAV_X + 16, y,
		SOUND_NAV_WIDTH - 32, SOUND_ROW_HEIGHT, NULL );
}

static void Sound_SetSettingBounds( menucommon_s *item, int id,
	const char *label, int y ) {
	item->ownerdraw = Sound_DrawSetting;
	Sound_SetBounds( item, id, SOUND_COLUMN_X, y,
		SOUND_COLUMN_WIDTH, SOUND_ROW_HEIGHT, label );
}

static void Sound_SetSliderBounds( menuslider_s *item, int id,
	const char *label, int y ) {
	item->generic.ownerdraw = Sound_DrawSlider;
	Sound_SetBounds( &item->generic, id, SOUND_COLUMN_X, y,
		SOUND_COLUMN_WIDTH, SOUND_ROW_HEIGHT, label );
	/* Slider_Key uses x as the start of its legacy ten-step input range. */
	item->generic.x = SOUND_COLUMN_X + 128;
}


/*
=================
UI_SoundOptionsMenu_Event
=================
*/
static void UI_SoundOptionsMenu_Event( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
/*
	case ID_A3D:
		if( soundOptionsInfo.a3d.curvalue ) {
			trap_Cmd_ExecuteText( EXEC_NOW, "s_enable_a3d\n" );
		}
		else {
			trap_Cmd_ExecuteText( EXEC_NOW, "s_disable_a3d\n" );
		}
		soundOptionsInfo.a3d.curvalue = (int)trap_Cvar_VariableValue( "s_usingA3D" );
		break;
*/
	case ID_BACK:
		UI_PopMenu();
		break;

	case ID_APPLY:
		trap_Cvar_SetValue( "s_volume", soundOptionsInfo.sfxvolume.curvalue / 10 );
		soundOptionsInfo.sfxvolume_original = soundOptionsInfo.sfxvolume.curvalue;

		trap_Cvar_SetValue( "s_musicvolume", soundOptionsInfo.musicvolume.curvalue / 10 );
		soundOptionsInfo.musicvolume_original = soundOptionsInfo.musicvolume.curvalue;

		// Check if something changed that requires the sound system to be restarted.
		if (soundOptionsInfo.quality_original != soundOptionsInfo.quality.curvalue
			|| soundOptionsInfo.soundSystem_original != soundOptionsInfo.soundSystem.curvalue)
		{
			int speed;

			switch ( soundOptionsInfo.quality.curvalue )
			{
				default:
				case 0:
					speed = 11025;
					break;
				case 1:
					speed = 22050;
					break;
				case 2:
					speed = 44100;
					break;
			}

			if (speed == DEFAULT_SDL_SND_SPEED)
				speed = 0;

			trap_Cvar_SetValue( "s_sdlSpeed", speed );
			soundOptionsInfo.quality_original = soundOptionsInfo.quality.curvalue;

			trap_Cvar_SetValue( "s_useOpenAL", (soundOptionsInfo.soundSystem.curvalue == UISND_OPENAL) );
			soundOptionsInfo.soundSystem_original = soundOptionsInfo.soundSystem.curvalue;

			UI_ForceMenuOff();
			trap_Cmd_ExecuteText( EXEC_APPEND, "snd_restart\n" );
		}
		break;
	}
}

/*
=================
SoundOptions_UpdateMenuItems
=================
*/
static void SoundOptions_UpdateMenuItems( void )
{
	if ( soundOptionsInfo.soundSystem.curvalue == UISND_SDL )
	{
		soundOptionsInfo.quality.generic.flags &= ~QMF_GRAYED;
	}
	else
	{
		soundOptionsInfo.quality.generic.flags |= QMF_GRAYED;
	}

	soundOptionsInfo.apply.generic.flags |= QMF_HIDDEN|QMF_INACTIVE;

	if ( soundOptionsInfo.sfxvolume_original != soundOptionsInfo.sfxvolume.curvalue )
	{
		soundOptionsInfo.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( soundOptionsInfo.musicvolume_original != soundOptionsInfo.musicvolume.curvalue )
	{
		soundOptionsInfo.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( soundOptionsInfo.soundSystem_original != soundOptionsInfo.soundSystem.curvalue )
	{
		soundOptionsInfo.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( soundOptionsInfo.quality_original != soundOptionsInfo.quality.curvalue )
	{
		soundOptionsInfo.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
}

/*
================
SoundOptions_MenuDraw
================
*/
void SoundOptions_MenuDraw (void)
{
	SoundOptions_UpdateMenuItems();

	Frontend_DrawBackground( soundScrimColor );
	Frontend_DrawPanel( SOUND_FRAME_X, SOUND_FRAME_Y,
		SOUND_FRAME_WIDTH, SOUND_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( SOUND_FRAME_X + 24, SOUND_FRAME_Y + 24,
		"Sound", UI_LEFT | UI_BIGFONT, soundTextColor );
	Frontend_DrawText( SOUND_FRAME_X + 24, SOUND_FRAME_Y + 48,
		"Balance music, effects and output",
		UI_LEFT | UI_SMALLFONT, soundMutedColor );
	Frontend_DrawStatusChip( SOUND_FRAME_X + SOUND_FRAME_WIDTH - 104,
		SOUND_FRAME_Y + 26, "Settings", soundAccentColor, 1.0f );

	Frontend_DrawCard( SOUND_DETAIL_X, SOUND_DETAIL_Y,
		SOUND_DETAIL_WIDTH, SOUND_DETAIL_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( SOUND_DETAIL_X + 16, SOUND_DETAIL_Y + 22,
		"Audio mix", UI_LEFT | UI_SMALLFONT, soundMutedColor );

	Menu_Draw( &soundOptionsInfo.menu );

	Frontend_DrawText( SOUND_FRAME_X + 24, SOUND_FRAME_Y + 384,
		"Select an option   Left / right adjust   Esc back",
		UI_LEFT | UI_SMALLFONT, soundMutedColor );
}

/*
===============
UI_SoundOptionsMenu_Init
===============
*/
static void UI_SoundOptionsMenu_Init( void ) {
	int				y;
	int				speed;

	memset( &soundOptionsInfo, 0, sizeof(soundOptionsInfo) );

	UI_SoundOptionsMenu_Cache();
	soundOptionsInfo.menu.wrapAround = qtrue;
	soundOptionsInfo.menu.fullscreen = qtrue;
	soundOptionsInfo.menu.draw		= SoundOptions_MenuDraw;

	soundOptionsInfo.banner.generic.type		= MTYPE_BTEXT;
	soundOptionsInfo.banner.generic.flags		= QMF_CENTER_JUSTIFY;
	soundOptionsInfo.banner.generic.x			= 320;
	soundOptionsInfo.banner.generic.y			= 16;
	soundOptionsInfo.banner.string				= "SYSTEM SETUP";
	soundOptionsInfo.banner.color				= color_white;
	soundOptionsInfo.banner.style				= UI_CENTER;

// STONELANCE
/*
	soundOptionsInfo.framel.generic.type		= MTYPE_BITMAP;
	soundOptionsInfo.framel.generic.name		= ART_FRAMEL;
	soundOptionsInfo.framel.generic.flags		= QMF_INACTIVE;
	soundOptionsInfo.framel.generic.x			= 0;  
	soundOptionsInfo.framel.generic.y			= 78;
	soundOptionsInfo.framel.width				= 256;
	soundOptionsInfo.framel.height				= 329;

	soundOptionsInfo.framer.generic.type		= MTYPE_BITMAP;
	soundOptionsInfo.framer.generic.name		= ART_FRAMER;
	soundOptionsInfo.framer.generic.flags		= QMF_INACTIVE;
	soundOptionsInfo.framer.generic.x			= 376;
	soundOptionsInfo.framer.generic.y			= 76;
	soundOptionsInfo.framer.width				= 256;
	soundOptionsInfo.framer.height				= 334;
*/
// END

	soundOptionsInfo.graphics.generic.type		= MTYPE_PTEXT;
	soundOptionsInfo.graphics.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	soundOptionsInfo.graphics.generic.id		= ID_GRAPHICS;
	soundOptionsInfo.graphics.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.graphics.generic.x			= 216;
	soundOptionsInfo.graphics.generic.y			= 240 - 2 * PROP_HEIGHT;
	soundOptionsInfo.graphics.string			= "GRAPHICS";
	soundOptionsInfo.graphics.style				= UI_RIGHT;
// BAGPUSS
//	soundOptionsInfo.graphics.color				= color_red;
	soundOptionsInfo.graphics.color				= text_color_normal;
// END


	soundOptionsInfo.advanced_graphics.generic.type		= MTYPE_PTEXT;
	soundOptionsInfo.advanced_graphics.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	soundOptionsInfo.advanced_graphics.generic.id			= ID_ADVANCED_GRAPHICS;
	soundOptionsInfo.advanced_graphics.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.advanced_graphics.generic.x			= 216;
	soundOptionsInfo.advanced_graphics.generic.y			= 240 - PROP_HEIGHT;
	soundOptionsInfo.advanced_graphics.string				= "ADVANCED GRAPHICS";
	soundOptionsInfo.advanced_graphics.style				= UI_RIGHT;
	soundOptionsInfo.advanced_graphics.color				= text_color_normal;

	soundOptionsInfo.display.generic.type		= MTYPE_PTEXT;
	soundOptionsInfo.display.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	soundOptionsInfo.display.generic.id			= ID_DISPLAY;
	soundOptionsInfo.display.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.display.generic.x			= 216;
	soundOptionsInfo.display.generic.y			= 240;
	soundOptionsInfo.display.string				= "DISPLAY";
	soundOptionsInfo.display.style				= UI_RIGHT;
// BAGPUSS
//	soundOptionsInfo.display.color				= color_red;
	soundOptionsInfo.display.color				= text_color_normal;
// END

	soundOptionsInfo.sound.generic.type			= MTYPE_PTEXT;
	soundOptionsInfo.sound.generic.flags		= QMF_RIGHT_JUSTIFY;
	soundOptionsInfo.sound.generic.id			= ID_SOUND;
	soundOptionsInfo.sound.generic.callback		= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.sound.generic.x			= 216;
	soundOptionsInfo.sound.generic.y			= 240 + PROP_HEIGHT;
	soundOptionsInfo.sound.string				= "SOUND";
	soundOptionsInfo.sound.style				= UI_RIGHT;
// BAGPUSS
//	soundOptionsInfo.sound.color				= color_red;
	soundOptionsInfo.sound.color				= text_color_normal;
// END

	soundOptionsInfo.network.generic.type		= MTYPE_PTEXT;
	soundOptionsInfo.network.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	soundOptionsInfo.network.generic.id			= ID_NETWORK;
	soundOptionsInfo.network.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.network.generic.x			= 216;
	soundOptionsInfo.network.generic.y			= 240 + 2 * PROP_HEIGHT;
	soundOptionsInfo.network.string				= "NETWORK";
	soundOptionsInfo.network.style				= UI_RIGHT;
// BAGPUSS
//	soundOptionsInfo.network.color				= color_red;
	soundOptionsInfo.network.color				= text_color_normal;
// END

	y = 240 - 2 * (BIGCHAR_HEIGHT + 2);
	soundOptionsInfo.sfxvolume.generic.type		= MTYPE_SLIDER;
	soundOptionsInfo.sfxvolume.generic.name		= "Effects Volume:";
	soundOptionsInfo.sfxvolume.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.sfxvolume.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.sfxvolume.generic.ownerdraw	= UI_RallySlider_Draw;
	soundOptionsInfo.sfxvolume.generic.id		= ID_EFFECTSVOLUME;
	soundOptionsInfo.sfxvolume.generic.x		= 400;
	soundOptionsInfo.sfxvolume.generic.y		= y;
	soundOptionsInfo.sfxvolume.minvalue			= 0;
	soundOptionsInfo.sfxvolume.maxvalue			= 10;

	y += BIGCHAR_HEIGHT+2;
	soundOptionsInfo.musicvolume.generic.type		= MTYPE_SLIDER;
	soundOptionsInfo.musicvolume.generic.name		= "Music Volume:";
	soundOptionsInfo.musicvolume.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.musicvolume.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.musicvolume.generic.ownerdraw	= UI_RallySlider_Draw;
	soundOptionsInfo.musicvolume.generic.id			= ID_MUSICVOLUME;
	soundOptionsInfo.musicvolume.generic.x			= 400;
	soundOptionsInfo.musicvolume.generic.y			= y;
	soundOptionsInfo.musicvolume.minvalue			= 0;
	soundOptionsInfo.musicvolume.maxvalue			= 10;

	y += BIGCHAR_HEIGHT+2;
	soundOptionsInfo.soundSystem.generic.type		= MTYPE_SPINCONTROL;
	soundOptionsInfo.soundSystem.generic.name		= "Sound System:";
	soundOptionsInfo.soundSystem.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.soundSystem.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.soundSystem.generic.id			= ID_SOUNDSYSTEM;
	soundOptionsInfo.soundSystem.generic.x			= 400;
	soundOptionsInfo.soundSystem.generic.y			= y;
	soundOptionsInfo.soundSystem.itemnames			= soundSystem_items;

	y += BIGCHAR_HEIGHT+2;
	soundOptionsInfo.quality.generic.type		= MTYPE_SPINCONTROL;
	soundOptionsInfo.quality.generic.name		= "SDL Sound Quality:";
	soundOptionsInfo.quality.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.quality.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.quality.generic.id			= ID_QUALITY;
	soundOptionsInfo.quality.generic.x			= 400;
	soundOptionsInfo.quality.generic.y			= y;
	soundOptionsInfo.quality.itemnames			= quality_items;

/*
	y += BIGCHAR_HEIGHT+2;
	soundOptionsInfo.a3d.generic.type			= MTYPE_RADIOBUTTON;
	soundOptionsInfo.a3d.generic.name			= "A3D:";
	soundOptionsInfo.a3d.generic.flags			= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.a3d.generic.callback		= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.a3d.generic.id				= ID_A3D;
	soundOptionsInfo.a3d.generic.x				= 400;
	soundOptionsInfo.a3d.generic.y				= y;
*/
// STONELANCE
/*
	soundOptionsInfo.back.generic.type			= MTYPE_BITMAP;
	soundOptionsInfo.back.generic.name			= ART_BACK0;
	soundOptionsInfo.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	soundOptionsInfo.back.generic.callback		= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.back.generic.id			= ID_BACK;
	soundOptionsInfo.back.generic.x				= 0;
	soundOptionsInfo.back.generic.y				= 480-64;
	soundOptionsInfo.back.width					= 128;
	soundOptionsInfo.back.height				= 64;
	soundOptionsInfo.back.focuspic				= ART_BACK1;

	soundOptionsInfo.apply.generic.type			= MTYPE_BITMAP;
	soundOptionsInfo.apply.generic.name			= ART_ACCEPT0;
	soundOptionsInfo.apply.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_HIDDEN|QMF_INACTIVE;
	soundOptionsInfo.apply.generic.callback		= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.apply.generic.id			= ID_APPLY;
	soundOptionsInfo.apply.generic.x			= 640;
	soundOptionsInfo.apply.generic.y			= 480-64;
	soundOptionsInfo.apply.width				= 128;
	soundOptionsInfo.apply.height				= 64;
	soundOptionsInfo.apply.focuspic				= ART_ACCEPT1;
*/
	soundOptionsInfo.back.generic.type			= MTYPE_PTEXT;
	soundOptionsInfo.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	soundOptionsInfo.back.generic.x				= 20;
	soundOptionsInfo.back.generic.y				= 480 - 50;
	soundOptionsInfo.back.generic.id			= ID_BACK;
	soundOptionsInfo.back.generic.callback		= UI_SoundOptionsMenu_Event; 
	soundOptionsInfo.back.string				= "< BACK";
	soundOptionsInfo.back.color					= text_color_normal;
	soundOptionsInfo.back.style					= UI_LEFT | UI_SMALLFONT;
// END

	soundOptionsInfo.apply.generic.type			= MTYPE_PTEXT;
	soundOptionsInfo.apply.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_HIDDEN|QMF_INACTIVE;
	soundOptionsInfo.apply.generic.x			= 620;
	soundOptionsInfo.apply.generic.y			= 480 - 50;
	soundOptionsInfo.apply.generic.id			= ID_APPLY;
	soundOptionsInfo.apply.generic.callback		= UI_SoundOptionsMenu_Event; 
	soundOptionsInfo.apply.string				= "APPLY";
	soundOptionsInfo.apply.color				= text_color_normal;
	soundOptionsInfo.apply.style				= UI_RIGHT | UI_SMALLFONT;

// STONELANCE
/*
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.framel );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.framer );
*/
// END
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.sfxvolume );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.musicvolume );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.soundSystem );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.quality );
//	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.a3d );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.back );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.apply );

	soundOptionsInfo.sfxvolume.curvalue = soundOptionsInfo.sfxvolume_original = trap_Cvar_VariableValue( "s_volume" ) * 10;
	soundOptionsInfo.musicvolume.curvalue = soundOptionsInfo.musicvolume_original = trap_Cvar_VariableValue( "s_musicvolume" ) * 10;

	if (trap_Cvar_VariableValue( "s_useOpenAL" ))
		soundOptionsInfo.soundSystem_original = UISND_OPENAL;
	else
		soundOptionsInfo.soundSystem_original = UISND_SDL;

	soundOptionsInfo.soundSystem.curvalue = soundOptionsInfo.soundSystem_original;

	speed = trap_Cvar_VariableValue( "s_sdlSpeed" );
	if (!speed) // Check for default
		speed = DEFAULT_SDL_SND_SPEED;

	if (speed <= 11025)
		soundOptionsInfo.quality_original = 0;
	else if (speed <= 22050)
		soundOptionsInfo.quality_original = 1;
	else // 44100
		soundOptionsInfo.quality_original = 2;
	soundOptionsInfo.quality.curvalue = soundOptionsInfo.quality_original;

	/* Menu_AddItem initializes the legacy widgets and overwrites their
	 * default bounds. Apply the frontend layout after that initialization. */
	Sound_SetSliderBounds( &soundOptionsInfo.sfxvolume,
		ID_EFFECTSVOLUME, "Effects volume", SOUND_ROW_START_Y );
	Sound_SetSliderBounds( &soundOptionsInfo.musicvolume,
		ID_MUSICVOLUME, "Music volume",
		SOUND_ROW_START_Y + SOUND_ROW_HEIGHT + SOUND_ROW_GAP );
	Sound_SetSettingBounds( &soundOptionsInfo.soundSystem.generic,
		ID_SOUNDSYSTEM, "Sound system",
		SOUND_ROW_START_Y + 2 * ( SOUND_ROW_HEIGHT + SOUND_ROW_GAP ) );
	Sound_SetSettingBounds( &soundOptionsInfo.quality.generic,
		ID_QUALITY, "SDL quality",
		SOUND_ROW_START_Y + 3 * ( SOUND_ROW_HEIGHT + SOUND_ROW_GAP ) );

	Sound_SetBounds( &soundOptionsInfo.back.generic, ID_BACK,
		SOUND_NAV_X, SOUND_ACTION_Y, SOUND_ACTION_WIDTH,
		SOUND_ACTION_HEIGHT, NULL );
	soundOptionsInfo.back.generic.ownerdraw = Sound_DrawAction;
	Sound_SetBounds( &soundOptionsInfo.apply.generic, ID_APPLY,
		SOUND_FRAME_X + SOUND_FRAME_WIDTH - SOUND_ACTION_WIDTH,
		SOUND_ACTION_Y, SOUND_ACTION_WIDTH, SOUND_ACTION_HEIGHT, NULL );
	soundOptionsInfo.apply.generic.ownerdraw = Sound_DrawAction;

//	soundOptionsInfo.a3d.curvalue = (int)trap_Cvar_VariableValue( "s_usingA3D" );
}


/*
===============
UI_SoundOptionsMenu_Cache
===============
*/
void UI_SoundOptionsMenu_Cache( void ) {
// STONELANCE
/*
	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
	trap_R_RegisterShaderNoMip( ART_ACCEPT0 );
	trap_R_RegisterShaderNoMip( ART_ACCEPT1 );
*/
// END
}


/*
===============
UI_SoundOptionsMenu
===============
*/
void UI_SoundOptionsMenu( void ) {
// STONELANCE FIXME: get rid of this after proper tansitions are added
	uis.transitionIn = 0;
// END

	UI_SoundOptionsMenu_Init();
	UI_PushMenu( &soundOptionsInfo.menu );
	Menu_SetCursorToItem( &soundOptionsInfo.menu, &soundOptionsInfo.sfxvolume );
}
