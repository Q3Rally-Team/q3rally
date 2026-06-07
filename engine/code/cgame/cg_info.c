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
// cg_info.c -- display information while data is being loading

#include "cg_local.h"

#define MAX_LOADING_PLAYER_ICONS	16
#define MAX_LOADING_ITEM_ICONS		26

static int			loadingPlayerIconCount;
static int			loadingItemIconCount;
static qhandle_t	loadingPlayerIcons[MAX_LOADING_PLAYER_ICONS];
static qhandle_t	loadingItemIcons[MAX_LOADING_ITEM_ICONS];

static vec4_t loadingTopScrimColor    = { 0.00f, 0.00f, 0.00f, 0.58f };
static vec4_t loadingBottomScrimColor = { 0.00f, 0.00f, 0.00f, 0.66f };
static vec4_t loadingPanelColor       = { 0.04f, 0.05f, 0.08f, 0.72f };
static vec4_t loadingPanelBandColor   = { 0.08f, 0.10f, 0.16f, 0.82f };
static vec4_t loadingBorderColor      = { 0.34f, 0.48f, 0.76f, 0.72f };
static vec4_t loadingAccentColor      = { 0.50f, 0.70f, 1.00f, 1.00f };
static vec4_t loadingMutedTextColor   = { 0.60f, 0.66f, 0.77f, 1.00f };

/*
===================
CG_DrawLoadingFrame
===================
*/
static void CG_DrawLoadingFrame( void ) {
	CG_FillRect( 0, 0, SCREEN_WIDTH, 132, loadingTopScrimColor );
	CG_FillRect( 0, 286, SCREEN_WIDTH, SCREEN_HEIGHT - 286, loadingBottomScrimColor );

	CG_FillRect( 0, 130, SCREEN_WIDTH, 2, loadingAccentColor );
	CG_FillRect( 0, 146, SCREEN_WIDTH, 126, loadingPanelColor );
	CG_FillRect( 0, 146, SCREEN_WIDTH, 22, loadingPanelBandColor );
	CG_FillRect( 0, 146, SCREEN_WIDTH, 2, loadingAccentColor );
	CG_DrawRect( 0, 146, SCREEN_WIDTH, 126, 1, loadingBorderColor );

	UI_DrawProportionalString( 320, 152, "MAP DETAILS",
		UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, loadingMutedTextColor );
}

/*
======================
CG_LoadingScreenDebugPause
======================
*/
static void CG_LoadingScreenDebugPause( void ) {
	int pause;
	int endTime;
	int nextUpdate;
	char pauseBuffer[16];

	trap_Cvar_VariableStringBuffer( "cg_loadingScreenPause", pauseBuffer, sizeof( pauseBuffer ) );
	pause = atoi( pauseBuffer );
	if ( pause <= 0 ) {
		return;
	}
	if ( pause > 10000 ) {
		pause = 10000;
	}

	endTime = trap_Milliseconds() + pause;
	nextUpdate = 0;
	while ( trap_Milliseconds() < endTime ) {
		if ( trap_Milliseconds() >= nextUpdate ) {
			trap_UpdateScreen();
			nextUpdate = trap_Milliseconds() + 50;
		}
	}
}


/*
===================
CG_DrawLoadingIcons
===================
*/
static void CG_DrawLoadingIcons( void ) {
	int		n;
	int		x, y;
	int		col, row;

	for( n = 0; n < loadingPlayerIconCount; n++ ) {
		col = n % 8;
		row = n / 8;
		x = 32 + col * 74;
		y = 290 + row * 54;
		CG_DrawPic( x, y, 48, 48, loadingPlayerIcons[n] );
	}

	for( n = 0; n < loadingItemIconCount; n++ ) {
		y = 396;
		if( n >= 13 ) {
			y += 36;
		}
		x = 16 + n % 13 * 48;
		CG_DrawPic( x, y, 32, 32, loadingItemIcons[n] );
	}
}


/*
======================
CG_LoadingString

======================
*/
void CG_LoadingString( const char *s ) {
	Q_strncpyz( cg.infoScreenText, s, sizeof( cg.infoScreenText ) );

	trap_UpdateScreen();
	CG_LoadingScreenDebugPause();
}

/*
===================
CG_LoadingItem
===================
*/
void CG_LoadingItem( int itemNum ) {
	gitem_t		*item;

	item = &bg_itemlist[itemNum];
	
	if ( item->icon && loadingItemIconCount < MAX_LOADING_ITEM_ICONS ) {
		loadingItemIcons[loadingItemIconCount++] = trap_R_RegisterShaderNoMip( item->icon );
	}

	CG_LoadingString( item->pickup_name );
}

