/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2025 Q3Rally Team

This file is part of q3rally source code and is distributed under the GPLv2+.
===========================================================================
*/

#include "ui_local.h"

/* ---------------------------------------------------------
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

/* simple proportional text wrapper (C89) */
static void UI_DrawWrappedProportional( int x, int y, int maxWidth, int lineHeight,
                                        const char *text, int style, vec4_t color ) {
    char line[1024];
    char word[256];
    char test[1024];
    const char *s;
    float scale;
    int wi;
    int w;

    if (!text || !*text) {
        return;
    }

    line[0] = '\0';
    s = text;
    scale = UI_ProportionalSizeScale(style);

    while (*s) {
        /* respect explicit newlines */
        if (*s == '\n') {
            if (line[0]) {
                UI_DrawProportionalString(x, y, line, style, color);
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

        w = (int)(UI_ProportionalStringWidth(test) * scale);
        if (w <= maxWidth) {
            if (line[0]) Q_strcat(line, sizeof(line), " ");
            Q_strcat(line, sizeof(line), word);
        } else {
            if (line[0]) {
                UI_DrawProportionalString(x, y, line, style, color);
                y += lineHeight;
                Q_strncpyz(line, word, sizeof(line));
            } else {
                /* extremely long single word: draw anyway */
                UI_DrawProportionalString(x, y, word, style, color);
                y += lineHeight;
            }
        }
    }

    if (line[0]) {
        UI_DrawProportionalString(x, y, line, style, color);
    }
}

#define NAME_BUFSIZE 64
#define DESC_BUFSIZE 128
#define MAX_VISIBLE_BOTS 10
#define DESC_MAXWIDTH 750
#define DESC_LINEHEIGHT 16

/* control IDs */
#define ID_BOT0  1000
#define ID_PREV  2000
#define ID_NEXT  2001
#define ID_BACK  10

/* semi-transparent background color */
static vec4_t garage_background = { 0.0f, 0.0f, 0.0f, 0.25f };

static char botNames[MAX_BOTS][NAME_BUFSIZE];
static char botModels[MAX_BOTS][NAME_BUFSIZE];
static char botAIFiles[MAX_BOTS][NAME_BUFSIZE];
static char botDescriptions[MAX_BOTS][DESC_BUFSIZE];
static char botPersonalities[MAX_BOTS][DESC_BUFSIZE];
static qhandle_t botIcons[MAX_BOTS];
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
                UI_PlayerInfo_SetModel(&s_garagePlayerInfo, botModels[botSelected], NULL, NULL, NULL);
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
                UI_PlayerInfo_SetModel(&s_garagePlayerInfo, botModels[botSelected], NULL, NULL, NULL);
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
                UI_PlayerInfo_SetModel(&s_garagePlayerInfo, botModels[botSelected], NULL, NULL, NULL);
            }
        }
        UI_BotsMenu_DrawBotPage();
    }
}

static sfxHandle_t UI_BotsMenu_Key(int key) {
    return Menu_DefaultKey(&s_bots.menu, key);
}

static void UI_BotsMenu_Draw(void) {
    int boxX = 0, boxY = 100, boxW = 640, boxH = 360;
    UI_FillRect(boxX, boxY, boxW, boxH, garage_background);
    Menu_Draw(&s_bots.menu);
    if (botSelected >= 0 && botSelected < botCount) {
        if (botIcons[botSelected]) {
            UI_DrawHandlePic(330, 375, 92, 92, botIcons[botSelected]);
        }
        UI_DrawProportionalString(20, 320, botPersonalities[botSelected], UI_LEFT | UI_SMALLFONT, colorCyan);
        UI_DrawWrappedProportional( 20, 340, DESC_MAXWIDTH, DESC_LINEHEIGHT, botDescriptions[botSelected], UI_LEFT | UI_SMALLFONT, colorYellow );
        UI_DrawPlayer(270, 0, 425, 425, &s_garagePlayerInfo, uis.realtime);
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
            botIcons[botCount] = UI_LoadModelIconFor(model);
            
            botCount++;
        }
    }
}

