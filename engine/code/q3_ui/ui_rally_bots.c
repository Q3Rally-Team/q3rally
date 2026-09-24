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

#include "ui_local.h"
#include "ui_rally_frontend.h"
static qhandle_t UI_GenerateBotPlateShader( const char *botName, int botIndex, char *plateShaderName, int plateShaderNameSize ) {
    char          output[MAX_QPATH];
    char          shaderName[MAX_QPATH];
    qhandle_t     h;

    Com_sprintf( output, sizeof(output), "models/players/plates/player%d.tga", botIndex );
    Com_sprintf( shaderName, sizeof(shaderName), "models/players/plates/player%d", botIndex );
    CreateLicensePlateImage( "models/players/plates/usa_california.tga", output, botName, 10 );

    h = trap_R_RegisterShader( shaderName );
    if ( plateShaderName && plateShaderNameSize > 0 ) {
        Q_strncpyz( plateShaderName, va("player%d", botIndex), plateShaderNameSize );
    }
    Com_Printf( "Q3R UI Plate: bot %d (%s) -> %s (handle %d)\n", botIndex, botName, shaderName, h );
    return h;
}

/* forward declaration — defined later in this file */
static void UI_CanonicalizeWeapon( const char *in, char *out, int outSize );

/* ==========================================================
   WEAPON TEXT -> weapon_t MAPPING
   ========================================================== */

typedef struct { const char *canon; weapon_t wp; } weaponEnumMap_t;
static const weaponEnumMap_t s_weaponEnumMap[] = {
    { "GAUNTLET",        WP_GAUNTLET        },
    { "CHAINSAW",        WP_GAUNTLET        },
    { "MACHINEGUN",      WP_MACHINEGUN      },
    { "SHOTGUN",         WP_SHOTGUN         },
    { "GRENADELAUNCHER", WP_GRENADE_LAUNCHER},
    { "ROCKETLAUNCHER",  WP_ROCKET_LAUNCHER },
    { "LIGHTNING",       WP_LIGHTNING       },
    { "RAILGUN",         WP_RAILGUN         },
    { "PLASMAGUN",       WP_PLASMAGUN       },
    { "BFG10K",          WP_BFG             },
    { "FLAMETHROWER",    WP_FLAME_THROWER   },
};
#define WEAPONENUMMAP_COUNT ((int)(sizeof(s_weaponEnumMap)/sizeof(s_weaponEnumMap[0])))

static weapon_t UI_WeaponEnumFromText( const char *favoriteText ) {
    char canon[32];
    int  i;
    if ( !favoriteText || !*favoriteText ) return WP_MACHINEGUN;
    UI_CanonicalizeWeapon( favoriteText, canon, sizeof(canon) );
    for ( i = 0; i < WEAPONENUMMAP_COUNT; i++ ) {
        if ( Q_stricmp( s_weaponEnumMap[i].canon, canon ) == 0 )
            return s_weaponEnumMap[i].wp;
    }
    return WP_MACHINEGUN;   /* sensible default */
}

/* ==========================================================
   Icon helper: models/players/<model>/icon_<skin>
   - Do not pass a file extension (engine resolves .tga/.jpg)
   - Fallback order: icon_<skin> -> icon_default -> placeholder
   --------------------------------------------------------- */
static qhandle_t UI_LoadModelIconFor( const char *modelSkin ) {
    char model[MAX_QPATH] = "roadster";
    char skin [MAX_QPATH] = "default";
    const char *slash = NULL;
    char path[MAX_QPATH];
    qhandle_t h;

    if (modelSkin && *modelSkin) {
        slash = strchr(modelSkin, '/');
        if (slash) {
            int len = (int)(slash - modelSkin);
            if (len >= (int)sizeof(model)) len = (int)sizeof(model) - 1;
            Q_strncpyz(model, modelSkin, len + 1);
            Q_strncpyz(skin, slash + 1, sizeof(skin));
        } else {
            Q_strncpyz(model, modelSkin, sizeof(model));
        }
    }

    Com_sprintf(path, sizeof(path), "models/players/%s/icon_%s", model, skin);
    h = trap_R_RegisterShaderNoMip(path);
    if (h) return h;

    Com_sprintf(path, sizeof(path), "models/players/%s/icon_default", model);
    h = trap_R_RegisterShaderNoMip(path);
    if (h) return h;

    return trap_R_RegisterShaderNoMip("menu/art/unknownbot");
}

