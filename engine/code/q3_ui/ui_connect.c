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
#include "ui_rally_theme.h"
#include "ui_rally_frontend.h"

/*
===============================================================================

CONNECTION SCREEN

===============================================================================
*/

qboolean	passwordNeeded = qtrue;
menufield_s passwordField;

#define CONNECT_FRAME_X            48
#define CONNECT_FRAME_Y            20
#define CONNECT_FRAME_WIDTH        544
#define CONNECT_FRAME_HEIGHT       420
#define CONNECT_MAP_X              72
#define CONNECT_MAP_Y              112
#define CONNECT_MAP_WIDTH          224
#define CONNECT_MAP_HEIGHT         260
#define CONNECT_MAP_IMAGE_X        84
#define CONNECT_MAP_IMAGE_Y        148
#define CONNECT_MAP_IMAGE_WIDTH     200
#define CONNECT_MAP_IMAGE_HEIGHT    112
#define CONNECT_STATUS_X            320
#define CONNECT_STATUS_Y            112
#define CONNECT_STATUS_WIDTH        248
#define CONNECT_STATUS_HEIGHT       260
#define CONNECT_ACTION_Y            402

static vec4_t connectTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t connectMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t connectScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t connectStatusColor = UI_FRONTEND_COLOR_STATUS;
static qhandle_t connectMapShader;
static char connectMapShaderName[MAX_QPATH];

static void UI_ConnectFitText( char *out, int outSize, const char *text,
	int maxWidth ) {
	Q_strncpyz( out, text ? text : "", outSize );
	while ( out[0] && Frontend_TextWidth( out, UI_SMALLFONT ) > maxWidth ) {
		out[strlen( out ) - 1] = '\0';
	}
}

static qhandle_t UI_ConnectMapShader( const char *mapname ) {
	char imageName[MAX_QPATH];

	if ( !mapname || !mapname[0] ) {
		Q_strncpyz( imageName, "gfx/ui/q3rally_missing_map_shot",
			sizeof( imageName ) );
	} else {
		Com_sprintf( imageName, sizeof( imageName ), "levelshots/%s", mapname );
	}

	if ( Q_stricmp( imageName, connectMapShaderName ) ) {
		Q_strncpyz( connectMapShaderName, imageName,
			sizeof( connectMapShaderName ) );
		connectMapShader = trap_R_RegisterShaderNoMip( imageName );
		if ( !connectMapShader && Q_stricmp( imageName,
			"gfx/ui/q3rally_missing_map_shot" ) ) {
			connectMapShader = trap_R_RegisterShaderNoMip(
				"gfx/ui/q3rally_missing_map_shot" );
		}
	}

	return connectMapShader;
}

static void UI_ReadableSize ( char *buf, int bufsize, int value )
{
	if (value > 1024*1024*1024 ) { // gigs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d GB", 
			(value % (1024*1024*1024))*100 / (1024*1024*1024) );
	} else if (value > 1024*1024 ) { // megs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d MB", 
			(value % (1024*1024))*100 / (1024*1024) );
	} else if (value > 1024 ) { // kilos
		Com_sprintf( buf, bufsize, "%d KB", value / 1024 );
	} else { // bytes
		Com_sprintf( buf, bufsize, "%d bytes", value );
	}
}

// Assumes time is in msec
static void UI_PrintTime ( char *buf, int bufsize, int time ) {
	time /= 1000;  // change to seconds

	if (time > 3600) { // in the hours range
		Com_sprintf( buf, bufsize, "%d hr %d min", time / 3600, (time % 3600) / 60 );
	} else if (time > 60) { // mins
		Com_sprintf( buf, bufsize, "%d min %d sec", time / 60, time % 60 );
	} else  { // secs
		Com_sprintf( buf, bufsize, "%d sec", time );
	}
}

