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
=============================================================================

START SERVER MENU *****

=============================================================================
*/


#include "ui_local.h"
#include "ui_rally_frontend.h"

#define GAMESERVER_SELECT		"menu/art/maps_select"
#define GAMESERVER_SELECTED		"menu/art/maps_selected"
#define GAMESERVER_UNKNOWNMAP	"menu/art/unknownmap"
#define GAMESERVER_MISSING_MAP_SHOT "gfx/ui/q3rally_missing_map_shot"

#define MAX_MAPSPERPAGE		20

#define MAX_STATSPERPAGE	5
#define MAX_MAPSTATS		16

#define	MAX_SERVERSTEXT	8192

#define MAX_SERVERMAPS	64
#define MAX_NAMELENGTH  28
#define MAPNAMEBUFFER_SIZE 64

#define ID_GAMETYPE				10
#define ID_PICTURE				11
#define ID_PREVPAGE				15
#define ID_NEXTPAGE				16
#define ID_STARTSERVERBACK		17
#define ID_STARTSERVERNEXT		18
#define ID_LIST					19

#define STARTSERVER_FRAME_X          24
#define STARTSERVER_FRAME_Y          20
#define STARTSERVER_FRAME_WIDTH      592
#define STARTSERVER_FRAME_HEIGHT     440
#define STARTSERVER_FILTER_X         40
#define STARTSERVER_FILTER_Y         88
#define STARTSERVER_FILTER_WIDTH     280
#define STARTSERVER_FILTER_HEIGHT    24
#define STARTSERVER_LIST_X           40
#define STARTSERVER_LIST_Y           154
#define STARTSERVER_LIST_WIDTH       260
#define STARTSERVER_LIST_HEIGHT      244
#define STARTSERVER_ROW_X            48
#define STARTSERVER_ROW_HEIGHT       24
#define STARTSERVER_ROW_GAP          3
#define STARTSERVER_VISIBLE_ROWS     8
#define STARTSERVER_DETAIL_X         320
#define STARTSERVER_DETAIL_Y         88
#define STARTSERVER_DETAIL_WIDTH     256
#define STARTSERVER_DETAIL_HEIGHT    304
#define STARTSERVER_ACTION_Y         420
#define STARTSERVER_ACTION_WIDTH     112
#define STARTSERVER_ACTION_HEIGHT    24

static vec4_t startServerTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t startServerMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t startServerAccentColor = UI_FRONTEND_COLOR_ACCENT;


typedef struct {
	menuframework_s	menu;

	menutext_s		banner;
	menulist_s		gametype;
	menutext_s		back;
	menutext_s		next;
	menubitmap_s	mappic;
	menutext_s		mapname;
	menulist_s		list;
	menulist_s		statlist;
	menubitmap_s	item_null;

	qboolean		multiplayer;
	int				currentmap;
	int				nummaps;
	int				top;
	char			maplist[MAX_SERVERMAPS][MAX_NAMELENGTH];
	char			mapinfo[MAX_SERVERMAPS][BIG_INFO_STRING];
	char			maplistname[MAX_SERVERMAPS][MAX_NAMELENGTH];
	char			mapstats[MAX_MAPSTATS][128];
	char*			items[MAX_SERVERMAPS];
	char*			statitems[MAX_SERVERMAPS];
	int				numstats;
	int				mapGamebits[MAX_SERVERMAPS];
        } startserver_t;

static startserver_t s_startserver;

static void StartServer_MenuEvent( void *ptr, int event );
static void StartServer_Update( void );
static const char *gametype_items[15];

static int StartServer_CvarIntOrDefault( const char *name, int defaultValue ) {
	char value[32];
	trap_Cvar_VariableStringBuffer( name, value, sizeof( value ) );
	return value[0] ? atoi( value ) : defaultValue;
}

static qboolean StartServer_IsKothSelection( void );

static void StartServer_FitText( char *out, int outSize, const char *text,
                                 int maxWidth ) {
        int len;

        Q_strncpyz( out, text ? text : "", outSize );
        if ( Frontend_TextWidth( out, UI_SMALLFONT ) <= maxWidth ) {
                return;
        }

        len = (int)strlen( out );
        while ( len > 3 && Frontend_TextWidth( out, UI_SMALLFONT ) > maxWidth ) {
                len--;
                out[len] = '\0';
        }
        if ( len >= 3 ) {
                out[len - 3] = '.';
                out[len - 2] = '.';
                out[len - 1] = '.';
        }
}

static void StartServer_DrawAction( void *self ) {
        menutext_s *button;
        qboolean focus;
        qboolean disabled;
        vec4_t disabledColor;

        button = (menutext_s *)self;
        focus = ( Menu_ItemAtCursor( button->generic.parent ) == button );
        disabled = ( button->generic.flags & QMF_GRAYED ) ? qtrue : qfalse;
        if ( disabled ) {
                Vector4Copy( startServerMutedColor, disabledColor );
                disabledColor[3] = 0.35f;
                Frontend_DrawText( ( button->generic.left + button->generic.right ) / 2,
                        button->generic.top + 4, button->string,
                        UI_CENTER | UI_SMALLFONT, disabledColor );
                return;
        }

        Frontend_DrawButton( button->generic.left, button->generic.top,
                button->generic.right - button->generic.left,
                button->generic.bottom - button->generic.top,
                button->string, 1.0f, focus, UI_FRONTEND_TEXT_CENTER );
}

static void StartServer_DrawGametype( void *self ) {
        menulist_s *list;
        qboolean focus;
        char label[64];

        list = (menulist_s *)self;
        focus = ( Menu_ItemAtCursor( list->generic.parent ) == list );
        Com_sprintf( label, sizeof( label ), "Game type  %s",
                gametype_items[list->curvalue] );
        Frontend_DrawButton( list->generic.left, list->generic.top,
                list->generic.right - list->generic.left,
                list->generic.bottom - list->generic.top,
                label, 1.0f, focus, UI_FRONTEND_TEXT_LEFT );
}

static void StartServer_DrawMapList( void *self ) {
        menulist_s *list;
        int i;

        list = (menulist_s *)self;
        if ( !s_startserver.nummaps ) {
                Frontend_DrawText( STARTSERVER_ROW_X,
                        STARTSERVER_LIST_Y + 16,
                        StartServer_IsKothSelection() ?
                                "No KOTH-ready maps; add a hill zone to a map." : "No maps found",
                        UI_LEFT | UI_SMALLFONT, startServerMutedColor );
                return;
        }

        for ( i = 0; i < STARTSERVER_VISIBLE_ROWS; i++ ) {
                int index;
                int y;
                char name[64];

                index = list->top + i;
                if ( index < 0 || index >= s_startserver.nummaps ) {
                        break;
                }
                y = STARTSERVER_LIST_Y + i *
                        ( STARTSERVER_ROW_HEIGHT + STARTSERVER_ROW_GAP );
                StartServer_FitText( name, sizeof( name ),
                        s_startserver.maplistname[index], 220 );
                Frontend_DrawNavButton( STARTSERVER_ROW_X, y,
                        STARTSERVER_LIST_WIDTH - 16, STARTSERVER_ROW_HEIGHT,
                        name, 1.0f, index == list->curvalue,
                        UI_FRONTEND_TEXT_LEFT );
        }
}

static sfxHandle_t StartServer_MenuKey( int key ) {
        int row;
        int index;

        if ( key == K_MOUSE1 &&
             uis.cursorx >= STARTSERVER_ROW_X &&
             uis.cursorx <= STARTSERVER_ROW_X + STARTSERVER_LIST_WIDTH - 16 &&
             uis.cursory >= STARTSERVER_LIST_Y &&
             uis.cursory < STARTSERVER_LIST_Y + STARTSERVER_VISIBLE_ROWS *
                 ( STARTSERVER_ROW_HEIGHT + STARTSERVER_ROW_GAP ) ) {
                row = ( uis.cursory - STARTSERVER_LIST_Y ) /
                        ( STARTSERVER_ROW_HEIGHT + STARTSERVER_ROW_GAP );
                if ( uis.cursory >= STARTSERVER_LIST_Y + row *
                        ( STARTSERVER_ROW_HEIGHT + STARTSERVER_ROW_GAP ) +
                        STARTSERVER_ROW_HEIGHT ) {
                        return menu_null_sound;
                }

                index = s_startserver.list.top + row;
                if ( index >= 0 && index < s_startserver.nummaps ) {
                        s_startserver.list.oldvalue =
                                s_startserver.list.curvalue;
                        s_startserver.list.curvalue = index;
                        s_startserver.currentmap = index;
                        StartServer_Update();
                        return s_startserver.list.oldvalue == index ?
                                menu_null_sound : menu_move_sound;
                }
                return menu_null_sound;
        }

        return Menu_DefaultKey( &s_startserver.menu, key );
}

static void StartServer_Draw( void ) {
        vec4_t scrimColor = UI_FRONTEND_COLOR_SCRIM;
        char imageName[64];
        qhandle_t mapShader;
        int i;

        Frontend_DrawBackground( scrimColor );
        Frontend_DrawPanel( STARTSERVER_FRAME_X, STARTSERVER_FRAME_Y,
                STARTSERVER_FRAME_WIDTH, STARTSERVER_FRAME_HEIGHT, 1.0f,
                UI_FRONTEND_STYLE_FRAME );
        Frontend_DrawText( STARTSERVER_FRAME_X + 24, STARTSERVER_FRAME_Y + 24,
                "Create server", UI_LEFT | UI_BIGFONT, startServerTextColor );
        Frontend_DrawText( STARTSERVER_FRAME_X + 24, STARTSERVER_FRAME_Y + 48,
                "Choose a game type and track", UI_LEFT | UI_SMALLFONT,
                startServerMutedColor );
        Frontend_DrawStatusChip( STARTSERVER_FRAME_X + STARTSERVER_FRAME_WIDTH - 96,
                STARTSERVER_FRAME_Y + 26, "Host", startServerAccentColor, 1.0f );

        Frontend_DrawCard( STARTSERVER_LIST_X, STARTSERVER_LIST_Y - 22,
                STARTSERVER_LIST_WIDTH, STARTSERVER_LIST_HEIGHT + 22,
                1.0f, qfalse );
        Frontend_DrawCard( STARTSERVER_DETAIL_X, STARTSERVER_DETAIL_Y,
                STARTSERVER_DETAIL_WIDTH, STARTSERVER_DETAIL_HEIGHT,
                1.0f, qfalse );
        Frontend_DrawText( STARTSERVER_LIST_X + 16, STARTSERVER_LIST_Y - 20,
                "Available tracks", UI_LEFT | UI_SMALLFONT,
                startServerMutedColor );
        Frontend_DrawText( STARTSERVER_DETAIL_X + 16, STARTSERVER_DETAIL_Y + 18,
                "Track preview", UI_LEFT | UI_SMALLFONT,
                startServerMutedColor );

        Menu_Draw( &s_startserver.menu );

        if ( s_startserver.nummaps > 0 && s_startserver.currentmap >= 0 &&
             s_startserver.currentmap < s_startserver.nummaps ) {
                Com_sprintf( imageName, sizeof( imageName ), "levelshots/%s",
                        s_startserver.maplist[s_startserver.currentmap] );
                mapShader = trap_R_RegisterShaderNoMip( imageName );
                if ( !mapShader ) {
                        mapShader = trap_R_RegisterShaderNoMip(
                                GAMESERVER_MISSING_MAP_SHOT );
                }
                if ( mapShader ) {
                        UI_DrawHandlePic( STARTSERVER_DETAIL_X + 16,
                                STARTSERVER_DETAIL_Y + 42, 224, 126,
                                mapShader );
                }
                StartServer_FitText( imageName, sizeof( imageName ),
                        s_startserver.maplistname[s_startserver.currentmap],
                        STARTSERVER_DETAIL_WIDTH - 32 );
                Frontend_DrawText( STARTSERVER_DETAIL_X + 16,
                        STARTSERVER_DETAIL_Y + 188, imageName,
                        UI_LEFT | UI_BIGFONT, startServerTextColor );
                for ( i = 0; i < s_startserver.numstats && i < 5; i++ ) {
                Frontend_DrawText( STARTSERVER_DETAIL_X + 16,
                                STARTSERVER_DETAIL_Y + 216 + i * 18,
                                s_startserver.statitems[i],
                                UI_LEFT | UI_SMALLFONT, startServerMutedColor );
                }
        } else {
                Frontend_DrawText( STARTSERVER_DETAIL_X + 16,
                        STARTSERVER_DETAIL_Y + 80, "No track available",
                        UI_LEFT | UI_SMALLFONT, startServerMutedColor );
        }

        Frontend_DrawText( STARTSERVER_FRAME_X + 24,
                STARTSERVER_FRAME_Y + 384,
                "Select a track to continue", UI_LEFT | UI_SMALLFONT,
                startServerMutedColor );
        Frontend_DrawText( STARTSERVER_FRAME_X + STARTSERVER_FRAME_WIDTH - 24,
                STARTSERVER_FRAME_Y + 384, "Enter next   Esc back",
                UI_RIGHT | UI_SMALLFONT, startServerMutedColor );
}

static const char *gametype_items[] = {

    "Racing",
    "Racing Deathmatch",
    "Sprint",
    "Demolition Derby",
    "Last Car Standing",
    "Elimination",
	"Deathmatch",
	"Team Deathmatch",
	"Team Racing",
	"Team Racing Deathmatch",
	"Capture the Flag",
	"4-Team CTF",
	"Domination",
    "King of the Hill",
	0
};

// gametype_items[gametype_remap2[s_serveroptions.gametype]]
// gametype_remap maps display-list index -> raw GT_ value (used when starting server)
// gametype_remap2 maps raw GT_ value -> gametype_items display index
// Team gametypes start at GT_TEAM=16; indices 8-15 are unused gaps filled with 0.
static int gametype_remap[] = {GT_RACING, GT_RACING_DM, GT_SPRINT, GT_DERBY, GT_LCS, GT_ELIMINATION, GT_DEATHMATCH, GT_TEAM, GT_TEAM_RACING, GT_TEAM_RACING_DM, GT_CTF, GT_CTF4, GT_DOMINATION, GT_KOTH};
static int gametype_remap2[] = {
	0,  // GT_RACING          = 0
	1,  // GT_RACING_DM       = 1
	2,  // GT_SINGLE_PLAYER   = 2  (reused as Sprint display)
	3,  // GT_DERBY           = 3
	4,  // GT_LCS             = 4
	5,  // GT_ELIMINATION     = 5
	6,  // GT_DEATHMATCH      = 6
	2,  // GT_SPRINT          = 7
	0,  // 8  (unused)
	0,  // 9  (unused)
	0,  // 10 (unused)
	0,  // 11 (unused)
	0,  // 12 (unused)
	0,  // 13 (unused)
	0,  // 14 (unused)
	0,  // 15 (unused)
	7,  // GT_TEAM            = 16
	8,  // GT_TEAM_RACING     = 17
	9,  // GT_TEAM_RACING_DM  = 18
	10, // GT_CTF             = 19
	11, // GT_CTF4            = 20
	12, // GT_DOMINATION      = 21
	13, // GT_KOTH            = 22
};



int		allowLength[3];
int		reversable;
static int	trackLengthValueByUiIndex[3];
static int	trackLengthUiCount;

static void UI_ServerOptionsMenu( qboolean multiplayer );
static void ServerOptions_InitBotNames( void );

char *UI_GetStatKey(int num){
	switch(num){
	case MS_NUMSTARTS:
		return "starts";

	case MS_LAPTIME:
		return "laptime";

	case MS_NUMCHECKPOINTS:
		return "checkpoints";

	case MS_NUMOBSERVERSPOTS:
		return "observerspots";

	case MS_NUMWEAPONS:
		return "weapons";

	case MS_NUMPOWERUPS:
		return "powerups";

	case MS_REVERSABLE:
		return "reversable";

	case MS_TRACKLENGTHS:
		return "tracklengths";
    
    case MS_BOTSUPPORT:
        return "botsupport";
        
    case MS_NUMTEAMS:
        return "teams";

	default:
		return "";
	}
}

char *UI_GetStatName(int num){
	switch(num){
	case MS_NUMSTARTS:
		return "Start positions:";

	case MS_LAPTIME:
		return "Laptime:";

	case MS_NUMCHECKPOINTS:
		return "Checkpoints:";

	case MS_NUMOBSERVERSPOTS:
		return "Observer spots:";

	case MS_NUMWEAPONS:
		return "Weapons:";

	case MS_NUMPOWERUPS:
		return "Powerups:";

	case MS_REVERSABLE:
		return "Reversable:";

	case MS_TRACKLENGTHS:
		return "Track lengths available:";
    
    case MS_BOTSUPPORT:
        return "Bot Support:";
        
    case MS_NUMTEAMS:
        return "Teams:";

	default:
		return "";
	}
}


char *UI_GetDefaultStatValue( int num ){
	switch(num){
	case MS_NUMSTARTS:
		return "Unknown";

	case MS_LAPTIME:
		return "Unknown";

	case MS_NUMCHECKPOINTS:
		return "Unknown";

	case MS_NUMOBSERVERSPOTS:
		return "Unknown";

	case MS_NUMWEAPONS:
		return "Unknown";

	case MS_NUMPOWERUPS:
		return "Unknown";

	case MS_REVERSABLE:
		return "0";

	case MS_TRACKLENGTHS:
		return "0 1 2";
        
    case MS_BOTSUPPORT:
        return "Unknown";
        
    case MS_NUMTEAMS:
        return "Unknown";

	default:
		return "";
	}
}


char *UI_GetStatValue( const char *info, int num ){
	char	*s;
	char	*result;

	s = UI_GetStatKey( num );

	result = Info_ValueForKey( info, s );

	if ( !result || !strcmp(result, "")){
		result = UI_GetDefaultStatValue( num );
	}

	// process numbers into valid strings
	switch ( num ){
	case MS_REVERSABLE:
		if ( !strcmp(result, "0") ){
			reversable = 0;
			result = "No";
		}
		else if ( !strcmp(result, "1") ){
			reversable = 1;
			result = "Yes";
		}
		break;

	case MS_TRACKLENGTHS:
		allowLength[0] = allowLength[1] = allowLength[2] = 1;

		if ( !strchr(result, '0') )
			allowLength[0] = 0;
		if ( !strchr(result, '1') )
			allowLength[1] = 0;
		if ( !strchr(result, '2') )
			allowLength[2] = 0;

		if ( allowLength[0] && allowLength[1] && allowLength[2] )
			result = "All";
		else if ( !allowLength[0] && allowLength[1] && allowLength[2] )
			result = "Medium, Long";
		else if ( !allowLength[0] && !allowLength[1] && allowLength[2] )
			result = "Long";
		else if ( allowLength[0] && !allowLength[1] && allowLength[2] )
			result = "Short, Long";
		else if ( allowLength[0] && !allowLength[1] && !allowLength[2] )
			result = "Short";
		else if ( !allowLength[0] && allowLength[1] && !allowLength[2] )
			result = "Medium";
		else if ( allowLength[0] && allowLength[1] && !allowLength[2] )
			result = "Short, Medium";
		else
			result = "All";

		break;
	}

	return result;
}

