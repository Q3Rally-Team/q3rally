/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2025 Q3Rally Team (Per Thormann - q3rally@gmail.com)

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

/* simple proportional text wrapper (C89) */
static void UI_DrawWrappedProportional( int x, int y, int maxWidth, int lineHeight,
                                        const char *text, int style, vec4_t color ) {
    char line[512];
    char word[128];
    char test[640];
    const char *s;
    int w;

    if ( !text || !*text ) {
        return;
    }

    line[0] = '\0';
    s = text;

    while ( *s ) {
        int wi = 0;

        /* handle manual line breaks */
        if ( *s == '\n' ) {
            UI_DrawProportionalString( x, y, line, style, color );
            y += lineHeight;
            line[0] = '\0';
            s++;
            continue;
        }

        /* read next word */
        while ( *s && *s != ' ' && *s != '\n' && wi < (int)sizeof(word) - 1 ) {
            word[wi++] = *s++;
        }
        word[wi] = '\0';

        /* build test string: current line + space + word */
        Q_strncpyz( test, line, sizeof( test ) );
        if ( line[0] ) {
            Q_strcat( test, sizeof( test ), " " );
        }
        Q_strcat( test, sizeof( test ), word );

        w = UI_ProportionalStringWidth( test );
        if ( w > maxWidth && line[0] ) {
            /* draw current line and start a new one with the word */
            UI_DrawProportionalString( x, y, line, style, color );
            y += lineHeight;
            Q_strncpyz( line, word, sizeof( line ) );
        } else {
            /* append word to line */
            Q_strncpyz( line, test, sizeof( line ) );
        }

        /* skip a single space after the word */
        if ( *s == ' ' ) {
            s++;
        }
    }

    /* draw remaining text */
    if ( line[0] ) {
        UI_DrawProportionalString( x, y, line, style, color );
    }
}

#define NAME_BUFSIZE 64
#define DESC_BUFSIZE 128
#define MAX_VISIBLE_BOTS 10
#define DESC_MAXWIDTH 750
#define DESC_LINEHEIGHT 16

/* semi-transparent background color */
static vec4_t garage_background = { 0.0f, 0.0f, 0.0f, 0.25f };

static char botNames[MAX_BOTS][NAME_BUFSIZE];
static char botModels[MAX_BOTS][NAME_BUFSIZE];
static char botAIFiles[MAX_BOTS][NAME_BUFSIZE];
static char botDescriptions[MAX_BOTS][DESC_BUFSIZE];
static char botPersonalities[MAX_BOTS][DESC_BUFSIZE];
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
    if (event != QM_ACTIVATED) {
        return;
    }
    UI_PopMenu();
}

static void UI_BotsMenu_BotSelectEvent(void *ptr, int event) {
    int i;
    if (event != QM_ACTIVATED) {
        return;
    }
    for (i = 0; i < MAX_VISIBLE_BOTS; i++) {
        if (ptr == &botItems[i]) {
            int newSelected = botPage * MAX_VISIBLE_BOTS + i;
            if (newSelected >= 0 && newSelected < botCount) {
                botSelected = newSelected;
                Com_Printf("[DEBUG] Bot selected: %d (%s)\n", botSelected, botNames[botSelected]);
                UI_PlayerInfo_SetModel(&s_garagePlayerInfo, botModels[botSelected], NULL, NULL, NULL);
            }
            break;
        }
    }
    UI_BotsMenu_DrawBotPage();
}

static void UI_BotsMenu_NextPage(void *ptr, int event) {
    if (event != QM_ACTIVATED) {
        return;
    }
    if ((botPage + 1) * MAX_VISIBLE_BOTS < botCount) {
        botPage++;
        /* select first bot on the new page */
        botSelected = botPage * MAX_VISIBLE_BOTS;
        if (botSelected >= botCount) {
            botSelected = -1;
        } else {
            UI_PlayerInfo_SetModel(&s_garagePlayerInfo, botModels[botSelected], NULL, NULL, NULL);
        }
        Com_Printf("[DEBUG] Page changed to: %d\n", botPage);
        UI_BotsMenu_DrawBotPage();
    }
}

static void UI_BotsMenu_PrevPage(void *ptr, int event) {
    if (event != QM_ACTIVATED) {
        return;
    }
    if (botPage > 0) {
        botPage--;
        /* select first bot on the new page */
        botSelected = botPage * MAX_VISIBLE_BOTS;
        if (botSelected >= botCount) {
            botSelected = -1;
        } else {
            UI_PlayerInfo_SetModel(&s_garagePlayerInfo, botModels[botSelected], NULL, NULL, NULL);
        }
        Com_Printf("[DEBUG] Page changed to: %d\n", botPage);
        UI_BotsMenu_DrawBotPage();
    }
}

static sfxHandle_t UI_BotsMenu_Key(int key) {
    return Menu_DefaultKey(&s_bots.menu, key);
}

