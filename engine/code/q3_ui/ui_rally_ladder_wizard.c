/*
===========================================================================
Copyright (C) 2002-2026 Q3Rally Team
===========================================================================
*/
// ui_rally_ladder_wizard.c – Offline tracking registration wizard

#include "ui_local.h"
#include "ui_rally_frontend.h"

#define WIZARD_SCREEN_W     640
#define WIZARD_SCREEN_H     480
#define WIZARD_FRAME_X      48
#define WIZARD_FRAME_Y      28
#define WIZARD_FRAME_W      544
#define WIZARD_FRAME_H      420
#define WIZARD_PANEL_W      496
#define WIZARD_PANEL_H      270
#define WIZARD_PANEL_X      72
#define WIZARD_PANEL_Y      112
#define WIZARD_BTN_Y        388
#define WIZARD_BTN_WIDTH    120
#define WIZARD_BTN_CANCEL_X 64
#define WIZARD_BTN_REGISTER_X 256
#define WIZARD_BTN_NEVER_X  448

#define ID_WIZARD_YES       10
#define ID_WIZARD_NO        11
#define ID_WIZARD_NEVER     14
#define ID_OWNER_NAME       12
#define ID_OWNER_EMAIL      13

typedef enum { WIZARD_PAGE_CONFIRM = 0, WIZARD_PAGE_DONE } wizardPage_t;
typedef enum {
    WIZARD_RESULT_NONE = 0,
    WIZARD_RESULT_PENDING,
    WIZARD_RESULT_SUCCESS,
    WIZARD_RESULT_ERROR
} wizardResult_t;

static struct {
    menuframework_s menu;
    menufield_s     ownerName;
    menufield_s     ownerEmail;
    menutext_s      btnYes;
    menutext_s      btnNo;
    menutext_s      btnNever;
    wizardPage_t    page;
    wizardResult_t  result;
    char            playerName[PROFILE_MAX_NAME];
    char            serverName[65];
    char            serverNameCompareKey[65];
    char            serverNameNotice[128];
    char            statusLine[128];
    qboolean        submitting;
} s_wizard;

static vec4_t wizardScrim  = UI_FRONTEND_COLOR_SCRIM;
static vec4_t wizardText   = UI_FRONTEND_COLOR_TEXT;
static vec4_t wizardMuted  = UI_FRONTEND_COLOR_MUTED;
static vec4_t wizardAccent = UI_FRONTEND_COLOR_ACCENT;
static vec4_t wizardStatus = UI_FRONTEND_COLOR_STATUS;
static vec4_t wizardError  = UI_FRONTEND_COLOR_STATUS;

static void LadderWizard_MenuEvent( void *ptr, int event );
static void LadderWizard_Draw( void );
static void LadderWizard_UpdateButtons( void );
static sfxHandle_t LadderWizard_MenuKey( int key );
static void LadderWizard_FinishSubmission( wizardResult_t result, const char *statusLine );
static void LadderWizard_ComputeFormLayout( int *contentLeft,
                                            int *contentRight,
                                            int *fieldX,
                                            int *fieldWidthChars );
static void LadderWizard_DrawField( void *self );
static void LadderWizard_DrawButton( void *self );

/* ── Localized UI strings (English default) ─────────────────────────────────── */
#define WIZARD_TEXT_REGISTER                 "REGISTER"
#define WIZARD_TEXT_CANCEL                   "CANCEL"
#define WIZARD_TEXT_ABORT                    "ABORT"
#define WIZARD_TEXT_NEVER_SHOW               "NEVER SHOW AGAIN"
#define WIZARD_TEXT_SERVERNAME_TRUNCATED     "Note: Server name was truncated to 64 characters."
#define WIZARD_TEXT_SERVERNAME_NORMALIZED    "Note: Server name was normalized for the service."
#define WIZARD_TEXT_CONFIRM_QUESTION         "Do you want to track offline matches on the ladder?"
#define WIZARD_TEXT_OWNER_LABEL              "Owner:"
#define WIZARD_TEXT_EMAIL_LABEL              "Email:"
#define WIZARD_TEXT_SUBMITTING               "Submitting... please wait."
#define WIZARD_TEXT_CANCEL_HINT              "Cancel only closes this dialog."
#define WIZARD_TEXT_NEVER_HINT               "\"Never show again\" suppresses it permanently."
#define WIZARD_TEXT_SUCCESS                  "Registration successful!"
#define WIZARD_TEXT_PENDING_APPROVAL         "Offline profile pending ladder admin approval."
#define WIZARD_TEXT_FAILED                   "Registration failed."
#define WIZARD_TEXT_FAILED_HINT              "Please check data/connection and try again."
#define WIZARD_TEXT_DISMISSED_LOG            "Ladder wizard permanently dismissed via 'Never show again'.\n"

#define WIZARD_CONTENT_PAD_X        24
#define WIZARD_FORM_TOP_Y           ( WIZARD_PANEL_Y + 116 )
#define WIZARD_FORM_ROW_H           28
#define WIZARD_FORM_LABEL_OFFSET_Y  -12
#define WIZARD_FIELD_SAFE_PAD_PX    ( SMALLCHAR_WIDTH * 2 )
#define WIZARD_FIELD_MIN_CHARS      10

/* ── CVar registration ───────────────────────────────────────────────────────── */

static vmCvar_t ui_ladderWizardDismissed;
static vmCvar_t ui_ladderWizardCompleted;
static vmCvar_t ui_ladderWizardProfiles;
static vmCvar_t ui_ladderWizardDismissedProfiles;