static void UI_SetupMapStatsForArena( int arena ){
	int				i;
	const char		*info;
	char			*s;

	if (arena < 0 || arena >= s_startserver.nummaps){
		for (i = 0; i < MAX_MAPSTATS; i++){
			Q_strncpyz(s_startserver.mapstats[i], "", 128);
		}
		return;
	}

	//info = UI_GetArenaInfoByNumber( arena );
	info = s_startserver.mapinfo[arena];

	s_startserver.numstats = 0;
	for (i = 0; i < MAX_MAPSTATS; i++){
		s = UI_GetStatValue(info, i);

		if (!s || !strcmp(s, ""))
			continue;

		Com_sprintf(s_startserver.mapstats[s_startserver.numstats], 128, "%30s %s", UI_GetStatName(i), s);
		s_startserver.numstats++;
	}

	for (i = s_startserver.numstats; i < MAX_MAPSTATS; i++){
		Q_strncpyz(s_startserver.mapstats[i], "", 128);
	}

	s_startserver.statlist.numitems = s_startserver.numstats;
	for( i = 0; i < s_startserver.numstats; i++ ) {
		s_startserver.statitems[i] = s_startserver.mapstats[i];
	}
}
// END

static const struct {
        const char* name;
        int bit;
} gametype_bitnames[] = {
        { "q3r_racing", GT_RACING },
        { "q3r_racing_dm", GT_RACING_DM },
        { "q3r_sprint", GT_SPRINT },
        { "q3r_derby", GT_DERBY },
        { "q3r_lcs", GT_LCS },
        { "q3r_elimination", GT_ELIMINATION },
        { "q3r_dm", GT_DEATHMATCH },
        { "q3r_single", GT_SINGLE_PLAYER },
        { "q3r_team_racing", GT_TEAM_RACING },
        { "q3r_team_racing_dm", GT_TEAM_RACING_DM },
        { "q3r_team_dm", GT_TEAM },
        { "q3r_ctf", GT_CTF },
        { "q3r_ctf4", GT_CTF4 },
        { "q3r_dom", GT_DOMINATION },
        { "q3r_koth", GT_KOTH }, /* Q3Rally KOTH */
};

/*
=================
GametypeBits
=================
*/
static int GametypeBits( char *string ) {
        int             bits;
        char    *p;
        char    *token;
        int             i;

        bits = 0;
        p = string;
        while( 1 ) {
                token = COM_ParseExt( &p, qfalse );
                if( token[0] == 0 ) {
                        break;
                }

                for( i = 0; i < sizeof( gametype_bitnames ) / sizeof( gametype_bitnames[0] ); i++ ) {
                        if( Q_stricmp( token, gametype_bitnames[i].name ) == 0 ) {
                                bits |= 1 << gametype_bitnames[i].bit;
                                break;
                        }
                }
        }
        return bits;
}

static qboolean ServerOptions_IsRacingGametype( int gametype ) {
		switch ( gametype ) {
		case GT_RACING:
		case GT_RACING_DM:
		case GT_SPRINT:
		case GT_TEAM_RACING:
		case GT_TEAM_RACING_DM:
		case GT_SINGLE_PLAYER:
			return qtrue;
		default:
			return qfalse;
		}
}


/*
=================
StartServer_Update
=================
*/
static void StartServer_Update( void ) {

	static	char	picname[64];

	Com_sprintf( picname, sizeof(picname), "levelshots/%s", s_startserver.maplist[s_startserver.list.curvalue] );
	s_startserver.mappic.generic.name   = picname;
	s_startserver.mappic.shader         = 0;

	// no servers to start
	if( !s_startserver.nummaps ) {

		// set the map name
Q_strncpyz( s_startserver.mapname.string, "NO MAPS FOUND", MAPNAMEBUFFER_SIZE );

UI_SetupMapStatsForArena(-1);

	}
	else {

		// set the map name
Q_strncpyz( s_startserver.mapname.string, s_startserver.maplist[s_startserver.currentmap], MAPNAMEBUFFER_SIZE );

UI_SetupMapStatsForArena(s_startserver.currentmap);
	}

	Q_strupr( s_startserver.mapname.string );
}


/*
=================
StartServer_MapEvent
=================
*/
static void StartServer_MapEvent( void* ptr, int event ) {

	int id = ((menucommon_s*)ptr)->id;

	if( event != QM_ACTIVATED && id != ID_LIST ) {
		return;
	}

	switch( id ) {
	case ID_LIST:
		if( event == QM_GOTFOCUS ) {
			s_startserver.currentmap = s_startserver.list.curvalue;
			StartServer_Update();
		}
		break;
	}

	StartServer_Update();
}


/*
=================
StartServer_GametypeEvent
=================
*/
static void StartServer_GametypeEvent( void* ptr, int event ) {
	int			i;
	int			count;
	int			gamebits;
	int			matchbits;
	const char	*info;

	if( event != QM_ACTIVATED) {
		return;
	}

	count = UI_GetNumArenas();
	s_startserver.nummaps = 0;
	matchbits = 1 << gametype_remap[s_startserver.gametype.curvalue];

        for( i = 0; i < count; i++ ) {
                info = UI_GetArenaInfoByNumber( i );

                gamebits = GametypeBits( Info_ValueForKey( info, "type") );
                if( !( gamebits & matchbits ) ) {
                        continue;
                }

                if( s_startserver.nummaps >= MAX_SERVERMAPS ) {
                        break;
                }

                Q_strncpyz(s_startserver.mapinfo[s_startserver.nummaps], info, sizeof(s_startserver.mapinfo[i]));

                Q_strncpyz( s_startserver.maplist[s_startserver.nummaps], Info_ValueForKey( info, "map"), MAX_NAMELENGTH );

                Q_strncpyz( s_startserver.maplistname[s_startserver.nummaps], Info_ValueForKey( info, "longname"), MAX_NAMELENGTH );
                if (s_startserver.maplistname[s_startserver.nummaps][0] == 0)
                        Q_strncpyz( s_startserver.maplistname[s_startserver.nummaps], s_startserver.maplist[s_startserver.nummaps], MAX_NAMELENGTH );
                else
                        Q_strupr( s_startserver.maplistname[s_startserver.nummaps] );

                s_startserver.mapGamebits[s_startserver.nummaps] = gamebits;
                s_startserver.nummaps++;
        }

	s_startserver.currentmap = 0;
	s_startserver.top = 0;
	s_startserver.list.top = s_startserver.top;
	s_startserver.list.curvalue = s_startserver.currentmap;
	s_startserver.list.numitems = s_startserver.nummaps;

	StartServer_Update();
}


/*
=================
StartServer_MenuEvent
=================
*/
static void StartServer_MenuEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {

	case ID_STARTSERVERNEXT:
		trap_Cvar_SetValue( "g_gameType", gametype_remap[s_startserver.gametype.curvalue] );
		UI_ServerOptionsMenu( s_startserver.multiplayer );
		break;

	case ID_STARTSERVERBACK:
		UI_PopMenu();
		break;
	}
}


/*
=========================
StartServer_LevelshotDraw
=========================
*/
static void StartServer_LevelshotDraw(void *self) {
    menubitmap_s *b;
    char *s;
    char *info;
    char author[MAX_QPATH];
    int x, y, w, h;
    int image_offset = 10;
    int bottom_margin = 10;

    b = (menubitmap_s *)self;

    if (!b->generic.name) {
        return;
    }

    // Register the shader if not already loaded
    if (b->generic.name && !b->shader) {
        b->shader = trap_R_RegisterShaderNoMip(b->generic.name);
        if (!b->shader && b->errorpic) {
            b->shader = trap_R_RegisterShaderNoMip(b->errorpic);
        }
    }

    // Set fixed width for background rectangle
    w = 256;
    x = b->generic.x - w / 2;
    y = b->generic.y;
    UI_FillRect(x, y, w, 140, menu_back_color);

    // Abort if the selected map index is invalid
    if (s_startserver.list.curvalue < 0 || 
        s_startserver.list.curvalue >= s_startserver.nummaps) {
        return;
    }

    // Draw map preview image (levelshot)
    x = b->generic.x - b->width / 2;
    y = b->generic.y + image_offset;  // Shift image down by 10px
    w = b->width;
    h = b->height;

    if (b->shader) {
        UI_DrawHandlePic(x, y, w, h, b->shader);
    }

    // Center text relative to image
    x += b->width / 2;
    y += h + 4;  // Space between image and text

    // Draw map name
    UI_DrawString(
        x,
        y,
        s_startserver.maplistname[s_startserver.list.curvalue],
        UI_CENTER | UI_SMALLFONT,
        text_color_normal
    );

    // Get author info
    info = s_startserver.mapinfo[s_startserver.currentmap];
    s = Info_ValueForKey(info, "author");

    if (!s || !strcmp(s, "")) {
        Com_sprintf(author, sizeof(author), "Author: Unknown");
    } else {
        Com_sprintf(author, sizeof(author), "Author: %s", s);
    }

    y += SMALLCHAR_HEIGHT;

    // Calculate available space and ensure bottom alignment
    if ((y + SMALLCHAR_HEIGHT) > (b->generic.y + h + image_offset + 140 - bottom_margin)) {
        y = b->generic.y + h + image_offset + 140 - SMALLCHAR_HEIGHT - bottom_margin;
    }

    // Draw author name
    UI_DrawString(
        x,
        y,
        author,
        UI_CENTER | UI_SMALLFONT,
        text_color_normal
    );

    // If highlighted, draw focus border
    x = b->generic.x;
    y = b->generic.y;
    w = b->width;
    h = b->height + 28;

    if (b->generic.flags & QMF_HIGHLIGHT) {
        UI_DrawHandlePic(x, y, w, h, b->focusshader);
    }
}

/*
=================
StartServer_MenuInit
=================
*/
static void StartServer_MenuInit( void ) {
	
    int	i;
static char mapnamebuffer[MAPNAMEBUFFER_SIZE];

	// zero set all our globals
	memset( &s_startserver, 0 ,sizeof(startserver_t) );

	StartServer_Cache();

	s_startserver.menu.wrapAround = qtrue;
	s_startserver.menu.fullscreen = qtrue;
	s_startserver.menu.draw = StartServer_Draw;
	s_startserver.menu.key = StartServer_MenuKey;

	s_startserver.banner.generic.type  = MTYPE_BTEXT;
	s_startserver.banner.generic.x	   = 320;
	s_startserver.banner.generic.y	   = 16;
	s_startserver.banner.string        = "MAP SELECT";
	s_startserver.banner.color         = text_color_normal;
	s_startserver.banner.style         = UI_CENTER;

	s_startserver.gametype.generic.type		= MTYPE_SPINCONTROL;
	s_startserver.gametype.generic.name		= "Game Type:";
	s_startserver.gametype.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_startserver.gametype.generic.callback	= StartServer_GametypeEvent;
	s_startserver.gametype.generic.id		= ID_GAMETYPE;
	s_startserver.gametype.generic.x		= 320;
	s_startserver.gametype.generic.y		= 480 - 50;
	s_startserver.gametype.itemnames		= gametype_items;

	s_startserver.list.generic.type			= MTYPE_LISTBOX;
	s_startserver.list.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_startserver.list.generic.id			= ID_LIST;
	s_startserver.list.scrollbarAlignment	= SB_RIGHT;
	s_startserver.list.generic.callback		= StartServer_MapEvent;
	s_startserver.list.generic.x			= 30;
	s_startserver.list.generic.y			= 80;
	s_startserver.list.width				= 40;
	s_startserver.list.height				= MAX_MAPSPERPAGE;
	s_startserver.list.itemnames			= (const char **)s_startserver.items;
	s_startserver.list.numitems				= s_startserver.nummaps;
	                for( i = 0; i < s_startserver.nummaps; i++ ) {
		s_startserver.items[i] = s_startserver.maplistname[i];
	}

	s_startserver.statlist.generic.type			= MTYPE_LISTBOX;
	s_startserver.statlist.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS | QMF_SCROLL_ONLY | QMF_MOUSEONLY;
	s_startserver.statlist.scrollbarAlignment	= SB_RIGHT | SB_HIDE;
	s_startserver.statlist.generic.x			= 290;
	s_startserver.statlist.generic.y			= 272;
	s_startserver.statlist.width				= 52;
	s_startserver.statlist.height				= 8;
	s_startserver.statlist.itemnames			= (const char **)s_startserver.statitems;
	s_startserver.statlist.numitems				= s_startserver.numstats;
	for( i = 0; i < s_startserver.numstats; i++ ) {
		s_startserver.statitems[i] = s_startserver.mapstats[i];
	}

	s_startserver.mappic.generic.type		= MTYPE_BITMAP;
	s_startserver.mappic.generic.flags		= QMF_CENTER_JUSTIFY|QMF_INACTIVE;
	s_startserver.mappic.generic.x			= 445;
	s_startserver.mappic.generic.y			= 80;
	s_startserver.mappic.generic.id			= ID_PICTURE;
	s_startserver.mappic.width  			= 170;
	s_startserver.mappic.height  			= 96;
	s_startserver.mappic.errorpic			= GAMESERVER_UNKNOWNMAP;
	s_startserver.mappic.generic.ownerdraw	= StartServer_LevelshotDraw;

	s_startserver.mapname.generic.type  = MTYPE_PTEXT;
	s_startserver.mapname.generic.flags = QMF_CENTER_JUSTIFY|QMF_INACTIVE;
	s_startserver.mapname.generic.x	    = 455;
	s_startserver.mapname.generic.y	    = 228;
	s_startserver.mapname.string        = mapnamebuffer;
	s_startserver.mapname.style         = UI_CENTER|UI_BIGFONT;
	s_startserver.mapname.color         = text_color_normal;

	s_startserver.back.generic.type			= MTYPE_PTEXT;
	s_startserver.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_startserver.back.generic.x			= 20;
	s_startserver.back.generic.y			= 480 - 50;
	s_startserver.back.generic.id			= ID_STARTSERVERBACK;
	s_startserver.back.generic.callback		= StartServer_MenuEvent; 
	s_startserver.back.string				= "< BACK";
	s_startserver.back.color				= text_color_normal;
	s_startserver.back.style				= UI_LEFT | UI_SMALLFONT;

	s_startserver.next.generic.type			= MTYPE_PTEXT;
	s_startserver.next.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_startserver.next.generic.x			= 640 - 20;
	s_startserver.next.generic.y			= 480 - 50;
	s_startserver.next.generic.id			= ID_STARTSERVERNEXT;
	s_startserver.next.generic.callback		= StartServer_MenuEvent; 
	s_startserver.next.string				= "NEXT >";
	s_startserver.next.color				= text_color_normal;
	s_startserver.next.style				= UI_RIGHT | UI_SMALLFONT;

	s_startserver.item_null.generic.type	= MTYPE_BITMAP;
	s_startserver.item_null.generic.flags	= QMF_LEFT_JUSTIFY|QMF_MOUSEONLY|QMF_SILENT;
	s_startserver.item_null.generic.x		= 0;
	s_startserver.item_null.generic.y		= 0;
	s_startserver.item_null.width			= 640;
	s_startserver.item_null.height			= 480;

	/* Keep the legacy data widgets for navigation and callbacks, but render
	 * them through the shared frontend components. */
	s_startserver.banner.generic.flags = QMF_INACTIVE | QMF_HIDDEN;

	s_startserver.gametype.generic.x = STARTSERVER_FILTER_X +
		STARTSERVER_FILTER_WIDTH / 2;
	s_startserver.gametype.generic.y = STARTSERVER_FILTER_Y + 12;
	s_startserver.gametype.generic.left = STARTSERVER_FILTER_X;
	s_startserver.gametype.generic.top = STARTSERVER_FILTER_Y;
	s_startserver.gametype.generic.right = STARTSERVER_FILTER_X +
		STARTSERVER_FILTER_WIDTH;
	s_startserver.gametype.generic.bottom = STARTSERVER_FILTER_Y +
		STARTSERVER_FILTER_HEIGHT;
	s_startserver.gametype.generic.flags |= QMF_NODEFAULTINIT;
	s_startserver.gametype.numitems = ARRAY_LEN( gametype_items ) - 1;
	s_startserver.gametype.generic.ownerdraw = StartServer_DrawGametype;

	s_startserver.list.generic.x = STARTSERVER_ROW_X +
		( STARTSERVER_LIST_WIDTH - 16 ) / 2;
	s_startserver.list.generic.y = STARTSERVER_LIST_Y;
	s_startserver.list.generic.left = STARTSERVER_ROW_X;
	s_startserver.list.generic.top = STARTSERVER_LIST_Y;
	s_startserver.list.generic.right = STARTSERVER_ROW_X +
		STARTSERVER_LIST_WIDTH - 16;
	s_startserver.list.generic.bottom = STARTSERVER_LIST_Y +
		STARTSERVER_VISIBLE_ROWS * ( STARTSERVER_ROW_HEIGHT + STARTSERVER_ROW_GAP );
	s_startserver.list.generic.flags |= QMF_NODEFAULTINIT;
	s_startserver.list.generic.ownerdraw = StartServer_DrawMapList;
	s_startserver.list.height = STARTSERVER_VISIBLE_ROWS;

	s_startserver.mappic.generic.flags |= QMF_HIDDEN;
	s_startserver.mapname.generic.flags |= QMF_HIDDEN;
	s_startserver.statlist.generic.flags |= QMF_HIDDEN;
	s_startserver.item_null.generic.flags |= QMF_HIDDEN;

	s_startserver.back.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
	s_startserver.back.generic.x = 96;
	s_startserver.back.generic.y = STARTSERVER_ACTION_Y +
		STARTSERVER_ACTION_HEIGHT / 2;
	s_startserver.back.generic.left = 40;
	s_startserver.back.generic.top = STARTSERVER_ACTION_Y;
	s_startserver.back.generic.right = 40 + STARTSERVER_ACTION_WIDTH;
	s_startserver.back.generic.bottom = STARTSERVER_ACTION_Y +
		STARTSERVER_ACTION_HEIGHT;
	s_startserver.back.generic.flags |= QMF_NODEFAULTINIT;
	s_startserver.back.generic.ownerdraw = StartServer_DrawAction;
	s_startserver.back.string = "Back";
	s_startserver.back.style = UI_CENTER | UI_SMALLFONT;

	s_startserver.next.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
	s_startserver.next.generic.x = 544;
	s_startserver.next.generic.y = STARTSERVER_ACTION_Y +
		STARTSERVER_ACTION_HEIGHT / 2;
	s_startserver.next.generic.left = 488;
	s_startserver.next.generic.top = STARTSERVER_ACTION_Y;
	s_startserver.next.generic.right = 488 + STARTSERVER_ACTION_WIDTH;
	s_startserver.next.generic.bottom = STARTSERVER_ACTION_Y +
		STARTSERVER_ACTION_HEIGHT;
	s_startserver.next.generic.flags |= QMF_NODEFAULTINIT;
	s_startserver.next.generic.ownerdraw = StartServer_DrawAction;
	s_startserver.next.string = "Next";
	s_startserver.next.style = UI_CENTER | UI_SMALLFONT;

	Menu_AddItem( &s_startserver.menu, &s_startserver.banner );
	Menu_AddItem( &s_startserver.menu, &s_startserver.list );
	Menu_AddItem( &s_startserver.menu, &s_startserver.statlist );
	Menu_AddItem( &s_startserver.menu, &s_startserver.back );
	Menu_AddItem( &s_startserver.menu, &s_startserver.gametype );
	Menu_AddItem( &s_startserver.menu, &s_startserver.next );
	Menu_AddItem( &s_startserver.menu, &s_startserver.mappic );
	Menu_AddItem( &s_startserver.menu, &s_startserver.mapname );
	Menu_AddItem( &s_startserver.menu, &s_startserver.item_null );

	StartServer_GametypeEvent( NULL, QM_ACTIVATED );
}


