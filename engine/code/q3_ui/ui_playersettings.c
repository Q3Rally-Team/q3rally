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
#include "../game/bg_achievements.h"

// STONELANCE
/*
#define ART_FRAMEL			"menu/art/frame2_l"
#define ART_FRAMER			"menu/art/frame1_r"
#define ART_MODEL0			"menu/art/model_0"
#define ART_MODEL1			"menu/art/model_1"
#define ART_BACK0			"menu/art/back_0"
#define ART_BACK1			"menu/art/back_1"
*/
#define ART_SELECT			"menu/art/menu_select"
#define ART_SELECTED		"menu/art/menu_selected"
#define ART_PORT			"menu/art/menu_port"
#define ART_LEFT0			"menu/art/arrow_l0"
#define ART_LEFT1			"menu/art/arrow_l1"
#define ART_RIGHT0			"menu/art/arrow_r0"
#define ART_RIGHT1			"menu/art/arrow_r1"
// END
#define ART_FX_BASE			"menu/art/fx_base"
#define ART_FX_BLUE			"menu/art/fx_blue"
#define ART_FX_CYAN			"menu/art/fx_cyan"
#define ART_FX_GREEN		"menu/art/fx_grn"
#define ART_FX_RED			"menu/art/fx_red"
#define ART_FX_TEAL			"menu/art/fx_teal"
#define ART_FX_WHITE		"menu/art/fx_white"
#define ART_FX_YELLOW		"menu/art/fx_yel"
#define ART_MEDAL_DRIVEN_LOCKED         "menu/achievements/medal_driven_locked"
#define ART_MEDAL_DRIVEN_UNLOCKED       "menu/achievements/medal_driven_unlocked"
#define ART_MEDAL_KILLS_LOCKED          "menu/achievements/medal_kills_locked"
#define ART_MEDAL_KILLS_UNLOCKED        "menu/achievements/medal_kills_unlocked"
#define ART_MEDAL_WINS_LOCKED           "menu/achievements/medal_wins_locked"
#define ART_MEDAL_WINS_UNLOCKED         "menu/achievements/medal_wins_unlocked"
#define ART_MEDAL_FLAGS_LOCKED          "menu/achievements/medal_flags_locked"
#define ART_MEDAL_FLAGS_UNLOCKED        "menu/achievements/medal_flags_unlocked"
#define ART_MEDAL_ASSISTS_LOCKED        "menu/achievements/medal_assists_locked"
#define ART_MEDAL_ASSISTS_UNLOCKED      "menu/achievements/medal_assists_unlocked"
#define ART_MEDAL_FUEL_LOCKED           "menu/achievements/medal_fuel_locked"
#define ART_MEDAL_FUEL_UNLOCKED         "menu/achievements/medal_fuel_unlocked"
#define ART_MEDAL_ACCURACY_LOCKED       "menu/achievements/medal_accuracy_locked"
#define ART_MEDAL_ACCURACY_UNLOCKED     "menu/achievements/medal_accuracy_unlocked"
#define ART_MEDAL_EXCELLENT_LOCKED      "menu/achievements/medal_excellent_locked"
#define ART_MEDAL_EXCELLENT_UNLOCKED    "menu/achievements/medal_excellent_unlocked"
#define ART_MEDAL_IMPRESSIVE_LOCKED     "menu/achievements/medal_impressive_locked"
#define ART_MEDAL_IMPRESSIVE_UNLOCKED   "menu/achievements/medal_impressive_unlocked"
#define ART_MEDAL_PERFECT_LOCKED        "menu/achievements/medal_perfect_locked"
#define ART_MEDAL_PERFECT_UNLOCKED      "menu/achievements/medal_perfect_unlocked"

#define ID_NAME			10
#define ID_HANDICAP		11
#define ID_EFFECTS		12
#define ID_BACK			13
// STONELANCE
// #define ID_MODEL		14
#define ID_CUSTOMIZE	14

#define ID_FAVORITE1	15
#define ID_FAVORITE2	16
#define ID_FAVORITE3	17
#define ID_FAVORITE4	18
#define ID_LEFT			19
#define ID_RIGHT		20
#define ID_PLATE		21
#define ID_COUNTRY		22
#define ID_AVATAR		23
// END

#define ID_TAB_PROFILE		30
#define ID_TAB_VEHICLE		31
#define ID_TAB_STATS		32
#define ID_TAB_ACHIEVEMENTS	33
#define ID_GENDER		40
#define ID_BIRTH_DAY		41
#define ID_BIRTH_MONTH		42
#define ID_BIRTH_YEAR		43
#define ID_AVATAR_IMPORT_PATH	50
#define ID_AVATAR_IMPORT_CONFIRM	51
#define ID_AVATAR_IMPORT_CANCEL	52

#define TAB_PROFILE		0
#define TAB_VEHICLE		1
#define TAB_STATS		2
#define TAB_ACHIEVEMENTS	3

#define PLAYERSETTINGS_TAB_COUNT		4
#define PLAYERSETTINGS_FRAME_X		24
#define PLAYERSETTINGS_FRAME_Y		20
#define PLAYERSETTINGS_FRAME_WIDTH	592
#define PLAYERSETTINGS_FRAME_HEIGHT	440
#define PLAYERSETTINGS_TAB_WIDTH		128
#define PLAYERSETTINGS_TAB_GAP		4
#define PLAYERSETTINGS_TAB_TOP		88
#define PLAYERSETTINGS_TAB_HEIGHT	26
#define PLAYERSETTINGS_TAB_TEXT_OFFSET	5
#define PLAYERSETTINGS_CONTENT_TOP	( PLAYERSETTINGS_TAB_TOP + PLAYERSETTINGS_TAB_HEIGHT + 12 )

#define PLAYERSETTINGS_PROFILE_PANEL_LEFT		48
#define PLAYERSETTINGS_PROFILE_PANEL_TOP		126
#define PLAYERSETTINGS_PROFILE_PANEL_WIDTH		544
#define PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN	8
#define PLAYERSETTINGS_PROFILE_PANEL_BOTTOM_EXTRA	12
#define PLAYERSETTINGS_PROFILE_FORM_X		48
#define PLAYERSETTINGS_PROFILE_FORM_Y		126
#define PLAYERSETTINGS_PROFILE_FORM_WIDTH		292
#define PLAYERSETTINGS_PROFILE_FORM_HEIGHT		294
#define PLAYERSETTINGS_PROFILE_FORM_RIGHT		( PLAYERSETTINGS_PROFILE_FORM_X + PLAYERSETTINGS_PROFILE_FORM_WIDTH - 8 )
#define PLAYERSETTINGS_PROFILE_PRESENCE_X		356
#define PLAYERSETTINGS_PROFILE_PRESENCE_Y		126
#define PLAYERSETTINGS_PROFILE_PRESENCE_WIDTH	236
#define PLAYERSETTINGS_PROFILE_PRESENCE_HEIGHT	294
#define PLAYERSETTINGS_PROFILE_FIELD_LEFT		( PLAYERSETTINGS_PROFILE_PANEL_LEFT + PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN )
#define PLAYERSETTINGS_PROFILE_ROW_RIGHT		( PLAYERSETTINGS_PROFILE_PANEL_LEFT + PLAYERSETTINGS_PROFILE_PANEL_WIDTH - PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN )
#define PLAYERSETTINGS_PROFILE_LABEL_OFFSET	16
#define PLAYERSETTINGS_PROFILE_VALUE_OFFSET	128
#define PLAYERSETTINGS_PROFILE_VALUE_BASELINE	18
#define PLAYERSETTINGS_PROFILE_FIELD_HEIGHT	32
#define PLAYERSETTINGS_PROFILE_ROW_HEIGHT		36

#define PLAYERSETTINGS_BACK_BUTTON_LEFT			48
#define PLAYERSETTINGS_BACK_BUTTON_Y			440

#define PLAYERSETTINGS_MAX_ACHIEVEMENT_TIERS            8
#define PLAYERSETTINGS_ACHIEVEMENTS_PER_LINE            2
#define PLAYERSETTINGS_ACHIEVEMENTS_PER_PAGE            4
#define PLAYERSETTINGS_ACHIEVEMENT_MEDAL_SIZE           60.0f
#define PLAYERSETTINGS_ACHIEVEMENT_ROW_GAP              16
#define PLAYERSETTINGS_ACHIEVEMENT_TITLE_OFFSET         6
#define PLAYERSETTINGS_ACHIEVEMENT_HEADER_LINE_HEIGHT   32.0f
#define PLAYERSETTINGS_ACHIEVEMENT_HEADER_GAP           18.0f
#define PLAYERSETTINGS_ACHIEVEMENT_ENTRY_VERTICAL_GAP   24.0f
#define PLAYERSETTINGS_ACHIEVEMENT_COLUMN_GAP           32.0f
#define PLAYERSETTINGS_ACHIEVEMENT_TEXT_GAP             16.0f
#define PLAYERSETTINGS_ACHIEVEMENT_ENTRY_ROWS_PER_PAGE  (( PLAYERSETTINGS_ACHIEVEMENTS_PER_PAGE + PLAYERSETTINGS_ACHIEVEMENTS_PER_LINE - 1 ) / PLAYERSETTINGS_ACHIEVEMENTS_PER_LINE )
#define PLAYERSETTINGS_ACHIEVEMENT_ROW_HEIGHT           ( PLAYERSETTINGS_ACHIEVEMENT_HEADER_LINE_HEIGHT + PLAYERSETTINGS_ACHIEVEMENT_HEADER_GAP + PLAYERSETTINGS_ACHIEVEMENT_ENTRY_ROWS_PER_PAGE * PLAYERSETTINGS_ACHIEVEMENT_MEDAL_SIZE + ( PLAYERSETTINGS_ACHIEVEMENT_ENTRY_ROWS_PER_PAGE - 1 ) * PLAYERSETTINGS_ACHIEVEMENT_ENTRY_VERTICAL_GAP )

#define PLAYERSETTINGS_STATS_ROW_HEIGHT		36
#define PLAYERSETTINGS_STATS_ROW_GAP		4
#define PLAYERSETTINGS_STATS_VALUE_OFFSET		260
#define PLAYERSETTINGS_STATS_VALUE_BASELINE		PLAYERSETTINGS_PROFILE_VALUE_BASELINE
#define PLAYERSETTINGS_STATS_CARD_BOTTOM		PLAYERSETTINGS_BACK_BUTTON_Y
#define PLAYERSETTINGS_STATS_CARD_CONTENT_HEIGHT \
	( PLAYERSETTINGS_STATS_CARD_BOTTOM - PLAYERSETTINGS_PROFILE_PANEL_TOP \
	  - PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN - 2 \
	  - PLAYERSETTINGS_PROFILE_PANEL_BOTTOM_EXTRA )

#define PLAYERSETTINGS_ACHIEVEMENT_CATEGORY_COUNT       BG_ACHIEVEMENT_CATEGORY_COUNT
#define PLAYERSETTINGS_ACHIEVEMENT_FIRST_SECTION_ROW    0
#define PLAYERSETTINGS_ACHIEVEMENT_SECTION_COUNT        ( PLAYERSETTINGS_ACHIEVEMENT_CATEGORY_COUNT * 2 )
#define PLAYERSETTINGS_ACHIEVEMENT_ROW_COUNT            PLAYERSETTINGS_ACHIEVEMENT_SECTION_COUNT
#define PLAYERSETTINGS_ACHIEVEMENT_CONTENT_MARGIN	0.0f

#define MAX_NAMELENGTH	20
// STONELANCE
#define NUM_FAVORITES		4
#define MAX_PLAYERMODELS	256
#define BIRTH_YEAR_START	1950
#define BIRTH_YEAR_END	2100
#define BIRTH_YEAR_COUNT	( ( BIRTH_YEAR_END ) - ( BIRTH_YEAR_START ) + 1 )
#define BIRTH_DAY_MAX	31
// END


static vec4_t profileRowEvenFillColor = { 0.10f, 0.10f, 0.10f, 0.60f };
static vec4_t profileRowOddFillColor = { 0.14f, 0.14f, 0.14f, 0.60f };
static vec4_t profileRowBorderColor = { 0.35f, 0.35f, 0.35f, 0.85f };
static vec4_t avatarImageBackgroundColor = { 0.05f, 0.05f, 0.05f, 0.80f };
static vec4_t avatarImageMissingColor = { 0.6f, 0.2f, 0.2f, 1.0f };
static vec4_t playerSettingsScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t playerSettingsTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t playerSettingsMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t playerSettingsAccentColor = UI_FRONTEND_COLOR_ACCENT;
static vec4_t playerSettingsStatusColor = UI_FRONTEND_COLOR_ACCENT;

typedef struct {
	double		threshold;
	const char		*name;
	const char		*description;
} playersettingsAchievementTierDef_t;

static const playersettingsAchievementTierDef_t s_distanceAchievementTiers[] = {
        { 10.0, "Sunday Driver", "Cover a relaxed 10 km overall." },
        { 50.0, "Daily Commuter", "Cruise through a combined 50 km." },
        { 150.0, "Road Tripper", "Accumulate 150 km behind the wheel." },
        { 300.0, "Night Rider", "Push past 300 km of distance." },
        { 600.0, "Highway Hero", "Rack up 600 km on the odometer." },
        { 1500.0, "Endurance Ace", "Stay in the race for 1,500 km total." },
        { 5000.0, "Globetrotter", "Log an epic 5,000 km journey." },
        { 10000.0, "Legend of Asphalt", "Master the asphalt for 10,000 km." }
};

static const playersettingsAchievementTierDef_t s_killAchievementTiers[] = {
        { 10.0, "Spark Starter", "Score 10 takedowns." },
        { 25.0, "Arc Blazer", "Deliver 25 total kills." },
        { 50.0, "Demolition Driver", "Knock rivals out 50 times." },
        { 100.0, "Pit Boss", "Dominate the arena with 100 kills." },
        { 250.0, "Arena Menace", "Send 250 opponents packing." },
        { 500.0, "Road Reaper", "Leave 500 wrecks behind." },
        { 1000.0, "Overdrive Executioner", "Achieve 1,000 eliminations." },
        { 2500.0, "Apocalypse Engine", "Crush 2,500 opponents." }
};

static const playersettingsAchievementTierDef_t s_winAchievementTiers[] = {
    { 1.0, "Checkered Debut", "Win your very first race." },
    { 3.0, "Podium Regular", "Collect 3 total victories." },
    { 10.0, "Championship Hopeful", "Secure 10 race wins." },
    { 20.0, "Series Star", "Reach 20 gold finishes." },
    { 30.0, "Circuit Royalty", "Earn 30 overall wins." },
    { 40.0, "Dynasty Driver", "Stack up 40 victories." },
    { 50.0, "Hall of Fame", "Celebrate 50 race wins." },
    { 100.0, "Centennial Champion", "Claim 100 career victories." }
};

static const playersettingsAchievementTierDef_t s_sprintWinAchievementTiers[] = {
    { 1.0, "Sprint Rookie", "Win your first sprint race." },
    { 3.0, "Quick Podiums", "Collect 3 sprint victories." },
    { 10.0, "Sprint Specialist", "Win 10 sprint races." },
    { 20.0, "Speed Series Star", "Reach 20 sprint wins." },
    { 30.0, "Dash Dominator", "Earn 30 sprint victories." },
    { 40.0, "Momentum Master", "Stack up 40 sprint wins." },
    { 50.0, "Sprint Hall of Fame", "Celebrate 50 sprint wins." },
    { 100.0, "Sprint Century", "Bring home 100 sprint victories." }
};

static const playersettingsAchievementTierDef_t s_flagCaptureAchievementTiers[] = {
        { 1.0, "Flag Rookie", "Capture your first flag." },
        { 5.0, "Fast Courier", "Deliver 5 flags to base." },
        { 10.0, "Relay Racer", "Bank 10 successful captures." },
        { 25.0, "Siege Runner", "Snatch 25 flags." },
        { 50.0, "Banner Bandit", "Swipe 50 flags." },
        { 75.0, "Frontline Phantom", "Steal 75 flags unnoticed." },
        { 100.0, "Flagship", "Secure 100 captures." },
        { 150.0, "Mythic Messenger", "Run home 150 flags." }
};

static const playersettingsAchievementTierDef_t s_flagAssistAchievementTiers[] = {
        { 1.0, "Helping Hand", "Assist with 1 flag capture." },
        { 5.0, "Wingman", "Support 5 flag scores." },
        { 10.0, "Shield Mate", "Help with 10 captures." },
        { 25.0, "Escort Elite", "Escort 25 flags safely." },
        { 50.0, "Guardian Angel", "Guide 50 flags home." },
        { 75.0, "Formation Leader", "Add 75 total assists." },
        { 100.0, "Tactical Anchor", "Reach 100 flag assists." },
        { 150.0, "Legendary Support", "Record 150 flag assists." }
};

static const playersettingsAchievementTierDef_t s_fuelAchievementTiers[] = {
        { 10.0, "Fuel Sipper", "Burn through 10 L of fuel." },
        { 50.0, "Tank Tipper", "Spend 50 L on the throttle." },
        { 100.0, "Octane Addict", "Consume 100 L overall." },
        { 250.0, "Combustion Captain", "Use 250 L chasing speed." },
        { 500.0, "Turbo Baron", "Torch 500 L of fuel." },
        { 1000.0, "Inferno Investor", "Pour 1,000 L into momentum." },
        { 2500.0, "Petrol Pharaoh", "Spend 2,500 L keeping pace." },
        { 5000.0, "Galaxy Guzzler", "Atomize 5,000 L in total." }
};

static const playersettingsAchievementTierDef_t s_accuracyAchievementTiers[] = {
        { 1.0, "Sharpshooter", "Finish a match with 75% accuracy." },
        { 5.0, "Deadeye", "Post 75% accuracy in 5 matches." },
        { 10.0, "Pinpoint", "Hit the 75% mark in 10 matches." },
        { 20.0, "Bullseye", "Hold 75% accuracy across 20 matches." },
        { 35.0, "Laser Focus", "Reach 75% accuracy in 35 matches." },
        { 50.0, "Hawkeye", "Log 50 high-accuracy matches." },
        { 75.0, "True Aim", "Strike 75% accuracy in 75 matches." },
        { 100.0, "Mark V Master", "Break 100 matches with 75% accuracy." }
};

static const playersettingsAchievementTierDef_t s_excellentAchievementTiers[] = {
        { 1.0, "Quick Combo", "Earn a double kill medal." },
        { 5.0, "Chain Reaction", "Score 5 Excellent medals." },
        { 10.0, "Volley Driver", "Rack up 10 Excellent medals." },
        { 20.0, "Momentum Maker", "Stack 20 double kills." },
        { 35.0, "Blitz Conductor", "Collect 35 Excellent medals." },
        { 50.0, "Overdrive Duellist", "Secure 50 Excellent medals." },
        { 75.0, "Carnage Maestro", "Reach 75 Excellent medals." },
        { 100.0, "Legendary Combo", "Achieve 100 Excellent medals." }
};

static const playersettingsAchievementTierDef_t s_impressiveAchievementTiers[] = {
        { 1.0, "Rail Pair", "Land 2 rail hits in a row." },
        { 5.0, "Polished Aim", "Earn 5 Impressive medals." },
        { 10.0, "Steel Focus", "Earn 10 Impressive medals." },
        { 20.0, "Ion Sight", "Collect 20 Impressive medals." },
        { 35.0, "Rail Virtuoso", "Collect 35 Impressive medals." },
        { 50.0, "Beam Savant", "Earn 50 Impressive medals." },
        { 75.0, "Arc Maestro", "Secure 75 Impressive medals." },
        { 100.0, "Impeccable", "Reach 100 Impressive medals." }
};

static const playersettingsAchievementTierDef_t s_perfectAchievementTiers[] = {
        { 1.0, "Unscathed", "Finish a match without dying." },
        { 3.0, "Untouched", "Win 3 matches without dying." },
        { 5.0, "Flawless", "Win 5 matches without dying." },
        { 10.0, "Immaculate", "Finish 10 matches death-free." },
        { 20.0, "Ghost Driver", "Complete 20 matches without a death." },
        { 35.0, "Untouchable", "Survive 35 matches without falling." },
        { 50.0, "Perfect Storm", "End 50 matches without dying." },
        { 75.0, "Mythic Survivor", "Reach 75 perfect finishes." }
};

typedef enum {
        PLAYERSETTINGS_ACHIEVEMENT_ICON_DRIVEN = 0,
        PLAYERSETTINGS_ACHIEVEMENT_ICON_KILLS,
        PLAYERSETTINGS_ACHIEVEMENT_ICON_WINS,
        PLAYERSETTINGS_ACHIEVEMENT_ICON_FLAGS,
        PLAYERSETTINGS_ACHIEVEMENT_ICON_FLAG_ASSISTS,
        PLAYERSETTINGS_ACHIEVEMENT_ICON_FUEL,
        PLAYERSETTINGS_ACHIEVEMENT_ICON_ACCURACY,
        PLAYERSETTINGS_ACHIEVEMENT_ICON_EXCELLENT,
        PLAYERSETTINGS_ACHIEVEMENT_ICON_IMPRESSIVE,
        PLAYERSETTINGS_ACHIEVEMENT_ICON_PERFECT,
        PLAYERSETTINGS_ACHIEVEMENT_ICON_COUNT
} playersettingsAchievementIcon_t;

typedef struct playersettings_pagination_state_s {
	int	currentPage;
} playersettingsPaginationState_t;

typedef struct playersettings_pagination_info_s {
	int	rowCount;
	int	rowsPerPage;
	int	totalPages;
	int	firstRow;
	int	lastRow;
	float	rowOffset;
} playersettingsPaginationInfo_t;

typedef struct playersettings_rect_s {
	float	x;
	float	y;
	float	w;
	float	h;
} playersettingsRect_t;

static void PlayerSettings_DrawStatsLabelValue( int row, const char *label, const char *value );
static void PlayerSettings_DrawStatsMessage( int row, const char *message );
static void PlayerSettings_DrawAchievementsPanelBackground( void );
static void PlayerSettings_DrawAchievementsTab( void );
static void PlayerSettings_ClampAchievementTierPage( void );
static void PlayerSettings_GetAchievementRowBounds( int row, int *top, int *bottom );
static const playersettingsPaginationInfo_t *PlayerSettings_UpdateStatsPaginationInfo( void );
static const playersettingsPaginationInfo_t *PlayerSettings_UpdateAchievementsPaginationInfo( void );
static void PlayerSettings_DrawBackItem( void *self );
static void PlayerSettings_DrawPlateItem( void *self );
static void PlayerSettings_DrawModernField( void *self );
static void PlayerSettings_DrawModernChoice( void *self );
static void PlayerSettings_DrawModernBirthDate( void *self );
static void PlayerSettings_DrawModernEffects( void *self );
static void PlayerSettings_DrawAvatarImportField( void *self );
static void PlayerSettings_DrawAvatarImportButton( void *self );
static void PlayerSettings_AvatarImportMenuEvent( void *ptr, int event );
static sfxHandle_t PlayerSettings_AvatarImportKey( int key );
static void PlayerSettings_AvatarImportDraw( void );
static void PlayerSettings_OpenAvatarImport( void );
static void PlayerSettings_DrawVehicleControl( void *self );
static void PlayerSettings_DrawFavoriteButton( void *self );
static void PlayerSettings_DrawVehiclePanelBackground( void );

static const char *const s_achievementMedalLockedPaths[PLAYERSETTINGS_ACHIEVEMENT_ICON_COUNT] = {
        ART_MEDAL_DRIVEN_LOCKED,
        ART_MEDAL_KILLS_LOCKED,
        ART_MEDAL_WINS_LOCKED,
        ART_MEDAL_FLAGS_LOCKED,
        ART_MEDAL_ASSISTS_LOCKED,
        ART_MEDAL_FUEL_LOCKED,
        ART_MEDAL_ACCURACY_LOCKED,
        ART_MEDAL_EXCELLENT_LOCKED,
        ART_MEDAL_IMPRESSIVE_LOCKED,
        ART_MEDAL_PERFECT_LOCKED
};
static const char *const s_achievementMedalUnlockedPaths[PLAYERSETTINGS_ACHIEVEMENT_ICON_COUNT] = {
        ART_MEDAL_DRIVEN_UNLOCKED,
        ART_MEDAL_KILLS_UNLOCKED,
        ART_MEDAL_WINS_UNLOCKED,
        ART_MEDAL_FLAGS_UNLOCKED,
        ART_MEDAL_ASSISTS_UNLOCKED,
        ART_MEDAL_FUEL_UNLOCKED,
        ART_MEDAL_ACCURACY_UNLOCKED,
        ART_MEDAL_EXCELLENT_UNLOCKED,
        ART_MEDAL_IMPRESSIVE_UNLOCKED,
        ART_MEDAL_PERFECT_UNLOCKED
};

static qhandle_t PlayerSettings_RegisterAchievementMedal( const char *basePath ) {
        static const char *const s_extensions[] = { ".tga", ".jpg", ".png" };
        char assetPath[MAX_QPATH];
        int i;

        for ( i = 0; i < ARRAY_LEN( s_extensions ); ++i ) {
                fileHandle_t file;
                int length;

                Q_strncpyz( assetPath, basePath, sizeof( assetPath ) );
                Q_strcat( assetPath, sizeof( assetPath ), s_extensions[i] );

                file = 0;
                length = trap_FS_FOpenFile( assetPath, &file, FS_READ );
                if ( length > 0 ) {
                        trap_FS_FCloseFile( file );
                        return trap_R_RegisterShaderNoMip( assetPath );
                }

                if ( file ) {
                        trap_FS_FCloseFile( file );
                }
        }

        return 0;
}

#define PLAYERSETTINGS_DISPLAY_ACHIEVEMENT_TOTAL        BG_ACHIEVEMENT_TOTAL_COUNT
#define PLAYERSETTINGS_PAGINATION_BUTTON_WIDTH          96.0f
#define PLAYERSETTINGS_PAGINATION_BUTTON_HEIGHT         24.0f
#define PLAYERSETTINGS_PAGINATION_BUTTON_GAP            80.0f
#define PLAYERSETTINGS_PAGINATION_BUTTON_MARGIN         4.0f
#define PLAYERSETTINGS_ACHIEVEMENT_TIER_BUTTON_WIDTH    48.0f
#define PLAYERSETTINGS_ACHIEVEMENT_TIER_BUTTON_HEIGHT   20.0f
#define PLAYERSETTINGS_ACHIEVEMENT_TIER_BUTTON_GAP      8.0f
#define PLAYERSETTINGS_STATS_PAGINATION_RESERVED_HEIGHT \
        ( PLAYERSETTINGS_PAGINATION_BUTTON_HEIGHT + PLAYERSETTINGS_PAGINATION_BUTTON_MARGIN * 2 )
#define PLAYERSETTINGS_ACHIEVEMENTS_PAGINATION_RESERVED_HEIGHT \
        ( PLAYERSETTINGS_PAGINATION_BUTTON_HEIGHT + PLAYERSETTINGS_PAGINATION_BUTTON_MARGIN * 2 )

static const char *const s_genderItems[] = {
        "Unspecified",
        "Female",
        "Male",
        "Non-binary",
        "Other",
        NULL
};

static const char *const s_birthMonthItems[] = {
        "-",
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December",
        NULL
};

static const char *s_birthDayItems[BIRTH_DAY_MAX + 2];
static char s_birthDayStrings[BIRTH_DAY_MAX + 1][3];

static const char *s_birthYearItems[BIRTH_YEAR_COUNT + 2];
static char s_birthYearStrings[BIRTH_YEAR_COUNT][5];

static qboolean s_birthDateListsInitialized;

static const char *s_statsLabelCurrentRank = "Current rank";
static const char *s_statsLabelNextRank = "Next rank";

typedef enum {
        PROFILE_ROW_NAME = 0,
        PROFILE_ROW_GENDER,
        PROFILE_ROW_BIRTHDATE,
        PROFILE_ROW_AVATAR,
        PROFILE_ROW_COUNTRY,
        PROFILE_ROW_HANDICAP,
        PROFILE_ROW_EFFECTS,
        PROFILE_ROW_COUNT
} profileRow_t;

