/*
===========================================================================
Copyright (C) 2002-2026 Q3Rally Team
===========================================================================
*/
// ui_rally_profile_wizard.c
//
// Full-screen profile creation wizard. Shown automatically whenever no
// active profile is set (first launch, after profile deletion, etc.).
//
// Flow:
//   Page 1 — Enter display name
//   Page 2 — Gender, Birth date, Avatar, Country  (all optional, skippable)
//   Page 3 — Ladder opt-in (optional) or skip → done
//
// KEY DESIGN RULES:
//   - Each page owns its own set of menu items.
//   - PW_SetPageItems() hides+deactivates all items, then shows only the
//     items belonging to the current page.  This prevents keyboard focus
//     from leaking to items on other pages.
//   - The single Com_Memset is done in UI_ProfileWizard_Show so all
//     pointers are valid before any item is initialised.

#include "ui_local.h"
#include "ui_rally_theme.h"
#include "ui_rally_frontend.h"

// ── Layout constants ──────────────────────────────────────────────────────────

#define PW_W            640
#define PW_H            480
#define PW_CX           ( PW_W / 2 )

#define PW_PANEL_W      496
#define PW_PANEL_H      270
#define PW_PANEL_X      72
#define PW_PANEL_Y      112

#define PW_FRAME_X      48
#define PW_FRAME_Y      28
#define PW_FRAME_W      544
#define PW_FRAME_H      420

#define PW_PAD          28
#define PW_CONTENT_X    ( PW_PANEL_X + PW_PAD )
#define PW_CONTENT_W    ( PW_PANEL_W - PW_PAD * 2 )
#define PW_TITLE_Y      54
#define PW_BODY_Y       ( PW_PANEL_Y + 48 )

#define PW_BTN_Y        414
#define PW_BTN_WIDTH    104
#define PW_BTN_NEXT_X   472
#define PW_BTN_BACK_X   64
#define PW_BTN_SKIP_X   268

// Page 2 field layout — starts lower to give heading room
#define PW_P2_LABEL_X   ( PW_PANEL_X + PW_PAD )
#define PW_P2_SPIN_X    ( PW_PANEL_X + 130 )
#define PW_P2_ROW_H     28
#define PW_P2_ROW_1     ( PW_BODY_Y + 28 )
#define PW_P2_ROW_2     ( PW_P2_ROW_1 + PW_P2_ROW_H )
#define PW_P2_ROW_3     ( PW_P2_ROW_2 + PW_P2_ROW_H )
#define PW_P2_ROW_4     ( PW_P2_ROW_3 + PW_P2_ROW_H )
#define PW_P2_ROW_5     ( PW_P2_ROW_4 + PW_P2_ROW_H + 8 )
#define PW_P2_ROW_6     ( PW_P2_ROW_5 + PW_P2_ROW_H )

#define PW_AVATAR_SIZE  48
#define PW_AVATAR_X     ( PW_PANEL_X + PW_PANEL_W - PW_PAD - PW_AVATAR_SIZE )
#define PW_AVATAR_Y     ( PW_P2_ROW_5 - 4 )

#define PW_DOT_Y        394
#define PW_DOT_SPACING  14
#define PW_PAGES        3
#define PW_STATUS_Y     ( PW_DOT_Y - 22 )
#define PW_HINT_Y       ( PW_FRAME_Y + PW_FRAME_H + 8 )

// ── IDs ───────────────────────────────────────────────────────────────────────

#define ID_PW_NEXT      20
#define ID_PW_BACK      21
#define ID_PW_SKIP      22
#define ID_PW_NAME      23
#define ID_PW_AVATAR    24
#define ID_PW_COUNTRY   25
#define ID_PW_GENDER    26
#define ID_PW_BDAY      27
#define ID_PW_BMONTH    28
#define ID_PW_BYEAR     29

// ── Pages ─────────────────────────────────────────────────────────────────────

typedef enum {
    PW_PAGE_NAME    = 0,
    PW_PAGE_DETAILS = 1,
    PW_PAGE_LADDER  = 2,
    PW_PAGE_DONE    = 3
} pwPage_t;

typedef enum {
    PW_RESULT_NONE = 0,
    PW_RESULT_PENDING,
    PW_RESULT_OK,
    PW_RESULT_ERROR
} pwResult_t;

// ── Colours ───────────────────────────────────────────────────────────────────

static vec4_t pwScrim    = UI_FRONTEND_COLOR_SCRIM;
static vec4_t pwText     = UI_FRONTEND_COLOR_TEXT;
static vec4_t pwMuted    = UI_FRONTEND_COLOR_MUTED;
static vec4_t pwAccent   = UI_FRONTEND_COLOR_ACCENT;
static vec4_t pwStatus   = UI_FRONTEND_COLOR_STATUS;
static vec4_t pwSuccess  = UI_FRONTEND_COLOR_ACCENT;
static vec4_t pwError    = UI_FRONTEND_COLOR_STATUS;
static vec4_t pwDotOn    = UI_FRONTEND_COLOR_ACCENT;
static vec4_t pwDotOff   = UI_FRONTEND_COLOR_MUTED;
static vec4_t pwAvatarBg = UI_FRONTEND_COLOR_PANEL_ALT;

// ── Lists ─────────────────────────────────────────────────────────────────────

static const char *s_pwGenderItems[] = {
    "Unspecified", "Female", "Male", "Non-binary", "Other", NULL
};
#define PW_GENDER_COUNT 5

#define PW_BIRTH_YEAR_START  1950
#define PW_BIRTH_YEAR_END    2100
#define PW_BIRTH_YEAR_COUNT  ( ( PW_BIRTH_YEAR_END ) - ( PW_BIRTH_YEAR_START ) + 1 )
#define PW_BIRTH_DAY_MAX     31

static const char *s_pwBirthMonthItems[] = {
    "-",
    "January", "February", "March",    "April",
    "May",     "June",     "July",     "August",
    "September","October", "November", "December",
    NULL
};
#define PW_BIRTH_MONTH_COUNT 13

static const char  *s_pwBirthDayItems[ PW_BIRTH_DAY_MAX + 2 ];
static char         s_pwBirthDayStrings[ PW_BIRTH_DAY_MAX + 1 ][ 3 ];

static const char  *s_pwBirthYearItems[ PW_BIRTH_YEAR_COUNT + 2 ];
static char         s_pwBirthYearStrings[ PW_BIRTH_YEAR_COUNT ][ 5 ];

static qboolean s_pwBirthListsBuilt = qfalse;

