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

static char* serverinfo_artlist[] =
{
	NULL
};

#define ID_ADD	 100
#define ID_BACK	 101

#define SERVERINFO_FRAME_X             24
#define SERVERINFO_FRAME_Y             20
#define SERVERINFO_FRAME_WIDTH         592
#define SERVERINFO_FRAME_HEIGHT        440
#define SERVERINFO_CARD_X              48
#define SERVERINFO_CARD_Y              112
#define SERVERINFO_CARD_WIDTH          544
#define SERVERINFO_CARD_HEIGHT         260
#define SERVERINFO_COLUMN_WIDTH        256
#define SERVERINFO_ROW_Y               144
#define SERVERINFO_ROW_HEIGHT          24
#define SERVERINFO_ROW_GAP             2
#define SERVERINFO_ROWS_PER_COLUMN     8
#define SERVERINFO_ACTION_Y            420
#define SERVERINFO_ACTION_WIDTH        144
#define SERVERINFO_ACTION_HEIGHT       24

static vec4_t serverInfoTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t serverInfoMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t serverInfoAccentColor = UI_FRONTEND_COLOR_ACCENT;

/*
 * The server info string also contains implementation details used by the
 * engine.  Showing that complete string makes the card grow sideways and is
 * not useful to a player.  Keep the main page focused on settings that help
 * decide whether to join the server.
 */
typedef struct
{
	const char *key;
	const char *label;
} serverInfoField_t;

static const serverInfoField_t serverInfoFields[] = {
	{ "sv_hostname",          "Server" },
	{ "g_gametype",           "Mode" },
	{ "mapname",              "Map" },
	{ "clients",              "Players" },
	{ "sv_maxclients",        "Max players" },
	{ "timelimit",            "Time limit" },
	{ "fraglimit",            "Score limit" },
	{ "capturelimit",         "Capture limit" },
	{ "laplimit",             "Lap limit" },
	{ "g_timeTrialLaps",      "Time trial laps" },
	{ "g_trackLength",        "Track length" },
	{ "g_trackReversed",      "Reverse track" },
	{ "g_useFuel",            "Fuel" },
	{ "g_eliminationWeapons", "Weapons" },
	{ "g_needpass",           "Password" },
	{ "sv_pure",              "Pure server" },
	{ NULL,                    NULL }
};

static const char *ServerInfo_GameTypeName( int gametype )
{
	switch ( gametype ) {
	case GT_RACING:          return "Racing";
	case GT_RACING_DM:       return "Racing DM";
	case GT_SINGLE_PLAYER:   return "Single Player";
	case GT_DERBY:           return "Demolition Derby";
	case GT_LCS:             return "Last Car Standing";
	case GT_ELIMINATION:     return "Elimination";
	case GT_DEATHMATCH:      return "Deathmatch";
	case GT_SPRINT:          return "Sprint";
	case GT_TEAM:            return "Team Deathmatch";
	case GT_TEAM_RACING:     return "Team Racing";
	case GT_TEAM_RACING_DM:  return "Team Racing DM";
	case GT_CTF:             return "Capture the Flag";
	case GT_CTF4:            return "4-Team CTF";
	case GT_DOMINATION:      return "Domination";
	case GT_KOTH:            return "King of the Hill";
	default:                 return NULL;
	}
}

static qboolean ServerInfo_FormatValue( const serverInfoField_t *field,
	const char *raw, char *formatted, int formattedSize )
{
	int value;
	const char *name;

	if ( !raw || !raw[0] ) {
		return qfalse;
	}

	if ( !Q_stricmp( field->key, "g_gametype" ) ) {
		name = ServerInfo_GameTypeName( atoi( raw ) );
		if ( name ) {
			Q_strncpyz( formatted, name, formattedSize );
		} else {
			Q_strncpyz( formatted, raw, formattedSize );
		}
		return qtrue;
	}

	if ( !Q_stricmp( field->key, "g_trackLength" ) ) {
		value = atoi( raw );
		if ( value == 0 ) {
			Q_strncpyz( formatted, "Short", formattedSize );
		} else if ( value == 1 ) {
			Q_strncpyz( formatted, "Medium", formattedSize );
		} else if ( value == 2 ) {
			Q_strncpyz( formatted, "Long", formattedSize );
		} else {
			Q_strncpyz( formatted, raw, formattedSize );
		}
		return qtrue;
	}

	if ( !Q_stricmp( field->key, "g_trackReversed" ) ||
		!Q_stricmp( field->key, "g_useFuel" ) ||
		!Q_stricmp( field->key, "g_eliminationWeapons" ) ||
		!Q_stricmp( field->key, "g_needpass" ) ||
		!Q_stricmp( field->key, "sv_pure" ) ) {
		Q_strncpyz( formatted, atoi( raw ) ? "On" : "Off", formattedSize );
		return qtrue;
	}

	Q_strncpyz( formatted, raw, formattedSize );
	return qtrue;
}

