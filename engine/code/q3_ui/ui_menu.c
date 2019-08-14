/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2019 Q3Rally Team (Per Thormann - q3rally@gmail.com)

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
// STONELANCE
// #define ID_TEAMARENA                 15
// END
#define ID_MODS                         16
#define ID_EXIT                         17


// STONELANCE
//#define MAIN_BANNER_MODEL             "models/mapobjects/banners/banner5.md3"
//#define MAIN_MENU_VERTICAL_SPACING    34

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
// STONELANCE
//      menutext_s              teamArena;
// END
        menutext_s              mods;
        menutext_s              exit;

// STONELANCE
        menutext_s              banner;
        menubitmap_s            carlogo;

        playerInfo_t    playerinfo;
        char                    modelskin[MAX_QPATH];
        char                    rimskin[MAX_QPATH];
        char                    headskin[MAX_QPATH];

//      qhandle_t               bannerModel;
// END
} mainmenu_t;


static mainmenu_t s_main;


// STONELANCE (new function)
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
                car = (int)(UI_Random() * numItems);
                strncpy(carName, cars[car], sizeof(carName));
        }
        else
                strncpy(carName, DEFAULT_MODEL, sizeof(carName));

//      Com_Printf("car: %d, numItems %d\n", car, numItems);
//      Com_Printf("carName: %s\n", carName);

        // get skins for the choosen car
        numItems = UI_BuildFileList( va("models/players/%s", carName), "skin", "", qtrue, qfalse, qfalse, 0, cars);

        // choose a skin from the list randomly
        if (numItems){
                skin = UI_Random() * numItems;
                strncpy(skinName, cars[skin], sizeof(skinName));
        }
        else
                strncpy(skinName, DEFAULT_SKIN, sizeof(skinName));

//      Com_Printf("skin: %d, numItems %d\n", skin, numItems);
//      Com_Printf("skinName: %s\n", skinName);

        // FIXME: choose rim randomly?
        Com_sprintf(s_main.modelskin, sizeof(s_main.modelskin), "%s/%s", carName, skinName);
}


/*
=================
MainMenu_Update
=================
*/
void MainMenu_Update( void ){
/*
        seed = rand();

        if(s_main.modelskin[0])
                return;

        // get model
        model = (int)(uis.realtime % s_main.nummodels);

        buffptr  = s_main.modelnames[model] + strlen("models/players/");
        strcpy(s_main.modelskin, buffptr);
*/
        MainMenu_BuildList();

        trap_Cvar_VariableStringBuffer( "rim", s_main.rimskin, sizeof( s_main.rimskin ) );
        trap_Cvar_VariableStringBuffer( "head", s_main.headskin, sizeof( s_main.headskin ) );
       
        MainMenu_UpdateModel();
}
// END


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
// STONELANCE
        UI_Rally_CreditMenu();
// END
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
// STONELANCE
        case ID_SINGLEPLAYER:
        case ID_MULTIPLAYER:
        case ID_SETUP:
        case ID_DEMOS:
        case ID_CINEMATICS:
        case ID_MODS:
                s_main.menu.transitionMenu = ((menucommon_s*)ptr)->id;
                uis.transitionOut = uis.realtime;
                break;

/*
        case ID_SINGLEPLAYER:
                UI_SPLevelMenu();
                break;

        case ID_MULTIPLAYER:
                UI_ArenaServersMenu();
                break;

        case ID_SETUP:
                UI_SetupMenu();
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

        case ID_TEAMARENA:
                trap_Cvar_Set( "fs_game", BASETA);
                trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
                break;
*/
// END

        case ID_EXIT:
                UI_ConfirmMenu( "EXIT GAME?", 0, MainMenu_ExitAction );
                break;
        }
}


// STONELANCE
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
        uis.text_color[0] = text_color_normal[0];
        uis.text_color[1] = text_color_normal[1];
        uis.text_color[2] = text_color_normal[2];
        uis.text_color[3] = text_color_normal[3] * frac;

        s_main.banner.color = uis.text_color;

        s_main.singleplayer.color = uis.text_color;
        s_main.multiplayer.color = uis.text_color;
        s_main.setup.color = uis.text_color;
        s_main.cinematics.color = uis.text_color;
        s_main.demos.color = uis.text_color;
        s_main.mods.color = uis.text_color;
        s_main.exit.color = uis.text_color;

        s_main.carlogo.generic.x = (int)(640 - 440 * frac);
}
// END


