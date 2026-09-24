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
#include "ui_local.h"
#include "ui_rally_frontend.h"

/*********************************************************************************
	SPECIFY SERVER
*********************************************************************************/

#define SPECIFYSERVER_FRAMEL	"menu/art/frame2_l"
#define SPECIFYSERVER_FRAMER	"menu/art/frame1_r"
#define SPECIFYSERVER_BACK0		"menu/art/back_0"
#define SPECIFYSERVER_BACK1		"menu/art/back_1"
#define SPECIFYSERVER_FIGHT0	"menu/art/fight_0"
#define SPECIFYSERVER_FIGHT1	"menu/art/fight_1"

#define ID_SPECIFYSERVERBACK	102
#define ID_SPECIFYSERVERGO		103

#define SPECIFY_FRAME_X        24
#define SPECIFY_FRAME_Y        20
#define SPECIFY_FRAME_WIDTH    592
#define SPECIFY_FRAME_HEIGHT   440
#define SPECIFY_CARD_X         64
#define SPECIFY_CARD_Y         124
#define SPECIFY_CARD_WIDTH     512
#define SPECIFY_CARD_HEIGHT    164
#define SPECIFY_FIELD_X        288
#define SPECIFY_FIELD_WIDTH    264
#define SPECIFY_FIELD_HEIGHT   28
#define SPECIFY_ADDRESS_Y      168
#define SPECIFY_PORT_Y         224
#define SPECIFY_ACTION_Y       420
#define SPECIFY_ACTION_WIDTH   96
#define SPECIFY_ACTION_HEIGHT  24

static vec4_t specifyTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t specifyMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t specifyAccentColor = UI_FRONTEND_COLOR_ACCENT;

static char* specifyserver_artlist[] =
{
	SPECIFYSERVER_FRAMEL,
	SPECIFYSERVER_FRAMER,
	SPECIFYSERVER_BACK0,	
	SPECIFYSERVER_BACK1,	
	SPECIFYSERVER_FIGHT0,
	SPECIFYSERVER_FIGHT1,
	NULL
};

typedef struct
{
	menuframework_s	menu;
	menutext_s		banner;
	menufield_s		domain;
	menufield_s		port;
	menutext_s	go;
	menutext_s	back;
} specifyserver_t;

static specifyserver_t	s_specifyserver;

static void SpecifyServer_DrawAction( void *self ) {
	menutext_s *button;
	qboolean focus;

	button = (menutext_s *)self;
	focus = ( Menu_ItemAtCursor( button->generic.parent ) == button );
	Frontend_DrawButton( button->generic.left, button->generic.top,
		button->generic.right - button->generic.left,
		button->generic.bottom - button->generic.top,
		button->string, 1.0f, focus, UI_FRONTEND_TEXT_CENTER );
}

static void SpecifyServer_DrawField( void *self ) {
	menufield_s *field;
	qboolean focus;

	field = (menufield_s *)self;
	focus = ( Menu_ItemAtCursor( field->generic.parent ) == field );
	Frontend_DrawCard( field->generic.left, field->generic.top,
		field->generic.right - field->generic.left,
		field->generic.bottom - field->generic.top, 1.0f, focus );
	Frontend_DrawText( field->generic.left + 12, field->generic.top + 7,
		field->field.buffer[0] ? field->field.buffer : "Enter value",
		UI_LEFT | UI_SMALLFONT,
		field->field.buffer[0] ? specifyTextColor : specifyMutedColor );
}