/* Canonicalize free-text weapon names from bots.txt into tokens like "ROCKETLAUNCHER" */
static void UI_CanonicalizeWeapon( const char *in, char *out, int outSize ) {
    int i = 0, j = 0;
    char c;

    if (!out || outSize <= 0) {
        return;
    }
    out[0] = '\0';

    if (!in || !*in) {
        return;
    }

    /* Uppercase letters, drop spaces/hyphens/underscores; keep digits (rare) */
    while ((c = in[i++]) != '\0' && j < outSize - 1) {
        if (c == ' ' || c == '-' || c == '_') {
            continue;
        }
        if (c >= 'a' && c <= 'z') {
            c = (char)(c - 'a' + 'A');
        }
        out[j++] = c;
    }
    out[j] = '\0';

    /* Strip leading "WEAPON" if present */
    if (!Q_stricmpn(out, "WEAPON", 6)) {
        int k = 0;
        while (out[6 + k]) { out[k] = out[6 + k]; k++; }
        out[k] = '\0';
    }
}

/* Canonical token -> icon shader (icons/iconw_*) */
typedef struct { const char *canon; const char *icon; } weaponiconmap_t;
static const weaponiconmap_t s_weaponIconMap[] = {
    { "GAUNTLET",        "icons/iconw_gauntlet" },  /* Chainsaw alias uses same icon */
    { "CHAINSAW",        "icons/iconw_gauntlet" },
    { "MACHINEGUN",      "icons/iconw_machinegun" },
    { "SHOTGUN",         "icons/iconw_shotgun" },
    { "GRENADELAUNCHER", "icons/iconw_grenade" },
    { "ROCKETLAUNCHER",  "icons/iconw_rocket" },
    { "LIGHTNING",       "icons/iconw_lightning" },
    { "RAILGUN",         "icons/iconw_railgun" },
    { "PLASMAGUN",       "icons/iconw_plasma" },
    { "BFG10k",          "icons/iconw_bfg" },
    { "FLAMETHROWER",    "icons/iconw_flame" }
};
#define WEAPONICONMAP_COUNT ( (int)(sizeof(s_weaponIconMap)/sizeof(s_weaponIconMap[0])) )

/* Resolve icon handle from favoriteweapon text */
static qhandle_t UI_WeaponIconFromText( const char *favoriteText ) {
    char canon[32]; int i;
    if (!favoriteText || !*favoriteText) return 0;
    UI_CanonicalizeWeapon(favoriteText, canon, sizeof(canon));
    if (!canon[0]) return 0;
    for (i = 0; i < WEAPONICONMAP_COUNT; i++) {
        if (Q_stricmp(s_weaponIconMap[i].canon, canon) == 0) {
            return trap_R_RegisterShaderNoMip(s_weaponIconMap[i].icon);
        }
    }
    return 0;
}

/* Small word wrapper for the modern frontend text atlas. */
static void UI_DrawWrappedProportional( int x, int y, int maxWidth, int lineHeight,
                                        const char *text, int style, vec4_t color ) {
    char line[1024];
    char word[256];
    char test[1024];
    const char *s;
    int wi;
    int w;

    if (!text || !*text) {
        return;
    }

    line[0] = '\0';
    s = text;

    while (*s) {
        /* respect explicit newlines */
        if (*s == '\n') {
            if (line[0]) {
                Frontend_DrawText(x, y, line, style, color);
                line[0] = '\0';
            }
            y += lineHeight;
            s++;
            continue;
        }

        /* read next word */
        wi = 0;
        while (*s && *s != ' ' && *s != '\n' && wi < (int)sizeof(word) - 1) {
            word[wi++] = *s++;
        }
        word[wi] = '\0';

        /* collapse multiple spaces */
        while (*s == ' ') s++;

        if (!word[0]) continue;

        if (line[0]) {
            Q_strncpyz(test, line, sizeof(test));
            Q_strcat(test, sizeof(test), " ");
            Q_strcat(test, sizeof(test), word);
        } else {
            Q_strncpyz(test, word, sizeof(test));
        }

        w = Frontend_TextWidth(test, style);
        if (w <= maxWidth) {
            if (line[0]) Q_strcat(line, sizeof(line), " ");
            Q_strcat(line, sizeof(line), word);
        } else {
            if (line[0]) {
                Frontend_DrawText(x, y, line, style, color);
                y += lineHeight;
                Q_strncpyz(line, word, sizeof(line));
            } else {
                /* extremely long single word: draw anyway */
                Frontend_DrawText(x, y, word, style, color);
                y += lineHeight;
            }
        }
    }

    if (line[0]) {
        Frontend_DrawText(x, y, line, style, color);
    }
}

