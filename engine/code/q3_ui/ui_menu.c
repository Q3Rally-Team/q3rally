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
#include "ui_rally_theme.h"
#include "ui_rally_frontend.h"


#define ID_SINGLEPLAYER                 10
#define ID_MULTIPLAYER                  11
#define ID_SETUP                        12
#define ID_DEMOS                        13
#define ID_CINEMATICS                   14
#define ID_PROFILE_ACTION               15

#define ID_MODS                         16
#define ID_RIVALS                       17
#define ID_EXIT                         18
// Q3RALLY DOWNLOADS START
#define ID_DOWNLOADS                    19
// Q3RALLY DOWNLOADS END
#define ID_PROFILE_STATS                20

#define MAIN_BANNER_MODEL               "models/mapobjects/q3rtitle/q3rtitle.md3"
#define MAIN_MENU_VERTICAL_SPACING      50
#define MAIN_MENU_PROFILE_STAT_INTERVAL 4200
#define MAIN_MENU_PROFILE_STAT_COUNT    8
// END


typedef struct {
        menuframework_s menu;

        menutext_s              singleplayer;
        menutext_s              multiplayer;
        menutext_s              setup;
        menutext_s              demos;
        menutext_s              cinematics;
        menutext_s              mods;
        menutext_s              rivals;
        // Q3RALLY DOWNLOADS START
        menutext_s              downloads;
        // Q3RALLY DOWNLOADS END
        menutext_s              exit;
        menutext_s              profileAction;
        menutext_s              profileStatsAction;
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
        char                    plateskin[MAX_QPATH];
        float                   visualAlpha;
        qboolean                vehicleDragging;

} mainmenu_t;


static mainmenu_t s_main;
static vec4_t s_profileActionColor;
static vec4_t s_frontendScrim = UI_FRONTEND_COLOR_SCRIM;
static vec4_t s_frontendHeroOverlay = UI_FRONTEND_COLOR_HERO_OVERLAY;
static vec4_t s_frontendAccent = UI_FRONTEND_COLOR_ACCENT;
static vec4_t s_frontendText = UI_FRONTEND_COLOR_TEXT;
static vec4_t s_frontendMuted = UI_FRONTEND_COLOR_MUTED;
static vec4_t s_frontendStatus = UI_FRONTEND_COLOR_STATUS;

static float MainMenu_ViewportLeft( void ) {
        if ( uis.xscale <= 0.0f ) {
                return 0.0f;
        }
        return -uis.bias / uis.xscale;
}

static float MainMenu_ViewportRight( void ) {
        return SCREEN_WIDTH - MainMenu_ViewportLeft();
}

static float MainMenu_RailX( void ) {
        return MainMenu_ViewportLeft() + 24.0f;
}

static float MainMenu_RailWidth( void ) {
        return 210.0f;
}

static float MainMenu_NavX( void ) {
        return MainMenu_RailX() + 14.0f;
}

static float MainMenu_NavWidth( void ) {
        return MainMenu_RailWidth() - 28.0f;
}

static float MainMenu_HeroX( void ) {
        return MainMenu_RailX() + MainMenu_RailWidth() + 16.0f;
}

static float MainMenu_HeroRight( void ) {
        return MainMenu_ViewportRight() - 24.0f;
}

static float MainMenu_HeroWidth( void ) {
        return MainMenu_HeroRight() - MainMenu_HeroX();
}

static void MainMenu_ColorWithAlpha( vec4_t out, const float *baseColor ) {
        out[0] = baseColor[0];
        out[1] = baseColor[1];
        out[2] = baseColor[2];
        out[3] = baseColor[3] * s_main.visualAlpha;
}

static void MainMenu_SetInteractiveBounds( menutext_s *item, int left, int right ) {
        item->generic.left = left;
        item->generic.right = right;
        item->generic.top = item->generic.y - 6;
        item->generic.bottom = item->generic.y + SMALLCHAR_HEIGHT + 6;
}

static void MainMenu_DrawNavItem( void *self ) {
        menutext_s *item;
        qboolean focus;
        float navX;
        float navWidth;
        int top;

        item = (menutext_s *)self;
        focus = ( Menu_ItemAtCursor( item->generic.parent ) == item );
        navX = MainMenu_NavX();
        navWidth = MainMenu_NavWidth();
        top = item->generic.y - 9;

        Frontend_DrawNavButton( (int)navX, top, (int)navWidth, 24,
                                item->string, s_main.visualAlpha, focus,
                                UI_FRONTEND_TEXT_LEFT );
}

