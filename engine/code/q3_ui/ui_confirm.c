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

CONFIRMATION MENU

=======================================================================
*/


#include "ui_local.h"
#include "ui_rally_frontend.h"


#define ID_CONFIRM_NO		10
#define ID_CONFIRM_YES		11

#define CONFIRM_PANEL_X		112
#define CONFIRM_PANEL_Y		136
#define CONFIRM_PANEL_W		416
#define CONFIRM_PANEL_H		208
#define CONFIRM_BUTTON_Y		286
#define CONFIRM_BUTTON_W		112
#define CONFIRM_BUTTON_H		28
#define CONFIRM_YES_X		184
#define CONFIRM_NO_X		344
#define CONFIRM_OK_X		264


typedef struct {
	menuframework_s menu;

	menutext_s		no;
	menutext_s		yes;

	int				slashX;
	const char *	question;
	void			(*draw)( void );
	void			(*action)( qboolean result );
	
	int style;
	const char **lines;
} confirmMenu_t;


static confirmMenu_t	s_confirm;
static vec4_t confirmScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t confirmTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t confirmMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t confirmAccentColor = UI_FRONTEND_COLOR_ACCENT;


static void ConfirmMenu_DrawButton( void *self ) {
	menutext_s *button;
	qboolean focus;

	button = (menutext_s *)self;
	focus = ( Menu_ItemAtCursor( button->generic.parent ) == button );
	Frontend_DrawButton( button->generic.left, button->generic.top,
		button->generic.right - button->generic.left,
		button->generic.bottom - button->generic.top,
		button->string, 1.0f, focus, UI_FRONTEND_TEXT_CENTER );
}


static void ConfirmMenu_DrawShell( const char *title ) {
	if ( s_confirm.menu.fullscreen ) {
		Frontend_DrawBackground( confirmScrimColor );
	} else {
		UI_FillRect( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, confirmScrimColor );
	}

	Frontend_DrawPanel( CONFIRM_PANEL_X, CONFIRM_PANEL_Y,
		CONFIRM_PANEL_W, CONFIRM_PANEL_H, 1.0f, UI_FRONTEND_STYLE_SURFACE );
	if ( title && title[0] ) {
		Frontend_DrawText( CONFIRM_PANEL_X + UI_FRONTEND_SPACE_LG,
			CONFIRM_PANEL_Y + UI_FRONTEND_SPACE_LG, title,
			UI_LEFT | UI_BIGFONT, confirmTextColor );
	}
}


static void ConfirmMenu_DrawQuestion( void ) {
	if ( s_confirm.question && s_confirm.question[0] ) {
		Frontend_DrawText( 320, CONFIRM_PANEL_Y + 70, s_confirm.question,
			UI_CENTER | UI_SMALLFONT, confirmTextColor );
	}
}


/*
=================
ConfirmMenu_Event
=================
*/
static void ConfirmMenu_Event( void* ptr, int event ) {
	qboolean	result;

	if( event != QM_ACTIVATED ) {
		return;
	}

	UI_PopMenu();

	if( ((menucommon_s*)ptr)->id == ID_CONFIRM_NO ) {
		result = qfalse;
	}
	else {
		result = qtrue;
	}

	if( s_confirm.action ) {
		s_confirm.action( result );
	}
}


/*
=================
ConfirmMenu_Key
=================
*/
static sfxHandle_t ConfirmMenu_Key( int key ) {
	switch ( key ) {
	case K_KP_LEFTARROW:
	case K_LEFTARROW:
	case K_KP_RIGHTARROW:
	case K_RIGHTARROW:
		key = K_TAB;
		break;

	case 'n':
	case 'N':
		ConfirmMenu_Event( &s_confirm.no, QM_ACTIVATED );
		return menu_move_sound;

	case 'y':
	case 'Y':
		ConfirmMenu_Event( &s_confirm.yes, QM_ACTIVATED );
		return menu_move_sound;
	}

	return Menu_DefaultKey( &s_confirm.menu, key );
}


/*
=================
MessaheMenu_Draw
=================
*/
static void MessageMenu_Draw( void ) {
	int i;
	int y;

	ConfirmMenu_DrawShell( "Notice" );

	y = CONFIRM_PANEL_Y + 72;
	for ( i = 0; s_confirm.lines && s_confirm.lines[i]; i++ ) {
		Frontend_DrawText( 320, y, s_confirm.lines[i],
			UI_CENTER | UI_SMALLFONT, confirmMutedColor );
		y += 20;
	}

	Menu_Draw( &s_confirm.menu );
}

/*
=================
ConfirmMenu_Draw
=================
*/
static void ConfirmMenu_Draw( void ) {
	ConfirmMenu_DrawShell( "Confirm action" );
	ConfirmMenu_DrawQuestion();

	if( s_confirm.draw ) {
		s_confirm.draw();
	}

	Menu_Draw( &s_confirm.menu );
}


/*
=================
ConfirmMenu_Cache
=================
*/
void ConfirmMenu_Cache( void ) {
}


