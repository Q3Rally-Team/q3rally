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
//   Page 2 — Gender, Birth date (Day/Month/Year), Avatar, Country
//             (all fields optional; page is skippable)
//   Page 3 — Ladder opt-in (optional) or skip → done
//
// The wizard writes the profile to disk at the end of Page 1 so Pages 2
// and 3 always have a valid profile to update. Page 2 updates the profile
// info fields. Page 3 fires "ladder_player_register <uuid> <n>" and
// waits for "ladder_player_register_result ok/err" via UI_ConsoleCommand.

#include "ui_local.h"

// ── Layout constants ──────────────────────────────────────────────────────────

#define PW_W            640
#define PW_H            480
#define PW_CX           ( PW_W / 2 )

// Panel — tall enough for all Page 2 fields
#define PW_PANEL_W      480
#define PW_PANEL_H      370
#define PW_PANEL_X      ( ( PW_W - PW_PANEL_W ) / 2 )
#define PW_PANEL_Y      ( ( PW_H - PW_PANEL_H ) / 2 )

// Content area inside panel
#define PW_PAD          28
#define PW_CONTENT_X    ( PW_PANEL_X + PW_PAD )
#define PW_CONTENT_W    ( PW_PANEL_W - PW_PAD * 2 )
#define PW_TITLE_Y      ( PW_PANEL_Y + 20 )
#define PW_BODY_Y       ( PW_PANEL_Y + 50 )

// Button row at bottom of panel
#define PW_BTN_Y        ( PW_PANEL_Y + PW_PANEL_H - 40 )
#define PW_BTN_NEXT_X   ( PW_PANEL_X + PW_PANEL_W - PW_PAD )
#define PW_BTN_BACK_X   ( PW_PANEL_X + PW_PAD )
#define PW_BTN_SKIP_X   ( PW_CX )

// Page 2 field layout — left column labels, right column spincontrols
#define PW_P2_LABEL_X   ( PW_PANEL_X + PW_PAD )
#define PW_P2_SPIN_X    ( PW_PANEL_X + 130 )
#define PW_P2_ROW_H     28
#define PW_P2_ROW_1     ( PW_BODY_Y + 14 )                  // Gender
#define PW_P2_ROW_2     ( PW_P2_ROW_1 + PW_P2_ROW_H )       // Birth day
#define PW_P2_ROW_3     ( PW_P2_ROW_2 + PW_P2_ROW_H )       // Birth month
#define PW_P2_ROW_4     ( PW_P2_ROW_3 + PW_P2_ROW_H )       // Birth year
#define PW_P2_ROW_5     ( PW_P2_ROW_4 + PW_P2_ROW_H + 8 )  // Avatar (extra gap)
#define PW_P2_ROW_6     ( PW_P2_ROW_5 + PW_P2_ROW_H )       // Country

// Avatar preview — right side, vertically centred on rows 5-6
#define PW_AVATAR_SIZE  48
#define PW_AVATAR_X     ( PW_PANEL_X + PW_PANEL_W - PW_PAD - PW_AVATAR_SIZE )
#define PW_AVATAR_Y     ( PW_P2_ROW_5 - 4 )

// Progress dots below panel
#define PW_DOT_Y        ( PW_PANEL_Y + PW_PANEL_H + 10 )
#define PW_DOT_SPACING  14
#define PW_PAGES        3

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

static vec4_t pwBg       = { 0.06f, 0.06f, 0.10f, 0.98f };
static vec4_t pwDim      = { 0.00f, 0.00f, 0.00f, 0.55f };
static vec4_t pwBorder   = { 0.32f, 0.44f, 0.72f, 0.55f };
static vec4_t pwTitle    = { 0.72f, 0.84f, 1.00f, 1.00f };
static vec4_t pwText     = { 0.74f, 0.78f, 0.88f, 1.00f };
static vec4_t pwMuted    = { 0.50f, 0.54f, 0.62f, 1.00f };
static vec4_t pwAccent   = { 0.48f, 0.66f, 1.00f, 1.00f };
static vec4_t pwSuccess  = { 0.40f, 0.85f, 0.55f, 1.00f };
static vec4_t pwError    = { 1.00f, 0.40f, 0.40f, 1.00f };
static vec4_t pwDotOn    = { 0.48f, 0.66f, 1.00f, 1.00f };
static vec4_t pwDotOff   = { 0.30f, 0.32f, 0.40f, 1.00f };
static vec4_t pwAvatarBg = { 0.08f, 0.08f, 0.14f, 1.00f };