static void LadderWizard_RegisterCvars( void ) {
    trap_Cvar_Register( &ui_ladderWizardDismissed,
                        "ladder_wizard_dismissed", "0",
                        CVAR_ARCHIVE | CVAR_USERINFO );
    trap_Cvar_Register( &ui_ladderWizardCompleted,
                        "ladder_wizard_completed", "0",
                        CVAR_ARCHIVE | CVAR_USERINFO );
    trap_Cvar_Register( &ui_ladderWizardProfiles,
                        "ladder_wizard_profiles", "",
                        CVAR_ARCHIVE );
    trap_Cvar_Register( &ui_ladderWizardDismissedProfiles,
                        "ladder_wizard_dismissed_profiles", "",
                        CVAR_ARCHIVE );
}

static qboolean LadderWizard_ValidateEmail( const char *email ) {
    const char *at;

    if ( !email || !email[0] ) {
        return qfalse;
    }

    at = strchr( email, '@' );
    if ( !at || at == email || !at[1] ) {
        return qfalse;
    }

    if ( !strchr( at + 1, '.' ) ) {
        return qfalse;
    }

    return qtrue;
}

static qboolean LadderWizard_IsWhitespace( char c ) {
    return ( c == ' ' || c == '\t' || c == '\n' || c == '\r' );
}

static void LadderWizard_ComputeFormLayout( int *contentLeft,
                                            int *contentRight,
                                            int *fieldX,
                                            int *fieldWidthChars ) {
    int left;
    int right;
    int x;
    int usableWidth;
    int widthChars;
    int fieldRight;

    left = WIZARD_PANEL_X + WIZARD_CONTENT_PAD_X;
    right = WIZARD_PANEL_X + WIZARD_PANEL_W - WIZARD_CONTENT_PAD_X;
    x = left;

    usableWidth = right - left - WIZARD_FIELD_SAFE_PAD_PX;
    widthChars = usableWidth / SMALLCHAR_WIDTH;
    if ( widthChars < WIZARD_FIELD_MIN_CHARS ) {
        widthChars = WIZARD_FIELD_MIN_CHARS;
    }

    fieldRight = x + widthChars * SMALLCHAR_WIDTH;
    if ( fieldRight > right ) {
        widthChars = ( right - x ) / SMALLCHAR_WIDTH;
        if ( widthChars < 1 ) {
            widthChars = 1;
        }
    }

    if ( contentLeft ) {
        *contentLeft = left;
    }
    if ( contentRight ) {
        *contentRight = right;
    }
    if ( fieldX ) {
        *fieldX = x;
    }
    if ( fieldWidthChars ) {
        *fieldWidthChars = widthChars;
    }
}

static qboolean LadderWizard_ItemHasFocus( const menucommon_s *item ) {
    return ( item && item->parent &&
             Menu_ItemAtCursor( item->parent ) == item ) ? qtrue : qfalse;
}

static void LadderWizard_DrawField( void *self ) {
    menufield_s *field = (menufield_s *)self;
    qboolean focus = LadderWizard_ItemHasFocus( &field->generic );
    int valueX = field->generic.x + 112;
    int right = WIZARD_PANEL_X + WIZARD_PANEL_W - WIZARD_CONTENT_PAD_X;
    vec4_t labelColor;
    vec4_t valueColor;

    Vector4Copy( focus ? wizardAccent : wizardMuted, labelColor );
    Vector4Copy( wizardText, valueColor );
    Frontend_DrawText( field->generic.x, field->generic.y,
                       field->generic.name ? field->generic.name : "Field",
                       UI_LEFT | UI_SMALLFONT, labelColor );
    Frontend_DrawText( valueX, field->generic.y, field->field.buffer,
                       UI_LEFT | UI_SMALLFONT, valueColor );
    UI_FillRect( valueX, field->generic.y + 18, right - valueX, 2,
                 focus ? wizardAccent : wizardMuted );
    if ( focus ) {
        UI_DrawChar( valueX + field->field.cursor * SMALLCHAR_WIDTH,
                     field->generic.y, trap_Key_GetOverstrikeMode() ? 11 : 10,
                     UI_BLINK | UI_SMALLFONT, wizardAccent );
    }
}

static void LadderWizard_DrawButton( void *self ) {
    menutext_s *button = (menutext_s *)self;
    qboolean focus = LadderWizard_ItemHasFocus( &button->generic );
    qboolean active = !( button->generic.flags & QMF_INACTIVE );

    Frontend_DrawNavButton( button->generic.left, button->generic.top,
                            button->generic.right - button->generic.left,
                            button->generic.bottom - button->generic.top,
                            button->string ? button->string : "", 1.0f,
                            active && focus, UI_CENTER );
}

static void LadderWizard_MakeCompareKey( const char *src, char *dst, size_t dstSize ) {
    size_t i;
    size_t j;

    if ( !dst || !dstSize ) {
        return;
    }

    if ( !src ) {
        dst[0] = '\0';
        return;
    }

    for ( i = 0, j = 0; src[i] && j + 1 < dstSize; ++i ) {
        unsigned char c = (unsigned char)src[i];

        if ( Q_IsColorString( &src[i] ) ) {
            ++i;
            continue;
        }

        dst[j++] = tolower( c );
    }

    dst[j] = '\0';
}