typedef enum {
STATS_ROW_PLAYER_SCORE = 0,
STATS_ROW_CURRENT_RANK,
STATS_ROW_NEXT_RANK,
STATS_ROW_GAMES_PLAYED,
STATS_ROW_DISTANCE,
STATS_ROW_FUEL,
STATS_ROW_TOP_SPEED,
STATS_ROW_BEST_LAP,
STATS_ROW_KILLS,
STATS_ROW_DAMAGE_DEALT,
STATS_ROW_DAMAGE_TAKEN,
STATS_ROW_WINS,
STATS_ROW_FLAGS_CAPTURED,
STATS_ROW_FLAG_ASSISTS,
STATS_ROW_AWARDS,
STATS_ROW_VEHICLE,
/* ── GT_RACING ─────────────────────── */
STATS_ROW_RACING_HEADER,
STATS_ROW_RACING_WINS,
STATS_ROW_RACING_PODIUMS,
STATS_ROW_RACING_COMPLETED,
/* ── GT_RACING_DM ──────────────────── */
STATS_ROW_RACING_DM_HEADER,
STATS_ROW_RACING_DM_WINS,
STATS_ROW_RACING_DM_PODIUMS,
STATS_ROW_RACING_DM_COMPLETED,
/* ── GT_SPRINT ─────────────────────── */
STATS_ROW_SPRINT_HEADER,
STATS_ROW_SPRINT_WINS,
STATS_ROW_SPRINT_COMPLETED,
STATS_ROW_SPRINT_BEST,
/* ── GT_DERBY ──────────────────────── */
STATS_ROW_DERBY_HEADER,
STATS_ROW_DERBY_WINS,
STATS_ROW_DERBY_COMPLETED,
STATS_ROW_DERBY_KILLS,
/* ── GT_LCS ────────────────────────── */
STATS_ROW_LCS_HEADER,
STATS_ROW_LCS_WINS,
STATS_ROW_LCS_COMPLETED,
/* ── GT_ELIMINATION ────────────────── */
STATS_ROW_ELIM_HEADER,
STATS_ROW_ELIM_WINS,
STATS_ROW_ELIM_COMPLETED,
STATS_ROW_ELIM_ROUNDS,
/* ── GT_DEATHMATCH ─────────────────── */
STATS_ROW_DM_HEADER,
STATS_ROW_DM_WINS,
STATS_ROW_DM_COMPLETED,
STATS_ROW_DM_KILLS,
/* ── GT_TEAM ───────────────────────── */
STATS_ROW_TEAM_HEADER,
STATS_ROW_TEAM_WINS,
STATS_ROW_TEAM_COMPLETED,
STATS_ROW_TEAM_KILLS,
/* ── GT_TEAM_RACING ────────────────── */
STATS_ROW_TEAM_RACING_HEADER,
STATS_ROW_TEAM_RACING_WINS,
STATS_ROW_TEAM_RACING_PODIUMS,
STATS_ROW_TEAM_RACING_COMPLETED,
/* ── GT_TEAM_RACING_DM ─────────────── */
STATS_ROW_TEAM_RACING_DM_HEADER,
STATS_ROW_TEAM_RACING_DM_WINS,
STATS_ROW_TEAM_RACING_DM_PODIUMS,
STATS_ROW_TEAM_RACING_DM_COMPLETED,
/* ── GT_CTF ────────────────────────── */
STATS_ROW_CTF_HEADER,
STATS_ROW_CTF_WINS,
STATS_ROW_CTF_COMPLETED,
STATS_ROW_CTF_CAPTURES,
/* ── GT_CTF4 ───────────────────────── */
STATS_ROW_CTF4_HEADER,
STATS_ROW_CTF4_WINS,
STATS_ROW_CTF4_COMPLETED,
STATS_ROW_CTF4_CAPTURES,
/* ── GT_DOMINATION ─────────────────── */
STATS_ROW_DOM_HEADER,
STATS_ROW_DOM_WINS,
STATS_ROW_DOM_COMPLETED,
/* ── GT_KOTH ───────────────────────── */
STATS_ROW_KOTH_HEADER,
STATS_ROW_KOTH_WINS,
STATS_ROW_KOTH_COMPLETED,
STATS_ROW_COUNT
} statsRow_t;

typedef struct {
	const char *title;
	int firstRow;
	int lastRow;
} playersettingsStatsCategory_t;

/* Keep each category short enough to fit as a focused, single page. */
static const playersettingsStatsCategory_t s_statsCategories[] = {
	{ "Overview", STATS_ROW_PLAYER_SCORE, STATS_ROW_FUEL },
	{ "Performance", STATS_ROW_TOP_SPEED, STATS_ROW_BEST_LAP },
	{ "Combat", STATS_ROW_KILLS, STATS_ROW_WINS },
	{ "Objectives", STATS_ROW_FLAGS_CAPTURED, STATS_ROW_VEHICLE },
	{ "Racing", STATS_ROW_RACING_WINS, STATS_ROW_RACING_COMPLETED },
	{ "Racing DM", STATS_ROW_RACING_DM_WINS, STATS_ROW_RACING_DM_COMPLETED },
	{ "Sprint", STATS_ROW_SPRINT_WINS, STATS_ROW_SPRINT_BEST },
	{ "Derby", STATS_ROW_DERBY_WINS, STATS_ROW_DERBY_KILLS },
	{ "Last Car Standing", STATS_ROW_LCS_WINS, STATS_ROW_LCS_COMPLETED },
	{ "Elimination", STATS_ROW_ELIM_WINS, STATS_ROW_ELIM_ROUNDS },
	{ "Deathmatch", STATS_ROW_DM_WINS, STATS_ROW_DM_KILLS },
	{ "Team Deathmatch", STATS_ROW_TEAM_WINS, STATS_ROW_TEAM_KILLS },
	{ "Team Racing", STATS_ROW_TEAM_RACING_WINS, STATS_ROW_TEAM_RACING_COMPLETED },
	{ "Team Racing DM", STATS_ROW_TEAM_RACING_DM_WINS, STATS_ROW_TEAM_RACING_DM_COMPLETED },
	{ "Capture the Flag", STATS_ROW_CTF_WINS, STATS_ROW_CTF_CAPTURES },
	{ "4-Team CTF", STATS_ROW_CTF4_WINS, STATS_ROW_CTF4_CAPTURES },
	{ "Domination", STATS_ROW_DOM_WINS, STATS_ROW_DOM_COMPLETED },
	{ "King of the Hill", STATS_ROW_KOTH_WINS, STATS_ROW_KOTH_COMPLETED }
};

typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menutext_s			tabProfile;
	menutext_s			tabVehicle;
	menutext_s			tabStats;
	menutext_s			tabAchievements;
// STONELANCE
/*
	menubitmap_s		framel;
	menubitmap_s		framer;
*/
// END
	menubitmap_s		player;

	menufield_s			name;
	menulist_s			gender;
	menutext_s			birthDateLabel;
	menulist_s			birthDay;
	menulist_s			birthMonth;
	menulist_s			birthYear;
	menufield_s			avatar;
	menulist_s			country;
	menulist_s			handicap;
	menulist_s			effects;

// STONELANCE
//	menubitmap_s		back;
	menutext_s			back;
	menutext_s			customize;
	menutext_s			plate;

	menubitmap_s		left;
	menutext_s			modelname;
	menubitmap_s		right;

	menutext_s			favorites;
	menubitmap_s		favpics[NUM_FAVORITES];
	menubitmap_s		favpicbuttons[NUM_FAVORITES];
	menubitmap_s		ports[NUM_FAVORITES];

	char				modelList[MAX_PLAYERMODELS][MAX_QPATH];
	int					selectedModel;
	int					numModels;
	int					allModels;

	char				modelskin[MAX_QPATH];
	char				rimskin[MAX_QPATH];
	char				headskin[MAX_QPATH];

	char				favIcons[NUM_FAVORITES][MAX_QPATH];
	qboolean			modelChanged;
// END

	qhandle_t			fxBasePic;
	qhandle_t			fxPic[7];
        qhandle_t                       achievementMedalLocked[PLAYERSETTINGS_ACHIEVEMENT_ICON_COUNT];
        qhandle_t                       achievementMedalUnlocked[PLAYERSETTINGS_ACHIEVEMENT_ICON_COUNT];
	playerInfo_t		playerinfo;
	int					current_fx;
	char				playerModel[MAX_QPATH];
	int					currentTab;
	profile_info_t	profileInfo;
	qhandle_t	avatarShader;
	qboolean	avatarShaderInitialized;
	char		avatarShaderName[MAX_QPATH];
	char		avatarProfileName[PROFILE_MAX_NAME];
	char		avatarDisplayPath[MAX_OSPATH];
	char		avatarActionLine[64];
	menuframework_s	avatarImportMenu;
	menufield_s		avatarImportPath;
	menutext_s		avatarImportConfirm;
	menutext_s		avatarImportCancel;
	char		avatarImportStatus[128];
	playersettingsPaginationState_t	statsPagination;
	playersettingsPaginationState_t	achievementsPagination;
	playersettingsPaginationState_t	achievementsTierPagination;
	playersettingsPaginationInfo_t	statsPaginationInfo;
	playersettingsPaginationInfo_t	achievementsPaginationInfo;
	playersettingsRect_t	statsPrevPageButton;
	playersettingsRect_t	statsNextPageButton;
	playersettingsRect_t	achievementsPrevPageButton;
	playersettingsRect_t	achievementsNextPageButton;
	playersettingsRect_t	achievementsTierPrevPageButton;
	playersettingsRect_t	achievementsTierNextPageButton;
} playersettings_t;

static playersettings_t	s_playersettings;

static int gamecodetoui[] = {4,2,3,0,5,1,6};
static int uitogamecode[] = {4,6,2,3,1,5,7};

static const char *handicap_items[] = {
"None",
"95",
"90",
"85",
	"80",
	"75",
	"70",
	"65",
	"60",
	"55",
	"50",
	"45",
	"40",
	"35",
	"30",
	"25",
	"20",
	"15",
	"10",
"5",
0
};

/* Keep country selection consistent with the profile wizard.  The profile
 * stores the short ISO-style code, so the settings screen presents the same
 * compact value while still using the normal spin-control input path. */
static const char *country_codes[] = {
"",
"AT", "AU", "BE", "BR", "CA", "CH", "CN", "CZ", "DE",
"DK", "ES", "FI", "FR", "GB", "GR", "HU", "ID", "IN",
"IT", "JP", "KR", "MX", "NL", "NO", "NZ", "PL", "PT",
"RO", "RU", "SE", "SG", "SK", "TH", "TR", "TW", "UA",
"US", "VN", "ZA", "AR", "CL",
0
};

static const char *country_names[] = {
"Not specified",
"Austria", "Australia", "Belgium", "Brazil", "Canada", "Switzerland",
"China", "Czech Republic", "Germany", "Denmark", "Spain", "Finland",
"France", "United Kingdom", "Greece", "Hungary", "Indonesia", "India",
"Italy", "Japan", "South Korea", "Mexico", "Netherlands", "Norway",
"New Zealand", "Poland", "Portugal", "Romania", "Russia", "Sweden",
"Singapore", "Slovakia", "Thailand", "Turkey", "Taiwan", "Ukraine",
"United States", "Vietnam", "South Africa", "Argentina", "Chile",
0
};

static void PlayerSettings_InitBirthDateLists( void ) {
	int i;

	if ( s_birthDateListsInitialized ) {
		return;
	}

	s_birthDayItems[0] = "-";
	for ( i = 1; i <= BIRTH_DAY_MAX; ++i ) {
		Com_sprintf( s_birthDayStrings[i], sizeof( s_birthDayStrings[i] ), "%d", i );
		s_birthDayItems[i] = s_birthDayStrings[i];
	}
	s_birthDayItems[BIRTH_DAY_MAX + 1] = NULL;

	s_birthYearItems[0] = "-";
	for ( i = 0; i < BIRTH_YEAR_COUNT; ++i ) {
		Com_sprintf( s_birthYearStrings[i], sizeof( s_birthYearStrings[i] ), "%d", BIRTH_YEAR_START + i );
		s_birthYearItems[i + 1] = s_birthYearStrings[i];
	}
	s_birthYearItems[BIRTH_YEAR_COUNT + 1] = NULL;

	s_birthDateListsInitialized = qtrue;
}

static int PlayerSettings_GetBirthYearFromIndex( int index ) {
	if ( index <= 0 || index > BIRTH_YEAR_COUNT ) {
		return 0;
	}

	return BIRTH_YEAR_START + index - 1;
}

static int PlayerSettings_GetBirthYearIndex( int year ) {
	if ( year < BIRTH_YEAR_START || year > BIRTH_YEAR_END ) {
		return 0;
	}

	return ( year - BIRTH_YEAR_START ) + 1;
}

static int PlayerSettings_GetDaysInMonth( int year, int month ) {
	static const int daysPerMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int days;

	if ( month < 1 || month > 12 ) {
		return BIRTH_DAY_MAX;
	}

	days = daysPerMonth[month];

	if ( month == 2 ) {
		qboolean leap;

		leap = ( ( year % 4 ) == 0 && ( year % 100 ) != 0 ) || ( ( year % 400 ) == 0 );
		if ( leap ) {
			days = 29;
		}
	}

	return days;
}

static void PlayerSettings_UpdateBirthDateDayItems( void ) {
        int maxDay;
        int monthIndex;
        int yearIndex;

        maxDay = BIRTH_DAY_MAX;
        monthIndex = s_playersettings.birthMonth.curvalue;
        yearIndex = s_playersettings.birthYear.curvalue;

        if ( monthIndex > 0 && monthIndex <= 12 && yearIndex > 0 && yearIndex <= BIRTH_YEAR_COUNT ) {
                int year;

                year = PlayerSettings_GetBirthYearFromIndex( yearIndex );
                maxDay = PlayerSettings_GetDaysInMonth( year, monthIndex );
        }

        s_playersettings.birthDay.numitems = maxDay + 1;

        if ( s_playersettings.birthDay.curvalue >= s_playersettings.birthDay.numitems ) {
                s_playersettings.birthDay.curvalue = s_playersettings.birthDay.numitems - 1;
        }
}

static int PlayerSettings_FindGenderIndex( const char *value ) {
	int i;

	if ( !value || !value[0] ) {
		return 0;
	}

	for ( i = 1; s_genderItems[i]; ++i ) {
		if ( !Q_stricmp( value, s_genderItems[i] ) ) {
			return i;
		}
	}

	return 0;
}

static int PlayerSettings_FindCountryIndex( const char *value ) {
	int i;

	if ( !value || !value[0] ) {
		return 0;
	}

	for ( i = 1; country_codes[i]; ++i ) {
		if ( !Q_stricmp( value, country_codes[i] ) ) {
			return i;
		}
	}

	return 0;
}

static const char *PlayerSettings_GetGenderValue( int index ) {
	if ( index <= 0 || !s_genderItems[index] ) {
		return "";
	}

	return s_genderItems[index];
}

static qboolean PlayerSettings_ParseBirthDate( const char *value, int *outYear, int *outMonth, int *outDay ) {
	int year;
	int month;
	int day;
	int i;

	if ( !value ) {
		return qfalse;
	}

	if ( value[0] == '\0' ) {
		return qfalse;
	}

	for ( i = 0; i < 10; ++i ) {
		char c = value[i];

		if ( i == 4 || i == 7 ) {
			if ( c != '-' ) {
				return qfalse;
			}
			continue;
		}

		if ( c < '0' || c > '9' ) {
			return qfalse;
		}
	}

	if ( value[10] != '\0' ) {
		return qfalse;
	}

	year = ( value[0] - '0' ) * 1000 + ( value[1] - '0' ) * 100 + ( value[2] - '0' ) * 10 + ( value[3] - '0' );
	month = ( value[5] - '0' ) * 10 + ( value[6] - '0' );
	day = ( value[8] - '0' ) * 10 + ( value[9] - '0' );

	if ( year < BIRTH_YEAR_START || year > BIRTH_YEAR_END ) {
		return qfalse;
	}

	if ( month < 1 || month > 12 ) {
		return qfalse;
	}

	if ( day < 1 || day > PlayerSettings_GetDaysInMonth( year, month ) ) {
		return qfalse;
	}

	if ( outYear ) {
		*outYear = year;
	}

	if ( outMonth ) {
		*outMonth = month;
	}

	if ( outDay ) {
		*outDay = day;
	}

	return qtrue;
}