static void PW_BuildBirthDateLists( void ) {
    int i;
    if ( s_pwBirthListsBuilt ) return;
    s_pwBirthDayItems[0] = "-";
    for ( i = 1; i <= PW_BIRTH_DAY_MAX; ++i ) {
        Com_sprintf( s_pwBirthDayStrings[i], sizeof( s_pwBirthDayStrings[i] ), "%d", i );
        s_pwBirthDayItems[i] = s_pwBirthDayStrings[i];
    }
    s_pwBirthDayItems[ PW_BIRTH_DAY_MAX + 1 ] = NULL;
    s_pwBirthYearItems[0] = "-";
    for ( i = 0; i < PW_BIRTH_YEAR_COUNT; ++i ) {
        Com_sprintf( s_pwBirthYearStrings[i], sizeof( s_pwBirthYearStrings[i] ),
                     "%d", PW_BIRTH_YEAR_START + i );
        s_pwBirthYearItems[ i + 1 ] = s_pwBirthYearStrings[i];
    }
    s_pwBirthYearItems[ PW_BIRTH_YEAR_COUNT + 1 ] = NULL;
    s_pwBirthListsBuilt = qtrue;
}

#define PW_AVATAR_COUNT 10
static const char *s_avatarShaderPaths[ PW_AVATAR_COUNT ] = {
    "", "gfx/avatars/preset/driver_01", "gfx/avatars/preset/driver_02",
    "gfx/avatars/preset/driver_03", "gfx/avatars/preset/driver_04",
    "gfx/avatars/preset/driver_05", "gfx/avatars/preset/driver_06",
    "gfx/avatars/preset/driver_07", "gfx/avatars/preset/driver_08",
    "gfx/avatars/preset/driver_09",
};
static const char *s_avatarDisplayNames[ PW_AVATAR_COUNT + 1 ] = {
    "None",
    "Driver 01", "Driver 02", "Driver 03",
    "Driver 04", "Driver 05", "Driver 06",
    "Driver 07", "Driver 08", "Driver 09",
    NULL
};

#define PW_COUNTRY_COUNT 42
static const char *s_countryCodes[ PW_COUNTRY_COUNT ] = {
    "",
    "AT","AU","BE","BR","CA","CH","CN","CZ","DE",
    "DK","ES","FI","FR","GB","GR","HU","ID","IN",
    "IT","JP","KR","MX","NL","NO","NZ","PL","PT",
    "RO","RU","SE","SG","SK","TH","TR","TW","UA",
    "US","VN","ZA","AR","CL",
};
static const char *s_countryNames[ PW_COUNTRY_COUNT + 1 ] = {
    "Not specified",
    "Austria",       "Australia",      "Belgium",       "Brazil",
    "Canada",        "Switzerland",    "China",         "Czech Republic",
    "Germany",       "Denmark",        "Spain",         "Finland",
    "France",        "United Kingdom", "Greece",        "Hungary",
    "Indonesia",     "India",          "Italy",         "Japan",
    "South Korea",   "Mexico",         "Netherlands",   "Norway",
    "New Zealand",   "Poland",         "Portugal",      "Romania",
    "Russia",        "Sweden",         "Singapore",     "Slovakia",
    "Thailand",      "Turkey",         "Taiwan",        "Ukraine",
    "United States", "Vietnam",        "South Africa",  "Argentina",
    "Chile",
    NULL
};

// ── State ─────────────────────────────────────────────────────────────────────

static struct {
    menuframework_s menu;

    // Page 1
    menufield_s     nameField;

    // Page 2
    menulist_s      genderSpin;
    menulist_s      bDaySpin;
    menulist_s      bMonthSpin;
    menulist_s      bYearSpin;
    menulist_s      avatarSpin;
    menulist_s      countrySpin;

    // Navigation buttons (shared across pages)
    menutext_s      btnNext;
    menutext_s      btnBack;
    menutext_s      btnSkip;

    pwPage_t        page;
    pwResult_t      ladderResult;
    pwResult_t      offlineKeyResult;

    char            profileName[ PROFILE_MAX_NAME ];
    char            offlineServerName[ 68 ];
    char            statusLine[ 128 ];
    qboolean        submitting;

    qhandle_t       avatarShader;
    int             avatarCurIdx;

    playerInfo_t    doneVehicleInfo;   // rotating vehicle on the Done page

} s_pw;

// ── Forward declarations ──────────────────────────────────────────────────────

static void PW_Draw( void );
static void PW_MenuEvent( void *ptr, int event );
static sfxHandle_t PW_MenuKey( int key );
static void PW_SetPage( pwPage_t page );
static void PW_SetPageItems( pwPage_t page );
static void PW_UpdateButtons( void );
static void PW_SetChoiceBounds( void );
static void PW_LoadAvatarShader( int idx );
static void PW_DrawField( void *self );
static void PW_DrawChoice( void *self );
static void PW_DrawButton( void *self );

// ── Item visibility control ───────────────────────────────────────────────────
//
// Called on every page transition. Hides and deactivates ALL interactive
// items, then re-enables only the ones that belong to the target page.
// This is what prevents keyboard focus from leaking to off-page items.

static void PW_SetPageItems( pwPage_t page ) {
    // All items off — covers buttons too; PW_UpdateButtons will re-enable
    // the appropriate buttons for this page right after.
    unsigned int allOff = QMF_INACTIVE | QMF_HIDDEN;

    s_pw.nameField.generic.flags  = allOff;

    s_pw.genderSpin.generic.flags  = allOff;
    s_pw.bDaySpin.generic.flags    = allOff;
    s_pw.bMonthSpin.generic.flags  = allOff;
    s_pw.bYearSpin.generic.flags   = allOff;
    s_pw.avatarSpin.generic.flags  = allOff;
    s_pw.countrySpin.generic.flags = allOff;

    s_pw.btnNext.generic.flags = allOff;
    s_pw.btnBack.generic.flags = allOff;
    s_pw.btnSkip.generic.flags = allOff;

    switch ( page ) {
    case PW_PAGE_NAME:
        s_pw.nameField.generic.flags = QMF_SMALLFONT;
        break;

    case PW_PAGE_DETAILS:
        s_pw.genderSpin.generic.flags  = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
        s_pw.bDaySpin.generic.flags    = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
        s_pw.bMonthSpin.generic.flags  = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
        s_pw.bYearSpin.generic.flags   = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
        s_pw.avatarSpin.generic.flags  = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
        s_pw.countrySpin.generic.flags = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
        break;

    case PW_PAGE_LADDER:
    case PW_PAGE_DONE:
        // No interactive input items on these pages — buttons only.
        break;
    }
    // Buttons are handled by PW_UpdateButtons() which is called right after
    // PW_SetPageItems() in PW_SetPage().
}

// ── Helpers ───────────────────────────────────────────────────────────────────

static qboolean PW_IsValidName( const char *name ) {
    int len, i;
    if ( !name || !name[0] ) return qfalse;
    len = strlen( name );
    if ( len < 1 || len >= PROFILE_MAX_NAME ) return qfalse;
    for ( i = 0; i < len; ++i ) {
        char c = name[i];
        if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ||
             ( c >= '0' && c <= '9' ) ||
             c == '_' || c == '-' || c == '.' ) continue;
        return qfalse;
    }
    return qtrue;
}