static void UI_DisplayDownloadInfo( const char *downloadName, int x, int y,
	int width, float alpha ) {
	int downloadSize;
	int downloadCount;
	int downloadTime;
	int xferRate;
	int progress;
	char downloadLabel[64];
	char dlSizeBuf[64];
	char totalSizeBuf[64];
	char xferRateBuf[64];
	char dlTimeBuf[64];

	downloadSize = trap_Cvar_VariableValue( "cl_downloadSize" );
	downloadCount = trap_Cvar_VariableValue( "cl_downloadCount" );
	downloadTime = trap_Cvar_VariableValue( "cl_downloadTime" );
	progress = downloadSize > 0 ? downloadCount * 100 / downloadSize : 0;

	UI_ConnectFitText( downloadLabel, sizeof( downloadLabel ), downloadName,
		width - 32 );
	Frontend_DrawText( x + 16, y + 48, "Downloading", UI_LEFT | UI_SMALLFONT,
		connectMutedColor );
	Frontend_DrawText( x + 16, y + 72, downloadLabel,
		UI_LEFT | UI_SMALLFONT, connectTextColor );
	Frontend_DrawProgress( x + 16, y + 94, width - 32, 7,
		progress / 100.0f, alpha );

	UI_ReadableSize( dlSizeBuf, sizeof( dlSizeBuf ), downloadCount );
	UI_ReadableSize( totalSizeBuf, sizeof( totalSizeBuf ), downloadSize );
	Frontend_DrawText( x + 16, y + 116,
		va( "%d%%   %s of %s", progress, dlSizeBuf, totalSizeBuf ),
		UI_LEFT | UI_SMALLFONT, connectMutedColor );

	if ( downloadCount < 4096 || !downloadTime ) {
		Frontend_DrawText( x + 16, y + 146, "Estimating remaining time...",
			UI_LEFT | UI_SMALLFONT, connectMutedColor );
		return;
	}

	if ( ( uis.realtime - downloadTime ) / 1000 ) {
		xferRate = downloadCount / (( uis.realtime - downloadTime ) / 1000);
	} else {
		xferRate = 0;
	}

	if ( downloadSize && xferRate ) {
		int remaining;
		int totalSeconds;

		totalSeconds = downloadSize / xferRate;
		remaining = ( totalSeconds -
			((( downloadCount / 1024 ) * totalSeconds ) /
			(downloadSize / 1024 )) ) * 1000;
		UI_PrintTime( dlTimeBuf, sizeof( dlTimeBuf ), remaining );
		Frontend_DrawText( x + 16, y + 146, "Time left",
			UI_LEFT | UI_SMALLFONT, connectMutedColor );
		Frontend_DrawText( x + width - 16, y + 146, dlTimeBuf,
			UI_RIGHT | UI_SMALLFONT, connectTextColor );
	} else {
		Frontend_DrawText( x + 16, y + 146, "Time left",
			UI_LEFT | UI_SMALLFONT, connectMutedColor );
		Frontend_DrawText( x + width - 16, y + 146, "Estimating",
			UI_RIGHT | UI_SMALLFONT, connectTextColor );
	}

	UI_ReadableSize( xferRateBuf, sizeof( xferRateBuf ), xferRate );
	Frontend_DrawText( x + 16, y + 172, "Transfer rate",
		UI_LEFT | UI_SMALLFONT, connectMutedColor );
	Frontend_DrawText( x + width - 16, y + 172,
		xferRate ? va( "%s/s", xferRateBuf ) : "-",
		UI_RIGHT | UI_SMALLFONT, connectTextColor );
}