// ── Gender list ───────────────────────────────────────────────────────────────
// Must stay in sync with s_genderItems in ui_playersettings.c.

static const char *s_pwGenderItems[] = {
    "Unspecified",
    "Female",
    "Male",
    "Non-binary",
    "Other",
    NULL
};
#define PW_GENDER_COUNT 5

// ── Birth date lists ──────────────────────────────────────────────────────────
// Mirror of the lists in ui_playersettings.c, local to this file.

#define PW_BIRTH_YEAR_START  1950
#define PW_BIRTH_YEAR_END    2100
#define PW_BIRTH_YEAR_COUNT  ( ( PW_BIRTH_YEAR_END ) - ( PW_BIRTH_YEAR_START ) + 1 )
#define PW_BIRTH_DAY_MAX     31

static const char *s_pwBirthMonthItems[] = {
    "-",
    "January",   "February", "March",    "April",
    "May",       "June",     "July",     "August",
    "September", "October",  "November", "December",
    NULL
};
#define PW_BIRTH_MONTH_COUNT 13   // "-" plus 12 months

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
        Com_sprintf( s_pwBirthDayStrings[i],
                     sizeof( s_pwBirthDayStrings[i] ), "%d", i );
        s_pwBirthDayItems[i] = s_pwBirthDayStrings[i];
    }
    s_pwBirthDayItems[ PW_BIRTH_DAY_MAX + 1 ] = NULL;

    s_pwBirthYearItems[0] = "-";
    for ( i = 0; i < PW_BIRTH_YEAR_COUNT; ++i ) {
        Com_sprintf( s_pwBirthYearStrings[i],
                     sizeof( s_pwBirthYearStrings[i] ),
                     "%d", PW_BIRTH_YEAR_START + i );
        s_pwBirthYearItems[ i + 1 ] = s_pwBirthYearStrings[i];
    }
    s_pwBirthYearItems[ PW_BIRTH_YEAR_COUNT + 1 ] = NULL;

    s_pwBirthListsBuilt = qtrue;
}

// ── Avatar list ───────────────────────────────────────────────────────────────

#define PW_AVATAR_COUNT 10

static const char *s_avatarShaderPaths[ PW_AVATAR_COUNT ] = {
    "",
    "gfx/avatars/preset/driver_01",
    "gfx/avatars/preset/driver_02",
    "gfx/avatars/preset/driver_03",
    "gfx/avatars/preset/driver_04",
    "gfx/avatars/preset/driver_05",
    "gfx/avatars/preset/driver_06",
    "gfx/avatars/preset/driver_07",
    "gfx/avatars/preset/driver_08",
    "gfx/avatars/preset/driver_09",
};

static const char *s_avatarDisplayNames[ PW_AVATAR_COUNT ] = {
    "None",
    "Driver 01", "Driver 02", "Driver 03",
    "Driver 04", "Driver 05", "Driver 06",
    "Driver 07", "Driver 08", "Driver 09",
};

// ── Country list ──────────────────────────────────────────────────────────────

#define PW_COUNTRY_COUNT 42

static const char *s_countryCodes[ PW_COUNTRY_COUNT ] = {
    "",
    "AT", "AU", "BE", "BR", "CA", "CH", "CN", "CZ", "DE",
    "DK", "ES", "FI", "FR", "GB", "GR", "HU", "ID", "IN",
    "IT", "JP", "KR", "MX", "NL", "NO", "NZ", "PL", "PT",
    "RO", "RU", "SE", "SG", "SK", "TH", "TR", "TW", "UA",
    "US", "VN", "ZA", "AR", "CL",
};

