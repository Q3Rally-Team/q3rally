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
//
/*
=======================================================================

MAIN MENU

=======================================================================
*/


#include "ui_local.h"


#define ID_SINGLEPLAYER                 10
#define ID_MULTIPLAYER                  11
#define ID_SETUP                        12
#define ID_DEMOS                        13
#define ID_CINEMATICS                   14
#define ID_PROFILE_ACTION               15

#define ID_MODS                         16
#define ID_GARAGE                       17
#define ID_EXIT                         18

#define MAIN_BANNER_MODEL               "models/mapobjects/q3rtitle/q3rtitle.md3"
#define MAIN_MENU_VERTICAL_SPACING      50
#define MAX_PLAYERMODELS                256
// END


typedef struct {
        menuframework_s menu;

        menutext_s              singleplayer;
        menutext_s              multiplayer;
        menutext_s              setup;
        menutext_s              demos;
        menutext_s              cinematics;
        menutext_s              mods;
        menutext_s              garage;
        menutext_s              exit;
        menutext_s              profileAction;
        menutext_s              profileInfoLine1;
        menutext_s              profileInfoLine2;
        char                    profileRankLine[64];
        char                    profilePointsLine[64];

        menutext_s              banner;
        menubitmap_s            carlogo;

        playerInfo_t    playerinfo;
        char                    modelskin[MAX_QPATH];
        char                    rimskin[MAX_QPATH];
        char                    headskin[MAX_QPATH];

} mainmenu_t;


static mainmenu_t s_main;
static vec4_t s_profileActionColor;

static void MainMenu_UpdateProfileTexts( void ) {
        const profile_stats_t *activeProfileStats;
        profile_rank_t activeRank;
        const char *activeProfileName;
        qboolean hasActiveProfile;

        activeProfileName = UI_Profile_GetActiveName();
        hasActiveProfile = UI_Profile_HasActiveProfile();
        s_main.profileAction.string = ( hasActiveProfile && activeProfileName && activeProfileName[0] ) ? (char *)activeProfileName : "CREATE";

        activeProfileStats = UI_Profile_GetActiveStats();
        if ( activeProfileStats && UI_Profile_GetRank( activeProfileStats, &activeRank ) && activeRank.current && activeRank.current->name ) {
                Com_sprintf( s_main.profileRankLine, sizeof( s_main.profileRankLine ), "RANK: %s", activeRank.current->name );
                Com_sprintf( s_main.profilePointsLine, sizeof( s_main.profilePointsLine ), "POINTS: %d", activeProfileStats->playerScore );
        } else {
                Q_strncpyz( s_main.profileRankLine, "RANK: -", sizeof( s_main.profileRankLine ) );
                Q_strncpyz( s_main.profilePointsLine, "POINTS: 0", sizeof( s_main.profilePointsLine ) );
        }

        s_main.profileInfoLine1.string = s_main.profileRankLine;
        s_main.profileInfoLine2.string = s_main.profilePointsLine;
}

/*
=================
MainMenu_UpdateModel
=================
*/
static void MainMenu_UpdateModel( void )
{
        vec3_t  viewangles;
        vec3_t  moveangles;
        char    plate[MAX_QPATH];

        VectorClear( viewangles );
        VectorClear( moveangles );

        trap_Cvar_VariableStringBuffer( "plate", plate, sizeof( plate ) );
        UI_PlayerInfo_SetModel( &s_main.playerinfo, s_main.modelskin, s_main.rimskin, s_main.headskin, plate);
        UI_PlayerInfo_SetInfo( &s_main.playerinfo, LEGS_IDLE, TORSO_STAND, viewangles, moveangles, WP_NONE, qfalse );
}


/*
=================
MainMenu_DrawPlayer
=================
*/
static void MainMenu_DrawPlayer( void *self ) {
        menubitmap_s    *b;

        uis.mainMenu = 1;

        b = (menubitmap_s*) self;
        UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, &s_main.playerinfo, uis.realtime );
}