static void SpecifyServer_Draw( void ) {
	vec4_t scrimColor = UI_FRONTEND_COLOR_SCRIM;

	Frontend_DrawBackground( scrimColor );
	Frontend_DrawPanel( SPECIFY_FRAME_X, SPECIFY_FRAME_Y,
		SPECIFY_FRAME_WIDTH, SPECIFY_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( SPECIFY_FRAME_X + 24, SPECIFY_FRAME_Y + 24,
		"Connect to server", UI_LEFT | UI_BIGFONT, specifyTextColor );
	Frontend_DrawText( SPECIFY_FRAME_X + 24, SPECIFY_FRAME_Y + 48,
		"Enter an address and optional port", UI_LEFT | UI_SMALLFONT,
		specifyMutedColor );
	Frontend_DrawStatusChip( SPECIFY_FRAME_X + SPECIFY_FRAME_WIDTH - 92,
		SPECIFY_FRAME_Y + 26, "Online", specifyAccentColor, 1.0f );

	Frontend_DrawCard( SPECIFY_CARD_X, SPECIFY_CARD_Y,
		SPECIFY_CARD_WIDTH, SPECIFY_CARD_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( SPECIFY_CARD_X + 24, SPECIFY_CARD_Y + 22,
		"Connection details", UI_LEFT | UI_SMALLFONT, specifyMutedColor );
	Frontend_DrawText( SPECIFY_CARD_X + 24, SPECIFY_ADDRESS_Y + 8,
		"Server address", UI_LEFT | UI_SMALLFONT,
		specifyMutedColor );
	Frontend_DrawText( SPECIFY_CARD_X + 24, SPECIFY_PORT_Y + 8,
		"Port", UI_LEFT | UI_SMALLFONT,
		specifyMutedColor );
	Frontend_DrawText( SPECIFY_CARD_X + 24, SPECIFY_CARD_Y + 142,
		"Example: rally.example.com:27960", UI_LEFT | UI_SMALLFONT,
		specifyMutedColor );
	Frontend_DrawText( SPECIFY_FRAME_X + SPECIFY_FRAME_WIDTH - 24,
		SPECIFY_FRAME_Y + 364, "Enter connect   Esc back",
		UI_RIGHT | UI_SMALLFONT, specifyMutedColor );

	Menu_Draw( &s_specifyserver.menu );
}

/*
=================
SpecifyServer_Event
=================
*/
static void SpecifyServer_Event( void* ptr, int event )
{
	char	buff[256];

	switch (((menucommon_s*)ptr)->id)
	{
		case ID_SPECIFYSERVERGO:
			if (event != QM_ACTIVATED)
				break;

			if (s_specifyserver.domain.field.buffer[0])
			{
				strcpy(buff,s_specifyserver.domain.field.buffer);
				if (s_specifyserver.port.field.buffer[0])
					Com_sprintf( buff+strlen(buff), 128, ":%s", s_specifyserver.port.field.buffer );

				trap_Cmd_ExecuteText( EXEC_APPEND, va( "connect %s\n", buff ) );
			}
			break;

		case ID_SPECIFYSERVERBACK:
			if (event != QM_ACTIVATED)
				break;

			UI_PopMenu();
			break;
	}
}

/*
=================
SpecifyServer_MenuInit
=================
*/
void SpecifyServer_MenuInit( void )
{
	// zero set all our globals
	memset( &s_specifyserver, 0 ,sizeof(specifyserver_t) );

	SpecifyServer_Cache();

	s_specifyserver.menu.wrapAround = qtrue;
	s_specifyserver.menu.fullscreen = qtrue;
	s_specifyserver.menu.draw = SpecifyServer_Draw;

	s_specifyserver.banner.generic.type	 = MTYPE_BTEXT;
	s_specifyserver.banner.generic.x     = 320;
	s_specifyserver.banner.generic.y     = 16;
	s_specifyserver.banner.string		 = "SPECIFY SERVER";
	s_specifyserver.banner.color  		 = color_white;
	s_specifyserver.banner.style  		 = UI_CENTER;

	s_specifyserver.domain.generic.type       = MTYPE_FIELD;
	s_specifyserver.domain.generic.name       = "Address:";
	s_specifyserver.domain.generic.flags      = QMF_PULSEIFFOCUS|QMF_SMALLFONT|QMF_NODEFAULTINIT;
	s_specifyserver.domain.generic.x	      = 206;
	s_specifyserver.domain.generic.y	      = 220;
	s_specifyserver.domain.field.widthInChars = 38;
	s_specifyserver.domain.field.maxchars     = 80;

	s_specifyserver.port.generic.type       = MTYPE_FIELD;
	s_specifyserver.port.generic.name	    = "Port:";
	s_specifyserver.port.generic.flags	    = QMF_PULSEIFFOCUS|QMF_SMALLFONT|QMF_NUMBERSONLY|QMF_NODEFAULTINIT;
	s_specifyserver.port.generic.x	        = 206;
	s_specifyserver.port.generic.y	        = 250;
	s_specifyserver.port.field.widthInChars = 6;
	s_specifyserver.port.field.maxchars     = 5;

	s_specifyserver.go.generic.type	    = MTYPE_PTEXT;
	s_specifyserver.go.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_specifyserver.go.generic.callback = SpecifyServer_Event;
	s_specifyserver.go.generic.id	      = ID_SPECIFYSERVERGO;
	s_specifyserver.go.generic.x		    = 640 - 20;
	s_specifyserver.go.generic.y		    = 480 - 50;
	s_specifyserver.go.string				    = "CONNECT >";
	s_specifyserver.go.color				    = text_color_normal;
	s_specifyserver.go.style				  = UI_RIGHT | UI_SMALLFONT;
	
	s_specifyserver.back.generic.type			= MTYPE_PTEXT;
	s_specifyserver.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_specifyserver.back.generic.callback = SpecifyServer_Event;
	s_specifyserver.back.generic.id	      = ID_SPECIFYSERVERBACK; 
	s_specifyserver.back.generic.x			  = 20;
	s_specifyserver.back.generic.y			  = 480 - 50;
	s_specifyserver.back.string				    = "< BACK";
	s_specifyserver.back.color				    = text_color_normal;
	s_specifyserver.back.style				    = UI_LEFT | UI_SMALLFONT;

	s_specifyserver.banner.generic.flags = QMF_INACTIVE | QMF_HIDDEN;

	s_specifyserver.domain.generic.x = SPECIFY_FIELD_X + SPECIFY_FIELD_WIDTH / 2;
	s_specifyserver.domain.generic.y = SPECIFY_ADDRESS_Y + SPECIFY_FIELD_HEIGHT / 2;
	s_specifyserver.domain.generic.left = SPECIFY_FIELD_X;
	s_specifyserver.domain.generic.top = SPECIFY_ADDRESS_Y;
	s_specifyserver.domain.generic.right = SPECIFY_FIELD_X + SPECIFY_FIELD_WIDTH;
	s_specifyserver.domain.generic.bottom = SPECIFY_ADDRESS_Y + SPECIFY_FIELD_HEIGHT;
	s_specifyserver.domain.generic.ownerdraw = SpecifyServer_DrawField;

	s_specifyserver.port.generic.x = SPECIFY_FIELD_X + SPECIFY_FIELD_WIDTH / 2;
	s_specifyserver.port.generic.y = SPECIFY_PORT_Y + SPECIFY_FIELD_HEIGHT / 2;
	s_specifyserver.port.generic.left = SPECIFY_FIELD_X;
	s_specifyserver.port.generic.top = SPECIFY_PORT_Y;
	s_specifyserver.port.generic.right = SPECIFY_FIELD_X + SPECIFY_FIELD_WIDTH;
	s_specifyserver.port.generic.bottom = SPECIFY_PORT_Y + SPECIFY_FIELD_HEIGHT;
	s_specifyserver.port.generic.ownerdraw = SpecifyServer_DrawField;

	s_specifyserver.back.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS |
		QMF_NODEFAULTINIT;
	s_specifyserver.back.generic.x = 112;
	s_specifyserver.back.generic.y = SPECIFY_ACTION_Y + SPECIFY_ACTION_HEIGHT / 2;
	s_specifyserver.back.generic.left = 64;
	s_specifyserver.back.generic.top = SPECIFY_ACTION_Y;
	s_specifyserver.back.generic.right = 64 + SPECIFY_ACTION_WIDTH;
	s_specifyserver.back.generic.bottom = SPECIFY_ACTION_Y + SPECIFY_ACTION_HEIGHT;
	s_specifyserver.back.generic.ownerdraw = SpecifyServer_DrawAction;
	s_specifyserver.back.string = "Back";
	s_specifyserver.back.style = UI_CENTER | UI_SMALLFONT;

	s_specifyserver.go.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS |
		QMF_NODEFAULTINIT;
	s_specifyserver.go.generic.x = 528;
	s_specifyserver.go.generic.y = SPECIFY_ACTION_Y + SPECIFY_ACTION_HEIGHT / 2;
	s_specifyserver.go.generic.left = 480;
	s_specifyserver.go.generic.top = SPECIFY_ACTION_Y;
	s_specifyserver.go.generic.right = 480 + SPECIFY_ACTION_WIDTH;
	s_specifyserver.go.generic.bottom = SPECIFY_ACTION_Y + SPECIFY_ACTION_HEIGHT;
	s_specifyserver.go.generic.ownerdraw = SpecifyServer_DrawAction;
	s_specifyserver.go.string = "Connect";
	s_specifyserver.go.style = UI_CENTER | UI_SMALLFONT;

	Menu_AddItem( &s_specifyserver.menu, &s_specifyserver.banner );
	Menu_AddItem( &s_specifyserver.menu, &s_specifyserver.domain );
	Menu_AddItem( &s_specifyserver.menu, &s_specifyserver.port );
	Menu_AddItem( &s_specifyserver.menu, &s_specifyserver.go );
	Menu_AddItem( &s_specifyserver.menu, &s_specifyserver.back );

	Com_sprintf( s_specifyserver.port.field.buffer, 6, "%i", 27960 );
}

/*
=================
SpecifyServer_Cache
=================
*/
void SpecifyServer_Cache( void )
{
	int	i;

	// touch all our pics
	for (i=0; ;i++)
	{
		if (!specifyserver_artlist[i])
			break;
		trap_R_RegisterShaderNoMip(specifyserver_artlist[i]);
	}
}

/*
=================
UI_SpecifyServerMenu
=================
*/
void UI_SpecifyServerMenu( void )
{
	SpecifyServer_MenuInit();
	UI_PushMenu( &s_specifyserver.menu );
}