static void MainMenu_DrawProfileAction( void *self ) {
        menutext_s *item;
        qboolean focus;
        vec4_t textColor;
        vec4_t mutedColor;
        float navX;
        float navWidth;
        int top;

        item = (menutext_s *)self;
        focus = ( Menu_ItemAtCursor( item->generic.parent ) == item );
        navX = MainMenu_NavX();
        navWidth = MainMenu_NavWidth();
        top = item->generic.y - 16;

        Frontend_DrawCard( (int)navX, top, (int)navWidth, 82,
                           s_main.visualAlpha, focus );
        MainMenu_ColorWithAlpha( textColor, focus ? s_frontendAccent : s_frontendText );
        MainMenu_ColorWithAlpha( mutedColor, s_frontendMuted );

        Frontend_DrawStatusChip( (int)navX + 12, top + 10, "Profile",
                                 focus ? s_frontendAccent : s_frontendStatus,
                                 s_main.visualAlpha );
        Frontend_DrawText( (int)( navX + 30 ), top + 27, item->string,
                           UI_LEFT | UI_SMALLFONT | UI_DROPSHADOW,
                           textColor );
        Frontend_DrawText( (int)( navX + 30 ), top + 48, s_main.profileRankLine,
                           UI_LEFT | UI_SMALLFONT, mutedColor );
        Frontend_DrawText( (int)( navX + 30 ), top + 63, s_main.profilePointsLine,
                           UI_LEFT | UI_SMALLFONT, mutedColor );
}

static void MainMenu_DrawBrand( void *self ) {
        vec4_t accentColor;
        vec4_t textColor;
        float railX;

        (void)self;

        MainMenu_ColorWithAlpha( accentColor, s_frontendAccent );
        MainMenu_ColorWithAlpha( textColor, s_frontendText );
        railX = MainMenu_RailX();

        UI_FillRect( railX + 22, 50, 6, 6, accentColor );
        Frontend_DrawText( (int)( railX + 38 ), 48, "Q3RALLY",
                           UI_LEFT | UI_BIGFONT | UI_DROPSHADOW, textColor );
}

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
                Com_sprintf( s_main.profileRankLine, sizeof( s_main.profileRankLine ), "Rank: %s", activeRank.current->name );
                Com_sprintf( s_main.profilePointsLine, sizeof( s_main.profilePointsLine ), "Points: %d", activeProfileStats->playerScore );
        } else {
                Q_strncpyz( s_main.profileRankLine, "Rank: -", sizeof( s_main.profileRankLine ) );
                Q_strncpyz( s_main.profilePointsLine, "Points: 0", sizeof( s_main.profilePointsLine ) );
        }

        s_main.profileInfoLine1.string = s_main.profileRankLine;
        s_main.profileInfoLine2.string = s_main.profilePointsLine;
}

static int MainMenu_ProfileStatIndex( void ) {
        const char *profileName;
        int hash;
        int cycle;

        profileName = UI_Profile_GetActiveName();
        hash = 17;
        if ( profileName ) {
                while ( *profileName ) {
                        hash = ( hash * 33 + (unsigned char)*profileName ) & 0x7fffffff;
                        profileName++;
                }
        }

        cycle = uis.realtime / MAIN_MENU_PROFILE_STAT_INTERVAL;
        return ( cycle + hash ) % MAIN_MENU_PROFILE_STAT_COUNT;
}

static float MainMenu_ProfileStatFade( void ) {
        float phase;

        phase = ( uis.realtime % MAIN_MENU_PROFILE_STAT_INTERVAL ) /
                (float)MAIN_MENU_PROFILE_STAT_INTERVAL;
        if ( phase < 0.16f ) {
                return phase / 0.16f;
        }
        if ( phase > 0.84f ) {
                return ( 1.0f - phase ) / 0.16f;
        }
        return 1.0f;
}