#define NAME_BUFSIZE 64
#define DESC_BUFSIZE 256
#define MAX_VISIBLE_BOTS 10
#define DESC_MAXWIDTH 332
#define DESC_LINEHEIGHT 18

/* Modern rivals layout. Coordinates use the shared 640x480 virtual space. */
#define RIVALS_FRAME_X       4
#define RIVALS_FRAME_Y       20
#define RIVALS_FRAME_W       632
#define RIVALS_FRAME_H       440
#define RIVALS_LIST_X        20
#define RIVALS_LIST_Y        100
#define RIVALS_LIST_W        204
#define RIVALS_LIST_H        304
#define RIVALS_ROW_X         32
#define RIVALS_ROW_Y         134
#define RIVALS_ROW_W         180
#define RIVALS_ROW_H         22
#define RIVALS_ROW_GAP       3
#define RIVALS_HERO_X        236
#define RIVALS_HERO_Y        88
#define RIVALS_HERO_W        400
#define RIVALS_HERO_H        328
#define RIVALS_DETAIL_X      252
#define RIVALS_DETAIL_Y      316
#define RIVALS_ACTION_Y      424
#define RIVALS_ACTION_H      24
#define RIVALS_ACTION_W      112
#define RIVALS_BACK_X        20
#define RIVALS_PREV_X        264
#define RIVALS_NEXT_X        504

/* control IDs */
#define ID_BOT0  1000
#define ID_PREV  2000
#define ID_NEXT  2001
#define ID_BACK  10

static vec4_t rivalsTextColor  = UI_FRONTEND_COLOR_TEXT;
static vec4_t rivalsMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t rivalsAccentColor = UI_FRONTEND_COLOR_ACCENT;

/* Draw weapon icon to the right of the car icon; derives position from given rect (no layout change) */
static void UI_DrawWeaponIconNextTo( int x, int y, int w, int h, const char *favoriteText ) {
    qhandle_t wi;
    if (!favoriteText || !*favoriteText) return;
    wi = UI_WeaponIconFromText(favoriteText);
    if (!wi) return;
    {
#define WEAPON_ICON_DY 32
    int wx = x + w + 24;
    int wy = y + WEAPON_ICON_DY;
        UI_DrawHandlePic(wx, wy, 40, 40, wi);
    }
}

static char botNames[MAX_BOTS][NAME_BUFSIZE];
static char botModels[MAX_BOTS][NAME_BUFSIZE];
static char botAIFiles[MAX_BOTS][NAME_BUFSIZE];
static char botDescriptions[MAX_BOTS][DESC_BUFSIZE];
static char botPersonalities[MAX_BOTS][DESC_BUFSIZE];
static char botFavWeapon[MAX_BOTS][DESC_BUFSIZE];
static char botPlateNames[MAX_BOTS][NAME_BUFSIZE];
static qhandle_t botIcons[MAX_BOTS];
static qhandle_t botPlateShaders[MAX_BOTS];
static weapon_t  botWeapons[MAX_BOTS];
static int botCount = 0;
static int botPage = 0;
static int botSelected = -1;

static menutext_s botItems[MAX_VISIBLE_BOTS];
static menutext_s nextButton;
static menutext_s prevButton;

typedef struct {
    menuframework_s menu;
    menutext_s      banner;
    menutext_s      back;
} botsmenu_t;

static botsmenu_t s_bots;
static playerInfo_t s_garagePlayerInfo;

static void UI_BotsMenu_Init(void);
static void UI_BotsMenu_DrawBotPage(void);