static void PW_TrimName( char *name ) {
    int start = 0, end, len, i;
    if ( !name ) return;
    len = strlen( name );
    while ( start < len && ( name[start] == ' ' || name[start] == '\t' ) ) ++start;
    end = len;
    while ( end > start && ( name[end-1] == ' ' || name[end-1] == '\t' ) ) --end;
    if ( start > 0 || end < len ) {
        for ( i = 0; i < end - start; ++i ) name[i] = name[ start + i ];
        name[ end - start ] = '\0';
    }
}

static void PW_LoadAvatarShader( int idx ) {
    if ( idx <= 0 || idx >= PW_AVATAR_COUNT || !s_avatarShaderPaths[idx][0] ) {
        s_pw.avatarShader = 0;
        s_pw.avatarCurIdx = 0;
        return;
    }
    s_pw.avatarShader = trap_R_RegisterShaderNoMip( s_avatarShaderPaths[idx] );
    s_pw.avatarCurIdx = idx;
}

// ── Profile creation / update ─────────────────────────────────────────────────

static qboolean PW_CommitName( void ) {
    char name[ PROFILE_MAX_NAME ];
    Q_strncpyz( name, s_pw.nameField.field.buffer, sizeof( name ) );
    PW_TrimName( name );
    if ( !PW_IsValidName( name ) ) {
        Q_strncpyz( s_pw.statusLine, "Name must use letters, numbers, _ - . only.", sizeof( s_pw.statusLine ) );
        return qfalse;
    }
    if ( !UI_Profile_WriteDefaultFile( name ) ) {
        Q_strncpyz( s_pw.statusLine, "Failed to create profile file.", sizeof( s_pw.statusLine ) );
        return qfalse;
    }
    Q_strncpyz( s_pw.profileName, name, sizeof( s_pw.profileName ) );
    UI_Profile_ActivateProfile( name );
    trap_Cvar_Set( "name", name );
    {
        profile_info_t  info;
        profile_stats_t stats;
        Com_Memset( &info,  0, sizeof( info  ) );
        Com_Memset( &stats, 0, sizeof( stats ) );
        if ( UI_Profile_ReadData( name, &info, &stats ) && info.uuid[0] ) {
            trap_Cvar_Set( "cl_uuid", info.uuid );
        }
    }
    s_pw.statusLine[0] = '\0';
    return qtrue;
}

static void PW_CommitDetails( void ) {
    profile_info_t  info;
    profile_stats_t stats;
    int avatarIdx  = s_pw.avatarSpin.curvalue;
    int countryIdx = s_pw.countrySpin.curvalue;
    int genderIdx  = s_pw.genderSpin.curvalue;
    int bDay       = s_pw.bDaySpin.curvalue;
    int bMonth     = s_pw.bMonthSpin.curvalue;
    int bYear      = s_pw.bYearSpin.curvalue;

    if ( !UI_Profile_ReadData( s_pw.profileName, &info, &stats ) ) {
        Com_Memset( &info,  0, sizeof( info  ) );
        Com_Memset( &stats, 0, sizeof( stats ) );
    }
    if ( genderIdx > 0 && genderIdx < PW_GENDER_COUNT && s_pwGenderItems[ genderIdx ] ) {
        Q_strncpyz( info.gender, s_pwGenderItems[ genderIdx ], sizeof( info.gender ) );
    } else {
        info.gender[0] = '\0';
    }
    if ( bDay > 0 && bMonth > 0 && bYear > 0 ) {
        int year = PW_BIRTH_YEAR_START + ( bYear - 1 );
        Com_sprintf( info.birthDate, sizeof( info.birthDate ), "%04d-%02d-%02d", year, bMonth, bDay );
    } else {
        info.birthDate[0] = '\0';
    }
    if ( avatarIdx > 0 && avatarIdx < PW_AVATAR_COUNT ) {
        Q_strncpyz( info.avatar, s_avatarShaderPaths[ avatarIdx ], sizeof( info.avatar ) );
    } else {
        info.avatar[0] = '\0';
    }
    if ( countryIdx > 0 && countryIdx < PW_COUNTRY_COUNT ) {
        Q_strncpyz( info.country, s_countryCodes[ countryIdx ], sizeof( info.country ) );
    } else {
        info.country[0] = '\0';
    }
    UI_Profile_WriteFile( s_pw.profileName, &info, &stats );
    UI_Profile_MarkStatsDirty();
}

static void PW_BuildOfflineServerName( const char *profileName, char *out, int outSize ) {
    char raw[128];
    int i, j;
    char c;
    Com_sprintf( raw, sizeof( raw ), "%s_OFFLINE", profileName );
    j = 0;
    for ( i = 0; raw[i] && j < outSize - 1; ++i ) {
        c = raw[i];
        if ( Q_IsColorString( &raw[i] ) ) { ++i; continue; }
        if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ||
             ( c >= '0' && c <= '9' ) ||
             c == '_' || c == '-' || c == '.' || c == ' ' ) {
            out[j++] = c;
        } else {
            out[j++] = '_';
        }
    }
    out[j] = '\0';
    if ( !out[0] ) Q_strncpyz( out, "q3rally_offline", outSize );
}

static void PW_StartLadderRegister( void ) {
    char uuid[ PROFILE_MAX_UUID ];
    char cmd[ 384 ];

    trap_Cvar_VariableStringBuffer( "cl_uuid", uuid, sizeof( uuid ) );
    if ( !uuid[0] ) {
        Q_strncpyz( s_pw.statusLine, "No UUID found. Please restart and try again.", sizeof( s_pw.statusLine ) );
        s_pw.ladderResult     = PW_RESULT_ERROR;
        s_pw.offlineKeyResult = PW_RESULT_ERROR;
        PW_UpdateButtons();
        return;
    }

    /* ladder_player_register (POST /players/register) is not yet implemented
     * in the engine — mark it OK immediately so it doesn't block the flow.
     * The offline server-key registration below is the functional step. */
    s_pw.ladderResult = PW_RESULT_OK;

    /* Register an offline server key via the existing ladder_register command.
     * ownerEmail uses the in-game placeholder recognised by register.php
     * (Accept: application/json path auto-approves offline keys).          */
    PW_BuildOfflineServerName( s_pw.profileName, s_pw.offlineServerName, sizeof( s_pw.offlineServerName ) );
    trap_Cvar_Set( "sv_ladderUrl", "https://ladder.q3rally.com/index.php/matches" );
    trap_Cvar_Set( "sv_hostname", s_pw.offlineServerName );

    Com_sprintf( cmd, sizeof( cmd ),
                 "ladder_register \"%s\" \"ingame@q3rally.com\" \"%s\" \"agree\"\n",
                 s_pw.profileName, s_pw.offlineServerName );
    trap_Cmd_ExecuteText( EXEC_APPEND, cmd );
    s_pw.offlineKeyResult = PW_RESULT_PENDING;

    s_pw.submitting = qtrue;
    Q_strncpyz( s_pw.statusLine, "Connecting to ladder...", sizeof( s_pw.statusLine ) );
    PW_UpdateButtons();
}