static void UI_BotsMenu_Draw(void) {
    int boxX = 0;
    int boxY = 100;
    int boxW = 640;
    int boxH = 300;

    UI_FillRect(boxX, boxY, boxW, boxH, garage_background);

    Menu_Draw(&s_bots.menu);

    if (botSelected >= 0 && botSelected < botCount) {
        UI_DrawProportionalString( 20, 300, botPersonalities[botSelected], UI_LEFT | UI_SMALLFONT, colorCyan);
        UI_DrawWrappedProportional( 20, 320, DESC_MAXWIDTH, DESC_LINEHEIGHT, botDescriptions[botSelected], UI_LEFT | UI_SMALLFONT, colorYellow );
        
        // Draw 3D model of selected bot
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
    if (!f) {
        return;
    }

    buffer = UI_Alloc(len + 1);
    trap_FS_Read(buffer, len, f);
    buffer[len] = '\0';
    trap_FS_FCloseFile(f);

    text_p = buffer;
    botCount = 0;

    while (1) {
        token = COM_ParseExt(&text_p, qtrue);
        if (!token[0]) {
            break;
        }

        if (Q_stricmp(token, "{")) {
            break;
        }

        name[0] = '\0';
        model[0] = '\0';
        aifile[0] = '\0';
        description[0] = '\0';
        personality[0] = '\0';

        while (1) {
            token = COM_ParseExt(&text_p, qtrue);
            if (!token[0]) {
                break;
            }
            if (!Q_stricmp(token, "}")) {
                break;
            }

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
            botCount++;
        }
    }
}

static void UI_BotsMenu_DrawBotPage(void) {
    int i;
    int start;
    int y;
    int index;

    memset(s_bots.menu.items, 0, sizeof(s_bots.menu.items));
    s_bots.menu.nitems = 0;

    start = botPage * MAX_VISIBLE_BOTS;
    y = 120;

    for (i = 0; i < MAX_VISIBLE_BOTS; i++) {
        index = start + i;

        if (index < botCount) {
            botItems[i].generic.type = MTYPE_PTEXT;
            botItems[i].generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS | QMF_MOUSEONLY | QMF_HIGHLIGHT_IF_FOCUS;
            botItems[i].generic.x = 25;
            botItems[i].generic.y = y;
            botItems[i].generic.callback = UI_BotsMenu_BotSelectEvent;
            botItems[i].generic.id = i;
            botItems[i].style = UI_SMALLFONT;
            botItems[i].string = botNames[index];
            botItems[i].color = (index == botSelected) ? color_yellow : color_white;
            Menu_AddItem(&s_bots.menu, &botItems[i]);
            y += 16;
        } else {
            botItems[i].generic.flags = QMF_INACTIVE;
            botItems[i].string = "";
        }
    }

    prevButton.generic.type = MTYPE_PTEXT;
    prevButton.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    prevButton.generic.x = 15;
    prevButton.generic.y = 380;
    prevButton.generic.callback = UI_BotsMenu_PrevPage;
    prevButton.string = "< PREV";
    prevButton.color = color_white;
    prevButton.style = UI_LEFT | UI_SMALLFONT;
    Menu_AddItem(&s_bots.menu, &prevButton);

    nextButton.generic.type = MTYPE_PTEXT;
    nextButton.generic.flags = QMF_RIGHT_JUSTIFY | QMF_PULSEIFFOCUS;
    nextButton.generic.x = 455;
    nextButton.generic.y = 380;
    nextButton.generic.callback = UI_BotsMenu_NextPage;
    nextButton.string = "NEXT >";
    nextButton.color = color_white;
    nextButton.style = UI_RIGHT | UI_SMALLFONT;
    Menu_AddItem(&s_bots.menu, &nextButton);

    Menu_AddItem(&s_bots.menu, &s_bots.banner);
    Menu_AddItem(&s_bots.menu, &s_bots.back);
}

void UI_BotsMenu(void) {
    UI_BotsMenu_Init();
    UI_PushMenu(&s_bots.menu);
}

static void UI_BotsMenu_Init(void) {
    UI_BotsMenu_ParseBots();

    memset(&s_bots, 0, sizeof(s_bots));

    s_bots.menu.wrapAround = qtrue;
    s_bots.menu.fullscreen = qtrue;
    s_bots.menu.draw = UI_BotsMenu_Draw;
    s_bots.menu.key = UI_BotsMenu_Key;

    s_bots.banner.generic.type   = MTYPE_BTEXT;
    s_bots.banner.generic.flags  = QMF_CENTER_JUSTIFY;
    s_bots.banner.generic.x      = 320;
    s_bots.banner.generic.y      = 16;
    s_bots.banner.string         = "THE GARAGE";
    s_bots.banner.color          = colorWhite;
    s_bots.banner.style          = UI_CENTER;

    s_bots.back.generic.type     = MTYPE_PTEXT;
    s_bots.back.generic.flags    = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    s_bots.back.generic.x        = 15;
    s_bots.back.generic.y        = 440;
    s_bots.back.generic.callback = UI_BotsMenu_BackEvent;
    s_bots.back.string           = "< BACK";
    s_bots.back.color            = colorWhite;
    s_bots.back.style            = UI_LEFT | UI_SMALLFONT;

    /* initial page build */
    UI_BotsMenu_DrawBotPage();

    /* auto-select first bot if available, and rebuild so highlight is correct */
    if (botCount > 0) {
        botSelected = 0;
        UI_PlayerInfo_SetModel(&s_garagePlayerInfo, botModels[0], NULL, NULL, NULL);

        /* rebuild list so the first entry is highlighted (yellow) */
        UI_BotsMenu_DrawBotPage();
    }
}