/* Apply model, weapon and plate for a selected rival */
static void UI_BotsMenu_SetRival( int index ) {
    vec3_t viewAngles  = { 0, 180, 0 };
    vec3_t moveAngles  = { 0,   0, 0 };
    char   plate[MAX_QPATH];
    weapon_t wp;

    if ( index < 0 || index >= botCount ) return;

    wp = botWeapons[index];
    /* The Chainsaw is represented by WP_GAUNTLET for gameplay, but its
     * hand-weapon preview is not meaningful on a car's tag_weapon mount.
     * Keep the favorite weapon in the profile text/icon and render the car
     * without an attached weapon to avoid entering that legacy path. */
    if ( wp == WP_GAUNTLET ) {
        wp = WP_NONE;
    }

    /*
     * Reload/invalidate point for RIVALS:
     * regenerate and re-register plate shader whenever a rival is (re)selected
     * so the latest generated file is always bound.
     */
    botPlateShaders[index] = 0;
    botPlateShaders[index] = UI_GenerateBotPlateShader( botNames[index], index, botPlateNames[index], sizeof(botPlateNames[index]) );

    if ( botPlateNames[index][0] ) {
        Q_strncpyz( plate, botPlateNames[index], sizeof(plate) );
    } else {
        trap_Cvar_VariableStringBuffer( "plate", plate, sizeof(plate) );
        if ( !plate[0] ) Q_strncpyz( plate, "usa_california", sizeof(plate) );
    }

    UI_PlayerInfo_SetModel( &s_garagePlayerInfo, botModels[index],
                            DEFAULT_RIM, DEFAULT_HEAD, plate );

    /* Ensure plateShader points to the exact freshly-generated shader handle. */
    if ( botPlateShaders[index] ) {
        Com_Printf( "RIVALS: Bot %d uses generated plate '%s' with marker '%s' (handle %d)\n",
                    index, botPlateNames[index], plate, botPlateShaders[index] );
        s_garagePlayerInfo.plateShader = botPlateShaders[index];
    }

    UI_PlayerInfo_SetInfo( &s_garagePlayerInfo,
        LEGS_IDLE, TORSO_STAND,
        viewAngles, moveAngles,
        wp, qfalse );
}

static void UI_BotsMenu_BackEvent(void *ptr, int event) {
    if (event != QM_ACTIVATED) return;
    UI_PopMenu();
}

static void UI_BotsMenu_BotSelectEvent(void *ptr, int event) {
    int i;
    int index;

    if (event != QM_ACTIVATED) return;

    for (i = 0; i < MAX_VISIBLE_BOTS; i++) {
        if ((void*)&botItems[i] == ptr) {
            index = botPage * MAX_VISIBLE_BOTS + i;
            if (index >= 0 && index < botCount) {
                botSelected = index;
                UI_BotsMenu_SetRival( botSelected );
                UI_BotsMenu_DrawBotPage();
            }
            break;
        }
    }
}


static void UI_BotsMenu_NextPage(void *ptr, int event) {
    int start;
    if (event != QM_ACTIVATED) return;
    if ((botPage + 1) * MAX_VISIBLE_BOTS < botCount) {
        botPage++;
        start = botPage * MAX_VISIBLE_BOTS;
        if (botSelected < start || botSelected >= start + MAX_VISIBLE_BOTS) {
            botSelected = (start < botCount) ? start : botCount - 1;
            if (botSelected >= 0) {
                UI_BotsMenu_SetRival( botSelected );
            }
        }
        UI_BotsMenu_DrawBotPage();
    }
}

static void UI_BotsMenu_PrevPage(void *ptr, int event) {
    int start;
    if (event != QM_ACTIVATED) return;
    if (botPage > 0) {
        botPage--;
        start = botPage * MAX_VISIBLE_BOTS;
        if (botSelected < start || botSelected >= start + MAX_VISIBLE_BOTS) {
            botSelected = (start < botCount) ? start : botCount - 1;
            if (botSelected >= 0) {
                UI_BotsMenu_SetRival( botSelected );
            }
        }
        UI_BotsMenu_DrawBotPage();
    }
}

static sfxHandle_t UI_BotsMenu_Key(int key) {
    int row;
    int index;

    /* The legacy menu mouse router uses label-derived bounds for PText
     * widgets. Rival rows are deliberately full-width custom cards, so map
     * their hit area explicitly before the generic router can fall through
     * to the Back action. */
    if ( key == K_MOUSE1 &&
         uis.cursorx >= RIVALS_ROW_X &&
         uis.cursorx <= RIVALS_ROW_X + RIVALS_ROW_W &&
         uis.cursory >= RIVALS_ROW_Y &&
         uis.cursory < RIVALS_ROW_Y + MAX_VISIBLE_BOTS *
             ( RIVALS_ROW_H + RIVALS_ROW_GAP ) ) {
        row = ( uis.cursory - RIVALS_ROW_Y ) /
              ( RIVALS_ROW_H + RIVALS_ROW_GAP );
        if ( uis.cursory >= RIVALS_ROW_Y + row *
             ( RIVALS_ROW_H + RIVALS_ROW_GAP ) + RIVALS_ROW_H ) {
            return 0;
        }

        index = botPage * MAX_VISIBLE_BOTS + row;
        if ( row >= 0 && row < MAX_VISIBLE_BOTS &&
             index >= 0 && index < botCount ) {
            botSelected = index;
            UI_BotsMenu_SetRival( botSelected );
            UI_BotsMenu_DrawBotPage();
            return menu_move_sound;
        }
    }

    return Menu_DefaultKey(&s_bots.menu, key);
}