// ── Page transitions ──────────────────────────────────────────────────────────

static void PW_SetPage( pwPage_t page ) {
    s_pw.page          = page;
    s_pw.statusLine[0] = '\0';
    if ( page != PW_PAGE_LADDER ) {
        s_pw.ladderResult     = PW_RESULT_NONE;
        s_pw.offlineKeyResult = PW_RESULT_NONE;
        s_pw.submitting       = qfalse;
    }
    // Hide/show items before updating buttons so focus can't fall on
    // an item from a different page.
    PW_SetPageItems( page );
    PW_UpdateButtons();

    // Set keyboard focus to the first interactive item on this page.
    switch ( page ) {
    case PW_PAGE_NAME:
        Menu_SetCursorToItem( &s_pw.menu, &s_pw.nameField );
        break;
    case PW_PAGE_DETAILS:
        Menu_SetCursorToItem( &s_pw.menu, &s_pw.genderSpin );
        break;
    case PW_PAGE_LADDER:
    case PW_PAGE_DONE:
        Menu_SetCursorToItem( &s_pw.menu, &s_pw.btnNext );
        break;
    }

    // Load rotating vehicle for the Done page.
    if ( page == PW_PAGE_DONE ) {
        char model[ MAX_QPATH ];
        Com_Memset( &s_pw.doneVehicleInfo, 0, sizeof( s_pw.doneVehicleInfo ) );
        // Use the player's current car if set, else fall back to roadster/blue.
        trap_Cvar_VariableStringBuffer( "model", model, sizeof( model ) );
        if ( !model[0] ) {
            Q_strncpyz( model, "roadster/blue", sizeof( model ) );
        }
        UI_PlayerInfo_SetModel( &s_pw.doneVehicleInfo, model, NULL, NULL, NULL );
    }
}

static void PW_UpdateButtons( void ) {
    // Buttons start hidden; PW_SetPageItems already hid them.
    // Re-enable the appropriate subset here.
    switch ( s_pw.page ) {

    case PW_PAGE_NAME:
        s_pw.btnNext.string        = "NEXT";
        s_pw.btnNext.generic.flags = QMF_RIGHT_JUSTIFY | QMF_PULSEIFFOCUS;
        break;

    case PW_PAGE_DETAILS:
        s_pw.btnNext.string        = "NEXT";
        s_pw.btnNext.generic.flags = QMF_RIGHT_JUSTIFY  | QMF_PULSEIFFOCUS;
        s_pw.btnBack.string        = "BACK";
        s_pw.btnBack.generic.flags = QMF_LEFT_JUSTIFY   | QMF_PULSEIFFOCUS;
        s_pw.btnSkip.string        = "SKIP";
        s_pw.btnSkip.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
        break;

    case PW_PAGE_LADDER:
        if ( s_pw.submitting ) {
            s_pw.btnNext.string        = "JOIN LADDER";
            s_pw.btnNext.generic.flags = QMF_RIGHT_JUSTIFY  | QMF_INACTIVE;
            s_pw.btnBack.string        = "BACK";
            s_pw.btnBack.generic.flags = QMF_LEFT_JUSTIFY   | QMF_INACTIVE;
            s_pw.btnSkip.string        = "SKIP";
            s_pw.btnSkip.generic.flags = QMF_CENTER_JUSTIFY | QMF_INACTIVE;
        } else if ( s_pw.ladderResult     == PW_RESULT_OK &&
                    s_pw.offlineKeyResult == PW_RESULT_OK ) {
            s_pw.btnNext.string        = "DONE";
            s_pw.btnNext.generic.flags = QMF_RIGHT_JUSTIFY | QMF_PULSEIFFOCUS;
        } else if ( s_pw.ladderResult     == PW_RESULT_ERROR ||
                    s_pw.offlineKeyResult == PW_RESULT_ERROR ) {
            s_pw.btnNext.string        = "RETRY";
            s_pw.btnNext.generic.flags = QMF_RIGHT_JUSTIFY  | QMF_PULSEIFFOCUS;
            s_pw.btnBack.string        = "BACK";
            s_pw.btnBack.generic.flags = QMF_LEFT_JUSTIFY   | QMF_PULSEIFFOCUS;
            s_pw.btnSkip.string        = "SKIP";
            s_pw.btnSkip.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
        } else {
            s_pw.btnNext.string        = "JOIN LADDER";
            s_pw.btnNext.generic.flags = QMF_RIGHT_JUSTIFY  | QMF_PULSEIFFOCUS;
            s_pw.btnBack.string        = "BACK";
            s_pw.btnBack.generic.flags = QMF_LEFT_JUSTIFY   | QMF_PULSEIFFOCUS;
            s_pw.btnSkip.string        = "SKIP";
            s_pw.btnSkip.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
        }
        break;

    case PW_PAGE_DONE:
        s_pw.btnNext.string        = "START RACING";
        s_pw.btnNext.generic.flags = QMF_RIGHT_JUSTIFY | QMF_PULSEIFFOCUS;
        break;
    }

    UI_ReflowPTextBounds( &s_pw.btnNext );
    UI_ReflowPTextBounds( &s_pw.btnBack );
    UI_ReflowPTextBounds( &s_pw.btnSkip );

    /* The frontend buttons use generous, stable mouse hitboxes rather than
     * the narrow legacy proportional-text bounds. */
    s_pw.btnBack.generic.left = PW_BTN_BACK_X;
    s_pw.btnBack.generic.top = PW_BTN_Y;
    s_pw.btnBack.generic.right = PW_BTN_BACK_X + PW_BTN_WIDTH;
    s_pw.btnBack.generic.bottom = PW_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;
    s_pw.btnSkip.generic.left = PW_BTN_SKIP_X;
    s_pw.btnSkip.generic.top = PW_BTN_Y;
    s_pw.btnSkip.generic.right = PW_BTN_SKIP_X + PW_BTN_WIDTH;
    s_pw.btnSkip.generic.bottom = PW_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;
    s_pw.btnNext.generic.left = PW_BTN_NEXT_X;
    s_pw.btnNext.generic.top = PW_BTN_Y;
    s_pw.btnNext.generic.right = PW_BTN_NEXT_X + PW_BTN_WIDTH;
    s_pw.btnNext.generic.bottom = PW_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;
}

/* The choice rows are custom drawn across the full content width.  The
 * default SpinControl_Init bounds only cover the old value text, so mouse
 * clicks on the visible label/underline could miss the control entirely. */