static void LadderWizard_NormalizeServerName( const char *src,
                                              char *dst,
                                              size_t dstSize,
                                              qboolean *wasTruncated ) {
    char cleaned[256];
    size_t i;
    size_t j;
    size_t start;
    size_t end;
    size_t len;

    if ( wasTruncated ) {
        *wasTruncated = qfalse;
    }

    if ( !dst || !dstSize ) {
        return;
    }

    if ( !src || !src[0] ) {
        dst[0] = '\0';
        return;
    }

    for ( i = 0, j = 0; src[i] && j + 1 < sizeof( cleaned ); ++i ) {
        if ( Q_IsColorString( &src[i] ) ) {
            ++i;
            continue;
        }
        if ( src[i] == '"' || src[i] == '\\' || src[i] == ';' ) {
            cleaned[j++] = '_';
            continue;
        }
        if ( (unsigned char)src[i] < 0x20 || (unsigned char)src[i] > 0x7E ) {
            continue;
        }
        if ( ( src[i] >= '0' && src[i] <= '9' ) ||
             ( src[i] >= 'a' && src[i] <= 'z' ) ||
             ( src[i] >= 'A' && src[i] <= 'Z' ) ||
             src[i] == '_' || src[i] == '-' || src[i] == '.' || src[i] == ' ' ) {
            cleaned[j++] = src[i];
        } else {
            cleaned[j++] = '_';
        }
    }
    cleaned[j] = '\0';

    start = 0;
    while ( cleaned[start] && LadderWizard_IsWhitespace( cleaned[start] ) ) {
        ++start;
    }

    end = strlen( cleaned );
    while ( end > start && LadderWizard_IsWhitespace( cleaned[end - 1] ) ) {
        --end;
    }

    len = end - start;
    if ( len >= dstSize ) {
        len = dstSize - 1;
        if ( wasTruncated ) {
            *wasTruncated = qtrue;
        }
    }

    memcpy( dst, cleaned + start, len );
    dst[len] = '\0';
}

static qboolean LadderWizard_ProfileListContains( const char *list,
                                                  const char *profileName ) {
    const char *cursor;
    int profileLength;

    if ( !list || !profileName || !profileName[0] ) {
        return qfalse;
    }

    profileLength = strlen( profileName );
    cursor = list;
    while ( *cursor ) {
        const char *separator = strchr( cursor, ',' );
        int tokenLength = separator ? (int)( separator - cursor ) : strlen( cursor );

        if ( tokenLength == profileLength &&
             Q_stricmpn( cursor, profileName, tokenLength ) == 0 ) {
            return qtrue;
        }
        if ( !separator ) {
            break;
        }
        cursor = separator + 1;
    }

    return qfalse;
}

static void LadderWizard_ProfileListAdd( const char *cvarName,
                                         const char *profileName ) {
    char list[1024];

    if ( !cvarName || !cvarName[0] || !profileName || !profileName[0] ) {
        return;
    }

    trap_Cvar_VariableStringBuffer( cvarName, list, sizeof( list ) );
    if ( LadderWizard_ProfileListContains( list, profileName ) ) {
        return;
    }
    if ( list[0] ) {
        Q_strcat( list, sizeof( list ), "," );
    }
    Q_strcat( list, sizeof( list ), profileName );
    trap_Cvar_Set( cvarName, list );
}

static void LadderWizard_GetActiveProfile( char *profileName, int profileNameSize ) {
    if ( !profileName || profileNameSize <= 0 ) {
        return;
    }
    trap_Cvar_VariableStringBuffer( "profile_active", profileName,
                                    profileNameSize );
}

void UI_LadderWizard_MarkProfileRegistered( void ) {
    char profileName[PROFILE_MAX_NAME];

    LadderWizard_RegisterCvars();
    LadderWizard_GetActiveProfile( profileName, sizeof( profileName ) );
    LadderWizard_ProfileListAdd( "ladder_wizard_profiles", profileName );
    trap_Cvar_Update( &ui_ladderWizardProfiles );
}

void UI_LadderWizard_MarkProfileDismissed( void ) {
    char profileName[PROFILE_MAX_NAME];

    LadderWizard_RegisterCvars();
    LadderWizard_GetActiveProfile( profileName, sizeof( profileName ) );
    LadderWizard_ProfileListAdd( "ladder_wizard_dismissed_profiles",
                                 profileName );
    trap_Cvar_Update( &ui_ladderWizardDismissedProfiles );
}

static void LadderWizard_SanitizeArg( const char *src, char *dst, size_t dstSize ) {
    size_t i;
    size_t j;

    if ( !dst || !dstSize ) {
        return;
    }

    if ( !src ) {
        dst[0] = '\0';
        return;
    }

    for ( i = 0, j = 0; src[i] && j + 1 < dstSize; ++i ) {
        if ( src[i] == '"' || src[i] == '\\' || src[i] == '\n' || src[i] == '\r' ) {
            continue;
        }
        dst[j++] = src[i];
    }

    dst[j] = '\0';
}

static qboolean LadderWizard_ContainsToken( const char *haystack, const char *needle ) {
    size_t i;
    size_t needleLen;

    if ( !haystack || !needle || !needle[0] ) {
        return qfalse;
    }

    needleLen = strlen( needle );
    for ( i = 0; haystack[i]; ++i ) {
        if ( Q_stricmpn( haystack + i, needle, needleLen ) == 0 ) {
            return qtrue;
        }
    }

    return qfalse;
}