/*
=================
StartServer_Cache
=================
*/
void StartServer_Cache( void )
{
	int				i;
	const char		*info;
	qboolean		precache;
	char			picname[64];

	trap_R_RegisterShaderNoMip( GAMESERVER_SELECT );	
	trap_R_RegisterShaderNoMip( GAMESERVER_SELECTED );	
	trap_R_RegisterShaderNoMip( GAMESERVER_UNKNOWNMAP );
	trap_R_RegisterShaderNoMip( GAMESERVER_MISSING_MAP_SHOT );

	precache = trap_Cvar_VariableValue("com_buildscript");

        s_startserver.nummaps = UI_GetNumArenas();
        if( s_startserver.nummaps > MAX_SERVERMAPS ) {
                s_startserver.nummaps = MAX_SERVERMAPS;
        }

        for( i = 0; i < s_startserver.nummaps; i++ ) {
		info = UI_GetArenaInfoByNumber( i );

		Q_strncpyz( s_startserver.maplist[i], Info_ValueForKey( info, "map"), MAX_NAMELENGTH );

		Q_strncpyz( s_startserver.maplistname[i], Info_ValueForKey( info, "longname"), MAX_NAMELENGTH );
		if (s_startserver.maplistname[i][0] == 0)
			Q_strncpyz( s_startserver.maplistname[i], s_startserver.maplist[i], MAX_NAMELENGTH );
		else
			Q_strupr( s_startserver.maplistname[i] );

		s_startserver.mapGamebits[i] = GametypeBits( Info_ValueForKey( info, "type") );

		if( precache ) {
			Com_sprintf( picname, sizeof(picname), "levelshots/%s", s_startserver.maplist[i] );
			trap_R_RegisterShaderNoMip(picname);
			
		}
	}

}


/*
=================
UI_StartServerMenu
=================
*/
void UI_StartServerMenu( qboolean multiplayer ) {
	StartServer_MenuInit();
	s_startserver.multiplayer = multiplayer;
	UI_PushMenu( &s_startserver.menu );
}



/*
=============================================================================

SERVER OPTIONS MENU *****

=============================================================================
*/

#define ID_PLAYER_TYPE			10
#define ID_MAXCLIENTS			11
#define ID_DEDICATED			12
#define ID_GO					13
#define ID_BACK					14
#define ID_TRACK_LENGTH			15
#define ID_TRACK_REVERSED		16
#define ID_GHOST_ONLY		17
#define PLAYER_SLOTS			12

/* Modern host-options surface. The original menu items remain the source of
 * truth for cvars and events; these coordinates only define their new visual
 * presentation and mouse hitboxes. */
#define SERVEROPT_FRAME_X             24
#define SERVEROPT_FRAME_Y             20
#define SERVEROPT_FRAME_WIDTH         592
#define SERVEROPT_FRAME_HEIGHT        440
#define SERVEROPT_PLAYER_X            40
#define SERVEROPT_PLAYER_Y            112
#define SERVEROPT_PLAYER_WIDTH        248
#define SERVEROPT_PLAYER_HEIGHT       286
#define SERVEROPT_PLAYER_ROW_X        40
#define SERVEROPT_PLAYER_ROW_Y        140
#define SERVEROPT_PLAYER_ROW_WIDTH    248
#define SERVEROPT_PLAYER_ROW_HEIGHT   20
#define SERVEROPT_PLAYER_ROW_GAP      1
#define SERVEROPT_OPTION_X            304
#define SERVEROPT_OPTION_Y            112
#define SERVEROPT_OPTION_WIDTH        272
#define SERVEROPT_OPTION_HEIGHT       286
#define SERVEROPT_OPTION_START_Y      212
#define SERVEROPT_OPTION_ROW_HEIGHT   19
#define SERVEROPT_OPTION_ROW_GAP      1
#define SERVEROPT_OPTION_COLUMN_WIDTH 240
#define SERVEROPT_BOT_X               40
#define SERVEROPT_BOT_Y               86
#define SERVEROPT_BOT_WIDTH           248
#define SERVEROPT_BOT_HEIGHT          24
#define SERVEROPT_ACTION_Y            420
#define SERVEROPT_ACTION_WIDTH        112
#define SERVEROPT_ACTION_HEIGHT       24

static vec4_t serverOptionsTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t serverOptionsMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t serverOptionsAccentColor = UI_FRONTEND_COLOR_ACCENT;


typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		mappic;
	menulist_s			dedicated;
	menufield_s			timelimit;
	menufield_s			fraglimit;
	menufield_s			flaglimit;
	menufield_s			kothPtsTick;
	menufield_s			kothPtsCapture;
	menufield_s			kothPtsDefend;
	menuradiobutton_s	kothOvertime;
	menufield_s			kothOvertimeHold;
	menuradiobutton_s	friendlyfire;
	menufield_s			hostname;
    menulist_s          dominationSpawnStyle;
	menuradiobutton_s   sigillocator;
	menufield_s			dominationScoreInterval;
	menufield_s			dominationCaptureDelay;
	menulist_s			trackLength;
	menulist_s                      reversed;
        menuradiobutton_s       pure;
	menuradiobutton_s       eliminationWeapons;
	menuradiobutton_s       ghostOnly;
	menufield_s				minPlayers;
	menulist_s			botSkill;
	menutext_s			player0;
	menulist_s			playerType[PLAYER_SLOTS];
	menutext_s			playerName[PLAYER_SLOTS];
	menulist_s			playerTeam[PLAYER_SLOTS];
	menutext_s			go;
	menutext_s			back;

	qboolean			multiplayer;
	int					gametype;
	char				mapnamebuffer[32];
	char				playerNameBuffers[PLAYER_SLOTS][16];

	int					ghostPlaybackRestore;

	qboolean			newBot;
	int					newBotIndex;
	char				newBotName[16];

} serveroptions_t;

static serveroptions_t s_serveroptions;

static const char *dedicated_list[] = {
	"No",
	"LAN",
	"Internet",
	0
};

static char *track_length_list[] = {
	"Short",
	"Medium",
	"Long",
	0
};

static const char *reversed_list[] = {
	"No",
	"Yes",
	0
};

static const char *playerType_list[] = {
	"Open",
	"Bot",
	"----",
	0
};

static const char *playerTeam_list[] = {
	"Blue",
	"Red",
	"Green",
	"Yellow",
	0
};

static qboolean StartServer_IsKothSelection( void ) {
	int index = s_startserver.gametype.curvalue;
	return ( index >= 0 && index < sizeof( gametype_remap ) / sizeof( gametype_remap[0] ) &&
		gametype_remap[index] == GT_KOTH ) ? qtrue : qfalse;
}

static const char *playerTeam_twoTeam_list[] = {
	"Blue",
	"Red",
	0
};

static const char *botSkill_list[] = {
	"I Can Win",
	"Bring It On",
	"Hurt Me Plenty",
	"Hardcore",
	"Nightmare!",
	0
};

// for dominationSpawnStyle
static const char *dtfspawn_list[] = {
  "DM Spawns",
  "CTF Team Spawns",
  0
};

static int ServerOptions_TrackLengthValueFromIndex( int listIndex ) {
	if ( trackLengthUiCount <= 0 ) {
		return 0;
	}

	if ( listIndex < 0 || listIndex >= trackLengthUiCount ) {
		return trackLengthValueByUiIndex[0];
	}

	return trackLengthValueByUiIndex[listIndex];
}

static int ServerOptions_TrackLengthIndexFromValue( int trackLengthValue ) {
	int i;

	trackLengthValue = (int)Com_Clamp( 0, 2, trackLengthValue );
	for ( i = 0; i < trackLengthUiCount; i++ ) {
		if ( trackLengthValueByUiIndex[i] == trackLengthValue ) {
			return i;
		}
	}

	return 0;
}

static qboolean ServerOptions_AllowsFourTeams( void ) {
	return ( s_serveroptions.gametype == GT_CTF4 ||
		s_serveroptions.gametype == GT_DOMINATION ) ? qtrue : qfalse;
}

static void ServerOptions_ClampPlayerTeams( void ) {
	int n;

	if ( s_serveroptions.gametype < GT_TEAM || ServerOptions_AllowsFourTeams() ) {
		return;
	}

	for ( n = 0; n < PLAYER_SLOTS; n++ ) {
		if ( s_serveroptions.playerTeam[n].curvalue > 1 ) {
			s_serveroptions.playerTeam[n].curvalue = n & 1;
		}
	}
}

static void ServerOptions_UpdatePlayerTeamLists( void ) {
	int n;
	int teamCount;
	const char **teamList;

	teamList = ServerOptions_AllowsFourTeams() ? playerTeam_list : playerTeam_twoTeam_list;
	// Menu_AddItem caches numitems before the gametype-specific list is set.
	teamCount = 0;
	while ( teamList[teamCount] ) {
		teamCount++;
	}

	for ( n = 0; n < PLAYER_SLOTS; n++ ) {
		s_serveroptions.playerTeam[n].itemnames = teamList;
		s_serveroptions.playerTeam[n].numitems = teamCount;
		if ( s_serveroptions.playerTeam[n].curvalue < 0 ||
			s_serveroptions.playerTeam[n].curvalue >= teamCount ) {
			s_serveroptions.playerTeam[n].curvalue = 0;
		}
	}

	ServerOptions_ClampPlayerTeams();
}

/*
=================
BotAlreadySelected
=================
*/
static qboolean BotAlreadySelected( const char *checkName ) {
	int		n;

	for( n = 1; n < PLAYER_SLOTS; n++ ) {
		if( s_serveroptions.playerType[n].curvalue != 1 ) {
			continue;
		}
		if( (s_serveroptions.gametype >= GT_TEAM) &&
			(s_serveroptions.playerTeam[n].curvalue != s_serveroptions.playerTeam[s_serveroptions.newBotIndex].curvalue ) ) {
			continue;
		}
		if( Q_stricmp( checkName, s_serveroptions.playerNameBuffers[n] ) == 0 ) {
			return qtrue;
		}
	}

	return qfalse;
}


/*
=================
ServerOptions_Start
=================
*/
static void ServerOptions_Start( void ) {
	int		timelimit;
	int		fraglimit;
    int     dominationSpawnStyle;
	int		dominationScoreInterval;
	int		dominationCaptureDelay;
	int     sigillocator;
	int		maxclients;
	int		dedicated;
	int		friendlyfire;
	int		flaglimit;
	int		kothPtsTick;
	int		kothPtsCapture;
	int		kothPtsDefend;
	int		kothOvertime;
	int		kothOvertimeHoldSec;
	int		pure;
	int		trackLength;
	int		reversed;
	int		eliminationWeapons;
	int		skill;
	qboolean	isRacingGametype;
	int		trackLengthValue;
	int		n;
	char	buf[64];

	timelimit	 = atoi( s_serveroptions.timelimit.field.buffer );
	fraglimit	 = ( s_serveroptions.gametype == GT_SPRINT ) ? 1 : atoi( s_serveroptions.fraglimit.field.buffer );
	flaglimit	 = atoi( s_serveroptions.flaglimit.field.buffer );
	kothPtsTick	 = atoi( s_serveroptions.kothPtsTick.field.buffer );
	kothPtsCapture = atoi( s_serveroptions.kothPtsCapture.field.buffer );
	kothPtsDefend = atoi( s_serveroptions.kothPtsDefend.field.buffer );
	kothOvertime = s_serveroptions.kothOvertime.curvalue;
	kothOvertimeHoldSec = atoi( s_serveroptions.kothOvertimeHold.field.buffer );
	dominationScoreInterval = atoi( s_serveroptions.dominationScoreInterval.field.buffer );
	dominationCaptureDelay = atoi( s_serveroptions.dominationCaptureDelay.field.buffer );
	dedicated	 = s_serveroptions.dedicated.curvalue;
	friendlyfire = s_serveroptions.friendlyfire.curvalue;
	pure		 = s_serveroptions.pure.curvalue;
	skill		 = s_serveroptions.botSkill.curvalue + 1;
    dominationSpawnStyle = s_serveroptions.dominationSpawnStyle.curvalue; // dtf
    sigillocator = s_serveroptions.sigillocator.curvalue; // dtf
	trackLength  = s_serveroptions.trackLength.curvalue;
	trackLengthValue = ServerOptions_TrackLengthValueFromIndex( trackLength );
	reversed     = s_serveroptions.reversed.curvalue;
	eliminationWeapons = s_serveroptions.eliminationWeapons.curvalue;
	isRacingGametype = ServerOptions_IsRacingGametype( s_serveroptions.gametype );

	//set maxclients
	for( n = 0, maxclients = 0; n < PLAYER_SLOTS; n++ ) {
		if( s_serveroptions.playerType[n].curvalue == 2 ) {
			continue;
		}
		if( (s_serveroptions.playerType[n].curvalue == 1) && (s_serveroptions.playerNameBuffers[n][0] == 0) ) {
			continue;
		}
		maxclients++;
	}

	switch( s_serveroptions.gametype ) {

case GT_RACING:
case GT_RACING_DM:
case GT_SPRINT:
case GT_ELIMINATION:
default:
		trap_Cvar_SetValue( "ui_racing_laplimit", fraglimit );
		trap_Cvar_SetValue( "ui_racing_timelimit", timelimit );
		break;

	case GT_TEAM_RACING:
	case GT_TEAM_RACING_DM:
		trap_Cvar_SetValue( "ui_team_racing_laplimit", fraglimit );
		trap_Cvar_SetValue( "ui_team_racing_timelimit", timelimit );
		trap_Cvar_SetValue( "ui_team_racing_friendly", friendlyfire );
		break;

	case GT_DERBY:
		trap_Cvar_SetValue( "ui_derby_timelimit", timelimit );
		trap_Cvar_SetValue( "ui_derby_minplayers", atoi( s_serveroptions.minPlayers.field.buffer ) );
		trap_Cvar_SetValue( "g_derbyMinPlayers",   atoi( s_serveroptions.minPlayers.field.buffer ) );
		break;
		
	case GT_LCS:
		trap_Cvar_SetValue( "ui_lcs_timelimit", timelimit );
		trap_Cvar_SetValue( "ui_lcs_minplayers", atoi( s_serveroptions.minPlayers.field.buffer ) );
		trap_Cvar_SetValue( "g_derbyMinPlayers",   atoi( s_serveroptions.minPlayers.field.buffer ) );
		break;

	case GT_DEATHMATCH:
		trap_Cvar_SetValue( "ui_dm_fraglimit", fraglimit );
		trap_Cvar_SetValue( "ui_dm_timelimit", timelimit );
		break;

	case GT_TEAM:
		trap_Cvar_SetValue( "ui_team_fraglimit", fraglimit );
		trap_Cvar_SetValue( "ui_team_timelimit", timelimit );
		trap_Cvar_SetValue( "ui_team_friendly", friendlyfire );
		break;

	case GT_CTF:
	case GT_CTF4:
		trap_Cvar_SetValue( "ui_ctf_capturelimit", flaglimit );
		trap_Cvar_SetValue( "ui_ctf_timelimit", timelimit );
		trap_Cvar_SetValue( "ui_ctf_friendly", friendlyfire );
		break;
		
	    // Q3Rally Code Start - KOTH
	case GT_KOTH:
	{
		int kothScoreWin = flaglimit;
		int kothTick = kothPtsTick;
		int kothCapture = kothPtsCapture;
		int kothDefend = kothPtsDefend;
		int kothOtEnabled = kothOvertime;
		int kothOtHoldMs = kothOvertimeHoldSec * 1000;

		if ( kothScoreWin <= 0 ) kothScoreWin = 100;
		if ( kothOtHoldMs <= 0 ) kothOtHoldMs = 10000;

		kothScoreWin = (int)Com_Clamp( 1, 999, kothScoreWin );
		kothTick = (int)Com_Clamp( 0, 999, kothTick );
		kothCapture = (int)Com_Clamp( 0, 999, kothCapture );
		kothDefend = (int)Com_Clamp( 0, 999, kothDefend );
		kothOtEnabled = (int)Com_Clamp( 0, 1, kothOtEnabled );
		kothOtHoldMs = (int)Com_Clamp( 1000, 120000, kothOtHoldMs );

		trap_Cvar_SetValue( "g_kothScoreWin", kothScoreWin );
		trap_Cvar_SetValue( "g_kothCaptureTime", 3000 );
		trap_Cvar_SetValue( "g_kothRespawnWave", 5000 );
		trap_Cvar_SetValue( "g_kothPtsTick", kothTick );
		trap_Cvar_SetValue( "g_kothPtsCapture", kothCapture );
		trap_Cvar_SetValue( "g_kothPtsDefend", kothDefend );
		trap_Cvar_SetValue( "g_kothOvertime", kothOtEnabled );
		trap_Cvar_SetValue( "g_kothOvertimeHoldTime", kothOtHoldMs );
		trap_Cvar_SetValue( "ui_koth_scorelimit", kothScoreWin );
		trap_Cvar_SetValue( "ui_koth_pts_tick", kothTick );
		trap_Cvar_SetValue( "ui_koth_pts_capture", kothCapture );
		trap_Cvar_SetValue( "ui_koth_pts_defend", kothDefend );
		trap_Cvar_SetValue( "ui_koth_overtime", kothOtEnabled );
		trap_Cvar_SetValue( "ui_koth_overtime_hold", kothOtHoldMs );
		trap_Cvar_SetValue( "ui_koth_timelimit", timelimit );
		trap_Cvar_SetValue( "ui_koth_friendly", friendlyfire );
		break;
	}
    // Q3Rally Code END - KOTH

    case GT_DOMINATION:
		trap_Cvar_SetValue( "g_dominationSpawnStyle", Com_Clamp( 0, 1, dominationSpawnStyle ) );
		trap_Cvar_SetValue( "g_dominationScoreInterval", Com_Clamp( 0, 99999, dominationScoreInterval * 1000 ) );
		trap_Cvar_SetValue( "g_dominationCaptureDelay", Com_Clamp( 0, 9999, dominationCaptureDelay * 1000 ) );
		trap_Cvar_SetValue( "cg_sigilLocator", Com_Clamp( 0, 1, sigillocator) );
		trap_Cvar_SetValue( "ui_dom_capturelimit", flaglimit );
		trap_Cvar_SetValue( "ui_dom_timelimit", timelimit );
		trap_Cvar_SetValue( "ui_dom_friendly", friendlyfire );
		break;

	}

	trap_Cvar_SetValue( "sv_maxclients", Com_Clamp( 0, 12, maxclients ) );
	trap_Cvar_SetValue( "dedicated", Com_Clamp( 0, 2, dedicated ) );
	trap_Cvar_SetValue ("timelimit", Com_Clamp( 0, timelimit, timelimit ) );
	trap_Cvar_SetValue ("fraglimit", Com_Clamp( 0, fraglimit, fraglimit ) );
	trap_Cvar_SetValue ("laplimit", Com_Clamp( 0, fraglimit, fraglimit ) );
	// Q3Rally Fix: KOTH uses g_kothScoreWin, not capturelimit - reset to 0 to avoid false exits
	if ( s_serveroptions.gametype == GT_KOTH ) {
		trap_Cvar_SetValue( "capturelimit", 0 );
	} else {
		trap_Cvar_SetValue( "capturelimit", Com_Clamp( 0, flaglimit, flaglimit ) );
	}
	trap_Cvar_SetValue( "g_friendlyfire", friendlyfire );
	trap_Cvar_SetValue( "sv_pure", pure );
	trap_Cvar_SetValue( "g_trackLength", Com_Clamp( 0, trackLengthValue, 2 ) );
	trap_Cvar_SetValue( "g_trackReversed", Com_Clamp( 0, reversed, 1 ) );
	trap_Cvar_SetValue( "ui_racing_tracklength", Com_Clamp( 0, trackLengthValue, 2 ) );
	trap_Cvar_SetValue( "ui_racing_trackreversed", Com_Clamp( 0, reversed, 1 ) );
	if ( isRacingGametype ) {
		trap_Cvar_SetValue( "ui_ghostonly", s_serveroptions.ghostOnly.curvalue );
		if ( s_serveroptions.ghostOnly.curvalue ) {
			int playbackValue;

			playbackValue = s_serveroptions.ghostPlaybackRestore > 0 ? s_serveroptions.ghostPlaybackRestore : 1;
			trap_Cvar_SetValue( "cg_ghostPlayback", playbackValue );
		}
	} else {
		trap_Cvar_SetValue( "ui_ghostonly", 0 );
	}

        if ( s_serveroptions.gametype == GT_ELIMINATION ) {
                trap_Cvar_SetValue( "ui_elimination_weapons", eliminationWeapons );
                trap_Cvar_SetValue( "g_eliminationWeapons", eliminationWeapons );
        }
	trap_Cvar_Set( "sv_hostname", s_serveroptions.hostname.field.buffer );
	ServerOptions_ClampPlayerTeams();

	// the wait commands will allow the dedicated to take effect
	trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait ; wait ; map %s\n", s_startserver.maplist[s_startserver.currentmap] ) );

	// add bots
	if( !isRacingGametype || !s_serveroptions.ghostOnly.curvalue ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "wait 3\n" );
		for( n = 1; n < PLAYER_SLOTS; n++ ) {
			if( s_serveroptions.playerType[n].curvalue != 1 ) {
				continue;
			}
			if( s_serveroptions.playerNameBuffers[n][0] == 0 ) {
				continue;
			}
			if( s_serveroptions.playerNameBuffers[n][0] == '-' ) {
				continue;
			}
			if( s_serveroptions.gametype >= GT_TEAM ) {
				Com_sprintf( buf, sizeof(buf), "addbot %s %i %s\n", s_serveroptions.playerNameBuffers[n], skill,
					playerTeam_list[s_serveroptions.playerTeam[n].curvalue] );
			}
			else {
				Com_sprintf( buf, sizeof(buf), "addbot %s %i\n", s_serveroptions.playerNameBuffers[n], skill );
			}
			trap_Cmd_ExecuteText( EXEC_APPEND, buf );
		}
	}

	// set player's team
	if( dedicated == 0 && s_serveroptions.gametype >= GT_TEAM ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait 5; team %s\n", playerTeam_list[s_serveroptions.playerTeam[0].curvalue] ) );
	}
}