static void UI_BotsMenu_DrawBanner( void *self ) {
    Frontend_DrawText( RIVALS_FRAME_X + 24, RIVALS_FRAME_Y + 24,
                       "Rivals", UI_LEFT | UI_BIGFONT, rivalsTextColor );
    Frontend_DrawText( RIVALS_FRAME_X + 24, RIVALS_FRAME_Y + 48,
                       "Choose a driver to inspect their setup",
                       UI_LEFT | UI_SMALLFONT, rivalsMutedColor );
    Frontend_DrawStatusChip( RIVALS_FRAME_X + RIVALS_FRAME_W - 94,
                             RIVALS_FRAME_Y + 26, "Roster",
                             rivalsAccentColor, 1.0f );
}

static void UI_BotsMenu_DrawRivalItem( void *self ) {
    menutext_s *item;
    int index;

    item = (menutext_s *)self;
    if ( item->generic.flags & QMF_INACTIVE ) {
        return;
    }

    index = 0;
    while ( index < MAX_VISIBLE_BOTS && &botItems[index] != item ) {
        index++;
    }
    if ( index >= MAX_VISIBLE_BOTS ) {
        return;
    }

    Frontend_DrawNavButton( item->generic.left, item->generic.top,
                            item->generic.right - item->generic.left,
                            item->generic.bottom - item->generic.top,
                            item->string, 1.0f,
                            botPage * MAX_VISIBLE_BOTS + index == botSelected,
                            UI_FRONTEND_TEXT_LEFT );
}

static void UI_BotsMenu_DrawAction( void *self ) {
    menutext_s *button;
    qboolean focus;
    qboolean disabled;
    vec4_t disabledColor;
    int x;
    int y;
    int width;

    button = (menutext_s *)self;
    focus = ( Menu_ItemAtCursor( button->generic.parent ) == button );
    disabled = ( button->generic.flags & QMF_GRAYED ) ? qtrue : qfalse;
    x = button->generic.left;
    y = button->generic.top;
    width = button->generic.right - button->generic.left;

    if ( disabled ) {
        Vector4Copy( rivalsMutedColor, disabledColor );
        disabledColor[3] = 0.35f;
        Frontend_DrawText( x + width / 2, y + 6, button->string,
                           UI_CENTER | UI_SMALLFONT, disabledColor );
        return;
    }

    Frontend_DrawButton( x, y, width,
                         button->generic.bottom - button->generic.top,
                         button->string, 1.0f, focus,
                         UI_FRONTEND_TEXT_CENTER );
}