static void MainMenu_DrawProfileStat( float heroX, float heroWidth,
                                      qboolean focus ) {
        const profile_stats_t *stats;
        vec4_t labelColor;
        vec4_t valueColor;
        char value[64];
        const char *label;
        int statIndex;
        int statX;
        int statY;
        float fade;

        if ( !UI_Profile_HasActiveProfile() ) {
                return;
        }

        stats = UI_Profile_GetActiveStats();
        if ( !stats ) {
                return;
        }

        statIndex = MainMenu_ProfileStatIndex();
        label = "Points";
        value[0] = '\0';

        switch ( statIndex ) {
        case 0:
                label = "Points";
                Com_sprintf( value, sizeof( value ), "%d", stats->playerScore );
                break;
        case 1:
                label = "Wins";
                Com_sprintf( value, sizeof( value ), "%d", stats->wins );
                break;
        case 2:
                label = "Races";
                Com_sprintf( value, sizeof( value ), "%d", stats->gamesPlayed );
                break;
        case 3:
                label = "Distance";
                Com_sprintf( value, sizeof( value ), "%.1f km", stats->distanceKm );
                break;
        case 4:
                label = "Top speed";
                Com_sprintf( value, sizeof( value ), "%.0f km/h", stats->topSpeedKph );
                break;
        case 5:
                label = "Best lap";
                if ( stats->bestLapMs > 0 ) {
                        Com_sprintf( value, sizeof( value ), "%d:%02d.%03d",
                                     stats->bestLapMs / 60000,
                                     ( stats->bestLapMs / 1000 ) % 60,
                                     stats->bestLapMs % 1000 );
                } else {
                        Q_strncpyz( value, "-", sizeof( value ) );
                }
                break;
        case 6:
                label = "Podiums";
                Com_sprintf( value, sizeof( value ), "%d", stats->racingPodiums );
                break;
        default:
                label = "Fuel used";
                Com_sprintf( value, sizeof( value ), "%.1f l", stats->fuelUsed );
                break;
        }

        fade = MainMenu_ProfileStatFade() * s_main.visualAlpha;
        statX = (int)( heroX + heroWidth - 144.0f );
        statY = 354;
        MainMenu_ColorWithAlpha( valueColor, s_frontendAccent );
        valueColor[3] *= fade;
        UI_FillRect( statX, statY, 128, 1, valueColor );
        UI_FillRect( statX, statY, 2, 58, valueColor );
        Frontend_DrawStatusChip( statX + 12, statY + 8, "Profile Stats",
                                 focus ? s_frontendAccent : s_frontendStatus,
                                 fade );

        MainMenu_ColorWithAlpha( labelColor, s_frontendMuted );
        labelColor[3] *= fade;
        Frontend_DrawText( statX + 12, statY + 29, label,
                           UI_LEFT | UI_SMALLFONT, labelColor );
        Frontend_DrawText( statX + 12, statY + 44, value,
                           UI_LEFT | UI_BIGFONT | UI_DROPSHADOW, valueColor );
}

static void MainMenu_DrawProfileStatAction( void *self ) {
        menutext_s *item;
        qboolean focus;

        item = (menutext_s *)self;
        focus = ( Menu_ItemAtCursor( item->generic.parent ) == item );
        MainMenu_DrawProfileStat( MainMenu_HeroX(), MainMenu_HeroWidth(), focus );
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

        /* Keep the garage preview still until the player starts dragging it.
         * The active car faces the camera in a three-quarter front view. */
        moveangles[YAW] = 204.0f;

        Q_strncpyz( plate, s_main.plateskin, sizeof( plate ) );
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
        qboolean         cursorInVehicle;
        qboolean         mouseDown;
        int               deltaX;

        uis.mainMenu = 1;

        b = (menubitmap_s*) self;

        cursorInVehicle = ( uis.cursorx >= b->generic.x &&
                            uis.cursorx <= b->generic.x + b->width &&
                            uis.cursory >= b->generic.y &&
                            uis.cursory <= b->generic.y + b->height );
        mouseDown = trap_Key_IsDown( K_MOUSE1 );

        if ( !mouseDown ) {
                s_main.vehicleDragging = qfalse;
        } else if ( !s_main.vehicleDragging && cursorInVehicle ) {
                s_main.vehicleDragging = qtrue;
        }

        if ( s_main.vehicleDragging ) {
                deltaX = uis.cursorx - uis.cursorpx;
                s_main.playerinfo.moveAngles[YAW] = AngleNormalize360(
                        s_main.playerinfo.moveAngles[YAW] + deltaX * 0.75f );
        }

        UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, &s_main.playerinfo, uis.realtime );
}