/*
=================
MainMenu_BuildList
=================
*/
static void MainMenu_BuildList( void )
{
        int             numItems;
        int             car, skin;
        char    cars[MAX_PLAYERMODELS][MAX_QPATH];
        char    carName[MAX_QPATH];
        char    skinName[MAX_QPATH];

        // get car list
        numItems = UI_BuildFileList("models/players", "md3", "body", qtrue, qtrue, qfalse, 0, cars);

        // choose one from list randomly
        if (numItems){
               car = UI_RandomInt( numItems );
                Q_strncpyz(carName, cars[car], sizeof(carName));
        }
        else {
                Q_strncpyz(carName, DEFAULT_MODEL, sizeof(carName));
        }

        // get skins for the choosen car
        numItems = UI_BuildFileList( va("models/players/%s", carName), "skin", "", qtrue, qfalse, qfalse, 0, cars);

        // choose a skin from the list randomly
        if (numItems){
               skin = UI_RandomInt( numItems );
                Q_strncpyz(skinName, cars[skin], sizeof(skinName));
        }
        else {
                Q_strncpyz(skinName, DEFAULT_SKIN, sizeof(skinName));
        }

        // FIXME: choose rim randomly?
        Com_sprintf(s_main.modelskin, sizeof(s_main.modelskin), "%s/%s", carName, skinName);
}


/*
=================
MainMenu_Update
=================
*/
void MainMenu_Update( void ){

        MainMenu_BuildList();

        trap_Cvar_VariableStringBuffer( "rim", s_main.rimskin, sizeof( s_main.rimskin ) );
        trap_Cvar_VariableStringBuffer( "head", s_main.headskin, sizeof( s_main.headskin ) );
       
        MainMenu_UpdateModel();
}

/*
=================
MainMenu_ExitAction
=================
*/
static void MainMenu_ExitAction( qboolean result ) {
        if( !result ) {
                return;
        }
        UI_PopMenu();

        UI_Rally_CreditMenu();

        UI_CreditMenu();
}



/*
=================
Main_MenuEvent
=================
*/
void Main_MenuEvent (void* ptr, int event) {
        if( event != QM_ACTIVATED ) {
                return;
        }


        switch( ((menucommon_s*)ptr)->id ) {

        case ID_SINGLEPLAYER:
        case ID_MULTIPLAYER:
        case ID_SETUP:
        case ID_GARAGE:
        case ID_DEMOS:
        case ID_CINEMATICS:
        case ID_MODS:
                s_main.menu.transitionMenu = ((menucommon_s*)ptr)->id;
                uis.transitionOut = uis.realtime;
                break;

        case ID_PROFILE_ACTION:
                UI_ProfileOverlay_Open( qfalse );
                break;

        case ID_EXIT:
                UI_ConfirmMenu( "EXIT GAME?", 0, MainMenu_ExitAction );
                break;
        }
}

/*
===============
MainMenu_ChangeMenu
===============
*/
void MainMenu_ChangeMenu( int menuId ){

        switch( menuId) {
        case ID_SINGLEPLAYER:
                UI_StartServerMenu( qfalse );
                break;

        case ID_MULTIPLAYER:
                UI_ArenaServersMenu();
                break;

        case ID_SETUP:
                uis.mainMenu = 0;
                UI_SetupMenu();
                break;

        case ID_GARAGE:
                UI_BotsMenu();
                break;

        case ID_DEMOS:
                UI_DemosMenu();
                break;

        case ID_CINEMATICS:
                UI_CinematicsMenu();
                break;

        case ID_MODS:
                UI_ModsMenu();
                break;
        }
}


/*
===============
MainMenu_RunTransition
===============
*/
void MainMenu_RunTransition( float frac ) {
        vec4_t profileActionColor;

        uis.text_color[0] = text_color_normal[0];
        uis.text_color[1] = text_color_normal[1];
        uis.text_color[2] = text_color_normal[2];
        uis.text_color[3] = text_color_normal[3] * frac;

        s_main.banner.color = uis.text_color;

        s_main.singleplayer.color = uis.text_color;
        s_main.multiplayer.color = uis.text_color;
        s_main.setup.color = uis.text_color;
        s_main.garage.color = uis.text_color;
        s_main.cinematics.color = uis.text_color;
        s_main.demos.color = uis.text_color;
        s_main.mods.color = uis.text_color;
        s_main.exit.color = uis.text_color;

        s_profileActionColor[0] = color_red[0];
        s_profileActionColor[1] = color_red[1];
        s_profileActionColor[2] = color_red[2];
        s_profileActionColor[3] = color_red[3] * frac;
        s_main.profileAction.color = s_profileActionColor;

        s_main.profileInfoLine1.color = uis.text_color;
        s_main.profileInfoLine2.color = uis.text_color;

        s_main.carlogo.generic.x = (int)(640 - 440 * frac);
}