typedef struct
{
	menuframework_s	menu;
	menutext_s		banner;
	menutext_s	    back;
	menutext_s		add;
	char			info[MAX_INFO_STRING];
	int				numlines;
} serverinfo_t;

static serverinfo_t	s_serverinfo;

static const char *ServerInfo_FieldValue( const char *key )
{
	const char *value;

	value = Info_ValueForKey( s_serverinfo.info, key );
	if ( value[0] || Q_stricmp( key, "sv_hostname" ) ) {
		return value;
	}

	/* Browser responses use the shorter key, while CS_SERVERINFO normally
	 * exposes sv_hostname.  Accept both so the panel also works for local
	 * and older servers. */
	return Info_ValueForKey( s_serverinfo.info, "hostname" );
}

static void ServerInfo_DrawAction( void *self ) {
	menutext_s *button;
	qboolean focus;
	qboolean disabled;
	const float *textColor;

	button = (menutext_s *)self;
	focus = ( Menu_ItemAtCursor( button->generic.parent ) == button );
	disabled = ( button->generic.flags & QMF_GRAYED ) ? qtrue : qfalse;
	textColor = disabled ? serverInfoMutedColor :
		( focus ? serverInfoAccentColor : serverInfoTextColor );

	Frontend_DrawCard( button->generic.left, button->generic.top,
		button->generic.right - button->generic.left,
		button->generic.bottom - button->generic.top, 1.0f, focus );
	Frontend_DrawText( button->generic.left + 12,
		button->generic.top + 5, button->string,
		UI_LEFT | UI_SMALLFONT, textColor );
}


/*
=================
Favorites_Add

Add current server to favorites
=================
*/
void Favorites_Add( void )
{
	char	adrstr[128];
	char	serverbuff[128];
	int		i;
	int		best;

	trap_Cvar_VariableStringBuffer( "cl_currentServerAddress", serverbuff, sizeof(serverbuff) );
	if (!serverbuff[0])
		return;

	best = 0;
	for (i=0; i<MAX_FAVORITESERVERS; i++)
	{
		trap_Cvar_VariableStringBuffer( va("server%d",i+1), adrstr, sizeof(adrstr) );
		if (!Q_stricmp(serverbuff,adrstr))
		{
			// already in list
			return;
		}
		
		// use first empty or non-numeric available slot
		if ((adrstr[0]  < '0' || adrstr[0] > '9' ) && !best)
			best = i+1;
	}

	if (best)
		trap_Cvar_Set( va("server%d",best), serverbuff);
}


/*
=================
ServerInfo_Event
=================
*/
static void ServerInfo_Event( void* ptr, int event )
{
	switch (((menucommon_s*)ptr)->id)
	{
		case ID_ADD:
			if (event != QM_ACTIVATED)
				break;
		
			Favorites_Add();
			UI_PopMenu();
			break;

		case ID_BACK:
			if (event != QM_ACTIVATED)
				break;

			UI_PopMenu();
			break;
	}
}