static void UI_BotsMenu_Draw(void) {
    vec4_t scrimColor = UI_FRONTEND_COLOR_SCRIM;
    char pageStr[32];
    const char *favoriteWeapon;

    Frontend_DrawBackground( scrimColor );

    Frontend_DrawPanel( RIVALS_FRAME_X, RIVALS_FRAME_Y,
                        RIVALS_FRAME_W, RIVALS_FRAME_H, 1.0f,
                        UI_FRONTEND_STYLE_FRAME );
    Frontend_DrawCard( RIVALS_LIST_X, RIVALS_LIST_Y,
                       RIVALS_LIST_W, RIVALS_LIST_H, 1.0f, qfalse );
    Frontend_DrawCard( RIVALS_HERO_X, RIVALS_HERO_Y,
                       RIVALS_HERO_W, RIVALS_HERO_H, 1.0f, qfalse );
    Frontend_DrawText( RIVALS_LIST_X + 12, RIVALS_LIST_Y + 14,
                       "Rival roster", UI_LEFT | UI_SMALLFONT,
                       rivalsMutedColor );
    Frontend_DrawText( RIVALS_HERO_X + 16, RIVALS_HERO_Y + 14,
                       "Rival profile", UI_LEFT | UI_SMALLFONT,
                       rivalsMutedColor );

    Menu_Draw( &s_bots.menu );

    if ( botCount > MAX_VISIBLE_BOTS ) {
        int totalPages = ( botCount + MAX_VISIBLE_BOTS - 1 ) / MAX_VISIBLE_BOTS;
        Com_sprintf( pageStr, sizeof(pageStr), "Page %d of %d",
                     botPage + 1, totalPages );
        Frontend_DrawText( RIVALS_LIST_X + RIVALS_LIST_W / 2,
                           RIVALS_LIST_Y + RIVALS_LIST_H - 20, pageStr,
                           UI_CENTER | UI_SMALLFONT, rivalsMutedColor );
    }

    if ( botSelected >= 0 && botSelected < botCount ) {
        favoriteWeapon = botFavWeapon[botSelected][0] ?
                         botFavWeapon[botSelected] : "-";

        UI_DrawPlayer( RIVALS_HERO_X + 16, RIVALS_HERO_Y + 28,
                       RIVALS_HERO_W - 32, 204,
                       &s_garagePlayerInfo, uis.realtime );

        Frontend_DrawText( RIVALS_DETAIL_X, RIVALS_DETAIL_Y,
                           botNames[botSelected], UI_LEFT | UI_BIGFONT,
                           rivalsTextColor );
        Frontend_DrawText( RIVALS_DETAIL_X, RIVALS_DETAIL_Y + 22,
                           botPersonalities[botSelected], UI_LEFT | UI_SMALLFONT,
                           rivalsMutedColor );
        Frontend_DrawText( RIVALS_DETAIL_X, RIVALS_DETAIL_Y + 42,
                           va( "Favorite weapon  /  %s", favoriteWeapon ),
                           UI_LEFT | UI_SMALLFONT, rivalsAccentColor );
        UI_DrawWeaponIconNextTo( RIVALS_HERO_X + RIVALS_HERO_W - 104,
                                 RIVALS_DETAIL_Y, 40, 40,
                                 botFavWeapon[botSelected][0] ?
                                 botFavWeapon[botSelected] : NULL );
        UI_DrawWrappedProportional( RIVALS_DETAIL_X, RIVALS_DETAIL_Y + 62,
                                    DESC_MAXWIDTH, DESC_LINEHEIGHT,
                                    botDescriptions[botSelected],
                                    UI_LEFT | UI_SMALLFONT, rivalsMutedColor );
    } else {
        Frontend_DrawText( RIVALS_HERO_X + RIVALS_HERO_W / 2,
                           RIVALS_HERO_Y + 150, "Select a rival",
                           UI_CENTER | UI_BIGFONT, rivalsMutedColor );
    }
}

static void UI_BotsMenu_ParseBots(void) {
    char *text_p;
    char *token;
    char *buffer;
    int len;
    fileHandle_t f;
    char name[NAME_BUFSIZE];
    char model[NAME_BUFSIZE];
    char aifile[NAME_BUFSIZE];
    char description[DESC_BUFSIZE];
    char personality[DESC_BUFSIZE];
    char favoriteweapon[DESC_BUFSIZE];

    len = trap_FS_FOpenFile("scripts/bots.txt", &f, FS_READ);
    if (!f) return;
    buffer = (char *)UI_Alloc(len + 1);
    trap_FS_Read(buffer, len, f);
    buffer[len] = '\0';
    trap_FS_FCloseFile(f);

    text_p = buffer;
    botCount = 0;

    while (1) {
        token = COM_ParseExt(&text_p, qtrue);
        if (!token[0]) break;
        if (token[0] != '{') continue;

        name[0] = '\0';
        model[0] = '\0';
        aifile[0] = '\0';
        description[0] = '\0';
        personality[0] = '\0';
        favoriteweapon[0] = '\0';

        while (1) {
            token = COM_ParseExt(&text_p, qtrue);
            if (!token[0]) break;
            if (token[0] == '}') break;

            if (!Q_stricmp(token, "name")) {
                token = COM_ParseExt(&text_p, qfalse);
                Q_strncpyz(name, token, NAME_BUFSIZE);
            } else if (!Q_stricmp(token, "model")) {
                token = COM_ParseExt(&text_p, qfalse);
                Q_strncpyz(model, token, NAME_BUFSIZE);
            } else if (!Q_stricmp(token, "aifile")) {
                token = COM_ParseExt(&text_p, qfalse);
                Q_strncpyz(aifile, token, NAME_BUFSIZE);
            } else if (!Q_stricmp(token, "description")) {
                token = COM_ParseExt(&text_p, qfalse);
                Q_strncpyz(description, token, DESC_BUFSIZE);
            } else if (!Q_stricmp(token, "personality")) {
                token = COM_ParseExt(&text_p, qfalse);
                Q_strncpyz(personality, token, DESC_BUFSIZE);
            } else if (!Q_stricmp(token, "favoriteweapon")) {
                token = COM_ParseExt(&text_p, qfalse);
                Q_strncpyz(favoriteweapon, token, DESC_BUFSIZE);
            } else {
                token = COM_ParseExt(&text_p, qfalse);
            }
        }

        if (botCount < MAX_BOTS) {
            Q_strncpyz(botNames[botCount], name, NAME_BUFSIZE);
            Q_strncpyz(botModels[botCount], model, NAME_BUFSIZE);
            Q_strncpyz(botAIFiles[botCount], aifile, NAME_BUFSIZE);
            Q_strncpyz(botDescriptions[botCount], description, DESC_BUFSIZE);
            Q_strncpyz(botPersonalities[botCount], personality, DESC_BUFSIZE);
            Q_strncpyz(botFavWeapon[botCount], favoriteweapon, DESC_BUFSIZE);
            botPlateNames[botCount][0] = '\0';
            botIcons[botCount]        = UI_LoadModelIconFor(model);
            botWeapons[botCount]      = UI_WeaponEnumFromText(favoriteweapon);
            botPlateShaders[botCount] = 0;
            
            botCount++;
        }
    }
}