static const char *s_countryNames[ PW_COUNTRY_COUNT ] = {
    "Not specified",
    "Austria",        "Australia",     "Belgium",      "Brazil",
    "Canada",         "Switzerland",   "China",        "Czech Republic",
    "Germany",        "Denmark",       "Spain",        "Finland",
    "France",         "United Kingdom","Greece",       "Hungary",
    "Indonesia",      "India",         "Italy",        "Japan",
    "South Korea",    "Mexico",        "Netherlands",  "Norway",
    "New Zealand",    "Poland",        "Portugal",     "Romania",
    "Russia",         "Sweden",        "Singapore",    "Slovakia",
    "Thailand",       "Turkey",        "Taiwan",       "Ukraine",
    "United States",  "Vietnam",       "South Africa", "Argentina",
    "Chile",
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

    // Navigation buttons (repurposed per page)
    menutext_s      btnNext;
    menutext_s      btnBack;
    menutext_s      btnSkip;

    pwPage_t        page;
    pwResult_t      ladderResult;      // POST /players/register
    pwResult_t      offlineKeyResult;  // ladder_register (offline server key)

    char            profileName[ PROFILE_MAX_NAME ];
    char            offlineServerName[ 68 ];  // PLAYERNAME_OFFLINE, normalized
    char            statusLine[ 128 ];
    qboolean        submitting;

    // Avatar preview shader
    qhandle_t       avatarShader;
    int             avatarCurIdx;

} s_pw;

// ── Forward declarations ──────────────────────────────────────────────────────