static void LadderWizard_StartRegistration( void ) {
    char ownerName[64];
    char ownerEmail[128];
    char serverName[64];
    char cmd[384];

    Q_strncpyz( ownerName, s_wizard.ownerName.field.buffer, sizeof( ownerName ) );
    Q_strncpyz( ownerEmail, s_wizard.ownerEmail.field.buffer, sizeof( ownerEmail ) );

    if ( !ownerName[0] ) {
        Q_strncpyz( s_wizard.statusLine, "Please enter an owner name.", sizeof( s_wizard.statusLine ) );
        return;
    }

    if ( !LadderWizard_ValidateEmail( ownerEmail ) ) {
        Q_strncpyz( s_wizard.statusLine, "Please enter a valid owner email.", sizeof( s_wizard.statusLine ) );
        return;
    }

    LadderWizard_SanitizeArg( ownerName, ownerName, sizeof( ownerName ) );
    LadderWizard_SanitizeArg( ownerEmail, ownerEmail, sizeof( ownerEmail ) );
    LadderWizard_SanitizeArg( s_wizard.serverName, serverName, sizeof( serverName ) );

    if ( !ownerName[0] || !ownerEmail[0] || !serverName[0] ) {
        Q_strncpyz( s_wizard.statusLine, "Invalid registration data.", sizeof( s_wizard.statusLine ) );
        return;
    }

    trap_Cvar_Set( "sv_ladderUrl", "https://ladder.q3rally.com/index.php/matches" );
    trap_Cvar_Set( "sv_hostname", s_wizard.serverName );

    Com_sprintf( cmd, sizeof( cmd ),
                 "ladder_register \"%s\" \"%s\" \"%s\" \"agree\"\n",
                 ownerName, ownerEmail, serverName );
    trap_Cmd_ExecuteText( EXEC_APPEND, cmd );

    s_wizard.result = WIZARD_RESULT_PENDING;
    s_wizard.submitting = qtrue;
    Q_strncpyz( s_wizard.statusLine,
                "Submitting registration to /register endpoint...",
                sizeof( s_wizard.statusLine ) );
    LadderWizard_UpdateButtons();
}

static void LadderWizard_FinishSubmission( wizardResult_t result, const char *statusLine ) {
    s_wizard.submitting = qfalse;
    s_wizard.page = WIZARD_PAGE_DONE;
    s_wizard.result = result;

    if ( statusLine && statusLine[0] ) {
        Q_strncpyz( s_wizard.statusLine, statusLine, sizeof( s_wizard.statusLine ) );
    } else {
        s_wizard.statusLine[0] = '\0';
    }

    LadderWizard_UpdateButtons();
    Menu_SetCursorToItem( &s_wizard.menu, &s_wizard.btnYes );
}

static void LadderWizard_UpdateButtons( void ) {
    if ( s_wizard.page == WIZARD_PAGE_CONFIRM ) {
        s_wizard.btnYes.string = WIZARD_TEXT_REGISTER;
        s_wizard.btnNo.string  = WIZARD_TEXT_CANCEL;
        s_wizard.btnNever.string = WIZARD_TEXT_NEVER_SHOW;

        s_wizard.ownerName.generic.flags &= ~( QMF_INACTIVE | QMF_GRAYED );
        s_wizard.ownerEmail.generic.flags &= ~( QMF_INACTIVE | QMF_GRAYED );
        if ( s_wizard.submitting ) {
            s_wizard.ownerName.generic.flags |= QMF_INACTIVE;
            s_wizard.ownerEmail.generic.flags |= QMF_INACTIVE;
        }

        s_wizard.btnYes.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
        if ( s_wizard.submitting ) {
            s_wizard.btnYes.generic.flags |= QMF_INACTIVE;
            s_wizard.btnNo.string = WIZARD_TEXT_ABORT;
            s_wizard.btnNever.generic.flags = QMF_INACTIVE | QMF_HIDDEN;
        } else {
            s_wizard.btnNever.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
        }
        s_wizard.btnNo.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    } else {
        if ( s_wizard.result == WIZARD_RESULT_SUCCESS ) {
            s_wizard.btnYes.string    = "OK";
            s_wizard.btnYes.generic.x = WIZARD_SCREEN_W / 2 - 145;
            s_wizard.btnNo.string     = "";
            s_wizard.btnNo.generic.flags    = QMF_INACTIVE | QMF_HIDDEN;
            s_wizard.btnNever.string        = "";
            s_wizard.btnNever.generic.flags = QMF_INACTIVE | QMF_HIDDEN;
            s_wizard.ownerName.generic.flags  = QMF_INACTIVE | QMF_HIDDEN;
            s_wizard.ownerEmail.generic.flags = QMF_INACTIVE | QMF_HIDDEN;
        } else {
            s_wizard.btnYes.string    = "RETRY";
            s_wizard.btnYes.generic.x = WIZARD_SCREEN_W / 2 - 145;
            s_wizard.btnNo.string     = "BACK";
            s_wizard.btnNo.generic.flags    = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
            s_wizard.btnNever.string        = "";
            s_wizard.btnNever.generic.flags = QMF_INACTIVE | QMF_HIDDEN;
        }

        s_wizard.btnYes.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    }

    UI_ReflowPTextBounds( &s_wizard.btnYes );
    UI_ReflowPTextBounds( &s_wizard.btnNo );
    UI_ReflowPTextBounds( &s_wizard.btnNever );

    /* Use stable frontend-sized hitboxes instead of the narrow legacy text
     * bounds, so the three footer actions are easy to reach with the mouse. */
    s_wizard.btnNo.generic.left = WIZARD_BTN_CANCEL_X;
    s_wizard.btnNo.generic.top = WIZARD_BTN_Y;
    s_wizard.btnNo.generic.right = WIZARD_BTN_CANCEL_X + WIZARD_BTN_WIDTH;
    s_wizard.btnNo.generic.bottom = WIZARD_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;
    s_wizard.btnYes.generic.left = WIZARD_BTN_REGISTER_X;
    s_wizard.btnYes.generic.top = WIZARD_BTN_Y;
    s_wizard.btnYes.generic.right = WIZARD_BTN_REGISTER_X + WIZARD_BTN_WIDTH;
    s_wizard.btnYes.generic.bottom = WIZARD_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;
    s_wizard.btnNever.generic.left = WIZARD_BTN_NEVER_X;
    s_wizard.btnNever.generic.top = WIZARD_BTN_Y;
    s_wizard.btnNever.generic.right = WIZARD_BTN_NEVER_X + WIZARD_BTN_WIDTH;
    s_wizard.btnNever.generic.bottom = WIZARD_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;
}