/*
=================
PlayerSettings_DrawName
=================
*/
static void PlayerSettings_DrawName( void *self ) {
	menufield_s		*f;
	qboolean		focus;
	int				style;
	char			*txt;
	char			c;
	float			*color;
	int				n;
	int				basex, x, y;
// STONELANCE
//	char			name[32];
// END

	f = (menufield_s*)self;
	basex = f->generic.x;
	y = f->generic.y;
	focus = (f->generic.parent->cursor == f->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
// STONELANCE
/*
	color = text_color_normal;
	if( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}
	UI_DrawProportionalString( basex, y, "Name", style, color );
*/
	color = uis.text_color;
	if( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawProportionalString( basex + PLAYERSETTINGS_PROFILE_LABEL_OFFSET, y, "Name", style, color );
// END

	// draw the actual name
	basex += PLAYERSETTINGS_PROFILE_VALUE_OFFSET;
// STONELANCE
//	y += PROP_HEIGHT;
	y += PLAYERSETTINGS_PROFILE_VALUE_BASELINE;
// END
	txt = f->field.buffer;
// STONELANCE
//	color = g_color_table[ColorIndex(COLOR_WHITE)];
// END
	x = basex;
	while ( (c = *txt) != 0 ) {
		if ( !focus && Q_IsColorString( txt ) ) {
			n = ColorIndex( *(txt+1) );
			if( n == 0 ) {
				n = 7;
			}
			color = g_color_table[n];
			txt += 2;
			continue;
		}
		UI_DrawChar( x, y, c, style, color );
		txt++;
		x += SMALLCHAR_WIDTH;
	}

	// draw cursor if we have focus
	if( focus ) {
		if ( trap_Key_GetOverstrikeMode() ) {
			c = 11;
		} else {
			c = 10;
		}

		style &= ~UI_PULSE;
		style |= UI_BLINK;

		UI_DrawChar( basex + f->field.cursor * SMALLCHAR_WIDTH, y, c, style, color_white );
	}

// STONELANCE
/*
	// draw at bottom also using proportional font
	Q_strncpyz( name, f->field.buffer, sizeof(name) );
	Q_CleanStr( name );
	UI_DrawProportionalString( 320, 440, name, UI_CENTER|UI_BIGFONT, text_color_normal );
*/
// END
}


static void PlayerSettings_DrawProfileField( void *self ) {
        menufield_s *f = (menufield_s *)self;
        qboolean disabled;
        qboolean focus;
        int style;
	char *txt;
	char c;
	float *color;
	int n;
	int basex, x, y;

	basex = f->generic.x;
	y = f->generic.y;
	disabled = ( qboolean )( f->generic.flags & ( QMF_GRAYED | QMF_INACTIVE ) );
	focus = !disabled && ( f->generic.parent->cursor == f->generic.menuPosition );

	style = UI_LEFT | UI_SMALLFONT;
	color = disabled ? text_color_disabled : uis.text_color;
	if ( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawProportionalString( basex + PLAYERSETTINGS_PROFILE_LABEL_OFFSET, y, f->generic.name ? f->generic.name : "", style, color );

	basex += PLAYERSETTINGS_PROFILE_VALUE_OFFSET;
	y += PLAYERSETTINGS_PROFILE_VALUE_BASELINE;
	txt = f->field.buffer;
	color = disabled ? text_color_disabled : g_color_table[ColorIndex( COLOR_WHITE )];
	x = basex;

	while ( ( c = *txt ) != 0 ) {
		if ( !disabled && !focus && Q_IsColorString( txt ) ) {
			n = ColorIndex( *( txt + 1 ) );
			if ( n == 0 ) {
				n = 7;
			}
			color = g_color_table[n];
			txt += 2;
			continue;
		}
		UI_DrawChar( x, y, c, style, color );
		txt++;
		x += SMALLCHAR_WIDTH;
	}

	if ( focus ) {
		if ( trap_Key_GetOverstrikeMode() ) {
			c = 11;
		} else {
			c = 10;
		}

		style &= ~UI_PULSE;
		style |= UI_BLINK;

		UI_DrawChar( basex + f->field.cursor * SMALLCHAR_WIDTH, y, c, style, color_white );
	}
}

static qboolean PlayerSettings_ItemHasFocus( menucommon_s *item ) {
	return ( item && Menu_ItemAtCursor( item->parent ) == item ) ? qtrue : qfalse;
}

static void PlayerSettings_DrawModernField( void *self ) {
	menufield_s *field;
	qboolean focus;
	vec4_t labelColor;
	vec4_t valueColor;
	const char *label;

	field = (menufield_s *)self;
	focus = PlayerSettings_ItemHasFocus( &field->generic );
	label = field->generic.name ? field->generic.name : "";
	Vector4Copy( focus ? playerSettingsAccentColor : playerSettingsMutedColor, labelColor );
	Vector4Copy( ( field->generic.flags & QMF_GRAYED ) ? playerSettingsMutedColor : playerSettingsTextColor, valueColor );

	Frontend_DrawText( field->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET,
	                   field->generic.y, label, UI_LEFT | UI_SMALLFONT,
	                   labelColor );
	Frontend_DrawText( field->generic.x + PLAYERSETTINGS_PROFILE_VALUE_OFFSET,
	                   field->generic.y + PLAYERSETTINGS_PROFILE_VALUE_BASELINE,
	                   field->field.buffer, UI_LEFT | UI_SMALLFONT,
	                   valueColor );

	if ( focus ) {
		UI_FillRect( field->generic.x + PLAYERSETTINGS_PROFILE_VALUE_OFFSET,
		             field->generic.bottom - 2,
		             field->generic.right - field->generic.x - PLAYERSETTINGS_PROFILE_VALUE_OFFSET,
		             2, playerSettingsAccentColor );
		UI_DrawChar( field->generic.x + PLAYERSETTINGS_PROFILE_VALUE_OFFSET +
		             field->field.cursor * SMALLCHAR_WIDTH,
		             field->generic.y + PLAYERSETTINGS_PROFILE_VALUE_BASELINE,
		             trap_Key_GetOverstrikeMode() ? 11 : 10,
		             UI_BLINK | UI_SMALLFONT, playerSettingsAccentColor );
	}
}

static void PlayerSettings_DrawModernChoice( void *self ) {
	menulist_s *choice;
	qboolean focus;
	vec4_t labelColor;
	vec4_t valueColor;
	const char *value;
	char buffer[96];

	choice = (menulist_s *)self;
	focus = PlayerSettings_ItemHasFocus( &choice->generic );
	value = "-";
	if ( choice->itemnames && choice->curvalue >= 0 && choice->curvalue < choice->numitems &&
	     choice->itemnames[choice->curvalue] ) {
		value = choice->itemnames[choice->curvalue];
	}
	Com_sprintf( buffer, sizeof( buffer ), "< %s >", value );
	Vector4Copy( focus ? playerSettingsAccentColor : playerSettingsMutedColor, labelColor );
	Vector4Copy( ( choice->generic.flags & QMF_GRAYED ) ? playerSettingsMutedColor : playerSettingsTextColor, valueColor );

	Frontend_DrawText( choice->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET,
	                   choice->generic.y, choice->generic.name ? choice->generic.name : "",
	                   UI_LEFT | UI_SMALLFONT, labelColor );
	Frontend_DrawText( choice->generic.right - PLAYERSETTINGS_PROFILE_LABEL_OFFSET,
	                   choice->generic.y + PLAYERSETTINGS_PROFILE_VALUE_BASELINE,
	                   buffer, UI_RIGHT | UI_SMALLFONT, valueColor );
	if ( focus ) {
		UI_FillRect( choice->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET,
		             choice->generic.bottom + 1,
		             choice->generic.right - choice->generic.x - PLAYERSETTINGS_PROFILE_LABEL_OFFSET * 2,
		             2, playerSettingsAccentColor );
	}
}

static void PlayerSettings_DrawModernBirthDate( void *self ) {
	menulist_s *choice;
	qboolean focus;
	vec4_t labelColor;
	vec4_t valueColor;
	const char *value;
	char buffer[48];

	choice = (menulist_s *)self;
	focus = PlayerSettings_ItemHasFocus( &choice->generic );
	value = "-";
	if ( choice->itemnames && choice->curvalue >= 0 && choice->curvalue < choice->numitems &&
	     choice->itemnames[choice->curvalue] ) {
		value = choice->itemnames[choice->curvalue];
	}
	Com_sprintf( buffer, sizeof( buffer ), "< %s >", value );
	Vector4Copy( focus ? playerSettingsAccentColor : playerSettingsMutedColor, labelColor );
	Vector4Copy( playerSettingsTextColor, valueColor );

	Frontend_DrawText( choice->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET,
	                   choice->generic.y - PLAYERSETTINGS_PROFILE_VALUE_BASELINE,
	                   choice->generic.name ? choice->generic.name : "",
	                   UI_LEFT | UI_SMALLFONT, labelColor );
	Frontend_DrawText( choice->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET,
	                   choice->generic.y, buffer, UI_LEFT | UI_SMALLFONT,
	                   valueColor );
}

static void PlayerSettings_DrawModernEffects( void *self ) {
	menulist_s *choice;
	qboolean focus;
	vec4_t labelColor;
	char buffer[32];

	choice = (menulist_s *)self;
	focus = PlayerSettings_ItemHasFocus( &choice->generic );
	Vector4Copy( focus ? playerSettingsAccentColor : playerSettingsMutedColor, labelColor );
	Com_sprintf( buffer, sizeof( buffer ), "%d / %d", choice->curvalue + 1, choice->numitems );

	Frontend_DrawText( choice->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET,
	                   choice->generic.y, "Effects", UI_LEFT | UI_SMALLFONT,
	                   labelColor );
	Frontend_DrawProgress( choice->generic.x + 80,
	                       choice->generic.y + 8, 72, 4,
	                       choice->numitems > 1 ? (float)choice->curvalue / (float)( choice->numitems - 1 ) : 0.0f,
	                       uis.tFrac );
	Frontend_DrawText( choice->generic.right - PLAYERSETTINGS_PROFILE_LABEL_OFFSET,
	                   choice->generic.y + PLAYERSETTINGS_PROFILE_VALUE_BASELINE,
	                   buffer, UI_RIGHT | UI_SMALLFONT, playerSettingsTextColor );
}


static void PlayerSettings_SetAvatarProfileName( const char *profileName ) {
	if ( !profileName ) {
		profileName = "";
	}

	if ( !Q_stricmp( profileName, s_playersettings.avatarProfileName ) ) {
		return;
	}

	Q_strncpyz( s_playersettings.avatarProfileName, profileName, sizeof( s_playersettings.avatarProfileName ) );

	if ( profileName[0] ) {
		/* Read the avatar path directly from the profile JSON.
		 * The wizard saves the full preset path (e.g. gfx/avatars/preset/driver_01)
		 * into info.avatar — use that as-is as the shader name.
		 * Only fall back to the legacy profile-name-based path if info.avatar
		 * is empty (pre-wizard profiles or manually cleared). */
		profile_info_t  info;
		profile_stats_t stats;
		Com_Memset( &info,  0, sizeof( info  ) );
		Com_Memset( &stats, 0, sizeof( stats ) );
		if ( UI_Profile_ReadData( profileName, &info, &stats ) && info.avatar[0] ) {
			Q_strncpyz( s_playersettings.profileInfo.avatar,
			            info.avatar,
			            sizeof( s_playersettings.profileInfo.avatar ) );
			/* Display path shown in the UI tooltip — derive .tga from shader name */
			Com_sprintf( s_playersettings.avatarDisplayPath,
			             sizeof( s_playersettings.avatarDisplayPath ),
			             "baseq3r/%s.tga", info.avatar );
		} else {
			s_playersettings.profileInfo.avatar[0]  = '\0';
			s_playersettings.avatarDisplayPath[0]   = '\0';
		}
	} else {
		s_playersettings.profileInfo.avatar[0]  = '\0';
		s_playersettings.avatarDisplayPath[0]   = '\0';
	}

	s_playersettings.avatarShader             = 0;
	s_playersettings.avatarShaderInitialized  = qfalse;
	s_playersettings.avatarShaderName[0]      = '\0';
}


static void PlayerSettings_EnsureAvatarShader( void ) {
	const char *shaderName;

	PlayerSettings_SetAvatarProfileName( UI_Profile_GetActiveName() );

	shaderName = s_playersettings.profileInfo.avatar;

	if ( !shaderName[0] ) {
		if ( s_playersettings.avatarShaderInitialized || s_playersettings.avatarShaderName[0] || s_playersettings.avatarShader ) {
			s_playersettings.avatarShader = 0;
			s_playersettings.avatarShaderInitialized = qfalse;
			s_playersettings.avatarShaderName[0] = '\0';
		}
		return;
	}

	if ( s_playersettings.avatarShaderInitialized && !Q_stricmp( shaderName, s_playersettings.avatarShaderName ) ) {
		return;
	}

	Q_strncpyz( s_playersettings.avatarShaderName, shaderName, sizeof( s_playersettings.avatarShaderName ) );
	s_playersettings.avatarShader = trap_R_RegisterShaderNoMip( shaderName );
	s_playersettings.avatarShaderInitialized = qtrue;
}


static void PlayerSettings_DrawClippedSmallString( int x, int y, int maxX, const char *text, int style, float *color ) {
	int drawX;
	char c;

	if ( maxX <= x ) {
		return;
	}

	drawX = x;
	while ( ( c = *text ) != 0 ) {
		if ( drawX + SMALLCHAR_WIDTH > maxX ) {
			break;
		}
		UI_DrawChar( drawX, y, c, style, color );
		++text;
		drawX += SMALLCHAR_WIDTH;
	}
}

static void PlayerSettings_DrawAchievementText( int x, int y, int maxX,
	const char *text, const float *color ) {
	char clipped[128];
	int length;

	if ( !text || !text[0] || maxX <= x ) {
		return;
	}

	Q_strncpyz( clipped, text, sizeof( clipped ) );
	length = 0;
	while ( clipped[length] ) {
		++length;
	}
	while ( length > 0 && x + Frontend_TextWidth( clipped, UI_SMALLFONT ) > maxX ) {
		clipped[--length] = '\0';
	}

	if ( clipped[0] ) {
		Frontend_DrawText( x, y, clipped, UI_LEFT | UI_SMALLFONT, color );
	}
}

static void PlayerSettings_DrawAchievementDescription( int x, int y, int maxX,
	const char *text, const float *color ) {
	const char *cursor;
	int line;

	if ( !text || !text[0] || maxX <= x ) {
		return;
	}

	cursor = text;
	for ( line = 0; line < 2 && cursor[0]; ++line ) {
		char buffer[128];
		int length;
		int consumed;
		int lastSpace;

		length = 0;
		consumed = 0;
		lastSpace = -1;
		while ( cursor[consumed] && length < (int)sizeof( buffer ) - 1 ) {
			buffer[length] = cursor[consumed];
			buffer[length + 1] = '\0';
			if ( x + Frontend_TextWidth( buffer, UI_SMALLFONT ) > maxX ) {
				break;
			}
			if ( cursor[consumed] == ' ' ) {
				lastSpace = length;
			}
			++length;
			++consumed;
		}

		if ( length <= 0 ) {
			buffer[0] = cursor[0];
			buffer[1] = '\0';
			length = 1;
			consumed = 1;
		} else if ( cursor[consumed] && lastSpace >= 0 ) {
			buffer[lastSpace] = '\0';
			consumed = lastSpace + 1;
		}

		Frontend_DrawText( x, y + line * 12, buffer,
		                   UI_LEFT | UI_SMALLFONT, color );
		while ( cursor[consumed] == ' ' ) {
			++consumed;
		}
		cursor += consumed;
	}
}


static void PlayerSettings_DrawAvatarImage( void *self ) {
	menufield_s *f = (menufield_s *)self;
	qboolean inactive;
	qboolean disabled;
	qboolean focus;
	int style;
	float *color;
	int basex;
	int y;
	int rowHeight;
	int imageSize;
	int rightEdge;
	int imageX;
	int imageY;
	int textRight;
	int textLineY;
	int secondaryStyle;
	float *secondaryColor;
	vec4_t background;
	vec4_t border;
	const char *line1;
	const char *line2;
	const char *actionLine;
	char combinedLine[MAX_OSPATH + 64];

	inactive = ( qboolean )( f->generic.flags & QMF_INACTIVE );
	disabled = ( qboolean )( f->generic.flags & QMF_GRAYED );
	focus = !inactive && ( f->generic.parent->cursor == f->generic.menuPosition );

	style = UI_LEFT | UI_SMALLFONT;
	color = disabled ? text_color_disabled : uis.text_color;
	if ( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	Frontend_DrawText( f->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET,
	                   f->generic.y, f->generic.name ? f->generic.name : "",
	                   UI_LEFT | UI_SMALLFONT,
	                   focus ? playerSettingsAccentColor : playerSettingsMutedColor );

	PlayerSettings_EnsureAvatarShader();

	basex = f->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET;
	y = f->generic.y;
	rowHeight = f->generic.bottom - f->generic.top;
	rightEdge = f->generic.right - 8;
	if ( rightEdge < basex ) {
		rightEdge = basex;
	}
	imageSize = rowHeight - 8;
	if ( imageSize < 0 ) {
		imageSize = 0;
	}
	if ( imageSize > 96 ) {
		imageSize = 96;
	}
	if ( imageSize > rightEdge - basex ) {
		imageSize = rightEdge - basex;
	}
	if ( imageSize < 0 ) {
		imageSize = 0;
	}

	if ( imageSize > 0 ) {
		imageX = rightEdge - imageSize;
		if ( imageX < basex ) {
			imageX = basex;
		}
		textRight = imageX - 8;
	} else {
		imageX = rightEdge;
		textRight = rightEdge;
	}
	if ( textRight < basex ) {
		textRight = basex;
	}
	imageY = f->generic.top + ( rowHeight - imageSize ) / 2;

	Vector4Copy( avatarImageBackgroundColor, background );
	if ( disabled ) {
		background[3] *= 0.6f;
	}
	background[3] *= uis.tFrac;

	Vector4Copy( profileRowBorderColor, border );
	border[3] *= uis.tFrac;

	if ( imageSize > 0 ) {
		UI_FillRect( imageX, imageY, imageSize, imageSize, background );

		if ( s_playersettings.avatarShader ) {
			trap_R_SetColor( NULL );
			UI_DrawHandlePic( imageX, imageY, imageSize, imageSize, s_playersettings.avatarShader );
		} else if ( s_playersettings.profileInfo.avatar[0] ) {
			Frontend_DrawText( imageX + imageSize / 2, imageY + imageSize / 2 - 6,
			                   "Missing", UI_CENTER | UI_SMALLFONT,
			                   avatarImageMissingColor );
		} else {
			Frontend_DrawText( imageX + imageSize / 2, imageY + imageSize / 2 - 6,
			                   "No avatar", UI_CENTER | UI_SMALLFONT,
			                   playerSettingsMutedColor );
		}

		UI_DrawRect( imageX, imageY, imageSize, imageSize, border );
	}

	style = UI_LEFT | UI_SMALLFONT;
	color = disabled ? text_color_disabled : g_color_table[ColorIndex( COLOR_WHITE )];
	if ( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	textLineY = y + PLAYERSETTINGS_PROFILE_VALUE_BASELINE;
	if ( s_playersettings.profileInfo.avatar[0] &&
	     strstr( s_playersettings.profileInfo.avatar, "gfx/avatars/custom/" ) ) {
		line1 = "Custom image";
		line2 = "";
	} else if ( s_playersettings.avatarProfileName[0] ) {
		Com_sprintf( combinedLine, sizeof( combinedLine ), "Preset: %s", s_playersettings.avatarProfileName );
		line1 = combinedLine;
		line2 = "";
	} else {
		Com_sprintf( combinedLine, sizeof( combinedLine ), "No active profile. Create or select a profile to display an avatar." );
		line1 = combinedLine;
		line2 = "";
	}

	Frontend_DrawText( basex, textLineY, line1, UI_LEFT | UI_SMALLFONT,
	                   focus ? playerSettingsAccentColor : playerSettingsTextColor );
	actionLine = s_playersettings.avatarActionLine;
	if ( !disabled && actionLine[0] ) {
		Frontend_DrawText( basex, textLineY + SMALLCHAR_HEIGHT + 4, actionLine,
		                   UI_LEFT | UI_SMALLFONT,
		                   focus ? playerSettingsAccentColor : playerSettingsMutedColor );
	}

	secondaryStyle = UI_LEFT | UI_SMALLFONT;
	secondaryColor = disabled ? text_color_disabled : text_color_normal;
	if ( line2[0] ) {
		Frontend_DrawText( basex, textLineY + SMALLCHAR_HEIGHT + 2, line2,
		                   UI_LEFT | UI_SMALLFONT, playerSettingsMutedColor );
	}
}


static void PlayerSettings_DrawHandicap( void *self ) {
	menulist_s		*item;
	qboolean		focus;
	int				style;
	float			*color;

	item = (menulist_s *)self;
	focus = (item->generic.parent->cursor == item->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
// STONELANCE
/*
	color = text_color_normal;
	if( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawProportionalString( item->generic.x, item->generic.y, "Handicap", style, color );
	UI_DrawProportionalString( item->generic.x + 64, item->generic.y + PROP_HEIGHT, handicap_items[item->curvalue], style, color );
*/
	color = uis.text_color;
	if( focus && !(uis.transitionIn || uis.transitionOut)) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawProportionalString( item->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET, item->generic.y, "Handicap", style, color );
	UI_DrawString( item->generic.x + PLAYERSETTINGS_PROFILE_VALUE_OFFSET, item->generic.y + PLAYERSETTINGS_PROFILE_VALUE_BASELINE, handicap_items[item->curvalue], style, color );
// END
}


/*
=================
PlayerSettings_DrawEffects
=================
*/
static void PlayerSettings_DrawEffects( void *self ) {
	menulist_s		*item;
	qboolean		focus;
	int				style;
	float			*color;

	item = (menulist_s *)self;
	focus = (item->generic.parent->cursor == item->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
// STONELANCE
/*
	color = text_color_normal;
	if( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawProportionalString( item->generic.x, item->generic.y, "Effects", style, color );

	UI_DrawHandlePic( item->generic.x + 64, item->generic.y + PROP_HEIGHT + 8, 128, 8, s_playersettings.fxBasePic );
	UI_DrawHandlePic( item->generic.x + 64 + item->curvalue * 16 + 8, item->generic.y + PROP_HEIGHT + 6, 16, 12, s_playersettings.fxPic[item->curvalue] );
*/

	color = uis.text_color;
	if( focus && !(uis.transitionIn || uis.transitionOut)) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawProportionalString( item->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET, item->generic.y, "Effects", style, color );

	{
		const int sliderX = item->generic.x + PLAYERSETTINGS_PROFILE_VALUE_OFFSET;
		const int sliderY = item->generic.y + PLAYERSETTINGS_PROFILE_VALUE_BASELINE;

		UI_DrawHandlePic( sliderX, sliderY, 128, 16, s_playersettings.fxBasePic );
		UI_DrawHandlePic( sliderX + 5 + item->curvalue * 17, sliderY, 16, 16, s_playersettings.fxPic[item->curvalue] );
	}
// END
}


// STONELANCE
/*
=================
PlayerSettings_DrawCustomize
=================
*/
static void PlayerSettings_DrawCustomize( void *self ) {
	menulist_s		*item;
	qboolean		focus;

	item = (menulist_s *)self;
	focus = ( Menu_ItemAtCursor( item->generic.parent ) == &item->generic );
	Frontend_DrawButtonFocused( item->generic.left, item->generic.top,
	                     item->generic.right - item->generic.left,
	                     item->generic.bottom - item->generic.top,
	                     "Customize vehicle", uis.tFrac, focus, UI_CENTER );
}

static void PlayerSettings_DrawPlateItem( void *self ) {
	menutext_s *item;
	qboolean focus;

	item = (menutext_s *)self;
	focus = ( Menu_ItemAtCursor( item->generic.parent ) == &item->generic );
	Frontend_DrawButtonFocused( item->generic.left, item->generic.top,
	                     item->generic.right - item->generic.left,
	                     item->generic.bottom - item->generic.top,
	                     "Change plate", uis.tFrac, focus, UI_CENTER );
}

static void PlayerSettings_DrawBackItem( void *self ) {
	menutext_s *item;
	qboolean focus;

	item = (menutext_s *)self;
	focus = ( Menu_ItemAtCursor( item->generic.parent ) == &item->generic );
	Frontend_DrawButtonFocused( item->generic.left, item->generic.top,
	                     item->generic.right - item->generic.left,
	                     item->generic.bottom - item->generic.top,
	                     "Back", uis.tFrac, focus, UI_CENTER );
}

static void PlayerSettings_DrawVehicleControl( void *self ) {
	menubitmap_s *item;
	qboolean focus;
	const char *label;

	item = (menubitmap_s *)self;
	focus = ( Menu_ItemAtCursor( item->generic.parent ) == &item->generic );
	label = item->generic.id == ID_LEFT ? "<  Previous" : "Next  >";
	Frontend_DrawButtonFocused( item->generic.left, item->generic.top,
	                     item->generic.right - item->generic.left,
	                     item->generic.bottom - item->generic.top,
	                     label, uis.tFrac, focus, UI_CENTER );
}

static void PlayerSettings_DrawFavoriteButton( void *self ) {
	menubitmap_s *item;
	qboolean focus;
	qboolean disabled;
	int slot;
	char label[32];

	item = (menubitmap_s *)self;
	focus = ( Menu_ItemAtCursor( item->generic.parent ) == &item->generic );
	disabled = ( qboolean )( item->generic.flags & ( QMF_GRAYED | QMF_INACTIVE ) );
	slot = item->generic.id - ID_FAVORITE1 + 1;
	Com_sprintf( label, sizeof( label ), "Favorite %d", slot );

	if ( disabled ) {
		Frontend_DrawText( ( item->generic.left + item->generic.right ) / 2,
		                   item->generic.top + 8, "Empty",
		                   UI_CENTER | UI_SMALLFONT, playerSettingsMutedColor );
		return;
	}

	Frontend_DrawButtonFocused( item->generic.left, item->generic.top,
	                     item->generic.right - item->generic.left,
	                     item->generic.bottom - item->generic.top,
	                     label, uis.tFrac, focus, UI_CENTER );
}

static void PlayerSettings_DrawVehiclePanelBackground( void ) {
	const char *modelName;

	modelName = s_playersettings.modelname.string;
	if ( !modelName || !modelName[0] ) {
		modelName = "Unknown vehicle";
	}

	Frontend_DrawCard( PLAYERSETTINGS_PROFILE_PANEL_LEFT,
	                   PLAYERSETTINGS_PROFILE_PANEL_TOP,
	                   PLAYERSETTINGS_PROFILE_PANEL_WIDTH, 294,
	                   uis.tFrac, qfalse );
	Frontend_DrawCard( 64, 150, 330, 168, uis.tFrac, qfalse );
	Frontend_DrawCard( 410, 150, 166, 194, uis.tFrac, qfalse );

	Frontend_DrawText( 80, 168, "Showroom", UI_LEFT | UI_SMALLFONT,
	                   playerSettingsMutedColor );
	Frontend_DrawText( 229, 168, modelName, UI_CENTER | UI_SMALLFONT,
	                   playerSettingsTextColor );

	Frontend_DrawText( 426, 168, "Vehicle details", UI_LEFT | UI_SMALLFONT,
	                   playerSettingsMutedColor );
	Frontend_DrawText( 426, 198, "Model", UI_LEFT | UI_SMALLFONT,
	                   playerSettingsMutedColor );
	Frontend_DrawText( 560, 198, modelName, UI_RIGHT | UI_SMALLFONT,
	                   playerSettingsTextColor );
	Frontend_DrawText( 426, 222, "Skin", UI_LEFT | UI_SMALLFONT,
	                   playerSettingsMutedColor );
	Frontend_DrawText( 560, 222, s_playersettings.modelskin,
	                   UI_RIGHT | UI_SMALLFONT, playerSettingsTextColor );
	Frontend_DrawText( 426, 246, "Rim", UI_LEFT | UI_SMALLFONT,
	                   playerSettingsMutedColor );
	Frontend_DrawText( 560, 246, s_playersettings.rimskin,
	                   UI_RIGHT | UI_SMALLFONT, playerSettingsTextColor );

	Frontend_DrawText( 64, 340, "Saved setups", UI_LEFT | UI_SMALLFONT,
	                   playerSettingsMutedColor );
}

static void PlayerSettings_SetWidgetVisible( menucommon_s *item, qboolean visible ) {
        if ( !item ) {
                return;
	}

        if ( visible ) {
                item->flags &= ~QMF_HIDDEN;
	} else {
                item->flags |= QMF_HIDDEN;
	}
}


static void PlayerSettings_DrawProfileList( void *self ) {
        menulist_s *item = (menulist_s *)self;
        qboolean disabled;
        qboolean focus;
        int style;
        float *color;
        const char *value;
        char buffer[64];

        disabled = (qboolean)( item->generic.flags & ( QMF_GRAYED | QMF_INACTIVE ) );
        focus = !disabled && ( item->generic.parent->cursor == item->generic.menuPosition );

        style = UI_LEFT | UI_SMALLFONT;
        color = disabled ? text_color_disabled : uis.text_color;
        if ( focus ) {
                style |= UI_PULSE;
                color = text_color_highlight;
	}

        if ( item->generic.name && item->generic.name[0] ) {
	UI_DrawProportionalString( item->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET, item->generic.y, item->generic.name, style, color );
	}

        value = "";
        if ( item->itemnames && item->curvalue >= 0 && item->curvalue < item->numitems ) {
                value = item->itemnames[item->curvalue];
	}

	Com_sprintf( buffer, sizeof( buffer ), "< %s >", value );
	UI_DrawString( item->generic.x + PLAYERSETTINGS_PROFILE_VALUE_OFFSET, item->generic.y + PLAYERSETTINGS_PROFILE_VALUE_BASELINE, buffer, style, color );
}


static void PlayerSettings_DrawBirthDateComponent( void *self ) {
        menulist_s *item = (menulist_s *)self;
        qboolean disabled;
        qboolean focus;
        int style;
        float *color;
        const char *value;
        char buffer[32];

        disabled = (qboolean)( item->generic.flags & ( QMF_GRAYED | QMF_INACTIVE ) );
        focus = !disabled && ( item->generic.parent->cursor == item->generic.menuPosition );

        style = UI_LEFT | UI_SMALLFONT;
        color = disabled ? text_color_disabled : uis.text_color;
        if ( focus ) {
                style |= UI_PULSE;
                color = text_color_highlight;
	}

	if ( item->generic.name && item->generic.name[0] ) {
	UI_DrawString( item->generic.x + PLAYERSETTINGS_PROFILE_LABEL_OFFSET, item->generic.y - PLAYERSETTINGS_PROFILE_VALUE_BASELINE, item->generic.name, style, color );
}

        value = "";
        if ( item->itemnames && item->curvalue >= 0 && item->curvalue < item->numitems ) {
                value = item->itemnames[item->curvalue];
	}

	Com_sprintf( buffer, sizeof( buffer ), "< %s >", value );
	UI_DrawString( item->generic.x + PLAYERSETTINGS_PROFILE_VALUE_OFFSET, item->generic.y, buffer, style, color );
}

static void PlayerSettings_BirthDateChanged( void *ptr, int event ) {
        menucommon_s *item;

        if ( event != QM_ACTIVATED ) {
                return;
	}

        item = (menucommon_s *)ptr;
        if ( item->id == ID_BIRTH_MONTH || item->id == ID_BIRTH_YEAR ) {
                PlayerSettings_UpdateBirthDateDayItems();
	}
}

static int PlayerSettings_GetTabCenter( int index ) {
        int totalWidth;
        int start;

        totalWidth = PLAYERSETTINGS_TAB_COUNT * PLAYERSETTINGS_TAB_WIDTH + (PLAYERSETTINGS_TAB_COUNT - 1) * PLAYERSETTINGS_TAB_GAP;
        start = 320 - totalWidth / 2 + PLAYERSETTINGS_TAB_WIDTH / 2;

        return start + index * (PLAYERSETTINGS_TAB_WIDTH + PLAYERSETTINGS_TAB_GAP);
}

static void PlayerSettings_ConfigureTab( menutext_s *tab, int index ) {
        int center;

        if ( !tab ) {
                return;
	}

        center = PlayerSettings_GetTabCenter( index );

        tab->generic.x = center;
        tab->generic.y = PLAYERSETTINGS_TAB_TOP + PLAYERSETTINGS_TAB_TEXT_OFFSET;
        tab->generic.left = center - PLAYERSETTINGS_TAB_WIDTH / 2;
        tab->generic.right = center + PLAYERSETTINGS_TAB_WIDTH / 2;
        tab->generic.top = PLAYERSETTINGS_TAB_TOP;
        tab->generic.bottom = PLAYERSETTINGS_TAB_TOP + PLAYERSETTINGS_TAB_HEIGHT;
}

static int PlayerSettings_TabFromId( int id ) {
        switch ( id ) {
        case ID_TAB_PROFILE:
                return TAB_PROFILE;
        case ID_TAB_STATS:
                return TAB_STATS;
        case ID_TAB_ACHIEVEMENTS:
                return TAB_ACHIEVEMENTS;
        case ID_TAB_VEHICLE:
                return TAB_VEHICLE;
        default:
                break;
	}

        return TAB_PROFILE;
}

static void PlayerSettings_DrawTabItem( void *self ) {
        menutext_s *tab;
        qboolean focus;
        qboolean selected;

        tab = (menutext_s *)self;
        focus = ( Menu_ItemAtCursor( tab->generic.parent ) == &tab->generic );
        selected = ( s_playersettings.currentTab == PlayerSettings_TabFromId( tab->generic.id ) );

        Frontend_DrawNavButton( tab->generic.left, tab->generic.top,
                                PLAYERSETTINGS_TAB_WIDTH,
                                PLAYERSETTINGS_TAB_HEIGHT,
                                tab->string, uis.tFrac,
                                selected || focus, UI_CENTER );
}


static void PlayerSettings_GetProfileRowBounds( int row, int *top, int *bottom ) {
	int rowTop;
	int rowBottom;

	rowTop = PLAYERSETTINGS_PROFILE_PANEL_TOP + PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN + 2;
	rowBottom = rowTop + PLAYERSETTINGS_PROFILE_ROW_HEIGHT;

	switch ( row ) {
	case PROFILE_ROW_NAME:
		rowTop = s_playersettings.name.generic.top;
		rowBottom = s_playersettings.name.generic.bottom;
		break;
	case PROFILE_ROW_GENDER:
		rowTop = s_playersettings.gender.generic.top;
		rowBottom = s_playersettings.gender.generic.bottom;
		break;
	case PROFILE_ROW_BIRTHDATE:
		rowTop = s_playersettings.birthDateLabel.generic.top;
		rowBottom = s_playersettings.birthDay.generic.bottom;
		if ( s_playersettings.birthMonth.generic.bottom > rowBottom ) {
			rowBottom = s_playersettings.birthMonth.generic.bottom;
		}
		if ( s_playersettings.birthYear.generic.bottom > rowBottom ) {
			rowBottom = s_playersettings.birthYear.generic.bottom;
		}
		break;
	case PROFILE_ROW_AVATAR:
		rowTop = s_playersettings.avatar.generic.top;
		rowBottom = s_playersettings.avatar.generic.bottom;
		break;
	case PROFILE_ROW_COUNTRY:
		rowTop = s_playersettings.country.generic.top;
		rowBottom = s_playersettings.country.generic.bottom;
		break;
	case PROFILE_ROW_HANDICAP:
		rowTop = s_playersettings.handicap.generic.top;
		rowBottom = s_playersettings.handicap.generic.bottom;
		break;
	case PROFILE_ROW_EFFECTS:
		rowTop = s_playersettings.effects.generic.top;
		rowBottom = s_playersettings.effects.generic.bottom;
		break;
	default:
		break;
	}

	if ( rowBottom <= rowTop ) {
		rowBottom = rowTop + PLAYERSETTINGS_PROFILE_ROW_HEIGHT;
	}

	if ( top ) {
		*top = rowTop;
	}
	if ( bottom ) {
		*bottom = rowBottom;
	}
}


static void PlayerSettings_DrawProfilePanelBackground( void ) {
	Frontend_DrawCard( PLAYERSETTINGS_PROFILE_FORM_X,
	                   PLAYERSETTINGS_PROFILE_FORM_Y,
	                   PLAYERSETTINGS_PROFILE_FORM_WIDTH,
	                   PLAYERSETTINGS_PROFILE_FORM_HEIGHT,
	                   uis.tFrac, qfalse );
	Frontend_DrawCard( PLAYERSETTINGS_PROFILE_PRESENCE_X,
	                   PLAYERSETTINGS_PROFILE_PRESENCE_Y,
	                   PLAYERSETTINGS_PROFILE_PRESENCE_WIDTH,
	                   PLAYERSETTINGS_PROFILE_PRESENCE_HEIGHT,
	                   uis.tFrac, qfalse );
	Frontend_DrawText( PLAYERSETTINGS_PROFILE_FORM_X + 16,
	                   PLAYERSETTINGS_PROFILE_FORM_Y + 18,
	                   "Identity", UI_LEFT | UI_SMALLFONT,
	                   playerSettingsMutedColor );
	Frontend_DrawText( PLAYERSETTINGS_PROFILE_PRESENCE_X + 16,
	                   PLAYERSETTINGS_PROFILE_PRESENCE_Y + 18,
	                   "Presence", UI_LEFT | UI_SMALLFONT,
	                   playerSettingsMutedColor );
	Frontend_DrawStatusChip( PLAYERSETTINGS_PROFILE_PRESENCE_X +
	                         PLAYERSETTINGS_PROFILE_PRESENCE_WIDTH - 92,
	                         PLAYERSETTINGS_PROFILE_PRESENCE_Y + 16,
	                         UI_Profile_HasActiveProfile() ? "Active" : "Empty",
	                         UI_Profile_HasActiveProfile() ? playerSettingsStatusColor : playerSettingsMutedColor,
	                         uis.tFrac );
}



static float PlayerSettings_GetScrollContentTop( void ) {
	if ( s_playersettings.currentTab == TAB_STATS ) {
		return PLAYERSETTINGS_PROFILE_PANEL_TOP + PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN + 22;
	}
	if ( s_playersettings.currentTab == TAB_ACHIEVEMENTS ) {
		return PLAYERSETTINGS_PROFILE_PANEL_TOP + PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN + 48;
	}

	return PLAYERSETTINGS_PROFILE_PANEL_TOP + PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN + 2;
}

static float PlayerSettings_GetScrollViewportTop( void ) {
	if ( s_playersettings.currentTab == TAB_STATS ) {
		return PLAYERSETTINGS_PROFILE_PANEL_TOP + PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN + 20;
	}
	if ( s_playersettings.currentTab == TAB_ACHIEVEMENTS ) {
		return PLAYERSETTINGS_PROFILE_PANEL_TOP + PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN + 46;
	}

	return PLAYERSETTINGS_PROFILE_PANEL_TOP + PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN;
}

static float PlayerSettings_GetPanelBottomForContent( float contentHeight ) {
	float panelTop;
	float panelBottom;

panelTop = PLAYERSETTINGS_PROFILE_PANEL_TOP;
panelBottom = panelTop + PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN + 2 + contentHeight + PLAYERSETTINGS_PROFILE_PANEL_BOTTOM_EXTRA;
if ( panelBottom < panelTop ) {
panelBottom = panelTop;
}
if ( panelBottom > 440.0f ) {
panelBottom = 440.0f;
}

	return panelBottom;
}

static float PlayerSettings_GetScrollViewportBottom( float contentHeight ) {
	return PlayerSettings_GetPanelBottomForContent( contentHeight ) - PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN;
}

static float PlayerSettings_GetPaginatedViewportBottom( float contentHeight, float reservedHeight ) {
	float viewportTop;
	float viewportBottom;

	viewportTop = PlayerSettings_GetScrollViewportTop();
	viewportBottom = PlayerSettings_GetScrollViewportBottom( contentHeight );
	if ( reservedHeight > 0.0f ) {
		viewportBottom -= reservedHeight;
	}

	if ( viewportBottom < viewportTop ) {
		viewportBottom = viewportTop;
	}

	return viewportBottom;
}

static void PlayerSettings_GetPaginatedViewportBounds( float contentHeight, float reservedHeight, float *top, float *bottom ) {
	float viewportTop;
	float viewportBottom;

	viewportTop = PlayerSettings_GetScrollViewportTop();
	viewportBottom = PlayerSettings_GetPaginatedViewportBottom( contentHeight, reservedHeight );

	if ( top ) {
		*top = viewportTop;
	}
	if ( bottom ) {
		*bottom = viewportBottom;
	}
}

static float PlayerSettings_GetStatsRowSpacing( void ) {
	return PLAYERSETTINGS_STATS_ROW_HEIGHT + PLAYERSETTINGS_STATS_ROW_GAP;
}

static float PlayerSettings_GetAchievementsRowSpacing( void ) {
	return PLAYERSETTINGS_ACHIEVEMENT_ROW_HEIGHT + PLAYERSETTINGS_ACHIEVEMENT_ROW_GAP;
}

static const playersettingsStatsCategory_t *PlayerSettings_GetStatsCategory( void ) {
	int page;

	page = s_playersettings.statsPagination.currentPage;
	if ( page < 0 ) {
		page = 0;
	}
	if ( page >= ARRAY_LEN( s_statsCategories ) ) {
		page = ARRAY_LEN( s_statsCategories ) - 1;
	}

	return &s_statsCategories[page];
}

static float PlayerSettings_GetStatsPageContentHeight( void ) {
	/* The card and its footer stay fixed while the category changes. */
	return PLAYERSETTINGS_STATS_CARD_CONTENT_HEIGHT;
}

static float PlayerSettings_GetAchievementsContentHeight( void ) {
	/* One achievement track is shown per page inside the shared fixed card. */
	return PLAYERSETTINGS_STATS_CARD_CONTENT_HEIGHT;
}

static int PlayerSettings_GetAchievementTierPagesForCount( int tierCount ) {
        int tiersPerPage;

        tiersPerPage = PLAYERSETTINGS_ACHIEVEMENTS_PER_PAGE;
        if ( tiersPerPage <= 0 ) {
                return 1;
        }

        if ( tierCount <= 0 ) {
                return 1;
        }

        return ( tierCount + tiersPerPage - 1 ) / tiersPerPage;
}

static int PlayerSettings_GetAchievementMaxTierCount( void ) {
        static const int s_tierCounts[] = {
                ARRAY_LEN( s_distanceAchievementTiers ),
                ARRAY_LEN( s_killAchievementTiers ),
                ARRAY_LEN( s_winAchievementTiers ),
                ARRAY_LEN( s_sprintWinAchievementTiers ),
                ARRAY_LEN( s_flagCaptureAchievementTiers ),
                ARRAY_LEN( s_flagAssistAchievementTiers ),
                ARRAY_LEN( s_fuelAchievementTiers ),
                ARRAY_LEN( s_accuracyAchievementTiers ),
                ARRAY_LEN( s_excellentAchievementTiers ),
                ARRAY_LEN( s_impressiveAchievementTiers ),
                ARRAY_LEN( s_perfectAchievementTiers )
        };
        int maxCount;
        int i;

        maxCount = 0;
        for ( i = 0; i < ARRAY_LEN( s_tierCounts ); ++i ) {
                if ( s_tierCounts[i] > maxCount ) {
                        maxCount = s_tierCounts[i];
                }
        }

        if ( maxCount <= 0 ) {
                maxCount = PLAYERSETTINGS_MAX_ACHIEVEMENT_TIERS;
        }

        return maxCount;
}

static int PlayerSettings_GetAchievementTotalPageCount( void ) {
        int tierCount;

        tierCount = PlayerSettings_GetAchievementMaxTierCount();
        if ( tierCount <= 0 ) {
                return 1;
        }

        return PlayerSettings_GetAchievementTierPagesForCount( tierCount );
}


static void PlayerSettings_ClearRect( playersettingsRect_t *rect ) {
	if ( !rect ) {
		return;
	}

	rect->x = rect->y = rect->w = rect->h = 0.0f;
}

static qboolean PlayerSettings_RectContainsCursor( const playersettingsRect_t *rect ) {
	if ( !rect || rect->w <= 0.0f || rect->h <= 0.0f ) {
		return qfalse;
	}

	return UI_CursorInRect( (int)rect->x, (int)rect->y, (int)rect->w, (int)rect->h );
}

static const playersettingsPaginationInfo_t *PlayerSettings_UpdateStatsPaginationInfo( void ) {
	const playersettingsStatsCategory_t *category;
	int categoryCount;

	categoryCount = ARRAY_LEN( s_statsCategories );
	if ( categoryCount <= 0 ) {
		Com_Memset( &s_playersettings.statsPaginationInfo, 0,
		            sizeof( s_playersettings.statsPaginationInfo ) );
		return &s_playersettings.statsPaginationInfo;
	}

	if ( s_playersettings.statsPagination.currentPage < 0 ) {
		s_playersettings.statsPagination.currentPage = 0;
	}
	if ( s_playersettings.statsPagination.currentPage >= categoryCount ) {
		s_playersettings.statsPagination.currentPage = categoryCount - 1;
	}

	category = PlayerSettings_GetStatsCategory();
	s_playersettings.statsPaginationInfo.rowCount = STATS_ROW_COUNT;
	s_playersettings.statsPaginationInfo.rowsPerPage = category->lastRow - category->firstRow + 1;
	s_playersettings.statsPaginationInfo.totalPages = categoryCount;
	s_playersettings.statsPaginationInfo.firstRow = category->firstRow;
	s_playersettings.statsPaginationInfo.lastRow = category->lastRow;
	s_playersettings.statsPaginationInfo.rowOffset =
		category->firstRow * PlayerSettings_GetStatsRowSpacing();

	return &s_playersettings.statsPaginationInfo;
}

static const playersettingsPaginationInfo_t *PlayerSettings_UpdateAchievementsPaginationInfo( void ) {
	int sectionCount;

	sectionCount = PLAYERSETTINGS_ACHIEVEMENT_SECTION_COUNT;
	if ( sectionCount <= 0 ) {
		Com_Memset( &s_playersettings.achievementsPaginationInfo, 0,
		            sizeof( s_playersettings.achievementsPaginationInfo ) );
		return &s_playersettings.achievementsPaginationInfo;
	}

	if ( s_playersettings.achievementsPagination.currentPage < 0 ) {
		s_playersettings.achievementsPagination.currentPage = 0;
	}
	if ( s_playersettings.achievementsPagination.currentPage >= sectionCount ) {
		s_playersettings.achievementsPagination.currentPage = sectionCount - 1;
	}

	s_playersettings.achievementsPaginationInfo.rowCount = sectionCount;
	s_playersettings.achievementsPaginationInfo.rowsPerPage = 1;
	s_playersettings.achievementsPaginationInfo.totalPages = sectionCount;
	s_playersettings.achievementsPaginationInfo.firstRow = s_playersettings.achievementsPagination.currentPage;
	s_playersettings.achievementsPaginationInfo.lastRow = s_playersettings.achievementsPagination.currentPage;
	s_playersettings.achievementsPaginationInfo.rowOffset =
		s_playersettings.achievementsPagination.currentPage * PlayerSettings_GetAchievementsRowSpacing();

	return &s_playersettings.achievementsPaginationInfo;
}

static void PlayerSettings_ClampAchievementTierPage( void ) {
	int totalPages;

	totalPages = PlayerSettings_GetAchievementTotalPageCount();
	if ( totalPages < 1 ) {
		totalPages = 1;
	}

	if ( s_playersettings.achievementsTierPagination.currentPage >= totalPages ) {
		s_playersettings.achievementsTierPagination.currentPage = totalPages - 1;
	}
	if ( s_playersettings.achievementsTierPagination.currentPage < 0 ) {
		s_playersettings.achievementsTierPagination.currentPage = 0;
	}
}

static qboolean PlayerSettings_HandlePaginationCommand(
	playersettingsPaginationState_t *state,
	const playersettingsPaginationInfo_t *info,
	int delta ) {
	int newPage;

	if ( !state || !info || info->totalPages <= 1 || delta == 0 ) {
		return qfalse;
	}

	newPage = state->currentPage + delta;
	if ( newPage < 0 ) {
		newPage = 0;
	}
	if ( newPage >= info->totalPages ) {
		newPage = info->totalPages - 1;
	}

	if ( newPage == state->currentPage ) {
		return qfalse;
	}

	state->currentPage = newPage;
	return qtrue;
}

static qboolean PlayerSettings_HandlePaginationKey(
	playersettingsPaginationState_t *state,
	const playersettingsPaginationInfo_t *info,
	int key ) {
	int delta;

	delta = 0;
	switch ( key ) {
	case K_LEFTARROW:
	case K_UPARROW:
	case K_KP_LEFTARROW:
	case K_KP_UPARROW:
	case K_PGUP:
		delta = -1;
		break;
	case K_RIGHTARROW:
	case K_DOWNARROW:
	case K_KP_RIGHTARROW:
	case K_KP_DOWNARROW:
	case K_PGDN:
		delta = 1;
		break;
	default:
		return qfalse;
	}

	return PlayerSettings_HandlePaginationCommand( state, info, delta );
}

static qboolean PlayerSettings_HandlePaginationClick(
	playersettingsPaginationState_t *state,
	const playersettingsPaginationInfo_t *info,
	const playersettingsRect_t *prevRect,
	const playersettingsRect_t *nextRect ) {
	int delta;

	if ( !state || !info || info->totalPages <= 1 ) {
		return qfalse;
	}

	delta = 0;
	if ( state->currentPage > 0 && PlayerSettings_RectContainsCursor( prevRect ) ) {
		delta = -1;
	} else if ( state->currentPage < info->totalPages - 1 && PlayerSettings_RectContainsCursor( nextRect ) ) {
		delta = 1;
	}

	if ( !delta ) {
		return qfalse;
	}

	return PlayerSettings_HandlePaginationCommand( state, info, delta );
}

static void PlayerSettings_DrawPaginationButton( const char *label, const playersettingsRect_t *rect, qboolean enabled, qboolean hovered ) {
	if ( !rect || rect->w <= 0.0f || rect->h <= 0.0f ) {
		return;
	}

	if ( enabled ) {
		Frontend_DrawButton( (int)rect->x, (int)rect->y,
		                     (int)rect->w, (int)rect->h, label,
		                     uis.tFrac, hovered, UI_CENTER );
	} else {
		Frontend_DrawText( (int)( rect->x + rect->w * 0.5f ),
		                   (int)( rect->y + ( rect->h - SMALLCHAR_HEIGHT ) * 0.5f ),
		                   label, UI_CENTER | UI_SMALLFONT,
		                   playerSettingsMutedColor );
	}
}

static void PlayerSettings_DrawPaginationControls(
playersettingsPaginationState_t *state,
const playersettingsPaginationInfo_t *info,
float contentHeight,
float reservedHeight,
playersettingsRect_t *prevRect,
playersettingsRect_t *nextRect ) {
float viewportTop;
	float viewportBottom;
	float y;
	float centerX;
	float panelLeft;
	float panelRight;
	char pageBuffer[32];
	qboolean prevHover;
	qboolean nextHover;

	PlayerSettings_ClearRect( prevRect );
	PlayerSettings_ClearRect( nextRect );

	if ( !state || !info || info->totalPages <= 1 ) {
		return;
	}

	if ( reservedHeight < PLAYERSETTINGS_PAGINATION_BUTTON_HEIGHT + PLAYERSETTINGS_PAGINATION_BUTTON_MARGIN * 2.0f ) {
		reservedHeight = PLAYERSETTINGS_PAGINATION_BUTTON_HEIGHT + PLAYERSETTINGS_PAGINATION_BUTTON_MARGIN * 2.0f;
	}
	PlayerSettings_GetPaginatedViewportBounds(
		contentHeight,
		reservedHeight,
		&viewportTop,
		&viewportBottom );
	y = viewportBottom + reservedHeight - PLAYERSETTINGS_PAGINATION_BUTTON_HEIGHT - PLAYERSETTINGS_PAGINATION_BUTTON_MARGIN;
        centerX = PLAYERSETTINGS_PROFILE_PANEL_LEFT + PLAYERSETTINGS_PROFILE_PANEL_WIDTH * 0.5f;
	panelLeft = PLAYERSETTINGS_PROFILE_PANEL_LEFT + PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN;
	panelRight = PLAYERSETTINGS_PROFILE_PANEL_LEFT + PLAYERSETTINGS_PROFILE_PANEL_WIDTH - PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN;

	prevRect->x = centerX - PLAYERSETTINGS_PAGINATION_BUTTON_GAP * 0.5f - PLAYERSETTINGS_PAGINATION_BUTTON_WIDTH;
	prevRect->y = y;
	prevRect->w = PLAYERSETTINGS_PAGINATION_BUTTON_WIDTH;
	prevRect->h = PLAYERSETTINGS_PAGINATION_BUTTON_HEIGHT;

	nextRect->x = centerX + PLAYERSETTINGS_PAGINATION_BUTTON_GAP * 0.5f;
	nextRect->y = y;
	nextRect->w = PLAYERSETTINGS_PAGINATION_BUTTON_WIDTH;
	nextRect->h = PLAYERSETTINGS_PAGINATION_BUTTON_HEIGHT;

	if ( prevRect->x < panelLeft ) {
		float shift = panelLeft - prevRect->x;
		prevRect->x += shift;
		nextRect->x += shift;
	}
	if ( nextRect->x + nextRect->w > panelRight ) {
		float shift = ( nextRect->x + nextRect->w ) - panelRight;
		prevRect->x -= shift;
		nextRect->x -= shift;
	}

	prevHover = PlayerSettings_RectContainsCursor( prevRect );
	nextHover = PlayerSettings_RectContainsCursor( nextRect );

	PlayerSettings_DrawPaginationButton( "<< Prev", prevRect, ( state->currentPage > 0 ), prevHover );
	PlayerSettings_DrawPaginationButton( "Next >>", nextRect, ( state->currentPage < info->totalPages - 1 ), nextHover );

	Com_sprintf( pageBuffer, sizeof( pageBuffer ), "Page %d / %d", state->currentPage + 1, info->totalPages );
	Frontend_DrawText(
		(int)centerX,
		(int)( y + ( PLAYERSETTINGS_PAGINATION_BUTTON_HEIGHT - SMALLCHAR_HEIGHT ) * 0.5f ),
		pageBuffer, UI_CENTER | UI_SMALLFONT, playerSettingsAccentColor );
}

static void PlayerSettings_DrawStatsPaginationControls( void ) {
	const playersettingsPaginationInfo_t *info;

	info = PlayerSettings_UpdateStatsPaginationInfo();
	PlayerSettings_DrawPaginationControls(
		&s_playersettings.statsPagination,
		info,
		PlayerSettings_GetStatsPageContentHeight(),
		PLAYERSETTINGS_STATS_PAGINATION_RESERVED_HEIGHT,
		&s_playersettings.statsPrevPageButton,
		&s_playersettings.statsNextPageButton );
}

static void PlayerSettings_DrawAchievementsPaginationControls( void ) {
	const playersettingsPaginationInfo_t *info;

	info = PlayerSettings_UpdateAchievementsPaginationInfo();
	PlayerSettings_DrawPaginationControls(
		&s_playersettings.achievementsPagination,
		info,
		PlayerSettings_GetAchievementsContentHeight(),
		PLAYERSETTINGS_ACHIEVEMENTS_PAGINATION_RESERVED_HEIGHT,
		&s_playersettings.achievementsPrevPageButton,
		&s_playersettings.achievementsNextPageButton );
}

static void PlayerSettings_GetStatsRowBounds( int row, int *top, int *bottom ) {
	float rowTop;
	float rowBottom;
	float spacing;
	float contentTop;
	float offset;

	PlayerSettings_UpdateStatsPaginationInfo();

if ( row < 0 ) {
row = 0;
}
if ( row >= STATS_ROW_COUNT ) {
		row = STATS_ROW_COUNT - 1;
	}

	spacing = PlayerSettings_GetStatsRowSpacing();
	contentTop = PlayerSettings_GetScrollContentTop();
	offset = s_playersettings.statsPaginationInfo.rowOffset;
	rowTop = contentTop + row * spacing - offset;
	rowBottom = rowTop + PLAYERSETTINGS_STATS_ROW_HEIGHT;

	if ( top ) {
		*top = (int)rowTop;
	}
	if ( bottom ) {
		*bottom = (int)rowBottom;
	}
}



static void PlayerSettings_DrawStatsPanelBackground( void ) {
vec4_t borderColor;
vec4_t titleColor;
vec4_t titleAccentColor;
float contentHeight;
float viewportTop;
float viewportBottom;
int panelTop;
int panelBottom;
int i;
const playersettingsPaginationInfo_t *info;
const playersettingsStatsCategory_t *category;

contentHeight = PlayerSettings_GetStatsPageContentHeight();
info = PlayerSettings_UpdateStatsPaginationInfo();
category = PlayerSettings_GetStatsCategory();

panelTop = PLAYERSETTINGS_PROFILE_PANEL_TOP;
panelBottom = (int)PlayerSettings_GetPanelBottomForContent( contentHeight );

Frontend_DrawPanel( PLAYERSETTINGS_PROFILE_PANEL_LEFT, panelTop,
                    PLAYERSETTINGS_PROFILE_PANEL_WIDTH,
                    panelBottom - panelTop, uis.tFrac,
                    UI_FRONTEND_STYLE_SURFACE );

Vector4Copy( playerSettingsTextColor, titleColor );
titleColor[3] *= uis.tFrac;
Vector4Copy( playerSettingsAccentColor, titleAccentColor );
titleAccentColor[3] *= uis.tFrac;
UI_FillRect( PLAYERSETTINGS_PROFILE_FIELD_LEFT + 16, panelTop + 10,
             UI_FRONTEND_STATUS_DOT, UI_FRONTEND_STATUS_DOT, titleAccentColor );
Frontend_DrawText( PLAYERSETTINGS_PROFILE_FIELD_LEFT + 32,
                   panelTop + 4,
                   category->title, UI_LEFT | UI_BIGFONT,
                   titleColor );
UI_FillRect( PLAYERSETTINGS_PROFILE_FIELD_LEFT + 32, panelTop + 24,
             Frontend_TextWidth( category->title, UI_BIGFONT ), 2,
             titleAccentColor );

Vector4Copy( profileRowBorderColor, borderColor );
borderColor[3] *= uis.tFrac;

PlayerSettings_GetPaginatedViewportBounds(
        contentHeight,
        PLAYERSETTINGS_STATS_PAGINATION_RESERVED_HEIGHT,
        &viewportTop,
        &viewportBottom );

if ( !info || info->lastRow < info->firstRow ) {
return;
}

for ( i = info->firstRow; i <= info->lastRow; ++i ) {
int rowTop;
int rowBottom;

PlayerSettings_GetStatsRowBounds( i, &rowTop, &rowBottom );

if ( rowTop < (int)viewportTop ) {
rowTop = (int)viewportTop;
}
if ( rowBottom > (int)viewportBottom ) {
rowBottom = (int)viewportBottom;
}
if ( rowBottom <= rowTop ) {
continue;
}

if ( i & 1 ) {
        vec4_t rowColor;
        Vector4Copy( profileRowOddFillColor, rowColor );
        rowColor[3] *= uis.tFrac * 0.55f;
        UI_FillRect( PLAYERSETTINGS_PROFILE_FIELD_LEFT, rowTop,
                     PLAYERSETTINGS_PROFILE_PANEL_WIDTH - PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN * 2,
                     rowBottom - rowTop, rowColor );
}

UI_FillRect( PLAYERSETTINGS_PROFILE_FIELD_LEFT, rowBottom - 1,
             PLAYERSETTINGS_PROFILE_PANEL_WIDTH - PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN * 2,
             1, borderColor );
}
}

static void PlayerSettings_DrawStatsTab( void ) {
        const profile_stats_t *stats;
        profile_rank_t rank;
        qboolean hasRank;
        char buffer[96];
        char vehicleName[64];
        char vehicleTime[16];
        char *slash;

PlayerSettings_UpdateStatsPaginationInfo();

if ( !UI_Profile_HasActiveProfile() ) {
PlayerSettings_DrawStatsMessage( STATS_ROW_PLAYER_SCORE, "No active profile selected." );
PlayerSettings_DrawStatsMessage( STATS_ROW_CURRENT_RANK, "Create or select a profile from the main menu." );
return;
}

stats = UI_Profile_GetActiveStats();
if ( !stats ) {
PlayerSettings_DrawStatsMessage( STATS_ROW_PLAYER_SCORE, "Unable to read profile statistics." );
return;
}

/* ── General ─────────────────────────────────────────────────────── */
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->playerScore );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_PLAYER_SCORE, "Player Score", buffer );

hasRank = UI_Profile_GetRank( stats, &rank );

if ( hasRank && rank.current ) {
Q_strncpyz( buffer, rank.current->name, sizeof( buffer ) );
} else {
Q_strncpyz( buffer, "--", sizeof( buffer ) );
}
PlayerSettings_DrawStatsLabelValue( STATS_ROW_CURRENT_RANK, s_statsLabelCurrentRank, buffer );

if ( hasRank && rank.next ) {
Com_sprintf( buffer, sizeof( buffer ), "%s (%d)", rank.next->name, rank.next->minimumScore - stats->playerScore );
} else {
Q_strncpyz( buffer, "--", sizeof( buffer ) );
}
PlayerSettings_DrawStatsLabelValue( STATS_ROW_NEXT_RANK, s_statsLabelNextRank, buffer );

Com_sprintf( buffer, sizeof( buffer ), "%d", stats->gamesPlayed );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_GAMES_PLAYED, "Games Played", buffer );

Com_sprintf( buffer, sizeof( buffer ), "%.2f km", stats->distanceKm );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DISTANCE, "Distance Driven", buffer );

Com_sprintf( buffer, sizeof( buffer ), "%.1f L", stats->fuelUsed );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_FUEL, "Fuel Used", buffer );

        Com_sprintf( buffer, sizeof( buffer ), "%.1f km/h", stats->topSpeedKph );
        PlayerSettings_DrawStatsLabelValue( STATS_ROW_TOP_SPEED, "Top Speed", buffer );

        if ( stats->bestLapMs > 0 ) {
                int minutes = stats->bestLapMs / 60000;
                int seconds = ( stats->bestLapMs % 60000 ) / 1000;
                int millis = stats->bestLapMs % 1000;
                Com_sprintf( buffer, sizeof( buffer ), "%02d:%02d.%03d", minutes, seconds, millis );
        } else {
                Q_strncpyz( buffer, "--", sizeof( buffer ) );
        }
        PlayerSettings_DrawStatsLabelValue( STATS_ROW_BEST_LAP, "Best Lap", buffer );

        Com_sprintf( buffer, sizeof( buffer ), "%d / %d", stats->kills, stats->deaths );
        PlayerSettings_DrawStatsLabelValue( STATS_ROW_KILLS, "Kills / Deaths", buffer );

        Com_sprintf( buffer, sizeof( buffer ), "%d", stats->damageDealt );
        PlayerSettings_DrawStatsLabelValue( STATS_ROW_DAMAGE_DEALT, "Damage Dealt", buffer );

        Com_sprintf( buffer, sizeof( buffer ), "%d", stats->damageTaken );
        PlayerSettings_DrawStatsLabelValue( STATS_ROW_DAMAGE_TAKEN, "Damage Taken", buffer );

Com_sprintf( buffer, sizeof( buffer ), "%d / %d", stats->wins, stats->losses );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_WINS, "Wins / Losses", buffer );

Com_sprintf( buffer, sizeof( buffer ), "%d / %d", stats->flagCaptures, stats->flagAssists );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_FLAGS_CAPTURED, "Flags Cap / Assists", buffer );

Com_sprintf( buffer, sizeof( buffer ), "Acc: %d  Exc: %d  Imp: %d  Perf: %d",
        stats->accuracyAwards, stats->excellentAwards,
        stats->impressiveAwards, stats->perfectAwards );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_AWARDS, "Awards", buffer );

        if ( stats->mostUsedVehicle[0] ) {
                Q_strncpyz( vehicleName, stats->mostUsedVehicle, sizeof( vehicleName ) );
                slash = strchr( vehicleName, '/' );
                if ( slash ) { *slash = '\0'; }
                if ( vehicleName[0] >= 'a' && vehicleName[0] <= 'z' ) {
                        vehicleName[0] = vehicleName[0] - 'a' + 'A';
                }
                if ( stats->mostUsedVehicleTimeMs > 0 ) {
                        int hours   = stats->mostUsedVehicleTimeMs / ( 1000 * 60 * 60 );
                        int minutes = ( stats->mostUsedVehicleTimeMs / ( 1000 * 60 ) ) % 60;
                        int seconds = ( stats->mostUsedVehicleTimeMs / 1000 ) % 60;
                        Com_sprintf( vehicleTime, sizeof( vehicleTime ), "%02d:%02d:%02d", hours, minutes, seconds );
                } else {
                        Q_strncpyz( vehicleTime, "--", sizeof( vehicleTime ) );
                }
                Com_sprintf( buffer, sizeof( buffer ), "%s -- %s", vehicleName, vehicleTime );
        } else {
                Q_strncpyz( buffer, "--", sizeof( buffer ) );
        }
        PlayerSettings_DrawStatsLabelValue( STATS_ROW_VEHICLE, "Most Used Vehicle", buffer );