static void PW_Draw( void );
static void PW_MenuEvent( void *ptr, int event );
static sfxHandle_t PW_MenuKey( int key );
static void PW_SetPage( pwPage_t page );
static void PW_UpdateButtons( void );
static void PW_LoadAvatarShader( int idx );

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
             c == '_' || c == '-' || c == '.' ) {
            continue;
        }
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
        Q_strncpyz( s_pw.statusLine,
                    "Name must use letters, numbers, _ - . only.",
                    sizeof( s_pw.statusLine ) );
        return qfalse;
    }

    if ( !UI_Profile_WriteDefaultFile( name ) ) {
        Q_strncpyz( s_pw.statusLine,
                    "Failed to create profile file.",
                    sizeof( s_pw.statusLine ) );
        return qfalse;
    }

    Q_strncpyz( s_pw.profileName, name, sizeof( s_pw.profileName ) );

    /* Activate via the same path as the profile overlay — this sets
     * uis.activeProfile, updates ui_profileActive, calls MainMenu_Prepare,
     * and marks stats dirty so cl_uuid gets read on the next data refresh. */
    UI_Profile_ActivateProfile( name );
    trap_Cvar_Set( "name", name );

    /* Read back the UUID immediately so Page 3 has it without waiting
     * for the next data refresh cycle. */
    {
        profile_info_t  info;
        profile_stats_t stats;
        Com_Memset( &info, 0, sizeof( info ) );
        Com_Memset( &stats, 0, sizeof( stats ) );
        if ( UI_Profile_ReadData( name, &info, &stats ) && info.uuid[0] ) {
            trap_Cvar_Set( "cl_uuid", info.uuid );
            trap_Print( va( "Q3Rally Profile: wizard UUID %s for '%s'\n",
                            info.uuid, name ) );
        } else {
            trap_Print( S_COLOR_YELLOW "Q3Rally Profile: WARNING – no UUID found after profile creation\n" );
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
    int bDay       = s_pw.bDaySpin.curvalue;   // 0 = "-", 1..31
    int bMonth     = s_pw.bMonthSpin.curvalue; // 0 = "-", 1..12
    int bYear      = s_pw.bYearSpin.curvalue;  // 0 = "-", else 1-based index

    if ( !UI_Profile_ReadData( s_pw.profileName, &info, &stats ) ) {
        Com_Memset( &info,  0, sizeof( info  ) );
        Com_Memset( &stats, 0, sizeof( stats ) );
    }

    // Gender — index 0 = "Unspecified" → store empty string
    if ( genderIdx > 0 && genderIdx < PW_GENDER_COUNT &&
         s_pwGenderItems[ genderIdx ] ) {
        Q_strncpyz( info.gender, s_pwGenderItems[ genderIdx ],
                    sizeof( info.gender ) );
    } else {
        info.gender[0] = '\0';
    }

    // Birth date — only write YYYY-MM-DD when all three fields are set
    if ( bDay > 0 && bMonth > 0 && bYear > 0 ) {
        int year = PW_BIRTH_YEAR_START + ( bYear - 1 );
        Com_sprintf( info.birthDate, sizeof( info.birthDate ),
                     "%04d-%02d-%02d", year, bMonth, bDay );
    } else {
        info.birthDate[0] = '\0';
    }

    // Avatar
    if ( avatarIdx > 0 && avatarIdx < PW_AVATAR_COUNT ) {
        Q_strncpyz( info.avatar, s_avatarShaderPaths[ avatarIdx ],
                    sizeof( info.avatar ) );
    } else {
        info.avatar[0] = '\0';
    }

    // Country
    if ( countryIdx > 0 && countryIdx < PW_COUNTRY_COUNT ) {
        Q_strncpyz( info.country, s_countryCodes[ countryIdx ],
                    sizeof( info.country ) );
    } else {
        info.country[0] = '\0';
    }

    UI_Profile_WriteFile( s_pw.profileName, &info, &stats );
    UI_Profile_MarkStatsDirty();
}

static void PW_BuildOfflineServerName( const char *profileName,
                                       char *out, int outSize ) {
    /* Mirror of LadderWizard_NormalizeServerName: strip color codes,
     * keep only safe chars, append _OFFLINE suffix. */
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

    if ( !out[0] ) {
        Q_strncpyz( out, "q3rally_offline", outSize );
    }
}

static void PW_StartLadderRegister( void ) {
    char uuid[ PROFILE_MAX_UUID ];
    char cmd[ 384 ];

    /* cl_uuid was set in PW_CommitName immediately after WriteDefaultFile. */
    trap_Cvar_VariableStringBuffer( "cl_uuid", uuid, sizeof( uuid ) );

    if ( !uuid[0] ) {
        Q_strncpyz( s_pw.statusLine,
                    "No UUID found. Please restart and try again.",
                    sizeof( s_pw.statusLine ) );
        s_pw.ladderResult     = PW_RESULT_ERROR;
        s_pw.offlineKeyResult = PW_RESULT_ERROR;
        PW_UpdateButtons();
        return;
    }

    /* ── 1. Player profile registration (POST /players/register) ── */
    Com_sprintf( cmd, sizeof( cmd ),
                 "ladder_player_register \"%s\" \"%s\"\n",
                 uuid, s_pw.profileName );
    trap_Cmd_ExecuteText( EXEC_APPEND, cmd );
    trap_Print( va( "Q3Rally Ladder: registering player UUID %s name '%s'\n",
                    uuid, s_pw.profileName ) );
    s_pw.ladderResult = PW_RESULT_PENDING;

    /* ── 2. Offline server key registration ── *
     * Use ladder_offline_register which posts JSON to /index.php/register
     * with type=offline — auto-approved, no e-mail required.          */
    PW_BuildOfflineServerName( s_pw.profileName,
                               s_pw.offlineServerName,
                               sizeof( s_pw.offlineServerName ) );

    trap_Cvar_Set( "sv_ladderEnabled", "1" );
    trap_Cvar_Set( "sv_ladderUrl",
                   "https://ladder.q3rally.com/index.php/matches" );
    trap_Cvar_Set( "sv_hostname", s_pw.offlineServerName );

    Com_sprintf( cmd, sizeof( cmd ),
                 "ladder_offline_register \"%s\" \"%s\"\n",
                 s_pw.profileName, s_pw.offlineServerName );
    trap_Cmd_ExecuteText( EXEC_APPEND, cmd );
    s_pw.offlineKeyResult = PW_RESULT_PENDING;

    s_pw.submitting = qtrue;
    Q_strncpyz( s_pw.statusLine, "Connecting to ladder...",
                sizeof( s_pw.statusLine ) );
    PW_UpdateButtons();
}

// ── Page transitions ──────────────────────────────────────────────────────────

static void PW_SetPage( pwPage_t page ) {
    s_pw.page             = page;
    s_pw.statusLine[0]    = '\0';
    if ( page != PW_PAGE_LADDER ) {
        /* Reset both results when leaving the ladder page */
        s_pw.ladderResult     = PW_RESULT_NONE;
        s_pw.offlineKeyResult = PW_RESULT_NONE;
        s_pw.submitting       = qfalse;
    }
    PW_UpdateButtons();
}

static void PW_UpdateButtons( void ) {
    s_pw.btnNext.generic.flags = QMF_INACTIVE | QMF_HIDDEN;
    s_pw.btnBack.generic.flags = QMF_INACTIVE | QMF_HIDDEN;
    s_pw.btnSkip.generic.flags = QMF_INACTIVE | QMF_HIDDEN;

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
            // Initial state — player hasn't clicked yet
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
}

// ── Draw ──────────────────────────────────────────────────────────────────────

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
    UI_DrawProportionalString( PW_CX, PW_TITLE_Y,
        "CREATE YOUR PROFILE",
        UI_CENTER | UI_SMALLFONT, pwTitle );

    UI_DrawString( PW_CX, PW_BODY_Y,
        "Choose a display name for your profile.",
        UI_CENTER | UI_SMALLFONT, pwText );
    UI_DrawString( PW_CX, PW_BODY_Y + 14,
        "Letters, numbers, _ - . only.",
        UI_CENTER | UI_SMALLFONT, pwMuted );

    UI_DrawString( PW_CONTENT_X, PW_BODY_Y + 44,
        "Name:", UI_LEFT | UI_SMALLFONT, pwText );

    if ( s_pw.statusLine[0] ) {
        UI_DrawString( PW_CX, PW_BTN_Y - 18,
            s_pw.statusLine,
            UI_CENTER | UI_SMALLFONT, pwError );
    }
}