/*
=================
UI_ConfirmMenu_Stlye
=================
*/
void UI_ConfirmMenu_Style( const char *question, int style, void (*draw)( void ), void (*action)( qboolean result ) ) {
	uiClientState_t	cstate;

	// zero set all our globals
	memset( &s_confirm, 0, sizeof(s_confirm) );

	ConfirmMenu_Cache();

	s_confirm.question = question;
	s_confirm.draw = draw;
	s_confirm.action = action;
	s_confirm.style = style;

	s_confirm.menu.draw       = ConfirmMenu_Draw;
	s_confirm.menu.key        = ConfirmMenu_Key;
	s_confirm.menu.wrapAround = qtrue;
// STONELANCE
	s_confirm.menu.transparent = qtrue;
// END

	trap_GetClientState( &cstate );
	if ( cstate.connState >= CA_CONNECTED ) {
		s_confirm.menu.fullscreen = qfalse;
	}
	else {
		s_confirm.menu.fullscreen = qtrue;
	}

	s_confirm.yes.generic.type		= MTYPE_PTEXT;
	s_confirm.yes.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_confirm.yes.generic.callback	= ConfirmMenu_Event;
	s_confirm.yes.generic.id		= ID_CONFIRM_YES;
	s_confirm.yes.generic.x			= CONFIRM_YES_X + CONFIRM_BUTTON_W / 2;
	s_confirm.yes.generic.y			= CONFIRM_BUTTON_Y + CONFIRM_BUTTON_H / 2;
	s_confirm.yes.generic.left		= CONFIRM_YES_X;
	s_confirm.yes.generic.top			= CONFIRM_BUTTON_Y;
	s_confirm.yes.generic.right		= CONFIRM_YES_X + CONFIRM_BUTTON_W;
	s_confirm.yes.generic.bottom		= CONFIRM_BUTTON_Y + CONFIRM_BUTTON_H;
	s_confirm.yes.generic.ownerdraw	= ConfirmMenu_DrawButton;
	s_confirm.yes.string			= "YES";
	s_confirm.yes.color				= confirmTextColor;
	s_confirm.yes.style				= UI_CENTER | UI_SMALLFONT;

	s_confirm.no.generic.type		= MTYPE_PTEXT;
	s_confirm.no.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_confirm.no.generic.callback	= ConfirmMenu_Event;
	s_confirm.no.generic.id			= ID_CONFIRM_NO;
	s_confirm.no.generic.x		    = CONFIRM_NO_X + CONFIRM_BUTTON_W / 2;
	s_confirm.no.generic.y		    = CONFIRM_BUTTON_Y + CONFIRM_BUTTON_H / 2;
	s_confirm.no.generic.left		= CONFIRM_NO_X;
	s_confirm.no.generic.top		    = CONFIRM_BUTTON_Y;
	s_confirm.no.generic.right		= CONFIRM_NO_X + CONFIRM_BUTTON_W;
	s_confirm.no.generic.bottom		= CONFIRM_BUTTON_Y + CONFIRM_BUTTON_H;
	s_confirm.no.generic.ownerdraw	= ConfirmMenu_DrawButton;
	s_confirm.no.string				= "NO";
	s_confirm.no.color			    = confirmTextColor;
	s_confirm.no.style			    = UI_CENTER | UI_SMALLFONT;

	Menu_AddItem( &s_confirm.menu,	&s_confirm.yes );             
	Menu_AddItem( &s_confirm.menu,	&s_confirm.no );

	UI_PushMenu( &s_confirm.menu );

	Menu_SetCursorToItem( &s_confirm.menu, &s_confirm.no );
}

/*
=================
UI_ConfirmMenu
=================
*/
void UI_ConfirmMenu( const char *question, void (*draw)( void ), void (*action)( qboolean result ) ) {
	UI_ConfirmMenu_Style(question, UI_CENTER|UI_INVERSE, draw, action);
}

/*
=================
UI_Message
hacked over from Confirm stuff
=================
*/
void UI_Message( const char **lines ) {
	uiClientState_t	cstate;
	
	// zero set all our globals
	memset( &s_confirm, 0, sizeof(s_confirm) );

	ConfirmMenu_Cache();

	s_confirm.lines = lines;
	s_confirm.style = UI_CENTER|UI_SMALLFONT;

	s_confirm.menu.draw       = MessageMenu_Draw;
	s_confirm.menu.key        = ConfirmMenu_Key;
	s_confirm.menu.wrapAround = qtrue;
	
	trap_GetClientState( &cstate );
	if ( cstate.connState >= CA_CONNECTED ) {
		s_confirm.menu.fullscreen = qfalse;
	}
	else {
		s_confirm.menu.fullscreen = qtrue;
	}

	s_confirm.yes.generic.type		= MTYPE_PTEXT;
	s_confirm.yes.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_confirm.yes.generic.callback	= ConfirmMenu_Event;
	s_confirm.yes.generic.id		= ID_CONFIRM_YES;
	s_confirm.yes.generic.x			= CONFIRM_OK_X + CONFIRM_BUTTON_W / 2;
	s_confirm.yes.generic.y			= CONFIRM_BUTTON_Y + CONFIRM_BUTTON_H / 2;
	s_confirm.yes.generic.left		= CONFIRM_OK_X;
	s_confirm.yes.generic.top			= CONFIRM_BUTTON_Y;
	s_confirm.yes.generic.right		= CONFIRM_OK_X + CONFIRM_BUTTON_W;
	s_confirm.yes.generic.bottom		= CONFIRM_BUTTON_Y + CONFIRM_BUTTON_H;
	s_confirm.yes.generic.ownerdraw	= ConfirmMenu_DrawButton;
	s_confirm.yes.string			= "OK";
	s_confirm.yes.color				= confirmAccentColor;
	s_confirm.yes.style				= UI_CENTER | UI_SMALLFONT;

	Menu_AddItem( &s_confirm.menu,	&s_confirm.yes );
	
	UI_PushMenu( &s_confirm.menu );

	Menu_SetCursorToItem( &s_confirm.menu, &s_confirm.yes );
}