static qboolean LadderWizard_IsFocusableItem( const menucommon_s *item ) {
    if ( !item ) {
        return qfalse;
    }

    if ( item->flags & ( QMF_INACTIVE | QMF_GRAYED | QMF_HIDDEN ) ) {
        return qfalse;
    }

    return qtrue;
}

static sfxHandle_t LadderWizard_MoveFocus( int dir ) {
    menucommon_s *order[] = {
        (menucommon_s *)&s_wizard.ownerName,
        (menucommon_s *)&s_wizard.ownerEmail,
        (menucommon_s *)&s_wizard.btnYes,
        (menucommon_s *)&s_wizard.btnNo,
        (menucommon_s *)&s_wizard.btnNever
    };
    menucommon_s *current;
    int count;
    int i;
    int currentIndex;

    count = ARRAY_LEN( order );
    if ( !count || !dir ) {
        return 0;
    }

    current = Menu_ItemAtCursor( &s_wizard.menu );
    currentIndex = 0;
    for ( i = 0; i < count; ++i ) {
        if ( order[i] == current ) {
            currentIndex = i;
            break;
        }
    }

    for ( i = 1; i <= count; ++i ) {
        int next = ( currentIndex + dir * i + count ) % count;
        if ( LadderWizard_IsFocusableItem( order[next] ) ) {
            Menu_SetCursorToItem( &s_wizard.menu, order[next] );
            return menu_move_sound;
        }
    }

    return 0;
}

static sfxHandle_t LadderWizard_MenuKey( int key ) {
    if ( key == K_TAB ) {
        if ( trap_Key_IsDown( K_SHIFT ) ) {
            return LadderWizard_MoveFocus( -1 );
        }
        return LadderWizard_MoveFocus( 1 );
    }

    switch ( key ) {
    case K_UPARROW:
    case K_KP_UPARROW:
    case K_LEFTARROW:
    case K_KP_LEFTARROW:
        return LadderWizard_MoveFocus( -1 );
    case K_DOWNARROW:
    case K_KP_DOWNARROW:
    case K_RIGHTARROW:
    case K_KP_RIGHTARROW:
        return LadderWizard_MoveFocus( 1 );
    }

    return Menu_DefaultKey( &s_wizard.menu, key );
}

/* ── MaybeShow ───────────────────────────────────────────────────────────────── */

void UI_LadderWizard_MaybeShow( void ) {
    char profileName[PROFILE_MAX_NAME];
    char dismissedProfiles[1024];

    LadderWizard_RegisterCvars();
    trap_Cvar_Update( &ui_ladderWizardDismissed );
    trap_Cvar_Update( &ui_ladderWizardDismissedProfiles );

    LadderWizard_GetActiveProfile( profileName, sizeof( profileName ) );

    if ( !profileName[0] ) return;
    /* Don't show standalone wizard while the new profile wizard is active —
     * it handles offline key registration as part of its Page 3 flow.      */
    if ( UI_ProfileWizard_IsActive() ) return;

    /* The engine activates the profile's protected key/name pair before this
     * check. A completion flag from another profile must never suppress setup. */
    if ( trap_Cvar_VariableValue( "sv_ladderProfileReady" ) != 0 ) {
        return;
    }

    trap_Cvar_VariableStringBuffer( "ladder_wizard_dismissed_profiles",
                                    dismissedProfiles,
                                    sizeof( dismissedProfiles ) );

    if ( LadderWizard_ProfileListContains( dismissedProfiles, profileName ) ) {
        return;
    }

    UI_LadderWizardMenu();
}

/* ── Menu init ───────────────────────────────────────────────────────────────── */

