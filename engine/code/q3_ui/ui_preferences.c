/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

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

/*
=======================================================================

GAME OPTIONS MENU

=======================================================================
*/


#include "ui_local.h"
#include "ui_rally_frontend.h"

#define PREFERENCES_X_POS		360

#define PREFERENCES_FRAME_X          24
#define PREFERENCES_FRAME_Y          20
#define PREFERENCES_FRAME_WIDTH      592
#define PREFERENCES_FRAME_HEIGHT     440
#define PREFERENCES_LEFT_X           48
#define PREFERENCES_RIGHT_X          332
#define PREFERENCES_CARD_Y           104
#define PREFERENCES_CARD_WIDTH       260
#define PREFERENCES_CARD_HEIGHT      300
#define PREFERENCES_ROW_HEIGHT       30
#define PREFERENCES_ROW_GAP          4
#define PREFERENCES_ROW_Y            150
#define PREFERENCES_ROW_INSET        16
#define PREFERENCES_ROW_WIDTH        ( PREFERENCES_CARD_WIDTH - 32 )
#define PREFERENCES_ACTION_Y         420

#define ID_SIMPLEITEMS			128
#define ID_HIGHQUALITYSKY		129
#define ID_EJECTINGBRASS		130
#define ID_WALLMARKS			131
#define ID_DYNAMICLIGHTS		132
#define ID_IDENTIFYTARGET		133
#define ID_SYNCEVERYFRAME		134
#define ID_FORCEMODEL			135
#define ID_DRAWTEAMOVERLAY		136
#define ID_ALLOWDOWNLOAD		137
#define ID_BACK					138
#define ID_DRAWFPS              139
#define ID_SIGILSWITCH          140

#define	NUM_CROSSHAIRS			10


typedef struct {
	menuframework_s		menu;

	menutext_s			banner;

	menuradiobutton_s	simpleitems;
	menuradiobutton_s	brass;
	menuradiobutton_s	wallmarks;
	menuradiobutton_s	dynamiclights;
	menuradiobutton_s	identifytarget;
	menuradiobutton_s	highqualitysky;
	menuradiobutton_s	synceveryframe;
	menuradiobutton_s	forcemodel;
	menulist_s			drawteamoverlay;
	menuradiobutton_s	allowdownload;
    menuradiobutton_s   drawfps;
    menuradiobutton_s   sigilswitch;

	menutext_s			back;

	qhandle_t			crosshairShader[NUM_CROSSHAIRS];
} preferences_t;

static preferences_t s_preferences;

static vec4_t preferencesScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t preferencesTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t preferencesMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t preferencesAccentColor = UI_FRONTEND_COLOR_ACCENT;
static vec4_t preferencesFocusColor = UI_FRONTEND_COLOR_FOCUS_BG;
static vec4_t preferencesBorderColor = UI_FRONTEND_COLOR_BORDER;

static void Preferences_MenuDraw( void );
static void Preferences_DrawRadio( void *self );
static void Preferences_DrawChoice( void *self );
static void Preferences_DrawAction( void *self );

static const char *teamoverlay_names[] =
{
	"Off",
	"Upper right",
	"Lower right",
	"Lower left",
	0
};

static void Preferences_SetMenuItems( void ) {

	s_preferences.simpleitems.curvalue		= trap_Cvar_VariableValue( "cg_simpleItems" ) != 0;
	s_preferences.brass.curvalue			= trap_Cvar_VariableValue( "cg_brassTime" ) != 0;
	s_preferences.wallmarks.curvalue		= trap_Cvar_VariableValue( "cg_marks" ) != 0;
	s_preferences.identifytarget.curvalue	= trap_Cvar_VariableValue( "cg_drawCrosshairNames" ) != 0;
	s_preferences.dynamiclights.curvalue	= trap_Cvar_VariableValue( "r_dynamiclight" ) != 0;
	s_preferences.highqualitysky.curvalue	= trap_Cvar_VariableValue ( "r_fastsky" ) == 0;
	s_preferences.synceveryframe.curvalue	= trap_Cvar_VariableValue( "r_finish" ) != 0;
	s_preferences.forcemodel.curvalue		= trap_Cvar_VariableValue( "cg_forcemodel" ) != 0;
	s_preferences.drawteamoverlay.curvalue	= Com_Clamp( 0, 3, trap_Cvar_VariableValue( "cg_drawTeamOverlay" ) );
	s_preferences.allowdownload.curvalue	= trap_Cvar_VariableValue( "cl_allowDownload" ) != 0;
    s_preferences.drawfps.curvalue          = trap_Cvar_VariableValue( "cg_DrawFPS" ) != 0;
    s_preferences.sigilswitch.curvalue      = trap_Cvar_VariableValue( "cg_sigilSwitch" ) != 0;
}