static void PW_SetChoiceBounds( void ) {
    menulist_s *choices[] = {
        &s_pw.genderSpin,
        &s_pw.bDaySpin,
        &s_pw.bMonthSpin,
        &s_pw.bYearSpin,
        &s_pw.avatarSpin,
        &s_pw.countrySpin
    };
    int i;
    int right = PW_PANEL_X + PW_PANEL_W - PW_PAD;

    for ( i = 0; i < ARRAY_LEN( choices ); ++i ) {
        choices[i]->generic.left   = PW_P2_LABEL_X;
        choices[i]->generic.right  = right;
        choices[i]->generic.top    = choices[i]->generic.y;
        choices[i]->generic.bottom = choices[i]->generic.y + PW_P2_ROW_H - 1;
    }
}

// ── Draw ──────────────────────────────────────────────────────────────────────

static qboolean PW_ItemHasFocus( const menucommon_s *item ) {
    return ( item && item->parent &&
             Menu_ItemAtCursor( item->parent ) == item ) ? qtrue : qfalse;
}

static void PW_DrawField( void *self ) {
    menufield_s *field = (menufield_s *)self;
    qboolean focus = PW_ItemHasFocus( &field->generic );
    int valueX = field->generic.x + 116;
    int right = PW_PANEL_X + PW_PANEL_W - PW_PAD;
    vec4_t labelColor;
    vec4_t valueColor;

    Vector4Copy( focus ? pwAccent : pwMuted, labelColor );
    Vector4Copy( pwText, valueColor );
    Frontend_DrawText( field->generic.x, field->generic.y,
                       field->generic.name ? field->generic.name : "Name",
                       UI_LEFT | UI_SMALLFONT, labelColor );
    Frontend_DrawText( valueX, field->generic.y, field->field.buffer,
                       UI_LEFT | UI_SMALLFONT, valueColor );
    UI_FillRect( valueX, field->generic.y + 18, right - valueX, 2,
                 focus ? pwAccent : pwMuted );
    if ( focus ) {
        UI_DrawChar( valueX + field->field.cursor * SMALLCHAR_WIDTH,
                     field->generic.y, trap_Key_GetOverstrikeMode() ? 11 : 10,
                     UI_BLINK | UI_SMALLFONT, pwAccent );
    }
}

static void PW_DrawChoice( void *self ) {
    menulist_s *choice = (menulist_s *)self;
    qboolean focus = PW_ItemHasFocus( &choice->generic );
    const char *value = "Not specified";
    char buffer[96];
    int right = PW_PANEL_X + PW_PANEL_W - PW_PAD;
    vec4_t labelColor;
    vec4_t valueColor;

    if ( choice->itemnames && choice->curvalue >= 0 &&
         choice->curvalue < choice->numitems &&
         choice->itemnames[choice->curvalue] ) {
        value = choice->itemnames[choice->curvalue];
    }
    Com_sprintf( buffer, sizeof( buffer ), "< %s >", value );
    if ( choice == &s_pw.avatarSpin ) {
        right = PW_AVATAR_X - PW_PAD / 2;
    }

    Vector4Copy( focus ? pwAccent : pwMuted, labelColor );
    Vector4Copy( pwText, valueColor );
    Frontend_DrawText( PW_P2_LABEL_X, choice->generic.y,
                       choice->generic.name ? choice->generic.name : "Option",
                       UI_LEFT | UI_SMALLFONT, labelColor );
    Frontend_DrawText( right, choice->generic.y, buffer,
                       UI_RIGHT | UI_SMALLFONT, valueColor );
    UI_FillRect( PW_P2_LABEL_X, choice->generic.y + 18,
                 right - PW_P2_LABEL_X, 2, focus ? pwAccent : pwMuted );
}

static void PW_DrawButton( void *self ) {
    menutext_s *button = (menutext_s *)self;
    qboolean focus = PW_ItemHasFocus( &button->generic );
    qboolean active = !( button->generic.flags & QMF_INACTIVE );
    int align = ( button == &s_pw.btnSkip ) ? UI_CENTER : UI_LEFT;

    Frontend_DrawNavButton( button->generic.left, button->generic.top,
                            button->generic.right - button->generic.left,
                            button->generic.bottom - button->generic.top,
                            button->string ? button->string : "", 1.0f,
                            active && focus, align );
}

static void PW_DrawDots( void ) {
    int i;
    int totalW   = ( PW_PAGES - 1 ) * PW_DOT_SPACING + 6;
    int startX   = PW_CX - totalW / 2;
    int activePg = ( s_pw.page >= PW_PAGE_DONE ) ? PW_PAGES - 1 : (int)s_pw.page;
    for ( i = 0; i < PW_PAGES; ++i ) {
        int x = startX + i * PW_DOT_SPACING;
        vec4_t *col = ( i == activePg ) ? &pwDotOn : &pwDotOff;
        UI_FillRect( x, PW_DOT_Y, 6, 6, *col );
    }
}

static void PW_DrawPage1( void ) {
    Frontend_DrawText( PW_FRAME_X + 24, PW_TITLE_Y, "Create profile",
                       UI_LEFT | UI_BIGFONT, pwText );
    Frontend_DrawText( PW_FRAME_X + 24, PW_TITLE_Y + 26,
                       "Choose a display name for your profile",
                       UI_LEFT | UI_SMALLFONT, pwMuted );
    Frontend_DrawText( PW_CONTENT_X, PW_BODY_Y,
                       "Letters, numbers, _ - . only.",
                       UI_LEFT | UI_SMALLFONT, pwMuted );
    if ( s_pw.statusLine[0] ) {
        Frontend_DrawText( PW_CX, PW_STATUS_Y, s_pw.statusLine,
                           UI_CENTER | UI_SMALLFONT, pwError );
    }
}

static void PW_DrawPage2( void ) {
    Frontend_DrawText( PW_FRAME_X + 24, PW_TITLE_Y, "Personalise",
                       UI_LEFT | UI_BIGFONT, pwText );
    Frontend_DrawText( PW_FRAME_X + 24, PW_TITLE_Y + 26,
                       "Optional details for your driver profile",
                       UI_LEFT | UI_SMALLFONT, pwMuted );
    UI_FillRect( PW_AVATAR_X, PW_AVATAR_Y, PW_AVATAR_SIZE, PW_AVATAR_SIZE, pwAvatarBg );
    if ( s_pw.avatarShader ) {
        UI_DrawHandlePic( PW_AVATAR_X, PW_AVATAR_Y, PW_AVATAR_SIZE, PW_AVATAR_SIZE, s_pw.avatarShader );
    } else {
        Frontend_DrawText( PW_AVATAR_X + PW_AVATAR_SIZE / 2,
                           PW_AVATAR_Y + PW_AVATAR_SIZE / 2 - 4, "?",
                           UI_CENTER | UI_SMALLFONT, pwMuted );
    }
}