static void PW_DrawPage2( void ) {
    UI_DrawProportionalString( PW_CX, PW_TITLE_Y,
        "PERSONALISE",
        UI_CENTER | UI_SMALLFONT, pwTitle );

    UI_DrawString( PW_CX, PW_BODY_Y,
        "All fields are optional. You can change them later in Settings.",
        UI_CENTER | UI_SMALLFONT, pwMuted );

    // Row labels (left column)
    UI_DrawString( PW_P2_LABEL_X, PW_P2_ROW_1 + 2,
        "Gender:",    UI_LEFT | UI_SMALLFONT, pwText );
    UI_DrawString( PW_P2_LABEL_X, PW_P2_ROW_2 + 2,
        "Birth day:", UI_LEFT | UI_SMALLFONT, pwText );
    UI_DrawString( PW_P2_LABEL_X, PW_P2_ROW_3 + 2,
        "Month:",     UI_LEFT | UI_SMALLFONT, pwText );
    UI_DrawString( PW_P2_LABEL_X, PW_P2_ROW_4 + 2,
        "Year:",      UI_LEFT | UI_SMALLFONT, pwText );
    UI_DrawString( PW_P2_LABEL_X, PW_P2_ROW_5 + 2,
        "Avatar:",    UI_LEFT | UI_SMALLFONT, pwText );
    UI_DrawString( PW_P2_LABEL_X, PW_P2_ROW_6 + 2,
        "Country:",   UI_LEFT | UI_SMALLFONT, pwText );

    // Avatar preview box — right side, rows 5-6
    UI_FillRect( PW_AVATAR_X, PW_AVATAR_Y,
                 PW_AVATAR_SIZE, PW_AVATAR_SIZE, pwAvatarBg );
    if ( s_pw.avatarShader ) {
        UI_DrawHandlePic( PW_AVATAR_X, PW_AVATAR_Y,
                          PW_AVATAR_SIZE, PW_AVATAR_SIZE,
                          s_pw.avatarShader );
    } else {
        UI_DrawString( PW_AVATAR_X + PW_AVATAR_SIZE / 2,
                       PW_AVATAR_Y + PW_AVATAR_SIZE / 2 - 4,
                       "?", UI_CENTER | UI_SMALLFONT, pwMuted );
    }
}

static void PW_DrawPage3( void ) {
    UI_DrawProportionalString( PW_CX, PW_TITLE_Y,
        "Q3RALLY LADDER",
        UI_CENTER | UI_SMALLFONT, pwTitle );

    if ( s_pw.ladderResult     == PW_RESULT_OK &&
         s_pw.offlineKeyResult == PW_RESULT_OK ) {
        UI_DrawString( PW_CX, PW_BODY_Y,
            "You're on the ladder!",
            UI_CENTER | UI_SMALLFONT, pwSuccess );
        UI_DrawString( PW_CX, PW_BODY_Y + 18,
            "Profile and offline tracking are both active.",
            UI_CENTER | UI_SMALLFONT, pwText );
        UI_DrawString( PW_CX, PW_BODY_Y + 34,
            "Stats update automatically after each match.",
            UI_CENTER | UI_SMALLFONT, pwText );
        return;
    }

    UI_DrawString( PW_CX, PW_BODY_Y,
        "Track your stats on the Q3Rally online ladder.",
        UI_CENTER | UI_SMALLFONT, pwText );
    UI_DrawString( PW_CX, PW_BODY_Y + 18,
        "Includes online and offline matches. No password needed.",
        UI_CENTER | UI_SMALLFONT, pwMuted );

    if ( s_pw.submitting ) {
        UI_DrawString( PW_CX, PW_BTN_Y - 22,
            "Registering...",
            UI_CENTER | UI_SMALLFONT, pwAccent );
    } else if ( ( s_pw.ladderResult     == PW_RESULT_ERROR ||
                  s_pw.offlineKeyResult == PW_RESULT_ERROR ) &&
                s_pw.statusLine[0] ) {
        UI_DrawString( PW_CX, PW_BTN_Y - 22,
            s_pw.statusLine,
            UI_CENTER | UI_SMALLFONT, pwError );
    }
}

