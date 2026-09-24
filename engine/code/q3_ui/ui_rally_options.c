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

#include "ui_local.h"
#include "ui_rally_frontend.h"

qboolean isRaceObserver( int clientNum )
{
	return qfalse;
}

#define ID_UNITS                10
#define ID_SKID_LENGTH          11
#define ID_MANUAL_SHIFT         13
#define ID_TRANSMISSION_MODE    14
#define ID_ATMOSPHERIC_LEVEL	16
#define ID_POSITION_SPRITES     17
#define ID_CAM_TRACKING         18
#define ID_ENGINE_SOUNDS        19
#define ID_RVRL_PLAYERS         21
#define ID_RVRL_OBJECTS         22
#define ID_RVRL_SMOKE           23
#define ID_RVRL_MARKS           24
#define ID_RVRL_SPARKS          25

#define ID_SPEEDOMETER_MODE     28
#define ID_FUEL_CONSUMPTION     29

#define ID_MVRL_PLAYERS         30
#define ID_MVRL_OBJECTS         31
#define ID_MVRL_SMOKE           32
#define ID_MVRL_MARKS           33
#define ID_MVRL_SPARKS          34
#define ID_GHOST_PLAYBACK       35

#define ID_LADDER_OFFLINE       41
#define ID_BACK                 40

#define Q3ROPTIONS_TAB_TOP           64
#define Q3ROPTIONS_TAB_HEIGHT        32
#define Q3ROPTIONS_BACK_BUTTON_LEFT  24
#define Q3ROPTIONS_BACK_BUTTON_Y     ( Q3ROPTIONS_TAB_TOP + Q3ROPTIONS_TAB_HEIGHT + 14 )

#define Q3R_OPTIONS_FRAME_X          24
#define Q3R_OPTIONS_FRAME_Y          20
#define Q3R_OPTIONS_FRAME_WIDTH      592
#define Q3R_OPTIONS_FRAME_HEIGHT     440
#define Q3R_OPTIONS_LEFT_X           40
#define Q3R_OPTIONS_RIGHT_X          332
#define Q3R_OPTIONS_CARD_Y           104
#define Q3R_OPTIONS_CARD_WIDTH       268
#define Q3R_OPTIONS_CARD_HEIGHT      300
#define Q3R_OPTIONS_ROW_X_INSET      12
#define Q3R_OPTIONS_ROW_WIDTH        ( Q3R_OPTIONS_CARD_WIDTH - 24 )
#define Q3R_OPTIONS_ROW_HEIGHT       18
#define Q3R_OPTIONS_ROW_STEP         20
#define Q3R_OPTIONS_TOP_ROW_Y        148
#define Q3R_OPTIONS_MAIN_HEADING_Y   288
#define Q3R_OPTIONS_MAIN_ROW_Y       310
#define Q3R_OPTIONS_MAIN_ROW_STEP    18
#define Q3R_OPTIONS_REAR_HEADING_Y   260
#define Q3R_OPTIONS_REAR_ROW_Y       280
#define Q3R_OPTIONS_REAR_ROW_STEP    18
#define Q3R_OPTIONS_ACTION_Y         420

static vec4_t q3rOptionsScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t q3rOptionsTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t q3rOptionsMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t q3rOptionsAccentColor = UI_FRONTEND_COLOR_ACCENT;
static vec4_t q3rOptionsFocusColor = UI_FRONTEND_COLOR_FOCUS_BG;
static vec4_t q3rOptionsBorderColor = UI_FRONTEND_COLOR_BORDER;

static void Q3ROptions_MenuDraw( void );
static void Q3ROptions_DrawRadio( void *self );
static void Q3ROptions_DrawChoice( void *self );
static void Q3ROptions_DrawSlider( void *self );
static void Q3ROptions_DrawAction( void *self );

typedef struct {
	menuframework_s	menu;

	menutext_s		banner;

	menulist_s		units;
        menulist_s              speedometer;
	menulist_s		transmissionMode;
	menulist_s		atomspheric;

	menuradiobutton_s	manualShift;
	menuradiobutton_s	positionSprites;
	menuslider_s		skidlength;
	menuslider_s		camtracking;

	menuradiobutton_s	rvrl_players;
	menuradiobutton_s	rvrl_objects;
	menuradiobutton_s	rvrl_smoke;
	menuradiobutton_s	rvrl_marks;
	menuradiobutton_s	rvrl_sparks;

	menuradiobutton_s	mvrl_players;
	menuradiobutton_s	mvrl_objects;
	menuradiobutton_s	mvrl_smoke;
	menuradiobutton_s	mvrl_marks;
	menuradiobutton_s	mvrl_sparks;

	menutext_s		rvrl_heading;
	menutext_s		mvrl_heading;

	menulist_s		engineSounds;
	menulist_s		ghostPlayback;
	menuradiobutton_s	fuelConsumption;
	menuradiobutton_s	ladderOffline;

	menutext_s		back;
} q3roptionsmenu_t;

static q3roptionsmenu_t	s_q3roptions;

static const char *q3roptions_units[] = {
        "Imperial",
        "Metric",
        0
};

static const char *q3roptions_speedometer_mode[] = {
        "Analog",
        "Digital",
        0
};

static const char *q3roptions_transmission_mode[] = {
	"Automatic",
	"Manual",
	"Manual + Clutch",
	0
};

static const char *q3roptions_atmospheric[] = {
        "None",
        "Low",
        "High",
        "Ultra",
        0
};

static const char *q3roptions_ghostPlayback[] = {
        "Off",
        "Personal",
        "Server base",
        0
};

static const char *q3roptions_engine_sounds[] = {
	"Off",
	"Legacy",
	"Experimental",
	0
};


