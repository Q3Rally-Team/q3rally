/*
===========================================================================
Copyright (C) 2002-2026 Q3Rally Team
===========================================================================
*/
// ui_rally_ladder_wizard.c – Offline tracking registration wizard

#include "ui_local.h"

#define WIZARD_SCREEN_W     640
#define WIZARD_SCREEN_H     480
#define WIZARD_PANEL_W      460
#define WIZARD_PANEL_H      260
#define WIZARD_PANEL_X      ( ( WIZARD_SCREEN_W - WIZARD_PANEL_W ) / 2 )
#define WIZARD_PANEL_Y      ( ( WIZARD_SCREEN_H - WIZARD_PANEL_H ) / 2 )
#define WIZARD_BTN_Y        ( WIZARD_PANEL_Y + WIZARD_PANEL_H - 48 )

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
    char            apiKey[256];
    char            statusLine[128];
    qboolean        submitting;
} s_wizard;

static vec4_t wizardBg     = { 0.08f, 0.08f, 0.12f, 0.97f };
static vec4_t wizardDim    = { 0.00f, 0.00f, 0.00f, 0.45f };
static vec4_t wizardBorder = { 0.35f, 0.45f, 0.75f, 0.60f };
static vec4_t wizardTitle  = { 0.72f, 0.82f, 1.00f, 1.00f };
static vec4_t wizardText   = { 0.75f, 0.78f, 0.88f, 1.00f };
static vec4_t wizardAccent = { 0.50f, 0.65f, 1.00f, 1.00f };
static vec4_t wizardError  = { 1.00f, 0.42f, 0.42f, 1.00f };

static void LadderWizard_MenuEvent( void *ptr, int event );
static void LadderWizard_Draw( void );
static void LadderWizard_UpdateButtons( void );
static sfxHandle_t LadderWizard_MenuKey( int key );
static void LadderWizard_FinishSubmission( wizardResult_t result, const char *statusLine );
static void LadderWizard_ComputeFormLayout( int *contentLeft,
                                            int *contentRight,
                                            int *fieldX,
                                            int *fieldWidthChars );

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
#define WIZARD_TEXT_FAILED                   "Registration failed."
#define WIZARD_TEXT_FAILED_HINT              "Please check data/connection and try again."
#define WIZARD_TEXT_DISMISSED_LOG            "Ladder wizard permanently dismissed via 'Never show again'.\n"

#define WIZARD_CONTENT_PAD_X        24
#define WIZARD_FORM_TOP_Y           ( WIZARD_PANEL_Y + 108 )
#define WIZARD_FORM_ROW_H           28
#define WIZARD_FORM_LABEL_OFFSET_Y  -12
#define WIZARD_FIELD_SAFE_PAD_PX    ( SMALLCHAR_WIDTH * 2 )
#define WIZARD_FIELD_MIN_CHARS      10

/* ── CVar registration ───────────────────────────────────────────────────────── */

static vmCvar_t ui_ladderWizardDismissed;
static vmCvar_t ui_ladderWizardCompleted;