/*
===============
MainMenu_Cache
===============
*/
void MainMenu_Cache( void ) {
// STONELANCE
//      s_main.bannerModel = trap_R_RegisterModel( MAIN_BANNER_MODEL );

        MainMenu_Update();
// END
}


/*
===============
Main_MenuDraw
===============
*/
static void Main_MenuDraw( void ) {
// STONELANCE - dont draw 3d banner
/*
        refdef_t                refdef;
        refEntity_t             ent;
        vec3_t                  origin;
        vec3_t                  angles;
        float                   adjust;
        float                   x, y, w, h;
        vec4_t                  color = {0.5, 0, 0, 1};

        // setup the refdef

        memset( &refdef, 0, sizeof( refdef ) );

        refdef.rdflags = RDF_NOWORLDMODEL;

        AxisClear( refdef.viewaxis );

        x = 0;
        y = 0;
        w = 640;
        h = 120;
        UI_AdjustFrom640( &x, &y, &w, &h );
        refdef.x = x;
        refdef.y = y;
        refdef.width = w;
        refdef.height = h;

        adjust = 0; // JDC: Kenneth asked me to stop this 1.0 * sin( (float)uis.realtime / 1000 );
        refdef.fov_x = 60 + adjust;
        refdef.fov_y = 19.6875 + adjust;

        refdef.time = uis.realtime;

        origin[0] = 300;
        origin[1] = 0;
        origin[2] = -32;

        trap_R_ClearScene();

        // add the model

        memset( &ent, 0, sizeof(ent) );

        adjust = 5.0 * sin( (float)uis.realtime / 5000 );
        VectorSet( angles, 0, 180 + adjust, 0 );
        AnglesToAxis( angles, ent.axis );
        ent.hModel = s_main.bannerModel;
        VectorCopy( origin, ent.origin );
        VectorCopy( origin, ent.lightingOrigin );
        ent.renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW;
        VectorCopy( ent.origin, ent.oldorigin );

        trap_R_AddRefEntityToScene( &ent );

        trap_R_RenderScene( &refdef );
*/
// END

        // standard menu drawing

        Menu_Draw( &s_main.menu );

        if (uis.demoversion) {
// STONELANCE
                UI_DrawProportionalString( 320, 432, "DEMO      FOR MATURE AUDIENCES      DEMO", UI_CENTER|UI_SMALLFONT, text_color_normal );
                //UI_DrawString( 320, 460, Q3_VERSION " (c) 2002 - 2009 New Team Q3Rally | www.q3rally.com", UI_CENTER|UI_SMALLFONT, text_color_normal );
				UI_DrawString( 320, 460, Q3_VERSION " (c) 2002 - 2019 | 17 Years of Q3R | www.q3rally.com | It's damn fast baby!", UI_CENTER|UI_SMALLFONT, text_color_normal );
//              UI_DrawProportionalString( 320, 372, "DEMO      FOR MATURE AUDIENCES      DEMO", UI_CENTER|UI_SMALLFONT, color );
//              UI_DrawString( 320, 400, "Quake III Arena(c) 1999-2000, Id Software, Inc.  All Rights Reserved", UI_CENTER|UI_SMALLFONT, color );
// END
        } else {
// STONELANCE
                //UI_DrawString( 320, 460, Q3_VERSION " (c) 2002 - 2009 New Team Q3Rally | www.q3rally.com", UI_CENTER|UI_SMALLFONT, text_color_normal );
				UI_DrawString( 320, 460, Q3_VERSION " (c) 2002 - 2019 | 17 Years of Q3R | www.q3rally.com | It's damn fast baby!", UI_CENTER|UI_SMALLFONT, text_color_normal );
//              UI_DrawString( 320, 450, "Quake III Arena(c) 1999-2000, Id Software, Inc.  All Rights Reserved", UI_CENTER|UI_SMALLFONT, color );
// END
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
===============
UI_MainMenu

The main menu only comes up when not in a game,
so make sure that the attract loop server is down
and that local cinematics are killed
===============
*/
void UI_MainMenu( void ) {
	int		y;
//	qboolean teamArena = qfalse;
// STONELANCE
	int	x;
//	int	style = UI_CENTER | UI_DROPSHADOW;
	int	style = UI_RIGHT | UI_DROPSHADOW;

	trap_Cmd_ExecuteText( EXEC_APPEND, "music music/q3r_menumusic\n" );
// END

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

	MainMenu_Cache();

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
        s_main.banner.string                            = "Q 3 R a l l y";
        s_main.banner.color                             = text_color_normal;
        s_main.banner.style                             = UI_CENTER;

        x = 240;
        y = 100;


        s_main.singleplayer.generic.type                = MTYPE_PTEXT;
        s_main.singleplayer.generic.id                  = ID_SINGLEPLAYER;
        s_main.singleplayer.generic.callback            = Main_MenuEvent;
        s_main.singleplayer.style                       = style;
        s_main.singleplayer.generic.flags               = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
        s_main.singleplayer.generic.x                   = x - 10;
        s_main.singleplayer.generic.y                   = y + 12;
        s_main.singleplayer.string                      = "OFFLINE";
        s_main.singleplayer.color                       = text_color_normal;


        y += MAIN_MENU_VERTICAL_SPACING;
        s_main.multiplayer.generic.type                 = MTYPE_PTEXT;
        s_main.multiplayer.generic.id                   = ID_MULTIPLAYER;
        s_main.multiplayer.generic.callback             = Main_MenuEvent;
        s_main.multiplayer.style                        = style;
        s_main.multiplayer.generic.flags                = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
        s_main.multiplayer.generic.x                    = x - 10;
        s_main.multiplayer.generic.y                    = y + 12;
        s_main.multiplayer.string                       = "ONLINE";
        s_main.multiplayer.color                        = text_color_normal;


        y += MAIN_MENU_VERTICAL_SPACING;
        s_main.setup.generic.type                       = MTYPE_PTEXT;
        s_main.setup.generic.id                         = ID_SETUP;
        s_main.setup.generic.callback                   = Main_MenuEvent;
        s_main.setup.style                              = style;
        s_main.setup.generic.flags                      = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
        s_main.setup.generic.x                          = x - 10;
        s_main.setup.generic.y                          = y + 12;
        s_main.setup.string                             = "CONFIG";
        s_main.setup.color                              = text_color_normal;


        y += MAIN_MENU_VERTICAL_SPACING;
        s_main.demos.generic.type                       = MTYPE_PTEXT;
        s_main.demos.generic.id                         = ID_DEMOS;
        s_main.demos.generic.callback                   = Main_MenuEvent;
        s_main.demos.style                              = style;
        s_main.demos.generic.flags                      = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
        s_main.demos.generic.x                          = x - 10;
        s_main.demos.generic.y                          = y + 12;
        s_main.demos.string                             = "DEMOS";
        s_main.demos.color                              = text_color_normal;


        s_main.carlogo.generic.type                     = MTYPE_BITMAP;
        s_main.carlogo.generic.flags                    = QMF_INACTIVE;
        s_main.carlogo.generic.ownerdraw                = MainMenu_DrawPlayer;
        s_main.carlogo.generic.x                        = 200;
        s_main.carlogo.generic.y                        = 0;
        s_main.carlogo.width                            = 480;
        s_main.carlogo.height                           = 480;


        y += MAIN_MENU_VERTICAL_SPACING;
        s_main.exit.generic.type                        = MTYPE_PTEXT;
        s_main.exit.generic.id                          = ID_EXIT;
        s_main.exit.generic.callback                    = Main_MenuEvent;
        s_main.exit.style                               = style;
        s_main.exit.generic.flags                       = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
        s_main.exit.generic.x                           = x - 10;
        s_main.exit.generic.y                           = y + 12;
        s_main.exit.string                              = "QUIT";
        s_main.exit.color                               = text_color_normal;


        Menu_AddItem( &s_main.menu,     &s_main.banner );
        Menu_AddItem( &s_main.menu,     &s_main.carlogo );
        Menu_AddItem( &s_main.menu,     &s_main.singleplayer );
        Menu_AddItem( &s_main.menu,     &s_main.multiplayer );
        Menu_AddItem( &s_main.menu,     &s_main.setup );
        Menu_AddItem( &s_main.menu,     &s_main.demos );
        Menu_AddItem( &s_main.menu,     &s_main.exit );            

        trap_Key_SetCatcher( KEYCATCH_UI );
        uis.menusp = 0;

        UI_PushMenu ( &s_main.menu );


        uis.transitionIn = uis.realtime;

}