/*
===============
MainMenu_Prepare
===============
*/
void MainMenu_Prepare( void ) {

        UI_Profile_MarkStatsDirty();
        MainMenu_UpdateProfileTexts();
        MainMenu_Update();

}


/*
===============
Main_MenuDraw
===============
*/
static void Main_MenuDraw( void ) {

        MainMenu_UpdateProfileTexts();

        // standard menu drawing

        Menu_Draw( &s_main.menu );

        if (uis.demoversion) {

                UI_DrawProportionalString( 320, 432, "DEMO      FOR MATURE AUDIENCES      DEMO", UI_CENTER|UI_SMALLFONT, text_color_normal );
                UI_DrawString( 320, 460, Q3_VERSION " | 2002 - 2026 | www.q3rally.com | It's damn fast baby!", UI_CENTER|UI_SMALLFONT, text_color_normal );

        } else {

                UI_DrawString( 365, 460, Q3_VERSION " | 2002 - 2026 | www.q3rally.com | It's damn fast baby!", UI_CENTER|UI_SMALLFONT, text_color_normal );

        }

}



#if 0
/*
===============
UI_TeamArenaExists
===============
*/
static qboolean UI_TeamArenaExists( void ) {
	int		numdirs;
	char	dirlist[2048];
	char	*dirptr;
  char  *descptr;
	int		i;
	int		dirlen;

	numdirs = trap_FS_GetFileList( "$modlist", "", dirlist, sizeof(dirlist) );
	dirptr  = dirlist;
	for( i = 0; i < numdirs; i++ ) {
		dirlen = strlen( dirptr ) + 1;
    descptr = dirptr + dirlen;
		if (Q_stricmp(dirptr, BASETA) == 0) {
			return qtrue;
		}
    dirptr += dirlen + strlen(descptr) + 1;
	}
	return qfalse;
}
#endif

/*
=================
InitMenuText
=================
*/

static void InitMenuText(menutext_s *item, int id, char *label, int x, int y) {

        item->generic.type = MTYPE_PTEXT;
        item->generic.flags = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
        item->generic.id = id;
        item->generic.callback = Main_MenuEvent;
        item->generic.x = x;
        item->generic.y = y;
        item->style = UI_RIGHT|UI_DROPSHADOW;
        item->string = label;
        item->color = text_color_normal;
}

/*
=================
InitMenuTextInfo
=================
*/
static void InitMenuTextInfo(menutext_s *item, char *label, int x, int y) {

        item->generic.type = MTYPE_PTEXT;
        item->generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
        item->generic.x = x;
        item->generic.y = y;
        item->style = UI_LEFT|UI_SMALLFONT|UI_DROPSHADOW;
        item->string = label;
        item->color = text_color_normal;
}