/* ── Racing ──────────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_RACING_HEADER, "--- RACING ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->racingWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_RACING_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->racingPodiums );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_RACING_PODIUMS, "Podiums", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->racingCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_RACING_COMPLETED, "Races Completed", buffer );

/* ── Racing DM ───────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_RACING_DM_HEADER, "--- RACING DM ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->racingDmWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_RACING_DM_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->racingDmPodiums );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_RACING_DM_PODIUMS, "Podiums", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->racingDmCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_RACING_DM_COMPLETED, "Races Completed", buffer );

/* ── Sprint ──────────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_SPRINT_HEADER, "--- SPRINT ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->sprintWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_SPRINT_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->sprintCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_SPRINT_COMPLETED, "Completed", buffer );
if ( stats->sprintBestMs > 0 ) {
        int minutes = stats->sprintBestMs / 60000;
        int seconds = ( stats->sprintBestMs % 60000 ) / 1000;
        int millis  = stats->sprintBestMs % 1000;
        Com_sprintf( buffer, sizeof( buffer ), "%02d:%02d.%03d", minutes, seconds, millis );
} else {
        Q_strncpyz( buffer, "--", sizeof( buffer ) );
}
PlayerSettings_DrawStatsLabelValue( STATS_ROW_SPRINT_BEST, "Best Time", buffer );

/* ── Derby ───────────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DERBY_HEADER, "--- DERBY ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->derbyWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DERBY_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->derbyCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DERBY_COMPLETED, "Completed", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->derbyKills );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DERBY_KILLS, "Kills", buffer );

/* ── LCS ─────────────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_LCS_HEADER, "--- LAST CAR STANDING ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->lcsWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_LCS_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->lcsCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_LCS_COMPLETED, "Completed", buffer );

/* ── Elimination ─────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_ELIM_HEADER, "--- ELIMINATION ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->eliminationWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_ELIM_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->eliminationCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_ELIM_COMPLETED, "Completed", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->eliminationTotalRoundsLasted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_ELIM_ROUNDS, "Total Rounds Lasted", buffer );

/* ── Deathmatch ──────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DM_HEADER, "--- DEATHMATCH ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->dmWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DM_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->dmCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DM_COMPLETED, "Completed", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->dmKills );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DM_KILLS, "Kills", buffer );

/* ── Team DM ─────────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_HEADER, "--- TEAM DEATHMATCH ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->teamWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->teamCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_COMPLETED, "Completed", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->teamKills );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_KILLS, "Kills", buffer );

/* ── Team Racing ─────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_RACING_HEADER, "--- TEAM RACING ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->teamRacingWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_RACING_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->teamRacingPodiums );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_RACING_PODIUMS, "Podiums", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->teamRacingCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_RACING_COMPLETED, "Races Completed", buffer );

/* ── Team Racing DM ──────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_RACING_DM_HEADER, "--- TEAM RACING DM ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->teamRacingDmWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_RACING_DM_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->teamRacingDmPodiums );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_RACING_DM_PODIUMS, "Podiums", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->teamRacingDmCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_TEAM_RACING_DM_COMPLETED, "Races Completed", buffer );

/* ── CTF ─────────────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_CTF_HEADER, "--- CAPTURE THE FLAG ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->ctfWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_CTF_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->ctfCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_CTF_COMPLETED, "Completed", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->ctfCaptures );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_CTF_CAPTURES, "Flag Captures", buffer );

/* ── CTF4 ────────────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_CTF4_HEADER, "--- 4-TEAM CTF ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->ctf4Wins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_CTF4_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->ctf4Completed );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_CTF4_COMPLETED, "Completed", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->ctf4Captures );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_CTF4_CAPTURES, "Flag Captures", buffer );

/* ── Domination ──────────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DOM_HEADER, "--- DOMINATION ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->dominationWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DOM_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->dominationCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_DOM_COMPLETED, "Completed", buffer );

/* ── King of the Hill ────────────────────────────────────────────── */
PlayerSettings_DrawStatsLabelValue( STATS_ROW_KOTH_HEADER, "--- KING OF THE HILL ---", "" );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->kothWins );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_KOTH_WINS, "Wins", buffer );
Com_sprintf( buffer, sizeof( buffer ), "%d", stats->kothCompleted );
PlayerSettings_DrawStatsLabelValue( STATS_ROW_KOTH_COMPLETED, "Completed", buffer );
}