/*
=================
MainMenu_ReadActiveVehicle
=================
*/
static void MainMenu_ReadActiveVehicle( void )
{
        trap_Cvar_VariableStringBuffer( "model", s_main.modelskin, sizeof( s_main.modelskin ) );

        if ( !s_main.modelskin[0] ) {
                Com_sprintf( s_main.modelskin, sizeof( s_main.modelskin ), "%s/%s", DEFAULT_MODEL, DEFAULT_SKIN );
        } else if ( !strchr( s_main.modelskin, '/' ) ) {
                Q_strcat( s_main.modelskin, sizeof( s_main.modelskin ), "/" DEFAULT_SKIN );
        }
}


/*
=================
MainMenu_Update
=================
*/
void MainMenu_Update( void ){

        MainMenu_ReadActiveVehicle();

        trap_Cvar_VariableStringBuffer( "rim", s_main.rimskin, sizeof( s_main.rimskin ) );
        trap_Cvar_VariableStringBuffer( "head", s_main.headskin, sizeof( s_main.headskin ) );
        trap_Cvar_VariableStringBuffer( "plate", s_main.plateskin, sizeof( s_main.plateskin ) );
       
        MainMenu_UpdateModel();
}

/*
=================
MainMenu_SyncActiveVehicle
=================
*/
static void MainMenu_SyncActiveVehicle( void )
{
        char previousModel[MAX_QPATH];
        char rim[MAX_QPATH];
        char head[MAX_QPATH];
        char plate[MAX_QPATH];

        Q_strncpyz( previousModel, s_main.modelskin, sizeof( previousModel ) );
        MainMenu_ReadActiveVehicle();

        trap_Cvar_VariableStringBuffer( "rim", rim, sizeof( rim ) );
        trap_Cvar_VariableStringBuffer( "head", head, sizeof( head ) );
        trap_Cvar_VariableStringBuffer( "plate", plate, sizeof( plate ) );

        if ( !Q_stricmp( previousModel, s_main.modelskin ) &&
             !Q_stricmp( s_main.rimskin, rim ) &&
             !Q_stricmp( s_main.headskin, head ) &&
             !Q_stricmp( s_main.plateskin, plate ) ) {
                return;
        }

        Q_strncpyz( s_main.rimskin, rim, sizeof( s_main.rimskin ) );
        Q_strncpyz( s_main.headskin, head, sizeof( s_main.headskin ) );
        Q_strncpyz( s_main.plateskin, plate, sizeof( s_main.plateskin ) );
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
        case ID_RIVALS:
        case ID_DEMOS:
        case ID_CINEMATICS:
        case ID_MODS:
        // Q3RALLY DOWNLOADS START
        case ID_DOWNLOADS:
        // Q3RALLY DOWNLOADS END
                s_main.menu.transitionMenu = ((menucommon_s*)ptr)->id;
                uis.transitionOut = uis.realtime;
                break;

        case ID_PROFILE_ACTION:
                UI_ProfileOverlay_Open( qfalse );
                break;

        case ID_PROFILE_STATS:
                if ( UI_Profile_HasActiveProfile() ) {
                        UI_PlayerStatsMenu();
                }
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

        uis.mainMenu = 0;

        switch( menuId) {
        case ID_SINGLEPLAYER:
                UI_StartServerMenu( qfalse );
                break;

        case ID_MULTIPLAYER:
                UI_ArenaServersMenu();
                break;

        case ID_SETUP:
                UI_SetupMenu();
                break;

        case ID_RIVALS:
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

        // Q3RALLY DOWNLOADS START
        case ID_DOWNLOADS:
                UI_Rally_DownloadsMenu();
                break;
        // Q3RALLY DOWNLOADS END
        }
}