/*
=================
ServerOptions_InitPlayerItems
=================
*/
static void ServerOptions_InitPlayerItems( void ) {
        int             n;
        int             v;

        // init types
        if( s_serveroptions.multiplayer ) {
                v = 0;  // open
        }
        else {
                v = 1;  // bot
        }

        for( n = 0; n < PLAYER_SLOTS; n++ ) {
                s_serveroptions.playerType[n].curvalue = v;
        }

        if( s_serveroptions.multiplayer && (s_serveroptions.gametype < GT_TEAM) ) {
                for( n = 8; n < PLAYER_SLOTS; n++ ) {
                        s_serveroptions.playerType[n].curvalue = 2;
                }
        }

        // if not a dedicated server, first slot is reserved for the human on the server
        if( s_serveroptions.dedicated.curvalue == 0 ) {
                // human
                s_serveroptions.playerType[0].generic.flags |= QMF_INACTIVE;
                s_serveroptions.playerType[0].curvalue = 0;
                trap_Cvar_VariableStringBuffer( "name", s_serveroptions.playerNameBuffers[0], sizeof(s_serveroptions.playerNameBuffers[0]) );
                Q_CleanStr( s_serveroptions.playerNameBuffers[0] );
        }

        // init teams
        if( s_serveroptions.gametype >= GT_TEAM ) {
                for( n = 0; n < (PLAYER_SLOTS / 3); n++ ) {
                        s_serveroptions.playerTeam[n].curvalue = 0;
                }
                for( ; n < PLAYER_SLOTS; n++ ) {
                        s_serveroptions.playerTeam[n].curvalue = 1;
                }
                ServerOptions_UpdatePlayerTeamLists();
        }
        else {
                for( n = 0; n < PLAYER_SLOTS; n++ ) {
                        s_serveroptions.playerTeam[n].generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
                }
        }

	if( ServerOptions_IsRacingGametype( s_serveroptions.gametype ) && s_serveroptions.ghostOnly.curvalue ) {
                int startIndex;

                startIndex = (s_serveroptions.dedicated.curvalue == 0) ? 1 : 0;

                for( n = startIndex; n < PLAYER_SLOTS; n++ ) {
                        s_serveroptions.playerType[n].curvalue = 0;
                        Q_strncpyz( s_serveroptions.playerNameBuffers[n], "----", sizeof( s_serveroptions.playerNameBuffers[n] ) );
                }
        }
}



/*
=================
ServerOptions_SetPlayerItems
=================
*/
static void ServerOptions_SetPlayerItems( void ) {
	int		start;
	int		n;

	// names
	if( s_serveroptions.dedicated.curvalue == 0 ) {
		s_serveroptions.player0.string = "Human";
		s_serveroptions.playerName[0].generic.flags &= ~QMF_HIDDEN;

		start = 1;
	}
	else {
		s_serveroptions.player0.string = "Open";
		start = 0;
	}

	for( n = start; n < PLAYER_SLOTS; n++ ) {
		if( s_serveroptions.playerType[n].curvalue == 1 ) {
			s_serveroptions.playerName[n].generic.flags &= ~(QMF_INACTIVE|QMF_HIDDEN);
		}
		else {
			s_serveroptions.playerName[n].generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		}
	}

	// teams
	if( s_serveroptions.gametype < GT_TEAM ) {
		return;
	}
	ServerOptions_UpdatePlayerTeamLists();
	for( n = start; n < PLAYER_SLOTS; n++ ) {
		if( s_serveroptions.playerType[n].curvalue == 2 ) {
			s_serveroptions.playerTeam[n].generic.flags |= (QMF_INACTIVE|QMF_HIDDEN);
		}
		else {
			s_serveroptions.playerTeam[n].generic.flags &= ~(QMF_INACTIVE|QMF_HIDDEN);
		}
	}
}


/*
=================
ServerOptions_Event
=================
*/
static void ServerOptions_Event( void* ptr, int event ) {
        switch( ((menucommon_s*)ptr)->id ) {
        case ID_PLAYER_TYPE:
                if( event != QM_ACTIVATED ) {
                        break;
                }
                ServerOptions_SetPlayerItems();
                break;

        case ID_MAXCLIENTS:
        case ID_DEDICATED:
                ServerOptions_SetPlayerItems();
                break;

	case ID_GHOST_ONLY:
		if( event != QM_ACTIVATED || !ServerOptions_IsRacingGametype( s_serveroptions.gametype ) ) {
			break;
		}

		trap_Cvar_SetValue( "ui_ghostonly", s_serveroptions.ghostOnly.curvalue );
		if( ServerOptions_IsRacingGametype( s_serveroptions.gametype ) && s_serveroptions.ghostOnly.curvalue ) {
			trap_Cvar_SetValue( "cg_ghostPlayback", s_serveroptions.ghostPlaybackRestore > 0 ? s_serveroptions.ghostPlaybackRestore : 1 );
			ServerOptions_InitPlayerItems();
		}
		else {
			trap_Cvar_SetValue( "cg_ghostPlayback", s_serveroptions.ghostPlaybackRestore );
			ServerOptions_InitPlayerItems();
			ServerOptions_InitBotNames();
		}
                ServerOptions_SetPlayerItems();
                break;


        case ID_GO:
                if( event != QM_ACTIVATED ) {
                        break;
                }
                ServerOptions_Start();
                break;

        case ID_BACK:
                if( event != QM_ACTIVATED ) {
                        break;
                }
                UI_PopMenu();
                break;
        }
}


static void ServerOptions_PlayerNameEvent( void* ptr, int event ) {
	int		n;

	if( event != QM_ACTIVATED ) {
		return;
	}
	n = ((menutext_s*)ptr)->generic.id;
	s_serveroptions.newBotIndex = n;
	UI_BotSelectMenu( s_serveroptions.playerNameBuffers[n] );
}


/*
=================
ServerOptions_StatusBar
=================
*/
static void ServerOptions_StatusBar( void* ptr ) {
	UI_DrawString( 320, 440, "0 = NO LIMIT", UI_CENTER|UI_SMALLFONT, colorWhite );
}