void UI_LadderWizardMenu( void ) {
    int fieldX;
    int fieldWidthChars;

    memset( &s_wizard, 0, sizeof( s_wizard ) );

    trap_Cvar_VariableStringBuffer( "profile_active",
                                    s_wizard.playerName,
                                    sizeof( s_wizard.playerName ) );
    if ( !s_wizard.playerName[0] ) return;

    {
        char rawServerName[128];
        char finalCompareKey[65];
        qboolean truncated = qfalse;

        Com_sprintf( rawServerName, sizeof( rawServerName ), "%s_OFFLINE", s_wizard.playerName );
        LadderWizard_NormalizeServerName( rawServerName,
                                          s_wizard.serverName,
                                          sizeof( s_wizard.serverName ),
                                          &truncated );
        LadderWizard_MakeCompareKey( rawServerName,
                                     s_wizard.serverNameCompareKey,
                                     sizeof( s_wizard.serverNameCompareKey ) );
        if ( !s_wizard.serverName[0] ) {
            Q_strncpyz( s_wizard.serverName,
                        "q3rally_offline",
                        sizeof( s_wizard.serverName ) );
        }
        LadderWizard_MakeCompareKey( s_wizard.serverName,
                                     finalCompareKey,
                                     sizeof( finalCompareKey ) );

        if ( truncated ) {
            Q_strncpyz( s_wizard.serverNameNotice,
                        WIZARD_TEXT_SERVERNAME_TRUNCATED,
                        sizeof( s_wizard.serverNameNotice ) );
        } else if ( Q_stricmp( s_wizard.serverNameCompareKey, finalCompareKey ) != 0 ) {
            Q_strncpyz( s_wizard.serverNameNotice,
                        WIZARD_TEXT_SERVERNAME_NORMALIZED,
                        sizeof( s_wizard.serverNameNotice ) );
        }
    }
    s_wizard.page = WIZARD_PAGE_CONFIRM;
    s_wizard.result = WIZARD_RESULT_NONE;

    Q_strncpyz( s_wizard.ownerName.field.buffer, s_wizard.playerName,
                sizeof( s_wizard.ownerName.field.buffer ) );

    s_wizard.menu.draw       = LadderWizard_Draw;
    /* Modal dialog: draw our own full-screen dimmed backdrop and avoid
     * rendering/updating interaction from lower stack menus (e.g. main menu). */
    s_wizard.menu.transparent = qfalse;
    s_wizard.menu.fullscreen = qtrue;
    s_wizard.menu.wrapAround = qtrue;
    s_wizard.menu.showlogo   = qfalse;
    s_wizard.menu.key        = LadderWizard_MenuKey;

    s_wizard.ownerName.generic.type = MTYPE_FIELD;
    s_wizard.ownerName.generic.flags = QMF_SMALLFONT;
    s_wizard.ownerName.generic.id = ID_OWNER_NAME;
    s_wizard.ownerName.generic.name = "Owner";
    s_wizard.ownerName.generic.ownerdraw = LadderWizard_DrawField;
    LadderWizard_ComputeFormLayout( NULL, NULL, &fieldX, &fieldWidthChars );

    s_wizard.ownerName.generic.x = fieldX;
    s_wizard.ownerName.generic.y = WIZARD_FORM_TOP_Y;
    s_wizard.ownerName.field.widthInChars = fieldWidthChars;
    /* keep maxchars > widthInChars: long input scrolls instead of growing the field */
    s_wizard.ownerName.field.maxchars = 48;

    s_wizard.ownerEmail.generic.type = MTYPE_FIELD;
    s_wizard.ownerEmail.generic.flags = QMF_SMALLFONT;
    s_wizard.ownerEmail.generic.id = ID_OWNER_EMAIL;
    s_wizard.ownerEmail.generic.name = "Email";
    s_wizard.ownerEmail.generic.ownerdraw = LadderWizard_DrawField;
    s_wizard.ownerEmail.generic.x = fieldX;
    s_wizard.ownerEmail.generic.y = WIZARD_FORM_TOP_Y + WIZARD_FORM_ROW_H;
    s_wizard.ownerEmail.field.widthInChars = fieldWidthChars;
    s_wizard.ownerEmail.field.maxchars = 64;

    /* YES */
    s_wizard.btnYes.generic.type     = MTYPE_PTEXT;
    s_wizard.btnYes.generic.flags    = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    s_wizard.btnYes.generic.id       = ID_WIZARD_YES;
    s_wizard.btnYes.generic.callback = LadderWizard_MenuEvent;
    s_wizard.btnYes.generic.ownerdraw = LadderWizard_DrawButton;
    s_wizard.btnYes.generic.x        = WIZARD_BTN_REGISTER_X;
    s_wizard.btnYes.generic.y        = WIZARD_BTN_Y;
    s_wizard.btnYes.string           = WIZARD_TEXT_REGISTER;
    s_wizard.btnYes.style            = UI_CENTER | UI_SMALLFONT;
    s_wizard.btnYes.color            = wizardAccent;

    /* NO */
    s_wizard.btnNo.generic.type     = MTYPE_PTEXT;
    s_wizard.btnNo.generic.flags    = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    s_wizard.btnNo.generic.id       = ID_WIZARD_NO;
    s_wizard.btnNo.generic.callback = LadderWizard_MenuEvent;
    s_wizard.btnNo.generic.ownerdraw = LadderWizard_DrawButton;
    s_wizard.btnNo.generic.x        = WIZARD_BTN_CANCEL_X;
    s_wizard.btnNo.generic.y        = WIZARD_BTN_Y;
    s_wizard.btnNo.string           = WIZARD_TEXT_CANCEL;
    s_wizard.btnNo.style            = UI_CENTER | UI_SMALLFONT;
    s_wizard.btnNo.color            = wizardText;

    /* NEVER SHOW AGAIN */
    s_wizard.btnNever.generic.type     = MTYPE_PTEXT;
    s_wizard.btnNever.generic.flags    = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    s_wizard.btnNever.generic.id       = ID_WIZARD_NEVER;
    s_wizard.btnNever.generic.callback = LadderWizard_MenuEvent;
    s_wizard.btnNever.generic.ownerdraw = LadderWizard_DrawButton;
    s_wizard.btnNever.generic.x        = WIZARD_BTN_NEVER_X;
    s_wizard.btnNever.generic.y        = WIZARD_BTN_Y;
    s_wizard.btnNever.string           = WIZARD_TEXT_NEVER_SHOW;
    s_wizard.btnNever.style            = UI_CENTER | UI_SMALLFONT;
    s_wizard.btnNever.color            = wizardText;

    Menu_AddItem( &s_wizard.menu, &s_wizard.ownerName );
    Menu_AddItem( &s_wizard.menu, &s_wizard.ownerEmail );
    Menu_AddItem( &s_wizard.menu, &s_wizard.btnYes );
    Menu_AddItem( &s_wizard.menu, &s_wizard.btnNo );
    Menu_AddItem( &s_wizard.menu, &s_wizard.btnNever );

    /* MenuField_Init clears the edit buffer, so restore the profile name
     * after the fields have been registered. */
    Q_strncpyz( s_wizard.ownerName.field.buffer, s_wizard.playerName,
                sizeof( s_wizard.ownerName.field.buffer ) );

    s_wizard.btnNo.generic.left = WIZARD_BTN_CANCEL_X;
    s_wizard.btnNo.generic.top = WIZARD_BTN_Y;
    s_wizard.btnNo.generic.right = WIZARD_BTN_CANCEL_X + WIZARD_BTN_WIDTH;
    s_wizard.btnNo.generic.bottom = WIZARD_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;
    s_wizard.btnYes.generic.left = WIZARD_BTN_REGISTER_X;
    s_wizard.btnYes.generic.top = WIZARD_BTN_Y;
    s_wizard.btnYes.generic.right = WIZARD_BTN_REGISTER_X + WIZARD_BTN_WIDTH;
    s_wizard.btnYes.generic.bottom = WIZARD_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;
    s_wizard.btnNever.generic.left = WIZARD_BTN_NEVER_X;
    s_wizard.btnNever.generic.top = WIZARD_BTN_Y;
    s_wizard.btnNever.generic.right = WIZARD_BTN_NEVER_X + WIZARD_BTN_WIDTH;
    s_wizard.btnNever.generic.bottom = WIZARD_BTN_Y + UI_FRONTEND_BUTTON_HEIGHT;

    LadderWizard_UpdateButtons();
    Menu_SetCursorToItem( &s_wizard.menu, &s_wizard.ownerName );

    uis.transitionIn  = 0;
    uis.transitionOut = 0;

    UI_PushMenu( &s_wizard.menu );
}