static void PW_DrawPageDone( void ) {
    UI_DrawProportionalString( PW_CX, PW_TITLE_Y,
        "YOU'RE ALL SET",
        UI_CENTER | UI_SMALLFONT, pwTitle );
    UI_DrawString( PW_CX, PW_BODY_Y,
        va( "Welcome, %s!", s_pw.profileName ),
        UI_CENTER | UI_SMALLFONT, pwAccent );
    UI_DrawString( PW_CX, PW_BODY_Y + 22,
        "Your profile is ready. Settings can be changed",
        UI_CENTER | UI_SMALLFONT, pwText );
    UI_DrawString( PW_CX, PW_BODY_Y + 36,
        "at any time in Config > Player > Profile.",
        UI_CENTER | UI_SMALLFONT, pwText );
}

static void PW_Draw( void ) {
    UI_FillRect( 0, 0, PW_W, PW_H, pwDim );
    UI_FillRect( PW_PANEL_X, PW_PANEL_Y, PW_PANEL_W, PW_PANEL_H, pwBg );
    UI_DrawRect( PW_PANEL_X, PW_PANEL_Y, PW_PANEL_W, PW_PANEL_H, pwBorder );

    switch ( s_pw.page ) {
    case PW_PAGE_NAME:    PW_DrawPage1();    break;
    case PW_PAGE_DETAILS: PW_DrawPage2();    break;
    case PW_PAGE_LADDER:  PW_DrawPage3();    break;
    case PW_PAGE_DONE:    PW_DrawPageDone(); break;
    }

    PW_DrawDots();
    Menu_Draw( &s_pw.menu );
}

// ── Event handler ─────────────────────────────────────────────────────────────

static void PW_MenuEvent( void *ptr, int event ) {
    menucommon_s *item = (menucommon_s *)ptr;

    // Avatar spin changed — update preview immediately
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
                /* RETRY — reset both and fire again */
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
            PW_CommitDetails();         // write whatever the player set so far
            PW_SetPage( PW_PAGE_LADDER );
            break;
        case PW_PAGE_LADDER:
            PW_SetPage( PW_PAGE_DONE ); // skip ladder registration entirely
            break;
        default: break;
        }
        break;
    }
}

// ── Key handler ───────────────────────────────────────────────────────────────