/*
=================
Q3ROptions_MenuEvent
=================
*/
static void Q3ROptions_MenuEvent( void* ptr, int event ) {
	int		value;


	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id )
	{
        case ID_UNITS:
                trap_Cvar_SetValue( "cg_metricUnits", s_q3roptions.units.curvalue );
                break;

        case ID_SPEEDOMETER_MODE:
                trap_Cvar_SetValue( "cg_speedometerMode", s_q3roptions.speedometer.curvalue );
                break;

	case ID_TRANSMISSION_MODE:
		trap_Cvar_SetValue( "cg_transmissionMode", s_q3roptions.transmissionMode.curvalue );
		break;

	case ID_ATMOSPHERIC_LEVEL:
		trap_Cvar_SetValue( "cg_atmosphericLevel", s_q3roptions.atomspheric.curvalue );
		break;



	case ID_MANUAL_SHIFT:
		trap_Cvar_SetValue( "cg_manualShift", s_q3roptions.manualShift.curvalue );
		break;

	case ID_POSITION_SPRITES:
		trap_Cvar_SetValue( "cg_drawPositionSprites", s_q3roptions.positionSprites.curvalue );
		break;

	case ID_SKID_LENGTH:
		trap_Cvar_SetValue( "cg_minSkidLength", s_q3roptions.skidlength.curvalue );
		break;

	case ID_CAM_TRACKING:
		trap_Cvar_SetValue( "cg_tightCamTracking", s_q3roptions.camtracking.curvalue );
		break;

	case ID_ENGINE_SOUNDS:
		if ( s_q3roptions.engineSounds.curvalue <= 0 ) {
			trap_Cvar_SetValue( "cg_engineSounds", 0 );
		} else {
			trap_Cvar_SetValue( "cg_engineSounds", 1 );
			trap_Cvar_SetValue( "cg_engineAudioMode", ( s_q3roptions.engineSounds.curvalue == 2 ) ? 2 : 1 );
		}
		break;

	case ID_GHOST_PLAYBACK:
		trap_Cvar_SetValue( "cg_ghostPlayback", s_q3roptions.ghostPlayback.curvalue );
		break;

	case ID_FUEL_CONSUMPTION:
		trap_Cvar_SetValue( "g_useFuel", s_q3roptions.fuelConsumption.curvalue );
		break;

	case ID_LADDER_OFFLINE:
		trap_Cvar_SetValue( "sv_ladderEnabled", s_q3roptions.ladderOffline.curvalue );
		break;

	case ID_RVRL_PLAYERS:
	case ID_RVRL_OBJECTS:
	case ID_RVRL_SMOKE:
	case ID_RVRL_MARKS:
	case ID_RVRL_SPARKS:
		value = 0;
		value |= ( s_q3roptions.rvrl_players.curvalue ) ? RL_PLAYERS : 0;
		value |= ( s_q3roptions.rvrl_objects.curvalue ) ? RL_OBJECTS : 0;
		value |= ( s_q3roptions.rvrl_smoke.curvalue ) ? RL_SMOKE : 0;
		value |= ( s_q3roptions.rvrl_marks.curvalue ) ? RL_MARKS : 0;
		value |= ( s_q3roptions.rvrl_sparks.curvalue ) ? RL_SPARKS : 0;

		trap_Cvar_SetValue( "cg_rearViewRenderLevel", value );
		break;



	case ID_MVRL_PLAYERS:
	case ID_MVRL_OBJECTS:
	case ID_MVRL_SMOKE:
	case ID_MVRL_MARKS:
	case ID_MVRL_SPARKS:
		value = 0;
		value |= ( s_q3roptions.mvrl_players.curvalue ) ? RL_PLAYERS : 0;
		value |= ( s_q3roptions.mvrl_objects.curvalue ) ? RL_OBJECTS : 0;
		value |= ( s_q3roptions.mvrl_smoke.curvalue ) ? RL_SMOKE : 0;
		value |= ( s_q3roptions.mvrl_marks.curvalue ) ? RL_MARKS : 0;
		value |= ( s_q3roptions.mvrl_sparks.curvalue ) ? RL_SPARKS : 0;

		trap_Cvar_SetValue( "cg_mainViewRenderLevel", value );
		break;


	case ID_BACK:
		UI_PopMenu();
		break;
	}
}


/*
=================
Q3ROptions_StatusBar
=================
*/
static void Q3ROptions_StatusBar( void *self )
{
	const char *text;

	text = "Left / right adjust   Enter select   Esc back";

	switch( ((menucommon_s*)self)->id )
	{
        case ID_UNITS:
                text = "Choose metric or imperial speed units.";
                break;

        case ID_SPEEDOMETER_MODE:
                text = "Choose an analog or digital speedometer.";
                break;

	case ID_TRANSMISSION_MODE:
		text = "Choose automatic, manual or manual + clutch.";
		break;

	case ID_ATMOSPHERIC_LEVEL:
		text = "Set the amount of atmospheric effects.";
		break;



	case ID_MANUAL_SHIFT:
		text = "Choose how forward and reverse are selected.";
		break;

	case ID_POSITION_SPRITES:
		text = "Show race position markers above vehicles.";
		break;

	case ID_SKID_LENGTH:
		text = "Set the length of each skid segment.";
		break;

	case ID_CAM_TRACKING:
		text = "Set how tightly the camera follows the vehicle.";
		break;

	case ID_ENGINE_SOUNDS:
		text = "Choose off, legacy or experimental engine audio.";
		break;

	case ID_GHOST_PLAYBACK:
		text = "Personal uses a saved ghost for this vehicle; Server base uses the route provided by the server.";
		break;

	case ID_FUEL_CONSUMPTION:
		text = "Toggle fuel consumption during races.";
		break;

	case ID_LADDER_OFFLINE:
		text = "Allow offline results to sync to the Q3Rally Ladder.";
		break;

	case ID_RVRL_PLAYERS:
		text = "Show other players in the rear view mirror.";
		break;
	case ID_RVRL_OBJECTS:
		text = "Show map objects in the rear view mirror.";
		break;
	case ID_RVRL_SMOKE:
		text = "Show smoke in the rear view mirror.";
		break;
	case ID_RVRL_MARKS:
		text = "Show skidmarks in the rear view mirror.";
		break;
	case ID_RVRL_SPARKS:
		text = "Show sparks in the rear view mirror.";
		break;



	case ID_MVRL_PLAYERS:
		text = "Show other players in the main view.";
		break;
	case ID_MVRL_OBJECTS:
		text = "Show map objects in the main view.";
		break;
	case ID_MVRL_SMOKE:
		text = "Show smoke in the main view.";
		break;
	case ID_MVRL_MARKS:
		text = "Show marks in the main view.";
		break;
	case ID_MVRL_SPARKS:
		text = "Show sparks in the main view.";
		break;
	}

	Frontend_DrawText( Q3R_OPTIONS_FRAME_X + 24,
		Q3R_OPTIONS_FRAME_Y + 384, text,
		UI_LEFT | UI_SMALLFONT, q3rOptionsMutedColor );
}