/*
========================
UI_DrawConnectScreen

This will also be overlaid on the cgame info screen during loading
to prevent it from blinking away too rapidly on local or lan games.
========================
*/
void UI_DrawConnectScreen( qboolean overlay ) {
	uiClientState_t cstate;
	char info[MAX_INFO_VALUE];
	char mapPathName[MAX_QPATH];
	char mapName[MAX_QPATH];
	char serverName[96];
	char statusText[128];
	char messageText[128];
	char motd[128];
	const char *statusLabel;
	qhandle_t mapShader;
	qboolean downloading;
	float alpha;

	Menu_Cache();

	// see what information we should display
	trap_GetClientState( &cstate );

	info[0] = '\0';
	mapPathName[0] = '\0';
	mapName[0] = '\0';
	if ( trap_GetConfigString( CS_SERVERINFO, info, sizeof( info ) ) ) {
		Q_strncpyz( mapPathName, Info_ValueForKey( info, "mapname" ),
			sizeof( mapPathName ) );
	}
	Q_strncpyz( mapName, mapPathName, sizeof( mapName ) );
	if ( !mapName[0] ) {
		Q_strncpyz( mapName, "Unknown track", sizeof( mapName ) );
	}
	UI_ConnectFitText( mapName, sizeof( mapName ), mapName,
		CONNECT_MAP_WIDTH - 32 );
	UI_ConnectFitText( serverName, sizeof( serverName ), cstate.servername,
		CONNECT_STATUS_WIDTH - 32 );
	mapShader = UI_ConnectMapShader( mapPathName );
	alpha = overlay ? 0.96f : 1.0f;

	if ( !overlay ) {
		Frontend_DrawBackground( connectScrimColor );
	}

	Frontend_DrawPanel( CONNECT_FRAME_X, CONNECT_FRAME_Y,
		CONNECT_FRAME_WIDTH, CONNECT_FRAME_HEIGHT, alpha,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( CONNECT_FRAME_X + 24, CONNECT_FRAME_Y + 24,
		"Map loading", UI_LEFT | UI_BIGFONT, connectTextColor );
	Frontend_DrawText( CONNECT_FRAME_X + 24, CONNECT_FRAME_Y + 48,
		"Preparing the next rally stage", UI_LEFT | UI_SMALLFONT,
		connectMutedColor );

	statusLabel = "Connecting";
	statusText[0] = '\0';
	downloading = qfalse;

	switch ( cstate.connState ) {
	case CA_CONNECTING:
		Com_sprintf( statusText, sizeof( statusText ),
			"Awaiting challenge... %i", cstate.connectPacketCount );
		break;
	case CA_CHALLENGING:
		Com_sprintf( statusText, sizeof( statusText ),
			"Awaiting connection... %i", cstate.connectPacketCount );
		break;
	case CA_CONNECTED: {
		char downloadName[MAX_INFO_VALUE];

		trap_Cvar_VariableStringBuffer( "cl_downloadName", downloadName,
			sizeof( downloadName ) );
		if ( downloadName[0] ) {
			statusLabel = "Download";
			downloading = qtrue;
		} else {
			statusLabel = "Loading";
			Q_strncpyz( statusText, "Awaiting game state...",
				sizeof( statusText ) );
		}
		break;
	}
	case CA_LOADING:
		statusLabel = "Loading map";
		Q_strncpyz( statusText, "Preparing track resources...",
			sizeof( statusText ) );
		break;
	case CA_PRIMED:
		statusLabel = "Starting race";
		Q_strncpyz( statusText, "Finalizing session...", sizeof( statusText ) );
		break;
	default:
		statusLabel = "Waiting";
		Q_strncpyz( statusText, "Waiting for the game...", sizeof( statusText ) );
		break;
	}

	Frontend_DrawStatusChip( CONNECT_FRAME_X + CONNECT_FRAME_WIDTH - 136,
		CONNECT_FRAME_Y + 26, statusLabel, connectStatusColor, alpha );

	Frontend_DrawCard( CONNECT_MAP_X, CONNECT_MAP_Y, CONNECT_MAP_WIDTH,
		CONNECT_MAP_HEIGHT, alpha, qfalse );
	Frontend_DrawText( CONNECT_MAP_X + 16, CONNECT_MAP_Y + 18,
		"Track preview", UI_LEFT | UI_SMALLFONT, connectMutedColor );
	if ( mapShader ) {
		UI_DrawHandlePic( CONNECT_MAP_IMAGE_X, CONNECT_MAP_IMAGE_Y,
			CONNECT_MAP_IMAGE_WIDTH, CONNECT_MAP_IMAGE_HEIGHT, mapShader );
	}
	Frontend_DrawText( CONNECT_MAP_X + 16, CONNECT_MAP_Y + 174,
		mapName, UI_LEFT | UI_SMALLFONT, connectTextColor );
	Frontend_DrawText( CONNECT_MAP_X + 16, CONNECT_MAP_Y + 204,
		"Server", UI_LEFT | UI_SMALLFONT, connectMutedColor );
	Frontend_DrawText( CONNECT_MAP_X + 16, CONNECT_MAP_Y + 226,
		serverName[0] ? serverName : "Local session",
		UI_LEFT | UI_SMALLFONT, connectTextColor );

	Frontend_DrawCard( CONNECT_STATUS_X, CONNECT_STATUS_Y,
		CONNECT_STATUS_WIDTH, CONNECT_STATUS_HEIGHT, alpha, qfalse );
	Frontend_DrawText( CONNECT_STATUS_X + 16, CONNECT_STATUS_Y + 18,
		"Connection status", UI_LEFT | UI_SMALLFONT, connectMutedColor );

	if ( downloading ) {
		char downloadName[MAX_INFO_VALUE];

		trap_Cvar_VariableStringBuffer( "cl_downloadName", downloadName,
			sizeof( downloadName ) );
		UI_DisplayDownloadInfo( downloadName, CONNECT_STATUS_X,
			CONNECT_STATUS_Y + 30, CONNECT_STATUS_WIDTH, alpha );
	} else {
		UI_ConnectFitText( statusText, sizeof( statusText ), statusText,
			CONNECT_STATUS_WIDTH - 32 );
		Frontend_DrawText( CONNECT_STATUS_X + 16, CONNECT_STATUS_Y + 58,
			statusText, UI_LEFT | UI_SMALLFONT, connectTextColor );
	}

	if ( cstate.connState < CA_CONNECTED && cstate.messageString[0] ) {
		UI_ConnectFitText( messageText, sizeof( messageText ),
			cstate.messageString, CONNECT_STATUS_WIDTH - 32 );
		Frontend_DrawText( CONNECT_STATUS_X + 16,
			CONNECT_STATUS_Y + ( downloading ? 228 : 104 ), messageText,
			UI_LEFT | UI_SMALLFONT, connectMutedColor );
	}

	Q_strncpyz( motd, Info_ValueForKey( cstate.updateInfoString, "motd" ),
		sizeof( motd ) );
	UI_ConnectFitText( motd, sizeof( motd ), motd, CONNECT_FRAME_WIDTH - 48 );
	Frontend_DrawText( CONNECT_FRAME_X + 24, CONNECT_ACTION_Y,
		"Esc cancel", UI_LEFT | UI_SMALLFONT, connectMutedColor );
	if ( motd[0] ) {
		Frontend_DrawText( CONNECT_FRAME_X + CONNECT_FRAME_WIDTH - 24,
			CONNECT_ACTION_Y, motd, UI_RIGHT | UI_SMALLFONT,
			connectMutedColor );
	}

#if 0
	// display password field
	if ( passwordNeeded ) {
		s_ingame_menu.x = SCREEN_WIDTH * 0.50 - 128;
		s_ingame_menu.nitems = 0;
		s_ingame_menu.wrapAround = qtrue;

		passwordField.generic.type = MTYPE_FIELD;
		passwordField.generic.name = "Password:";
		passwordField.generic.callback = 0;
		passwordField.generic.x		= 10;
		passwordField.generic.y		= 180;
		Field_Clear( &passwordField.field );
		passwordField.width = 256;
		passwordField.field.widthInChars = 16;
		Q_strncpyz( passwordField.field.buffer, Cvar_VariableString("password"), 
			sizeof(passwordField.field.buffer) );

		Menu_AddItem( &s_ingame_menu, ( void * ) &s_customize_player_action );

		MField_Draw( &passwordField );
	}
#endif

}


/*
===================
UI_KeyConnect
===================
*/
void UI_KeyConnect( int key ) {
	if ( key == K_ESCAPE ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect\n" );
		return;
	}
}