static void UI_BotsMenu_DrawBotPage(void) {
    int start, i, index;
    int rowY;

    start = botPage * MAX_VISIBLE_BOTS;
    for (i = 0; i < MAX_VISIBLE_BOTS; i++) {
        index = start + i;
        rowY = RIVALS_ROW_Y + i * ( RIVALS_ROW_H + RIVALS_ROW_GAP );
        if (index < botCount) {
            botItems[i].string = botNames[index];
            botItems[i].color  = (index == botSelected) ? rivalsAccentColor : rivalsTextColor;
            botItems[i].generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
            botItems[i].generic.left   = RIVALS_ROW_X;
            botItems[i].generic.top    = rowY;
            botItems[i].generic.right  = RIVALS_ROW_X + RIVALS_ROW_W;
            botItems[i].generic.bottom = rowY + RIVALS_ROW_H;
        } else {
            botItems[i].generic.flags = QMF_INACTIVE;
            botItems[i].generic.left = RIVALS_ROW_X;
            botItems[i].generic.right = RIVALS_ROW_X + RIVALS_ROW_W;
            botItems[i].generic.top = rowY;
            botItems[i].generic.bottom = rowY + RIVALS_ROW_H;
            botItems[i].string = "";
            botItems[i].color  = rivalsTextColor;
        }
    }
    prevButton.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    nextButton.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    if ( botPage <= 0 ) {
        prevButton.generic.flags |= QMF_GRAYED;
    }
    if ( ( botPage + 1 ) * MAX_VISIBLE_BOTS >= botCount ) {
        nextButton.generic.flags |= QMF_GRAYED;
    }
}

void UI_BotsMenu(void) {
    memset(&s_bots, 0, sizeof(s_bots));
    s_bots.menu.key        = UI_BotsMenu_Key;
    s_bots.menu.draw       = UI_BotsMenu_Draw;
    s_bots.menu.fullscreen = qtrue;
    UI_BotsMenu_Init();
    UI_PushMenu(&s_bots.menu);
}