static void LadderWizard_RegisterCvars( void ) {
    trap_Cvar_Register( &ui_ladderWizardDismissed,
                        "ladder_wizard_dismissed", "0",
                        CVAR_ARCHIVE | CVAR_USERINFO );
    trap_Cvar_Register( &ui_ladderWizardCompleted,
                        "ladder_wizard_completed", "0",
                        CVAR_ARCHIVE | CVAR_USERINFO );
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

    trap_Cvar_Set( "sv_ladderEnabled", "1" );
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
        return;
    }

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
    char apiKey[256];

    LadderWizard_RegisterCvars();
    trap_Cvar_Update( &ui_ladderWizardDismissed );
    trap_Cvar_Update( &ui_ladderWizardCompleted );

    trap_Cvar_VariableStringBuffer( "profile_active", profileName, sizeof( profileName ) );
    trap_Cvar_VariableStringBuffer( "sv_ladderApiKey", apiKey,     sizeof( apiKey ) );

    if ( !profileName[0] ) return;
    if ( apiKey[0] )       return;
    /* Product decision: once completed, don't auto-show again even if API key is removed later. */
    if ( ui_ladderWizardCompleted.integer != 0 ) return;
    if ( ui_ladderWizardDismissed.integer != 0 ) return;

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
    LadderWizard_ComputeFormLayout( NULL, NULL, &fieldX, &fieldWidthChars );

    s_wizard.ownerName.generic.x = fieldX;
    s_wizard.ownerName.generic.y = WIZARD_FORM_TOP_Y;
    s_wizard.ownerName.field.widthInChars = fieldWidthChars;
    /* keep maxchars > widthInChars: long input scrolls instead of growing the field */
    s_wizard.ownerName.field.maxchars = 48;

    s_wizard.ownerEmail.generic.type = MTYPE_FIELD;
    s_wizard.ownerEmail.generic.flags = QMF_SMALLFONT;
    s_wizard.ownerEmail.generic.id = ID_OWNER_EMAIL;
    s_wizard.ownerEmail.generic.x = fieldX;
    s_wizard.ownerEmail.generic.y = WIZARD_FORM_TOP_Y + WIZARD_FORM_ROW_H;
    s_wizard.ownerEmail.field.widthInChars = fieldWidthChars;
    s_wizard.ownerEmail.field.maxchars = 64;

    /* YES */
    s_wizard.btnYes.generic.type     = MTYPE_PTEXT;
    s_wizard.btnYes.generic.flags    = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    s_wizard.btnYes.generic.id       = ID_WIZARD_YES;
    s_wizard.btnYes.generic.callback = LadderWizard_MenuEvent;
    s_wizard.btnYes.generic.x        = WIZARD_SCREEN_W / 2 - 145;
    s_wizard.btnYes.generic.y        = WIZARD_BTN_Y;
    s_wizard.btnYes.string           = WIZARD_TEXT_REGISTER;
    s_wizard.btnYes.style            = UI_CENTER | UI_SMALLFONT;
    s_wizard.btnYes.color            = wizardAccent;

    /* NO */
    s_wizard.btnNo.generic.type     = MTYPE_PTEXT;
    s_wizard.btnNo.generic.flags    = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    s_wizard.btnNo.generic.id       = ID_WIZARD_NO;
    s_wizard.btnNo.generic.callback = LadderWizard_MenuEvent;
    s_wizard.btnNo.generic.x        = WIZARD_SCREEN_W / 2;
    s_wizard.btnNo.generic.y        = WIZARD_BTN_Y;
    s_wizard.btnNo.string           = WIZARD_TEXT_CANCEL;
    s_wizard.btnNo.style            = UI_CENTER | UI_SMALLFONT;
    s_wizard.btnNo.color            = wizardText;

    /* NEVER SHOW AGAIN */
    s_wizard.btnNever.generic.type     = MTYPE_PTEXT;
    s_wizard.btnNever.generic.flags    = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    s_wizard.btnNever.generic.id       = ID_WIZARD_NEVER;
    s_wizard.btnNever.generic.callback = LadderWizard_MenuEvent;
    s_wizard.btnNever.generic.x        = WIZARD_SCREEN_W / 2 + 135;
    s_wizard.btnNever.generic.y        = WIZARD_BTN_Y;
    s_wizard.btnNever.string           = WIZARD_TEXT_NEVER_SHOW;
    s_wizard.btnNever.style            = UI_CENTER | UI_SMALLFONT;
    s_wizard.btnNever.color            = wizardText;

    Menu_AddItem( &s_wizard.menu, &s_wizard.ownerName );
    Menu_AddItem( &s_wizard.menu, &s_wizard.ownerEmail );
    Menu_AddItem( &s_wizard.menu, &s_wizard.btnYes );
    Menu_AddItem( &s_wizard.menu, &s_wizard.btnNo );
    Menu_AddItem( &s_wizard.menu, &s_wizard.btnNever );

    LadderWizard_UpdateButtons();
    Menu_SetCursorToItem( &s_wizard.menu, &s_wizard.ownerName );

    uis.transitionIn  = 0;
    uis.transitionOut = 0;

    UI_PushMenu( &s_wizard.menu );
}

/* ── Draw ────────────────────────────────────────────────────────────────────── */