static void ServerOptions_GhostOnlyStatusBar( void* ptr ) {
	(void)ptr;
	UI_DrawString( 320, 440, "Starts without AI drivers and enables the selected ghost playback.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
===========================
ServerOptions_LevelshotDraw
===========================
*/
static void ServerOptions_LevelshotDraw( void *self ) {
	menubitmap_s	*b;
	char			*s;
	char			*info;
	char			author[MAX_QPATH];
	int				x;
	int				y;
	int				w;
	int				h;
//	int				n;

	b = (menubitmap_s *)self;

	if( !b->generic.name ) {
		return;
	}

	if( b->generic.name && !b->shader ) {
		b->shader = trap_R_RegisterShaderNoMip( b->generic.name );
		if( !b->shader && b->errorpic ) {
			b->shader = trap_R_RegisterShaderNoMip( b->errorpic );
		}
		if( !b->shader ) {
			b->shader = trap_R_RegisterShaderNoMip(
				GAMESERVER_MISSING_MAP_SHOT );
		}
	}

	if( b->focuspic && !b->focusshader ) {
		b->focusshader = trap_R_RegisterShaderNoMip( b->focuspic );
	}

	w = 256;
	x = b->generic.x - w / 2;
	y = b->generic.y;
	UI_FillRect( x, y, w, 160, menu_back_color );

	if (s_startserver.list.curvalue < 0 || s_startserver.list.curvalue >= s_startserver.nummaps)
		return;

	x = b->generic.x - b->width / 2;
	y = b->generic.y + 10;
	w = b->width;
	h =	b->height;
	if( b->shader ) {
		UI_DrawHandlePic( x, y, w, h, b->shader );
	}

	x += b->width / 2;
	y += 96 + 4;

	UI_DrawString( x, y, s_startserver.maplistname[s_startserver.list.curvalue], UI_CENTER|UI_SMALLFONT, text_color_normal );

	info = s_startserver.mapinfo[s_startserver.currentmap];
	s = Info_ValueForKey( info, "author");
	if (!s || !strcmp(s, ""))
		Com_sprintf(author, sizeof(author), "Author: Unknown");
	else
		Com_sprintf(author, sizeof(author), "Author: %s", s);

	y += SMALLCHAR_HEIGHT;
	UI_DrawString( x, y, author, UI_CENTER|UI_SMALLFONT, text_color_normal );

	y += SMALLCHAR_HEIGHT + 2;
	{
		int gtIdx = s_serveroptions.gametype;
		if ( gtIdx < 0 || gtIdx >= (int)ARRAY_LEN(gametype_remap2) ) gtIdx = 0;
		UI_DrawString( x, y, gametype_items[gametype_remap2[gtIdx]], UI_CENTER|UI_SMALLFONT, text_color_normal );
	}

	x = b->generic.x;
	y = b->generic.y;
	w = b->width;
	h =	b->height + 28;
	if( b->generic.flags & QMF_HIGHLIGHT ) {	
		UI_DrawHandlePic( x, y, w, h, b->focusshader );
	}
}

/*
===============
ServerOptions_InitBotNames
===============
*/
static void ServerOptions_InitBotNames( void ) {
        int                     count;
        int                     n;
        const char      *arenaInfo;
        const char      *botInfo;
        char            *p;
        char            *bot;
        char            bots[MAX_INFO_STRING];
        int                     startIndex;

        startIndex = (s_serveroptions.dedicated.curvalue == 0) ? 1 : 0;

	if( ServerOptions_IsRacingGametype( s_serveroptions.gametype ) && s_serveroptions.ghostOnly.curvalue ) {
                for( n = startIndex; n < PLAYER_SLOTS; n++ ) {
                        s_serveroptions.playerType[n].curvalue = 0;
                        Q_strncpyz( s_serveroptions.playerNameBuffers[n], "----", sizeof( s_serveroptions.playerNameBuffers[n] ) );
                }

                return;
        }

    if( s_serveroptions.gametype > GT_DOMINATION ) {
		Q_strncpyz( s_serveroptions.playerNameBuffers[1], "Bobby", 16 );
		Q_strncpyz( s_serveroptions.playerNameBuffers[2], "Carla", 16 );

    if( s_serveroptions.gametype > GT_DOMINATION ) {
		Q_strncpyz( s_serveroptions.playerNameBuffers[3], "Paul", 16 );
		}    
		else {
			s_serveroptions.playerType[3].curvalue = 2;
		}
		s_serveroptions.playerType[4].curvalue = 2;
		s_serveroptions.playerType[5].curvalue = 2;

		Q_strncpyz( s_serveroptions.playerNameBuffers[6], "Alexandra", 16 );
		Q_strncpyz( s_serveroptions.playerNameBuffers[7], "Sam", 16 );
		Q_strncpyz( s_serveroptions.playerNameBuffers[8], "Dean", 16 );

    if( s_serveroptions.gametype > GT_DOMINATION ) {
		Q_strncpyz( s_serveroptions.playerNameBuffers[9], "Janine", 16 );
		}
		else {
			s_serveroptions.playerType[9].curvalue = 2;
		}
		s_serveroptions.playerType[10].curvalue = 2;
		s_serveroptions.playerType[11].curvalue = 2;

		return;
	}

	count = 1;	// skip the first slot, reserved for a human

	// get info for this map
	arenaInfo = UI_GetArenaInfoByMap( s_serveroptions.mapnamebuffer );

	// get the bot info - we'll seed with them if any are listed
	Q_strncpyz( bots, Info_ValueForKey( arenaInfo, "bots" ), sizeof(bots) );
	p = &bots[0];
	while( *p && count < PLAYER_SLOTS ) {
		//skip spaces
		while( *p && *p == ' ' ) {
			p++;
		}
		if( !p ) {
			break;
		}

		// mark start of bot name
		bot = p;

		// skip until space of null
		while( *p && *p != ' ' ) {
			p++;
		}
		if( *p ) {
			*p++ = 0;
		}

		botInfo = UI_GetBotInfoByName( bot );
		bot = Info_ValueForKey( botInfo, "name" );

		Q_strncpyz( s_serveroptions.playerNameBuffers[count], bot, sizeof(s_serveroptions.playerNameBuffers[count]) );
		count++;
	}

	// set the rest of the bot slots to "---"
	for( n = count; n < PLAYER_SLOTS; n++ ) {
		strcpy( s_serveroptions.playerNameBuffers[n], "--------" );
	}

	// pad up to #8 as open slots
	for( ;count < 8; count++ ) {
		s_serveroptions.playerType[count].curvalue = 0;
	}

	// close off the rest by default
	for( ;count < PLAYER_SLOTS; count++ ) {
		if( s_serveroptions.playerType[count].curvalue == 1 ) {
			s_serveroptions.playerType[count].curvalue = 2;
		}
	}
}



/*
=================
ServerOptions_SetMenuItems
=================
*/
static void ServerOptions_SetMenuItems( void ) {
	static char picname[64];

	switch( s_serveroptions.gametype ) {

	case GT_SPRINT:
		Q_strncpyz( s_serveroptions.fraglimit.field.buffer, "1", sizeof( s_serveroptions.fraglimit.field.buffer ) );
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_racing_timelimit" ) ) );
		break;

	case GT_RACING:

	case GT_RACING_DM:
	case GT_ELIMINATION:
	default:
		Com_sprintf( s_serveroptions.fraglimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_racing_laplimit" ) ) );
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_racing_timelimit" ) ) );
		break;

	case GT_TEAM_RACING:
	case GT_TEAM_RACING_DM:
		Com_sprintf( s_serveroptions.fraglimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_team_racing_laplimit" ) ) );
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_team_racing_timelimit" ) ) );
		s_serveroptions.friendlyfire.curvalue = (int)Com_Clamp( 0, 1, trap_Cvar_VariableValue( "ui_team_racing_friendly" ) );
		break;

	case GT_DERBY:
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_derby_timelimit" ) ) );
		{
			int mp = (int)trap_Cvar_VariableValue( "ui_derby_minplayers" );
			if ( mp < 2 ) mp = 2;
			Com_sprintf( s_serveroptions.minPlayers.field.buffer, 3, "%i", (int)Com_Clamp( 2, 99, mp ) );
		}
		break;
		
	case GT_LCS:
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_lcs_timelimit" ) ) );
		{
			int mp = (int)trap_Cvar_VariableValue( "ui_lcs_minplayers" );
			if ( mp < 2 ) mp = 2;
			Com_sprintf( s_serveroptions.minPlayers.field.buffer, 3, "%i", (int)Com_Clamp( 2, 99, mp ) );
		}
		break;

	case GT_DEATHMATCH:
		Com_sprintf( s_serveroptions.fraglimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_dm_fraglimit" ) ) );
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_dm_timelimit" ) ) );
		break;

	case GT_TEAM:
		Com_sprintf( s_serveroptions.fraglimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_team_fraglimit" ) ) );
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_team_timelimit" ) ) );
		s_serveroptions.friendlyfire.curvalue = (int)Com_Clamp( 0, 1, trap_Cvar_VariableValue( "ui_team_friendly" ) );
		break;

	case GT_CTF:
	case GT_CTF4:
		Com_sprintf( s_serveroptions.flaglimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 100, trap_Cvar_VariableValue( "ui_ctf_capturelimit" ) ) );
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_ctf_timelimit" ) ) );
		s_serveroptions.friendlyfire.curvalue = (int)Com_Clamp( 0, 1, trap_Cvar_VariableValue( "ui_ctf_friendly" ) );
		break;

	case GT_KOTH:
	{
		int kothScoreLimit = (int)trap_Cvar_VariableValue( "ui_koth_scorelimit" );
		int kothTick;
		int kothCapture;
		int kothDefend;
		int kothOtEnable;
		int kothOtHoldMs;
		if ( kothScoreLimit <= 0 ) {
			kothScoreLimit = (int)trap_Cvar_VariableValue( "g_kothScoreWin" );
		}
		if ( kothScoreLimit <= 0 ) {
			kothScoreLimit = 100;
		}
			kothScoreLimit = (int)Com_Clamp( 1, 999, kothScoreLimit );
			trap_Cvar_SetValue( "ui_koth_scorelimit", kothScoreLimit );
			Com_sprintf( s_serveroptions.flaglimit.field.buffer, 4, "%i", kothScoreLimit );
			kothTick = StartServer_CvarIntOrDefault( "ui_koth_pts_tick", -1 );
			if ( kothTick < 0 ) kothTick = StartServer_CvarIntOrDefault( "koth_pts_tick", -1 );
			if ( kothTick < 0 ) kothTick = StartServer_CvarIntOrDefault( "g_kothPtsTick", 1 );
			kothCapture = StartServer_CvarIntOrDefault( "ui_koth_pts_capture", -1 );
			if ( kothCapture < 0 ) kothCapture = StartServer_CvarIntOrDefault( "koth_pts_capture", -1 );
			if ( kothCapture < 0 ) kothCapture = StartServer_CvarIntOrDefault( "g_kothPtsCapture", 5 );
			kothDefend = StartServer_CvarIntOrDefault( "ui_koth_pts_defend", -1 );
			if ( kothDefend < 0 ) kothDefend = StartServer_CvarIntOrDefault( "koth_pts_defend", -1 );
			if ( kothDefend < 0 ) kothDefend = StartServer_CvarIntOrDefault( "g_kothPtsDefend", 3 );
		kothTick = (int)Com_Clamp( 0, 999, kothTick );
		kothCapture = (int)Com_Clamp( 0, 999, kothCapture );
		kothDefend = (int)Com_Clamp( 0, 999, kothDefend );
		trap_Cvar_SetValue( "ui_koth_pts_tick", kothTick );
		trap_Cvar_SetValue( "ui_koth_pts_capture", kothCapture );
		trap_Cvar_SetValue( "ui_koth_pts_defend", kothDefend );
		Com_sprintf( s_serveroptions.kothPtsTick.field.buffer, 4, "%i", kothTick );
		Com_sprintf( s_serveroptions.kothPtsCapture.field.buffer, 4, "%i", kothCapture );
		Com_sprintf( s_serveroptions.kothPtsDefend.field.buffer, 4, "%i", kothDefend );

		kothOtEnable = StartServer_CvarIntOrDefault( "ui_koth_overtime", -1 );
		if ( kothOtEnable < 0 ) kothOtEnable = StartServer_CvarIntOrDefault( "koth_overtime", -1 );
		if ( kothOtEnable < 0 ) kothOtEnable = StartServer_CvarIntOrDefault( "g_kothOvertime", 1 );
		kothOtEnable = (int)Com_Clamp( 0, 1, kothOtEnable );
		kothOtHoldMs = StartServer_CvarIntOrDefault( "ui_koth_overtime_hold", -1 );
		if ( kothOtHoldMs <= 0 ) kothOtHoldMs = StartServer_CvarIntOrDefault( "koth_overtime_hold", -1 );
		if ( kothOtHoldMs <= 0 ) kothOtHoldMs = StartServer_CvarIntOrDefault( "g_kothOvertimeHoldTime", 10000 );
		kothOtHoldMs = (int)Com_Clamp( 1000, 120000, kothOtHoldMs );
		trap_Cvar_SetValue( "ui_koth_overtime", kothOtEnable );
		trap_Cvar_SetValue( "ui_koth_overtime_hold", kothOtHoldMs );
		s_serveroptions.kothOvertime.curvalue = kothOtEnable;
		Com_sprintf( s_serveroptions.kothOvertimeHold.field.buffer, 4, "%i", kothOtHoldMs / 1000 );
			Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_koth_timelimit" ) ) );
			s_serveroptions.friendlyfire.curvalue = (int)Com_Clamp( 0, 1, trap_Cvar_VariableValue( "ui_koth_friendly" ) );
			break;
		}

	case GT_DOMINATION:
		Com_sprintf( s_serveroptions.flaglimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 100, trap_Cvar_VariableValue( "ui_dom_capturelimit" ) ) );
		Com_sprintf( s_serveroptions.timelimit.field.buffer, 4, "%i", (int)Com_Clamp( 0, 999, trap_Cvar_VariableValue( "ui_dom_timelimit" ) ) );
		s_serveroptions.friendlyfire.curvalue = (int)Com_Clamp( 0, 1, trap_Cvar_VariableValue( "ui_dom_friendly" ) );
		s_serveroptions.sigillocator.curvalue = (int)Com_Clamp( 0, 1, trap_Cvar_VariableValue( "cg_sigilLocator" ) );
		Com_sprintf( s_serveroptions.dominationScoreInterval.field.buffer, 6, "%i", (int)Com_Clamp( 0, 99999, trap_Cvar_VariableValue( "g_dominationScoreInterval" ) ) / 1000 );
		Com_sprintf( s_serveroptions.dominationCaptureDelay.field.buffer, 5, "%i", (int)Com_Clamp( 0, 9999, trap_Cvar_VariableValue( "g_dominationCaptureDelay" ) ) / 1000 );
		break;

	}

	if ( s_serveroptions.gametype == GT_ELIMINATION ) {
		s_serveroptions.eliminationWeapons.curvalue = (int)Com_Clamp( 0, 1, trap_Cvar_VariableValue( "ui_elimination_weapons" ) );
	} else {
		s_serveroptions.eliminationWeapons.curvalue = 0;
	}

	Q_strncpyz( s_serveroptions.hostname.field.buffer, UI_Cvar_VariableString( "sv_hostname" ), sizeof( s_serveroptions.hostname.field.buffer ) );
	s_serveroptions.pure.curvalue = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "sv_pure" ) );
	s_serveroptions.trackLength.curvalue = ServerOptions_TrackLengthIndexFromValue( (int)trap_Cvar_VariableValue( "ui_racing_tracklength" ) );
	s_serveroptions.reversed.curvalue = (int)Com_Clamp( 0, 1, trap_Cvar_VariableValue( "ui_racing_trackreversed" ) );
	s_serveroptions.ghostPlaybackRestore = (int)Com_Clamp( 0, 2, trap_Cvar_VariableValue( "cg_ghostPlayback" ) );
	if ( ServerOptions_IsRacingGametype( s_serveroptions.gametype ) ) {
		s_serveroptions.ghostOnly.curvalue = (int)Com_Clamp( 0, 1, trap_Cvar_VariableValue( "ui_ghostonly" ) );
	} else {
		s_serveroptions.ghostOnly.curvalue = 0;
	}

	// set the map pic
	Com_sprintf( picname, 64, "levelshots/%s", s_startserver.maplist[s_startserver.currentmap] );
	s_serveroptions.mappic.generic.name = picname;



	// set the map name
	strcpy( s_serveroptions.mapnamebuffer, s_startserver.mapname.string );
	Q_strupr( s_serveroptions.mapnamebuffer );

	// get the player selections initialized
	ServerOptions_InitPlayerItems();
	ServerOptions_SetPlayerItems();

	// seed bot names
	ServerOptions_InitBotNames();
	ServerOptions_SetPlayerItems();
}

/*
=================
PlayerName_Draw
=================
*/
static void PlayerName_Draw( void *item ) {
	menutext_s	*s;
	float		*color;
	int			x, y;
	int			style;
	qboolean	focus;

	s = (menutext_s *)item;

	x = s->generic.x;
	y =	s->generic.y;

	style = UI_SMALLFONT;
	focus = (s->generic.parent->cursor == s->generic.menuPosition);

	if ( s->generic.flags & QMF_GRAYED )
		color = text_color_disabled;
	else if ( focus )
	{
		color = text_color_highlight;
		style |= UI_PULSE;
	}
	else if ( s->generic.flags & QMF_BLINK )
	{
		color = text_color_highlight;
		style |= UI_BLINK;
	}
	else
		color = text_color_normal;

	if ( focus )
	{
		// draw cursor
		UI_FillRect( s->generic.left, s->generic.top, s->generic.right-s->generic.left+1, s->generic.bottom-s->generic.top+1, listbar_color ); 
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT, color);
	}

	UI_DrawString( x - SMALLCHAR_WIDTH, y, s->generic.name, style|UI_RIGHT, color );
	UI_DrawString( x + SMALLCHAR_WIDTH, y, s->string, style|UI_LEFT, color );
}

static int ServerOptions_PlayerSlotForItem( menucommon_s *item, int kind ) {
        int n;

        for ( n = 0; n < PLAYER_SLOTS; n++ ) {
                if ( kind == 0 && item == (menucommon_s *)&s_serveroptions.playerName[n] ) {
                        return n;
                }
                if ( kind == 1 && item == (menucommon_s *)&s_serveroptions.playerType[n] ) {
                        return n;
                }
                if ( kind == 2 && item == (menucommon_s *)&s_serveroptions.playerTeam[n] ) {
                        return n;
                }
        }

        return -1;
}

static void ServerOptions_DrawOptionCard( menucommon_s *item,
                                           const char *value ) {
        qboolean focus;
        qboolean disabled;
        vec4_t textColor;
        char label[64];
        char displayValue[64];
        int width;

        focus = ( Menu_ItemAtCursor( item->parent ) == item );
        disabled = ( item->flags & QMF_GRAYED ) ? qtrue : qfalse;
        width = item->right - item->left;
        Frontend_DrawCard( item->left, item->top, width,
                item->bottom - item->top, 1.0f,
                focus && !disabled );

        StartServer_FitText( label, sizeof( label ), item->name ? item->name : "Option",
                width / 2 - 12 );
        StartServer_FitText( displayValue, sizeof( displayValue ), value ? value : "",
                width / 2 - 12 );

        if ( disabled ) {
                Vector4Copy( serverOptionsMutedColor, textColor );
                textColor[3] *= 0.45f;
        } else if ( focus ) {
                Vector4Copy( serverOptionsAccentColor, textColor );
        } else {
                Vector4Copy( serverOptionsMutedColor, textColor );
        }

        Frontend_DrawText( item->left + 8,
                item->top + ( ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2 ), label,
                UI_LEFT | UI_SMALLFONT, textColor );
        Frontend_DrawText( item->right - 8,
                item->top + ( ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2 ),
                displayValue,
                UI_RIGHT | UI_SMALLFONT,
                disabled ? textColor : serverOptionsTextColor );
}

static void ServerOptions_DrawField( void *self ) {
        menufield_s *field;

        field = (menufield_s *)self;
        ServerOptions_DrawOptionCard( &field->generic, field->field.buffer );
}

static void ServerOptions_DrawList( void *self ) {
        menulist_s *list;
        const char *value;

        list = (menulist_s *)self;
        value = "";
        if ( list->itemnames && list->curvalue >= 0 &&
             list->itemnames[list->curvalue] ) {
                value = list->itemnames[list->curvalue];
        }
        ServerOptions_DrawOptionCard( &list->generic, value );
}

static void ServerOptions_DrawToggle( void *self ) {
        menuradiobutton_s *toggle;

        toggle = (menuradiobutton_s *)self;
        ServerOptions_DrawOptionCard( &toggle->generic,
                toggle->curvalue ? "On" : "Off" );
}

static void ServerOptions_DrawPlayerName( void *self ) {
        menutext_s *nameItem;
        int slot;
        int y;
        qboolean focus;
        char name[32];
        char type[16];

        nameItem = (menutext_s *)self;
        slot = nameItem->generic.id;
        if ( slot < 0 || slot >= PLAYER_SLOTS ) {
                return;
        }

        y = SERVEROPT_PLAYER_ROW_Y + slot *
                ( SERVEROPT_PLAYER_ROW_HEIGHT + SERVEROPT_PLAYER_ROW_GAP );
        focus = ( Menu_ItemAtCursor( nameItem->generic.parent ) == nameItem );
        Frontend_DrawCard( SERVEROPT_PLAYER_ROW_X, y,
                SERVEROPT_PLAYER_ROW_WIDTH, SERVEROPT_PLAYER_ROW_HEIGHT,
                1.0f, focus );

        if ( slot == 0 ) {
                Q_strncpyz( type, "Human", sizeof( type ) );
        } else if ( s_serveroptions.playerType[slot].itemnames &&
                    s_serveroptions.playerType[slot].curvalue >= 0 &&
                    s_serveroptions.playerType[slot].itemnames[
                            s_serveroptions.playerType[slot].curvalue] ) {
                Q_strncpyz( type,
                        s_serveroptions.playerType[slot].itemnames[
                                s_serveroptions.playerType[slot].curvalue],
                        sizeof( type ) );
        } else {
                Q_strncpyz( type, "Open", sizeof( type ) );
        }

        Q_strncpyz( name, s_serveroptions.playerNameBuffers[slot], sizeof( name ) );
        if ( !name[0] ) {
                Q_strncpyz( name, "Open slot", sizeof( name ) );
        }
        StartServer_FitText( name, sizeof( name ), name, 128 );

        Frontend_DrawText( SERVEROPT_PLAYER_ROW_X + 8, y + 3, type,
                UI_LEFT | UI_SMALLFONT,
                focus ? serverOptionsAccentColor : serverOptionsMutedColor );
        Frontend_DrawText( SERVEROPT_PLAYER_ROW_X + 58, y + 3, name,
                UI_LEFT | UI_SMALLFONT, serverOptionsTextColor );
}

static void ServerOptions_DrawPlayerType( void *self ) {
        menulist_s *typeItem;
        int slot;
        const char *value;
        qboolean focus;

        typeItem = (menulist_s *)self;
        slot = ServerOptions_PlayerSlotForItem( &typeItem->generic, 1 );
        if ( slot < 0 ) {
                return;
        }
        value = "Open";
        if ( typeItem->itemnames && typeItem->curvalue >= 0 &&
             typeItem->itemnames[typeItem->curvalue] ) {
                value = typeItem->itemnames[typeItem->curvalue];
        }
        focus = ( Menu_ItemAtCursor( typeItem->generic.parent ) == typeItem );
        Frontend_DrawText( typeItem->generic.left + 4,
                typeItem->generic.top + 3, value,
                UI_LEFT | UI_SMALLFONT,
                focus ? serverOptionsAccentColor : serverOptionsMutedColor );
}

static void ServerOptions_DrawPlayerTeam( void *self ) {
        menulist_s *teamItem;
        const char *value;
        qboolean focus;

        teamItem = (menulist_s *)self;
        value = "";
        if ( teamItem->itemnames && teamItem->curvalue >= 0 &&
             teamItem->itemnames[teamItem->curvalue] ) {
                value = teamItem->itemnames[teamItem->curvalue];
        }
        focus = ( Menu_ItemAtCursor( teamItem->generic.parent ) == teamItem );
        Frontend_DrawText( teamItem->generic.left,
                teamItem->generic.top + 3, value,
                UI_RIGHT | UI_SMALLFONT,
                focus ? serverOptionsAccentColor : serverOptionsMutedColor );
}

static void ServerOptions_DrawAction( void *self ) {
        menutext_s *button;
        qboolean focus;

        button = (menutext_s *)self;
        focus = ( Menu_ItemAtCursor( button->generic.parent ) == button );
        Frontend_DrawButton( button->generic.left, button->generic.top,
                button->generic.right - button->generic.left,
                button->generic.bottom - button->generic.top,
                button->string, 1.0f, focus, UI_FRONTEND_TEXT_CENTER );
}

static void ServerOptions_LayoutFrontend( void ) {
        menucommon_s *item;
        int n;
        int slot;
        int optionIndex;
        int row;
        int x;
        int y;

        s_serveroptions.banner.generic.flags |= QMF_HIDDEN | QMF_INACTIVE;
        s_serveroptions.mappic.generic.flags |= QMF_HIDDEN | QMF_INACTIVE;
        s_serveroptions.player0.generic.flags |= QMF_HIDDEN | QMF_INACTIVE;

        s_serveroptions.botSkill.generic.left = SERVEROPT_BOT_X;
        s_serveroptions.botSkill.generic.top = SERVEROPT_BOT_Y;
        s_serveroptions.botSkill.generic.right = SERVEROPT_BOT_X + SERVEROPT_BOT_WIDTH;
        s_serveroptions.botSkill.generic.bottom = SERVEROPT_BOT_Y + SERVEROPT_BOT_HEIGHT;
        s_serveroptions.botSkill.generic.x = SERVEROPT_BOT_X + 8;
        s_serveroptions.botSkill.generic.y = SERVEROPT_BOT_Y + 4;
        s_serveroptions.botSkill.generic.ownerdraw = ServerOptions_DrawList;

        for ( slot = 0; slot < PLAYER_SLOTS; slot++ ) {
                y = SERVEROPT_PLAYER_ROW_Y + slot *
                        ( SERVEROPT_PLAYER_ROW_HEIGHT + SERVEROPT_PLAYER_ROW_GAP );

                s_serveroptions.playerName[slot].generic.left =
                        SERVEROPT_PLAYER_ROW_X + 54;
                s_serveroptions.playerName[slot].generic.top = y;
                s_serveroptions.playerName[slot].generic.right =
                        SERVEROPT_PLAYER_ROW_X + 190;
                s_serveroptions.playerName[slot].generic.bottom =
                        y + SERVEROPT_PLAYER_ROW_HEIGHT;
                s_serveroptions.playerName[slot].generic.x =
                        s_serveroptions.playerName[slot].generic.left;
                s_serveroptions.playerName[slot].generic.y = y + 3;
                s_serveroptions.playerName[slot].generic.ownerdraw =
                        ServerOptions_DrawPlayerName;

                s_serveroptions.playerType[slot].generic.left =
                        SERVEROPT_PLAYER_ROW_X + 4;
                s_serveroptions.playerType[slot].generic.top = y;
                s_serveroptions.playerType[slot].generic.right =
                        SERVEROPT_PLAYER_ROW_X + 52;
                s_serveroptions.playerType[slot].generic.bottom =
                        y + SERVEROPT_PLAYER_ROW_HEIGHT;
                s_serveroptions.playerType[slot].generic.x =
                        s_serveroptions.playerType[slot].generic.left;
                s_serveroptions.playerType[slot].generic.y = y + 3;
                s_serveroptions.playerType[slot].generic.ownerdraw =
                        ServerOptions_DrawPlayerType;

                s_serveroptions.playerTeam[slot].generic.left =
                        SERVEROPT_PLAYER_ROW_X + 196;
                s_serveroptions.playerTeam[slot].generic.top = y;
                s_serveroptions.playerTeam[slot].generic.right =
                        SERVEROPT_PLAYER_ROW_X + SERVEROPT_PLAYER_ROW_WIDTH - 8;
                s_serveroptions.playerTeam[slot].generic.bottom =
                        y + SERVEROPT_PLAYER_ROW_HEIGHT;
                s_serveroptions.playerTeam[slot].generic.x =
                        s_serveroptions.playerTeam[slot].generic.right;
                s_serveroptions.playerTeam[slot].generic.y = y + 3;
                s_serveroptions.playerTeam[slot].generic.ownerdraw =
                        ServerOptions_DrawPlayerTeam;
        }

        optionIndex = 0;
        for ( n = 0; n < s_serveroptions.menu.nitems; n++ ) {
                item = (menucommon_s *)s_serveroptions.menu.items[n];
                if ( item == (menucommon_s *)&s_serveroptions.banner ||
                     item == (menucommon_s *)&s_serveroptions.mappic ||
                     item == (menucommon_s *)&s_serveroptions.player0 ||
                     item == (menucommon_s *)&s_serveroptions.botSkill ||
                     item == (menucommon_s *)&s_serveroptions.back ||
                     item == (menucommon_s *)&s_serveroptions.go ) {
                        continue;
                }

                if ( item->flags & QMF_HIDDEN ) {
                        continue;
                }

                if ( ServerOptions_PlayerSlotForItem( item, 0 ) >= 0 ||
                     ServerOptions_PlayerSlotForItem( item, 1 ) >= 0 ||
                     ServerOptions_PlayerSlotForItem( item, 2 ) >= 0 ) {
                        continue;
                }

                row = optionIndex;
                x = SERVEROPT_OPTION_X + 16;
                y = SERVEROPT_OPTION_START_Y + row *
                        ( SERVEROPT_OPTION_ROW_HEIGHT + SERVEROPT_OPTION_ROW_GAP );
                item->left = x;
                item->top = y;
                item->right = x + SERVEROPT_OPTION_COLUMN_WIDTH;
                item->bottom = y + SERVEROPT_OPTION_ROW_HEIGHT;
                item->x = x + 8;
                item->y = y + 7;

                if ( item->type == MTYPE_FIELD ) {
                        item->ownerdraw = ServerOptions_DrawField;
                } else if ( item->type == MTYPE_SPINCONTROL ) {
                        item->ownerdraw = ServerOptions_DrawList;
                } else if ( item->type == MTYPE_RADIOBUTTON ) {
                        item->ownerdraw = ServerOptions_DrawToggle;
                }
                optionIndex++;
        }

        s_serveroptions.back.generic.left = SERVEROPT_FRAME_X + 16;
        s_serveroptions.back.generic.top = SERVEROPT_ACTION_Y;
        s_serveroptions.back.generic.right = s_serveroptions.back.generic.left +
                SERVEROPT_ACTION_WIDTH;
        s_serveroptions.back.generic.bottom = SERVEROPT_ACTION_Y +
                SERVEROPT_ACTION_HEIGHT;
        s_serveroptions.back.generic.x = ( s_serveroptions.back.generic.left +
                s_serveroptions.back.generic.right ) / 2;
        s_serveroptions.back.generic.y = SERVEROPT_ACTION_Y + 4;
        s_serveroptions.back.generic.ownerdraw = ServerOptions_DrawAction;

        s_serveroptions.go.generic.left = SERVEROPT_FRAME_X + SERVEROPT_FRAME_WIDTH -
                24 - SERVEROPT_ACTION_WIDTH;
        s_serveroptions.go.generic.top = SERVEROPT_ACTION_Y;
        s_serveroptions.go.generic.right = s_serveroptions.go.generic.left +
                SERVEROPT_ACTION_WIDTH;
        s_serveroptions.go.generic.bottom = SERVEROPT_ACTION_Y +
                SERVEROPT_ACTION_HEIGHT;
        s_serveroptions.go.generic.x = ( s_serveroptions.go.generic.left +
                s_serveroptions.go.generic.right ) / 2;
        s_serveroptions.go.generic.y = SERVEROPT_ACTION_Y + 4;
        s_serveroptions.go.generic.ownerdraw = ServerOptions_DrawAction;
}

static void ServerOptions_Draw( void ) {
        vec4_t scrimColor = UI_FRONTEND_COLOR_SCRIM;
        char subtitle[128];
        qhandle_t mapShader;

        Frontend_DrawBackground( scrimColor );
        Frontend_DrawPanel( SERVEROPT_FRAME_X, SERVEROPT_FRAME_Y,
                SERVEROPT_FRAME_WIDTH, SERVEROPT_FRAME_HEIGHT, 1.0f,
                UI_FRONTEND_STYLE_FRAME );
        Frontend_DrawText( SERVEROPT_FRAME_X + 24, SERVEROPT_FRAME_Y + 24,
                "Host race", UI_LEFT | UI_BIGFONT, serverOptionsTextColor );
        Com_sprintf( subtitle, sizeof( subtitle ), "%s  ·  %s",
                s_serveroptions.mapnamebuffer,
                gametype_items[gametype_remap2[s_serveroptions.gametype]] );
        Frontend_DrawText( SERVEROPT_FRAME_X + 24, SERVEROPT_FRAME_Y + 48,
                subtitle, UI_LEFT | UI_SMALLFONT, serverOptionsMutedColor );
        Frontend_DrawStatusChip( SERVEROPT_FRAME_X + 310,
                SERVEROPT_FRAME_Y + 26, "Server options", serverOptionsAccentColor,
                1.0f );

        mapShader = 0;
        if ( s_serveroptions.mappic.generic.name ) {
                mapShader = trap_R_RegisterShaderNoMip(
                        s_serveroptions.mappic.generic.name );
        }
        if ( !mapShader ) {
                mapShader = trap_R_RegisterShaderNoMip(
                        GAMESERVER_MISSING_MAP_SHOT );
        }
        Frontend_DrawCard( SERVEROPT_PLAYER_X, SERVEROPT_PLAYER_Y,
                SERVEROPT_PLAYER_WIDTH, SERVEROPT_PLAYER_HEIGHT, 1.0f, qfalse );
        Frontend_DrawCard( SERVEROPT_OPTION_X, SERVEROPT_OPTION_Y,
                SERVEROPT_OPTION_WIDTH, SERVEROPT_OPTION_HEIGHT, 1.0f, qfalse );
        Frontend_DrawText( SERVEROPT_PLAYER_X + 16, SERVEROPT_PLAYER_Y + 16,
                "Players", UI_LEFT | UI_SMALLFONT, serverOptionsMutedColor );
        Frontend_DrawText( SERVEROPT_OPTION_X + 16, SERVEROPT_OPTION_Y + 16,
                "Server options", UI_LEFT | UI_SMALLFONT, serverOptionsMutedColor );
        Frontend_DrawText( SERVEROPT_OPTION_X + 16, SERVEROPT_OPTION_Y + 48,
                "Track preview", UI_LEFT | UI_SMALLFONT, serverOptionsMutedColor );
        if ( mapShader ) {
                UI_DrawHandlePic( SERVEROPT_OPTION_X + SERVEROPT_OPTION_WIDTH -
                        16 - 112, SERVEROPT_OPTION_Y + 32, 112, 64, mapShader );
        }

        Menu_Draw( &s_serveroptions.menu );

        Frontend_DrawText( SERVEROPT_FRAME_X + 24, SERVEROPT_FRAME_Y + 384,
                "Select a player or option", UI_LEFT | UI_SMALLFONT,
                serverOptionsMutedColor );
        Frontend_DrawText( SERVEROPT_FRAME_X + SERVEROPT_FRAME_WIDTH - 24,
                SERVEROPT_FRAME_Y + 384, "Enter start   Esc back",
                UI_RIGHT | UI_SMALLFONT, serverOptionsMutedColor );
}


/*
=================
ServerOptions_MenuInit
=================
*/
#define OPTIONS_X	456

static void ServerOptions_MenuInit( qboolean multiplayer ) {
	int		y;
	int		n;
	qboolean	limitFieldAdded;
	qboolean	showGhostMode;
//	static char cirname[64];
	

	memset( &s_serveroptions, 0 ,sizeof(serveroptions_t) );
	s_serveroptions.multiplayer = multiplayer;
	// Store the raw GT_ value so that comparisons like (gametype >= GT_TEAM)
	// work correctly. gametype_remap2 now covers the full GT_ range 0..GT_KOTH.
	s_serveroptions.gametype = (int)trap_Cvar_VariableValue( "g_gametype" );
	showGhostMode = ServerOptions_IsRacingGametype( s_serveroptions.gametype );

	ServerOptions_Cache();

	s_serveroptions.menu.wrapAround = qtrue;
	s_serveroptions.menu.fullscreen = qtrue;
	s_serveroptions.menu.draw = ServerOptions_Draw;

	s_serveroptions.banner.generic.type			= MTYPE_BTEXT;
	s_serveroptions.banner.generic.x			= 320;
	s_serveroptions.banner.generic.y			= 16;
	s_serveroptions.banner.string  				= "GAME SERVER";
	s_serveroptions.banner.color  				= text_color_normal;
	s_serveroptions.banner.style  				= UI_CENTER;
    
	s_serveroptions.mappic.generic.type			= MTYPE_BITMAP;
	s_serveroptions.mappic.generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_serveroptions.mappic.generic.x			= 445;
	s_serveroptions.mappic.generic.y			= 80;
	s_serveroptions.mappic.width				= 170;
	s_serveroptions.mappic.height				= 96;
	s_serveroptions.mappic.errorpic				= GAMESERVER_MISSING_MAP_SHOT;
	s_serveroptions.mappic.generic.ownerdraw	= ServerOptions_LevelshotDraw;
	y = 272;

	limitFieldAdded = qfalse;

	if( s_serveroptions.gametype == GT_KOTH ) {
		s_serveroptions.flaglimit.generic.type		= MTYPE_FIELD;
		s_serveroptions.flaglimit.generic.name		= "Score Limit:";
		s_serveroptions.flaglimit.generic.flags		= QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.flaglimit.generic.x			= OPTIONS_X;
		s_serveroptions.flaglimit.generic.y			= y;
		s_serveroptions.flaglimit.generic.statusbar	= ServerOptions_StatusBar;
		s_serveroptions.flaglimit.field.widthInChars = 3;
		s_serveroptions.flaglimit.field.maxchars	= 3;

		limitFieldAdded = qtrue;
	}
	else if( s_serveroptions.gametype == GT_CTF || s_serveroptions.gametype == GT_CTF4 || s_serveroptions.gametype == GT_DOMINATION ) {
		s_serveroptions.flaglimit.generic.type		= MTYPE_FIELD;
		s_serveroptions.flaglimit.generic.name		= "Capture Limit:";
		s_serveroptions.flaglimit.generic.flags		= QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.flaglimit.generic.x			= OPTIONS_X;
		s_serveroptions.flaglimit.generic.y			= y;
		s_serveroptions.flaglimit.generic.statusbar	= ServerOptions_StatusBar;
		s_serveroptions.flaglimit.field.widthInChars = 3;
		s_serveroptions.flaglimit.field.maxchars	= 3;

		limitFieldAdded = qtrue;
	}
	else if( s_serveroptions.gametype == GT_RACING ||
			s_serveroptions.gametype == GT_RACING_DM ||
			s_serveroptions.gametype == GT_TEAM_RACING ||
			s_serveroptions.gametype == GT_TEAM_RACING_DM ) {

		s_serveroptions.fraglimit.generic.type		= MTYPE_FIELD;
		s_serveroptions.fraglimit.generic.name		= "Laps:";
		s_serveroptions.fraglimit.generic.flags		= QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.fraglimit.generic.x			= OPTIONS_X;
		s_serveroptions.fraglimit.generic.y			= y;
		s_serveroptions.fraglimit.generic.statusbar	= ServerOptions_StatusBar;
		s_serveroptions.fraglimit.field.widthInChars = 3;
		s_serveroptions.fraglimit.field.maxchars	= 3;

		limitFieldAdded = qtrue;
	}
	else if( s_serveroptions.gametype != GT_DERBY && s_serveroptions.gametype != GT_LCS && s_serveroptions.gametype != GT_SPRINT ) {

		s_serveroptions.fraglimit.generic.type		= MTYPE_FIELD;
		s_serveroptions.fraglimit.generic.name		= "Frag Limit:";
		s_serveroptions.fraglimit.generic.flags		= QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.fraglimit.generic.x			= OPTIONS_X;
		s_serveroptions.fraglimit.generic.y			= y;
		s_serveroptions.fraglimit.generic.statusbar	= ServerOptions_StatusBar;
		s_serveroptions.fraglimit.field.widthInChars = 3;
		s_serveroptions.fraglimit.field.maxchars	= 3;

		limitFieldAdded = qtrue;
	}

	if ( limitFieldAdded ) {
		y += BIGCHAR_HEIGHT+2;
	}

	if ( s_serveroptions.gametype == GT_KOTH ) {
		s_serveroptions.kothPtsTick.generic.type		= MTYPE_FIELD;
		s_serveroptions.kothPtsTick.generic.name		= "Tick Pts:";
		s_serveroptions.kothPtsTick.generic.flags		= QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.kothPtsTick.generic.x			= OPTIONS_X;
		s_serveroptions.kothPtsTick.generic.y			= y;
		s_serveroptions.kothPtsTick.generic.statusbar	= ServerOptions_StatusBar;
		s_serveroptions.kothPtsTick.field.widthInChars = 3;
		s_serveroptions.kothPtsTick.field.maxchars	= 3;
		y += BIGCHAR_HEIGHT+2;

		s_serveroptions.kothPtsCapture.generic.type	= MTYPE_FIELD;
		s_serveroptions.kothPtsCapture.generic.name	= "Capture Pts:";
		s_serveroptions.kothPtsCapture.generic.flags	= QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.kothPtsCapture.generic.x		= OPTIONS_X;
		s_serveroptions.kothPtsCapture.generic.y		= y;
		s_serveroptions.kothPtsCapture.generic.statusbar = ServerOptions_StatusBar;
		s_serveroptions.kothPtsCapture.field.widthInChars = 3;
		s_serveroptions.kothPtsCapture.field.maxchars	= 3;
		y += BIGCHAR_HEIGHT+2;

		s_serveroptions.kothPtsDefend.generic.type	= MTYPE_FIELD;
		s_serveroptions.kothPtsDefend.generic.name	= "Defend Pts:";
		s_serveroptions.kothPtsDefend.generic.flags	= QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.kothPtsDefend.generic.x		= OPTIONS_X;
		s_serveroptions.kothPtsDefend.generic.y		= y;
		s_serveroptions.kothPtsDefend.generic.statusbar = ServerOptions_StatusBar;
		s_serveroptions.kothPtsDefend.field.widthInChars = 3;
		s_serveroptions.kothPtsDefend.field.maxchars	= 3;
		y += BIGCHAR_HEIGHT+2;

		s_serveroptions.kothOvertime.generic.type     = MTYPE_RADIOBUTTON;
		s_serveroptions.kothOvertime.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.kothOvertime.generic.x        = OPTIONS_X;
		s_serveroptions.kothOvertime.generic.y        = y;
		s_serveroptions.kothOvertime.generic.name     = "Overtime:";
		y += BIGCHAR_HEIGHT+2;

		s_serveroptions.kothOvertimeHold.generic.type       = MTYPE_FIELD;
		s_serveroptions.kothOvertimeHold.generic.name       = "OT Hold (s):";
		s_serveroptions.kothOvertimeHold.generic.flags      = QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.kothOvertimeHold.generic.x          = OPTIONS_X;
		s_serveroptions.kothOvertimeHold.generic.y          = y;
		s_serveroptions.kothOvertimeHold.generic.statusbar  = ServerOptions_StatusBar;
		s_serveroptions.kothOvertimeHold.field.widthInChars = 3;
		s_serveroptions.kothOvertimeHold.field.maxchars     = 3;
		y += BIGCHAR_HEIGHT+2;
	}

	s_serveroptions.timelimit.generic.type       = MTYPE_FIELD;
	s_serveroptions.timelimit.generic.name       = "Time Limit:";
	s_serveroptions.timelimit.generic.flags      = QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_serveroptions.timelimit.generic.x	         = OPTIONS_X;
	s_serveroptions.timelimit.generic.y	         = y;
	s_serveroptions.timelimit.generic.statusbar  = ServerOptions_StatusBar;
	s_serveroptions.timelimit.field.widthInChars = 3;
	s_serveroptions.timelimit.field.maxchars     = 3;

	if( s_serveroptions.gametype >= GT_TEAM ) {
		y += BIGCHAR_HEIGHT+2;
		s_serveroptions.friendlyfire.generic.type     = MTYPE_RADIOBUTTON;
		s_serveroptions.friendlyfire.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.friendlyfire.generic.x	      = OPTIONS_X;
		s_serveroptions.friendlyfire.generic.y	      = y;
		s_serveroptions.friendlyfire.generic.name	  = "Friendly Fire:";
	}

	y += BIGCHAR_HEIGHT+2;
	s_serveroptions.pure.generic.type			= MTYPE_RADIOBUTTON;
	s_serveroptions.pure.generic.flags			= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_serveroptions.pure.generic.x				= OPTIONS_X;
	s_serveroptions.pure.generic.y				= y;
	s_serveroptions.pure.generic.name			= "Pure Server:";

	if ( trap_Cvar_VariableValue( "fs_unpure" ) ) {
		// ZTM: Don't let users think they can modify sv_pure, it won't work.
		s_serveroptions.pure.generic.flags |= QMF_GRAYED;
	}

        if ( s_serveroptions.gametype == GT_ELIMINATION ) {
                y += BIGCHAR_HEIGHT+2;
                s_serveroptions.eliminationWeapons.generic.type = MTYPE_RADIOBUTTON;
                s_serveroptions.eliminationWeapons.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
                s_serveroptions.eliminationWeapons.generic.x = OPTIONS_X;
                s_serveroptions.eliminationWeapons.generic.y = y;
                s_serveroptions.eliminationWeapons.generic.name = "Enable Weapons:";
        }

        y += BIGCHAR_HEIGHT+2;
        if ( showGhostMode ) {
                s_serveroptions.ghostOnly.generic.type                  = MTYPE_RADIOBUTTON;
                s_serveroptions.ghostOnly.generic.flags         = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
                s_serveroptions.ghostOnly.generic.x                     = OPTIONS_X;
                s_serveroptions.ghostOnly.generic.y                     = y;
                s_serveroptions.ghostOnly.generic.name          = "Ghost-Only Race:";
                s_serveroptions.ghostOnly.generic.id                    = ID_GHOST_ONLY;
                s_serveroptions.ghostOnly.generic.callback      = ServerOptions_Event;
                s_serveroptions.ghostOnly.generic.statusbar     = ServerOptions_GhostOnlyStatusBar;
        } else {
                s_serveroptions.ghostOnly.generic.type                  = MTYPE_RADIOBUTTON;
                s_serveroptions.ghostOnly.generic.flags         = QMF_INACTIVE|QMF_HIDDEN;
                s_serveroptions.ghostOnly.curvalue                         = 0;
        }

	if ( s_serveroptions.gametype == GT_DERBY || s_serveroptions.gametype == GT_LCS ) {
		y += BIGCHAR_HEIGHT+2;
		s_serveroptions.minPlayers.generic.type			= MTYPE_FIELD;
		s_serveroptions.minPlayers.generic.name			= "Min Players:";
		s_serveroptions.minPlayers.generic.flags		= QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.minPlayers.generic.x			= OPTIONS_X;
		s_serveroptions.minPlayers.generic.y			= y;
		s_serveroptions.minPlayers.generic.statusbar	= ServerOptions_StatusBar;
		s_serveroptions.minPlayers.field.widthInChars	= 2;
		s_serveroptions.minPlayers.field.maxchars		= 2;
	}

        n = 0;
	trackLengthUiCount = 0;
        if ( allowLength[0] ){
                track_length_list[n] = "Short";
		trackLengthValueByUiIndex[n] = 0;
                n++;
		trackLengthUiCount++;
        }
	if ( allowLength[1] ){
		track_length_list[n] = "Medium";
		trackLengthValueByUiIndex[n] = 1;
		n++;
		trackLengthUiCount++;
	}
	if ( allowLength[2] ){
		track_length_list[n] = "Long";
		trackLengthValueByUiIndex[n] = 2;
		n++;
		trackLengthUiCount++;
	}
	if ( trackLengthUiCount <= 0 ) {
		track_length_list[0] = "Short";
		trackLengthValueByUiIndex[0] = 0;
		trackLengthUiCount = 1;
		n = 1;
	}
	track_length_list[n] = 0;

	y += BIGCHAR_HEIGHT+2;
	s_serveroptions.trackLength.generic.type		= MTYPE_SPINCONTROL;
	s_serveroptions.trackLength.generic.id			= ID_TRACK_LENGTH;
	s_serveroptions.trackLength.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_serveroptions.trackLength.generic.x			= OPTIONS_X;
	s_serveroptions.trackLength.generic.y			= y;
	s_serveroptions.trackLength.generic.name		= "Track Length:";
	s_serveroptions.trackLength.itemnames			= (const char **)track_length_list;

	y += BIGCHAR_HEIGHT+2;
	s_serveroptions.reversed.generic.type			= MTYPE_SPINCONTROL;
	s_serveroptions.reversed.generic.id				= ID_TRACK_REVERSED;
	s_serveroptions.reversed.generic.flags			= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_serveroptions.reversed.generic.x				= OPTIONS_X;
	s_serveroptions.reversed.generic.y				= y;
	s_serveroptions.reversed.generic.name			= "Track Reversed:";
	s_serveroptions.reversed.itemnames				= reversed_list;

	if( s_serveroptions.multiplayer ) {
		y += BIGCHAR_HEIGHT+2;
		s_serveroptions.dedicated.generic.type		= MTYPE_SPINCONTROL;
		s_serveroptions.dedicated.generic.id		= ID_DEDICATED;
		s_serveroptions.dedicated.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		s_serveroptions.dedicated.generic.callback	= ServerOptions_Event;
		s_serveroptions.dedicated.generic.x			= OPTIONS_X;
		s_serveroptions.dedicated.generic.y			= y;
		s_serveroptions.dedicated.generic.name		= "Dedicated:";
		s_serveroptions.dedicated.itemnames			= dedicated_list;
	}

	if( s_serveroptions.multiplayer ) {
		y += BIGCHAR_HEIGHT+2;
		s_serveroptions.hostname.generic.type       = MTYPE_FIELD;
		s_serveroptions.hostname.generic.name       = "Hostname:";
		s_serveroptions.hostname.generic.flags      = QMF_SMALLFONT;
		s_serveroptions.hostname.generic.x          = OPTIONS_X;
		s_serveroptions.hostname.generic.y	        = y;
		s_serveroptions.hostname.field.widthInChars = 18;
		s_serveroptions.hostname.field.maxchars     = 64;
	}

if (s_serveroptions.gametype == GT_DOMINATION) {
    s_serveroptions.dominationScoreInterval.generic.type       = MTYPE_FIELD;
    s_serveroptions.dominationScoreInterval.generic.name       = "Score Interval (s):";
    s_serveroptions.dominationScoreInterval.generic.flags      = QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
    s_serveroptions.dominationScoreInterval.generic.x	         = OPTIONS_X;
    s_serveroptions.dominationScoreInterval.generic.y	         = y;
    s_serveroptions.dominationScoreInterval.field.widthInChars = 5;
    s_serveroptions.dominationScoreInterval.field.maxchars     = 5;

    y += BIGCHAR_HEIGHT+2;
    s_serveroptions.dominationCaptureDelay.generic.type       = MTYPE_FIELD;
    s_serveroptions.dominationCaptureDelay.generic.name       = "Capture Delay (s):";
    s_serveroptions.dominationCaptureDelay.generic.flags      = QMF_NUMBERSONLY|QMF_PULSEIFFOCUS|QMF_SMALLFONT;
    s_serveroptions.dominationCaptureDelay.generic.x	         = OPTIONS_X;
    s_serveroptions.dominationCaptureDelay.generic.y	         = y;
    s_serveroptions.dominationCaptureDelay.field.widthInChars = 4;
    s_serveroptions.dominationCaptureDelay.field.maxchars     = 4;

    y += BIGCHAR_HEIGHT+2;
    s_serveroptions.dominationSpawnStyle.generic.type  = MTYPE_SPINCONTROL;
    s_serveroptions.dominationSpawnStyle.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
    s_serveroptions.dominationSpawnStyle.generic.x     = OPTIONS_X;
    s_serveroptions.dominationSpawnStyle.generic.y     = y;
    s_serveroptions.dominationSpawnStyle.generic.name  = "Spawn Style:";
    s_serveroptions.dominationSpawnStyle.itemnames     = dtfspawn_list;

    y += BIGCHAR_HEIGHT+2;
    s_serveroptions.sigillocator.generic.type   = MTYPE_RADIOBUTTON;
    s_serveroptions.sigillocator.generic.flags  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
    s_serveroptions.sigillocator.generic.x      = OPTIONS_X;
    s_serveroptions.sigillocator.generic.y      = y;
    s_serveroptions.sigillocator.generic.name   = "Flag Locator:";
  }

	y = 80;
	s_serveroptions.botSkill.generic.type			= MTYPE_SPINCONTROL;
	s_serveroptions.botSkill.generic.flags			= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_serveroptions.botSkill.generic.name			= "Bot Skill:  ";
	s_serveroptions.botSkill.generic.x				= 32 + (strlen(s_serveroptions.botSkill.generic.name) + 2 ) * SMALLCHAR_WIDTH;
	s_serveroptions.botSkill.generic.y				= y;
	s_serveroptions.botSkill.itemnames				= botSkill_list;
	s_serveroptions.botSkill.curvalue				= 1;

	y += SMALLCHAR_HEIGHT + 2;

	y += ( 2 * SMALLCHAR_HEIGHT );
	s_serveroptions.player0.generic.type			= MTYPE_TEXT;
	s_serveroptions.player0.generic.flags			= QMF_SMALLFONT;
	s_serveroptions.player0.generic.x				= 32 + SMALLCHAR_WIDTH;
	s_serveroptions.player0.generic.y				= y;
	s_serveroptions.player0.color					= text_color_normal;
	s_serveroptions.player0.style					= UI_LEFT|UI_SMALLFONT;

	for( n = 0; n < PLAYER_SLOTS; n++ ) {
		s_serveroptions.playerType[n].generic.type		= MTYPE_SPINCONTROL;
		s_serveroptions.playerType[n].generic.flags		= QMF_SMALLFONT;
		s_serveroptions.playerType[n].generic.id		= ID_PLAYER_TYPE;
		s_serveroptions.playerType[n].generic.callback	= ServerOptions_Event;
		s_serveroptions.playerType[n].generic.x			= 32;
		s_serveroptions.playerType[n].generic.y			= y;
		s_serveroptions.playerType[n].itemnames			= playerType_list;

		s_serveroptions.playerName[n].generic.type		= MTYPE_TEXT;
		s_serveroptions.playerName[n].generic.flags		= QMF_SMALLFONT;
		s_serveroptions.playerName[n].generic.x			= 96;
		s_serveroptions.playerName[n].generic.y			= y;
		s_serveroptions.playerName[n].generic.callback	= ServerOptions_PlayerNameEvent;
		s_serveroptions.playerName[n].generic.id		= n;
		s_serveroptions.playerName[n].generic.ownerdraw	= PlayerName_Draw;
		s_serveroptions.playerName[n].color				= text_color_normal;
        
		s_serveroptions.playerName[n].style				= UI_SMALLFONT;
		s_serveroptions.playerName[n].string			= s_serveroptions.playerNameBuffers[n];
		s_serveroptions.playerName[n].generic.top		= s_serveroptions.playerName[n].generic.y;
		s_serveroptions.playerName[n].generic.bottom	= s_serveroptions.playerName[n].generic.y + SMALLCHAR_HEIGHT;
		s_serveroptions.playerName[n].generic.left		= s_serveroptions.playerName[n].generic.x - SMALLCHAR_HEIGHT/ 2;
		s_serveroptions.playerName[n].generic.right		= s_serveroptions.playerName[n].generic.x + 16 * SMALLCHAR_WIDTH;

		s_serveroptions.playerTeam[n].generic.type		= MTYPE_SPINCONTROL;
		s_serveroptions.playerTeam[n].generic.flags		= QMF_SMALLFONT;
		s_serveroptions.playerTeam[n].generic.x			= 240;
		s_serveroptions.playerTeam[n].generic.y			= y;
		s_serveroptions.playerTeam[n].itemnames			= playerTeam_twoTeam_list;

		y += ( SMALLCHAR_HEIGHT + 4 );
	}

	s_serveroptions.back.generic.type		= MTYPE_PTEXT;
	s_serveroptions.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_serveroptions.back.generic.x			= 20;
	s_serveroptions.back.generic.y			= 480 - 50;
	s_serveroptions.back.generic.id			= ID_BACK;
	s_serveroptions.back.generic.callback	= ServerOptions_Event; 
	s_serveroptions.back.string				= "< BACK";
	s_serveroptions.back.color				= text_color_normal;
	s_serveroptions.back.style				= UI_LEFT | UI_SMALLFONT;

	s_serveroptions.go.generic.type			= MTYPE_PTEXT;
	s_serveroptions.go.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_serveroptions.go.generic.x			= 640 - 20;
	s_serveroptions.go.generic.y			= 480 - 50;
	s_serveroptions.go.generic.id			= ID_GO;
	s_serveroptions.go.generic.callback		= ServerOptions_Event; 
	s_serveroptions.go.string				= "RACE!";
	s_serveroptions.go.color				= text_color_normal;
	s_serveroptions.go.style				= UI_RIGHT | UI_SMALLFONT;

	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.banner );

	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.mappic );

	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.botSkill );

	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.player0 );
	for( n = 0; n < PLAYER_SLOTS; n++ ) {
		if( n != 0 ) {
			Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.playerType[n] );
		}
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.playerName[n] );
		if( s_serveroptions.gametype >= GT_TEAM ) {
			Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.playerTeam[n] );
		}
	}

	if( s_serveroptions.gametype == GT_CTF || s_serveroptions.gametype == GT_CTF4 || s_serveroptions.gametype == GT_DOMINATION || s_serveroptions.gametype == GT_KOTH ) {
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.flaglimit );
	}
	else if( s_serveroptions.gametype != GT_DERBY && s_serveroptions.gametype != GT_LCS && s_serveroptions.gametype != GT_SPRINT ) {
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.fraglimit );
	}

	if( s_serveroptions.gametype == GT_KOTH ) {
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.kothPtsTick );
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.kothPtsCapture );
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.kothPtsDefend );
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.kothOvertime );
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.kothOvertimeHold );
	}

	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.timelimit );

	if( s_serveroptions.gametype >= GT_TEAM ) {
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.friendlyfire );
	}

	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.pure );

	if ( showGhostMode ) {
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.ghostOnly );
	}

	if( s_serveroptions.gametype == GT_ELIMINATION ) {
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.eliminationWeapons );
	}

	if( s_serveroptions.gametype == GT_DERBY || s_serveroptions.gametype == GT_LCS ) {
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.minPlayers );
	}

	if( s_serveroptions.gametype == GT_RACING || s_serveroptions.gametype == GT_RACING_DM
	|| s_serveroptions.gametype == GT_SPRINT || s_serveroptions.gametype == GT_TEAM_RACING || s_serveroptions.gametype == GT_TEAM_RACING_DM) {
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.trackLength );

		if ( reversable )
			Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.reversed );
	}

	if( s_serveroptions.multiplayer ) {
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.dedicated );
	}

	if( s_serveroptions.multiplayer ) {
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.hostname );
	}

	if (s_serveroptions.gametype == GT_DOMINATION) {
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.dominationScoreInterval );
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.dominationCaptureDelay );
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.dominationSpawnStyle );
		Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.sigillocator );
	}

	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.back );
	Menu_AddItem( &s_serveroptions.menu, &s_serveroptions.go );

	ServerOptions_SetMenuItems();
	ServerOptions_LayoutFrontend();
}