static void Preferences_Event( void* ptr, int notification ) {
	if( notification != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {

	case ID_SIMPLEITEMS:
		trap_Cvar_SetValue( "cg_simpleItems", s_preferences.simpleitems.curvalue );
		break;

	case ID_HIGHQUALITYSKY:
		trap_Cvar_SetValue( "r_fastsky", !s_preferences.highqualitysky.curvalue );
		break;

	case ID_EJECTINGBRASS:
		if ( s_preferences.brass.curvalue )
			trap_Cvar_Reset( "cg_brassTime" );
		else
			trap_Cvar_SetValue( "cg_brassTime", 0 );
		break;

	case ID_WALLMARKS:
		trap_Cvar_SetValue( "cg_marks", s_preferences.wallmarks.curvalue );
		break;

	case ID_DYNAMICLIGHTS:
		trap_Cvar_SetValue( "r_dynamiclight", s_preferences.dynamiclights.curvalue );
		break;		

	case ID_IDENTIFYTARGET:
		trap_Cvar_SetValue( "cg_drawCrosshairNames", s_preferences.identifytarget.curvalue );
		break;

	case ID_SYNCEVERYFRAME:
		trap_Cvar_SetValue( "r_finish", s_preferences.synceveryframe.curvalue );
		break;

	case ID_FORCEMODEL:
		trap_Cvar_SetValue( "cg_forcemodel", s_preferences.forcemodel.curvalue );
		break;

	case ID_DRAWTEAMOVERLAY:
		trap_Cvar_SetValue( "cg_drawTeamOverlay", s_preferences.drawteamoverlay.curvalue );
		break;

	case ID_ALLOWDOWNLOAD:
		trap_Cvar_SetValue( "cl_allowDownload", s_preferences.allowdownload.curvalue );
		break;

    case ID_DRAWFPS:
        trap_Cvar_SetValue( "cg_DrawFPS", s_preferences.drawfps.curvalue );
        break;
    
    case ID_SIGILSWITCH:
        trap_Cvar_SetValue( "cg_sigilSwitch", s_preferences.sigilswitch.curvalue );
        break;
            
	case ID_BACK:
		UI_PopMenu();
		break;
	}
}

static void Preferences_SetBounds( menucommon_s *item, int x, int y,
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

static void Preferences_DrawRow( menucommon_s *item, const char *value ) {
	qboolean focus;
	qboolean disabled;
	vec4_t labelColor;
	vec4_t valueColor;

	focus = ( Menu_ItemAtCursor( item->parent ) == item );
	disabled = ( item->flags & ( QMF_GRAYED | QMF_INACTIVE ) ) ? qtrue : qfalse;

	if ( focus && !disabled ) {
		UI_FillRect( item->left, item->top,
			item->right - item->left, item->bottom - item->top,
			preferencesFocusColor );
		UI_FillRect( item->left, item->top, 2,
			item->bottom - item->top, preferencesAccentColor );
	}
	UI_FillRect( item->left, item->bottom - 1,
		item->right - item->left, 1, preferencesBorderColor );

	Vector4Copy( disabled ? preferencesMutedColor :
		(focus ? preferencesTextColor : preferencesMutedColor), labelColor );
	Vector4Copy( disabled ? preferencesMutedColor :
		(focus ? preferencesAccentColor : preferencesTextColor), valueColor );

	Frontend_DrawText( item->left + 12,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		item->name, UI_LEFT | UI_SMALLFONT, labelColor );
	Frontend_DrawText( item->right - 12,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		value, UI_RIGHT | UI_SMALLFONT, valueColor );
}

static void Preferences_DrawRadio( void *self ) {
	menuradiobutton_s *radio;

	radio = (menuradiobutton_s *)self;
	Preferences_DrawRow( &radio->generic,
		radio->curvalue ? "On" : "Off" );
}

static void Preferences_DrawChoice( void *self ) {
	menulist_s *choice;
	const char *value;

	choice = (menulist_s *)self;
	value = choice->itemnames[choice->curvalue];
	Preferences_DrawRow( &choice->generic, value ? value : "-" );
}

static void Preferences_DrawAction( void *self ) {
	menutext_s *action;
	qboolean focus;

	action = (menutext_s *)self;
	focus = ( Menu_ItemAtCursor( action->generic.parent ) == &action->generic );
	Frontend_DrawButton( action->generic.left, action->generic.top,
		action->generic.right - action->generic.left,
		action->generic.bottom - action->generic.top,
		action->string, 1.0f, focus, UI_CENTER );
}

static void Preferences_MenuDraw( void ) {
	Frontend_DrawBackground( preferencesScrimColor );
	Frontend_DrawPanel( PREFERENCES_FRAME_X, PREFERENCES_FRAME_Y,
		PREFERENCES_FRAME_WIDTH, PREFERENCES_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( PREFERENCES_FRAME_X + 24,
		PREFERENCES_FRAME_Y + 24, "Game options",
		UI_LEFT | UI_BIGFONT, preferencesTextColor );
	Frontend_DrawText( PREFERENCES_FRAME_X + 24,
		PREFERENCES_FRAME_Y + 48,
		"Tune gameplay rules, world detail and interface behavior",
		UI_LEFT | UI_SMALLFONT, preferencesMutedColor );
	Frontend_DrawStatusChip( PREFERENCES_FRAME_X + PREFERENCES_FRAME_WIDTH - 104,
		PREFERENCES_FRAME_Y + 26, "Settings",
		preferencesAccentColor, 1.0f );

	Frontend_DrawCard( PREFERENCES_LEFT_X, PREFERENCES_CARD_Y,
		PREFERENCES_CARD_WIDTH, PREFERENCES_CARD_HEIGHT, 1.0f, qfalse );
	Frontend_DrawCard( PREFERENCES_RIGHT_X, PREFERENCES_CARD_Y,
		PREFERENCES_CARD_WIDTH, PREFERENCES_CARD_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( PREFERENCES_LEFT_X + 16, PREFERENCES_CARD_Y + 22,
		"World & rendering", UI_LEFT | UI_SMALLFONT,
		preferencesMutedColor );
	Frontend_DrawText( PREFERENCES_RIGHT_X + 16, PREFERENCES_CARD_Y + 22,
		"Player & interface", UI_LEFT | UI_SMALLFONT,
		preferencesMutedColor );
	Frontend_DrawText( PREFERENCES_FRAME_X + 24,
		PREFERENCES_FRAME_Y + 384,
		"Select an option   Left / right adjust   Esc back",
		UI_LEFT | UI_SMALLFONT, preferencesMutedColor );

	Menu_Draw( &s_preferences.menu );
}

static void Preferences_MenuInit( void ) {
	int				y;

	memset( &s_preferences, 0 ,sizeof(preferences_t) );

	Preferences_Cache();

	s_preferences.menu.draw = Preferences_MenuDraw;
	s_preferences.menu.wrapAround = qtrue;
	s_preferences.menu.fullscreen = qtrue;

	s_preferences.banner.generic.type  = MTYPE_BTEXT;
	s_preferences.banner.generic.x	   = 320;
	s_preferences.banner.generic.y	   = 16;
	s_preferences.banner.string		   = "GAME OPTIONS";
	s_preferences.banner.color         = color_white;
	s_preferences.banner.style         = UI_CENTER;

    y = 144;
	s_preferences.simpleitems.generic.type        = MTYPE_RADIOBUTTON;
	s_preferences.simpleitems.generic.name	      = "Simple Items:";
	s_preferences.simpleitems.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.simpleitems.generic.callback    = Preferences_Event;
	s_preferences.simpleitems.generic.id          = ID_SIMPLEITEMS;
	s_preferences.simpleitems.generic.x	          = PREFERENCES_X_POS;
	s_preferences.simpleitems.generic.y	          = y;

	y += BIGCHAR_HEIGHT;
	s_preferences.wallmarks.generic.type          = MTYPE_RADIOBUTTON;
	s_preferences.wallmarks.generic.name	      = "Marks on Walls:";
	s_preferences.wallmarks.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.wallmarks.generic.callback      = Preferences_Event;
	s_preferences.wallmarks.generic.id            = ID_WALLMARKS;
	s_preferences.wallmarks.generic.x	          = PREFERENCES_X_POS;
	s_preferences.wallmarks.generic.y	          = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.brass.generic.type              = MTYPE_RADIOBUTTON;
	s_preferences.brass.generic.name	          = "Ejecting Brass:";
	s_preferences.brass.generic.flags	          = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.brass.generic.callback          = Preferences_Event;
	s_preferences.brass.generic.id                = ID_EJECTINGBRASS;
	s_preferences.brass.generic.x	              = PREFERENCES_X_POS;
	s_preferences.brass.generic.y	              = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.dynamiclights.generic.type      = MTYPE_RADIOBUTTON;
	s_preferences.dynamiclights.generic.name	  = "Dynamic Lights:";
	s_preferences.dynamiclights.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.dynamiclights.generic.callback  = Preferences_Event;
	s_preferences.dynamiclights.generic.id        = ID_DYNAMICLIGHTS;
	s_preferences.dynamiclights.generic.x	      = PREFERENCES_X_POS;
	s_preferences.dynamiclights.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.identifytarget.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.identifytarget.generic.name	  = "Identify Target:";
	s_preferences.identifytarget.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.identifytarget.generic.callback = Preferences_Event;
	s_preferences.identifytarget.generic.id       = ID_IDENTIFYTARGET;
	s_preferences.identifytarget.generic.x	      = PREFERENCES_X_POS;
	s_preferences.identifytarget.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.highqualitysky.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.highqualitysky.generic.name	  = "High Quality Sky:";
	s_preferences.highqualitysky.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.highqualitysky.generic.callback = Preferences_Event;
	s_preferences.highqualitysky.generic.id       = ID_HIGHQUALITYSKY;
	s_preferences.highqualitysky.generic.x	      = PREFERENCES_X_POS;
	s_preferences.highqualitysky.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.synceveryframe.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.synceveryframe.generic.name	  = "Sync Every Frame:";
	s_preferences.synceveryframe.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.synceveryframe.generic.callback = Preferences_Event;
	s_preferences.synceveryframe.generic.id       = ID_SYNCEVERYFRAME;
	s_preferences.synceveryframe.generic.x	      = PREFERENCES_X_POS;
	s_preferences.synceveryframe.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.forcemodel.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.forcemodel.generic.name	  = "Force Player Models:";
	s_preferences.forcemodel.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.forcemodel.generic.callback = Preferences_Event;
	s_preferences.forcemodel.generic.id       = ID_FORCEMODEL;
	s_preferences.forcemodel.generic.x	      = PREFERENCES_X_POS;
	s_preferences.forcemodel.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.drawteamoverlay.generic.type     = MTYPE_SPINCONTROL;
	s_preferences.drawteamoverlay.generic.name	   = "Draw Team Overlay:";
	s_preferences.drawteamoverlay.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.drawteamoverlay.generic.callback = Preferences_Event;
	s_preferences.drawteamoverlay.generic.id       = ID_DRAWTEAMOVERLAY;
	s_preferences.drawteamoverlay.generic.x	       = PREFERENCES_X_POS;
	s_preferences.drawteamoverlay.generic.y	       = y;
	s_preferences.drawteamoverlay.itemnames			= teamoverlay_names;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.allowdownload.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.allowdownload.generic.name	   = "Automatic Downloading:";
	s_preferences.allowdownload.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.allowdownload.generic.callback = Preferences_Event;
	s_preferences.allowdownload.generic.id       = ID_ALLOWDOWNLOAD;
	s_preferences.allowdownload.generic.x	       = PREFERENCES_X_POS;
	s_preferences.allowdownload.generic.y	       = y;

    y += BIGCHAR_HEIGHT+2;
	s_preferences.drawfps.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.drawfps.generic.name	   = "Draw FPS:";
	s_preferences.drawfps.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.drawfps.generic.callback = Preferences_Event;
	s_preferences.drawfps.generic.id       = ID_DRAWFPS;
	s_preferences.drawfps.generic.x	       = PREFERENCES_X_POS;
	s_preferences.drawfps.generic.y	       = y;

    y += BIGCHAR_HEIGHT+2;
    s_preferences.sigilswitch.generic.type      = MTYPE_RADIOBUTTON;
    s_preferences.sigilswitch.generic.name      = "Convert Entity to 3rd Sigil?:";
    s_preferences.sigilswitch.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
    s_preferences.sigilswitch.generic.callback  = Preferences_Event;
    s_preferences.sigilswitch.generic.id        = ID_SIGILSWITCH;
    s_preferences.sigilswitch.generic.x         = PREFERENCES_X_POS;
    s_preferences.sigilswitch.generic.y         = y;

	s_preferences.back.generic.type				= MTYPE_PTEXT;
	s_preferences.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_preferences.back.generic.x				= 20;
	s_preferences.back.generic.y				= 480 - 50;
	s_preferences.back.generic.id				= ID_BACK;
	s_preferences.back.generic.callback			= Preferences_Event; 
	s_preferences.back.string					= "< BACK";
	s_preferences.back.color					= text_color_normal;
	s_preferences.back.style					= UI_LEFT | UI_SMALLFONT;


	Menu_AddItem( &s_preferences.menu, &s_preferences.simpleitems );
	Menu_AddItem( &s_preferences.menu, &s_preferences.wallmarks );
	Menu_AddItem( &s_preferences.menu, &s_preferences.brass );
	Menu_AddItem( &s_preferences.menu, &s_preferences.dynamiclights );
	Menu_AddItem( &s_preferences.menu, &s_preferences.identifytarget );
	Menu_AddItem( &s_preferences.menu, &s_preferences.highqualitysky );
	Menu_AddItem( &s_preferences.menu, &s_preferences.synceveryframe );
	Menu_AddItem( &s_preferences.menu, &s_preferences.forcemodel );
	Menu_AddItem( &s_preferences.menu, &s_preferences.drawteamoverlay );
	Menu_AddItem( &s_preferences.menu, &s_preferences.allowdownload );
    Menu_AddItem( &s_preferences.menu, &s_preferences.drawfps );
    Menu_AddItem( &s_preferences.menu, &s_preferences.sigilswitch );

	Menu_AddItem( &s_preferences.menu, &s_preferences.back );

	Preferences_SetBounds( &s_preferences.simpleitems.generic,
		PREFERENCES_LEFT_X + PREFERENCES_ROW_INSET, PREFERENCES_ROW_Y,
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "Simple items" );
	Preferences_SetBounds( &s_preferences.wallmarks.generic,
		PREFERENCES_LEFT_X + PREFERENCES_ROW_INSET,
		PREFERENCES_ROW_Y + 1 * ( PREFERENCES_ROW_HEIGHT + PREFERENCES_ROW_GAP ),
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "Marks on walls" );
	Preferences_SetBounds( &s_preferences.brass.generic,
		PREFERENCES_LEFT_X + PREFERENCES_ROW_INSET,
		PREFERENCES_ROW_Y + 2 * ( PREFERENCES_ROW_HEIGHT + PREFERENCES_ROW_GAP ),
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "Ejecting brass" );
	Preferences_SetBounds( &s_preferences.dynamiclights.generic,
		PREFERENCES_LEFT_X + PREFERENCES_ROW_INSET,
		PREFERENCES_ROW_Y + 3 * ( PREFERENCES_ROW_HEIGHT + PREFERENCES_ROW_GAP ),
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "Dynamic lights" );
	Preferences_SetBounds( &s_preferences.highqualitysky.generic,
		PREFERENCES_LEFT_X + PREFERENCES_ROW_INSET,
		PREFERENCES_ROW_Y + 4 * ( PREFERENCES_ROW_HEIGHT + PREFERENCES_ROW_GAP ),
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "High quality sky" );
	Preferences_SetBounds( &s_preferences.synceveryframe.generic,
		PREFERENCES_LEFT_X + PREFERENCES_ROW_INSET,
		PREFERENCES_ROW_Y + 5 * ( PREFERENCES_ROW_HEIGHT + PREFERENCES_ROW_GAP ),
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "Sync every frame" );

	Preferences_SetBounds( &s_preferences.identifytarget.generic,
		PREFERENCES_RIGHT_X + PREFERENCES_ROW_INSET, PREFERENCES_ROW_Y,
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "Identify target" );
	Preferences_SetBounds( &s_preferences.forcemodel.generic,
		PREFERENCES_RIGHT_X + PREFERENCES_ROW_INSET,
		PREFERENCES_ROW_Y + 1 * ( PREFERENCES_ROW_HEIGHT + PREFERENCES_ROW_GAP ),
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "Force player models" );
	Preferences_SetBounds( &s_preferences.drawteamoverlay.generic,
		PREFERENCES_RIGHT_X + PREFERENCES_ROW_INSET,
		PREFERENCES_ROW_Y + 2 * ( PREFERENCES_ROW_HEIGHT + PREFERENCES_ROW_GAP ),
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "Team overlay" );
	Preferences_SetBounds( &s_preferences.allowdownload.generic,
		PREFERENCES_RIGHT_X + PREFERENCES_ROW_INSET,
		PREFERENCES_ROW_Y + 3 * ( PREFERENCES_ROW_HEIGHT + PREFERENCES_ROW_GAP ),
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "Automatic downloads" );
	Preferences_SetBounds( &s_preferences.drawfps.generic,
		PREFERENCES_RIGHT_X + PREFERENCES_ROW_INSET,
		PREFERENCES_ROW_Y + 4 * ( PREFERENCES_ROW_HEIGHT + PREFERENCES_ROW_GAP ),
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "Show FPS" );
	Preferences_SetBounds( &s_preferences.sigilswitch.generic,
		PREFERENCES_RIGHT_X + PREFERENCES_ROW_INSET,
		PREFERENCES_ROW_Y + 5 * ( PREFERENCES_ROW_HEIGHT + PREFERENCES_ROW_GAP ),
		PREFERENCES_ROW_WIDTH, PREFERENCES_ROW_HEIGHT, "Sigil switch" );

	s_preferences.simpleitems.generic.ownerdraw = Preferences_DrawRadio;
	s_preferences.wallmarks.generic.ownerdraw = Preferences_DrawRadio;
	s_preferences.brass.generic.ownerdraw = Preferences_DrawRadio;
	s_preferences.dynamiclights.generic.ownerdraw = Preferences_DrawRadio;
	s_preferences.highqualitysky.generic.ownerdraw = Preferences_DrawRadio;
	s_preferences.synceveryframe.generic.ownerdraw = Preferences_DrawRadio;
	s_preferences.identifytarget.generic.ownerdraw = Preferences_DrawRadio;
	s_preferences.forcemodel.generic.ownerdraw = Preferences_DrawRadio;
	s_preferences.drawteamoverlay.generic.ownerdraw = Preferences_DrawChoice;
	s_preferences.allowdownload.generic.ownerdraw = Preferences_DrawRadio;
	s_preferences.drawfps.generic.ownerdraw = Preferences_DrawRadio;
	s_preferences.sigilswitch.generic.ownerdraw = Preferences_DrawRadio;

	Preferences_SetBounds( &s_preferences.back.generic,
		PREFERENCES_FRAME_X + 24, PREFERENCES_ACTION_Y, 120, 24, NULL );
	s_preferences.back.string = "Back";
	s_preferences.back.generic.ownerdraw = Preferences_DrawAction;

	Preferences_SetMenuItems();
}


/*
=================
Preferences_Cache
=================
*/
void Preferences_Cache( void ) {
	int		n;

	for( n = 0; n < NUM_CROSSHAIRS; n++ ) {
		s_preferences.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%c", 'a' + n ) );
	}
}


/*
==================
UI_PreferencesMenu
==================
*/
void UI_PreferencesMenu( void ) {
	Preferences_MenuInit();
	UI_PushMenu( &s_preferences.menu );
}