static qboolean PlayerSettings_IsStatsRowVisible( int row ) {
        const playersettingsPaginationInfo_t *info;

        if ( row < 0 || row >= STATS_ROW_COUNT ) {
                return qfalse;
        }

        info = &s_playersettings.statsPaginationInfo;
        if ( info->rowCount != STATS_ROW_COUNT ) {
                info = PlayerSettings_UpdateStatsPaginationInfo();
        }

        if ( !info || info->lastRow < info->firstRow ) {
                return qfalse;
        }

        return ( row >= info->firstRow && row <= info->lastRow );
}

static void PlayerSettings_DrawStatsLabelValueWithColors( int row, const char *label, const vec4_t labelColor, const char *value, const vec4_t valueColor ) {
        int rowTop;
        int rowBottom;
        int y;
        int labelX;
        int valueX;
        vec4_t mutableLabelColor;
        vec4_t mutableValueColor;
        float viewportTop;
        float viewportBottom;

        if ( !PlayerSettings_IsStatsRowVisible( row ) ) {
                return;
        }

        labelX = PLAYERSETTINGS_PROFILE_FIELD_LEFT + PLAYERSETTINGS_PROFILE_LABEL_OFFSET;
        valueX = PLAYERSETTINGS_PROFILE_FIELD_LEFT + PLAYERSETTINGS_STATS_VALUE_OFFSET;

        PlayerSettings_GetStatsRowBounds( row, &rowTop, &rowBottom );
        PlayerSettings_GetPaginatedViewportBounds(
                PlayerSettings_GetStatsPageContentHeight(),
                PLAYERSETTINGS_STATS_PAGINATION_RESERVED_HEIGHT,
                &viewportTop,
                &viewportBottom );
	if ( rowBottom <= (int)viewportTop || rowTop >= (int)viewportBottom ) {
		return;
	}

	y = rowTop + PLAYERSETTINGS_STATS_VALUE_BASELINE;

	Vector4Copy( labelColor, mutableLabelColor );
	Vector4Copy( valueColor, mutableValueColor );
	if ( label && label[0] == '-' && label[1] == '-' ) {
		Vector4Copy( playerSettingsAccentColor, mutableLabelColor );
	}

	if ( label && label[0] ) {
		Frontend_DrawText( labelX, y, label, UI_LEFT | UI_SMALLFONT,
		                   mutableLabelColor );
	}

	if ( value && value[0] ) {
		Frontend_DrawText( valueX, y, value, UI_LEFT | UI_SMALLFONT,
		                   mutableValueColor );
	}
}


static void PlayerSettings_DrawStatsLabelValue( int row, const char *label, const char *value ) {
	PlayerSettings_DrawStatsLabelValueWithColors( row, label, playerSettingsMutedColor,
	                                             value, playerSettingsTextColor );
}


static void PlayerSettings_DrawStatsMessage( int row, const char *message ) {
        int rowTop;
        int rowBottom;
        int y;
        int x;
        float viewportTop;
        float viewportBottom;

        if ( !PlayerSettings_IsStatsRowVisible( row ) ) {
                return;
        }

        x = PLAYERSETTINGS_PROFILE_FIELD_LEFT + PLAYERSETTINGS_PROFILE_LABEL_OFFSET;

        PlayerSettings_GetStatsRowBounds( row, &rowTop, &rowBottom );
        PlayerSettings_GetPaginatedViewportBounds(
                PlayerSettings_GetStatsPageContentHeight(),
                PLAYERSETTINGS_STATS_PAGINATION_RESERVED_HEIGHT,
                &viewportTop,
                &viewportBottom );
	if ( rowBottom <= (int)viewportTop || rowTop >= (int)viewportBottom ) {
		return;
	}

	y = rowTop + PLAYERSETTINGS_STATS_VALUE_BASELINE;

	Frontend_DrawText( x, y, message, UI_LEFT | UI_SMALLFONT,
	                   playerSettingsTextColor );
}



/*
=================
PlayerSettings_DrawBackShaders
=================
*/
static void PlayerSettings_DrawBackShaders( void ) {
        const char *statusLabel;

        Frontend_DrawBackground( playerSettingsScrimColor );
        Frontend_DrawPanel( PLAYERSETTINGS_FRAME_X, PLAYERSETTINGS_FRAME_Y,
                            PLAYERSETTINGS_FRAME_WIDTH,
                            PLAYERSETTINGS_FRAME_HEIGHT, uis.tFrac,
                            UI_FRONTEND_STYLE_FRAME );
        Frontend_DrawText( PLAYERSETTINGS_FRAME_X + 24,
                           PLAYERSETTINGS_FRAME_Y + 24,
                           "Driver", UI_LEFT | UI_BIGFONT,
                           playerSettingsTextColor );
        Frontend_DrawText( PLAYERSETTINGS_FRAME_X + 24,
                           PLAYERSETTINGS_FRAME_Y + 48,
                           "Shape your driver identity and active vehicle",
                           UI_LEFT | UI_SMALLFONT, playerSettingsMutedColor );

        switch ( s_playersettings.currentTab ) {
        case TAB_VEHICLE:
                statusLabel = "Garage";
                PlayerSettings_DrawVehiclePanelBackground();
                break;
        case TAB_STATS:
                statusLabel = "Stats";
                PlayerSettings_DrawStatsPanelBackground();
                break;
        case TAB_ACHIEVEMENTS:
                statusLabel = "Awards";
                PlayerSettings_DrawAchievementsPanelBackground();
                break;
        case TAB_PROFILE:
        default:
                statusLabel = "Driver";
                PlayerSettings_DrawProfilePanelBackground();
                break;
        }

        Frontend_DrawStatusChip( PLAYERSETTINGS_FRAME_X +
                                 PLAYERSETTINGS_FRAME_WIDTH - 104,
                                 PLAYERSETTINGS_FRAME_Y + 26,
                                 statusLabel, playerSettingsStatusColor,
                                 uis.tFrac );

        Menu_Draw( &s_playersettings.menu );

        if ( s_playersettings.currentTab == TAB_STATS ) {
                PlayerSettings_DrawStatsTab();
                PlayerSettings_DrawStatsPaginationControls();
        } else if ( s_playersettings.currentTab == TAB_ACHIEVEMENTS ) {
                PlayerSettings_DrawAchievementsTab();
                PlayerSettings_DrawAchievementsPaginationControls();
        }

        Frontend_DrawText( PLAYERSETTINGS_FRAME_X + 166,
                           PLAYERSETTINGS_FRAME_Y + 408,
                           "Enter edit     Tab switch     Esc back",
                           UI_LEFT | UI_SMALLFONT, playerSettingsMutedColor );
}

static void PlayerSettings_GetAchievementRowBounds( int row, int *top, int *bottom ) {
	float	rowTop;
	float	rowBottom;
	float	spacing;
	float	contentTop;
	float	offset;
	const playersettingsPaginationInfo_t *paginationInfo;

	paginationInfo = &s_playersettings.achievementsPaginationInfo;
	if ( s_playersettings.achievementsPaginationInfo.rowCount != PLAYERSETTINGS_ACHIEVEMENT_ROW_COUNT ) {
		paginationInfo = PlayerSettings_UpdateAchievementsPaginationInfo();
	}

	if ( row < 0 ) {
		row = 0;
	}
	if ( row >= PLAYERSETTINGS_ACHIEVEMENT_ROW_COUNT ) {
		row = PLAYERSETTINGS_ACHIEVEMENT_ROW_COUNT - 1;
	}

	spacing = PlayerSettings_GetAchievementsRowSpacing();
	contentTop = PlayerSettings_GetScrollContentTop();
	offset = paginationInfo ? paginationInfo->rowOffset : s_playersettings.achievementsPaginationInfo.rowOffset;
	rowTop = contentTop + row * spacing - offset;
	rowBottom = rowTop + PLAYERSETTINGS_ACHIEVEMENT_ROW_HEIGHT + 40;

	if ( top ) {
		*top = (int)rowTop;
	}
	if ( bottom ) {
		*bottom = (int)rowBottom;
	}
}

static void PlayerSettings_DrawAchievementsPanelBackground( void ) {
vec4_t borderColor;
vec4_t titleColor;
vec4_t titleAccentColor;
vec4_t rowColor;
float contentHeight;
float viewportTop;
float viewportBottom;
int panelTop;
int panelBottom;
int i;
const playersettingsPaginationInfo_t *info;

contentHeight = PlayerSettings_GetAchievementsContentHeight();
info = PlayerSettings_UpdateAchievementsPaginationInfo();

panelTop = PLAYERSETTINGS_PROFILE_PANEL_TOP;
panelBottom = (int)PlayerSettings_GetPanelBottomForContent( contentHeight );

Frontend_DrawPanel( PLAYERSETTINGS_PROFILE_PANEL_LEFT, panelTop,
                    PLAYERSETTINGS_PROFILE_PANEL_WIDTH,
                    panelBottom - panelTop, uis.tFrac,
                    UI_FRONTEND_STYLE_SURFACE );

Vector4Copy( playerSettingsTextColor, titleColor );
titleColor[3] *= uis.tFrac;
Vector4Copy( playerSettingsAccentColor, titleAccentColor );
titleAccentColor[3] *= uis.tFrac;
UI_FillRect( PLAYERSETTINGS_PROFILE_FIELD_LEFT + 16, panelTop + 10,
             UI_FRONTEND_STATUS_DOT, UI_FRONTEND_STATUS_DOT, titleAccentColor );
Frontend_DrawText( PLAYERSETTINGS_PROFILE_FIELD_LEFT + 32,
                   panelTop + 4,
                   "Achievements", UI_LEFT | UI_BIGFONT,
                   titleColor );
UI_FillRect( PLAYERSETTINGS_PROFILE_FIELD_LEFT + 32, panelTop + 24,
             Frontend_TextWidth( "Achievements", UI_BIGFONT ), 2,
             titleAccentColor );

Vector4Copy( profileRowBorderColor, borderColor );
borderColor[3] *= uis.tFrac;

PlayerSettings_GetPaginatedViewportBounds(
		contentHeight,
		PLAYERSETTINGS_ACHIEVEMENTS_PAGINATION_RESERVED_HEIGHT,
		&viewportTop,
		&viewportBottom );

if ( !info || info->lastRow < info->firstRow ) {
	return;
}

for ( i = info->firstRow; i <= info->lastRow; ++i ) {
	int rowTop;
	int rowBottom;

	PlayerSettings_GetAchievementRowBounds( i, &rowTop, &rowBottom );

	if ( rowTop < (int)viewportTop ) {
		rowTop = (int)viewportTop;
	}
	if ( rowBottom > (int)viewportBottom ) {
		rowBottom = (int)viewportBottom;
	}
	if ( rowBottom <= rowTop ) {
		continue;
	}

	Vector4Copy( ( i & 1 ) ? profileRowOddFillColor : profileRowEvenFillColor, rowColor );
	rowColor[3] *= uis.tFrac * 0.55f;
	UI_FillRect( PLAYERSETTINGS_PROFILE_FIELD_LEFT, rowTop,
	             PLAYERSETTINGS_PROFILE_PANEL_WIDTH - PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN * 2,
	             rowBottom - rowTop, rowColor );
	UI_FillRect( PLAYERSETTINGS_PROFILE_FIELD_LEFT, rowBottom - 1,
	             PLAYERSETTINGS_PROFILE_PANEL_WIDTH - PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN * 2,
	             1, borderColor );
}
}

static int PlayerSettings_DrawAchievementSection( int row, const char *title, const playersettingsAchievementTierDef_t *tiers, int count, double progress, playersettingsAchievementIcon_t iconIndex, const playersettingsPaginationInfo_t *paginationInfo ) {
        int i;
        int rowTop;
        int rowBottom;
        int titleX;
        int titleY;
        float areaLeft;
        float areaRight;
        float availableWidth;
        float columnWidth;
        float columnGap;
        float gridTop;
        int unlockedCount;
        qboolean entryUnlocked[PLAYERSETTINGS_MAX_ACHIEVEMENT_TIERS];
        qhandle_t lockedIconHandle;
        qhandle_t unlockedIconHandle;
        float viewportTop;
        float viewportBottom;
        qboolean visible;
        int tierPageCount;
        int tiersPerPage;
        int pageIndex;
        int startTier;
        int endTier;

        if ( count > PLAYERSETTINGS_MAX_ACHIEVEMENT_TIERS ) {
                count = PLAYERSETTINGS_MAX_ACHIEVEMENT_TIERS;
        }
        if ( count <= 0 || !tiers ) {
                return 0;
        }

        lockedIconHandle = 0;
        unlockedIconHandle = 0;
        if ( iconIndex >= 0 && iconIndex < PLAYERSETTINGS_ACHIEVEMENT_ICON_COUNT ) {
                lockedIconHandle = s_playersettings.achievementMedalLocked[iconIndex];
                unlockedIconHandle = s_playersettings.achievementMedalUnlocked[iconIndex];
        }

        unlockedCount = 0;
        for ( i = 0; i < count; ++i ) {
                entryUnlocked[i] = ( progress >= tiers[i].threshold );
                if ( entryUnlocked[i] ) {
                        ++unlockedCount;
                }
        }

        PlayerSettings_GetAchievementRowBounds( row, &rowTop, &rowBottom );
        PlayerSettings_GetPaginatedViewportBounds(
                PlayerSettings_GetAchievementsContentHeight(),
                PLAYERSETTINGS_ACHIEVEMENTS_PAGINATION_RESERVED_HEIGHT,
                &viewportTop,
                &viewportBottom );
        visible = ( rowBottom > (int)viewportTop && rowTop < (int)viewportBottom );
        if ( paginationInfo && ( row < paginationInfo->firstRow || row > paginationInfo->lastRow ) ) {
                visible = qfalse;
        }

        titleX = PLAYERSETTINGS_PROFILE_FIELD_LEFT + PLAYERSETTINGS_PROFILE_LABEL_OFFSET;
        titleY = rowTop + PLAYERSETTINGS_ACHIEVEMENT_TITLE_OFFSET;

        if ( visible ) {
                Frontend_DrawText( titleX, titleY, title, UI_LEFT | UI_SMALLFONT,
                                   playerSettingsAccentColor );
        }

        areaLeft = PLAYERSETTINGS_PROFILE_FIELD_LEFT + PLAYERSETTINGS_ACHIEVEMENT_CONTENT_MARGIN;
        areaRight = PLAYERSETTINGS_PROFILE_ROW_RIGHT - PLAYERSETTINGS_ACHIEVEMENT_CONTENT_MARGIN;
        if ( areaRight <= areaLeft ) {
                areaRight = areaLeft + 1.0f;
        }

        availableWidth = areaRight - areaLeft;
        columnGap = ( PLAYERSETTINGS_ACHIEVEMENTS_PER_LINE > 1 ) ? PLAYERSETTINGS_ACHIEVEMENT_COLUMN_GAP : 0.0f;
        columnWidth = availableWidth - columnGap * ( PLAYERSETTINGS_ACHIEVEMENTS_PER_LINE - 1 );
        if ( PLAYERSETTINGS_ACHIEVEMENTS_PER_LINE > 0 ) {
                columnWidth /= PLAYERSETTINGS_ACHIEVEMENTS_PER_LINE;
        }
        if ( columnWidth < PLAYERSETTINGS_ACHIEVEMENT_MEDAL_SIZE ) {
                columnWidth = PLAYERSETTINGS_ACHIEVEMENT_MEDAL_SIZE;
        }

        gridTop = rowTop + PLAYERSETTINGS_ACHIEVEMENT_TITLE_OFFSET + PLAYERSETTINGS_ACHIEVEMENT_HEADER_LINE_HEIGHT + PLAYERSETTINGS_ACHIEVEMENT_HEADER_GAP;

        tiersPerPage = PLAYERSETTINGS_ACHIEVEMENTS_PER_PAGE;
        if ( tiersPerPage <= 0 ) {
                tiersPerPage = count;
        }

        tierPageCount = PlayerSettings_GetAchievementTierPagesForCount( count );
        if ( tierPageCount < 1 ) {
                tierPageCount = 1;
        }

	pageIndex = s_playersettings.achievementsTierPagination.currentPage;
        if ( pageIndex < 0 ) {
                pageIndex = 0;
        } else if ( pageIndex >= tierPageCount ) {
                pageIndex = tierPageCount - 1;
        }

        startTier = pageIndex * tiersPerPage;
        if ( startTier < 0 ) {
                startTier = 0;
        } else if ( startTier > count ) {
                startTier = count;
        }

        endTier = startTier + tiersPerPage;
        if ( endTier > count ) {
                endTier = count;
        }

        if ( visible ) {
                for ( i = startTier; i < count && i < endTier; ++i ) {
                        int column;
                        int tierRow;
                        float entryLeft;
                        float entryTop;
                        float textX;
                        float nameY;
                        float descriptionY;
                        qhandle_t iconHandle;
                        float iconWidth;
                        const char *name;
                        const char *description;

                        column = ( i - startTier ) % PLAYERSETTINGS_ACHIEVEMENTS_PER_LINE;
                        tierRow = ( i - startTier ) / PLAYERSETTINGS_ACHIEVEMENTS_PER_LINE;
                        entryLeft = areaLeft + column * ( columnWidth + columnGap );
                        entryTop = gridTop + tierRow * ( PLAYERSETTINGS_ACHIEVEMENT_MEDAL_SIZE + PLAYERSETTINGS_ACHIEVEMENT_ENTRY_VERTICAL_GAP );

                        iconHandle = entryUnlocked[i] ? unlockedIconHandle : lockedIconHandle;
                        iconWidth = ( iconHandle != 0 ) ? PLAYERSETTINGS_ACHIEVEMENT_MEDAL_SIZE : 0.0f;
                        if ( iconWidth > 0.0f ) {
                                UI_DrawHandlePic( (int)entryLeft, (int)entryTop, PLAYERSETTINGS_ACHIEVEMENT_MEDAL_SIZE, PLAYERSETTINGS_ACHIEVEMENT_MEDAL_SIZE, iconHandle );
                        }

                        textX = entryLeft;
                        if ( iconWidth > 0.0f ) {
                                textX += iconWidth + PLAYERSETTINGS_ACHIEVEMENT_TEXT_GAP;
                        }

                        name = tiers[i].name;
                        description = tiers[i].description;
                        nameY = entryTop + 8.0f;
                        descriptionY = nameY + 14.0f;

                        PlayerSettings_DrawAchievementText(
                                (int)textX, (int)nameY,
                                (int)( entryLeft + columnWidth ), name,
                                entryUnlocked[i] ? playerSettingsAccentColor : playerSettingsMutedColor );
                        PlayerSettings_DrawAchievementDescription(
                                (int)textX, (int)descriptionY,
                                (int)( entryLeft + columnWidth ), description,
                                entryUnlocked[i] ? playerSettingsTextColor : playerSettingsMutedColor );
                }
        }

        return unlockedCount;
}

static void PlayerSettings_DrawAchievementsTab( void ) {
const profile_stats_t *stats;
    int unlockedAchievements;
int displayTotalAchievements;
char progressBuffer[32];
char headerBuffer[64];
    int row;
    int winTierCount;
    int winFirstPageCount;
    int winSecondPageCount;
    int sprintTierCount;
    int sprintFirstPageCount;
    int sprintSecondPageCount;
    const playersettingsPaginationInfo_t *paginationInfo;

paginationInfo = PlayerSettings_UpdateAchievementsPaginationInfo();
PlayerSettings_ClampAchievementTierPage();

        if ( !UI_Profile_HasActiveProfile() ) {
                int messageX;

                messageX = PLAYERSETTINGS_PROFILE_FIELD_LEFT + PLAYERSETTINGS_PROFILE_LABEL_OFFSET;
                Frontend_DrawText( messageX, 216, "No active profile selected.",
                                   UI_LEFT | UI_SMALLFONT, playerSettingsTextColor );
                Frontend_DrawText( messageX, 240, "Create or select a profile from the main menu.",
                                   UI_LEFT | UI_SMALLFONT, playerSettingsMutedColor );
                return;
        }

        stats = UI_Profile_GetActiveStats();
        if ( !stats ) {
                int messageX;

                messageX = PLAYERSETTINGS_PROFILE_FIELD_LEFT + PLAYERSETTINGS_PROFILE_LABEL_OFFSET;
                Frontend_DrawText( messageX, 220, "Unable to read profile statistics.",
                                   UI_LEFT | UI_SMALLFONT, playerSettingsTextColor );
                return;
        }

        unlockedAchievements = 0;
        displayTotalAchievements = PLAYERSETTINGS_DISPLAY_ACHIEVEMENT_TOTAL;

        winTierCount = ARRAY_LEN( s_winAchievementTiers );
        winFirstPageCount = winTierCount < PLAYERSETTINGS_ACHIEVEMENTS_PER_PAGE ? winTierCount : PLAYERSETTINGS_ACHIEVEMENTS_PER_PAGE;
        winSecondPageCount = winTierCount - winFirstPageCount;

        sprintTierCount = ARRAY_LEN( s_sprintWinAchievementTiers );
        sprintFirstPageCount = sprintTierCount < PLAYERSETTINGS_ACHIEVEMENTS_PER_PAGE ? sprintTierCount : PLAYERSETTINGS_ACHIEVEMENTS_PER_PAGE;
        sprintSecondPageCount = sprintTierCount - sprintFirstPageCount;

        row = PLAYERSETTINGS_ACHIEVEMENT_FIRST_SECTION_ROW;
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Distance Driven (1/2)", s_distanceAchievementTiers, 4, stats->distanceKm, PLAYERSETTINGS_ACHIEVEMENT_ICON_DRIVEN, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Distance Driven (2/2)", &s_distanceAchievementTiers[4], 4, stats->distanceKm, PLAYERSETTINGS_ACHIEVEMENT_ICON_DRIVEN, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Kills (1/2)", s_killAchievementTiers, 4, (double)stats->kills, PLAYERSETTINGS_ACHIEVEMENT_ICON_KILLS, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Kills (2/2)", &s_killAchievementTiers[4], 4, (double)stats->kills, PLAYERSETTINGS_ACHIEVEMENT_ICON_KILLS, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Races Won (1/2)", s_winAchievementTiers, winFirstPageCount, (double)stats->wins, PLAYERSETTINGS_ACHIEVEMENT_ICON_WINS, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Races Won (2/2)", &s_winAchievementTiers[winFirstPageCount], winSecondPageCount, (double)stats->wins, PLAYERSETTINGS_ACHIEVEMENT_ICON_WINS, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Sprint Wins (1/2)", s_sprintWinAchievementTiers, sprintFirstPageCount, (double)stats->sprintWins, PLAYERSETTINGS_ACHIEVEMENT_ICON_WINS, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Sprint Wins (2/2)", &s_sprintWinAchievementTiers[sprintFirstPageCount], sprintSecondPageCount, (double)stats->sprintWins, PLAYERSETTINGS_ACHIEVEMENT_ICON_WINS, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Flags Captured (1/2)", s_flagCaptureAchievementTiers, 4, (double)stats->flagCaptures, PLAYERSETTINGS_ACHIEVEMENT_ICON_FLAGS, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Flags Captured (2/2)", &s_flagCaptureAchievementTiers[4], 4, (double)stats->flagCaptures, PLAYERSETTINGS_ACHIEVEMENT_ICON_FLAGS, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Flag Assists (1/2)", s_flagAssistAchievementTiers, 4, (double)stats->flagAssists, PLAYERSETTINGS_ACHIEVEMENT_ICON_FLAG_ASSISTS, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Flag Assists (2/2)", &s_flagAssistAchievementTiers[4], 4, (double)stats->flagAssists, PLAYERSETTINGS_ACHIEVEMENT_ICON_FLAG_ASSISTS, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Fuel Consumed (1/2)", s_fuelAchievementTiers, 4, stats->fuelUsed, PLAYERSETTINGS_ACHIEVEMENT_ICON_FUEL, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Fuel Consumed (2/2)", &s_fuelAchievementTiers[4], 4, stats->fuelUsed, PLAYERSETTINGS_ACHIEVEMENT_ICON_FUEL, paginationInfo );

        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Accuracy (1/2)", s_accuracyAchievementTiers, 4, (double)stats->accuracyAwards, PLAYERSETTINGS_ACHIEVEMENT_ICON_ACCURACY, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Accuracy (2/2)", &s_accuracyAchievementTiers[4], 4, (double)stats->accuracyAwards, PLAYERSETTINGS_ACHIEVEMENT_ICON_ACCURACY, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Excellent (1/2)", s_excellentAchievementTiers, 4, (double)stats->excellentAwards, PLAYERSETTINGS_ACHIEVEMENT_ICON_EXCELLENT, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Excellent (2/2)", &s_excellentAchievementTiers[4], 4, (double)stats->excellentAwards, PLAYERSETTINGS_ACHIEVEMENT_ICON_EXCELLENT, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Impressive (1/2)", s_impressiveAchievementTiers, 4, (double)stats->impressiveAwards, PLAYERSETTINGS_ACHIEVEMENT_ICON_IMPRESSIVE, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Impressive (2/2)", &s_impressiveAchievementTiers[4], 4, (double)stats->impressiveAwards, PLAYERSETTINGS_ACHIEVEMENT_ICON_IMPRESSIVE, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Perfect (1/2)", s_perfectAchievementTiers, 4, (double)stats->perfectAwards, PLAYERSETTINGS_ACHIEVEMENT_ICON_PERFECT, paginationInfo );
        unlockedAchievements += PlayerSettings_DrawAchievementSection( row++, "Perfect (2/2)", &s_perfectAchievementTiers[4], 4, (double)stats->perfectAwards, PLAYERSETTINGS_ACHIEVEMENT_ICON_PERFECT, paginationInfo );

        if ( unlockedAchievements > displayTotalAchievements ) {
                unlockedAchievements = displayTotalAchievements;
        }

        Com_sprintf( progressBuffer, sizeof( progressBuffer ), "%d/%d", unlockedAchievements, displayTotalAchievements );
        Com_sprintf( headerBuffer, sizeof( headerBuffer ), "Achievements %s", progressBuffer );

        Frontend_DrawText( PLAYERSETTINGS_PROFILE_FIELD_LEFT + 32,
                           PLAYERSETTINGS_PROFILE_PANEL_TOP + 28,
                           headerBuffer, UI_LEFT | UI_SMALLFONT,
                           playerSettingsAccentColor );
        Frontend_DrawText( PLAYERSETTINGS_PROFILE_FIELD_LEFT + 32,
                           PLAYERSETTINGS_PROFILE_PANEL_TOP + 42,
                           "Complete challenges to unlock new medals.",
                           UI_LEFT | UI_SMALLFONT, playerSettingsMutedColor );
}