/* ── Draw ────────────────────────────────────────────────────────────────────── */

static void LadderWizard_Draw( void ) {
    int cx = WIZARD_SCREEN_W / 2;

    Frontend_DrawBackground( wizardScrim );
    Frontend_DrawPanel( WIZARD_FRAME_X, WIZARD_FRAME_Y,
                        WIZARD_FRAME_W, WIZARD_FRAME_H, uis.tFrac,
                        UI_FRONTEND_STYLE_FRAME );
    Frontend_DrawText( WIZARD_FRAME_X + 24, WIZARD_FRAME_Y + 24,
                       "Ladder setup", UI_LEFT | UI_BIGFONT, wizardText );
    Frontend_DrawText( WIZARD_FRAME_X + 24, WIZARD_FRAME_Y + 48,
                       "Register offline results and keep your profile connected",
                       UI_LEFT | UI_SMALLFONT, wizardMuted );
    Frontend_DrawStatusChip( WIZARD_FRAME_X + WIZARD_FRAME_W - 104,
                             WIZARD_FRAME_Y + 26, "Ladder", wizardStatus,
                             uis.tFrac );
    Frontend_DrawCard( WIZARD_PANEL_X, WIZARD_PANEL_Y,
                       WIZARD_PANEL_W, WIZARD_PANEL_H, uis.tFrac, qfalse );

    if ( s_wizard.page == WIZARD_PAGE_CONFIRM ) {
        Frontend_DrawText( WIZARD_PANEL_X + 24, WIZARD_PANEL_Y + 26,
                           "Offline match tracking", UI_LEFT | UI_SMALLFONT,
                           wizardAccent );
        Frontend_DrawText( cx, WIZARD_PANEL_Y + 52,
                           WIZARD_TEXT_CONFIRM_QUESTION,
                           UI_CENTER | UI_SMALLFONT, wizardText );
        Frontend_DrawText( cx, WIZARD_PANEL_Y + 72,
                           va( "Server: %s", s_wizard.serverName ),
                           UI_CENTER | UI_SMALLFONT, wizardMuted );
        if ( s_wizard.serverNameNotice[0] ) {
            Frontend_DrawText( cx, WIZARD_PANEL_Y + 90,
                               s_wizard.serverNameNotice,
                               UI_CENTER | UI_SMALLFONT, wizardError );
        }

        if ( s_wizard.submitting ) {
            Frontend_DrawText( cx, WIZARD_PANEL_Y + 194,
                               WIZARD_TEXT_SUBMITTING,
                               UI_CENTER | UI_SMALLFONT, wizardAccent );
        } else if ( s_wizard.statusLine[0] ) {
            Frontend_DrawText( cx, WIZARD_PANEL_Y + 194,
                               s_wizard.statusLine,
                               UI_CENTER | UI_SMALLFONT,
                               s_wizard.result == WIZARD_RESULT_PENDING ?
                               wizardAccent : wizardError );
        } else {
            Frontend_DrawText( cx, WIZARD_PANEL_Y + 194,
                               WIZARD_TEXT_CANCEL_HINT,
                               UI_CENTER | UI_SMALLFONT, wizardMuted );
            Frontend_DrawText( cx, WIZARD_PANEL_Y + 212,
                               WIZARD_TEXT_NEVER_HINT,
                               UI_CENTER | UI_SMALLFONT, wizardMuted );
        }
    } else if ( s_wizard.result == WIZARD_RESULT_SUCCESS ) {
        Frontend_DrawText( cx, WIZARD_PANEL_Y + 92,
                           "Registration successful!",
                           UI_CENTER | UI_BIGFONT, wizardAccent );
        Frontend_DrawText( cx, WIZARD_PANEL_Y + 132,
                           "Your client is now registered with the ladder.",
                           UI_CENTER | UI_SMALLFONT, wizardText );
        Frontend_DrawText( cx, WIZARD_PANEL_Y + 154,
                           "Settings have been saved automatically.",
                           UI_CENTER | UI_SMALLFONT, wizardMuted );
        Frontend_DrawText( cx, WIZARD_PANEL_Y + 184,
                           WIZARD_TEXT_PENDING_APPROVAL,
                           UI_CENTER | UI_SMALLFONT, wizardStatus );
    } else {
        Frontend_DrawText( cx, WIZARD_PANEL_Y + 80,
                           WIZARD_TEXT_FAILED,
                           UI_CENTER | UI_BIGFONT, wizardError );
        if ( s_wizard.statusLine[0] ) {
            Frontend_DrawText( cx, WIZARD_PANEL_Y + 122,
                               s_wizard.statusLine,
                               UI_CENTER | UI_SMALLFONT, wizardText );
        }
        Frontend_DrawText( cx, WIZARD_PANEL_Y + 154,
                           WIZARD_TEXT_FAILED_HINT,
                           UI_CENTER | UI_SMALLFONT, wizardMuted );
    }

    Menu_Draw( &s_wizard.menu );
    Frontend_DrawText( WIZARD_FRAME_X + 24, WIZARD_FRAME_Y + WIZARD_FRAME_H - 24,
                       "Enter select    Tab switch    Esc back",
                       UI_LEFT | UI_SMALLFONT, wizardMuted );
}