static sfxHandle_t PW_MenuKey( int key ) {
    if ( key == K_ESCAPE ) {
        // Wizard is mandatory — Escape intentionally blocked
        return 0;
    }
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

    fieldX = PW_CONTENT_X;
    fieldW = ( PW_CONTENT_W - SMALLCHAR_WIDTH * 2 ) / SMALLCHAR_WIDTH;
    if ( fieldW < 8 ) fieldW = 8;

    s_pw.nameField.generic.type       = MTYPE_FIELD;
    s_pw.nameField.generic.flags      = QMF_SMALLFONT;
    s_pw.nameField.generic.id         = ID_PW_NAME;
    s_pw.nameField.generic.x          = fieldX;
    s_pw.nameField.generic.y          = PW_BODY_Y + 62;
    s_pw.nameField.field.widthInChars = fieldW;
    s_pw.nameField.field.maxchars     = PROFILE_MAX_NAME - 1;

    // ── Page 2: gender ────────────────────────────────────────────────────────

    s_pw.genderSpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.genderSpin.generic.flags    = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
    s_pw.genderSpin.generic.id       = ID_PW_GENDER;
    s_pw.genderSpin.generic.callback = PW_MenuEvent;
    s_pw.genderSpin.generic.x        = PW_P2_SPIN_X;
    s_pw.genderSpin.generic.y        = PW_P2_ROW_1;
    s_pw.genderSpin.itemnames        = (const char **)s_pwGenderItems;
    s_pw.genderSpin.numitems         = PW_GENDER_COUNT;
    s_pw.genderSpin.curvalue         = 0;

    // ── Page 2: birth day ─────────────────────────────────────────────────────

    s_pw.bDaySpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.bDaySpin.generic.flags    = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
    s_pw.bDaySpin.generic.id       = ID_PW_BDAY;
    s_pw.bDaySpin.generic.callback = PW_MenuEvent;
    s_pw.bDaySpin.generic.x        = PW_P2_SPIN_X;
    s_pw.bDaySpin.generic.y        = PW_P2_ROW_2;
    s_pw.bDaySpin.itemnames        = (const char **)s_pwBirthDayItems;
    s_pw.bDaySpin.numitems         = PW_BIRTH_DAY_MAX + 1;  // index 0="-", 1..31
    s_pw.bDaySpin.curvalue         = 0;

    // ── Page 2: birth month ───────────────────────────────────────────────────

    s_pw.bMonthSpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.bMonthSpin.generic.flags    = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
    s_pw.bMonthSpin.generic.id       = ID_PW_BMONTH;
    s_pw.bMonthSpin.generic.callback = PW_MenuEvent;
    s_pw.bMonthSpin.generic.x        = PW_P2_SPIN_X;
    s_pw.bMonthSpin.generic.y        = PW_P2_ROW_3;
    s_pw.bMonthSpin.itemnames        = (const char **)s_pwBirthMonthItems;
    s_pw.bMonthSpin.numitems         = PW_BIRTH_MONTH_COUNT;
    s_pw.bMonthSpin.curvalue         = 0;

    // ── Page 2: birth year ────────────────────────────────────────────────────

    s_pw.bYearSpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.bYearSpin.generic.flags    = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
    s_pw.bYearSpin.generic.id       = ID_PW_BYEAR;
    s_pw.bYearSpin.generic.callback = PW_MenuEvent;
    s_pw.bYearSpin.generic.x        = PW_P2_SPIN_X;
    s_pw.bYearSpin.generic.y        = PW_P2_ROW_4;
    s_pw.bYearSpin.itemnames        = (const char **)s_pwBirthYearItems;
    s_pw.bYearSpin.numitems         = PW_BIRTH_YEAR_COUNT + 1;  // 0="-", rest=years
    s_pw.bYearSpin.curvalue         = 0;

    // ── Page 2: avatar ────────────────────────────────────────────────────────

    s_pw.avatarSpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.avatarSpin.generic.flags    = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
    s_pw.avatarSpin.generic.id       = ID_PW_AVATAR;
    s_pw.avatarSpin.generic.callback = PW_MenuEvent;
    s_pw.avatarSpin.generic.x        = PW_P2_SPIN_X;
    s_pw.avatarSpin.generic.y        = PW_P2_ROW_5;
    s_pw.avatarSpin.itemnames        = (const char **)s_avatarDisplayNames;
    s_pw.avatarSpin.numitems         = PW_AVATAR_COUNT;
    s_pw.avatarSpin.curvalue         = 0;

    // ── Page 2: country ───────────────────────────────────────────────────────

    s_pw.countrySpin.generic.type     = MTYPE_SPINCONTROL;
    s_pw.countrySpin.generic.flags    = QMF_SMALLFONT | QMF_PULSEIFFOCUS;
    s_pw.countrySpin.generic.id       = ID_PW_COUNTRY;
    s_pw.countrySpin.generic.callback = PW_MenuEvent;
    s_pw.countrySpin.generic.x        = PW_P2_SPIN_X;
    s_pw.countrySpin.generic.y        = PW_P2_ROW_6;
    s_pw.countrySpin.itemnames        = (const char **)s_countryNames;
    s_pw.countrySpin.numitems         = PW_COUNTRY_COUNT;
    s_pw.countrySpin.curvalue         = 0;

    // ── Navigation buttons ────────────────────────────────────────────────────

    s_pw.btnNext.generic.type     = MTYPE_PTEXT;
    s_pw.btnNext.generic.id       = ID_PW_NEXT;
    s_pw.btnNext.generic.callback = PW_MenuEvent;
    s_pw.btnNext.generic.x        = PW_BTN_NEXT_X;
    s_pw.btnNext.generic.y        = PW_BTN_Y;
    s_pw.btnNext.string           = "NEXT";
    s_pw.btnNext.style            = UI_RIGHT | UI_SMALLFONT;
    s_pw.btnNext.color            = pwAccent;

    s_pw.btnBack.generic.type     = MTYPE_PTEXT;
    s_pw.btnBack.generic.id       = ID_PW_BACK;
    s_pw.btnBack.generic.callback = PW_MenuEvent;
    s_pw.btnBack.generic.x        = PW_BTN_BACK_X;
    s_pw.btnBack.generic.y        = PW_BTN_Y;
    s_pw.btnBack.string           = "BACK";
    s_pw.btnBack.style            = UI_LEFT | UI_SMALLFONT;
    s_pw.btnBack.color            = pwText;

    s_pw.btnSkip.generic.type     = MTYPE_PTEXT;
    s_pw.btnSkip.generic.id       = ID_PW_SKIP;
    s_pw.btnSkip.generic.callback = PW_MenuEvent;
    s_pw.btnSkip.generic.x        = PW_BTN_SKIP_X;
    s_pw.btnSkip.generic.y        = PW_BTN_Y;
    s_pw.btnSkip.string           = "SKIP";
    s_pw.btnSkip.style            = UI_CENTER | UI_SMALLFONT;
    s_pw.btnSkip.color            = pwMuted;

    // ── Register all items ────────────────────────────────────────────────────

    Menu_AddItem( &s_pw.menu, &s_pw.nameField   );
    Menu_AddItem( &s_pw.menu, &s_pw.genderSpin  );
    Menu_AddItem( &s_pw.menu, &s_pw.bDaySpin    );
    Menu_AddItem( &s_pw.menu, &s_pw.bMonthSpin  );
    Menu_AddItem( &s_pw.menu, &s_pw.bYearSpin   );
    Menu_AddItem( &s_pw.menu, &s_pw.avatarSpin  );
    Menu_AddItem( &s_pw.menu, &s_pw.countrySpin );
    Menu_AddItem( &s_pw.menu, &s_pw.btnNext     );
    Menu_AddItem( &s_pw.menu, &s_pw.btnBack     );
    Menu_AddItem( &s_pw.menu, &s_pw.btnSkip     );

    PW_UpdateButtons();
    Menu_SetCursorToItem( &s_pw.menu, &s_pw.nameField );

    uis.transitionIn  = 0;
    uis.transitionOut = 0;

    UI_PushMenu( &s_pw.menu );
}