/*
===============
MainMenu_RunTransition
===============
*/
void MainMenu_RunTransition( float frac ) {
        s_main.visualAlpha = frac;

        uis.text_color[0] = text_color_normal[0];
        uis.text_color[1] = text_color_normal[1];
        uis.text_color[2] = text_color_normal[2];
        uis.text_color[3] = text_color_normal[3] * frac;

        s_main.banner.color = uis.text_color;

        s_main.singleplayer.color = uis.text_color;
        s_main.multiplayer.color = uis.text_color;
        s_main.setup.color = uis.text_color;
        s_main.rivals.color = uis.text_color;
        s_main.cinematics.color = uis.text_color;
        s_main.demos.color = uis.text_color;
        s_main.mods.color = uis.text_color;
        // Q3RALLY DOWNLOADS START
        s_main.downloads.color = uis.text_color;
        // Q3RALLY DOWNLOADS END
        s_main.exit.color = uis.text_color;

        s_profileActionColor[0] = color_red[0];
        s_profileActionColor[1] = color_red[1];
        s_profileActionColor[2] = color_red[2];
        s_profileActionColor[3] = color_red[3] * frac;
        s_main.profileAction.color = s_profileActionColor;

        s_main.profileInfoLine1.color = uis.text_color;
        s_main.profileInfoLine2.color = uis.text_color;

        s_main.carlogo.generic.x = (int)(MainMenu_HeroX() + 40.0f - (1.0f - frac) * 80.0f);
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
        vec4_t scrimColor;
        vec4_t heroOverlayColor;
        vec4_t textColor;
        vec4_t mutedColor;
        float railX;
        float heroX;
        float heroWidth;

        /* The player can change the vehicle in setup while this menu stays
         * alive. Refresh the garage preview as soon as the menu is drawn
         * again, without requiring a complete UI restart. */
        MainMenu_SyncActiveVehicle();
        MainMenu_UpdateProfileTexts();

        MainMenu_ColorWithAlpha( scrimColor, s_frontendScrim );
        MainMenu_ColorWithAlpha( heroOverlayColor, s_frontendHeroOverlay );
        MainMenu_ColorWithAlpha( textColor, s_frontendText );
        MainMenu_ColorWithAlpha( mutedColor, s_frontendMuted );

        railX = MainMenu_RailX();
        heroX = MainMenu_HeroX();
        heroWidth = MainMenu_HeroWidth();

        /* The generated garage scene is the shared frontend backdrop. The
         * scrim keeps it atmospheric while leaving the subject visible. */
        Frontend_DrawBackground( scrimColor );

        Frontend_DrawSidebar( (int)railX, 32, (int)MainMenu_RailWidth(), 410,
                              NULL, s_main.visualAlpha );

        UI_SetColor( heroOverlayColor );
        UI_DrawHandlePic( heroX, 32, heroWidth, 410,
                          Frontend_BackgroundShader() );
        UI_SetColor( NULL );
        UI_FillRect( heroX, 32, heroWidth, 410, heroOverlayColor );
        Frontend_DrawPanel( (int)heroX, 32, (int)heroWidth, 410,
                            s_main.visualAlpha, UI_FRONTEND_STYLE_FRAME );

        Frontend_DrawText( (int)( heroX + 16 ), 52, "Garage / active vehicle",
                           UI_LEFT | UI_SMALLFONT, mutedColor );
        Frontend_DrawStatusChip( (int)( heroX + heroWidth - 60 ), 52, "Ready",
                                 s_frontendAccent, s_main.visualAlpha );

        Menu_Draw( &s_main.menu );

        Frontend_DrawText( (int)( heroX + 16 ), 370, "Ready for the next rally",
                           UI_LEFT | UI_SMALLFONT, textColor );
        Frontend_DrawText( (int)( heroX + 16 ), 388,
                           va( "Model  -  %s", s_main.modelskin ),
                           UI_LEFT | UI_SMALLFONT, mutedColor );
        Frontend_DrawText( (int)( heroX + 16 ), 410, "Q3Rally  -  2002-2026",
                           UI_LEFT | UI_SMALLFONT, mutedColor );

        if (uis.demoversion) {

                UI_DrawProportionalString( 320, 440, "DEMO      FOR MATURE AUDIENCES      DEMO", UI_CENTER|UI_SMALLFONT, text_color_normal );
                Frontend_DrawText( 320, 456, Q3_VERSION " | www.q3rally.com | It's damn fast baby!",
                                   UI_CENTER | UI_SMALLFONT, text_color_normal );

        } else {

                Frontend_DrawText( 320, 456, Q3_VERSION " | www.q3rally.com | It's damn fast baby!",
                                   UI_CENTER | UI_SMALLFONT, text_color_normal );

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
        qboolean returnToConfig;


        returnToConfig = trap_Cvar_VariableValue( "q3r_ui_return_config" ) != 0.0f;
        if ( returnToConfig ) {
                trap_Cvar_Set( "q3r_ui_return_config", "0" );
        }


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
        s_main.banner.generic.ownerdraw                 = MainMenu_DrawBrand;
        s_main.banner.generic.x                         = 54;
        s_main.banner.generic.y                         = 48;
        s_main.banner.string                            = "Q3RALLY";
        s_main.banner.color                             = text_color_normal;
        s_main.banner.style                             = UI_LEFT|UI_DROPSHADOW;

        x = (int)MainMenu_NavX() + 8;
        y = 90;
        // Keep the navigation compact so the brand, menu and profile card read
        // as one focused left rail on wide screens.
        menuSpacing = 28;

        
	InitMenuText(&s_main.singleplayer, ID_SINGLEPLAYER, "OFFLINE", x, y + 12);


	y += menuSpacing;
	InitMenuText(&s_main.multiplayer, ID_MULTIPLAYER, "ONLINE", x, y + 12);


	y += menuSpacing;
	InitMenuText(&s_main.setup, ID_SETUP, "CONFIG", x, y + 12);


	y += menuSpacing;
	InitMenuText(&s_main.rivals, ID_RIVALS, "RIVALS", x, y + 12);
        
        
	y += menuSpacing;
	InitMenuText(&s_main.demos, ID_DEMOS, "DEMOS", x, y + 12);

        // Q3RALLY DOWNLOADS START
        y += menuSpacing;
        InitMenuText(&s_main.downloads, ID_DOWNLOADS, "DOWNLOADS", x, y + 12);
        // Q3RALLY DOWNLOADS END

        s_main.carlogo.generic.type                     = MTYPE_BITMAP;
        s_main.carlogo.generic.flags                    = QMF_INACTIVE;
        s_main.carlogo.generic.ownerdraw                = MainMenu_DrawPlayer;
        s_main.carlogo.generic.x                        = (int)( MainMenu_HeroX() + 40.0f );
        s_main.carlogo.generic.y                        = 92;
        s_main.carlogo.width                            = (int)( MainMenu_HeroWidth() - 120.0f );
        s_main.carlogo.height                           = 228;
        
	y += menuSpacing;
	InitMenuText(&s_main.exit, ID_EXIT, "QUIT", x, y + 12);


        y += menuSpacing;
        /* Give the profile tile a little more breathing room below the
         * navigation instead of making it feel like another menu item. */
        profileY = y + 42;
        profileInfoY = y + 36;
        InitMenuText(&s_main.profileAction, ID_PROFILE_ACTION, "CREATE", x, profileY);
        s_main.profileAction.generic.flags = QMF_RIGHT_JUSTIFY;
        s_main.profileAction.generic.ownerdraw = MainMenu_DrawProfileAction;

        Q_strncpyz( s_main.profileRankLine, "Rank: -", sizeof( s_main.profileRankLine ) );
        Q_strncpyz( s_main.profilePointsLine, "Points: 0", sizeof( s_main.profilePointsLine ) );
        InitMenuTextInfo(&s_main.profileInfoLine1, s_main.profileRankLine, x + 20, profileInfoY);
        InitMenuTextInfo(&s_main.profileInfoLine2, s_main.profilePointsLine, x + 20, profileInfoY + 16);
        MainMenu_UpdateProfileTexts();

        s_main.profileStatsAction.generic.type = MTYPE_PTEXT;
        s_main.profileStatsAction.generic.flags = QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
        s_main.profileStatsAction.generic.id = ID_PROFILE_STATS;
        s_main.profileStatsAction.generic.callback = Main_MenuEvent;
        s_main.profileStatsAction.generic.ownerdraw = MainMenu_DrawProfileStatAction;
        s_main.profileStatsAction.generic.x = (int)( MainMenu_HeroX() + MainMenu_HeroWidth() - 144.0f );
        s_main.profileStatsAction.generic.y = 354;
        s_main.profileStatsAction.generic.left = s_main.profileStatsAction.generic.x;
        s_main.profileStatsAction.generic.top = s_main.profileStatsAction.generic.y;
        s_main.profileStatsAction.generic.right = s_main.profileStatsAction.generic.x + 128;
        s_main.profileStatsAction.generic.bottom = s_main.profileStatsAction.generic.y + 58;
        s_main.profileStatsAction.string = "PROFILE STATS";
        s_main.profileStatsAction.style = UI_LEFT | UI_SMALLFONT;


        Menu_AddItem( &s_main.menu,     &s_main.banner );
        Menu_AddItem( &s_main.menu,     &s_main.carlogo );
        Menu_AddItem( &s_main.menu,     &s_main.singleplayer );
        Menu_AddItem( &s_main.menu,     &s_main.multiplayer );
        Menu_AddItem( &s_main.menu,     &s_main.setup );
        Menu_AddItem( &s_main.menu,     &s_main.rivals );
        Menu_AddItem( &s_main.menu,     &s_main.demos );
        // Q3RALLY DOWNLOADS START
        Menu_AddItem( &s_main.menu,     &s_main.downloads );
        // Q3RALLY DOWNLOADS END
        Menu_AddItem( &s_main.menu,     &s_main.exit );            
        Menu_AddItem( &s_main.menu,     &s_main.profileAction );
        Menu_AddItem( &s_main.menu,     &s_main.profileStatsAction );

        s_main.singleplayer.generic.ownerdraw = MainMenu_DrawNavItem;
        s_main.multiplayer.generic.ownerdraw = MainMenu_DrawNavItem;
        s_main.setup.generic.ownerdraw = MainMenu_DrawNavItem;
        s_main.rivals.generic.ownerdraw = MainMenu_DrawNavItem;
        s_main.demos.generic.ownerdraw = MainMenu_DrawNavItem;
        s_main.downloads.generic.ownerdraw = MainMenu_DrawNavItem;
        s_main.exit.generic.ownerdraw = MainMenu_DrawNavItem;

        MainMenu_SetInteractiveBounds( &s_main.singleplayer, (int)MainMenu_NavX(), (int)( MainMenu_NavX() + MainMenu_NavWidth() ) );
        MainMenu_SetInteractiveBounds( &s_main.multiplayer, (int)MainMenu_NavX(), (int)( MainMenu_NavX() + MainMenu_NavWidth() ) );
        MainMenu_SetInteractiveBounds( &s_main.setup, (int)MainMenu_NavX(), (int)( MainMenu_NavX() + MainMenu_NavWidth() ) );
        MainMenu_SetInteractiveBounds( &s_main.rivals, (int)MainMenu_NavX(), (int)( MainMenu_NavX() + MainMenu_NavWidth() ) );
        MainMenu_SetInteractiveBounds( &s_main.demos, (int)MainMenu_NavX(), (int)( MainMenu_NavX() + MainMenu_NavWidth() ) );
        MainMenu_SetInteractiveBounds( &s_main.downloads, (int)MainMenu_NavX(), (int)( MainMenu_NavX() + MainMenu_NavWidth() ) );
        MainMenu_SetInteractiveBounds( &s_main.exit, (int)MainMenu_NavX(), (int)( MainMenu_NavX() + MainMenu_NavWidth() ) );
        MainMenu_SetInteractiveBounds( &s_main.profileAction, (int)MainMenu_NavX(), (int)( MainMenu_NavX() + MainMenu_NavWidth() ) );
        s_main.profileAction.generic.top = profileY - 16;
        s_main.profileAction.generic.bottom = profileY + 66;

        trap_Key_SetCatcher( KEYCATCH_UI );
        uis.menusp = 0;

        UI_PushMenu ( &s_main.menu );

        /* Profile wizard — must run first. If no active profile exists, this
         * pushes the creation wizard on top of the main menu and blocks all
         * other auto-show logic until the player completes it.
         * UI_ProfileOverlay_MaybeShow and UI_LadderWizard_MaybeShow both
         * check for an active profile internally and will no-op when called
         * from here while the wizard is on screen. */
        UI_ProfileWizard_MaybeShow();

        UI_ProfileOverlay_MaybeShow();

        /* Q3RALLY LADDER: show offline tracking wizard for existing players
         * who update to v0.8 and have not yet registered. */
        UI_LadderWizard_MaybeShow();

        if ( returnToConfig && uis.activemenu == &s_main.menu ) {
                UI_SetupMenu();
        }

        if ( uis.activemenu == &s_main.menu ) {
                uis.transitionIn = uis.realtime;
        }

}