/* ── Event handler ───────────────────────────────────────────────────────────── */

static void LadderWizard_MenuEvent( void *ptr, int event ) {
    if ( event != QM_ACTIVATED ) return;

    switch ( ( (menucommon_s *)ptr )->id ) {
    case ID_WIZARD_YES:
        if ( s_wizard.page == WIZARD_PAGE_CONFIRM ) {
            if ( s_wizard.result != WIZARD_RESULT_PENDING ) {
                LadderWizard_StartRegistration();
            }
        } else if ( s_wizard.result == WIZARD_RESULT_SUCCESS ) {
            trap_Cvar_SetValue( "ladder_wizard_completed", 1 );
            trap_Cvar_Update( &ui_ladderWizardCompleted );
            UI_PopMenu();
        } else {
            s_wizard.page = WIZARD_PAGE_CONFIRM;
            s_wizard.result = WIZARD_RESULT_NONE;
            s_wizard.statusLine[0] = '\0';
            LadderWizard_UpdateButtons();
        }
        break;

    case ID_WIZARD_NO:
        if ( s_wizard.submitting ) {
            Q_strncpyz( s_wizard.statusLine,
                        "Registration still in progress. Please wait for completion.",
                        sizeof( s_wizard.statusLine ) );
            return;
        }
        UI_PopMenu();
        break;

    case ID_WIZARD_NEVER:
        if ( s_wizard.page == WIZARD_PAGE_CONFIRM && s_wizard.result != WIZARD_RESULT_PENDING ) {
            UI_LadderWizard_MarkProfileDismissed();
            trap_Cvar_SetValue( "ladder_wizard_dismissed", 1 );
            trap_Cvar_Update( &ui_ladderWizardDismissed );
            trap_Print( S_COLOR_YELLOW WIZARD_TEXT_DISMISSED_LOG );
            UI_PopMenu();
        }
        break;
    }
}

/* Called by engine when ladder_register succeeds */
void UI_LadderWizard_OnSuccess( void ) {
    /* sv_ladderEnabled, sv_ladderUrl, sv_ladderApiKey and sv_hostname are
     * set by SV_LadderFinishRegister in engine code – the UI VM lacks write
     * access to protected server cvars.  writeconfig is also triggered from
     * there, after all cvars are set.                                       */
    trap_Cvar_SetValue( "ladder_wizard_completed", 1 );
    trap_Cvar_Update( &ui_ladderWizardCompleted );
    UI_LadderWizard_MarkProfileRegistered();

    LadderWizard_FinishSubmission( WIZARD_RESULT_SUCCESS, "" );
}

void UI_LadderWizard_OnError( const char *msg ) {
    if ( msg && msg[0] ) {
        const char *statusMsg = msg;
        if ( LadderWizard_ContainsToken( msg, "timeout" ) ||
             LadderWizard_ContainsToken( msg, "network" ) ) {
            statusMsg = "Network/timeout error. Please retry.";
        }
        Q_strncpyz( s_wizard.statusLine, statusMsg, sizeof( s_wizard.statusLine ) );
        trap_Print( va( "Ladder wizard: registration failed: %s\n", msg ) );
    } else {
        Q_strncpyz( s_wizard.statusLine,
                    "Registration request failed.",
                    sizeof( s_wizard.statusLine ) );
        trap_Print( "Ladder wizard: registration failed (no error message).\n" );
    }

    LadderWizard_FinishSubmission( WIZARD_RESULT_ERROR, s_wizard.statusLine );
}