/*
=================
ServerOptions_Cache
=================
*/
void ServerOptions_Cache( void ) {

	trap_R_RegisterShaderNoMip( GAMESERVER_UNKNOWNMAP );
	trap_R_RegisterShaderNoMip( GAMESERVER_MISSING_MAP_SHOT );
}


/*
=================
UI_ServerOptionsMenu
=================
*/
static void UI_ServerOptionsMenu( qboolean multiplayer ) {
	ServerOptions_MenuInit( multiplayer );
	UI_PushMenu( &s_serveroptions.menu );
}



/*
=============================================================================

BOT SELECT MENU *****

=============================================================================
*/


#define BOTSELECT_BACK0			"menu/art/back_0"
#define BOTSELECT_BACK1			"menu/art/back_1"
#define BOTSELECT_ACCEPT0		"menu/art/accept_0"
#define BOTSELECT_ACCEPT1		"menu/art/accept_1"
#define BOTSELECT_SELECT		"menu/art/opponents_select"
#define BOTSELECT_SELECTED		"menu/art/opponents_selected"
#define BOTSELECT_ARROWS		"menu/art/gs_arrows_0"
#define BOTSELECT_ARROWSL		"menu/art/gs_arrows_l"
#define BOTSELECT_ARROWSR		"menu/art/gs_arrows_r"