/*
=================
ServerInfo_MenuDrawFrontend
=================
*/
static void ServerInfo_MenuDrawFrontend( void )
{
	const serverInfoField_t *field;
	const char *rawValue;
	char formattedValue[MAX_INFO_VALUE];
	int fieldIndex;
	int shown;
	int column;
	int row;
	int x;
	int y;
	vec4_t scrimColor = UI_FRONTEND_COLOR_SCRIM;

	Frontend_DrawBackground( scrimColor );
	Frontend_DrawPanel( SERVERINFO_FRAME_X, SERVERINFO_FRAME_Y,
		SERVERINFO_FRAME_WIDTH, SERVERINFO_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( SERVERINFO_FRAME_X + 24, SERVERINFO_FRAME_Y + 24,
		"Server info", UI_LEFT | UI_BIGFONT, serverInfoTextColor );
	Frontend_DrawText( SERVERINFO_FRAME_X + 24, SERVERINFO_FRAME_Y + 48,
		"Player-relevant settings and connection details",
		UI_LEFT | UI_SMALLFONT, serverInfoMutedColor );
	Frontend_DrawStatusChip( SERVERINFO_FRAME_X + SERVERINFO_FRAME_WIDTH - 112,
		SERVERINFO_FRAME_Y + 26, "Online", serverInfoAccentColor, 1.0f );
	Frontend_DrawCard( SERVERINFO_CARD_X, SERVERINFO_CARD_Y,
		SERVERINFO_CARD_WIDTH, SERVERINFO_CARD_HEIGHT, 1.0f, qfalse );

	fieldIndex = 0;
	shown = 0;
	while ( serverInfoFields[fieldIndex].key &&
		shown < SERVERINFO_ROWS_PER_COLUMN * 2 ) {
		field = &serverInfoFields[fieldIndex];
		rawValue = ServerInfo_FieldValue( field->key );
		if ( ServerInfo_FormatValue( field, rawValue, formattedValue,
			sizeof(formattedValue) ) ) {
			column = shown / SERVERINFO_ROWS_PER_COLUMN;
			row = shown % SERVERINFO_ROWS_PER_COLUMN;
		x = SERVERINFO_CARD_X + 16 + column *
			( SERVERINFO_COLUMN_WIDTH + 16 );
		y = SERVERINFO_ROW_Y + row *
			( SERVERINFO_ROW_HEIGHT + SERVERINFO_ROW_GAP );
		Frontend_DrawCard( x, y, SERVERINFO_COLUMN_WIDTH,
			SERVERINFO_ROW_HEIGHT, 1.0f, qfalse );
		Frontend_DrawText( x + 8, y + 5, field->label,
			UI_LEFT | UI_SMALLFONT, serverInfoMutedColor );
		Frontend_DrawText( x + SERVERINFO_COLUMN_WIDTH - 8, y + 5,
			formattedValue, UI_RIGHT | UI_SMALLFONT, serverInfoTextColor );
			shown++;
		}
		fieldIndex++;
	}

	if ( !shown ) {
		Frontend_DrawText( SERVERINFO_CARD_X + 16, SERVERINFO_CARD_Y + 24,
			"No server information available", UI_LEFT | UI_SMALLFONT,
			serverInfoMutedColor );
	}

	Frontend_DrawText( SERVERINFO_FRAME_X + 24, SERVERINFO_FRAME_Y + 376,
		"Enter select   Esc back", UI_LEFT | UI_SMALLFONT,
		serverInfoMutedColor );
	Menu_Draw( &s_serverinfo.menu );
}

/*
=================
ServerInfo_MenuKey
=================
*/
static sfxHandle_t ServerInfo_MenuKey( int key )
{
	return ( Menu_DefaultKey( &s_serverinfo.menu, key ) );
}

/*
=================
ServerInfo_Cache
=================
*/
void ServerInfo_Cache( void )
{
	int	i;

	// touch all our pics
	for (i=0; ;i++)
	{
		if (!serverinfo_artlist[i])
			break;
		trap_R_RegisterShaderNoMip(serverinfo_artlist[i]);
	}
}

/*
=================
UI_ServerInfoMenu
=================
*/
void UI_ServerInfoMenu( void )
{
	const char		*s;
	char			key[MAX_INFO_KEY];
	char			value[MAX_INFO_VALUE];

	// zero set all our globals
	memset( &s_serverinfo, 0 ,sizeof(serverinfo_t) );

	ServerInfo_Cache();

	s_serverinfo.menu.draw       = ServerInfo_MenuDrawFrontend;
	s_serverinfo.menu.key        = ServerInfo_MenuKey;
	s_serverinfo.menu.wrapAround = qtrue;
	s_serverinfo.menu.fullscreen = qtrue;

	s_serverinfo.banner.generic.type  = MTYPE_BTEXT;
	s_serverinfo.banner.generic.x	  = 320;
	s_serverinfo.banner.generic.y	  = 16;
	s_serverinfo.banner.string		  = "SERVER INFO";
	s_serverinfo.banner.color	      = color_white;
	s_serverinfo.banner.style	      = UI_CENTER;
	s_serverinfo.banner.generic.flags = QMF_INACTIVE | QMF_HIDDEN;

	s_serverinfo.add.generic.type	  = MTYPE_PTEXT;
	s_serverinfo.add.generic.flags    = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_serverinfo.add.generic.callback = ServerInfo_Event;
	s_serverinfo.add.generic.id	      = ID_ADD;
	s_serverinfo.add.generic.x		  = 350;
	s_serverinfo.add.generic.y		  = 416;
	s_serverinfo.add.string  		  = "ADD TO FAVORITES";
	s_serverinfo.add.style  		  = UI_CENTER|UI_SMALLFONT;
	s_serverinfo.add.color			  =	color_red;
	if( trap_Cvar_VariableValue( "sv_running" ) ) {
		s_serverinfo.add.generic.flags |= QMF_GRAYED;
	}
	s_serverinfo.add.generic.left = SERVERINFO_FRAME_X + 16;
	s_serverinfo.add.generic.top = SERVERINFO_ACTION_Y;
	s_serverinfo.add.generic.right = s_serverinfo.add.generic.left +
		SERVERINFO_ACTION_WIDTH;
	s_serverinfo.add.generic.bottom = SERVERINFO_ACTION_Y +
		SERVERINFO_ACTION_HEIGHT;
	s_serverinfo.add.generic.x = ( s_serverinfo.add.generic.left +
		s_serverinfo.add.generic.right ) / 2;
	s_serverinfo.add.generic.y = SERVERINFO_ACTION_Y + 4;
	s_serverinfo.add.generic.flags |= QMF_NODEFAULTINIT;
	s_serverinfo.add.generic.ownerdraw = ServerInfo_DrawAction;

	s_serverinfo.back.generic.type	   = MTYPE_PTEXT;
	s_serverinfo.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_serverinfo.back.generic.callback = ServerInfo_Event;
	s_serverinfo.back.generic.id	   = ID_BACK;
	s_serverinfo.back.generic.x		   = 0;
	s_serverinfo.back.generic.y		   = 416;
    s_serverinfo.back.string           = "<BACK";
    s_serverinfo.back.style            = UI_LEFT|UI_SMALLFONT;
    s_serverinfo.back.color            = color_red;
	s_serverinfo.back.generic.left = SERVERINFO_FRAME_X + SERVERINFO_FRAME_WIDTH -
		16 - SERVERINFO_ACTION_WIDTH;
	s_serverinfo.back.generic.top = SERVERINFO_ACTION_Y;
	s_serverinfo.back.generic.right = s_serverinfo.back.generic.left +
		SERVERINFO_ACTION_WIDTH;
	s_serverinfo.back.generic.bottom = SERVERINFO_ACTION_Y +
		SERVERINFO_ACTION_HEIGHT;
	s_serverinfo.back.generic.x = ( s_serverinfo.back.generic.left +
		s_serverinfo.back.generic.right ) / 2;
	s_serverinfo.back.generic.y = SERVERINFO_ACTION_Y + 4;
	s_serverinfo.back.generic.flags |= QMF_NODEFAULTINIT;
	s_serverinfo.back.generic.ownerdraw = ServerInfo_DrawAction;

	trap_GetConfigString( CS_SERVERINFO, s_serverinfo.info, MAX_INFO_STRING );

	s_serverinfo.numlines = 0;
	s = s_serverinfo.info;
	while ( s ) {
		Info_NextPair( &s, key, value );
		if ( !key[0] ) {
			break;
		}
		s_serverinfo.numlines++;
	}

	if (s_serverinfo.numlines > 16)
		s_serverinfo.numlines = 16;

	Menu_AddItem( &s_serverinfo.menu, (void*) &s_serverinfo.banner );
	Menu_AddItem( &s_serverinfo.menu, (void*) &s_serverinfo.add );
	Menu_AddItem( &s_serverinfo.menu, (void*) &s_serverinfo.back );

	UI_PushMenu( &s_serverinfo.menu );
}