/*
===================
CG_LoadingClient
===================
*/
void CG_LoadingClient( int clientNum ) {
	const char		*info;
	char			*skin;
	char			personality[MAX_QPATH];
	char			model[MAX_QPATH];
	char			iconName[MAX_QPATH];

	info = CG_ConfigString( CS_PLAYERS + clientNum );

	if ( loadingPlayerIconCount < MAX_LOADING_PLAYER_ICONS ) {
		Q_strncpyz( model, Info_ValueForKey( info, "model" ), sizeof( model ) );
		skin = strrchr( model, '/' );
		if ( skin ) {
			*skin++ = '\0';
		} else {
// Q3Rally Code Start
//			skin = "default";
			skin = DEFAULT_SKIN;
// Q3Rally Code END
		}

		Com_sprintf( iconName, MAX_QPATH, "models/players/%s/icon_%s.tga", model, skin );
		
		loadingPlayerIcons[loadingPlayerIconCount] = trap_R_RegisterShaderNoMip( iconName );
		if ( !loadingPlayerIcons[loadingPlayerIconCount] ) {
// Q3Rally Code Start
/*
			Com_sprintf( iconName, MAX_QPATH, "models/players/characters/%s/icon_%s.tga", model, skin );
			loadingPlayerIcons[loadingPlayerIconCount] = trap_R_RegisterShaderNoMip( iconName );
		}
		if ( !loadingPlayerIcons[loadingPlayerIconCount] ) {
			Com_sprintf( iconName, MAX_QPATH, "models/players/%s/icon_%s.tga", DEFAULT_MODEL, "default" );
*/
			Com_sprintf( iconName, MAX_QPATH, "models/players/%s/icon_%s.tga", DEFAULT_MODEL, DEFAULT_SKIN );
// Q3Rally Code END
			loadingPlayerIcons[loadingPlayerIconCount] = trap_R_RegisterShaderNoMip( iconName );
		}
		if ( loadingPlayerIcons[loadingPlayerIconCount] ) {
			loadingPlayerIconCount++;
		}
	}

	Q_strncpyz( personality, Info_ValueForKey( info, "n" ), sizeof(personality) );
	Q_CleanStr( personality );

	if( cgs.gametype == GT_SINGLE_PLAYER ) {
		trap_S_RegisterSound( va( "sound/player/announce/%s.wav", personality ), qtrue );
	}

	CG_LoadingString( personality );
}