static void PW_DrawPage3( void ) {
    Frontend_DrawText( PW_FRAME_X + 24, PW_TITLE_Y, "Ladder tracking",
                       UI_LEFT | UI_BIGFONT, pwText );
    Frontend_DrawText( PW_FRAME_X + 24, PW_TITLE_Y + 26,
                       "Keep your offline results connected to your profile",
                       UI_LEFT | UI_SMALLFONT, pwMuted );

    if ( s_pw.ladderResult     == PW_RESULT_OK &&
         s_pw.offlineKeyResult == PW_RESULT_OK ) {
        Frontend_DrawText( PW_CX, PW_BODY_Y + 12, "You're on the ladder!",
                           UI_CENTER | UI_SMALLFONT, pwSuccess );
        Frontend_DrawText( PW_CX, PW_BODY_Y + 34,
                           "Profile and offline tracking are both active.",
                           UI_CENTER | UI_SMALLFONT, pwText );
        Frontend_DrawText( PW_CX, PW_BODY_Y + 52,
                           "Stats update automatically after each match.",
                           UI_CENTER | UI_SMALLFONT, pwMuted );
        return;
    }

    Frontend_DrawText( PW_CX, PW_BODY_Y + 12,
                       "Track your race results on the Q3Rally ladder.",
                       UI_CENTER | UI_SMALLFONT, pwText );
    Frontend_DrawText( PW_CX, PW_BODY_Y + 34,
                       "Online and offline matches. No password needed.",
                       UI_CENTER | UI_SMALLFONT, pwMuted );
    Frontend_DrawText( PW_CX, PW_BODY_Y + 54, "ladder.q3rally.com",
                       UI_CENTER | UI_SMALLFONT, pwAccent );

    if ( s_pw.submitting ) {
        Frontend_DrawText( PW_CX, PW_STATUS_Y, "Registering...",
                           UI_CENTER | UI_SMALLFONT, pwAccent );
    } else if ( ( s_pw.ladderResult     == PW_RESULT_ERROR ||
                  s_pw.offlineKeyResult == PW_RESULT_ERROR ) && s_pw.statusLine[0] ) {
        Frontend_DrawText( PW_CX, PW_STATUS_Y, s_pw.statusLine,
                           UI_CENTER | UI_SMALLFONT, pwError );
    }
}

static void PW_DrawPageDone( void ) {
    /* Vehicle preview — right half of panel, vertically centred in content area */
    int modelX = PW_PANEL_X + PW_PANEL_W / 2;
    int modelY = PW_PANEL_Y + 10;
    int modelW = PW_PANEL_W / 2 - 8;
    int modelH = PW_PANEL_H - PW_BTN_Y + PW_PANEL_Y + PW_PANEL_H - 60;
    /* clamp to a square that fits the right column */
    if ( modelH > modelW ) modelH = modelW;
    if ( s_pw.doneVehicleInfo.headModel ) {
        UI_DrawPlayer( modelX, modelY, modelW, modelH, &s_pw.doneVehicleInfo, uis.realtime );
    }

    /* Text — left column */
    Frontend_DrawText( PW_FRAME_X + 24, PW_TITLE_Y, "Profile ready",
                       UI_LEFT | UI_BIGFONT, pwText );
    Frontend_DrawText( PW_FRAME_X + 24, PW_TITLE_Y + 26,
                       "Your driver identity is ready to race",
                       UI_LEFT | UI_SMALLFONT, pwMuted );
    Frontend_DrawText( PW_CONTENT_X, PW_BODY_Y,
                       va( "Welcome, %s!", s_pw.profileName ),
                       UI_LEFT | UI_SMALLFONT, pwAccent );
    Frontend_DrawText( PW_CONTENT_X, PW_BODY_Y + 28,
                       "Start with PLAY OFFLINE for a first solo race.",
                       UI_LEFT | UI_SMALLFONT, pwText );
    Frontend_DrawText( PW_CONTENT_X, PW_BODY_Y + 52,
                       "Use CONFIG > CONTROLS to set your key bindings.",
                       UI_LEFT | UI_SMALLFONT, pwMuted );
    Frontend_DrawText( PW_CONTENT_X, PW_BODY_Y + 76,
                       "You can change profile details later in Settings.",
                       UI_LEFT | UI_SMALLFONT, pwMuted );
}

static void PW_Draw( void ) {
    const char *statusLabel;

    Frontend_DrawBackground( pwScrim );
    Frontend_DrawPanel( PW_FRAME_X, PW_FRAME_Y, PW_FRAME_W, PW_FRAME_H,
                        uis.tFrac, UI_FRONTEND_STYLE_FRAME );
    statusLabel = ( s_pw.page == PW_PAGE_LADDER ) ? "Ladder" : "Profile";
    Frontend_DrawStatusChip( PW_FRAME_X + PW_FRAME_W - 112,
                             PW_FRAME_Y + 26, statusLabel, pwStatus,
                             uis.tFrac );
    Frontend_DrawCard( PW_PANEL_X, PW_PANEL_Y, PW_PANEL_W, PW_PANEL_H,
                       uis.tFrac, qfalse );
    switch ( s_pw.page ) {
    case PW_PAGE_NAME:    PW_DrawPage1();    break;
    case PW_PAGE_DETAILS: PW_DrawPage2();    break;
    case PW_PAGE_LADDER:  PW_DrawPage3();    break;
    case PW_PAGE_DONE:    PW_DrawPageDone(); break;
    }
    PW_DrawDots();
    Menu_Draw( &s_pw.menu );
    Frontend_DrawText( PW_FRAME_X + 24, PW_HINT_Y,
                       "Enter select    Tab switch    Esc back",
                       UI_LEFT | UI_SMALLFONT, pwMuted );
}

// ── Event handler ─────────────────────────────────────────────────────────────

static void PW_MenuEvent( void *ptr, int event ) {
    menucommon_s *item = (menucommon_s *)ptr;

    if ( item->id == ID_PW_AVATAR && event == QM_ACTIVATED ) {
        PW_LoadAvatarShader( s_pw.avatarSpin.curvalue );
        return;
    }
    if ( event != QM_ACTIVATED ) return;

    switch ( item->id ) {

    case ID_PW_NEXT:
        switch ( s_pw.page ) {
        case PW_PAGE_NAME:
            if ( PW_CommitName() ) PW_SetPage( PW_PAGE_DETAILS );
            break;
        case PW_PAGE_DETAILS:
            PW_CommitDetails();
            PW_SetPage( PW_PAGE_LADDER );
            break;
        case PW_PAGE_LADDER:
            if ( s_pw.ladderResult     == PW_RESULT_OK &&
                 s_pw.offlineKeyResult == PW_RESULT_OK ) {
                PW_SetPage( PW_PAGE_DONE );
            } else {
                s_pw.ladderResult     = PW_RESULT_NONE;
                s_pw.offlineKeyResult = PW_RESULT_NONE;
                PW_StartLadderRegister();
            }
            break;
        case PW_PAGE_DONE:
            UI_PopMenu();
            break;
        }
        break;

    case ID_PW_BACK:
        switch ( s_pw.page ) {
        case PW_PAGE_DETAILS: PW_SetPage( PW_PAGE_NAME    ); break;
        case PW_PAGE_LADDER:  PW_SetPage( PW_PAGE_DETAILS ); break;
        default: break;
        }
        break;

    case ID_PW_SKIP:
        switch ( s_pw.page ) {
        case PW_PAGE_DETAILS:
            PW_CommitDetails();
            PW_SetPage( PW_PAGE_LADDER );
            break;
        case PW_PAGE_LADDER:
            PW_SetPage( PW_PAGE_DONE );
            break;
        default: break;
        }
        break;
    }
}