#define PLAYERGRID_COLS			4
#define PLAYERGRID_ROWS			4
#define MAX_MODELSPERPAGE		(PLAYERGRID_ROWS * PLAYERGRID_COLS)

#define BOTSELECT_FRAME_X            24
#define BOTSELECT_FRAME_Y            20
#define BOTSELECT_FRAME_WIDTH        592
#define BOTSELECT_FRAME_HEIGHT       440
#define BOTSELECT_GRID_X             48
#define BOTSELECT_GRID_Y             108
#define BOTSELECT_CARD_WIDTH         120
#define BOTSELECT_CARD_HEIGHT        44
#define BOTSELECT_CARD_GAP           8
#define BOTSELECT_GRID_WIDTH         512
#define BOTSELECT_GRID_HEIGHT        232
#define BOTSELECT_ACTION_Y           420
#define BOTSELECT_ACTION_WIDTH       112
#define BOTSELECT_ACTION_HEIGHT      24
#define BOTSELECT_PAGE_WIDTH         96

static vec4_t botSelectTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t botSelectMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t botSelectAccentColor = UI_FRONTEND_COLOR_ACCENT;


typedef struct {
	menuframework_s	menu;

	menutext_s		banner;

	menubitmap_s	pics[MAX_MODELSPERPAGE];
	menubitmap_s	picbuttons[MAX_MODELSPERPAGE];
	menutext_s		picnames[MAX_MODELSPERPAGE];

	menubitmap_s	arrows;
	menubitmap_s	left;
	menubitmap_s	right;

	menutext_s		go;
	menutext_s		back;

	int				numBots;
	int				modelpage;
	int				numpages;
	int				selectedmodel;
	int				sortedBotNums[MAX_BOTS];
	char			boticons[MAX_MODELSPERPAGE][MAX_QPATH];
	char			botnames[MAX_MODELSPERPAGE][16];
} botSelectInfo_t;

static botSelectInfo_t	botSelectInfo;


/*
=================
UI_BotSelectMenu_SortCompare
=================
*/
static int QDECL UI_BotSelectMenu_SortCompare( const void *arg1, const void *arg2 ) {
	int			num1, num2;
	const char	*info1, *info2;
	const char	*name1, *name2;

	num1 = *(int *)arg1;
	num2 = *(int *)arg2;

	info1 = UI_GetBotInfoByNumber( num1 );
	info2 = UI_GetBotInfoByNumber( num2 );

	name1 = Info_ValueForKey( info1, "name" );
	name2 = Info_ValueForKey( info2, "name" );

	return Q_stricmp( name1, name2 );
}


/*
=================
UI_BotSelectMenu_BuildList
=================
*/
static void UI_BotSelectMenu_BuildList( void ) {
	int		n;

	botSelectInfo.modelpage = 0;
	botSelectInfo.numBots = UI_GetNumBots();
	botSelectInfo.numpages = botSelectInfo.numBots / MAX_MODELSPERPAGE;
	if( botSelectInfo.numBots % MAX_MODELSPERPAGE ) {
		botSelectInfo.numpages++;
	}

	// initialize the array
	for( n = 0; n < botSelectInfo.numBots; n++ ) {
		botSelectInfo.sortedBotNums[n] = n;
	}

	// now sort it
	qsort( botSelectInfo.sortedBotNums, botSelectInfo.numBots, sizeof(botSelectInfo.sortedBotNums[0]), UI_BotSelectMenu_SortCompare );
}


/*
=================
ServerPlayerIcon
=================
*/
static void ServerPlayerIcon( const char *modelAndSkin, char *iconName, int iconNameMaxSize ) {
	char	*skin;
	char	model[MAX_QPATH];

	Q_strncpyz( model, modelAndSkin, sizeof(model));
	skin = strrchr( model, '/' );
	if ( skin ) {
		*skin++ = '\0';
	}
	else {

		skin = "red";

	}

	Com_sprintf(iconName, iconNameMaxSize, "models/players/%s/icon_%s.tga", model, skin );

	if( !trap_R_RegisterShaderNoMip( iconName ) ) {
		Com_sprintf(iconName, iconNameMaxSize, "models/players/%s/icon_red.tga", model );
	}

}


/*
=================
UI_BotSelectMenu_UpdateGrid
=================
*/
static void UI_BotSelectMenu_UpdateGrid( void ) {
	const char	*info;
	int			i;
    int			j;

	j = botSelectInfo.modelpage * MAX_MODELSPERPAGE;
	for( i = 0; i < (PLAYERGRID_ROWS * PLAYERGRID_COLS); i++, j++) {
		if( j < botSelectInfo.numBots ) { 
			info = UI_GetBotInfoByNumber( botSelectInfo.sortedBotNums[j] );
			ServerPlayerIcon( Info_ValueForKey( info, "model" ), botSelectInfo.boticons[i], MAX_QPATH );
			Q_strncpyz( botSelectInfo.botnames[i], Info_ValueForKey( info, "name" ), 16 );
			Q_CleanStr( botSelectInfo.botnames[i] );
 			botSelectInfo.pics[i].generic.name = botSelectInfo.boticons[i];
			if( BotAlreadySelected( botSelectInfo.botnames[i] ) ) {
				botSelectInfo.picnames[i].color = color_red;
			}
			else {

				botSelectInfo.picnames[i].color = text_color_normal;

			}
			botSelectInfo.picbuttons[i].generic.flags &= ~QMF_INACTIVE;
		}
		else {
			// dead slot
 			botSelectInfo.pics[i].generic.name         = NULL;
			botSelectInfo.picbuttons[i].generic.flags |= QMF_INACTIVE;
			botSelectInfo.botnames[i][0] = 0;
		}

 		botSelectInfo.pics[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		botSelectInfo.pics[i].shader               = 0;
 		botSelectInfo.picbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	// set selected model
	i = botSelectInfo.selectedmodel % MAX_MODELSPERPAGE;
	botSelectInfo.pics[i].generic.flags |= QMF_HIGHLIGHT;
	botSelectInfo.picbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;

	if( botSelectInfo.numpages > 1 ) {
		if( botSelectInfo.modelpage > 0 ) {
			botSelectInfo.left.generic.flags &= ~QMF_INACTIVE;
		}
		else {
			botSelectInfo.left.generic.flags |= QMF_INACTIVE;
		}

		if( botSelectInfo.modelpage < (botSelectInfo.numpages - 1) ) {
			botSelectInfo.right.generic.flags &= ~QMF_INACTIVE;
		}
		else {
			botSelectInfo.right.generic.flags |= QMF_INACTIVE;
		}
	}
	else {
		// hide left/right markers
		botSelectInfo.left.generic.flags |= QMF_INACTIVE;
		botSelectInfo.right.generic.flags |= QMF_INACTIVE;
	}
}


/*
=================
UI_BotSelectMenu_Default
=================
*/
static void UI_BotSelectMenu_Default( char *bot ) {
	const char	*botInfo;
	const char	*test;
	int			n;
	int			i;

	for( n = 0; n < botSelectInfo.numBots; n++ ) {
		botInfo = UI_GetBotInfoByNumber( n );
		test = Info_ValueForKey( botInfo, "name" );
		if( Q_stricmp( bot, test ) == 0 ) {
			break;
		}
	}
	if( n == botSelectInfo.numBots ) {
		botSelectInfo.selectedmodel = 0;
		return;
	}

	for( i = 0; i < botSelectInfo.numBots; i++ ) {
		if( botSelectInfo.sortedBotNums[i] == n ) {
			break;
		}
	}
	if( i == botSelectInfo.numBots ) {
		botSelectInfo.selectedmodel = 0;
		return;
	}

	botSelectInfo.selectedmodel = i;
}


/*
=================
UI_BotSelectMenu_LeftEvent
=================
*/
static void UI_BotSelectMenu_LeftEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}
	if( botSelectInfo.modelpage > 0 ) {
		botSelectInfo.modelpage--;
		botSelectInfo.selectedmodel = botSelectInfo.modelpage * MAX_MODELSPERPAGE;
		UI_BotSelectMenu_UpdateGrid();
	}
}