static void PlayerSettings_SetTab( int tab ) {
	int i;
	qboolean showProfile;
	qboolean showVehicle;

	if ( tab < TAB_PROFILE || tab >= PLAYERSETTINGS_TAB_COUNT ) {
		tab = TAB_PROFILE;
	}

	s_playersettings.currentTab = tab;

showProfile = ( tab == TAB_PROFILE );
showVehicle = ( tab == TAB_VEHICLE );

	PlayerSettings_SetWidgetVisible( &s_playersettings.name.generic, showProfile );
	PlayerSettings_SetWidgetVisible( &s_playersettings.gender.generic, showProfile );
	/* Birth date labels are drawn by the three modern date controls. */
	PlayerSettings_SetWidgetVisible( &s_playersettings.birthDay.generic, showProfile );
	PlayerSettings_SetWidgetVisible( &s_playersettings.birthMonth.generic, showProfile );
	PlayerSettings_SetWidgetVisible( &s_playersettings.birthYear.generic, showProfile );
	PlayerSettings_SetWidgetVisible( &s_playersettings.avatar.generic, showProfile );
	PlayerSettings_SetWidgetVisible( &s_playersettings.country.generic, showProfile );
	PlayerSettings_SetWidgetVisible( &s_playersettings.handicap.generic, showProfile );
	PlayerSettings_SetWidgetVisible( &s_playersettings.effects.generic, showProfile );
	/* The vehicle tab draws its own compact favorite strip. Keep the legacy
	 * heading hidden so it cannot reintroduce the old layout. */
	PlayerSettings_SetWidgetVisible( &s_playersettings.favorites.generic, qfalse );

	for ( i = 0; i < NUM_FAVORITES; ++i ) {
		PlayerSettings_SetWidgetVisible( &s_playersettings.ports[i].generic, qfalse );
		PlayerSettings_SetWidgetVisible( &s_playersettings.favpicbuttons[i].generic, showVehicle );
		PlayerSettings_SetWidgetVisible( &s_playersettings.favpics[i].generic, qfalse );
	}

	PlayerSettings_SetWidgetVisible( &s_playersettings.player.generic, showVehicle );
	PlayerSettings_SetWidgetVisible( &s_playersettings.left.generic, showVehicle );
	PlayerSettings_SetWidgetVisible( &s_playersettings.right.generic, showVehicle );
	PlayerSettings_SetWidgetVisible( &s_playersettings.modelname.generic, qfalse );
	PlayerSettings_SetWidgetVisible( &s_playersettings.customize.generic, showVehicle );
	PlayerSettings_SetWidgetVisible( &s_playersettings.plate.generic, showVehicle );

	s_playersettings.tabProfile.color = ( tab == TAB_PROFILE ) ? text_color_highlight : uis.text_color;
	s_playersettings.tabVehicle.color = ( tab == TAB_VEHICLE ) ? text_color_highlight : uis.text_color;
	s_playersettings.tabStats.color = ( tab == TAB_STATS ) ? text_color_highlight : uis.text_color;
	s_playersettings.tabAchievements.color = ( tab == TAB_ACHIEVEMENTS ) ? text_color_highlight : uis.text_color;

	switch ( tab ) {
	case TAB_PROFILE:
		Menu_SetCursorToItem( &s_playersettings.menu, &s_playersettings.tabProfile );
		break;
	case TAB_STATS:
		Menu_SetCursorToItem( &s_playersettings.menu, &s_playersettings.tabStats );
		break;
	case TAB_ACHIEVEMENTS:
		Menu_SetCursorToItem( &s_playersettings.menu, &s_playersettings.tabAchievements );
		break;
	case TAB_VEHICLE:
		Menu_SetCursorToItem( &s_playersettings.menu, &s_playersettings.tabVehicle );
		break;
	default:
		Menu_SetCursorToItem( &s_playersettings.menu, &s_playersettings.tabProfile );
		break;
	}
}

/*
=================
PlayerSettings_UpdateModel
=================
*/
static void PlayerSettings_UpdateModel( void )
{
	vec3_t	viewangles;
	vec3_t	moveangles;
	char	plate[MAX_QPATH];

	memset( &s_playersettings.playerinfo, 0, sizeof(playerInfo_t) );
	
	VectorClear( viewangles );
	VectorClear( moveangles );

	trap_Cvar_VariableStringBuffer( "plate", plate, sizeof( plate ) );
	UI_PlayerInfo_SetModel( &s_playersettings.playerinfo, s_playersettings.modelskin, s_playersettings.rimskin, s_playersettings.headskin, plate);
	UI_PlayerInfo_SetInfo( &s_playersettings.playerinfo, LEGS_IDLE, TORSO_STAND, viewangles, moveangles, WP_NONE, qfalse );
}
// END


/*
=================
PlayerSettings_DrawPlayer
=================
*/
static void PlayerSettings_DrawPlayer( void *self ) {
	menubitmap_s	*b;
// STONELANCE
/*
	vec3_t			viewangles;
	char			buf[MAX_QPATH];

	trap_Cvar_VariableStringBuffer( "model", buf, sizeof( buf ) );
	if ( strcmp( buf, s_playersettings.playerModel ) != 0 ) {
		UI_PlayerInfo_SetModel( &s_playersettings.playerinfo, buf );
		strcpy( s_playersettings.playerModel, buf );

		viewangles[YAW]   = 180 - 30;
		viewangles[PITCH] = 0;
		viewangles[ROLL]  = 0;
		UI_PlayerInfo_SetInfo( &s_playersettings.playerinfo, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
	}
*/
// END
	b = (menubitmap_s*) self;
	UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, &s_playersettings.playerinfo, uis.realtime );
}


// STONELANCE (new function)
static int PlayerSettings_FavoriteIndex( const char *favorite ) {
	if ( !favorite || Q_strncmp( favorite, "favoritecar", 11 ) ) {
		return -1;
	}

	return atoi( favorite + 11 ) - 1;
}

static qboolean PlayerSettings_TryProfileFavorite( const char *favorite, char *modelName, char *skinName, char *rimName, char *headName ) {
	const profile_info_t *profileInfo = UI_Profile_GetActiveInfo();
	int favoriteIndex;

	if ( !profileInfo ) {
		profileInfo = &s_playersettings.profileInfo;
	}

	favoriteIndex = PlayerSettings_FavoriteIndex( favorite );
	if ( !profileInfo || favoriteIndex < 0 || favoriteIndex >= PROFILE_MAX_FAVORITE_CARS ) {
		return qfalse;
	}

	if ( profileInfo->favoriteCars[favoriteIndex].model[0] && profileInfo->favoriteCars[favoriteIndex].skin[0] ) {
		if ( modelName ) {
			Q_strncpyz( modelName, profileInfo->favoriteCars[favoriteIndex].model, MAX_QPATH );
		}
		if ( skinName ) {
			Q_strncpyz( skinName, profileInfo->favoriteCars[favoriteIndex].skin, MAX_QPATH );
		}
		if ( rimName ) {
			Q_strncpyz( rimName, profileInfo->favoriteCars[favoriteIndex].rim, MAX_QPATH );
		}
		if ( headName ) {
			Q_strncpyz( headName, profileInfo->favoriteCars[favoriteIndex].head, MAX_QPATH );
		}

		return qtrue;
	}

	return qfalse;
}

static qboolean PlayerSettings_GetFavoriteValues( const char *favorite, char *modelName, char *skinName, char *rimName, char *headName ) {
	if ( PlayerSettings_TryProfileFavorite( favorite, modelName, skinName, rimName, headName ) ) {
		return qtrue; /* profile favorite found - success */
	}

	return GetValuesFromFavorite( favorite, modelName, skinName, rimName, headName );
}

// STONELANCE (new function)
/*
=================
LoadFavorite

=================
*/
static void LoadFavorite( const char *favorite ) {
	char		modelName[MAX_QPATH];
	char		skinName[MAX_QPATH];
	char		rimName[MAX_QPATH];
	char		headName[MAX_QPATH];
	int			i;
	qboolean	carFound;

PlayerSettings_GetFavoriteValues( favorite, modelName, skinName, rimName, headName );

	// find model in our list
	carFound = qfalse;
	for (i = 0; i < s_playersettings.allModels; i++)
	{
		if (!Q_stricmp( modelName, s_playersettings.modelList[i] ))
		{
			// found pic, set selection here
			s_playersettings.selectedModel = i;
			s_playersettings.modelname.string = s_playersettings.modelList[s_playersettings.selectedModel];
			carFound = qtrue;
			break;
		}
	}

	if (!carFound){
		s_playersettings.selectedModel = 0;

		// get model
		Com_sprintf(s_playersettings.modelskin, sizeof(s_playersettings.modelskin), "%s/%s", s_playersettings.modelList[s_playersettings.selectedModel], DEFAULT_SKIN);

		s_playersettings.modelname.string = s_playersettings.modelList[s_playersettings.selectedModel];

		// FIXME: check to see if these exist
		Q_strncpyz(s_playersettings.rimskin, DEFAULT_RIM, sizeof(s_playersettings.rimskin));
		Q_strncpyz(s_playersettings.headskin, DEFAULT_HEAD, sizeof(s_playersettings.headskin));

		s_playersettings.modelChanged = qtrue;
	}
	else {
		Com_sprintf(s_playersettings.modelskin, sizeof(s_playersettings.modelskin), "%s/%s", modelName, skinName);
		Q_strncpyz(s_playersettings.rimskin, rimName, sizeof(s_playersettings.rimskin));
		Q_strncpyz(s_playersettings.headskin, headName, sizeof(s_playersettings.headskin));

		trap_Cvar_Set( "model", s_playersettings.modelskin );
		trap_Cvar_Set( "rim", rimName );
		trap_Cvar_Set( "head", headName );

		s_playersettings.modelChanged = qtrue;
	}
}

/*
=================
PlayerSettings_UpdateFavorites

=================
*/
static void PlayerSettings_UpdateFavorites( void ) {
	int				i;
	char			buf[MAX_QPATH];
	char			modelName[MAX_QPATH];
	char			skinName[MAX_QPATH];
	char			rimName[MAX_QPATH];
	char			headName[MAX_QPATH];
	qboolean	error;
	const profile_info_t *info = UI_Profile_GetActiveInfo();
	const profile_info_t *profileInfo = info ? info : &s_playersettings.profileInfo;

	for ( i = 0; i < NUM_FAVORITES; i++ ) {
		const char *favoriteModel = NULL;
                const char *favoriteSkin = NULL;

if ( profileInfo && profileInfo->favoriteCars[i].model[0] && profileInfo->favoriteCars[i].skin[0] ) {
favoriteModel = profileInfo->favoriteCars[i].model;
favoriteSkin = profileInfo->favoriteCars[i].skin;
} else {
	Com_sprintf( buf, sizeof( buf ), "favoritecar%i", ( i + 1 ) );
error = PlayerSettings_GetFavoriteValues( buf, modelName, skinName, rimName, headName );
if ( !error ) {
favoriteModel = modelName;
favoriteSkin = skinName;
}
}

                if ( favoriteModel && favoriteSkin ) {
                        Com_sprintf( s_playersettings.favIcons[i], sizeof( s_playersettings.favIcons[i] ), "models/players/%s/icon_%s", favoriteModel, favoriteSkin );
                        s_playersettings.favpics[i].generic.name = s_playersettings.favIcons[i];
                        s_playersettings.favpicbuttons[i].generic.flags &= ~QMF_INACTIVE;
                } else {
                        s_playersettings.favpics[i].generic.name = NULL;
                        s_playersettings.favpicbuttons[i].generic.flags |= QMF_INACTIVE;
}

		s_playersettings.favpics[i].shader = 0;
	}
}



static void PlayerSettings_CopyFavoritesToProfile( profile_info_t *info ) {
	int i;

	if ( !info ) {
		return;
	}

	for ( i = 0; i < PROFILE_MAX_FAVORITE_CARS; ++i ) {
		char cvarName[16];
		char modelName[MAX_QPATH];
		char skinName[MAX_QPATH];
		char rimName[MAX_QPATH];
		char headName[MAX_QPATH];

	Com_sprintf( cvarName, sizeof( cvarName ), "favoritecar%d", i + 1 );
if ( !PlayerSettings_GetFavoriteValues( cvarName, modelName, skinName, rimName, headName ) ) {
			Q_strncpyz( info->favoriteCars[i].model, modelName, sizeof( info->favoriteCars[i].model ) );
			Q_strncpyz( info->favoriteCars[i].skin, skinName, sizeof( info->favoriteCars[i].skin ) );
			Q_strncpyz( info->favoriteCars[i].rim, rimName, sizeof( info->favoriteCars[i].rim ) );
			Q_strncpyz( info->favoriteCars[i].head, headName, sizeof( info->favoriteCars[i].head ) );
		} else {
			Com_Memset( &info->favoriteCars[i], 0, sizeof( info->favoriteCars[i] ) );
		}
	}
}

static void PlayerSettings_ApplyProfileFavorites( const profile_info_t *info ) {
	int i;

	if ( !info ) {
		return;
	}

	for ( i = 0; i < PROFILE_MAX_FAVORITE_CARS; ++i ) {
		char cvarName[16];
		char favoriteValue[MAX_QPATH * 4];

		Com_sprintf( cvarName, sizeof( cvarName ), "favoritecar%d", i + 1 );

		if ( info->favoriteCars[i].model[0] && info->favoriteCars[i].skin[0] ) {
			Com_sprintf( favoriteValue, sizeof( favoriteValue ), "%s/%s/%s/%s",
			             info->favoriteCars[i].model,
			             info->favoriteCars[i].skin,
			             info->favoriteCars[i].rim,
			             info->favoriteCars[i].head );
			trap_Cvar_Set( cvarName, favoriteValue );
		} else {
			trap_Cvar_Set( cvarName, "" );
		}
	}
}
/*
=================
PlayerSettings_Update
=================
*/
void PlayerSettings_Update( void ){
	trap_Cvar_VariableStringBuffer( "rim", s_playersettings.rimskin, sizeof( s_playersettings.rimskin ) );
	trap_Cvar_VariableStringBuffer( "head", s_playersettings.headskin, sizeof( s_playersettings.headskin ) );
	trap_Cvar_VariableStringBuffer( "model", s_playersettings.modelskin, sizeof( s_playersettings.modelskin ) );
	
	PlayerSettings_UpdateFavorites();
	PlayerSettings_UpdateModel();
}
// END


/*
=================
PlayerSettings_SaveChanges
=================
*/
static void PlayerSettings_SaveChanges( void ) {
	// name

// STONELANCE
	if (s_playersettings.modelChanged){
		trap_Cvar_Set( "model", s_playersettings.modelskin );
	}
// END

	if ( UI_Profile_HasActiveProfile() ) {
		profile_info_t info;
		int birthYear;
		int birthMonth;
		int birthDay;
		int maxDay;
		int countryIndex;
		const char *genderValue;
		const profile_info_t *activeInfo;

		activeInfo = UI_Profile_GetActiveInfo();
		if ( activeInfo ) {
			info = *activeInfo;
		} else {
			Com_Memset( &info, 0, sizeof( info ) );
		}

		genderValue = PlayerSettings_GetGenderValue( s_playersettings.gender.curvalue );
		Q_strncpyz( info.gender, genderValue, sizeof( info.gender ) );

		birthYear = PlayerSettings_GetBirthYearFromIndex( s_playersettings.birthYear.curvalue );
		birthMonth = s_playersettings.birthMonth.curvalue;
		birthDay = s_playersettings.birthDay.curvalue;
		if ( birthYear > 0 && birthMonth > 0 && birthDay > 0 ) {
			maxDay = PlayerSettings_GetDaysInMonth( birthYear, birthMonth );
			if ( birthDay > maxDay ) {
				birthDay = maxDay;
			}
			Com_sprintf( info.birthDate, sizeof( info.birthDate ), "%04d-%02d-%02d", birthYear, birthMonth, birthDay );
		}

		/* Avatar: the wizard stores the full preset shader path in info.avatar.
		 * Don't overwrite it here — preserve whatever is already in the
		 * loaded profile. The avatar field in this settings screen is
		 * read-only (managed by the wizard / profile editor). */
		countryIndex = s_playersettings.country.curvalue;
		if ( countryIndex < 0 || !country_codes[countryIndex] ) {
			countryIndex = 0;
		}
		Q_strncpyz( info.country, country_codes[countryIndex], sizeof( info.country ) );
                PlayerSettings_CopyFavoritesToProfile( &info );
                UI_Profile_SaveActiveInfo( &info );
        }

	// handicap
	trap_Cvar_SetValue( "handicap", 100 - s_playersettings.handicap.curvalue * 5 );

	// effects color
	trap_Cvar_SetValue( "color1", uitogamecode[s_playersettings.effects.curvalue] );
}


/*
=================
PlayerSettings_MenuKey
=================
*/
static sfxHandle_t PlayerSettings_MenuKey( int key ) {
	if( key == K_MOUSE2 || key == K_ESCAPE ) {
// STONELANCE
//		PlayerSettings_SaveChanges();
		s_playersettings.menu.transitionMenu = ID_BACK;
		uis.transitionOut = uis.realtime;
		return 0;
// END
	}
if ( s_playersettings.currentTab == TAB_STATS ) {
const playersettingsPaginationInfo_t *info;

info = PlayerSettings_UpdateStatsPaginationInfo();
if ( key == K_MOUSE1 ) {
if ( PlayerSettings_HandlePaginationClick( &s_playersettings.statsPagination, info, &s_playersettings.statsPrevPageButton, &s_playersettings.statsNextPageButton ) ) {
return menu_move_sound;
}
} else if ( PlayerSettings_HandlePaginationKey( &s_playersettings.statsPagination, info, key ) ) {
return menu_move_sound;
}
} else if ( s_playersettings.currentTab == TAB_ACHIEVEMENTS ) {
const playersettingsPaginationInfo_t *info;

info = PlayerSettings_UpdateAchievementsPaginationInfo();
if ( key == K_MOUSE1 ) {
if ( PlayerSettings_HandlePaginationClick( &s_playersettings.achievementsPagination, info, &s_playersettings.achievementsPrevPageButton, &s_playersettings.achievementsNextPageButton ) ) {
return menu_move_sound;
}
} else if ( PlayerSettings_HandlePaginationKey( &s_playersettings.achievementsPagination, info, key ) ) {
return menu_move_sound;
}
}
return Menu_DefaultKey( &s_playersettings.menu, key );
}


/*
=================
PlayerSettings_SetMenuItems
=================
*/
static void PlayerSettings_SetMenuItems( void ) {
//	vec3_t	viewangles;
	int		c;
	int		h;

// STONELANCE
	int			i;
	char		modelName[MAX_QPATH];
	char		*slash;
	qboolean	carFound;

	trap_Cvar_VariableStringBuffer( "rim", s_playersettings.rimskin, sizeof( s_playersettings.rimskin ) );
	trap_Cvar_VariableStringBuffer( "head", s_playersettings.headskin, sizeof( s_playersettings.headskin ) );
	trap_Cvar_VariableStringBuffer( "model", s_playersettings.modelskin, sizeof( s_playersettings.modelskin ) );
// END

        // name
        Q_strncpyz( s_playersettings.name.field.buffer,
                    UI_Cvar_VariableString( "name" ),
                    sizeof( s_playersettings.name.field.buffer ) );

        if ( UI_Profile_HasActiveProfile() ) {
                const profile_info_t *info = UI_Profile_GetActiveInfo();
                int parsedYear = 0;
                int parsedMonth = 0;
                int parsedDay = 0;

                if ( info ) {
                        s_playersettings.profileInfo = *info;

                        if ( s_playersettings.profileInfo.name[0] ) {
                                Q_strncpyz( s_playersettings.name.field.buffer,
                                            s_playersettings.profileInfo.name,
                                            sizeof( s_playersettings.name.field.buffer ) );
                        }
                } else {
                        Com_Memset( &s_playersettings.profileInfo, 0, sizeof( s_playersettings.profileInfo ) );
                }

		s_playersettings.gender.curvalue = PlayerSettings_FindGenderIndex( s_playersettings.profileInfo.gender );

		if ( PlayerSettings_ParseBirthDate( s_playersettings.profileInfo.birthDate, &parsedYear, &parsedMonth, &parsedDay ) ) {
			s_playersettings.birthYear.curvalue = PlayerSettings_GetBirthYearIndex( parsedYear );
			s_playersettings.birthMonth.curvalue = parsedMonth;
			s_playersettings.birthDay.curvalue = parsedDay;
		} else {
			s_playersettings.birthYear.curvalue = 0;
			s_playersettings.birthMonth.curvalue = 0;
			s_playersettings.birthDay.curvalue = 0;
		}
		PlayerSettings_UpdateBirthDateDayItems();

		s_playersettings.avatar.field.buffer[0] = '\0';
		Q_strncpyz( s_playersettings.avatarActionLine, "Open import dialog",
		            sizeof( s_playersettings.avatarActionLine ) );
		PlayerSettings_SetAvatarProfileName( UI_Profile_GetActiveName() );
		PlayerSettings_EnsureAvatarShader();
		s_playersettings.country.curvalue = PlayerSettings_FindCountryIndex( s_playersettings.profileInfo.country );

		s_playersettings.gender.generic.flags &= ~( QMF_GRAYED | QMF_INACTIVE );
		s_playersettings.birthDay.generic.flags &= ~( QMF_GRAYED | QMF_INACTIVE );
		s_playersettings.birthMonth.generic.flags &= ~( QMF_GRAYED | QMF_INACTIVE );
		s_playersettings.birthYear.generic.flags &= ~( QMF_GRAYED | QMF_INACTIVE );
		s_playersettings.avatar.generic.flags &= ~( QMF_GRAYED | QMF_INACTIVE );
		s_playersettings.country.generic.flags &= ~( QMF_GRAYED | QMF_INACTIVE );
		s_playersettings.birthDateLabel.color = uis.text_color;
	} else {
		Com_Memset( &s_playersettings.profileInfo, 0, sizeof( s_playersettings.profileInfo ) );
		s_playersettings.gender.curvalue = 0;
		s_playersettings.birthYear.curvalue = 0;
		s_playersettings.birthMonth.curvalue = 0;
		s_playersettings.birthDay.curvalue = 0;
		PlayerSettings_UpdateBirthDateDayItems();
		s_playersettings.avatar.field.buffer[0] = '\0';
		Q_strncpyz( s_playersettings.avatarActionLine, "Select a profile first",
		            sizeof( s_playersettings.avatarActionLine ) );
		PlayerSettings_SetAvatarProfileName( "" );
		PlayerSettings_EnsureAvatarShader();
		s_playersettings.country.curvalue = 0;

		s_playersettings.gender.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
		s_playersettings.birthDay.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
		s_playersettings.birthMonth.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
		s_playersettings.birthYear.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
                s_playersettings.avatar.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
                s_playersettings.country.generic.flags |= QMF_GRAYED | QMF_INACTIVE;
                s_playersettings.birthDateLabel.color = text_color_disabled;
        }

        PlayerSettings_ApplyProfileFavorites( &s_playersettings.profileInfo );


        // effects color
        c = trap_Cvar_VariableValue( "color1" ) - 1;
	if( c < 0 || c > 6 ) {
		c = 6;
	}
	s_playersettings.effects.curvalue = gamecodetoui[c];

	// model/skin
	memset( &s_playersettings.playerinfo, 0, sizeof(playerInfo_t) );
/*
	viewangles[YAW]   = 180 - 30;
	viewangles[PITCH] = 0;
	viewangles[ROLL]  = 0;
*/
// STONELANCE
	Q_strncpyz( modelName, s_playersettings.modelskin, sizeof( modelName ) );
	slash = strchr( modelName, '/' );
	if ( slash ) {
		*slash = 0;
	}

	s_playersettings.modelChanged = qfalse;

	// find model in our list
	carFound = qfalse;
	for (i = 0; i < s_playersettings.allModels; i++)
	{
		if (!Q_stricmp( modelName, s_playersettings.modelList[i] )){
			// found pic, set selection here
			s_playersettings.selectedModel = i;
			s_playersettings.modelname.string = s_playersettings.modelList[s_playersettings.selectedModel];
			carFound = qtrue;
			break;
		}
	}

	if (!carFound){
		s_playersettings.selectedModel = 0;

		// get model
		Com_sprintf( s_playersettings.modelskin, sizeof(s_playersettings.modelskin), "%s/%s", s_playersettings.modelList[s_playersettings.selectedModel], DEFAULT_SKIN);
		s_playersettings.modelname.string = s_playersettings.modelList[s_playersettings.selectedModel];
		s_playersettings.modelChanged = qtrue;
	}

/*
	UI_PlayerInfo_SetModel( &s_playersettings.playerinfo, UI_Cvar_VariableString( "model" ) );
	UI_PlayerInfo_SetInfo( &s_playersettings.playerinfo, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
*/

	PlayerSettings_UpdateModel();
	PlayerSettings_UpdateFavorites();
// END

	// handicap
	h = Com_Clamp( 5, 100, trap_Cvar_VariableValue("handicap") );
	s_playersettings.handicap.curvalue = 20 - h / 5;
}


// STONELANCE
/*
=================
PlayerSettings_PicEvent
=================
*/
static void PlayerSettings_PicEvent( void* ptr, int event )
{
	if (event != QM_ACTIVATED)
		return;

	switch(((menucommon_s*)ptr)->id){
	case ID_FAVORITE1:
		LoadFavorite("favoritecar1");
		break;

	case ID_FAVORITE2:
		LoadFavorite("favoritecar2");
		break;

	case ID_FAVORITE3:
		LoadFavorite("favoritecar3");
		break;

	case ID_FAVORITE4:
		LoadFavorite("favoritecar4");
		break;
	}

	PlayerSettings_UpdateModel();
}
// END


static qboolean PlayerSettings_ImportAvatarFromPath( const char *sourcePath ) {
	char shaderPath[MAX_QPATH];
	const profile_info_t *activeInfo;
	profile_info_t info;

	if ( !UI_Profile_HasActiveProfile() || !sourcePath || !sourcePath[0] ) {
		return qfalse;
	}

	if ( !trap_UI_ImportAvatarPath( UI_Profile_GetActiveName(), sourcePath,
								shaderPath, sizeof( shaderPath ) ) ) {
		return qfalse;
	}

	activeInfo = UI_Profile_GetActiveInfo();
	if ( !activeInfo ) {
		return qfalse;
	}

	info = *activeInfo;
	Q_strncpyz( info.avatar, shaderPath, sizeof( info.avatar ) );
	if ( !UI_Profile_SaveActiveInfo( &info ) ) {
		return qfalse;
	}

	s_playersettings.profileInfo = info;
	s_playersettings.avatarShader = 0;
	s_playersettings.avatarShaderInitialized = qfalse;
	s_playersettings.avatarShaderName[0] = '\0';
	Q_strncpyz( s_playersettings.avatarActionLine, "Custom image ready",
	            sizeof( s_playersettings.avatarActionLine ) );
	PlayerSettings_EnsureAvatarShader();
	return qtrue;
}