// ── Key handler ───────────────────────────────────────────────────────────────

static sfxHandle_t PW_MenuKey( int key ) {
    if ( key == K_ESCAPE ) return 0;  // Wizard is mandatory
    return Menu_DefaultKey( &s_pw.menu, key );
}

// ── Menu init ─────────────────────────────────────────────────────────────────

void UI_ProfileWizard_Show( void ) {
    int fieldX, fieldW;

    PW_BuildBirthDateLists();
    Com_Memset( &s_pw, 0, sizeof( s_pw ) );

    s_pw.page         = PW_PAGE_NAME;
    s_pw.ladderResult = PW_RESULT_NONE;

    s_pw.menu.draw        = PW_Draw;
    s_pw.menu.key         = PW_MenuKey;
    s_pw.menu.fullscreen  = qtrue;
    s_pw.menu.transparent = qfalse;
    s_pw.menu.wrapAround  = qtrue;
    s_pw.menu.showlogo    = qfalse;

    // ── Page 1: name field ────────────────────────────────────────────────────
    // Width is capped to the panel content area so the field never bleeds
    // beyond the panel border.
    fieldX = PW_CONTENT_X;
    fieldW = ( PW_CONTENT_W - 30 ) / SMALLCHAR_WIDTH;
    if ( fieldW > PROFILE_MAX_NAME - 1 ) fieldW = PROFILE_MAX_NAME - 1;
    if ( fieldW < 8 ) fieldW = 8;

    s_pw.nameField.generic.type       = MTYPE_FIELD;
    s_pw.nameField.generic.id         = ID_PW_NAME;
    s_pw.nameField.generic.name       = "Name";
    s_pw.nameField.generic.ownerdraw  = PW_DrawField;
    s_pw.nameField.generic.x          = fieldX;
    s_pw.nameField.generic.y          = PW_BODY_Y + 62;
    s_pw.nameField.field.widthInChars = fieldW;
    s_pw.nameField.field.maxchars     = PROFILE_MAX_NAME - 1;

    // ── Page 2: spin controls ─────────────────────────────────────────────────

    s_pw.genderSpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.genderSpin.generic.id       = ID_PW_GENDER;
    s_pw.genderSpin.generic.name     = "Gender";
    s_pw.genderSpin.generic.callback = PW_MenuEvent;
    s_pw.genderSpin.generic.ownerdraw = PW_DrawChoice;
    s_pw.genderSpin.generic.x        = PW_P2_SPIN_X;
    s_pw.genderSpin.generic.y        = PW_P2_ROW_1;
    s_pw.genderSpin.itemnames        = (const char **)s_pwGenderItems;
    s_pw.genderSpin.numitems         = PW_GENDER_COUNT;

    s_pw.bDaySpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.bDaySpin.generic.id       = ID_PW_BDAY;
    s_pw.bDaySpin.generic.name     = "Day";
    s_pw.bDaySpin.generic.callback = PW_MenuEvent;
    s_pw.bDaySpin.generic.ownerdraw = PW_DrawChoice;
    s_pw.bDaySpin.generic.x        = PW_P2_SPIN_X;
    s_pw.bDaySpin.generic.y        = PW_P2_ROW_2;
    s_pw.bDaySpin.itemnames        = (const char **)s_pwBirthDayItems;
    s_pw.bDaySpin.numitems         = PW_BIRTH_DAY_MAX + 1;

    s_pw.bMonthSpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.bMonthSpin.generic.id       = ID_PW_BMONTH;
    s_pw.bMonthSpin.generic.name     = "Month";
    s_pw.bMonthSpin.generic.callback = PW_MenuEvent;
    s_pw.bMonthSpin.generic.ownerdraw = PW_DrawChoice;
    s_pw.bMonthSpin.generic.x        = PW_P2_SPIN_X;
    s_pw.bMonthSpin.generic.y        = PW_P2_ROW_3;
    s_pw.bMonthSpin.itemnames        = (const char **)s_pwBirthMonthItems;
    s_pw.bMonthSpin.numitems         = PW_BIRTH_MONTH_COUNT;

    s_pw.bYearSpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.bYearSpin.generic.id       = ID_PW_BYEAR;
    s_pw.bYearSpin.generic.name     = "Year";
    s_pw.bYearSpin.generic.callback = PW_MenuEvent;
    s_pw.bYearSpin.generic.ownerdraw = PW_DrawChoice;
    s_pw.bYearSpin.generic.x        = PW_P2_SPIN_X;
    s_pw.bYearSpin.generic.y        = PW_P2_ROW_4;
    s_pw.bYearSpin.itemnames        = (const char **)s_pwBirthYearItems;
    s_pw.bYearSpin.numitems         = PW_BIRTH_YEAR_COUNT + 1;

    s_pw.avatarSpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.avatarSpin.generic.id       = ID_PW_AVATAR;
    s_pw.avatarSpin.generic.name     = "Avatar";
    s_pw.avatarSpin.generic.callback = PW_MenuEvent;
    s_pw.avatarSpin.generic.ownerdraw = PW_DrawChoice;
    s_pw.avatarSpin.generic.x        = PW_P2_SPIN_X;
    s_pw.avatarSpin.generic.y        = PW_P2_ROW_5;
    s_pw.avatarSpin.itemnames        = (const char **)s_avatarDisplayNames;
    s_pw.avatarSpin.numitems         = PW_AVATAR_COUNT;

    s_pw.countrySpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.countrySpin.generic.id       = ID_PW_COUNTRY;
    s_pw.countrySpin.generic.name     = "Country";
    s_pw.countrySpin.generic.callback = PW_MenuEvent;
    s_pw.countrySpin.generic.ownerdraw = PW_DrawChoice;
    s_pw.countrySpin.generic.x        = PW_P2_SPIN_X;
    s_pw.countrySpin.generic.y        = PW_P2_ROW_6;
    s_pw.countrySpin.itemnames        = (const char **)s_countryNames;
    s_pw.countrySpin.numitems         = PW_COUNTRY_COUNT;

    // ── Navigation buttons ────────────────────────────────────────────────────

    s_pw.btnNext.generic.type     = MTYPE_PTEXT;
    s_pw.btnNext.generic.id       = ID_PW_NEXT;
    s_pw.btnNext.generic.callback = PW_MenuEvent;
    s_pw.btnNext.generic.ownerdraw = PW_DrawButton;
    s_pw.btnNext.generic.x        = PW_BTN_NEXT_X;
    s_pw.btnNext.generic.y        = PW_BTN_Y;
    s_pw.btnNext.string           = "NEXT";
    s_pw.btnNext.style            = UI_RIGHT | UI_THEME_STYLE_BUTTON_FONT;
    s_pw.btnNext.color            = pwAccent;

    s_pw.btnBack.generic.type     = MTYPE_PTEXT;
    s_pw.btnBack.generic.id       = ID_PW_BACK;
    s_pw.btnBack.generic.callback = PW_MenuEvent;
    s_pw.btnBack.generic.ownerdraw = PW_DrawButton;
    s_pw.btnBack.generic.x        = PW_BTN_BACK_X;
    s_pw.btnBack.generic.y        = PW_BTN_Y;
    s_pw.btnBack.string           = "BACK";
    s_pw.btnBack.style            = UI_LEFT | UI_THEME_STYLE_BUTTON_FONT;
    s_pw.btnBack.color            = pwText;

    s_pw.btnSkip.generic.type     = MTYPE_PTEXT;
    s_pw.btnSkip.generic.id       = ID_PW_SKIP;
    s_pw.btnSkip.generic.callback = PW_MenuEvent;
    s_pw.btnSkip.generic.ownerdraw = PW_DrawButton;
    s_pw.btnSkip.generic.x        = PW_BTN_SKIP_X;
    s_pw.btnSkip.generic.y        = PW_BTN_Y;
    s_pw.btnSkip.string           = "SKIP";
    s_pw.btnSkip.style            = UI_CENTER | UI_THEME_STYLE_BUTTON_FONT;
    s_pw.btnSkip.color            = pwMuted;

    // ── Register all items ────────────────────────────────────────────────────
    // All items are registered but start hidden+inactive. PW_SetPage will
    // enable only the subset belonging to the initial page.

    Menu_AddItem( &s_pw.menu, &s_pw.nameField   );
    Menu_AddItem( &s_pw.menu, &s_pw.genderSpin  );
    Menu_AddItem( &s_pw.menu, &s_pw.bDaySpin    );
    Menu_AddItem( &s_pw.menu, &s_pw.bMonthSpin  );
    Menu_AddItem( &s_pw.menu, &s_pw.bYearSpin   );
    Menu_AddItem( &s_pw.menu, &s_pw.avatarSpin  );
    Menu_AddItem( &s_pw.menu, &s_pw.countrySpin );
    Menu_AddItem( &s_pw.menu, &s_pw.btnBack     );
    Menu_AddItem( &s_pw.menu, &s_pw.btnSkip     );
    Menu_AddItem( &s_pw.menu, &s_pw.btnNext     );

    /* Keep mouse hitboxes aligned with the custom full-width choice rows. */
    PW_SetChoiceBounds();

    s_pw.btnBack.generic.left = PW_BTN_BACK_X;
    s_pw.btnBack.generic.top = PW_BTN_Y;
    s_pw.btnBack.generic.right = PW_BTN_BACK_X + PW_BTN_WIDTH;
    s_pw.btnBack.generic.bottom = PW_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;
    s_pw.btnSkip.generic.left = PW_BTN_SKIP_X;
    s_pw.btnSkip.generic.top = PW_BTN_Y;
    s_pw.btnSkip.generic.right = PW_BTN_SKIP_X + PW_BTN_WIDTH;
    s_pw.btnSkip.generic.bottom = PW_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;
    s_pw.btnNext.generic.left = PW_BTN_NEXT_X;
    s_pw.btnNext.generic.top = PW_BTN_Y;
    s_pw.btnNext.generic.right = PW_BTN_NEXT_X + PW_BTN_WIDTH;
    s_pw.btnNext.generic.bottom = PW_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;

    uis.transitionIn  = 0;
    uis.transitionOut = 0;

    UI_PushMenu( &s_pw.menu );

    // Set initial page — this calls PW_SetPageItems + PW_UpdateButtons
    // and sets cursor focus correctly.
    PW_SetPage( PW_PAGE_NAME );
}