// ── Public interface ──────────────────────────────────────────────────────────

void UI_ProfileWizard_MaybeShow( void ) {
    char profileName[ PROFILE_MAX_NAME ];
    trap_Cvar_VariableStringBuffer( "profile_active", profileName,
                                    sizeof( profileName ) );
    if ( profileName[0] ) return;
    UI_ProfileWizard_Show();
}

qboolean UI_ProfileWizard_IsActive( void ) {
    /* The wizard is active if its menu is anywhere in the menu stack. */
    int i;
    for ( i = 0; i < uis.menusp; ++i ) {
        if ( uis.stack[i] == &s_pw.menu ) {
            return qtrue;
        }
    }
    /* Also check the top-level active menu */
    return ( uis.activemenu == &s_pw.menu ) ? qtrue : qfalse;
}

void UI_ProfileWizard_OnRegisterResult( qboolean success, const char *errorMsg ) {
    s_pw.ladderResult = success ? PW_RESULT_OK : PW_RESULT_ERROR;

    if ( !success ) {
        if ( errorMsg && errorMsg[0] ) {
            Q_strncpyz( s_pw.statusLine, errorMsg, sizeof( s_pw.statusLine ) );
        } else {
            Q_strncpyz( s_pw.statusLine,
                        "Could not reach the ladder. Try again later.",
                        sizeof( s_pw.statusLine ) );
        }
    }

    /* Stop spinning as soon as either result arrives — don't wait for both.
     * The buttons will show RETRY if anything failed. */
    s_pw.submitting = qfalse;

    if ( s_pw.ladderResult     == PW_RESULT_OK &&
         s_pw.offlineKeyResult == PW_RESULT_OK ) {
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

    /* Same as above — stop spinning immediately */
    s_pw.submitting = qfalse;

    if ( s_pw.ladderResult     == PW_RESULT_OK &&
         s_pw.offlineKeyResult == PW_RESULT_OK ) {
        trap_Cvar_SetValue( "ladder_wizard_completed", 1 );
        PW_SetPage( PW_PAGE_DONE );
        return;
    }

    PW_UpdateButtons();
}