/*
===============
UI_MainMenu

The main menu only comes up when not in a game,
so make sure that the attract loop server is down
and that local cinematics are killed
===============
*/
void UI_MainMenu( void ) {
	
	int x;
	int y;
        int profileY;
        int profileInfoY;
        int numMusicFiles;
        int selectedMusic;
        char musicFiles[256][MAX_QPATH];
        char musicCommand[MAX_QPATH];
        int menuSpacing;


        numMusicFiles = UI_BuildFileList("music", "ogg", "menumusic", qtrue, qfalse, qfalse, 0, musicFiles);

        if (numMusicFiles > 0) {
                selectedMusic = UI_RandomInt( numMusicFiles );
                Com_sprintf(musicCommand, sizeof(musicCommand), "music music/menumusic%s\n", musicFiles[selectedMusic]);
                trap_Cmd_ExecuteText(EXEC_APPEND, musicCommand);
        }

	trap_Cvar_Set( "sv_killserver", "1" );

	if( !uis.demoversion && !ui_cdkeychecked.integer ) {
		char	key[17];

		trap_GetCDKey( key, sizeof(key) );
		if( trap_VerifyCDKey( key, NULL ) == qfalse ) {
			UI_CDKeyMenu();
			return;
		}
	}

	memset( &s_main, 0 ,sizeof(mainmenu_t) );

        MainMenu_Prepare();

        s_main.menu.draw = Main_MenuDraw;
        s_main.menu.fullscreen = qtrue;
        s_main.menu.wrapAround = qtrue;
        s_main.menu.showlogo = qtrue;
        s_main.menu.transition = MainMenu_RunTransition;
        s_main.menu.changeMenu = MainMenu_ChangeMenu;
        s_main.banner.generic.type                      = MTYPE_BTEXT;
        s_main.banner.generic.flags                     = QMF_INACTIVE;
        s_main.banner.generic.x                         = 320;
        s_main.banner.generic.y                         = 17;
        s_main.banner.string                            = "Q3RALLY";
        s_main.banner.color                             = text_color_normal;
        s_main.banner.style                             = UI_CENTER|UI_DROPSHADOW;

        x = 175;
        y = 75;
        menuSpacing = MAIN_MENU_VERTICAL_SPACING - 5;

        
	InitMenuText(&s_main.singleplayer, ID_SINGLEPLAYER, "OFFLINE", x - 10, y + 12);


	y += menuSpacing;
	InitMenuText(&s_main.multiplayer, ID_MULTIPLAYER, "ONLINE", x - 10, y + 12);


	y += menuSpacing;
	InitMenuText(&s_main.setup, ID_SETUP, "CONFIG", x - 10, y + 12);


	y += menuSpacing;
	InitMenuText(&s_main.garage, ID_GARAGE, "THE GARAGE", x - 10, y + 12);
        
        
	y += menuSpacing;
	InitMenuText(&s_main.demos, ID_DEMOS, "DEMOS", x - 10, y + 12);


        s_main.carlogo.generic.type                     = MTYPE_BITMAP;
        s_main.carlogo.generic.flags                    = QMF_INACTIVE;
        s_main.carlogo.generic.ownerdraw                = MainMenu_DrawPlayer;
        s_main.carlogo.generic.x                        = 200;
        s_main.carlogo.generic.y                        = 0;
        s_main.carlogo.width                            = 480;
        s_main.carlogo.height                           = 480;
        
	y += menuSpacing;
	InitMenuText(&s_main.exit, ID_EXIT, "QUIT", x - 10, y + 12);


        y += menuSpacing;
        profileY = y + 22;
        profileInfoY = y + 16;
        InitMenuText(&s_main.profileAction, ID_PROFILE_ACTION, "CREATE", x - 10, profileY);
        s_main.profileAction.generic.flags = QMF_RIGHT_JUSTIFY;

        Q_strncpyz( s_main.profileRankLine, "RANK: -", sizeof( s_main.profileRankLine ) );
        Q_strncpyz( s_main.profilePointsLine, "POINTS: 0", sizeof( s_main.profilePointsLine ) );
        InitMenuTextInfo(&s_main.profileInfoLine1, s_main.profileRankLine, x + 20, profileInfoY);
        InitMenuTextInfo(&s_main.profileInfoLine2, s_main.profilePointsLine, x + 20, profileInfoY + 16);
        MainMenu_UpdateProfileTexts();


        Menu_AddItem( &s_main.menu,     &s_main.banner );
        Menu_AddItem( &s_main.menu,     &s_main.carlogo );
        Menu_AddItem( &s_main.menu,     &s_main.singleplayer );
        Menu_AddItem( &s_main.menu,     &s_main.multiplayer );
        Menu_AddItem( &s_main.menu,     &s_main.setup );
        Menu_AddItem( &s_main.menu,     &s_main.garage );
        Menu_AddItem( &s_main.menu,     &s_main.demos );
        Menu_AddItem( &s_main.menu,     &s_main.exit );            
        Menu_AddItem( &s_main.menu,     &s_main.profileAction );
        Menu_AddItem( &s_main.menu,     &s_main.profileInfoLine1 );
        Menu_AddItem( &s_main.menu,     &s_main.profileInfoLine2 );

        trap_Key_SetCatcher( KEYCATCH_UI );
        uis.menusp = 0;

        UI_PushMenu ( &s_main.menu );

        UI_ProfileOverlay_MaybeShow();

        if ( uis.activemenu == &s_main.menu ) {
                uis.transitionIn = uis.realtime;
        }

}