// ── Public interface ──────────────────────────────────────────────────────────

void UI_ProfileWizard_MaybeShow( void ) {
    char profileName[ PROFILE_MAX_NAME ];
    trap_Cvar_VariableStringBuffer( "profile_active", profileName, sizeof( profileName ) );
    if ( profileName[0] ) return;
    UI_ProfileWizard_Show();
}

qboolean UI_ProfileWizard_IsActive( void ) {
    int i;
    for ( i = 0; i < uis.menusp; ++i ) {
        if ( uis.stack[i] == &s_pw.menu ) return qtrue;
    }
    return ( uis.activemenu == &s_pw.menu ) ? qtrue : qfalse;
}

void UI_ProfileWizard_OnRegisterResult( qboolean success, const char *errorMsg ) {
    /* ladder_player_register is not yet implemented — this callback is never
     * called in the current build. Kept for future use. */
    s_pw.ladderResult = success ? PW_RESULT_OK : PW_RESULT_ERROR;
    if ( !success ) {
        Q_strncpyz( s_pw.statusLine,
                    ( errorMsg && errorMsg[0] ) ? errorMsg : "Could not reach the ladder. Try again later.",
                    sizeof( s_pw.statusLine ) );
    }
    s_pw.submitting = qfalse;
    if ( s_pw.ladderResult     == PW_RESULT_OK &&
         s_pw.offlineKeyResult == PW_RESULT_OK ) {
        UI_LadderWizard_MarkProfileRegistered();
        trap_Cvar_SetValue( "ladder_wizard_completed", 1 );
        PW_SetPage( PW_PAGE_DONE );
        return;
    }
    PW_UpdateButtons();
}

void UI_ProfileWizard_OnOfflineKeyResult( qboolean success, const char *errorMsg ) {
    s_pw.offlineKeyResult = success ? PW_RESULT_OK : PW_RESULT_ERROR;
    if ( !success && errorMsg && errorMsg[0] && !s_pw.statusLine[0] ) {
        Q_strncpyz( s_pw.statusLine, errorMsg, sizeof( s_pw.statusLine ) );
    }
    s_pw.submitting = qfalse;
    if ( s_pw.ladderResult     == PW_RESULT_OK &&
         s_pw.offlineKeyResult == PW_RESULT_OK ) {
        UI_LadderWizard_MarkProfileRegistered();
        trap_Cvar_SetValue( "ladder_wizard_completed", 1 );
        PW_SetPage( PW_PAGE_DONE );
        return;
    }
    PW_UpdateButtons();
}