static void PlayerSettings_DrawAvatarImportField( void *self ) {
	menufield_s *field;
	qboolean focus;
	vec4_t textColor;
	vec4_t placeholderColor;
	char visible[96];
	char prefix[96];
	int start;
	int count;
	int cursor;
	int i;
	int fieldX;
	int fieldY;
	int fieldW;

	field = (menufield_s *)self;
	focus = ( Menu_ItemAtCursor( field->generic.parent ) == &field->generic );
	fieldX = field->generic.x;
	fieldY = field->generic.y;
	fieldW = field->generic.right - field->generic.left;

	Vector4Copy( focus ? playerSettingsAccentColor : playerSettingsTextColor, textColor );
	Vector4Copy( playerSettingsMutedColor, placeholderColor );
	placeholderColor[3] = 0.75f;

	Frontend_DrawText( fieldX, fieldY - 22, "Image path",
	                   UI_LEFT | UI_SMALLFONT, playerSettingsMutedColor );
	Frontend_DrawPanel( fieldX - 8, fieldY, fieldW, 30, 1.0f,
	                    focus ? UI_FRONTEND_STYLE_ACTIVE : UI_FRONTEND_STYLE_CARD );

	start = field->field.scroll;
	if ( start < 0 ) {
		start = 0;
	}
	cursor = field->field.cursor;
	if ( cursor < start ) {
		start = cursor;
	}

	visible[0] = '\0';
	count = 0;
	while ( field->field.buffer[start + count] && count < (int)sizeof( visible ) - 1 ) {
		visible[count] = field->field.buffer[start + count];
		visible[count + 1] = '\0';
		if ( Frontend_TextWidth( visible, UI_LEFT | UI_SMALLFONT ) > fieldW - 20 ) {
			visible[count] = '\0';
			break;
		}
		count++;
	}

	if ( !visible[0] ) {
		Frontend_DrawText( fieldX, fieldY + 7,
		                   "D:/Pictures/avatar.png",
		                   UI_LEFT | UI_SMALLFONT, placeholderColor );
	} else {
		Frontend_DrawText( fieldX, fieldY + 7, visible,
		                   UI_LEFT | UI_SMALLFONT, textColor );
	}

	if ( focus && ( ( uis.realtime / 400 ) & 1 ) ) {
		prefix[0] = '\0';
		for ( i = start; i < cursor && i < (int)sizeof( prefix ) - 1; ++i ) {
			prefix[i - start] = field->field.buffer[i];
			prefix[i - start + 1] = '\0';
		}
		UI_FillRect( fieldX + Frontend_TextWidth( prefix, UI_LEFT | UI_SMALLFONT ),
		             fieldY + 6, 1, 13, playerSettingsAccentColor );
	}
}

static void PlayerSettings_DrawAvatarImportButton( void *self ) {
	menutext_s *button;
	qboolean focus;

	button = (menutext_s *)self;
	focus = ( Menu_ItemAtCursor( button->generic.parent ) == &button->generic );
	Frontend_DrawButton( button->generic.left, button->generic.top,
	                     button->generic.right - button->generic.left,
	                     button->generic.bottom - button->generic.top,
	                     button->string, 1.0f, focus, UI_CENTER );
}

static void PlayerSettings_AvatarImportDraw( void ) {
	Frontend_DrawBackground( playerSettingsScrimColor );
	Frontend_DrawPanel( 92, 84, 456, 312, 1.0f, UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( 120, 112, "Import avatar", UI_LEFT | UI_BIGFONT,
	                   playerSettingsTextColor );
	Frontend_DrawStatusChip( 462, 114, "Profile", playerSettingsAccentColor, 1.0f );
	Frontend_DrawText( 120, 142,
	                   "Add a custom image to the active driver profile",
	                   UI_LEFT | UI_SMALLFONT, playerSettingsMutedColor );

	Frontend_DrawCard( 120, 174, 400, 180, 1.0f, qfalse );
	Frontend_DrawText( 136, 198,
	                   "PNG, JPG or TGA up to 8 MB",
	                   UI_LEFT | UI_SMALLFONT, playerSettingsMutedColor );
	Frontend_DrawText( 136, 224,
	                   "Type the full path to the image on your computer.",
	                   UI_LEFT | UI_SMALLFONT, playerSettingsMutedColor );

	Menu_Draw( &s_playersettings.avatarImportMenu );

	if ( s_playersettings.avatarImportStatus[0] ) {
		Frontend_DrawText( 120, 366, s_playersettings.avatarImportStatus,
		                   UI_LEFT | UI_SMALLFONT, avatarImageMissingColor );
	}
	Frontend_DrawText( 120, 382,
	                   "Enter import     Tab switch     Esc back",
	                   UI_LEFT | UI_SMALLFONT, playerSettingsMutedColor );
}

static void PlayerSettings_AvatarImportMenuEvent( void *ptr, int event ) {
	menucommon_s *item;

	if ( event != QM_ACTIVATED ) {
		return;
	}

	item = (menucommon_s *)ptr;
	switch ( item->id ) {
	case ID_AVATAR_IMPORT_PATH:
	case ID_AVATAR_IMPORT_CONFIRM:
		if ( !s_playersettings.avatarImportPath.field.buffer[0] ) {
			Q_strncpyz( s_playersettings.avatarImportStatus,
			            "Enter an image path first",
			            sizeof( s_playersettings.avatarImportStatus ) );
			return;
		}
		if ( PlayerSettings_ImportAvatarFromPath( s_playersettings.avatarImportPath.field.buffer ) ) {
			UI_PopMenu();
			return;
		}
		Q_strncpyz( s_playersettings.avatarImportStatus,
		            "Import failed — check the path and format",
		            sizeof( s_playersettings.avatarImportStatus ) );
		break;

	case ID_AVATAR_IMPORT_CANCEL:
		UI_PopMenu();
		break;
	}
}

static sfxHandle_t PlayerSettings_AvatarImportKey( int key ) {
	if ( key == K_ESCAPE || key == K_MOUSE2 || key == K_PAD0_B ) {
		UI_PopMenu();
		return menu_out_sound;
	}

	return Menu_DefaultKey( &s_playersettings.avatarImportMenu, key );
}

static void PlayerSettings_OpenAvatarImport( void ) {
	menuframework_s *menu;

	menu = &s_playersettings.avatarImportMenu;
	Com_Memset( menu, 0, sizeof( *menu ) );
	Com_Memset( &s_playersettings.avatarImportPath, 0,
	            sizeof( s_playersettings.avatarImportPath ) );
	Com_Memset( &s_playersettings.avatarImportConfirm, 0,
	            sizeof( s_playersettings.avatarImportConfirm ) );
	Com_Memset( &s_playersettings.avatarImportCancel, 0,
	            sizeof( s_playersettings.avatarImportCancel ) );
	s_playersettings.avatarImportStatus[0] = '\0';

	menu->fullscreen = qtrue;
	menu->wrapAround = qfalse;
	menu->draw = PlayerSettings_AvatarImportDraw;
	menu->key = PlayerSettings_AvatarImportKey;

	s_playersettings.avatarImportPath.generic.type = MTYPE_FIELD;
	s_playersettings.avatarImportPath.generic.flags = QMF_SMALLFONT | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
	s_playersettings.avatarImportPath.generic.id = ID_AVATAR_IMPORT_PATH;
	s_playersettings.avatarImportPath.generic.x = 144;
	s_playersettings.avatarImportPath.generic.y = 260;
	s_playersettings.avatarImportPath.generic.callback = PlayerSettings_AvatarImportMenuEvent;
	s_playersettings.avatarImportPath.generic.ownerdraw = PlayerSettings_DrawAvatarImportField;
	s_playersettings.avatarImportPath.field.widthInChars = 48;
	s_playersettings.avatarImportPath.field.maxchars = MAX_EDIT_LINE - 1;
	MenuField_Init( &s_playersettings.avatarImportPath );
	s_playersettings.avatarImportPath.generic.left = 144;
	s_playersettings.avatarImportPath.generic.top = 260;
	s_playersettings.avatarImportPath.generic.right = 496;
	s_playersettings.avatarImportPath.generic.bottom = 290;

	s_playersettings.avatarImportConfirm.generic.type = MTYPE_PTEXT;
	s_playersettings.avatarImportConfirm.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
	s_playersettings.avatarImportConfirm.generic.id = ID_AVATAR_IMPORT_CONFIRM;
	s_playersettings.avatarImportConfirm.generic.callback = PlayerSettings_AvatarImportMenuEvent;
	s_playersettings.avatarImportConfirm.generic.ownerdraw = PlayerSettings_DrawAvatarImportButton;
	s_playersettings.avatarImportConfirm.generic.left = 144;
	s_playersettings.avatarImportConfirm.generic.top = 304;
	s_playersettings.avatarImportConfirm.generic.right = 312;
	s_playersettings.avatarImportConfirm.generic.bottom = 332;
	s_playersettings.avatarImportConfirm.string = "Import";

	s_playersettings.avatarImportCancel.generic.type = MTYPE_PTEXT;
	s_playersettings.avatarImportCancel.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
	s_playersettings.avatarImportCancel.generic.id = ID_AVATAR_IMPORT_CANCEL;
	s_playersettings.avatarImportCancel.generic.callback = PlayerSettings_AvatarImportMenuEvent;
	s_playersettings.avatarImportCancel.generic.ownerdraw = PlayerSettings_DrawAvatarImportButton;
	s_playersettings.avatarImportCancel.generic.left = 328;
	s_playersettings.avatarImportCancel.generic.top = 304;
	s_playersettings.avatarImportCancel.generic.right = 496;
	s_playersettings.avatarImportCancel.generic.bottom = 332;
	s_playersettings.avatarImportCancel.string = "Cancel";

	Menu_AddItem( menu, &s_playersettings.avatarImportPath );
	Menu_AddItem( menu, &s_playersettings.avatarImportConfirm );
	Menu_AddItem( menu, &s_playersettings.avatarImportCancel );
	Menu_SetCursorToItem( menu, &s_playersettings.avatarImportPath );

	/* The avatar dialog is a lightweight menu overlay. Clear any transition
	 * timer left by the profile screen so the field is immediately editable. */
	uis.transitionIn = 0;
	uis.transitionOut = 0;
	UI_PushMenu( menu );
}


/*
=================
PlayerSettings_MenuEvent
=================
*/
static void PlayerSettings_MenuEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_TAB_PROFILE:
		PlayerSettings_SetTab( TAB_PROFILE );
		break;

	case ID_TAB_VEHICLE:
		PlayerSettings_SetTab( TAB_VEHICLE );
		break;

	case ID_TAB_STATS:
		PlayerSettings_SetTab( TAB_STATS );
		break;

	case ID_TAB_ACHIEVEMENTS:
		PlayerSettings_SetTab( TAB_ACHIEVEMENTS );
		break;

	case ID_COUNTRY:
		/* Country is a normal spin control; the changed value is persisted
		 * together with the other profile fields when leaving this screen. */
		break;

	case ID_AVATAR:
		PlayerSettings_OpenAvatarImport();
		break;

	case ID_HANDICAP:
		trap_Cvar_Set( "handicap", va( "%i", 100 - 5 * s_playersettings.handicap.curvalue ) );
		break;

// STONELANCE
/*
	case ID_MODEL:
		PlayerSettings_SaveChanges();
		UI_PlayerModelMenu();
		break;

	case ID_BACK:
		PlayerSettings_SaveChanges();
		UI_PopMenu();
		break;
*/

	case ID_CUSTOMIZE:
	case ID_BACK:
		s_playersettings.menu.transitionMenu = ((menucommon_s*)ptr)->id;
		uis.transitionOut = uis.realtime;
		break;

	case ID_PLATE:
		UI_PlateSelectionMenu();
		break;

	case ID_LEFT:
		//Com_Printf("Clicked car selection LEFT\n");
		if (s_playersettings.selectedModel > 0)
		{
			s_playersettings.selectedModel--;

			//Com_Printf("PS: Car selected, %i\n", s_playersettings.selectedmodel);

			// get model
			Com_sprintf(s_playersettings.modelskin, sizeof(s_playersettings.modelskin), "%s/%s", s_playersettings.modelList[s_playersettings.selectedModel], DEFAULT_SKIN);

			//Com_Printf("PS: modelskin set to: %s\n", s_playersettings.modelskin);

			s_playersettings.modelname.string = s_playersettings.modelList[s_playersettings.selectedModel];

			//Com_Printf("PS: modelname set to: %s\n", s_playersettings.modelname.string);

			s_playersettings.modelChanged = qtrue;

			PlayerSettings_UpdateModel();
		}
		break;

	case ID_RIGHT:
		//Com_Printf("Clicked car selection RIGHT\n");
		if (s_playersettings.selectedModel < s_playersettings.numModels - 1 )
		{
			s_playersettings.selectedModel++;

			//Com_Printf("PS: Car selected, %i\n", s_playersettings.selectedmodel);

			// get model
			Com_sprintf(s_playersettings.modelskin, sizeof(s_playersettings.modelskin), "%s/%s", s_playersettings.modelList[s_playersettings.selectedModel], DEFAULT_SKIN);

			//Com_Printf("PS: modelskin set to: %s\n", s_playersettings.modelskin);

			s_playersettings.modelname.string = s_playersettings.modelList[s_playersettings.selectedModel];

			//Com_Printf("PS: modelname set to: %s\n", s_playersettings.modelname.string);

			s_playersettings.modelChanged = qtrue;

			PlayerSettings_UpdateModel();
		}
		break;
// END
	}
}


// STONELANCE
/*
=================
PlayerSettings_ChangeMenu
=================
*/
void PlayerSettings_ChangeMenu( int menuID ){

	switch(menuID){
	case ID_CUSTOMIZE:
		PlayerSettings_SaveChanges();
		s_playersettings.modelChanged = qfalse;
		UI_PlayerModelMenu( s_playersettings.modelname.string );
		break;

	case ID_BACK:
		PlayerSettings_SaveChanges();
		s_playersettings.modelChanged = qfalse;
//		uis.transitionIn = uis.realtime;
		UI_PopMenu();
		break;
	}
}


/*
=================
PlayerSettings_RunTransition
=================
*/
void PlayerSettings_RunTransition(float frac){
	uis.text_color[0] = text_color_normal[0];
	uis.text_color[1] = text_color_normal[1];
	uis.text_color[2] = text_color_normal[2];
	uis.text_color[3] = text_color_normal[3] * frac;

	s_playersettings.banner.color = uis.text_color;

	s_playersettings.customize.color = uis.text_color;
	s_playersettings.favorites.color = uis.text_color;
	s_playersettings.modelname.color = uis.text_color;
	s_playersettings.plate.color = uis.text_color;
}


/*
=================
PlayerSettings_BuildList
=================
*/
static void PlayerSettings_BuildList( void ){
	// get car list
	s_playersettings.numModels = UI_BuildFileList("models/players", "md3", "body", qtrue, qtrue, BL_EXCLUDE, 0, s_playersettings.modelList);
	s_playersettings.allModels = UI_BuildFileList("models/players", "md3", "body", qtrue, qtrue, BL_ONLY, s_playersettings.numModels, s_playersettings.modelList);
}
// END