/*
====================
CG_DrawInformation

Draw all the status / pacifier stuff during level loading
====================
*/
void CG_DrawInformation( void ) {
	const char	*s;
	const char	*info;
	const char	*sysInfo;
	int			y;
	int			value;
	qhandle_t	levelshot;
	qhandle_t	detail;
	char		buf[1024];

	info = CG_ConfigString( CS_SERVERINFO );
	sysInfo = CG_ConfigString( CS_SYSTEMINFO );

	s = Info_ValueForKey( info, "mapname" );
	levelshot = trap_R_RegisterShaderNoMip( va( "levelshots/%s", s ) );
	if ( !levelshot ) {
		levelshot = trap_R_RegisterShaderNoMip( "menu/art/unknownmap" );
	}
	trap_R_SetColor( NULL );
	trap_R_DrawStretchPic( 0, 0, cgs.glconfig.vidWidth, cgs.glconfig.vidHeight, 0, 0, 1, 1, levelshot );

	// blend a detail texture over it
	detail = trap_R_RegisterShader( "levelShotDetail" );
	trap_R_DrawStretchPic( 0, 0, cgs.glconfig.vidWidth, cgs.glconfig.vidHeight, 0, 0, 1, 1, detail );

	CG_DrawLoadingFrame();

	// draw the icons of things as they are loaded
	CG_DrawLoadingIcons();

	// the first 150 rows are reserved for the client connection
	// screen to write into
	if ( cg.infoScreenText[0] ) {
		UI_DrawProportionalString( 320, 128-32, va("Loading... %s", cg.infoScreenText),
			UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
	} else {
		UI_DrawProportionalString( 320, 128-32, "Awaiting snapshot...",
			UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
	}

        // draw info string information

        y = 178;

        // don't print server lines if playing a local game
	trap_Cvar_VariableStringBuffer( "sv_running", buf, sizeof( buf ) );
	if ( !atoi( buf ) ) {
		// server hostname
		Q_strncpyz(buf, Info_ValueForKey( info, "sv_hostname" ), 1024);
		Q_CleanStr(buf);
		UI_DrawProportionalString( 320, y, buf,
			UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
		y += PROP_HEIGHT;

		// pure server
		s = Info_ValueForKey( sysInfo, "sv_pure" );
		if ( s[0] == '1' ) {
			UI_DrawProportionalString( 320, y, "Pure Server",
				UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
			y += PROP_HEIGHT;
		}

		// server-specific message of the day
		s = CG_ConfigString( CS_MOTD );
		if ( s[0] ) {
			UI_DrawProportionalString( 320, y, s,
				UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
			y += PROP_HEIGHT;
		}

		// some extra space after hostname and motd
                y += 10;
        }

        // map-specific message (long map name)
        s = CG_ConfigString( CS_MESSAGE );
        if ( s[0] ) {
                UI_DrawProportionalString( 320, y, s,
			UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
		y += PROP_HEIGHT;
	}

	// cheats warning
	s = Info_ValueForKey( sysInfo, "sv_cheats" );
	if ( s[0] == '1' ) {
		UI_DrawProportionalString( 320, y, "CHEATS ARE ENABLED",
			UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
		y += PROP_HEIGHT;
	}

	// game type
	switch ( cgs.gametype ) {
// Q3Rally Code Start - removed gametype
/*
	case GT_FFA:
		s = "Free For All";
		break;
	case GT_SINGLE_PLAYER:
		s = "Single Player";
		break;
	case GT_TOURNAMENT:
		s = "Tournament";
		break;
*/
// Q3Rally Code END
	case GT_TEAM:
		s = "Team Deathmatch";
		break;
	case GT_CTF:
		s = "Capture The Flag";
		break;
// Q3Rally Code Start
	case GT_DOMINATION:
	    s = "Domination";        
		break;

/*
#ifdef MISSIONPACK
	case GT_1FCTF:
		s = "One Flag CTF";
		break;
	case GT_OBELISK:
		s = "Overload";
		break;
	case GT_HARVESTER:
		s = "Harvester";
		break;
#endif
*/
        case GT_RACING:
                s = "Racing";
                break;
        case GT_RACING_DM:
                s = "Racing Deathmatch";
                break;
        case GT_SPRINT:
                s = "Sprint";
                break;
        case GT_DERBY:
                s = "Demolition Derby";
                break;
        case GT_LCS:
                s = "Last Car Standing";
                break;
        case GT_ELIMINATION:
                s = "Elimination";
                break;
        case GT_TEAM_RACING:
                s = "Team Racing";
                break;
	case GT_TEAM_RACING_DM:
		s = "Team Racing Deathmatch";
		break;
	case GT_DEATHMATCH:
		s = "Deathmatch";
		break;
	case GT_SINGLE_PLAYER:
		s = "Time Trial";
		break;
// Q3Rally Code END
	default:
		s = "Unknown Gametype";
		break;
	}
	UI_DrawProportionalString( 320, y, s,
		UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
	y += PROP_HEIGHT;
		
	value = atoi( Info_ValueForKey( info, "timelimit" ) );
	if ( value ) {
		UI_DrawProportionalString( 320, y, va( "timelimit %i", value ),
			UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
		y += PROP_HEIGHT;
	}

// Q3Rally Code Start
/*
	if (cgs.gametype < GT_CTF ) {
		value = atoi( Info_ValueForKey( info, "fraglimit" ) );
		if ( value ) {
			UI_DrawProportionalString( 320, y, va( "fraglimit %i", value ),
				UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
			y += PROP_HEIGHT;
		}
	}
*/
// Q3Rally Code END

	if (cgs.gametype >= GT_CTF) {
		value = atoi( Info_ValueForKey( info, "capturelimit" ) );
		if ( value ) {
			UI_DrawProportionalString( 320, y, va( "capturelimit %i", value ),
				UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
		}
	}

// Q3Rally Code Start
	else if (isRallyRace()){
		value = atoi( Info_ValueForKey( info, "laplimit" ) );
		if ( value ) {
			UI_DrawProportionalString( 320, y, va( "laps %i", value ),
				UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
			y += PROP_HEIGHT;
		}
	}
	else {
		value = atoi( Info_ValueForKey( info, "fraglimit" ) );
		if ( value ) {
			UI_DrawProportionalString( 320, y, va( "fraglimit %i", value ),
				UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorWhite );
			y += PROP_HEIGHT;
		}
	}
// Q3Rally Code END
}