static void UI_BotsMenu_DrawBotPage(void) {
    int start, i, index;
    int tw;
    float sscale;

    start = botPage * MAX_VISIBLE_BOTS;
    for (i = 0; i < MAX_VISIBLE_BOTS; i++) {
        index = start + i;
        if (index < botCount) {
            botItems[i].string = botNames[index];
            botItems[i].color  = (index == botSelected) ? color_yellow : color_white;
            botItems[i].generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS | QMF_MOUSEONLY;
            sscale = UI_ProportionalSizeScale(botItems[i].style);
            tw = (int)(UI_ProportionalStringWidth(botItems[i].string) * sscale) + 8;
            botItems[i].generic.left   = botItems[i].generic.x;
            botItems[i].generic.top    = botItems[i].generic.y - 10;
            botItems[i].generic.right  = botItems[i].generic.x + tw;
            botItems[i].generic.bottom = botItems[i].generic.y + 10;
        } else {
            botItems[i].generic.flags = QMF_INACTIVE;
            botItems[i].generic.left = botItems[i].generic.right = botItems[i].generic.x;
            botItems[i].generic.top = botItems[i].generic.bottom = botItems[i].generic.y;
            botItems[i].string = "";
            botItems[i].color  = color_white;
        }
    }
    prevButton.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    nextButton.generic.flags = QMF_RIGHT_JUSTIFY | QMF_PULSEIFFOCUS;
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
    s_bots.banner.generic.x     = 320;
    s_bots.banner.generic.y     = 40;
    s_bots.banner.string        = "THE GARAGE";
    s_bots.banner.color         = color_white;
    s_bots.banner.style         = UI_CENTER;

    UI_BotsMenu_ParseBots();
    UI_PlayerInfo_SetModel(&s_garagePlayerInfo, "roadster/blue", NULL, NULL, NULL);

    Menu_AddItem(&s_bots.menu, &s_bots.banner);

    for (i = 0; i < MAX_VISIBLE_BOTS; i++) {
        botItems[i].generic.type = MTYPE_PTEXT;
        botItems[i].generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS | QMF_MOUSEONLY;
        botItems[i].generic.x = 15;
        botItems[i].generic.y = 140 + i * 16;
        botItems[i].generic.id = ID_BOT0 + i;
        botItems[i].generic.callback = UI_BotsMenu_BotSelectEvent;
        botItems[i].string = "";
        botItems[i].style = UI_LEFT | UI_SMALLFONT;
        botItems[i].color = color_white;
        Menu_AddItem(&s_bots.menu, &botItems[i]);
    }

    prevButton.generic.type = MTYPE_PTEXT;
    prevButton.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    prevButton.generic.x = 95;
    prevButton.generic.y = 415;
    prevButton.generic.id = ID_PREV;
    prevButton.generic.callback = UI_BotsMenu_PrevPage;
    prevButton.string = "< PREV";
    prevButton.color = color_white;
    prevButton.style = UI_LEFT | UI_SMALLFONT;
    Menu_AddItem(&s_bots.menu, &prevButton);

    nextButton.generic.type = MTYPE_PTEXT;
    nextButton.generic.flags = QMF_RIGHT_JUSTIFY | QMF_PULSEIFFOCUS;
    nextButton.generic.x = 620;
    nextButton.generic.y = 415;
    nextButton.generic.id = ID_NEXT;
    nextButton.generic.callback = UI_BotsMenu_NextPage;
    nextButton.string = "NEXT >";
    nextButton.color = color_white;
    nextButton.style = UI_RIGHT | UI_SMALLFONT;
    Menu_AddItem(&s_bots.menu, &nextButton);

    s_bots.back.generic.type     = MTYPE_PTEXT;
    s_bots.back.generic.flags    = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    s_bots.back.generic.x        = 15;
    s_bots.back.generic.y        = 415;
    s_bots.back.generic.id       = ID_BACK;
    s_bots.back.generic.callback = UI_BotsMenu_BackEvent;
    s_bots.back.string           = "< BACK";
    s_bots.back.color            = color_white;
    s_bots.back.style            = UI_LEFT | UI_SMALLFONT;
    Menu_AddItem(&s_bots.menu, &s_bots.back);

    if (botCount > 0) {
        botSelected = 0;
        UI_PlayerInfo_SetModel(&s_garagePlayerInfo, botModels[0], NULL, NULL, NULL);
    }
    UI_BotsMenu_DrawBotPage();
}