static void LadderWizard_Draw( void ) {
    int cx = WIZARD_SCREEN_W / 2;
    int ty = WIZARD_PANEL_Y + 60;
    int contentLeft;
    int labelY = WIZARD_FORM_TOP_Y + WIZARD_FORM_LABEL_OFFSET_Y;

    LadderWizard_ComputeFormLayout( &contentLeft, NULL, NULL, NULL );

    /* modal backdrop: static menu background + wizard-owned dimming */
    UI_FillRect( 0, 0, WIZARD_SCREEN_W, WIZARD_SCREEN_H, wizardDim );

    UI_FillRect( WIZARD_PANEL_X, WIZARD_PANEL_Y,
                 WIZARD_PANEL_W, WIZARD_PANEL_H, wizardBg );
    UI_DrawRect( WIZARD_PANEL_X, WIZARD_PANEL_Y,
                 WIZARD_PANEL_W, WIZARD_PANEL_H, wizardBorder );

    UI_DrawProportionalString( cx, WIZARD_PANEL_Y + 18,
                               "Q3RALLY LADDER",
                               UI_CENTER | UI_SMALLFONT, wizardTitle );

    Menu_Draw( &s_wizard.menu );

    if ( s_wizard.page == WIZARD_PAGE_CONFIRM ) {
        UI_DrawString( cx, ty,
            WIZARD_TEXT_CONFIRM_QUESTION,
            UI_CENTER | UI_SMALLFONT, wizardText );
        UI_DrawString( cx, ty + 16,
            va( "Server name: %s", s_wizard.serverName ),
            UI_CENTER | UI_SMALLFONT, wizardAccent );
        if ( s_wizard.serverNameNotice[0] ) {
            UI_DrawString( cx, ty + 32,
                s_wizard.serverNameNotice,
                UI_CENTER | UI_SMALLFONT, wizardError );
        }

        UI_DrawString( contentLeft, labelY,
            WIZARD_TEXT_OWNER_LABEL, UI_LEFT | UI_SMALLFONT, wizardText );
        UI_DrawString( contentLeft, labelY + WIZARD_FORM_ROW_H,
            WIZARD_TEXT_EMAIL_LABEL, UI_LEFT | UI_SMALLFONT, wizardText );

        if ( s_wizard.submitting ) {
            UI_DrawString( cx, ty + 96,
                WIZARD_TEXT_SUBMITTING,
                UI_CENTER | UI_SMALLFONT, wizardText );
        } else if ( s_wizard.statusLine[0] ) {
            UI_DrawString( cx, ty + 96,
                s_wizard.statusLine,
                UI_CENTER | UI_SMALLFONT,
                s_wizard.result == WIZARD_RESULT_PENDING ? wizardText : wizardError );
        } else {
            UI_DrawString( cx, ty + 96,
                WIZARD_TEXT_CANCEL_HINT,
                UI_CENTER | UI_SMALLFONT, wizardText );
            UI_DrawString( cx, ty + 110,
                WIZARD_TEXT_NEVER_HINT,
                UI_CENTER | UI_SMALLFONT, wizardText );
        }
    } else if ( s_wizard.result == WIZARD_RESULT_SUCCESS ) {
        /* Vertically center the three lines in the panel.
         * Panel content area: WIZARD_PANEL_Y+40 … WIZARD_BTN_Y
         * Three lines at 0 / +20 / +36, total height ~46px            */
        int sty = WIZARD_PANEL_Y + ( WIZARD_PANEL_H - 48 - 40 - 46 ) / 2 + 40;
        UI_DrawString( cx, sty,
            "Registration successful!",
            UI_CENTER | UI_SMALLFONT, wizardAccent );
        UI_DrawString( cx, sty + 20,
            "Your client is now registered with the ladder.",
            UI_CENTER | UI_SMALLFONT, wizardText );
        UI_DrawString( cx, sty + 36,
            "Settings have been saved automatically.",
            UI_CENTER | UI_SMALLFONT, wizardText );
    } else {
        UI_DrawString( cx, ty,
            WIZARD_TEXT_FAILED,
            UI_CENTER | UI_SMALLFONT, wizardError );
        if ( s_wizard.statusLine[0] ) {
            UI_DrawString( cx, ty + 16,
                s_wizard.statusLine,
                UI_CENTER | UI_SMALLFONT, wizardText );
        }
        UI_DrawString( cx, ty + 46,
            WIZARD_TEXT_FAILED_HINT,
            UI_CENTER | UI_SMALLFONT, wizardText );
    }
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
            trap_Cvar_SetValue( "ladder_wizard_dismissed", 1 );
            trap_Cvar_Update( &ui_ladderWizardDismissed );
            trap_Print( S_COLOR_YELLOW WIZARD_TEXT_DISMISSED_LOG );
            UI_PopMenu();
        }
        break;
    }
}

/* Called by engine when ladder_register succeeds */
void UI_LadderWizard_OnSuccess( const char *key ) {
    if ( !key || !key[0] ) {
        trap_Print( S_COLOR_YELLOW "Ladder wizard: registration response missing API key.\n" );
        UI_LadderWizard_OnError( "Server returned no API key." );
        return;
    }

    /* sv_ladderEnabled, sv_ladderUrl, sv_ladderApiKey and sv_hostname are
     * set by SV_LadderFinishRegister in engine code – the UI VM lacks write
     * access to protected server cvars.  writeconfig is also triggered from
     * there, after all cvars are set.                                       */
    trap_Cvar_SetValue( "ladder_wizard_completed", 1 );
    trap_Cvar_Update( &ui_ladderWizardCompleted );

    Q_strncpyz( s_wizard.apiKey, key, sizeof( s_wizard.apiKey ) );
    trap_Print( va( "Ladder wizard: registration succeeded, key prefix=%.8s...\n", key ) );
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