static void UI_BotsMenu_Init(void) {
    int i;
    trap_Cvar_Set("cg_viewsize", "100");
    s_bots.menu.wrapAround = qtrue;
    s_bots.menu.fullscreen = qtrue;

    s_bots.banner.generic.type  = MTYPE_BTEXT;
    s_bots.banner.generic.x     = RIVALS_FRAME_X + 24;
    s_bots.banner.generic.y     = RIVALS_FRAME_Y + 24;
    s_bots.banner.generic.flags = QMF_INACTIVE;
    s_bots.banner.string        = "Rivals";
    s_bots.banner.color         = rivalsTextColor;
    s_bots.banner.style         = UI_LEFT | UI_BIGFONT;
    s_bots.banner.generic.ownerdraw = UI_BotsMenu_DrawBanner;

    UI_BotsMenu_ParseBots();
    UI_PlayerInfo_SetModel(&s_garagePlayerInfo, "roadster/blue",
                           DEFAULT_RIM, DEFAULT_HEAD, DEFAULT_PLATE );

    Menu_AddItem(&s_bots.menu, &s_bots.banner);

    for (i = 0; i < MAX_VISIBLE_BOTS; i++) {
        botItems[i].generic.type = MTYPE_PTEXT;
        botItems[i].generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
        botItems[i].generic.x = RIVALS_ROW_X;
        botItems[i].generic.y = RIVALS_ROW_Y + i * ( RIVALS_ROW_H + RIVALS_ROW_GAP );
        botItems[i].generic.id = ID_BOT0 + i;
        botItems[i].generic.callback = UI_BotsMenu_BotSelectEvent;
        botItems[i].string = "";
        botItems[i].style = UI_LEFT | UI_SMALLFONT;
        botItems[i].color = rivalsTextColor;
        botItems[i].generic.ownerdraw = UI_BotsMenu_DrawRivalItem;
        Menu_AddItem(&s_bots.menu, &botItems[i]);
    }

    prevButton.generic.type = MTYPE_PTEXT;
    prevButton.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    prevButton.generic.x = 320;
    prevButton.generic.y = RIVALS_ACTION_Y;
    prevButton.generic.id = ID_PREV;
    prevButton.generic.callback = UI_BotsMenu_PrevPage;
    prevButton.string = "Previous";
    prevButton.color = rivalsTextColor;
    prevButton.style = UI_CENTER | UI_SMALLFONT;
    prevButton.generic.ownerdraw = UI_BotsMenu_DrawAction;

    nextButton.generic.type = MTYPE_PTEXT;
    nextButton.generic.flags = QMF_RIGHT_JUSTIFY | QMF_PULSEIFFOCUS;
    nextButton.generic.x = 544;
    nextButton.generic.y = RIVALS_ACTION_Y;
    nextButton.generic.id = ID_NEXT;
    nextButton.generic.callback = UI_BotsMenu_NextPage;
    nextButton.string = "Next";
    nextButton.color = rivalsTextColor;
    nextButton.style = UI_CENTER | UI_SMALLFONT;
    nextButton.generic.ownerdraw = UI_BotsMenu_DrawAction;

    s_bots.back.generic.type     = MTYPE_PTEXT;
    s_bots.back.generic.flags    = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    s_bots.back.generic.x        = RIVALS_FRAME_X + 72;
    s_bots.back.generic.y        = RIVALS_ACTION_Y;
    s_bots.back.generic.id       = ID_BACK;
    s_bots.back.generic.callback = UI_BotsMenu_BackEvent;
    s_bots.back.string           = "Back";
    s_bots.back.color            = rivalsTextColor;
    s_bots.back.style            = UI_CENTER | UI_SMALLFONT;
    s_bots.back.generic.ownerdraw = UI_BotsMenu_DrawAction;
    Menu_AddItem(&s_bots.menu, &s_bots.back);
    Menu_AddItem(&s_bots.menu, &prevButton);
    Menu_AddItem(&s_bots.menu, &nextButton);

    /* Keep the three navigation actions on a shared flat button grid. */
    s_bots.back.generic.left = RIVALS_BACK_X;
    s_bots.back.generic.top = RIVALS_ACTION_Y;
    s_bots.back.generic.right = RIVALS_BACK_X + RIVALS_ACTION_W;
    s_bots.back.generic.bottom = RIVALS_ACTION_Y + RIVALS_ACTION_H;
    prevButton.generic.left = RIVALS_PREV_X;
    prevButton.generic.top = RIVALS_ACTION_Y;
    prevButton.generic.right = RIVALS_PREV_X + RIVALS_ACTION_W;
    prevButton.generic.bottom = RIVALS_ACTION_Y + RIVALS_ACTION_H;
    nextButton.generic.left = RIVALS_NEXT_X;
    nextButton.generic.top = RIVALS_ACTION_Y;
    nextButton.generic.right = RIVALS_NEXT_X + RIVALS_ACTION_W;
    nextButton.generic.bottom = RIVALS_ACTION_Y + RIVALS_ACTION_H;

    if (botCount > 0) {
        botSelected = 0;
        UI_BotsMenu_SetRival( 0 );
    }
    UI_BotsMenu_DrawBotPage();
}