/*
=================
PlayerSettings_MenuInit
=================
*/
static void PlayerSettings_MenuInit( void ) {
	int		y;
	int		profileY;
// STONELANCE
	int		i, j, x;
	static char	modelname[32];
// END

	memset(&s_playersettings,0,sizeof(playersettings_t));

	PlayerSettings_Cache();
	PlayerSettings_InitBirthDateLists();

	s_playersettings.menu.key        = PlayerSettings_MenuKey;
	s_playersettings.menu.wrapAround = qtrue;
	s_playersettings.menu.fullscreen = qtrue;
// STONELANCE
	s_playersettings.menu.draw		 = PlayerSettings_DrawBackShaders;
	s_playersettings.menu.transition = PlayerSettings_RunTransition;
	s_playersettings.menu.changeMenu = PlayerSettings_ChangeMenu;
// END

	s_playersettings.banner.generic.type  = MTYPE_BTEXT;
	s_playersettings.banner.generic.x     = 320;
// STONELANCE
	s_playersettings.banner.generic.y     = 17;
// END
	s_playersettings.banner.string        = "PLAYER SETTINGS";
// STONELANCE
	s_playersettings.banner.color         = text_color_normal;
// END
	s_playersettings.banner.style         = UI_CENTER;
	s_playersettings.banner.generic.flags = QMF_INACTIVE | QMF_HIDDEN;

	s_playersettings.tabProfile.generic.type = MTYPE_PTEXT;
	s_playersettings.tabProfile.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
	s_playersettings.tabProfile.generic.id = ID_TAB_PROFILE;
	s_playersettings.tabProfile.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.tabProfile.generic.ownerdraw = PlayerSettings_DrawTabItem;
	PlayerSettings_ConfigureTab( &s_playersettings.tabProfile, TAB_PROFILE );
	s_playersettings.tabProfile.string = "Profile";
	s_playersettings.tabProfile.style = UI_CENTER | UI_SMALLFONT;
	s_playersettings.tabProfile.color = uis.text_color;

	s_playersettings.tabVehicle.generic.type = MTYPE_PTEXT;
	s_playersettings.tabVehicle.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
	s_playersettings.tabVehicle.generic.id = ID_TAB_VEHICLE;
	s_playersettings.tabVehicle.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.tabVehicle.generic.ownerdraw = PlayerSettings_DrawTabItem;
	PlayerSettings_ConfigureTab( &s_playersettings.tabVehicle, TAB_VEHICLE );
	s_playersettings.tabVehicle.string = "Vehicle";
	s_playersettings.tabVehicle.style = UI_CENTER | UI_SMALLFONT;
	s_playersettings.tabVehicle.color = uis.text_color;

	s_playersettings.tabStats.generic.type = MTYPE_PTEXT;
	s_playersettings.tabStats.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
	s_playersettings.tabStats.generic.id = ID_TAB_STATS;
	s_playersettings.tabStats.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.tabStats.generic.ownerdraw = PlayerSettings_DrawTabItem;
	PlayerSettings_ConfigureTab( &s_playersettings.tabStats, TAB_STATS );
	s_playersettings.tabStats.string = "Stats";
	s_playersettings.tabStats.style = UI_CENTER | UI_SMALLFONT;
	s_playersettings.tabStats.color = uis.text_color;

	s_playersettings.tabAchievements.generic.type = MTYPE_PTEXT;
	s_playersettings.tabAchievements.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
	s_playersettings.tabAchievements.generic.id = ID_TAB_ACHIEVEMENTS;
	s_playersettings.tabAchievements.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.tabAchievements.generic.ownerdraw = PlayerSettings_DrawTabItem;
	PlayerSettings_ConfigureTab( &s_playersettings.tabAchievements, TAB_ACHIEVEMENTS );
	s_playersettings.tabAchievements.string = "Achievements";
	s_playersettings.tabAchievements.style = UI_CENTER | UI_SMALLFONT;
	s_playersettings.tabAchievements.color = uis.text_color;

// STONELANCE
/*
	s_playersettings.framel.generic.type  = MTYPE_BITMAP;
	s_playersettings.framel.generic.name  = ART_FRAMEL;
	s_playersettings.framel.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_playersettings.framel.generic.x     = 0;
	s_playersettings.framel.generic.y     = 78;
	s_playersettings.framel.width         = 256;
	s_playersettings.framel.height        = 329;

	s_playersettings.framer.generic.type  = MTYPE_BITMAP;
	s_playersettings.framer.generic.name  = ART_FRAMER;
	s_playersettings.framer.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_playersettings.framer.generic.x     = 376;
	s_playersettings.framer.generic.y     = 76;
	s_playersettings.framer.width         = 256;
	s_playersettings.framer.height        = 334;
*/

//	y = 144;
	y = PLAYERSETTINGS_CONTENT_TOP;
	profileY = PLAYERSETTINGS_PROFILE_PANEL_TOP + PLAYERSETTINGS_PROFILE_PANEL_INNER_MARGIN + 30;
// END
	s_playersettings.name.generic.type			= MTYPE_FIELD;
	s_playersettings.name.generic.flags			= QMF_NODEFAULTINIT;
	s_playersettings.name.generic.ownerdraw		= PlayerSettings_DrawModernField;
	s_playersettings.name.generic.name			= "Name";
	s_playersettings.name.field.widthInChars	= MAX_NAMELENGTH;
	s_playersettings.name.field.maxchars		= MAX_NAMELENGTH;
			s_playersettings.name.generic.x				= PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.name.generic.y				= profileY;
	s_playersettings.name.generic.left			= PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.name.generic.top			= profileY;
	s_playersettings.name.generic.right		= PLAYERSETTINGS_PROFILE_FORM_RIGHT;
	s_playersettings.name.generic.bottom		= profileY + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;
	s_playersettings.name.generic.flags |= QMF_INACTIVE | QMF_GRAYED;

	profileY += PLAYERSETTINGS_PROFILE_ROW_HEIGHT;

	s_playersettings.gender.generic.type = MTYPE_SPINCONTROL;
	s_playersettings.gender.generic.flags = QMF_NODEFAULTINIT;
	s_playersettings.gender.generic.ownerdraw = PlayerSettings_DrawModernChoice;
	s_playersettings.gender.generic.id = ID_GENDER;
	s_playersettings.gender.generic.name = "Gender";
	s_playersettings.gender.generic.x = PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.gender.generic.y = profileY;
	s_playersettings.gender.generic.left = PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.gender.generic.top = profileY;
	s_playersettings.gender.generic.right = PLAYERSETTINGS_PROFILE_FORM_RIGHT;
	s_playersettings.gender.generic.bottom = profileY + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;
	s_playersettings.gender.itemnames = (const char **)s_genderItems;
	s_playersettings.gender.numitems = 0;
	while ( s_genderItems[s_playersettings.gender.numitems] ) {
		s_playersettings.gender.numitems++;
	}

	profileY += PLAYERSETTINGS_PROFILE_ROW_HEIGHT;

	s_playersettings.birthDateLabel.generic.type = MTYPE_TEXT;
	s_playersettings.birthDateLabel.generic.flags = QMF_INACTIVE | QMF_NODEFAULTINIT | QMF_HIDDEN;
	s_playersettings.birthDateLabel.generic.x = PLAYERSETTINGS_PROFILE_FIELD_LEFT + PLAYERSETTINGS_PROFILE_LABEL_OFFSET;
	s_playersettings.birthDateLabel.generic.y = profileY;
	s_playersettings.birthDateLabel.generic.left = PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.birthDateLabel.generic.top = profileY;
	s_playersettings.birthDateLabel.generic.right = PLAYERSETTINGS_PROFILE_FORM_RIGHT;
	s_playersettings.birthDateLabel.generic.bottom = profileY + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;
	s_playersettings.birthDateLabel.string = "Birth date";
	s_playersettings.birthDateLabel.style = UI_LEFT | UI_SMALLFONT;
	s_playersettings.birthDateLabel.color = uis.text_color;

	{
		const int birthDayWidth = 70;
		const int birthMonthWidth = 110;
		const int birthYearWidth = 76;
		const int birthColumnGap = 10;
		const int birthDayX = PLAYERSETTINGS_PROFILE_FIELD_LEFT;
		const int birthMonthX = birthDayX + birthDayWidth + birthColumnGap;
		const int birthYearX = birthMonthX + birthMonthWidth + birthColumnGap;
		const int birthRowY = profileY + PLAYERSETTINGS_PROFILE_VALUE_BASELINE;

		s_playersettings.birthDay.generic.type = MTYPE_SPINCONTROL;
		s_playersettings.birthDay.generic.flags = QMF_NODEFAULTINIT;
		s_playersettings.birthDay.generic.ownerdraw = PlayerSettings_DrawModernBirthDate;
		s_playersettings.birthDay.generic.id = ID_BIRTH_DAY;
		s_playersettings.birthDay.generic.name = "Day";
		s_playersettings.birthDay.generic.x = birthDayX;
		s_playersettings.birthDay.generic.y = birthRowY;
		s_playersettings.birthDay.generic.left = birthDayX;
		s_playersettings.birthDay.generic.top = profileY;
		s_playersettings.birthDay.generic.right = birthDayX + birthDayWidth;
		s_playersettings.birthDay.generic.bottom = profileY + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;
		s_playersettings.birthDay.generic.callback = PlayerSettings_BirthDateChanged;
		s_playersettings.birthDay.itemnames = s_birthDayItems;
		s_playersettings.birthDay.numitems = BIRTH_DAY_MAX + 1;

		s_playersettings.birthMonth.generic.type = MTYPE_SPINCONTROL;
		s_playersettings.birthMonth.generic.flags = QMF_NODEFAULTINIT;
		s_playersettings.birthMonth.generic.ownerdraw = PlayerSettings_DrawModernBirthDate;
		s_playersettings.birthMonth.generic.id = ID_BIRTH_MONTH;
		s_playersettings.birthMonth.generic.name = "Month";
		s_playersettings.birthMonth.generic.x = birthMonthX;
		s_playersettings.birthMonth.generic.y = birthRowY;
		s_playersettings.birthMonth.generic.left = birthMonthX;
		s_playersettings.birthMonth.generic.top = profileY;
		s_playersettings.birthMonth.generic.right = birthMonthX + birthMonthWidth;
		s_playersettings.birthMonth.generic.bottom = profileY + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;
		s_playersettings.birthMonth.generic.callback = PlayerSettings_BirthDateChanged;
		s_playersettings.birthMonth.itemnames = (const char **)s_birthMonthItems;
		s_playersettings.birthMonth.numitems = 0;
		for ( s_playersettings.birthMonth.numitems = 0; s_birthMonthItems[s_playersettings.birthMonth.numitems]; ++s_playersettings.birthMonth.numitems ) {
		}

		s_playersettings.birthYear.generic.type = MTYPE_SPINCONTROL;
		s_playersettings.birthYear.generic.flags = QMF_NODEFAULTINIT;
		s_playersettings.birthYear.generic.ownerdraw = PlayerSettings_DrawModernBirthDate;
		s_playersettings.birthYear.generic.id = ID_BIRTH_YEAR;
		s_playersettings.birthYear.generic.name = "Year";
		s_playersettings.birthYear.generic.x = birthYearX;
		s_playersettings.birthYear.generic.y = birthRowY;
		s_playersettings.birthYear.generic.left = birthYearX;
		s_playersettings.birthYear.generic.top = profileY;
		s_playersettings.birthYear.generic.right = birthYearX + birthYearWidth;
		s_playersettings.birthYear.generic.bottom = profileY + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;
		s_playersettings.birthYear.generic.callback = PlayerSettings_BirthDateChanged;
		s_playersettings.birthYear.itemnames = s_birthYearItems;
		s_playersettings.birthYear.numitems = BIRTH_YEAR_COUNT + 1;
	}

	profileY += PLAYERSETTINGS_PROFILE_ROW_HEIGHT;

	s_playersettings.avatar.generic.type = MTYPE_PTEXT;
	s_playersettings.avatar.generic.flags = QMF_NODEFAULTINIT | QMF_PULSEIFFOCUS;
	s_playersettings.avatar.generic.id = ID_AVATAR;
	s_playersettings.avatar.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.avatar.generic.ownerdraw = PlayerSettings_DrawAvatarImage;
	s_playersettings.avatar.generic.name = "Avatar";
	s_playersettings.avatar.field.widthInChars = PROFILE_MAX_AVATAR - 1;
	s_playersettings.avatar.field.maxchars = PROFILE_MAX_AVATAR - 1;
	s_playersettings.avatar.generic.x = PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.avatar.generic.y = profileY;
	s_playersettings.avatar.generic.left = PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.avatar.generic.top = profileY;
	s_playersettings.avatar.generic.right = PLAYERSETTINGS_PROFILE_ROW_RIGHT;
	s_playersettings.avatar.generic.bottom = profileY + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;

	profileY += PLAYERSETTINGS_PROFILE_ROW_HEIGHT;

	s_playersettings.country.generic.type = MTYPE_SPINCONTROL;
	s_playersettings.country.generic.flags = QMF_NODEFAULTINIT | QMF_PULSEIFFOCUS;
	s_playersettings.country.generic.id = ID_COUNTRY;
	s_playersettings.country.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.country.generic.ownerdraw = PlayerSettings_DrawModernChoice;
	s_playersettings.country.generic.name = "Country";
		s_playersettings.country.itemnames = (const char **)country_names;
		s_playersettings.country.numitems = ARRAY_LEN( country_names ) - 1;
	s_playersettings.country.generic.x = PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.country.generic.y = profileY;
	s_playersettings.country.generic.left = PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.country.generic.top = profileY;
	s_playersettings.country.generic.right = PLAYERSETTINGS_PROFILE_FORM_RIGHT;
	s_playersettings.country.generic.bottom = profileY + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;

	profileY += PLAYERSETTINGS_PROFILE_ROW_HEIGHT;

//	 y += 3 * PROP_HEIGHT;
// END
	s_playersettings.handicap.generic.type			= MTYPE_SPINCONTROL;
	s_playersettings.handicap.generic.flags		= QMF_NODEFAULTINIT | QMF_PULSEIFFOCUS;
	s_playersettings.handicap.generic.id			= ID_HANDICAP;
	s_playersettings.handicap.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.handicap.generic.ownerdraw	= PlayerSettings_DrawModernChoice;
	s_playersettings.handicap.generic.name		= "Handicap";
// STONELANCE
/*
	s_playersettings.handicap.generic.x			= 192;
	s_playersettings.handicap.generic.y			= y;
	s_playersettings.handicap.generic.left		= 192 - 8;
	s_playersettings.handicap.generic.top		= y - 8;
	s_playersettings.handicap.generic.right		= 192 + 200;
	s_playersettings.handicap.generic.bottom	= y + 2 * PROP_HEIGHT;
*/
	s_playersettings.handicap.generic.x			= PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.handicap.generic.y			= profileY;
	s_playersettings.handicap.generic.left		= PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.handicap.generic.top		= profileY;
	s_playersettings.handicap.generic.right		= PLAYERSETTINGS_PROFILE_FORM_RIGHT;
	s_playersettings.handicap.generic.bottom	= profileY + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;
// END
	s_playersettings.handicap.itemnames			= (const char **)handicap_items;
	s_playersettings.handicap.numitems			= ARRAY_LEN( handicap_items ) - 1;

	profileY += PLAYERSETTINGS_PROFILE_ROW_HEIGHT;

// STONELANCE
//	 y += 3 * PROP_HEIGHT;
// END
	s_playersettings.effects.generic.type			= MTYPE_SPINCONTROL;
	s_playersettings.effects.generic.flags		= QMF_NODEFAULTINIT;
	s_playersettings.effects.generic.id			= ID_EFFECTS;
	s_playersettings.effects.generic.ownerdraw	= PlayerSettings_DrawModernEffects;
// STONELANCE
/*
	s_playersettings.effects.generic.x			= 192;
	s_playersettings.effects.generic.y			= y;
	s_playersettings.effects.generic.left		= 192 - 8;
	s_playersettings.effects.generic.top		= y - 8;
	s_playersettings.effects.generic.right		= 192 + 200;
	s_playersettings.effects.generic.bottom	= y + 2* PROP_HEIGHT;
*/
	s_playersettings.effects.generic.x			= PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.effects.generic.y			= profileY;
	s_playersettings.effects.generic.left		= PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.effects.generic.top		= profileY;
	s_playersettings.effects.generic.right		= PLAYERSETTINGS_PROFILE_FORM_RIGHT;
	s_playersettings.effects.generic.bottom	= profileY + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;
// END
	s_playersettings.effects.numitems			= 7;

	/* Profile is intentionally two-column: editable identity on the left,
	 * avatar and appearance controls on the right. */
	s_playersettings.avatar.generic.x = PLAYERSETTINGS_PROFILE_PRESENCE_X + 8;
	s_playersettings.avatar.generic.y = PLAYERSETTINGS_PROFILE_PRESENCE_Y + 38;
	s_playersettings.avatar.generic.left = s_playersettings.avatar.generic.x;
	s_playersettings.avatar.generic.top = s_playersettings.avatar.generic.y;
	s_playersettings.avatar.generic.right = PLAYERSETTINGS_PROFILE_PRESENCE_X + PLAYERSETTINGS_PROFILE_PRESENCE_WIDTH - 8;
	s_playersettings.avatar.generic.bottom = s_playersettings.avatar.generic.y + 108;
	s_playersettings.country.generic.y = PLAYERSETTINGS_PROFILE_FORM_Y + 146;
	s_playersettings.country.generic.top = s_playersettings.country.generic.y;
	s_playersettings.country.generic.bottom = s_playersettings.country.generic.y + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;
	s_playersettings.country.generic.x = PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.country.generic.y = PLAYERSETTINGS_PROFILE_FORM_Y + 146;
	s_playersettings.country.generic.left = PLAYERSETTINGS_PROFILE_FIELD_LEFT;
	s_playersettings.country.generic.top = s_playersettings.country.generic.y;
	s_playersettings.country.generic.right = PLAYERSETTINGS_PROFILE_FORM_RIGHT;
	s_playersettings.country.generic.bottom = s_playersettings.country.generic.y + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;
	s_playersettings.handicap.generic.y = PLAYERSETTINGS_PROFILE_FORM_Y + 182;
	s_playersettings.handicap.generic.top = s_playersettings.handicap.generic.y;
	s_playersettings.handicap.generic.bottom = s_playersettings.handicap.generic.y + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;
	s_playersettings.effects.generic.x = PLAYERSETTINGS_PROFILE_PRESENCE_X + 8;
	s_playersettings.effects.generic.y = PLAYERSETTINGS_PROFILE_PRESENCE_Y + 174;
	s_playersettings.effects.generic.left = s_playersettings.effects.generic.x;
	s_playersettings.effects.generic.top = s_playersettings.effects.generic.y;
	s_playersettings.effects.generic.right = PLAYERSETTINGS_PROFILE_PRESENCE_X + PLAYERSETTINGS_PROFILE_PRESENCE_WIDTH - 8;
	s_playersettings.effects.generic.bottom = s_playersettings.effects.generic.y + PLAYERSETTINGS_PROFILE_FIELD_HEIGHT;

// STONELANCE
/*
	s_playersettings.model.generic.type			= MTYPE_BITMAP;
	s_playersettings.model.generic.name			= ART_MODEL0;
	s_playersettings.model.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playersettings.model.generic.id			= ID_MODEL;
	s_playersettings.model.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.model.generic.x			= 640;
	s_playersettings.model.generic.y			= 480-64;
	s_playersettings.model.width				= 128;
	s_playersettings.model.height				= 64;
	s_playersettings.model.focuspic				= ART_MODEL1;
*/
	s_playersettings.customize.generic.type		= MTYPE_PTEXT;
	s_playersettings.customize.generic.flags	= QMF_NODEFAULTINIT;
	s_playersettings.customize.generic.id		= ID_CUSTOMIZE;
	s_playersettings.customize.generic.ownerdraw= PlayerSettings_DrawCustomize;
	s_playersettings.customize.generic.x		= 410;
	s_playersettings.customize.generic.y		= 272;
	s_playersettings.customize.generic.left		= 410;
	s_playersettings.customize.generic.top		= 272;
	s_playersettings.customize.generic.right	= 576;
	s_playersettings.customize.generic.bottom	= 302;
	s_playersettings.customize.generic.callback	= PlayerSettings_MenuEvent; 
	s_playersettings.customize.color			= text_color_normal;
	s_playersettings.customize.style			= UI_RIGHT;
// END

	s_playersettings.player.generic.type		= MTYPE_BITMAP;
	s_playersettings.player.generic.flags		= QMF_INACTIVE;
	s_playersettings.player.generic.ownerdraw	= PlayerSettings_DrawPlayer;
// STONELANCE
/*
	s_playersettings.player.generic.x			= 400;
	s_playersettings.player.generic.y			= -40;
	s_playersettings.player.width				= 32*10;
	s_playersettings.player.height				= 56*10;
*/
	s_playersettings.player.generic.x	       = 94;
	s_playersettings.player.generic.y	       = 184;
	s_playersettings.player.width	           = 270;
	s_playersettings.player.height             = 82;


	y = 148;
	s_playersettings.modelname.generic.type   = MTYPE_PTEXT;
	s_playersettings.modelname.generic.flags  = QMF_CENTER_JUSTIFY|QMF_INACTIVE|QMF_HIDDEN;
	s_playersettings.modelname.generic.x	  = 229;
	s_playersettings.modelname.generic.y	  = y + 4;
	s_playersettings.modelname.string	      = modelname;
	s_playersettings.modelname.style		  = UI_CENTER;
	s_playersettings.modelname.color          = text_color_normal;

	s_playersettings.left.generic.type			= MTYPE_BITMAP;
	s_playersettings.left.generic.name			= ART_LEFT0;
	s_playersettings.left.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_NODEFAULTINIT;
	s_playersettings.left.generic.ownerdraw		= PlayerSettings_DrawVehicleControl;
	s_playersettings.left.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.left.generic.id			= ID_LEFT;
	s_playersettings.left.generic.x				= 74;
	s_playersettings.left.generic.y				= 274;
	s_playersettings.left.generic.left			= 74;
	s_playersettings.left.generic.top			= 274;
	s_playersettings.left.generic.right			= 224;
	s_playersettings.left.generic.bottom			= 306;
	s_playersettings.left.width  				= 150;
	s_playersettings.left.height  				= 32;
	s_playersettings.left.focuspic				= ART_LEFT1;
	
	s_playersettings.right.generic.type			= MTYPE_BITMAP;
	s_playersettings.right.generic.name			= ART_RIGHT0;
	s_playersettings.right.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_NODEFAULTINIT;
	s_playersettings.right.generic.ownerdraw		= PlayerSettings_DrawVehicleControl;
	s_playersettings.right.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.right.generic.id			= ID_RIGHT;
	s_playersettings.right.generic.x			= 234;
	s_playersettings.right.generic.y			= 274;
	s_playersettings.right.generic.left			= 234;
	s_playersettings.right.generic.top			= 274;
	s_playersettings.right.generic.right			= 384;
	s_playersettings.right.generic.bottom			= 306;
	s_playersettings.right.width  				= 150;
	s_playersettings.right.height  				= 32;
	s_playersettings.right.focuspic				= ART_RIGHT1;

	s_playersettings.favorites.generic.type   = MTYPE_PTEXT;
	s_playersettings.favorites.generic.flags  = QMF_CENTER_JUSTIFY|QMF_INACTIVE|QMF_HIDDEN;
	s_playersettings.favorites.generic.x	  = 320;
	s_playersettings.favorites.generic.y	  = 326;
	s_playersettings.favorites.string	      = "LOAD FAVORITE";
	s_playersettings.favorites.style		  = UI_CENTER|UI_SMALLFONT;
	s_playersettings.favorites.color          = text_color_normal;

	x =	64;
	y = 356;
	for (j=0; j<NUM_FAVORITES; j++)
	{
		s_playersettings.ports[j].generic.type		= MTYPE_BITMAP;
		s_playersettings.ports[j].generic.name		= ART_PORT;
		s_playersettings.ports[j].generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE|QMF_HIDDEN;
		s_playersettings.ports[j].generic.x			= x;
		s_playersettings.ports[j].generic.y			= y;
		s_playersettings.ports[j].width  			= 64;
		s_playersettings.ports[j].height  			= 64;

		s_playersettings.favpics[j].generic.type	= MTYPE_BITMAP;
		s_playersettings.favpics[j].generic.flags	= QMF_LEFT_JUSTIFY|QMF_INACTIVE|QMF_HIDDEN;
		s_playersettings.favpics[j].generic.x		= x;
		s_playersettings.favpics[j].generic.y		= y;
		s_playersettings.favpics[j].width  			= 64;
		s_playersettings.favpics[j].height  		= 64;
		s_playersettings.favpics[j].focuspic        = ART_SELECTED;
		s_playersettings.favpics[j].focuscolor      = text_color_highlight;

		s_playersettings.favpicbuttons[j].generic.type		= MTYPE_BITMAP;
		s_playersettings.favpicbuttons[j].generic.flags		= QMF_LEFT_JUSTIFY|QMF_NODEFAULTINIT|QMF_PULSEIFFOCUS;
		s_playersettings.favpicbuttons[j].generic.ownerdraw	= PlayerSettings_DrawFavoriteButton;
		s_playersettings.favpicbuttons[j].generic.id	    = ID_FAVORITE1 + j;
		s_playersettings.favpicbuttons[j].generic.callback	= PlayerSettings_PicEvent;
		s_playersettings.favpicbuttons[j].generic.x    		= x;
		s_playersettings.favpicbuttons[j].generic.y			= y;
		s_playersettings.favpicbuttons[j].generic.left		= x;
		s_playersettings.favpicbuttons[j].generic.top		= y;
		s_playersettings.favpicbuttons[j].generic.right		= x + 120;
		s_playersettings.favpicbuttons[j].generic.bottom	= y + 28;
		s_playersettings.favpicbuttons[j].width  		    = 120;
		s_playersettings.favpicbuttons[j].height  			= 28;
		s_playersettings.favpicbuttons[j].focuspic  		= ART_SELECT;
		s_playersettings.favpicbuttons[j].focuscolor  		= text_color_highlight;

		x += 128;
	}

	s_playersettings.plate.generic.type				= MTYPE_PTEXT;
	s_playersettings.plate.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_NODEFAULTINIT;
	s_playersettings.plate.generic.ownerdraw			= PlayerSettings_DrawPlateItem;
	s_playersettings.plate.generic.x				= 410;
	s_playersettings.plate.generic.y				= 310;
	s_playersettings.plate.generic.left			    = 410;
	s_playersettings.plate.generic.top				= 310;
	s_playersettings.plate.generic.right			= 576;
	s_playersettings.plate.generic.bottom			= 340;
	s_playersettings.plate.generic.id				= ID_PLATE;
	s_playersettings.plate.generic.callback			= PlayerSettings_MenuEvent; 
	s_playersettings.plate.string					= "CHANGE PLATE";
	s_playersettings.plate.color					= text_color_normal;
	s_playersettings.plate.style					= UI_LEFT | UI_SMALLFONT;


	s_playersettings.back.generic.type				= MTYPE_PTEXT;
	s_playersettings.back.generic.flags				= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playersettings.back.generic.ownerdraw				= PlayerSettings_DrawBackItem;
	s_playersettings.back.generic.x					= PLAYERSETTINGS_BACK_BUTTON_LEFT;
	s_playersettings.back.generic.y					= PLAYERSETTINGS_BACK_BUTTON_Y;
	s_playersettings.back.generic.left					= PLAYERSETTINGS_BACK_BUTTON_LEFT;
	s_playersettings.back.generic.top					= PLAYERSETTINGS_BACK_BUTTON_Y;
	s_playersettings.back.generic.right					= PLAYERSETTINGS_BACK_BUTTON_LEFT + 112;
	s_playersettings.back.generic.bottom					= PLAYERSETTINGS_BACK_BUTTON_Y + 24;
	s_playersettings.back.generic.id				= ID_BACK;
	s_playersettings.back.generic.callback			= PlayerSettings_MenuEvent; 
	s_playersettings.back.string					= "< BACK";
	s_playersettings.back.color						= text_color_normal;
	s_playersettings.back.style						= UI_LEFT | UI_SMALLFONT;

/*
	s_playersettings.back.generic.type			= MTYPE_BITMAP;
	s_playersettings.back.generic.name			= ART_BACK0;
	s_playersettings.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playersettings.back.generic.id			= ID_BACK;
	s_playersettings.back.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.back.generic.x				= 0;
	s_playersettings.back.generic.y				= 480-64;
	s_playersettings.back.width					= 128;
	s_playersettings.back.height				= 64;
	s_playersettings.back.focuspic				= ART_BACK1;

	s_playersettings.item_null.generic.type		= MTYPE_BITMAP;
	s_playersettings.item_null.generic.flags	= QMF_LEFT_JUSTIFY|QMF_MOUSEONLY|QMF_SILENT;
	s_playersettings.item_null.generic.x		= 0;
	s_playersettings.item_null.generic.y		= 0;
	s_playersettings.item_null.width			= 640;
	s_playersettings.item_null.height			= 480;
*/
// END

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.banner );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.tabProfile );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.tabVehicle );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.tabStats );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.tabAchievements );
// STONELANCE
/*
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.framel );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.framer );
*/
// END

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.name );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.gender );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.birthDateLabel );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.birthDay );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.birthMonth );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.birthYear );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.avatar );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.country );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.handicap );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.effects );

// STONELANCE
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.favorites );
	for (i=0; i<NUM_FAVORITES; i++)
	{
		Menu_AddItem( &s_playersettings.menu, &s_playersettings.ports[i] );
		Menu_AddItem( &s_playersettings.menu, &s_playersettings.favpicbuttons[i] );
		Menu_AddItem( &s_playersettings.menu, &s_playersettings.favpics[i] );
	}

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.player );

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.left );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.right );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.modelname );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.customize );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.plate );
// END

// STONELANCE
//	Menu_AddItem( &s_playersettings.menu, &s_playersettings.model );
// END

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.back );

// STONELANCE
//	Menu_AddItem( &s_playersettings.menu, &s_playersettings.item_null );
// END

	PlayerSettings_SetTab( TAB_PROFILE );
	PlayerSettings_SetMenuItems();

// STONELANCE
	uis.transitionIn = uis.realtime;
// END
}


/*
=================
PlayerSettings_Cache
=================
*/
void PlayerSettings_Cache( void ) {
        int i;
// STONELANCE
/*
	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_MODEL0 );
	trap_R_RegisterShaderNoMip( ART_MODEL1 );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
*/
// END

	s_playersettings.fxBasePic = trap_R_RegisterShaderNoMip( ART_FX_BASE );
	s_playersettings.fxPic[0] = trap_R_RegisterShaderNoMip( ART_FX_RED );
	s_playersettings.fxPic[1] = trap_R_RegisterShaderNoMip( ART_FX_YELLOW );
	s_playersettings.fxPic[2] = trap_R_RegisterShaderNoMip( ART_FX_GREEN );
	s_playersettings.fxPic[3] = trap_R_RegisterShaderNoMip( ART_FX_TEAL );
	s_playersettings.fxPic[4] = trap_R_RegisterShaderNoMip( ART_FX_BLUE );
	s_playersettings.fxPic[5] = trap_R_RegisterShaderNoMip( ART_FX_CYAN );
	s_playersettings.fxPic[6] = trap_R_RegisterShaderNoMip( ART_FX_WHITE );

        for ( i = 0; i < PLAYERSETTINGS_ACHIEVEMENT_ICON_COUNT; ++i ) {
                s_playersettings.achievementMedalLocked[i] = 0;
                s_playersettings.achievementMedalUnlocked[i] = 0;
        }
        for ( i = 0; i < PLAYERSETTINGS_ACHIEVEMENT_ICON_COUNT; ++i ) {
                s_playersettings.achievementMedalLocked[i] = PlayerSettings_RegisterAchievementMedal( s_achievementMedalLockedPaths[i] );
                s_playersettings.achievementMedalUnlocked[i] = PlayerSettings_RegisterAchievementMedal( s_achievementMedalUnlockedPaths[i] );
        }

// STONELANCE
	PlayerSettings_BuildList();
// END
}


/*
=================
UI_PlayerSettingsMenu
=================
*/
void UI_PlayerSettingsMenu( void ) {
	PlayerSettings_MenuInit();
	UI_PushMenu( &s_playersettings.menu );
}

void UI_PlayerStatsMenu( void ) {
	PlayerSettings_MenuInit();
	PlayerSettings_SetTab( TAB_STATS );
	UI_PushMenu( &s_playersettings.menu );
}


// STONELANCE
/*****************************************************

  Plate Selection

*****************************************************/


#define		ID_LIST			1
#define		ID_CANCEL		2
#define		ID_ACCEPT		3


#define		MAX_PLATEMODELS		256

typedef struct {
	menuframework_s		menu;

	menulist_s			list;

	menutext_s			cancel;
	menutext_s			accept;

	char				plateList[MAX_PLATEMODELS][MAX_QPATH];
	char*				items[MAX_PLATEMODELS];
	int					numPlates;

	char				plateSkin[MAX_QPATH];
} plateSelection_t;

static plateSelection_t	s_plateSelection;


/*
=================
PlateSelection_Event
=================
*/
static void PlateSelection_Event( void* ptr, int event ) {
	int		id;

	id = ((menucommon_s*)ptr)->id;

	if( event != QM_ACTIVATED && id != ID_LIST ) {
		return;
	}

	switch( id ) {
	case ID_LIST:
		// update plateSkin
		Q_strncpyz( s_plateSelection.plateSkin, s_plateSelection.plateList[s_plateSelection.list.curvalue], sizeof(s_plateSelection.plateSkin) );
		break;

	case ID_CANCEL:
		UI_PopMenu();
		uis.transitionIn = 0;
		break;

	case ID_ACCEPT:
		trap_Cvar_Set( "plate", s_plateSelection.plateSkin );
		PlayerSettings_UpdateModel();
		UI_PopMenu();
		uis.transitionIn = 0;
		break;
	}
}


/*
=================
PlateSelection_DrawMenu
=================
*/
static void PlateSelection_DrawMenu( void ) {
	refdef_t		refdef;
	refEntity_t		ent;
	vec3_t			origin;
	vec3_t			angles;
	float			x, y, w, h;

	// setup the refdef

	memset( &refdef, 0, sizeof( refdef ) );

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	x = 150;
	y = 115;
	w = 328;
	h = 232;
	UI_AdjustFrom640( &x, &y, &w, &h );
	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.fov_x = 180;
	refdef.fov_y = 180;

	refdef.time = uis.realtime;

	origin[0] = 300;
	origin[1] = 0;
	origin[2] = 0;

	trap_R_ClearScene();

	// draw license plate with selected skin

	memset( &ent, 0, sizeof(ent) );

	VectorSet( angles, 45, 45, 45 );
	AnglesToAxis( angles, ent.axis );

	if (strstr(s_plateSelection.plateSkin, "usa_"))
		ent.hModel = trap_R_RegisterModel("models/players/plates/plate_usa.md3");
	else
		ent.hModel = trap_R_RegisterModel("models/players/plates/plate_eu.md3");
	ent.customShader = trap_R_RegisterShaderNoMip( va("models/players/plates/%s", s_plateSelection.plateSkin) );

	VectorCopy( origin, ent.origin );
	VectorCopy( origin, ent.lightingOrigin );
	ent.renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW;
	VectorCopy( ent.origin, ent.oldorigin );

	trap_R_AddRefEntityToScene( &ent );

	trap_R_RenderScene( &refdef );

/*
	qhandle_t	plate;
	plate = trap_R_RegisterShaderNoMip( va("models/players/plates/%s", s_plateSelection.plateSkin) );

	if (strstr(s_plateSelection.plateSkin, "usa_"))
		UI_DrawHandlePic(250+32, 215, 64, 32, plate);
	else
		UI_DrawHandlePic(250, 215, 128, 32, plate);
*/

	Menu_Draw( &s_plateSelection.menu );
}


/*
=================
PlateSelection_SetMenuItems
=================
*/
static void PlateSelection_SetMenuItems( void ) {
	int		i;

	trap_Cvar_VariableStringBuffer( "plate", s_plateSelection.plateSkin, sizeof( s_plateSelection.plateSkin ) );

	if (!s_plateSelection.numPlates)
		return;

	// find model in our list
	for (i = 0; i < s_plateSelection.numPlates; i++)
	{
		if (!Q_stricmp( s_plateSelection.plateSkin, s_plateSelection.plateList[i] )){
			// found pic, set selection here
			s_plateSelection.list.curvalue = i;
			/* Set top so the selected item is visible, then clamp to valid range */
			s_plateSelection.list.top = i;
			if (s_plateSelection.list.top + s_plateSelection.list.height > s_plateSelection.numPlates)
				s_plateSelection.list.top = s_plateSelection.numPlates - s_plateSelection.list.height;
			if (s_plateSelection.list.top < 0)
				s_plateSelection.list.top = 0;

			return;
		}
	}

	s_plateSelection.list.curvalue = 0;
	s_plateSelection.list.top = 0;
	Q_strncpyz(s_plateSelection.plateSkin, s_plateSelection.plateList[0], sizeof(s_plateSelection.plateSkin));
}


/*
=================
PlateSelection_Cache
=================
*/
void PlateSelection_Cache( void ) {
	// get car list
	s_plateSelection.numPlates = UI_BuildFileList("models/players/plates", "tga", "*usa_", qtrue, qfalse, qtrue, 0, s_plateSelection.plateList);
	s_plateSelection.numPlates = UI_BuildFileList("models/players/plates", "tga", "*eu_", qtrue, qfalse, qtrue, s_plateSelection.numPlates, s_plateSelection.plateList);
}


/*
=================
PlateSelection_MenuInit
=================
*/
static void PlateSelection_MenuInit( void ) {
	int		i;

	memset(&s_plateSelection, 0, sizeof(plateSelection_t));

	PlateSelection_Cache();

	s_plateSelection.menu.wrapAround = qtrue;
	s_plateSelection.menu.transparent = qtrue;
	s_plateSelection.menu.fullscreen = qtrue;
	s_plateSelection.menu.draw		 = PlateSelection_DrawMenu;


	s_plateSelection.list.generic.type			= MTYPE_LISTBOX;
	s_plateSelection.list.scrollbarAlignment	= SB_RIGHT;
	s_plateSelection.list.generic.flags			= QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS;
	s_plateSelection.list.generic.id			= ID_LIST;
	s_plateSelection.list.generic.callback		= PlateSelection_Event;
	s_plateSelection.list.generic.x				= 50;
	s_plateSelection.list.generic.y				= 175;
	s_plateSelection.list.width					= 25;
	s_plateSelection.list.height				= 11;
	s_plateSelection.list.itemnames				= (const char **)s_plateSelection.items;
	s_plateSelection.list.numitems				= s_plateSelection.numPlates;
	for( i = 0; i < s_plateSelection.numPlates; i++ ) {
		s_plateSelection.items[i] = s_plateSelection.plateList[i];
	}

	s_plateSelection.cancel.generic.type				= MTYPE_PTEXT;
	s_plateSelection.cancel.generic.flags				= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_plateSelection.cancel.generic.x					= 250;
	s_plateSelection.cancel.generic.y					= 350;
	s_plateSelection.cancel.generic.id					= ID_CANCEL;
	s_plateSelection.cancel.generic.callback			= PlateSelection_Event; 
	s_plateSelection.cancel.string						= "Cancel";
	s_plateSelection.cancel.color						= text_color_normal;
	s_plateSelection.cancel.style						= UI_LEFT | UI_SMALLFONT;

	s_plateSelection.accept.generic.type				= MTYPE_PTEXT;
	s_plateSelection.accept.generic.flags				= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_plateSelection.accept.generic.x					= 350;
	s_plateSelection.accept.generic.y					= 350;
	s_plateSelection.accept.generic.id					= ID_ACCEPT;
	s_plateSelection.accept.generic.callback			= PlateSelection_Event; 
	s_plateSelection.accept.string						= "Accept";
	s_plateSelection.accept.color						= text_color_normal;
	s_plateSelection.accept.style						= UI_LEFT | UI_SMALLFONT;


	Menu_AddItem( &s_plateSelection.menu, &s_plateSelection.list );
	Menu_AddItem( &s_plateSelection.menu, &s_plateSelection.cancel );
	Menu_AddItem( &s_plateSelection.menu, &s_plateSelection.accept );


	PlateSelection_SetMenuItems();
}


/*
=================
UI_PlateSelectionMenu
=================
*/
void UI_PlateSelectionMenu( void ) {
	PlateSelection_MenuInit();
	UI_PushMenu( &s_plateSelection.menu );
}
// END