static void Q3ROptions_SetBounds( menucommon_s *item, int x, int y,
	int width, int height, const char *label ) {
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

static void Q3ROptions_DrawRow( menucommon_s *item, const char *value ) {
	qboolean focus;
	qboolean disabled;
	vec4_t labelColor;
	vec4_t valueColor;

	focus = ( Menu_ItemAtCursor( item->parent ) == item );
	disabled = ( item->flags & ( QMF_GRAYED | QMF_INACTIVE ) ) ? qtrue : qfalse;

	if ( focus && !disabled ) {
		UI_FillRect( item->left, item->top,
			item->right - item->left, item->bottom - item->top,
			q3rOptionsFocusColor );
		UI_FillRect( item->left, item->top, 2,
			item->bottom - item->top, q3rOptionsAccentColor );
	}
	UI_FillRect( item->left, item->bottom - 1,
		item->right - item->left, 1, q3rOptionsBorderColor );

	Vector4Copy( disabled ? q3rOptionsMutedColor :
		(focus ? q3rOptionsTextColor : q3rOptionsMutedColor), labelColor );
	Vector4Copy( disabled ? q3rOptionsMutedColor :
		(focus ? q3rOptionsAccentColor : q3rOptionsTextColor), valueColor );
	Frontend_DrawText( item->left + 10,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		item->name, UI_LEFT | UI_SMALLFONT, labelColor );
	Frontend_DrawText( item->right - 10,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		value, UI_RIGHT | UI_SMALLFONT, valueColor );
}

static void Q3ROptions_DrawRadio( void *self ) {
	menuradiobutton_s *radio;

	radio = (menuradiobutton_s *)self;
	Q3ROptions_DrawRow( &radio->generic,
		radio->curvalue ? "On" : "Off" );
}

static void Q3ROptions_DrawChoice( void *self ) {
	menulist_s *choice;
	const char *value;

	choice = (menulist_s *)self;
	value = choice->itemnames[choice->curvalue];
	Q3ROptions_DrawRow( &choice->generic, value ? value : "-" );
}

static void Q3ROptions_DrawSlider( void *self ) {
	menuslider_s *slider;
	menucommon_s *item;
	qboolean focus;
	qboolean disabled;
	vec4_t labelColor;
	vec4_t valueColor;
	vec4_t trackColor;
	float range;
	int trackX;
	int trackWidth;
	int trackY;
	char value[32];

	slider = (menuslider_s *)self;
	item = &slider->generic;
	focus = ( Menu_ItemAtCursor( item->parent ) == item );
	disabled = ( item->flags & ( QMF_GRAYED | QMF_INACTIVE ) ) ? qtrue : qfalse;

	if ( focus && !disabled ) {
		UI_FillRect( item->left, item->top,
			item->right - item->left, item->bottom - item->top,
			q3rOptionsFocusColor );
		UI_FillRect( item->left, item->top, 2,
			item->bottom - item->top, q3rOptionsAccentColor );
	}
	UI_FillRect( item->left, item->bottom - 1,
		item->right - item->left, 1, q3rOptionsBorderColor );

	Vector4Copy( disabled ? q3rOptionsMutedColor :
		(focus ? q3rOptionsTextColor : q3rOptionsMutedColor), labelColor );
	Vector4Copy( disabled ? q3rOptionsMutedColor :
		(focus ? q3rOptionsAccentColor : q3rOptionsTextColor), valueColor );
	Vector4Copy( disabled ? q3rOptionsMutedColor : q3rOptionsBorderColor, trackColor );

	if ( slider->maxvalue > slider->minvalue ) {
		range = ( slider->curvalue - slider->minvalue ) /
			( slider->maxvalue - slider->minvalue );
	} else {
		range = 0.0f;
	}
	if ( range < 0.0f ) range = 0.0f;
	if ( range > 1.0f ) range = 1.0f;
	slider->range = range;

	Com_sprintf( value, sizeof( value ), "%d", (int)( slider->curvalue + 0.5f ) );
	Frontend_DrawText( item->left + 10,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		item->name, UI_LEFT | UI_SMALLFONT, labelColor );

	trackX = item->left + 132;
	trackWidth = 76;
	trackY = item->top + ( item->bottom - item->top ) / 2;
	UI_FillRect( trackX, trackY, trackWidth, 2, trackColor );
	UI_FillRect( trackX, trackY, (int)( trackWidth * range ), 2,
		disabled ? q3rOptionsMutedColor : q3rOptionsAccentColor );
	UI_FillRect( trackX + (int)( trackWidth * range ) - 2, trackY - 3,
		4, 8, disabled ? q3rOptionsMutedColor : q3rOptionsAccentColor );
	Frontend_DrawText( item->right - 10,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		value, UI_RIGHT | UI_SMALLFONT, valueColor );
}

static void Q3ROptions_DrawAction( void *self ) {
	menutext_s *action;
	qboolean focus;

	action = (menutext_s *)self;
	focus = ( Menu_ItemAtCursor( action->generic.parent ) == &action->generic );
	Frontend_DrawButton( action->generic.left, action->generic.top,
		action->generic.right - action->generic.left,
		action->generic.bottom - action->generic.top,
		action->string, 1.0f, focus, UI_CENTER );
}

static void Q3ROptions_MenuDraw( void ) {
	Frontend_DrawBackground( q3rOptionsScrimColor );
	Frontend_DrawPanel( Q3R_OPTIONS_FRAME_X, Q3R_OPTIONS_FRAME_Y,
		Q3R_OPTIONS_FRAME_WIDTH, Q3R_OPTIONS_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( Q3R_OPTIONS_FRAME_X + 24,
		Q3R_OPTIONS_FRAME_Y + 24, "Q3Rally options",
		UI_LEFT | UI_BIGFONT, q3rOptionsTextColor );
	Frontend_DrawText( Q3R_OPTIONS_FRAME_X + 24,
		Q3R_OPTIONS_FRAME_Y + 48,
		"Personalize driving feel, HUD and vehicle rendering",
		UI_LEFT | UI_SMALLFONT, q3rOptionsMutedColor );
	Frontend_DrawStatusChip( Q3R_OPTIONS_FRAME_X + Q3R_OPTIONS_FRAME_WIDTH - 88,
		Q3R_OPTIONS_FRAME_Y + 26, "Q3R",
		q3rOptionsAccentColor, 1.0f );

	Frontend_DrawCard( Q3R_OPTIONS_LEFT_X, Q3R_OPTIONS_CARD_Y,
		Q3R_OPTIONS_CARD_WIDTH, Q3R_OPTIONS_CARD_HEIGHT, 1.0f, qfalse );
	Frontend_DrawCard( Q3R_OPTIONS_RIGHT_X, Q3R_OPTIONS_CARD_Y,
		Q3R_OPTIONS_CARD_WIDTH, Q3R_OPTIONS_CARD_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( Q3R_OPTIONS_LEFT_X + 12,
		Q3R_OPTIONS_CARD_Y + 22, "Driving & simulation",
		UI_LEFT | UI_SMALLFONT, q3rOptionsMutedColor );
	Frontend_DrawText( Q3R_OPTIONS_RIGHT_X + 12,
		Q3R_OPTIONS_CARD_Y + 22, "HUD & audio",
		UI_LEFT | UI_SMALLFONT, q3rOptionsMutedColor );
	Frontend_DrawText( Q3R_OPTIONS_LEFT_X + 12,
		Q3R_OPTIONS_MAIN_HEADING_Y, "Main view",
		UI_LEFT | UI_SMALLFONT, q3rOptionsMutedColor );
	Frontend_DrawText( Q3R_OPTIONS_RIGHT_X + 12,
		Q3R_OPTIONS_REAR_HEADING_Y, "Rear view",
		UI_LEFT | UI_SMALLFONT, q3rOptionsMutedColor );
	Menu_Draw( &s_q3roptions.menu );
}


/*
===============
Q3ROptions_MenuInit
===============
*/
void Q3ROptions_MenuInit( void ) {
//	int				y;

	memset( &s_q3roptions, 0, sizeof(q3roptionsmenu_t) );
	s_q3roptions.menu.draw = Q3ROptions_MenuDraw;
	s_q3roptions.menu.wrapAround = qtrue;
	s_q3roptions.menu.fullscreen = qtrue;


	// load current values
        s_q3roptions.units.curvalue = ui_metricUnits.integer;
        s_q3roptions.speedometer.curvalue = ui_speedometerMode.integer;
	s_q3roptions.transmissionMode.curvalue = (int)Com_Clamp( 0, 2, ui_transmissionMode.integer );
	s_q3roptions.atomspheric.curvalue = ui_atmosphericLevel.integer;

	s_q3roptions.manualShift.curvalue = ui_manualShift.integer;
	s_q3roptions.positionSprites.curvalue = ui_drawPositionSprites.integer;

	s_q3roptions.skidlength.curvalue = ui_minSkidLength.integer;
	s_q3roptions.camtracking.curvalue = ui_tightCamTracking.integer;

	if ( !ui_engineSounds.integer ) {
		s_q3roptions.engineSounds.curvalue = 0;
	} else {
		s_q3roptions.engineSounds.curvalue = ( Com_Clamp( 1, 2, ui_engineAudioMode.integer ) == 2 ) ? 2 : 1;
	}
	s_q3roptions.ghostPlayback.curvalue = Com_Clamp( 0, 2, ui_ghostPlayback.integer );
	s_q3roptions.fuelConsumption.curvalue = ui_useFuel.integer;
	s_q3roptions.ladderOffline.curvalue = trap_Cvar_VariableValue( "sv_ladderEnabled" ) != 0 ? 1 : 0;

	s_q3roptions.rvrl_players.curvalue = ( ui_rearViewRenderLevel.integer & RL_PLAYERS ) ? 1 : 0;
	s_q3roptions.rvrl_objects.curvalue = ( ui_rearViewRenderLevel.integer & RL_OBJECTS ) ? 1 : 0;
	s_q3roptions.rvrl_smoke.curvalue = ( ui_rearViewRenderLevel.integer & RL_SMOKE ) ? 1 : 0;
	s_q3roptions.rvrl_marks.curvalue = ( ui_rearViewRenderLevel.integer & RL_MARKS ) ? 1 : 0;
	s_q3roptions.rvrl_sparks.curvalue = ( ui_rearViewRenderLevel.integer & RL_SPARKS ) ? 1 : 0;

	s_q3roptions.mvrl_players.curvalue = ( ui_mainViewRenderLevel.integer & RL_PLAYERS ) ? 1 : 0;
	s_q3roptions.mvrl_objects.curvalue = ( ui_mainViewRenderLevel.integer & RL_OBJECTS ) ? 1 : 0;
	s_q3roptions.mvrl_smoke.curvalue = ( ui_mainViewRenderLevel.integer & RL_SMOKE ) ? 1 : 0;
	s_q3roptions.mvrl_marks.curvalue = ( ui_mainViewRenderLevel.integer & RL_MARKS ) ? 1 : 0;
	s_q3roptions.mvrl_sparks.curvalue = ( ui_mainViewRenderLevel.integer & RL_SPARKS ) ? 1 : 0;






	s_q3roptions.banner.generic.type	= MTYPE_BTEXT;
	s_q3roptions.banner.generic.flags	= QMF_CENTER_JUSTIFY;
	s_q3roptions.banner.generic.x		= 320;
	s_q3roptions.banner.generic.y		= 16;
	s_q3roptions.banner.string		    = "Q3R OPTIONS ";
	s_q3roptions.banner.color			= color_white;
	s_q3roptions.banner.style			= UI_CENTER;



	/* ----------------------------------------------------------------
	   Layout constants
	   Screen: 640x480. Two columns.
	   Left  column anchor: x=200  (Gameplay)
	   Right column anchor: x=500  (Visual / Audio)
	   Items start at y=TOP, step by STEP.
	   Render-level groups sit below the divider at y=RLY.
	   ---------------------------------------------------------------- */
#define LAY_TOP   100
#define LAY_STEP   22
#define LAY_L     200
#define LAY_R     500
#define LAY_RLY   306
#define LAY_SLD_Y ( LAY_TOP + LAY_STEP * 6 + 8 )   /* Skid + Camera Tracking row */
#define LAY_HDG_Y ( LAY_RLY - 26 )                  /* Render level headings */

	// ---- LEFT COLUMN: Gameplay ----

	s_q3roptions.transmissionMode.generic.type		= MTYPE_SPINCONTROL;
	s_q3roptions.transmissionMode.generic.name		= "Transmission:";
	s_q3roptions.transmissionMode.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_q3roptions.transmissionMode.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.transmissionMode.generic.statusbar	= Q3ROptions_StatusBar;
	s_q3roptions.transmissionMode.generic.id		= ID_TRANSMISSION_MODE;
	s_q3roptions.transmissionMode.generic.x			= LAY_L;
	s_q3roptions.transmissionMode.generic.y			= LAY_TOP + LAY_STEP * 0;
	s_q3roptions.transmissionMode.itemnames			= q3roptions_transmission_mode;

	s_q3roptions.manualShift.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.manualShift.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.manualShift.generic.x			= LAY_L;
	s_q3roptions.manualShift.generic.y			= LAY_TOP + LAY_STEP * 1;
	s_q3roptions.manualShift.generic.name		= "Manual F/R Select:";
	s_q3roptions.manualShift.generic.id			= ID_MANUAL_SHIFT;
	s_q3roptions.manualShift.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.manualShift.generic.statusbar	= Q3ROptions_StatusBar;

	s_q3roptions.atomspheric.generic.type		= MTYPE_SPINCONTROL;
	s_q3roptions.atomspheric.generic.name		= "Atmospheric Effects:";
	s_q3roptions.atomspheric.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_q3roptions.atomspheric.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.atomspheric.generic.statusbar	= Q3ROptions_StatusBar;
	s_q3roptions.atomspheric.generic.id			= ID_ATMOSPHERIC_LEVEL;
	s_q3roptions.atomspheric.generic.x			= LAY_L;
	s_q3roptions.atomspheric.generic.y			= LAY_TOP + LAY_STEP * 2;
	s_q3roptions.atomspheric.itemnames			= q3roptions_atmospheric;

	s_q3roptions.ghostPlayback.generic.type		= MTYPE_SPINCONTROL;
	s_q3roptions.ghostPlayback.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_q3roptions.ghostPlayback.generic.x		= LAY_L;
	s_q3roptions.ghostPlayback.generic.y		= LAY_TOP + LAY_STEP * 3;
	s_q3roptions.ghostPlayback.generic.name		= "Ghost Playback:";
	s_q3roptions.ghostPlayback.generic.id		= ID_GHOST_PLAYBACK;
	s_q3roptions.ghostPlayback.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.ghostPlayback.generic.statusbar = Q3ROptions_StatusBar;
	s_q3roptions.ghostPlayback.itemnames		= q3roptions_ghostPlayback;

	s_q3roptions.fuelConsumption.generic.type	= MTYPE_RADIOBUTTON;
	s_q3roptions.fuelConsumption.generic.flags	= QMF_SMALLFONT;
	s_q3roptions.fuelConsumption.generic.x		= LAY_L;
	s_q3roptions.fuelConsumption.generic.y		= LAY_TOP + LAY_STEP * 4;
	s_q3roptions.fuelConsumption.generic.name	= "Fuel Consumption:";
	s_q3roptions.fuelConsumption.generic.id		= ID_FUEL_CONSUMPTION;
	s_q3roptions.fuelConsumption.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.fuelConsumption.generic.statusbar	= Q3ROptions_StatusBar;

	/* Q3RALLY LADDER START */
	s_q3roptions.ladderOffline.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.ladderOffline.generic.flags	= QMF_SMALLFONT;
	s_q3roptions.ladderOffline.generic.x		= LAY_R;
	s_q3roptions.ladderOffline.generic.y		= LAY_TOP + LAY_STEP * 4;
	s_q3roptions.ladderOffline.generic.name		= "Ladder Offline Tracking:";
	s_q3roptions.ladderOffline.generic.id		= ID_LADDER_OFFLINE;
	s_q3roptions.ladderOffline.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.ladderOffline.generic.statusbar	= Q3ROptions_StatusBar;
	/* Q3RALLY LADDER END */

	s_q3roptions.skidlength.generic.type		= MTYPE_SLIDER;
	s_q3roptions.skidlength.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.skidlength.generic.x			= LAY_L;
	s_q3roptions.skidlength.generic.y			= LAY_SLD_Y;
	s_q3roptions.skidlength.generic.name		= "Skid Segment Length:";
	s_q3roptions.skidlength.generic.id			= ID_SKID_LENGTH;
	s_q3roptions.skidlength.minvalue			= 4;
	s_q3roptions.skidlength.maxvalue			= 24;
	s_q3roptions.skidlength.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.skidlength.generic.ownerdraw	= UI_RallySlider_Draw;
	s_q3roptions.skidlength.generic.statusbar	= Q3ROptions_StatusBar;

	// ---- RIGHT COLUMN: Visual / Audio ----

	s_q3roptions.speedometer.generic.type		= MTYPE_SPINCONTROL;
	s_q3roptions.speedometer.generic.name		= "Speedometer Mode:";
	s_q3roptions.speedometer.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_q3roptions.speedometer.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.speedometer.generic.statusbar	= Q3ROptions_StatusBar;
	s_q3roptions.speedometer.generic.id			= ID_SPEEDOMETER_MODE;
	s_q3roptions.speedometer.generic.x			= LAY_R;
	s_q3roptions.speedometer.generic.y			= LAY_TOP + LAY_STEP * 0;
	s_q3roptions.speedometer.itemnames			= q3roptions_speedometer_mode;

	s_q3roptions.units.generic.type				= MTYPE_SPINCONTROL;
	s_q3roptions.units.generic.name				= "Unit Type:";
	s_q3roptions.units.generic.flags			= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_q3roptions.units.generic.callback			= Q3ROptions_MenuEvent;
	s_q3roptions.units.generic.statusbar		= Q3ROptions_StatusBar;
	s_q3roptions.units.generic.id				= ID_UNITS;
	s_q3roptions.units.generic.x				= LAY_R;
	s_q3roptions.units.generic.y				= LAY_TOP + LAY_STEP * 1;
	s_q3roptions.units.itemnames				= q3roptions_units;

	s_q3roptions.engineSounds.generic.type		= MTYPE_SPINCONTROL;
	s_q3roptions.engineSounds.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_q3roptions.engineSounds.generic.x			= LAY_R;
	s_q3roptions.engineSounds.generic.y			= LAY_TOP + LAY_STEP * 2;
	s_q3roptions.engineSounds.generic.name		= "Engine Sounds:";
	s_q3roptions.engineSounds.generic.id		= ID_ENGINE_SOUNDS;
	s_q3roptions.engineSounds.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.engineSounds.generic.statusbar	= Q3ROptions_StatusBar;
	s_q3roptions.engineSounds.itemnames			= q3roptions_engine_sounds;

	s_q3roptions.positionSprites.generic.type	= MTYPE_RADIOBUTTON;
	s_q3roptions.positionSprites.generic.flags	= QMF_SMALLFONT;
	s_q3roptions.positionSprites.generic.x		= LAY_R;
	s_q3roptions.positionSprites.generic.y		= LAY_TOP + LAY_STEP * 3;
	s_q3roptions.positionSprites.generic.name	= "Race Position Sprites:";
	s_q3roptions.positionSprites.generic.id		= ID_POSITION_SPRITES;
	s_q3roptions.positionSprites.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.positionSprites.generic.statusbar	= Q3ROptions_StatusBar;

	s_q3roptions.camtracking.generic.type		= MTYPE_SLIDER;
	s_q3roptions.camtracking.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.camtracking.generic.x			= LAY_R;
	s_q3roptions.camtracking.generic.y			= LAY_SLD_Y;
	s_q3roptions.camtracking.generic.name		= "Camera Tracking Scale:";
	s_q3roptions.camtracking.generic.id			= ID_CAM_TRACKING;
	s_q3roptions.camtracking.minvalue			= 0;
	s_q3roptions.camtracking.maxvalue			= 5;
	s_q3roptions.camtracking.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.camtracking.generic.ownerdraw	= UI_RallySlider_Draw;
	s_q3roptions.camtracking.generic.statusbar	= Q3ROptions_StatusBar;

	// ---- RENDER LEVEL GROUPS (below both columns) ----

	s_q3roptions.rvrl_heading.generic.type		= MTYPE_PTEXT;
	s_q3roptions.rvrl_heading.generic.flags		= QMF_CENTER_JUSTIFY|QMF_INACTIVE;
	s_q3roptions.rvrl_heading.generic.x			= LAY_R - 20;
	s_q3roptions.rvrl_heading.generic.y			= LAY_HDG_Y;
	s_q3roptions.rvrl_heading.generic.id		= 0;
	s_q3roptions.rvrl_heading.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.rvrl_heading.string			= "Rear View Render Level:";
	s_q3roptions.rvrl_heading.color				= text_color_normal;
	s_q3roptions.rvrl_heading.style				= UI_CENTER | UI_SMALLFONT;

	s_q3roptions.rvrl_players.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.rvrl_players.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.rvrl_players.generic.x			= LAY_R;
	s_q3roptions.rvrl_players.generic.y			= LAY_RLY + LAY_STEP * 0;
	s_q3roptions.rvrl_players.generic.name		= "Players:";
	s_q3roptions.rvrl_players.generic.id		= ID_RVRL_PLAYERS;
	s_q3roptions.rvrl_players.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.rvrl_players.generic.statusbar	= Q3ROptions_StatusBar;

	s_q3roptions.rvrl_objects.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.rvrl_objects.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.rvrl_objects.generic.x			= LAY_R;
	s_q3roptions.rvrl_objects.generic.y			= LAY_RLY + LAY_STEP * 1;
	s_q3roptions.rvrl_objects.generic.name		= "Objects:";
	s_q3roptions.rvrl_objects.generic.id		= ID_RVRL_OBJECTS;
	s_q3roptions.rvrl_objects.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.rvrl_objects.generic.statusbar	= Q3ROptions_StatusBar;

	s_q3roptions.rvrl_smoke.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.rvrl_smoke.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.rvrl_smoke.generic.x			= LAY_R;
	s_q3roptions.rvrl_smoke.generic.y			= LAY_RLY + LAY_STEP * 2;
	s_q3roptions.rvrl_smoke.generic.name		= "Smoke:";
	s_q3roptions.rvrl_smoke.generic.id			= ID_RVRL_SMOKE;
	s_q3roptions.rvrl_smoke.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.rvrl_smoke.generic.statusbar	= Q3ROptions_StatusBar;

	s_q3roptions.rvrl_marks.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.rvrl_marks.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.rvrl_marks.generic.x			= LAY_R;
	s_q3roptions.rvrl_marks.generic.y			= LAY_RLY + LAY_STEP * 3;
	s_q3roptions.rvrl_marks.generic.name		= "Marks:";
	s_q3roptions.rvrl_marks.generic.id			= ID_RVRL_MARKS;
	s_q3roptions.rvrl_marks.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.rvrl_marks.generic.statusbar	= Q3ROptions_StatusBar;

	s_q3roptions.rvrl_sparks.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.rvrl_sparks.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.rvrl_sparks.generic.x			= LAY_R;
	s_q3roptions.rvrl_sparks.generic.y			= LAY_RLY + LAY_STEP * 4;
	s_q3roptions.rvrl_sparks.generic.name		= "Sparks:";
	s_q3roptions.rvrl_sparks.generic.id			= ID_RVRL_SPARKS;
	s_q3roptions.rvrl_sparks.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.rvrl_sparks.generic.statusbar	= Q3ROptions_StatusBar;

	s_q3roptions.mvrl_heading.generic.type		= MTYPE_PTEXT;
	s_q3roptions.mvrl_heading.generic.flags		= QMF_CENTER_JUSTIFY|QMF_INACTIVE;
	s_q3roptions.mvrl_heading.generic.x			= LAY_L - 20;
	s_q3roptions.mvrl_heading.generic.y			= LAY_HDG_Y;
	s_q3roptions.mvrl_heading.generic.id		= 0;
	s_q3roptions.mvrl_heading.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.mvrl_heading.string			= "Main View Render Level:";
	s_q3roptions.mvrl_heading.color				= text_color_normal;
	s_q3roptions.mvrl_heading.style				= UI_CENTER | UI_SMALLFONT;

	s_q3roptions.mvrl_players.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.mvrl_players.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.mvrl_players.generic.x			= LAY_L;
	s_q3roptions.mvrl_players.generic.y			= LAY_RLY + LAY_STEP * 0;
	s_q3roptions.mvrl_players.generic.name		= "Players:";
	s_q3roptions.mvrl_players.generic.id		= ID_MVRL_PLAYERS;
	s_q3roptions.mvrl_players.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.mvrl_players.generic.statusbar	= Q3ROptions_StatusBar;

	s_q3roptions.mvrl_objects.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.mvrl_objects.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.mvrl_objects.generic.x			= LAY_L;
	s_q3roptions.mvrl_objects.generic.y			= LAY_RLY + LAY_STEP * 1;
	s_q3roptions.mvrl_objects.generic.name		= "Objects:";
	s_q3roptions.mvrl_objects.generic.id		= ID_MVRL_OBJECTS;
	s_q3roptions.mvrl_objects.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.mvrl_objects.generic.statusbar	= Q3ROptions_StatusBar;

	s_q3roptions.mvrl_smoke.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.mvrl_smoke.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.mvrl_smoke.generic.x			= LAY_L;
	s_q3roptions.mvrl_smoke.generic.y			= LAY_RLY + LAY_STEP * 2;
	s_q3roptions.mvrl_smoke.generic.name		= "Smoke:";
	s_q3roptions.mvrl_smoke.generic.id			= ID_MVRL_SMOKE;
	s_q3roptions.mvrl_smoke.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.mvrl_smoke.generic.statusbar	= Q3ROptions_StatusBar;

	s_q3roptions.mvrl_marks.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.mvrl_marks.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.mvrl_marks.generic.x			= LAY_L;
	s_q3roptions.mvrl_marks.generic.y			= LAY_RLY + LAY_STEP * 3;
	s_q3roptions.mvrl_marks.generic.name		= "Marks:";
	s_q3roptions.mvrl_marks.generic.id			= ID_MVRL_MARKS;
	s_q3roptions.mvrl_marks.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.mvrl_marks.generic.statusbar	= Q3ROptions_StatusBar;

	s_q3roptions.mvrl_sparks.generic.type		= MTYPE_RADIOBUTTON;
	s_q3roptions.mvrl_sparks.generic.flags		= QMF_SMALLFONT;
	s_q3roptions.mvrl_sparks.generic.x			= LAY_L;
	s_q3roptions.mvrl_sparks.generic.y			= LAY_RLY + LAY_STEP * 4;
	s_q3roptions.mvrl_sparks.generic.name		= "Sparks:";
	s_q3roptions.mvrl_sparks.generic.id			= ID_MVRL_SPARKS;
	s_q3roptions.mvrl_sparks.generic.callback	= Q3ROptions_MenuEvent;
	s_q3roptions.mvrl_sparks.generic.statusbar	= Q3ROptions_StatusBar;



	s_q3roptions.back.generic.type			= MTYPE_PTEXT;
	s_q3roptions.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_q3roptions.back.generic.x				= 25;
	s_q3roptions.back.generic.y				= 480 - 40;
	s_q3roptions.back.generic.id			= ID_BACK;
	s_q3roptions.back.generic.callback		= Q3ROptions_MenuEvent; 
	s_q3roptions.back.string				= "< BACK";
	s_q3roptions.back.color					= text_color_normal;
	s_q3roptions.back.style					= UI_LEFT | UI_SMALLFONT;



	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.transmissionMode );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.manualShift );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.atomspheric );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.ghostPlayback );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.fuelConsumption );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.skidlength );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.camtracking );

	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.speedometer );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.units );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.engineSounds );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.positionSprites );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.ladderOffline ); /* Q3RALLY LADDER */

	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.mvrl_players );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.mvrl_objects );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.mvrl_smoke );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.mvrl_marks );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.mvrl_sparks );

	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.rvrl_players );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.rvrl_objects );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.rvrl_smoke );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.rvrl_marks );
	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.rvrl_sparks );

	Menu_AddItem( &s_q3roptions.menu, ( void * ) &s_q3roptions.back );

	Q3ROptions_SetBounds( &s_q3roptions.transmissionMode.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 0 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Transmission" );
	Q3ROptions_SetBounds( &s_q3roptions.manualShift.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 1 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Manual F/R select" );
	Q3ROptions_SetBounds( &s_q3roptions.atomspheric.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 2 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Atmospheric effects" );
	Q3ROptions_SetBounds( &s_q3roptions.ghostPlayback.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 3 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Ghost playback" );
	Q3ROptions_SetBounds( &s_q3roptions.fuelConsumption.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 4 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Fuel consumption" );
	Q3ROptions_SetBounds( &s_q3roptions.skidlength.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 5 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Skid segment length" );
	Q3ROptions_SetBounds( &s_q3roptions.camtracking.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 6 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Camera tracking" );

	Q3ROptions_SetBounds( &s_q3roptions.speedometer.generic,
		Q3R_OPTIONS_RIGHT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 0 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Speedometer" );
	Q3ROptions_SetBounds( &s_q3roptions.units.generic,
		Q3R_OPTIONS_RIGHT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 1 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Unit type" );
	Q3ROptions_SetBounds( &s_q3roptions.engineSounds.generic,
		Q3R_OPTIONS_RIGHT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 2 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Engine sounds" );
	Q3ROptions_SetBounds( &s_q3roptions.positionSprites.generic,
		Q3R_OPTIONS_RIGHT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 3 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Race position sprites" );
	Q3ROptions_SetBounds( &s_q3roptions.ladderOffline.generic,
		Q3R_OPTIONS_RIGHT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_TOP_ROW_Y + 4 * Q3R_OPTIONS_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Offline ladder sync" );

	Q3ROptions_SetBounds( &s_q3roptions.mvrl_players.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET, Q3R_OPTIONS_MAIN_ROW_Y,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Players" );
	Q3ROptions_SetBounds( &s_q3roptions.mvrl_objects.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_MAIN_ROW_Y + 1 * Q3R_OPTIONS_MAIN_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Objects" );
	Q3ROptions_SetBounds( &s_q3roptions.mvrl_smoke.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_MAIN_ROW_Y + 2 * Q3R_OPTIONS_MAIN_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Smoke" );
	Q3ROptions_SetBounds( &s_q3roptions.mvrl_marks.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_MAIN_ROW_Y + 3 * Q3R_OPTIONS_MAIN_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Marks" );
	Q3ROptions_SetBounds( &s_q3roptions.mvrl_sparks.generic,
		Q3R_OPTIONS_LEFT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_MAIN_ROW_Y + 4 * Q3R_OPTIONS_MAIN_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Sparks" );

	Q3ROptions_SetBounds( &s_q3roptions.rvrl_players.generic,
		Q3R_OPTIONS_RIGHT_X + Q3R_OPTIONS_ROW_X_INSET, Q3R_OPTIONS_REAR_ROW_Y,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Players" );
	Q3ROptions_SetBounds( &s_q3roptions.rvrl_objects.generic,
		Q3R_OPTIONS_RIGHT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_REAR_ROW_Y + 1 * Q3R_OPTIONS_REAR_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Objects" );
	Q3ROptions_SetBounds( &s_q3roptions.rvrl_smoke.generic,
		Q3R_OPTIONS_RIGHT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_REAR_ROW_Y + 2 * Q3R_OPTIONS_REAR_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Smoke" );
	Q3ROptions_SetBounds( &s_q3roptions.rvrl_marks.generic,
		Q3R_OPTIONS_RIGHT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_REAR_ROW_Y + 3 * Q3R_OPTIONS_REAR_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Marks" );
	Q3ROptions_SetBounds( &s_q3roptions.rvrl_sparks.generic,
		Q3R_OPTIONS_RIGHT_X + Q3R_OPTIONS_ROW_X_INSET,
		Q3R_OPTIONS_REAR_ROW_Y + 4 * Q3R_OPTIONS_REAR_ROW_STEP,
		Q3R_OPTIONS_ROW_WIDTH, Q3R_OPTIONS_ROW_HEIGHT, "Sparks" );

	s_q3roptions.transmissionMode.generic.ownerdraw = Q3ROptions_DrawChoice;
	s_q3roptions.manualShift.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.atomspheric.generic.ownerdraw = Q3ROptions_DrawChoice;
	s_q3roptions.ghostPlayback.generic.ownerdraw = Q3ROptions_DrawChoice;
	s_q3roptions.fuelConsumption.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.skidlength.generic.ownerdraw = Q3ROptions_DrawSlider;
	s_q3roptions.camtracking.generic.ownerdraw = Q3ROptions_DrawSlider;
	s_q3roptions.speedometer.generic.ownerdraw = Q3ROptions_DrawChoice;
	s_q3roptions.units.generic.ownerdraw = Q3ROptions_DrawChoice;
	s_q3roptions.engineSounds.generic.ownerdraw = Q3ROptions_DrawChoice;
	s_q3roptions.positionSprites.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.ladderOffline.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.mvrl_players.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.mvrl_objects.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.mvrl_smoke.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.mvrl_marks.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.mvrl_sparks.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.rvrl_players.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.rvrl_objects.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.rvrl_smoke.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.rvrl_marks.generic.ownerdraw = Q3ROptions_DrawRadio;
	s_q3roptions.rvrl_sparks.generic.ownerdraw = Q3ROptions_DrawRadio;

	/* Slider_Key uses x as the start of its legacy input range. Keep that
	 * interaction aligned with the modern track drawn by Q3ROptions_DrawSlider. */
	s_q3roptions.skidlength.generic.x =
		s_q3roptions.skidlength.generic.left + 116;
	s_q3roptions.camtracking.generic.x =
		s_q3roptions.camtracking.generic.left + 116;

	Q3ROptions_SetBounds( &s_q3roptions.back.generic,
		Q3R_OPTIONS_FRAME_X + 24, Q3R_OPTIONS_ACTION_Y, 120, 24, NULL );
	s_q3roptions.back.string = "Back";
	s_q3roptions.back.generic.statusbar = Q3ROptions_StatusBar;
	s_q3roptions.back.generic.ownerdraw = Q3ROptions_DrawAction;
}


/*
===============
UI_SystemConfigMenu
===============
*/
void UI_Q3ROptionsMenu( void )
{
// STONELANCE FIXME: get rid of this after proper tansitions are added
	uis.transitionIn = 0;
// END

	Q3ROptions_MenuInit();
	UI_PushMenu ( &s_q3roptions.menu );
}