/*
=================
UI_BotSelectMenu_RightEvent
=================
*/
static void UI_BotSelectMenu_RightEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}
	if( botSelectInfo.modelpage < botSelectInfo.numpages - 1 ) {
		botSelectInfo.modelpage++;
		botSelectInfo.selectedmodel = botSelectInfo.modelpage * MAX_MODELSPERPAGE;
		UI_BotSelectMenu_UpdateGrid();
	}
}


/*
=================
UI_BotSelectMenu_BotEvent
=================
*/
static void UI_BotSelectMenu_BotEvent( void* ptr, int event ) {
	int		i;

	if( event != QM_ACTIVATED ) {
		return;
	}

	for( i = 0; i < (PLAYERGRID_ROWS * PLAYERGRID_COLS); i++ ) {
 		botSelectInfo.pics[i].generic.flags &= ~QMF_HIGHLIGHT;
 		botSelectInfo.picbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	// set selected
	i = ((menucommon_s*)ptr)->id;
	botSelectInfo.pics[i].generic.flags |= QMF_HIGHLIGHT;
	botSelectInfo.picbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;
	botSelectInfo.selectedmodel = botSelectInfo.modelpage * MAX_MODELSPERPAGE + i;
}


/*
=================
UI_BotSelectMenu_BackEvent
=================
*/
static void UI_BotSelectMenu_BackEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}
	UI_PopMenu();
}


/*
=================
UI_BotSelectMenu_SelectEvent
=================
*/
static void UI_BotSelectMenu_SelectEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	/* Commit the selection while returning from the modal menu.  This used to
	 * happen from the level-shot ownerdraw, but that widget is hidden by the
	 * modern server-options layout and therefore never ran. */
	Q_strncpyz( s_serveroptions.playerNameBuffers[s_serveroptions.newBotIndex],
		botSelectInfo.botnames[botSelectInfo.selectedmodel % MAX_MODELSPERPAGE],
		16 );
	UI_PopMenu();
}


/*
=================
UI_BotSelectMenu_Cache
=================
*/
void UI_BotSelectMenu_Cache( void ) {
	trap_R_RegisterShaderNoMip( BOTSELECT_BACK0 );
	trap_R_RegisterShaderNoMip( BOTSELECT_BACK1 );
	trap_R_RegisterShaderNoMip( BOTSELECT_ACCEPT0 );
	trap_R_RegisterShaderNoMip( BOTSELECT_ACCEPT1 );
	trap_R_RegisterShaderNoMip( BOTSELECT_SELECT );
	trap_R_RegisterShaderNoMip( BOTSELECT_SELECTED );
	trap_R_RegisterShaderNoMip( BOTSELECT_ARROWS );
	trap_R_RegisterShaderNoMip( BOTSELECT_ARROWSL );
	trap_R_RegisterShaderNoMip( BOTSELECT_ARROWSR );
}

static void UI_BotSelectMenu_DrawAction( void *self ) {
        menucommon_s *item;
        qboolean focus;
        const char *label;

        item = (menucommon_s *)self;
        focus = ( Menu_ItemAtCursor( item->parent ) == item );
        if ( item == (menucommon_s *)&botSelectInfo.left ) {
                label = "Prev";
        } else if ( item == (menucommon_s *)&botSelectInfo.right ) {
                label = "Next";
        } else if ( item == (menucommon_s *)&botSelectInfo.go ) {
                label = "Select";
        } else {
                label = "Back";
        }

        Frontend_DrawButton( item->left, item->top,
                item->right - item->left, item->bottom - item->top,
                label, 1.0f, focus, UI_FRONTEND_TEXT_CENTER );
}

static void UI_BotSelectMenu_DrawBot( void *self ) {
        menubitmap_s *button;
        int index;
        int column;
        int row;
        int x;
        int y;
        int absoluteIndex;
        qboolean focus;
        qboolean selected;
        qhandle_t shader;
        char name[32];

        button = (menubitmap_s *)self;
        index = button->generic.id;
        if ( index < 0 || index >= MAX_MODELSPERPAGE ||
             !botSelectInfo.botnames[index][0] ) {
                return;
        }

        column = index % PLAYERGRID_COLS;
        row = index / PLAYERGRID_COLS;
        x = BOTSELECT_GRID_X + column *
                ( BOTSELECT_CARD_WIDTH + BOTSELECT_CARD_GAP );
        y = BOTSELECT_GRID_Y + row *
                ( BOTSELECT_CARD_HEIGHT + BOTSELECT_CARD_GAP );
        absoluteIndex = botSelectInfo.modelpage * MAX_MODELSPERPAGE + index;
        focus = ( Menu_ItemAtCursor( button->generic.parent ) == button );
        selected = ( absoluteIndex == botSelectInfo.selectedmodel );

        Frontend_DrawCard( x, y, BOTSELECT_CARD_WIDTH, BOTSELECT_CARD_HEIGHT,
                1.0f, focus || selected );
        shader = trap_R_RegisterShaderNoMip( botSelectInfo.boticons[index] );
        if ( shader ) {
                UI_DrawHandlePic( x + 6, y + 4, 36, 36, shader );
        }

        Q_strncpyz( name, botSelectInfo.botnames[index], sizeof( name ) );
        StartServer_FitText( name, sizeof( name ), name, 66 );
        Frontend_DrawText( x + 50, y + 14, name,
                UI_LEFT | UI_SMALLFONT,
                focus || selected ? botSelectAccentColor : botSelectTextColor );
}

static void UI_BotSelectMenu_Draw( void ) {
        vec4_t scrimColor = UI_FRONTEND_COLOR_SCRIM;
        char pageText[32];

        Frontend_DrawBackground( scrimColor );
        Frontend_DrawPanel( BOTSELECT_FRAME_X, BOTSELECT_FRAME_Y,
                BOTSELECT_FRAME_WIDTH, BOTSELECT_FRAME_HEIGHT, 1.0f,
                UI_FRONTEND_STYLE_FRAME );
        Frontend_DrawText( BOTSELECT_FRAME_X + 24, BOTSELECT_FRAME_Y + 24,
                "Select bot", UI_LEFT | UI_BIGFONT, botSelectTextColor );
        Frontend_DrawText( BOTSELECT_FRAME_X + 24, BOTSELECT_FRAME_Y + 48,
                "Choose a driver for this player", UI_LEFT | UI_SMALLFONT,
                botSelectMutedColor );
        Frontend_DrawStatusChip( BOTSELECT_FRAME_X + BOTSELECT_FRAME_WIDTH - 104,
                BOTSELECT_FRAME_Y + 26, "Roster", botSelectAccentColor, 1.0f );

        Frontend_DrawCard( BOTSELECT_GRID_X - 8, BOTSELECT_GRID_Y - 12,
                BOTSELECT_GRID_WIDTH, BOTSELECT_GRID_HEIGHT, 1.0f, qfalse );
        Frontend_DrawText( BOTSELECT_GRID_X + 8, BOTSELECT_GRID_Y - 26,
                "Available drivers", UI_LEFT | UI_SMALLFONT,
                botSelectMutedColor );

        Menu_Draw( &botSelectInfo.menu );

        Com_sprintf( pageText, sizeof( pageText ), "Page %d / %d",
                botSelectInfo.numpages ? botSelectInfo.modelpage + 1 : 0,
                botSelectInfo.numpages );
        Frontend_DrawText( BOTSELECT_FRAME_X + BOTSELECT_FRAME_WIDTH / 2,
                BOTSELECT_FRAME_Y + 368, pageText,
                UI_CENTER | UI_SMALLFONT, botSelectAccentColor );
        Frontend_DrawText( BOTSELECT_FRAME_X + 24, BOTSELECT_FRAME_Y + 384,
                "Select a driver   Enter accept   Esc back",
                UI_LEFT | UI_SMALLFONT, botSelectMutedColor );
}

static void UI_BotSelectMenu_Layout( void ) {
        int i;
        int column;
        int row;
        int x;
        int y;

        botSelectInfo.banner.generic.flags |= QMF_HIDDEN | QMF_INACTIVE;
        botSelectInfo.arrows.generic.flags |= QMF_HIDDEN | QMF_INACTIVE;

        for ( i = 0; i < MAX_MODELSPERPAGE; i++ ) {
                column = i % PLAYERGRID_COLS;
                row = i / PLAYERGRID_COLS;
                x = BOTSELECT_GRID_X + column *
                        ( BOTSELECT_CARD_WIDTH + BOTSELECT_CARD_GAP );
                y = BOTSELECT_GRID_Y + row *
                        ( BOTSELECT_CARD_HEIGHT + BOTSELECT_CARD_GAP );

                botSelectInfo.pics[i].generic.flags |= QMF_HIDDEN | QMF_INACTIVE;
                botSelectInfo.picnames[i].generic.flags |= QMF_HIDDEN | QMF_INACTIVE;
                botSelectInfo.picbuttons[i].generic.flags |= QMF_NODEFAULTINIT;
                botSelectInfo.picbuttons[i].generic.x = x;
                botSelectInfo.picbuttons[i].generic.y = y;
                botSelectInfo.picbuttons[i].generic.left = x;
                botSelectInfo.picbuttons[i].generic.top = y;
                botSelectInfo.picbuttons[i].generic.right = x + BOTSELECT_CARD_WIDTH;
                botSelectInfo.picbuttons[i].generic.bottom = y + BOTSELECT_CARD_HEIGHT;
                botSelectInfo.picbuttons[i].width = BOTSELECT_CARD_WIDTH;
                botSelectInfo.picbuttons[i].height = BOTSELECT_CARD_HEIGHT;
                botSelectInfo.picbuttons[i].generic.ownerdraw =
                        UI_BotSelectMenu_DrawBot;
        }

        botSelectInfo.left.generic.flags |= QMF_NODEFAULTINIT;
        botSelectInfo.left.generic.left = 220;
        botSelectInfo.left.generic.top = BOTSELECT_ACTION_Y;
        botSelectInfo.left.generic.right = 220 + BOTSELECT_PAGE_WIDTH;
        botSelectInfo.left.generic.bottom = BOTSELECT_ACTION_Y + BOTSELECT_ACTION_HEIGHT;
        botSelectInfo.left.generic.ownerdraw = UI_BotSelectMenu_DrawAction;

        botSelectInfo.right.generic.flags |= QMF_NODEFAULTINIT;
        botSelectInfo.right.generic.left = 324;
        botSelectInfo.right.generic.top = BOTSELECT_ACTION_Y;
        botSelectInfo.right.generic.right = 324 + BOTSELECT_PAGE_WIDTH;
        botSelectInfo.right.generic.bottom = BOTSELECT_ACTION_Y + BOTSELECT_ACTION_HEIGHT;
        botSelectInfo.right.generic.ownerdraw = UI_BotSelectMenu_DrawAction;

        botSelectInfo.back.generic.flags |= QMF_NODEFAULTINIT;
        botSelectInfo.back.generic.left = 40;
        botSelectInfo.back.generic.top = BOTSELECT_ACTION_Y;
        botSelectInfo.back.generic.right = 40 + BOTSELECT_ACTION_WIDTH;
        botSelectInfo.back.generic.bottom = BOTSELECT_ACTION_Y + BOTSELECT_ACTION_HEIGHT;
        botSelectInfo.back.generic.ownerdraw = UI_BotSelectMenu_DrawAction;

        botSelectInfo.go.generic.flags |= QMF_NODEFAULTINIT;
        botSelectInfo.go.generic.left = BOTSELECT_FRAME_X + BOTSELECT_FRAME_WIDTH -
                24 - BOTSELECT_ACTION_WIDTH;
        botSelectInfo.go.generic.top = BOTSELECT_ACTION_Y;
        botSelectInfo.go.generic.right = BOTSELECT_FRAME_X + BOTSELECT_FRAME_WIDTH - 24;
        botSelectInfo.go.generic.bottom = BOTSELECT_ACTION_Y + BOTSELECT_ACTION_HEIGHT;
        botSelectInfo.go.generic.ownerdraw = UI_BotSelectMenu_DrawAction;
}


static void UI_BotSelectMenu_Init( char *bot ) {
	int		i, j, k;
	int		x, y;

	memset( &botSelectInfo, 0 ,sizeof(botSelectInfo) );
	botSelectInfo.menu.wrapAround = qtrue;
	botSelectInfo.menu.fullscreen = qtrue;
	botSelectInfo.menu.draw = UI_BotSelectMenu_Draw;

	UI_BotSelectMenu_Cache();

	botSelectInfo.banner.generic.type	= MTYPE_BTEXT;
	botSelectInfo.banner.generic.x		= 320;
	botSelectInfo.banner.generic.y		= 16;
	botSelectInfo.banner.string			= "SELECT BOT";
	botSelectInfo.banner.color			= color_white;
	botSelectInfo.banner.style			= UI_CENTER;

	y = BOTSELECT_GRID_Y;
	for( i = 0, k = 0; i < PLAYERGRID_ROWS; i++) {
		x = BOTSELECT_GRID_X;
		for( j = 0; j < PLAYERGRID_COLS; j++, k++ ) {
			botSelectInfo.pics[k].generic.type				= MTYPE_BITMAP;
			botSelectInfo.pics[k].generic.flags				= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
			botSelectInfo.pics[k].generic.x					= x;
			botSelectInfo.pics[k].generic.y					= y;
 			botSelectInfo.pics[k].generic.name				= botSelectInfo.boticons[k];
			botSelectInfo.pics[k].width						= 64;
			botSelectInfo.pics[k].height					= 64;
			botSelectInfo.pics[k].focuspic					= BOTSELECT_SELECTED;
			botSelectInfo.pics[k].focuscolor				= colorRed;

			botSelectInfo.picbuttons[k].generic.type		= MTYPE_BITMAP;
			botSelectInfo.picbuttons[k].generic.flags		= QMF_LEFT_JUSTIFY|QMF_NODEFAULTINIT|QMF_PULSEIFFOCUS;
			botSelectInfo.picbuttons[k].generic.callback	= UI_BotSelectMenu_BotEvent;
			botSelectInfo.picbuttons[k].generic.id			= k;
			botSelectInfo.picbuttons[k].generic.x			= x - 16;
			botSelectInfo.picbuttons[k].generic.y			= y - 16;
			botSelectInfo.picbuttons[k].generic.left		= x;
			botSelectInfo.picbuttons[k].generic.top			= y;
			botSelectInfo.picbuttons[k].generic.right		= x + 64;
			botSelectInfo.picbuttons[k].generic.bottom		= y + 64;
			botSelectInfo.picbuttons[k].width				= 128;
			botSelectInfo.picbuttons[k].height				= 128;
			botSelectInfo.picbuttons[k].focuspic			= BOTSELECT_SELECT;
			botSelectInfo.picbuttons[k].focuscolor			= colorRed;

			botSelectInfo.picnames[k].generic.type			= MTYPE_TEXT;
			botSelectInfo.picnames[k].generic.flags			= QMF_SMALLFONT;
			botSelectInfo.picnames[k].generic.x				= x + 32;
			botSelectInfo.picnames[k].generic.y				= y + 64;
			botSelectInfo.picnames[k].string				= botSelectInfo.botnames[k];
			botSelectInfo.picnames[k].color					= text_color_normal;
			botSelectInfo.picnames[k].style					= UI_CENTER|UI_SMALLFONT;

			x += (64 + 6);
		}
		y += (64 + SMALLCHAR_HEIGHT + 6);
	}

	botSelectInfo.arrows.generic.type		= MTYPE_BITMAP;
	botSelectInfo.arrows.generic.name		= BOTSELECT_ARROWS;
	botSelectInfo.arrows.generic.flags		= QMF_INACTIVE;
	botSelectInfo.arrows.generic.x			= 260;
	botSelectInfo.arrows.generic.y			= 440;
	botSelectInfo.arrows.width				= 128;
	botSelectInfo.arrows.height				= 32;

	botSelectInfo.left.generic.type			= MTYPE_BITMAP;
	botSelectInfo.left.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	botSelectInfo.left.generic.callback		= UI_BotSelectMenu_LeftEvent;
	botSelectInfo.left.generic.x			= 260;
	botSelectInfo.left.generic.y			= 440;
	botSelectInfo.left.width  				= 64;
	botSelectInfo.left.height  				= 32;
	botSelectInfo.left.focuspic				= BOTSELECT_ARROWSL;

	botSelectInfo.right.generic.type	    = MTYPE_BITMAP;
	botSelectInfo.right.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	botSelectInfo.right.generic.callback	= UI_BotSelectMenu_RightEvent;
	botSelectInfo.right.generic.x			= 321;
	botSelectInfo.right.generic.y			= 440;
	botSelectInfo.right.width  				= 64;
	botSelectInfo.right.height  		    = 32;
	botSelectInfo.right.focuspic			= BOTSELECT_ARROWSR;

	botSelectInfo.back.generic.type		= MTYPE_PTEXT;
	botSelectInfo.back.generic.name		= BOTSELECT_BACK0;
	botSelectInfo.back.generic.flags	= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	botSelectInfo.back.generic.x		= 20;
	botSelectInfo.back.generic.y		= 480 - 50;
	botSelectInfo.back.generic.callback	= UI_BotSelectMenu_BackEvent; 
	botSelectInfo.back.string			= "< BACK";
	botSelectInfo.back.color			= text_color_normal;
	botSelectInfo.back.style			= UI_LEFT | UI_SMALLFONT;

	botSelectInfo.go.generic.type		= MTYPE_PTEXT;
	botSelectInfo.go.generic.name		= BOTSELECT_ACCEPT0;
	botSelectInfo.go.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	botSelectInfo.go.generic.x			= 640 - 20;
	botSelectInfo.go.generic.y			= 480 - 50;
	botSelectInfo.go.generic.callback	= UI_BotSelectMenu_SelectEvent; 
	botSelectInfo.go.string				= "ACCEPT";
	botSelectInfo.go.color				= text_color_normal;
	botSelectInfo.go.style				= UI_RIGHT | UI_SMALLFONT;

	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.banner );
	for( i = 0; i < MAX_MODELSPERPAGE; i++ ) {
		Menu_AddItem( &botSelectInfo.menu,	&botSelectInfo.pics[i] );
		Menu_AddItem( &botSelectInfo.menu,	&botSelectInfo.picbuttons[i] );
		Menu_AddItem( &botSelectInfo.menu,	&botSelectInfo.picnames[i] );
	}
	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.arrows );
	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.back );
	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.left );
	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.right );
	Menu_AddItem( &botSelectInfo.menu, &botSelectInfo.go );
	UI_BotSelectMenu_Layout();

	UI_BotSelectMenu_BuildList();
	UI_BotSelectMenu_Default( bot );
	botSelectInfo.modelpage = botSelectInfo.selectedmodel / MAX_MODELSPERPAGE;
	UI_BotSelectMenu_UpdateGrid();
}


/*
=================
UI_BotSelectMenu
=================
*/
void UI_BotSelectMenu( char *bot ) {
	UI_BotSelectMenu_Init( bot );
	UI_PushMenu( &botSelectInfo.menu );
}
