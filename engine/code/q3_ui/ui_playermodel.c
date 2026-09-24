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

// STONELANCE
/*
#define MODEL_BACK0			"menu/art/back_0"
#define MODEL_BACK1			"menu/art/back_1"
#define MODEL_SELECT		"menu/art/opponents_select"
#define MODEL_SELECTED		"menu/art/opponents_selected"
#define MODEL_FRAMEL		"menu/art/frame1_l"
#define MODEL_FRAMER		"menu/art/frame1_r"
#define MODEL_PORTS			"menu/art/player_models_ports"
#define MODEL_ARROWS		"menu/art/gs_arrows_0"
#define MODEL_ARROWSL		"menu/art/gs_arrows_l"
#define MODEL_ARROWSR		"menu/art/gs_arrows_r"
*/
#define MODEL_SELECT		"menu/art/menu_select"
#define MODEL_SELECTED		"menu/art/menu_selected"
#define MODEL_ARROWSL0		"menu/art/arrow_l0"
#define MODEL_ARROWSR0		"menu/art/arrow_r0"
#define MODEL_ARROWSL		"menu/art/arrow_l1"
#define MODEL_ARROWSR		"menu/art/arrow_r1"
#define MODEL_PORT			"menu/art/menu_port"
// END

#define LOW_MEMORY			(5 * 1024 * 1024)

static char* playermodel_artlist[] =
{
// STONELANCE
/*
	MODEL_BACK0,	
	MODEL_BACK1,	
	MODEL_SELECT,
	MODEL_SELECTED,
	MODEL_FRAMEL,
	MODEL_FRAMER,
	MODEL_PORTS,
	MODEL_ARROWS,
	MODEL_ARROWSL,
	MODEL_ARROWSR,
*/
	MODEL_SELECT,
	MODEL_SELECTED,
	MODEL_ARROWSL0,
	MODEL_ARROWSR0,
	MODEL_ARROWSL,
	MODEL_ARROWSR,
	MODEL_PORT,
// END
	NULL
};

#define PLAYERGRID_COLS		4
// STONELANCE
//#define PLAYERGRID_ROWS		4
#define PLAYERGRID_ROWS		1
// END
#define MAX_MODELSPERPAGE	(PLAYERGRID_ROWS*PLAYERGRID_COLS)

// STONELANCE
#define HEADGRID_COLS		1
#define HEADGRID_ROWS		2
#define MAX_HEADSPERPAGE	(HEADGRID_ROWS*HEADGRID_COLS)

#define RIMGRID_COLS		2
#define RIMGRID_ROWS		2
#define MAX_RIMSPERPAGE		(RIMGRID_ROWS*RIMGRID_COLS)

#define NUM_FAVORITES		4
#define MAX_HEADMODELS		256
#define MAX_RIMMODELS		256
// END

#define MAX_PLAYERMODELS	256

#define ID_PLAYERPIC0		0
#define ID_PLAYERPIC1		1
#define ID_PLAYERPIC2		2
#define ID_PLAYERPIC3		3
// STONELANCE
/*
#define ID_PLAYERPIC4		4
#define ID_PLAYERPIC5		5
#define ID_PLAYERPIC6		6
#define ID_PLAYERPIC7		7
#define ID_PLAYERPIC8		8
#define ID_PLAYERPIC9		9
#define ID_PLAYERPIC10		10
#define ID_PLAYERPIC11		11
#define ID_PLAYERPIC12		12
#define ID_PLAYERPIC13		13
#define ID_PLAYERPIC14		14
#define ID_PLAYERPIC15		15
*/
#define ID_HEADPIC0			6
#define ID_HEADPIC1			7
#define ID_RIMPIC0			10
#define ID_RIMPIC1			11
#define ID_RIMPIC2			12
#define ID_RIMPIC3			13
// END

#define ID_PREVPAGE			100
#define ID_NEXTPAGE			101
#define ID_BACK				102
// STONELANCE
#define ID_PREVRIMPAGE		103
#define ID_NEXTRIMPAGE		104
#define ID_SPINMODEL		105
#define ID_FAVORITE1		106
#define ID_FAVORITE2		107
#define ID_FAVORITE3		108
#define ID_FAVORITE4		109
#define ID_PREVHEADPAGE		110
#define ID_NEXTHEADPAGE		111
// END

#define PLAYERMODEL_FRAME_X             24
#define PLAYERMODEL_FRAME_Y             24
#define PLAYERMODEL_FRAME_WIDTH         592
#define PLAYERMODEL_FRAME_HEIGHT        432
#define PLAYERMODEL_PREVIEW_X            40
#define PLAYERMODEL_PREVIEW_Y            104
#define PLAYERMODEL_PREVIEW_WIDTH       288
#define PLAYERMODEL_PREVIEW_HEIGHT      248
#define PLAYERMODEL_FAVORITES_X          40
#define PLAYERMODEL_FAVORITES_Y         364
#define PLAYERMODEL_FAVORITES_WIDTH     288
#define PLAYERMODEL_FAVORITES_HEIGHT     64
#define PLAYERMODEL_PAINT_X             344
#define PLAYERMODEL_PAINT_Y             104
#define PLAYERMODEL_PAINT_WIDTH         256
#define PLAYERMODEL_PAINT_HEIGHT        128
#define PLAYERMODEL_RIMS_X              344
#define PLAYERMODEL_RIMS_Y              240
#define PLAYERMODEL_RIMS_WIDTH          256
#define PLAYERMODEL_RIMS_HEIGHT         116
#define PLAYERMODEL_HEADS_X             344
#define PLAYERMODEL_HEADS_Y             364
#define PLAYERMODEL_HEADS_WIDTH         256
#define PLAYERMODEL_HEADS_HEIGHT         64

static vec4_t playerModelScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t playerModelTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t playerModelMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t playerModelStatusColor = UI_FRONTEND_COLOR_ACCENT;

typedef struct
{
	menuframework_s	menu;
// STONELANCE
	/*
	menubitmap_s	pics[MAX_MODELSPERPAGE];
	menubitmap_s	picbuttons[MAX_MODELSPERPAGE];
	menubitmap_s	framel;
	menubitmap_s	framer;
	menubitmap_s	ports;
	menutext_s		banner;
	menubitmap_s	back;
	menubitmap_s	player;
	menubitmap_s	arrows;
	menubitmap_s	left;
	menubitmap_s	right;
	menutext_s		modelname;
	menutext_s		skinname;
	menutext_s		playername;

	int				nummodels;
	char			modelnames[MAX_PLAYERMODELS][128];
	int				modelpage;
	int				numpages;
	char			modelskin[64];
	int				selectedmodel;
*/
	menutext_s		banner;
	
	menubitmap_s	ports[MAX_MODELSPERPAGE];
	menubitmap_s	pics[MAX_MODELSPERPAGE];
	menubitmap_s	picbuttons[MAX_MODELSPERPAGE];
	menubitmap_s	up;
	menubitmap_s	down;
	menutext_s		paintname;
	menutext_s		paintPrev;
	menutext_s		paintNext;

	menubitmap_s	rimports[MAX_RIMSPERPAGE];
	menubitmap_s	rimpics[MAX_RIMSPERPAGE];
	menubitmap_s	rimpicbuttons[MAX_RIMSPERPAGE];
	menubitmap_s	rimup;
	menubitmap_s	rimdown;
	menutext_s		rimname;
	menutext_s		rimPrev;
	menutext_s		rimNext;

	menubitmap_s	headports[MAX_HEADSPERPAGE];
	menubitmap_s	headpics[MAX_HEADSPERPAGE];
	menubitmap_s	headpicbuttons[MAX_HEADSPERPAGE];
	menubitmap_s	headup;
	menubitmap_s	headdown;
	menutext_s		headPrev;
	menutext_s		headNext;

	menutext_s		back;

	menubitmap_s	favpics[NUM_FAVORITES];
	menubitmap_s	favpicbuttons[NUM_FAVORITES];
	menubitmap_s	favports[NUM_FAVORITES];
	menutext_s		favorites;

	menubitmap_s	player;
// END

	playerInfo_t	playerinfo;

// STONELANCE
	int				nummodels;
	char			modelList[MAX_PLAYERMODELS][MAX_QPATH];
	char			modelIcons[MAX_MODELSPERPAGE][MAX_QPATH];
	int				modelpage;
	int				numpages;
	char			modelskin[MAX_QPATH];
	char			modelname[MAX_QPATH];
	int				selectedmodel;

	int				numRims;
	char			rimList[MAX_RIMMODELS][MAX_QPATH];
	char			rimIcons[MAX_RIMSPERPAGE][MAX_QPATH];
	int				rimPage;
	int				numRimPages;
	char			rimskin[MAX_QPATH];
	int				selectedRim;

	int				numHeads;
	char			headList[MAX_HEADMODELS][MAX_QPATH];
	char			headIcons[MAX_HEADSPERPAGE][MAX_QPATH];
	int				headPage;
	int				numHeadPages;
	char			headskin[MAX_QPATH];
	int				selectedHead;

	char			favIcons[NUM_FAVORITES][MAX_QPATH];
	profile_info_t		profileInfo;
// END
} playermodel_t;

static playermodel_t s_playermodel;

static void PlayerModel_MenuEvent( void *ptr, int event );


static qboolean PlayerModel_ItemFocused( int id ) {
	menucommon_s *item;

	item = (menucommon_s *)Menu_ItemAtCursor( &s_playermodel.menu );
	return ( item && item->id == id ) ? qtrue : qfalse;
}


static void PlayerModel_DrawPageButton( void *self ) {
	menutext_s *button;
	menucommon_s *item;
	qboolean disabled;
	qboolean focused;

	button = (menutext_s *)self;
	item = &button->generic;
	disabled = ( item->flags & QMF_INACTIVE ) ? qtrue : qfalse;
	focused = ( Menu_ItemAtCursor( item->parent ) == item );

	if ( disabled ) {
		Frontend_DrawText( (item->left + item->right) / 2,
			item->top + (item->bottom - item->top - SMALLCHAR_HEIGHT) / 2,
			button->string, UI_CENTER | UI_SMALLFONT, playerModelMutedColor );
		return;
	}

	Frontend_DrawButton( item->left, item->top,
		item->right - item->left, item->bottom - item->top,
		button->string, uis.tFrac, focused, UI_CENTER );
}


static void PlayerModel_InitPageButton( menutext_s *button, int id,
	const char *label, int x, int y, int width ) {
	button->generic.type = MTYPE_PTEXT;
	button->generic.flags = QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
	button->generic.id = id;
	button->generic.x = x + width / 2;
	button->generic.y = y;
	button->generic.left = x;
	button->generic.top = y;
	button->generic.right = x + width;
	button->generic.bottom = y + 20;
	button->generic.callback = PlayerModel_MenuEvent;
	button->generic.ownerdraw = PlayerModel_DrawPageButton;
	button->string = (char *)label;
	button->style = UI_CENTER | UI_SMALLFONT;
	button->color = text_color_normal;
}


static void PlayerModel_DrawBackButton( void *self ) {
	menutext_s *button;
	menucommon_s *item;
	qboolean focused;

	button = (menutext_s *)self;
	item = &button->generic;
	focused = ( Menu_ItemAtCursor( item->parent ) == item );
	Frontend_DrawNavButton( item->left, item->top,
		item->right - item->left, item->bottom - item->top,
		button->string, uis.tFrac, focused, UI_LEFT );
}


static void PlayerModel_DrawSelectionTile( int x, int y, int width,
	int height, int id, qboolean selected ) {
	Frontend_DrawCard( x - 3, y - 3, width + 6, height + 6,
		uis.tFrac, selected || PlayerModel_ItemFocused( id ) );
}


static void PlayerModel_ApplyLayout( void ) {
	int i;
	int x;
	int y;

	for ( i = 0; i < MAX_MODELSPERPAGE; i++ ) {
		x = 360 + i * 54;
		y = 147;
		s_playermodel.pics[i].generic.x = x + 2;
		s_playermodel.pics[i].generic.y = y + 2;
		s_playermodel.pics[i].width = 44;
		s_playermodel.pics[i].height = 44;
		s_playermodel.picbuttons[i].generic.x = x;
		s_playermodel.picbuttons[i].generic.y = y;
		s_playermodel.picbuttons[i].generic.left = x;
		s_playermodel.picbuttons[i].generic.top = y;
		s_playermodel.picbuttons[i].generic.right = x + 48;
		s_playermodel.picbuttons[i].generic.bottom = y + 48;
		s_playermodel.picbuttons[i].width = 48;
		s_playermodel.picbuttons[i].height = 48;
		s_playermodel.picbuttons[i].focuspic = NULL;
	}

	for ( i = 0; i < MAX_RIMSPERPAGE; i++ ) {
		x = 360 + i * 54;
		y = 283;
		s_playermodel.rimpics[i].generic.x = x + 2;
		s_playermodel.rimpics[i].generic.y = y + 2;
		s_playermodel.rimpics[i].width = 44;
		s_playermodel.rimpics[i].height = 44;
		s_playermodel.rimpicbuttons[i].generic.x = x;
		s_playermodel.rimpicbuttons[i].generic.y = y;
		s_playermodel.rimpicbuttons[i].generic.left = x;
		s_playermodel.rimpicbuttons[i].generic.top = y;
		s_playermodel.rimpicbuttons[i].generic.right = x + 48;
		s_playermodel.rimpicbuttons[i].generic.bottom = y + 48;
		s_playermodel.rimpicbuttons[i].width = 48;
		s_playermodel.rimpicbuttons[i].height = 48;
		s_playermodel.rimpicbuttons[i].focuspic = NULL;
	}

	for ( i = 0; i < MAX_HEADSPERPAGE; i++ ) {
		x = 360 + i * 54;
		y = 392;
		s_playermodel.headpics[i].generic.x = x + 3;
		s_playermodel.headpics[i].generic.y = y;
		s_playermodel.headpics[i].width = 32;
		s_playermodel.headpics[i].height = 30;
		s_playermodel.headpicbuttons[i].generic.x = x;
		s_playermodel.headpicbuttons[i].generic.y = y;
		s_playermodel.headpicbuttons[i].generic.left = x;
		s_playermodel.headpicbuttons[i].generic.top = y;
		s_playermodel.headpicbuttons[i].generic.right = x + 38;
		s_playermodel.headpicbuttons[i].generic.bottom = y + 30;
		s_playermodel.headpicbuttons[i].width = 38;
		s_playermodel.headpicbuttons[i].height = 30;
		s_playermodel.headpicbuttons[i].focuspic = NULL;
	}

	for ( i = 0; i < NUM_FAVORITES; i++ ) {
		x = 50 + i * 60;
		y = 386;
		s_playermodel.favpics[i].generic.x = x + 2;
		s_playermodel.favpics[i].generic.y = y + 2;
		s_playermodel.favpics[i].width = 32;
		s_playermodel.favpics[i].height = 32;
		s_playermodel.favpicbuttons[i].generic.x = x;
		s_playermodel.favpicbuttons[i].generic.y = y;
		s_playermodel.favpicbuttons[i].generic.left = x;
		s_playermodel.favpicbuttons[i].generic.top = y;
		s_playermodel.favpicbuttons[i].generic.right = x + 36;
		s_playermodel.favpicbuttons[i].generic.bottom = y + 36;
		s_playermodel.favpicbuttons[i].width = 36;
		s_playermodel.favpicbuttons[i].height = 36;
		s_playermodel.favpicbuttons[i].focuspic = NULL;
	}

	s_playermodel.player.generic.x = PLAYERMODEL_PREVIEW_X + 8;
	s_playermodel.player.generic.y = PLAYERMODEL_PREVIEW_Y + 26;
	s_playermodel.player.generic.left = PLAYERMODEL_PREVIEW_X + 8;
	s_playermodel.player.generic.right = PLAYERMODEL_PREVIEW_X +
		PLAYERMODEL_PREVIEW_WIDTH - 8;
	s_playermodel.player.generic.top = PLAYERMODEL_PREVIEW_Y + 26;
	s_playermodel.player.generic.bottom = PLAYERMODEL_PREVIEW_Y +
		PLAYERMODEL_PREVIEW_HEIGHT - 28;
	s_playermodel.player.width = PLAYERMODEL_PREVIEW_WIDTH - 16;
	s_playermodel.player.height = PLAYERMODEL_PREVIEW_HEIGHT - 54;

	PlayerModel_InitPageButton( &s_playermodel.paintPrev, ID_PREVPAGE,
		"PREV", 500, 108, 38 );
	PlayerModel_InitPageButton( &s_playermodel.paintNext, ID_NEXTPAGE,
		"NEXT", 546, 108, 42 );
	PlayerModel_InitPageButton( &s_playermodel.rimPrev, ID_PREVRIMPAGE,
		"PREV", 500, 244, 38 );
	PlayerModel_InitPageButton( &s_playermodel.rimNext, ID_NEXTRIMPAGE,
		"NEXT", 546, 244, 42 );
	PlayerModel_InitPageButton( &s_playermodel.headPrev, ID_PREVHEADPAGE,
		"PREV", 500, 368, 38 );
	PlayerModel_InitPageButton( &s_playermodel.headNext, ID_NEXTHEADPAGE,
		"NEXT", 546, 368, 42 );

	s_playermodel.back.generic.type = MTYPE_PTEXT;
	s_playermodel.back.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS |
		QMF_NODEFAULTINIT;
	s_playermodel.back.generic.id = ID_BACK;
	s_playermodel.back.generic.x = 56;
	s_playermodel.back.generic.y = 433;
	s_playermodel.back.generic.left = 40;
	s_playermodel.back.generic.top = 430;
	s_playermodel.back.generic.right = 136;
	s_playermodel.back.generic.bottom = 454;
	s_playermodel.back.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.back.generic.ownerdraw = PlayerModel_DrawBackButton;
	s_playermodel.back.string = "< BACK";
}


// STONELANCE (new function)
/*
=================
PlayerModel_DrawBackShaders
=================
*/
static void PlayerModel_DrawBackShaders( void ) {
	int i;
	int x;
	int y;
	int selected;

	Frontend_DrawBackground( playerModelScrimColor );
	Frontend_DrawPanel( PLAYERMODEL_FRAME_X, PLAYERMODEL_FRAME_Y,
		PLAYERMODEL_FRAME_WIDTH, PLAYERMODEL_FRAME_HEIGHT, uis.tFrac,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( PLAYERMODEL_FRAME_X + 24, PLAYERMODEL_FRAME_Y + 19,
		"Customize vehicle", UI_LEFT | UI_BIGFONT, playerModelTextColor );
	Frontend_DrawText( PLAYERMODEL_FRAME_X + 24, PLAYERMODEL_FRAME_Y + 45,
		"Choose a paint, wheel set and driver for your car",
		UI_LEFT | UI_SMALLFONT, playerModelMutedColor );
	Frontend_DrawStatusChip( PLAYERMODEL_FRAME_X + PLAYERMODEL_FRAME_WIDTH - 96,
		PLAYERMODEL_FRAME_Y + 22, "GARAGE", playerModelStatusColor, uis.tFrac );

	Frontend_DrawCard( PLAYERMODEL_PREVIEW_X, PLAYERMODEL_PREVIEW_Y,
		PLAYERMODEL_PREVIEW_WIDTH, PLAYERMODEL_PREVIEW_HEIGHT, uis.tFrac, qtrue );
	Frontend_DrawText( PLAYERMODEL_PREVIEW_X + 16,
		PLAYERMODEL_PREVIEW_Y + 12, "VEHICLE PREVIEW",
		UI_LEFT | UI_SMALLFONT, playerModelMutedColor );
	Frontend_DrawText( PLAYERMODEL_PREVIEW_X + 16,
		PLAYERMODEL_PREVIEW_Y + PLAYERMODEL_PREVIEW_HEIGHT - 20,
		"DRAG TO ROTATE", UI_LEFT | UI_SMALLFONT, playerModelMutedColor );

	Frontend_DrawCard( PLAYERMODEL_FAVORITES_X, PLAYERMODEL_FAVORITES_Y,
		PLAYERMODEL_FAVORITES_WIDTH, PLAYERMODEL_FAVORITES_HEIGHT,
		uis.tFrac, qfalse );
	Frontend_DrawText( PLAYERMODEL_FAVORITES_X + 12,
		PLAYERMODEL_FAVORITES_Y + 8, "SAVED BUILDS",
		UI_LEFT | UI_SMALLFONT, playerModelMutedColor );

	Frontend_DrawCard( PLAYERMODEL_PAINT_X, PLAYERMODEL_PAINT_Y,
		PLAYERMODEL_PAINT_WIDTH, PLAYERMODEL_PAINT_HEIGHT, uis.tFrac, qfalse );
	Frontend_DrawText( PLAYERMODEL_PAINT_X + 16, PLAYERMODEL_PAINT_Y + 12,
		"PAINT", UI_LEFT | UI_SMALLFONT, playerModelMutedColor );
	Frontend_DrawText( PLAYERMODEL_PAINT_X + 142,
		PLAYERMODEL_PAINT_Y + 12,
		va( "%d / %d", s_playermodel.modelpage + 1,
			s_playermodel.numpages ), UI_RIGHT | UI_SMALLFONT,
		playerModelMutedColor );

	Frontend_DrawCard( PLAYERMODEL_RIMS_X, PLAYERMODEL_RIMS_Y,
		PLAYERMODEL_RIMS_WIDTH, PLAYERMODEL_RIMS_HEIGHT, uis.tFrac, qfalse );
	Frontend_DrawText( PLAYERMODEL_RIMS_X + 16, PLAYERMODEL_RIMS_Y + 12,
		"WHEELS", UI_LEFT | UI_SMALLFONT, playerModelMutedColor );
	Frontend_DrawText( PLAYERMODEL_RIMS_X + 142,
		PLAYERMODEL_RIMS_Y + 12,
		va( "%d / %d", s_playermodel.rimPage + 1,
			s_playermodel.numRimPages ), UI_RIGHT | UI_SMALLFONT,
		playerModelMutedColor );

	Frontend_DrawCard( PLAYERMODEL_HEADS_X, PLAYERMODEL_HEADS_Y,
		PLAYERMODEL_HEADS_WIDTH, PLAYERMODEL_HEADS_HEIGHT,
		uis.tFrac, qfalse );
	Frontend_DrawText( PLAYERMODEL_HEADS_X + 16,
		PLAYERMODEL_HEADS_Y + 10, "DRIVER", UI_LEFT | UI_SMALLFONT,
		playerModelMutedColor );
	Frontend_DrawText( PLAYERMODEL_HEADS_X + 142,
		PLAYERMODEL_HEADS_Y + 10,
		va( "%d / %d", s_playermodel.headPage + 1,
			s_playermodel.numHeadPages ), UI_RIGHT | UI_SMALLFONT,
		playerModelMutedColor );

	for ( i = 0; i < MAX_MODELSPERPAGE; i++ ) {
		x = 360 + i * 54;
		y = 147;
		if ( !s_playermodel.modelIcons[i][0] ) {
			continue;
		}
		selected = ( s_playermodel.selectedmodel / MAX_MODELSPERPAGE ==
			s_playermodel.modelpage &&
			s_playermodel.selectedmodel % MAX_MODELSPERPAGE == i );
		PlayerModel_DrawSelectionTile( x, y, 48, 48,
			ID_PLAYERPIC0 + i, selected );
	}
	Frontend_DrawText( PLAYERMODEL_PAINT_X + 16,
		PLAYERMODEL_PAINT_Y + PLAYERMODEL_PAINT_HEIGHT - 18,
		s_playermodel.paintname.string ? s_playermodel.paintname.string : "",
		UI_LEFT | UI_SMALLFONT, playerModelTextColor );

	for ( i = 0; i < MAX_RIMSPERPAGE; i++ ) {
		x = 360 + i * 54;
		y = 283;
		if ( !s_playermodel.rimIcons[i][0] ) {
			continue;
		}
		selected = ( s_playermodel.selectedRim / MAX_RIMSPERPAGE ==
			s_playermodel.rimPage &&
			s_playermodel.selectedRim % MAX_RIMSPERPAGE == i );
		PlayerModel_DrawSelectionTile( x, y, 48, 48,
			ID_RIMPIC0 + i, selected );
	}
	Frontend_DrawText( PLAYERMODEL_RIMS_X + 16,
		PLAYERMODEL_RIMS_Y + PLAYERMODEL_RIMS_HEIGHT - 18,
		s_playermodel.rimname.string ? s_playermodel.rimname.string : "",
		UI_LEFT | UI_SMALLFONT, playerModelTextColor );

	for ( i = 0; i < MAX_HEADSPERPAGE; i++ ) {
		x = 360 + i * 54;
		y = 392;
		if ( !s_playermodel.headIcons[i][0] ) {
			continue;
		}
		selected = ( s_playermodel.selectedHead / MAX_HEADSPERPAGE ==
			s_playermodel.headPage &&
			s_playermodel.selectedHead % MAX_HEADSPERPAGE == i );
		PlayerModel_DrawSelectionTile( x, y, 38, 30,
			ID_HEADPIC0 + i, selected );
	}
	Frontend_DrawText( PLAYERMODEL_HEADS_X + 120,
		PLAYERMODEL_HEADS_Y + PLAYERMODEL_HEADS_HEIGHT - 18,
		s_playermodel.headskin, UI_LEFT | UI_SMALLFONT,
		playerModelTextColor );

	for ( i = 0; i < NUM_FAVORITES; i++ ) {
		x = 50 + i * 60;
		y = 386;
		PlayerModel_DrawSelectionTile( x, y, 36, 36,
			ID_FAVORITE1 + i, qfalse );
		if ( !s_playermodel.favpics[i].generic.name ) {
			Frontend_DrawText( x + 18, y + 12, "+",
				UI_CENTER | UI_SMALLFONT, playerModelMutedColor );
		}
	}

	Frontend_DrawText( PLAYERMODEL_FRAME_X + 182,
		PLAYERMODEL_FRAME_Y + PLAYERMODEL_FRAME_HEIGHT - 24,
		"Enter select     Esc back", UI_LEFT | UI_SMALLFONT,
		playerModelMutedColor );

	Menu_Draw( &s_playermodel.menu );
}


/*
=================
PlayerModel_UpdateFavorites

=================
*/
static void PlayerModel_UpdateFavorites( void ) {
	int				i;
	char		buf[MAX_QPATH];
	char		modelName[MAX_QPATH];
	char		skinName[MAX_QPATH];
	char		rimName[MAX_QPATH];
	char		headName[MAX_QPATH];
	qboolean	error;
	const profile_info_t *info = &s_playermodel.profileInfo;

	for ( i = 0; i < NUM_FAVORITES; i++ ) {
		const char *favoriteModel = NULL;
		const char *favoriteSkin = NULL;
		const char *favoriteRim = NULL;
		const char *favoriteHead = NULL;

		if ( info && info->favoriteCars[i].model[0] && info->favoriteCars[i].skin[0] ) {
			favoriteModel = info->favoriteCars[i].model;
			favoriteSkin = info->favoriteCars[i].skin;
			favoriteRim = info->favoriteCars[i].rim;
			favoriteHead = info->favoriteCars[i].head;
			Com_sprintf( buf, sizeof( buf ), "favoritecar%i", ( i + 1 ) );
			Com_sprintf( modelName, sizeof( modelName ), "%s/%s/%s/%s",
			            favoriteModel,
			            favoriteSkin,
			            favoriteRim ? favoriteRim : "",
			            favoriteHead ? favoriteHead : "" );
			trap_Cvar_Set( buf, modelName );
			Q_strncpyz( modelName, favoriteModel, sizeof( modelName ) );
			Q_strncpyz( skinName, favoriteSkin, sizeof( skinName ) );
		} else {
			Com_sprintf( buf, sizeof( buf ), "favoritecar%i", ( i + 1 ) );

			error = GetValuesFromFavorite( buf, modelName, skinName, rimName, headName );

			if ( !error ) {
				favoriteModel = modelName;
				favoriteSkin = skinName;
				favoriteRim = rimName;
				favoriteHead = headName;
			}
		}

		if ( favoriteModel && favoriteSkin ) {
			Com_sprintf( s_playermodel.favIcons[i], sizeof( s_playermodel.favIcons[i] ), "models/players/%s/icon_%s", favoriteModel, favoriteSkin );
			s_playermodel.favpics[i].generic.name = s_playermodel.favIcons[i];
		} else {
			s_playermodel.favpics[i].generic.name = NULL;
			Com_sprintf( buf, sizeof( buf ), "favoritecar%i", ( i + 1 ) );
			trap_Cvar_Set( buf, "" );
		}
		s_playermodel.favpics[i].shader = 0;
	}
}


/*
=================
PlayerModel_UpdateHeadGrid
=================
*/
static void PlayerModel_UpdateHeadGrid( void )
{
	int	i;
    int	j;

	//Com_Printf("PM: Updating head gird.\n");
	//Com_Printf("PM: Current Headpage, %i\n", s_playermodel.headpage);

	j = s_playermodel.headPage * MAX_HEADSPERPAGE;
	for (i=0; i<HEADGRID_ROWS*HEADGRID_COLS; i++,j++)
	{
		if (j < s_playermodel.numHeads)
		{ 
			// head portrait
			Com_sprintf( s_playermodel.headIcons[i], sizeof(s_playermodel.headIcons[i]), "models/players/heads/icon_%s", s_playermodel.headList[j]);
			s_playermodel.headpicbuttons[i].generic.flags &= ~QMF_INACTIVE;
		}
		else
		{
			// dead slot
			Q_strncpyz( s_playermodel.headIcons[i], "", sizeof(s_playermodel.headIcons[i]));
			s_playermodel.headpicbuttons[i].generic.flags |= QMF_INACTIVE;
		}

		s_playermodel.headports[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.headpics[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.headpics[i].shader               = 0;
 		s_playermodel.headpicbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	if (s_playermodel.selectedHead / MAX_HEADSPERPAGE == s_playermodel.headPage)
	{
		// set selected head
		i = s_playermodel.selectedHead % MAX_HEADSPERPAGE;

		s_playermodel.headports[i].generic.flags       |= QMF_HIGHLIGHT;
		s_playermodel.headpicbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;
	}

	if (s_playermodel.numHeadPages > 1)
	{
		if (s_playermodel.headPage > 0)
			s_playermodel.headup.generic.flags &= ~QMF_INACTIVE;
		else
			s_playermodel.headup.generic.flags |= QMF_INACTIVE;

		if (s_playermodel.headPage < s_playermodel.numHeadPages-1)
			s_playermodel.headdown.generic.flags &= ~QMF_INACTIVE;
		else
			s_playermodel.headdown.generic.flags |= QMF_INACTIVE;
	}
	else
	{
		// hide up/down markers
		s_playermodel.headup.generic.flags |= QMF_INACTIVE;
		s_playermodel.headdown.generic.flags |= QMF_INACTIVE;
	}
	if (s_playermodel.headPage > 0)
		s_playermodel.headPrev.generic.flags &= ~QMF_INACTIVE;
	else
		s_playermodel.headPrev.generic.flags |= QMF_INACTIVE;
	if (s_playermodel.headPage < s_playermodel.numHeadPages - 1)
		s_playermodel.headNext.generic.flags &= ~QMF_INACTIVE;
	else
		s_playermodel.headNext.generic.flags |= QMF_INACTIVE;
}


/*
=================
PlayerModel_UpdateRimGrid
=================
*/
static void PlayerModel_UpdateRimGrid( void )
{
	int	i;
    int	j;

	j = s_playermodel.rimPage * MAX_RIMSPERPAGE;
	for (i=0; i<RIMGRID_ROWS*RIMGRID_COLS; i++,j++)
	{
		if (j < s_playermodel.numRims)
		{ 
			// rim portrait
			Com_sprintf( s_playermodel.rimIcons[i], sizeof(s_playermodel.rimIcons[i]), "models/players/wheels/icon_%s", s_playermodel.rimList[j]);
			s_playermodel.rimpicbuttons[i].generic.flags &= ~QMF_INACTIVE;
		}
		else
		{
			// dead slot
 			Q_strncpyz( s_playermodel.rimIcons[i], "", sizeof(s_playermodel.rimIcons[i]));
			s_playermodel.rimpicbuttons[i].generic.flags |= QMF_INACTIVE;
		}

		s_playermodel.rimports[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.rimpics[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.rimpics[i].shader               = 0;
 		s_playermodel.rimpicbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	if (s_playermodel.selectedRim / MAX_RIMSPERPAGE == s_playermodel.rimPage)
	{
		// set selected rim
		i = s_playermodel.selectedRim % MAX_RIMSPERPAGE;

		s_playermodel.rimports[i].generic.flags       |= QMF_HIGHLIGHT;
		s_playermodel.rimpicbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;
	}

	if (s_playermodel.numRimPages > 1)
	{
		if (s_playermodel.rimPage > 0)
			s_playermodel.rimup.generic.flags &= ~QMF_INACTIVE;
		else
			s_playermodel.rimup.generic.flags |= QMF_INACTIVE;

		if (s_playermodel.rimPage < s_playermodel.numRimPages-1)
			s_playermodel.rimdown.generic.flags &= ~QMF_INACTIVE;
		else
			s_playermodel.rimdown.generic.flags |= QMF_INACTIVE;
	}
	else
	{
		// hide up/down markers
		s_playermodel.rimup.generic.flags |= QMF_INACTIVE;
		s_playermodel.rimdown.generic.flags |= QMF_INACTIVE;
	}
	if (s_playermodel.rimPage > 0)
		s_playermodel.rimPrev.generic.flags &= ~QMF_INACTIVE;
	else
		s_playermodel.rimPrev.generic.flags |= QMF_INACTIVE;
	if (s_playermodel.rimPage < s_playermodel.numRimPages - 1)
		s_playermodel.rimNext.generic.flags &= ~QMF_INACTIVE;
	else
		s_playermodel.rimNext.generic.flags |= QMF_INACTIVE;
}
// END


/*
=================
PlayerModel_UpdateGrid
=================
*/
static void PlayerModel_UpdateGrid( void )
{
	int	i;
    int	j;

	j = s_playermodel.modelpage * MAX_MODELSPERPAGE;
	for (i=0; i<PLAYERGRID_ROWS*PLAYERGRID_COLS; i++,j++)
	{
		if (j < s_playermodel.nummodels)
		{ 
			// model/skin portrait
// STONELANCE
// 			s_playermodel.pics[i].generic.name         = s_playermodel.modelnames[j];
			Com_sprintf( s_playermodel.modelIcons[i], sizeof(s_playermodel.modelIcons[i]), "models/players/%s/icon_%s", s_playermodel.modelname, s_playermodel.modelList[j]);
// END
			s_playermodel.picbuttons[i].generic.flags &= ~QMF_INACTIVE;
		}
		else
		{
			// dead slot
// STONELANCE
// 			s_playermodel.pics[i].generic.name         = NULL;
 			Q_strncpyz( s_playermodel.modelIcons[i], "", sizeof(s_playermodel.modelIcons[i]));
// END

			s_playermodel.picbuttons[i].generic.flags |= QMF_INACTIVE;
		}

// STONELANCE
		s_playermodel.ports[i].generic.flags       &= ~QMF_HIGHLIGHT;
// END
 		s_playermodel.pics[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.pics[i].shader               = 0;
 		s_playermodel.picbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	if (s_playermodel.selectedmodel/MAX_MODELSPERPAGE == s_playermodel.modelpage)
	{
		// set selected model
		i = s_playermodel.selectedmodel % MAX_MODELSPERPAGE;

// STONELANCE
//		s_playermodel.pics[i].generic.flags       |= QMF_HIGHLIGHT;
		s_playermodel.ports[i].generic.flags       |= QMF_HIGHLIGHT;
// END
		s_playermodel.picbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;
	}

	if (s_playermodel.numpages > 1)
	{
		if (s_playermodel.modelpage > 0)
// STONELANCE
//			s_playermodel.left.generic.flags &= ~QMF_INACTIVE;
			s_playermodel.up.generic.flags &= ~QMF_INACTIVE;
// END
		else
// STONELANCE
//			s_playermodel.left.generic.flags |= QMF_INACTIVE;
			s_playermodel.up.generic.flags |= QMF_INACTIVE;
// END

		if (s_playermodel.modelpage < s_playermodel.numpages-1)
// STONELANCE
//			s_playermodel.right.generic.flags &= ~QMF_INACTIVE;
			s_playermodel.down.generic.flags &= ~QMF_INACTIVE;
// END
		else
// STONELANCE
//			s_playermodel.right.generic.flags |= QMF_INACTIVE;
			s_playermodel.down.generic.flags |= QMF_INACTIVE;
// END
	}
	else
	{
		// hide left/right markers
// STONELANCE
//		s_playermodel.left.generic.flags |= QMF_INACTIVE;
//		s_playermodel.right.generic.flags |= QMF_INACTIVE;
		s_playermodel.up.generic.flags |= QMF_INACTIVE;
		s_playermodel.down.generic.flags |= QMF_INACTIVE;
// END
	}
	if (s_playermodel.modelpage > 0)
		s_playermodel.paintPrev.generic.flags &= ~QMF_INACTIVE;
	else
		s_playermodel.paintPrev.generic.flags |= QMF_INACTIVE;
	if (s_playermodel.modelpage < s_playermodel.numpages - 1)
		s_playermodel.paintNext.generic.flags &= ~QMF_INACTIVE;
	else
		s_playermodel.paintNext.generic.flags |= QMF_INACTIVE;
}

/*
=================
PlayerModel_UpdateModel
=================
*/
static void PlayerModel_UpdateModel( void )
{
	vec3_t	viewangles;
	vec3_t	moveangles;
// STONELANCE
	char	plate[MAX_QPATH];

/*
	memset( &s_playermodel.playerinfo, 0, sizeof(playerInfo_t) );

	viewangles[YAW]   = 180 - 30;
	viewangles[PITCH] = 0;
	viewangles[ROLL]  = 0;
*/
	VectorClear( viewangles );
// END
	VectorClear( moveangles );

// STONELANCE
//	UI_PlayerInfo_SetModel( &s_playermodel.playerinfo, s_playermodel.modelskin );
	trap_Cvar_VariableStringBuffer( "plate", plate, sizeof( plate ) );
	UI_PlayerInfo_SetModel( &s_playermodel.playerinfo, s_playermodel.modelskin, s_playermodel.rimskin, s_playermodel.headskin, plate );
// END
	UI_PlayerInfo_SetInfo( &s_playermodel.playerinfo, LEGS_IDLE, TORSO_STAND, viewangles, moveangles, WP_MACHINEGUN, qfalse );
}

/*
=================
PlayerModel_SaveChanges
=================
*/
static void PlayerModel_SaveChanges( void )
{
	trap_Cvar_Set( "model", s_playermodel.modelskin );
	trap_Cvar_Set( "headmodel", s_playermodel.modelskin );
	trap_Cvar_Set( "team_model", s_playermodel.modelskin );
	trap_Cvar_Set( "team_headmodel", s_playermodel.modelskin );
// STONELANCE
	trap_Cvar_Set( "rim", s_playermodel.rimskin );
	trap_Cvar_Set( "head", s_playermodel.headskin );

	PlayerSettings_Update();
// END
}

/*
=================
PlayerModel_MenuEvent
=================
*/
static void PlayerModel_MenuEvent( void* ptr, int event )
{
	if (event != QM_ACTIVATED)
		return;

	switch (((menucommon_s*)ptr)->id)
	{
		case ID_PREVPAGE:
			if (s_playermodel.modelpage > 0)
			{
				s_playermodel.modelpage--;
				PlayerModel_UpdateGrid();
			}
			break;

		case ID_NEXTPAGE:
			if (s_playermodel.modelpage < s_playermodel.numpages-1)
			{
				s_playermodel.modelpage++;
				PlayerModel_UpdateGrid();
			}
			break;
// STONELANCE
/*
		case ID_BACK:
			PlayerModel_SaveChanges();
			UI_PopMenu();
			break;
*/

		case ID_PREVRIMPAGE:
			if (s_playermodel.rimPage > 0)
			{
				s_playermodel.rimPage--;
				PlayerModel_UpdateRimGrid();
			}
			break;

		case ID_NEXTRIMPAGE:
			if (s_playermodel.rimPage < s_playermodel.numRimPages-1)
			{
				s_playermodel.rimPage++;
				PlayerModel_UpdateRimGrid();
			}
			break;

		case ID_PREVHEADPAGE:
			if (s_playermodel.headPage > 0)
			{
				s_playermodel.headPage--;
				PlayerModel_UpdateHeadGrid();
			}
			break;

		case ID_NEXTHEADPAGE:
			if (s_playermodel.headPage < s_playermodel.numHeadPages-1)
			{
				s_playermodel.headPage++;
				PlayerModel_UpdateHeadGrid();
			}
			break;

		case ID_BACK:
			s_playermodel.menu.transitionMenu = ((menucommon_s*)ptr)->id;
			uis.transitionOut = uis.realtime;
			break;
// END
	}
}


// STONELANCE
/*
=================
PlayerModel_ChangeMenu
=================
*/
void PlayerModel_ChangeMenu(int menuID){

	switch(menuID){
		case ID_BACK:
			PlayerModel_SaveChanges();
			uis.spinView = 0;
//			uis.transitionIn = uis.realtime;
			UI_PopMenu();
			break;
	}
}


/*
=================
PlayerModel_RunTransition
=================
*/
void PlayerModel_RunTransition(float frac){
	uis.text_color[0] = text_color_normal[0];
	uis.text_color[1] = text_color_normal[1];
	uis.text_color[2] = text_color_normal[2];
	uis.text_color[3] = text_color_normal[3] * frac;
	s_playermodel.banner.color = uis.text_color;
	s_playermodel.favorites.color = uis.text_color;
	s_playermodel.back.color = uis.text_color;
}
// END


/*
=================
PlayerModel_MenuKey
=================
*/
static sfxHandle_t PlayerModel_MenuKey( int key )
{
	menucommon_s*	m;
	int				picnum;

	switch (key)
	{
		case K_KP_LEFTARROW:
		case K_LEFTARROW:
			m = Menu_ItemAtCursor(&s_playermodel.menu);
			picnum = m ? m->id - ID_PLAYERPIC0 : -1;
			if (picnum >= 0 && picnum < MAX_MODELSPERPAGE)
			{
				if (picnum > 0)
				{
					Menu_SetCursorToItem(&s_playermodel.menu,
						&s_playermodel.picbuttons[picnum - 1]);
					return (menu_move_sound);
					
				}
				else if (s_playermodel.modelpage > 0)
				{
					s_playermodel.modelpage--;
					PlayerModel_UpdateGrid();
					Menu_SetCursorToItem(&s_playermodel.menu,
						&s_playermodel.picbuttons[MAX_MODELSPERPAGE - 1]);
					return (menu_move_sound);
				}
				else
					return (menu_buzz_sound);
			}
			break;

		case K_KP_RIGHTARROW:
		case K_RIGHTARROW:
			m = Menu_ItemAtCursor(&s_playermodel.menu);
			picnum = m ? m->id - ID_PLAYERPIC0 : -1;
			if (picnum >= 0 && picnum < MAX_MODELSPERPAGE)
			{
				if ((picnum < MAX_MODELSPERPAGE - 1) &&
					(s_playermodel.modelpage * MAX_MODELSPERPAGE + picnum + 1 <
					s_playermodel.nummodels))
				{
					Menu_SetCursorToItem(&s_playermodel.menu,
						&s_playermodel.picbuttons[picnum + 1]);
					return (menu_move_sound);
				}					
				else if ((picnum == MAX_MODELSPERPAGE - 1) &&
					(s_playermodel.modelpage < s_playermodel.numpages - 1))
				{
					s_playermodel.modelpage++;
					PlayerModel_UpdateGrid();
					Menu_SetCursorToItem(&s_playermodel.menu,
						&s_playermodel.picbuttons[0]);
					return (menu_move_sound);
				}
				else
					return (menu_buzz_sound);
			}
			break;
			
		case K_MOUSE2:
		case K_ESCAPE:
// STONELANCE
//			PlayerModel_SaveChanges();
//			break;
			s_playermodel.menu.transitionMenu = ID_BACK;
			uis.transitionOut = uis.realtime;
			return 0;
// END
	}

	return ( Menu_DefaultKey( &s_playermodel.menu, key ) );
}


// STONELANCE (new function)
/*
=================
SaveFavorite

=================
*/
static int PlayerModel_FavoriteIndex( const char *favorite ) {
	if ( !favorite || Q_strncmp( favorite, "favoritecar", 11 ) ) {
		return -1;
	}

	return atoi( favorite + 11 ) - 1;
}

static void SaveFavorite( const char *favorite ) {
	int favoriteIndex;
	const char *slash;
	char modelName[MAX_QPATH];
	char skinName[MAX_QPATH];

	favoriteIndex = PlayerModel_FavoriteIndex( favorite );
	if ( favoriteIndex < 0 || favoriteIndex >= NUM_FAVORITES ) {
		return;
	}

	slash = strchr( s_playermodel.modelskin, '/' );
	if ( slash ) {
		int modelLength = slash - s_playermodel.modelskin;
		if ( modelLength >= MAX_QPATH ) {
			modelLength = MAX_QPATH - 1;
		}
		Com_Memcpy( modelName, s_playermodel.modelskin, modelLength );
		modelName[modelLength] = '\0';
		Q_strncpyz( skinName, slash + 1, sizeof( skinName ) );
	} else {
		Q_strncpyz( modelName, s_playermodel.modelskin, sizeof( modelName ) );
		skinName[0] = '\0';
	}

	if ( UI_Profile_HasActiveProfile() ) {
		profile_info_t info;
		const profile_info_t *activeInfo = UI_Profile_GetActiveInfo();

		if ( activeInfo ) {
			info = *activeInfo;
		} else {
			Com_Memset( &info, 0, sizeof( info ) );
		}

		Q_strncpyz( info.favoriteCars[favoriteIndex].model, modelName, sizeof( info.favoriteCars[favoriteIndex].model ) );
		Q_strncpyz( info.favoriteCars[favoriteIndex].skin, skinName, sizeof( info.favoriteCars[favoriteIndex].skin ) );
		Q_strncpyz( info.favoriteCars[favoriteIndex].rim, s_playermodel.rimskin, sizeof( info.favoriteCars[favoriteIndex].rim ) );
		Q_strncpyz( info.favoriteCars[favoriteIndex].head, s_playermodel.headskin, sizeof( info.favoriteCars[favoriteIndex].head ) );

		if ( UI_Profile_SaveActiveInfo( &info ) ) {
			s_playermodel.profileInfo = info;
		}
	} else {
		char buf[MAX_QPATH];

		Com_sprintf( buf, sizeof( buf ), "%s/%s/%s", s_playermodel.modelskin, s_playermodel.rimskin, s_playermodel.headskin );
		trap_Cvar_Set( favorite, buf );
	}

	PlayerModel_UpdateFavorites();
}


/*
=================
PlayerModel_RendererEvent
=================
*/
static void PlayerModel_RendererEvent( void* ptr, int event )
{
	if (event == QM_ACTIVATED)
		uis.spinView = qtrue;
}
// END


/*
=================
PlayerModel_PicEvent
=================
*/
static void PlayerModel_PicEvent( void* ptr, int event )
{
	int				modelnum;
	int				i;

	if (event != QM_ACTIVATED)
		return;

// STONELANCE
	switch(((menucommon_s*)ptr)->id){
	case ID_FAVORITE1:
		SaveFavorite("favoritecar1");
		return;

	case ID_FAVORITE2:
		SaveFavorite("favoritecar2");
		return;

	case ID_FAVORITE3:
		SaveFavorite("favoritecar3");
		return;

	case ID_FAVORITE4:
		SaveFavorite("favoritecar4");
		return;
	}
// END

	for (i=0; i<PLAYERGRID_ROWS*PLAYERGRID_COLS; i++)
	{
		// reset
// STONELANCE
		s_playermodel.ports[i].generic.flags       &= ~QMF_HIGHLIGHT;
// END
 		s_playermodel.pics[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.picbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	// set selected
	i = ((menucommon_s*)ptr)->id - ID_PLAYERPIC0;
// STONELANCE
//	s_playermodel.pics[i].generic.flags       |= QMF_HIGHLIGHT;
	s_playermodel.ports[i].generic.flags       |= QMF_HIGHLIGHT;
// END
	s_playermodel.picbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;

	// get model and strip icon_
	modelnum = s_playermodel.modelpage * MAX_MODELSPERPAGE + i;
// STONELANCE
/*
	buffptr  = s_playermodel.modelnames[modelnum] + strlen("models/players/");
	pdest    = strstr(buffptr,"icon_");
	if (pdest)
	{
		// track the whole model/skin name
		Q_strncpyz(s_playermodel.modelskin,buffptr,pdest-buffptr+1);
		strcat(s_playermodel.modelskin,pdest + 5);

		// separate the model name
		maxlen = pdest-buffptr;
		if (maxlen > 16)
			maxlen = 16;
		Q_strncpyz( s_playermodel.modelname.string, buffptr, maxlen );
		Q_strupr( s_playermodel.modelname.string );

		// separate the skin name
		maxlen = strlen(pdest+5)+1;
		if (maxlen > 16)
			maxlen = 16;

		Q_strncpyz( s_playermodel.skinname.string, pdest+5, maxlen );
		Q_strupr( s_playermodel.skinname.string );

		s_playermodel.selectedmodel = modelnum;

		if( trap_MemoryRemaining() > LOW_MEMORY ) {
			PlayerModel_UpdateModel();
		}
	}
*/
	Com_sprintf(s_playermodel.modelskin, sizeof(s_playermodel.modelskin), "%s/%s", s_playermodel.modelname, s_playermodel.modelList[modelnum]);
	s_playermodel.paintname.string = s_playermodel.modelList[modelnum];
	s_playermodel.selectedmodel = modelnum;

	if( trap_MemoryRemaining() > LOW_MEMORY ) {
		PlayerModel_UpdateModel();
	}
// END
}


// STONELANCE (new function)
/*
=================
PlayerModel_HeadPicEvent
=================
*/
static void PlayerModel_HeadPicEvent( void* ptr, int event )
{
	int				modelnum;
	int				i;

	if (event != QM_ACTIVATED)
		return;

	for (i=0; i<HEADGRID_ROWS*HEADGRID_COLS; i++)
	{
		// reset
		s_playermodel.headports[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.headpics[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.headpicbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	// set selected
	i = ((menucommon_s*)ptr)->id - ID_HEADPIC0;
	s_playermodel.headports[i].generic.flags       |= QMF_HIGHLIGHT;
	s_playermodel.headpicbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;

	// get model
	modelnum = s_playermodel.headPage*MAX_HEADSPERPAGE + i;

	Q_strncpyz( s_playermodel.headskin, s_playermodel.headList[modelnum], sizeof(s_playermodel.headskin) );
	s_playermodel.selectedHead = modelnum;

	if( trap_MemoryRemaining() > LOW_MEMORY ) {
		PlayerModel_UpdateModel();
	}
}
// END


// STONELANCE
/*
=================
PlayerModel_RimPicEvent
=================
*/
static void PlayerModel_RimPicEvent( void* ptr, int event )
{
	int				modelnum;
	int				i;

	if (event != QM_ACTIVATED)
		return;

	for (i=0; i<RIMGRID_ROWS*RIMGRID_COLS; i++)
	{
		// reset
		s_playermodel.rimports[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.rimpics[i].generic.flags       &= ~QMF_HIGHLIGHT;
 		s_playermodel.rimpicbuttons[i].generic.flags |= QMF_PULSEIFFOCUS;
	}

	// set selected
	i = ((menucommon_s*)ptr)->id - ID_RIMPIC0;
	s_playermodel.rimports[i].generic.flags       |= QMF_HIGHLIGHT;
	s_playermodel.rimpicbuttons[i].generic.flags &= ~QMF_PULSEIFFOCUS;

	// get model and strip icon_
	modelnum = s_playermodel.rimPage*MAX_RIMSPERPAGE + i;

	Q_strncpyz( s_playermodel.rimskin, s_playermodel.rimList[modelnum], sizeof(s_playermodel.rimskin) );
	s_playermodel.selectedRim = modelnum;

	if( trap_MemoryRemaining() > LOW_MEMORY ) {
		PlayerModel_UpdateModel();
	}
}
// END


/*
=================
PlayerModel_DrawPlayer
=================
*/
static void PlayerModel_DrawPlayer( void *self )
{
	menubitmap_s*	b;

	b = (menubitmap_s*) self;

	if( trap_MemoryRemaining() <= LOW_MEMORY ) {
		UI_DrawProportionalString( b->generic.x, b->generic.y + b->height / 2, "LOW MEMORY", UI_LEFT, color_red );
		return;
	}

// STONELANCE
	if (uis.spinView == qtrue && !trap_Key_IsDown( K_MOUSE1 )){
		uis.spinView = qfalse;
	}
// END

	UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, &s_playermodel.playerinfo, uis.realtime );
}


// STONELANCE
/*
=================
PlayerModel_BuildHeadList
=================
*/
static void PlayerModel_BuildHeadList( void )
{
	s_playermodel.selectedHead = 0;
	s_playermodel.headPage = 0;

	s_playermodel.numHeads = UI_BuildFileList("models/players/heads", "skin", "", qtrue, qfalse, qtrue, 0, s_playermodel.headList);

	s_playermodel.numHeadPages = s_playermodel.numHeads / MAX_HEADSPERPAGE;
	if (s_playermodel.numHeads % MAX_HEADSPERPAGE)
		s_playermodel.numHeadPages++;
}


/*
=================
PlayerModel_BuildRimList
=================
*/
static void PlayerModel_BuildRimList( void )
{
	s_playermodel.selectedRim = 0;
	s_playermodel.rimPage = 0;

	s_playermodel.numRims = UI_BuildFileList("models/players/wheels", "skin", "", qtrue, qfalse, qtrue, 0, s_playermodel.rimList);

	s_playermodel.numRimPages = s_playermodel.numRims / MAX_RIMSPERPAGE;
	if (s_playermodel.numRims % MAX_RIMSPERPAGE)
		s_playermodel.numRimPages++;
}
// END

/*
=================
PlayerModel_BuildList
=================
*/
static void PlayerModel_BuildList( void )
{
/*
	int		numdirs;
	int		numfiles;
	char	dirlist[2048];
	char	filelist[2048];
	char	skinname[64];
	char*	dirptr;
	char*	fileptr;
	int		i;
	int		j;
	int		dirlen;
	int		filelen;
	qboolean precache;

	precache = trap_Cvar_VariableValue("com_buildscript");

	s_playermodel.modelpage = 0;
	s_playermodel.nummodels = 0;

	// iterate directory of all player models
	numdirs = trap_FS_GetFileList("models/players", "/", dirlist, 2048 );
	dirptr  = dirlist;
	for (i=0; i<numdirs && s_playermodel.nummodels < MAX_PLAYERMODELS; i++,dirptr+=dirlen+1)
	{
		dirlen = strlen(dirptr);
		
		if (dirlen && dirptr[dirlen-1]=='/') dirptr[dirlen-1]='\0';

		if (!strcmp(dirptr,".") || !strcmp(dirptr,".."))
			continue;
			
		// iterate all skin files in directory
		numfiles = trap_FS_GetFileList( va("models/players/%s",dirptr), "tga", filelist, 2048 );
		fileptr  = filelist;
		for (j=0; j<numfiles && s_playermodel.nummodels < MAX_PLAYERMODELS;j++,fileptr+=filelen+1)
		{
			filelen = strlen(fileptr);

			COM_StripExtension(fileptr,skinname);

			// look for icon_????
			if (!Q_stricmpn(skinname,"icon_",5))
			{
				Com_sprintf( s_playermodel.modelnames[s_playermodel.nummodels++],
					sizeof( s_playermodel.modelnames[s_playermodel.nummodels] ),
					"models/players/%s/%s", dirptr, skinname );
				//if (s_playermodel.nummodels >= MAX_PLAYERMODELS)
				//	return;
			}

			if( precache ) {
				trap_S_RegisterSound( va( "sound/player/announce/%s_wins.wav", skinname), qfalse );
			}
		}
	}	

	//APSFIXME - Degenerate no models case
*/
	s_playermodel.selectedmodel	= 0;
	s_playermodel.modelpage = 0;

	s_playermodel.nummodels = UI_BuildFileList( va("models/players/%s", s_playermodel.modelname), "skin", "", qtrue, qfalse, qtrue, 0, s_playermodel.modelList);

	s_playermodel.numpages = s_playermodel.nummodels / MAX_MODELSPERPAGE;
	if (s_playermodel.nummodels % MAX_MODELSPERPAGE)
		s_playermodel.numpages++;
}

/*
=================
PlayerModel_SetMenuItems
=================
*/
static void PlayerModel_SetMenuItems( void )
{
	int				i;
// STONELANCE
//	int				maxlen;
// END
	char			modelskin[64];
// STONELANCE
//	char*			buffptr;
// END
	char*			pdest;

// STONELANCE
/*
	// name
	trap_Cvar_VariableStringBuffer( "name", s_playermodel.playername.string, 16 );
	Q_CleanStr( s_playermodel.playername.string );

	// model
//	trap_Cvar_VariableStringBuffer( "model", s_playermodel.modelskin, 64 );
*/
	trap_Cvar_VariableStringBuffer( "model", s_playermodel.modelskin, sizeof(s_playermodel.modelskin) );

	pdest = strchr( s_playermodel.modelskin, '/' );
	if ( pdest )
		Q_strncpyz( modelskin, pdest+1, sizeof( modelskin ) );
	else
		Q_strncpyz( modelskin, DEFAULT_SKIN, sizeof( modelskin ) );
// END
	
	// use default skin if none is set
	if (!strchr(s_playermodel.modelskin, '/')) {
		Q_strcat(s_playermodel.modelskin, 64, "/default");
	}
	
	// find model in our list
	for (i=0; i<s_playermodel.nummodels; i++)
	{
// STONELANCE
/*
		// strip icon_
		buffptr  = s_playermodel.modelnames[i] + strlen("models/players/");
		pdest    = strstr(buffptr,"icon_");
		if (pdest)
		{
			Q_strncpyz(modelskin,buffptr,pdest-buffptr+1);
			strcat(modelskin,pdest + 5);
		}
		else
			continue;

		if (!Q_stricmp( s_playermodel.modelskin, modelskin ))
		{
			// found pic, set selection here		
			s_playermodel.selectedmodel = i;
			s_playermodel.modelpage     = i/MAX_MODELSPERPAGE;

			// separate the model name
			maxlen = pdest-buffptr;
			if (maxlen > 16)
				maxlen = 16;
			Q_strncpyz( s_playermodel.modelname.string, buffptr, maxlen );
			Q_strupr( s_playermodel.modelname.string );

			// separate the skin name
			maxlen = strlen(pdest+5)+1;
			if (maxlen > 16)
				maxlen = 16;
			Q_strncpyz( s_playermodel.skinname.string, pdest+5, maxlen );
			Q_strupr( s_playermodel.skinname.string );
			break;
		}
*/
		if (!Q_stricmp( s_playermodel.modelList[i], modelskin )){
			s_playermodel.paintname.string = s_playermodel.modelList[i];
			s_playermodel.selectedmodel	= i;
			s_playermodel.modelpage		= i / MAX_MODELSPERPAGE;
		}
// END
	}

// STONELANCE
	// rim
	trap_Cvar_VariableStringBuffer( "rim", s_playermodel.rimskin, sizeof(s_playermodel.rimskin) );
	// find rim in our list
	for (i = 0; i < s_playermodel.numRims; i++){
		if (!Q_stricmp( s_playermodel.rimskin, s_playermodel.rimList[i] )){
			// found pic, set selection here		
			s_playermodel.selectedRim = i;
			s_playermodel.rimPage     = i / MAX_RIMSPERPAGE;
			break;
		}
	}

	// head
	trap_Cvar_VariableStringBuffer( "head", s_playermodel.headskin, sizeof(s_playermodel.headskin) );
	// find head in our list
	for (i = 0; i < s_playermodel.numHeads; i++){
		if (!Q_stricmp( s_playermodel.headskin, s_playermodel.headList[i] )){
			// found pic, set selection here		
			s_playermodel.selectedHead = i;
			s_playermodel.headPage     = i / MAX_HEADSPERPAGE;
			break;
		}
	}
// END
}

/*
=================
PlayerModel_MenuInit
=================
*/
static void PlayerModel_MenuInit( void )
{
	int			i;
// STONELANCE
//	int			j;
//	int			k;
// END
	int			x;
	int			y;
//	static char	playername[32];
//	static char	modelname[32];
	static char	skinname[32];

	// zero set all our globals
// STONELANCE
//	memset( &s_playermodel, 0 ,sizeof(playermodel_t) );
// END

	PlayerModel_Cache();
	
	if ( UI_Profile_HasActiveProfile() ) {
		const profile_info_t *info = UI_Profile_GetActiveInfo();
		if ( info ) {
			s_playermodel.profileInfo = *info;
		} else {
			Com_Memset( &s_playermodel.profileInfo, 0, sizeof( s_playermodel.profileInfo ) );
		}
	} else {
		Com_Memset( &s_playermodel.profileInfo, 0, sizeof( s_playermodel.profileInfo ) );
	}


	s_playermodel.menu.key        = PlayerModel_MenuKey;
	s_playermodel.menu.wrapAround = qtrue;
	s_playermodel.menu.fullscreen = qtrue;
// STONELANCE
	s_playermodel.menu.draw		  = PlayerModel_DrawBackShaders;
	s_playermodel.menu.transition = PlayerModel_RunTransition;
	s_playermodel.menu.changeMenu = PlayerModel_ChangeMenu;
// END

	s_playermodel.banner.generic.type  = MTYPE_BTEXT;
	s_playermodel.banner.generic.x     = 320;
// STONELANCE
/*
	s_playermodel.banner.generic.y     = 16;
	s_playermodel.banner.string        = "PLAYER MODEL";
	s_playermodel.banner.color         = color_white;
*/
	s_playermodel.banner.generic.y     = 17;
	s_playermodel.banner.string        = "CUSTOMIZE YOUR CAR";
	s_playermodel.banner.color         = text_color_normal;
// END
	s_playermodel.banner.style         = UI_CENTER;

// STONELANCE
/*
	s_playermodel.framel.generic.type  = MTYPE_BITMAP;
	s_playermodel.framel.generic.name  = MODEL_FRAMEL;
	s_playermodel.framel.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_playermodel.framel.generic.x     = 0;
	s_playermodel.framel.generic.y     = 78;
	s_playermodel.framel.width         = 256;
	s_playermodel.framel.height        = 329;

	s_playermodel.framer.generic.type  = MTYPE_BITMAP;
	s_playermodel.framer.generic.name  = MODEL_FRAMER;
	s_playermodel.framer.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_playermodel.framer.generic.x     = 376;
	s_playermodel.framer.generic.y     = 76;
	s_playermodel.framer.width         = 256;
	s_playermodel.framer.height        = 334;

	s_playermodel.ports.generic.type  = MTYPE_BITMAP;
	s_playermodel.ports.generic.name  = MODEL_PORTS;
	s_playermodel.ports.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_playermodel.ports.generic.x     = 50;
	s_playermodel.ports.generic.y     = 59;
	s_playermodel.ports.width         = 274;
	s_playermodel.ports.height        = 274;

	y =	59;
	for (i=0,k=0; i<PLAYERGRID_ROWS; i++)
	{
		x =	50;
		for (j=0; j<PLAYERGRID_COLS; j++,k++)
		{
			s_playermodel.pics[k].generic.type	   = MTYPE_BITMAP;
			s_playermodel.pics[k].generic.flags    = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
			s_playermodel.pics[k].generic.x		   = x;
			s_playermodel.pics[k].generic.y		   = y;
			s_playermodel.pics[k].width  		   = 64;
			s_playermodel.pics[k].height  		   = 64;
			s_playermodel.pics[k].focuspic         = MODEL_SELECTED;
			s_playermodel.pics[k].focuscolor       = colorRed;

			s_playermodel.picbuttons[k].generic.type	 = MTYPE_BITMAP;
			s_playermodel.picbuttons[k].generic.flags    = QMF_LEFT_JUSTIFY|QMF_NODEFAULTINIT|QMF_PULSEIFFOCUS;
			s_playermodel.picbuttons[k].generic.id	     = ID_PLAYERPIC0+k;
			s_playermodel.picbuttons[k].generic.callback = PlayerModel_PicEvent;
			s_playermodel.picbuttons[k].generic.x    	 = x - 16;
			s_playermodel.picbuttons[k].generic.y		 = y - 16;
			s_playermodel.picbuttons[k].generic.left	 = x;
			s_playermodel.picbuttons[k].generic.top		 = y;
			s_playermodel.picbuttons[k].generic.right	 = x + 64;
			s_playermodel.picbuttons[k].generic.bottom   = y + 64;
			s_playermodel.picbuttons[k].width  		     = 128;
			s_playermodel.picbuttons[k].height  		 = 128;
			s_playermodel.picbuttons[k].focuspic  		 = MODEL_SELECT;
			s_playermodel.picbuttons[k].focuscolor  	 = colorRed;

			x += 64+6;
		}
		y += 64+6;
	}

	s_playermodel.playername.generic.type  = MTYPE_PTEXT;
	s_playermodel.playername.generic.flags = QMF_CENTER_JUSTIFY|QMF_INACTIVE;
	s_playermodel.playername.generic.x	   = 320;
	s_playermodel.playername.generic.y	   = 440;
	s_playermodel.playername.string	       = playername;
	s_playermodel.playername.style		   = UI_CENTER;
	s_playermodel.playername.color         = text_color_normal;

	s_playermodel.modelname.generic.type  = MTYPE_PTEXT;
	s_playermodel.modelname.generic.flags = QMF_CENTER_JUSTIFY|QMF_INACTIVE;
	s_playermodel.modelname.generic.x	  = 497;
	s_playermodel.modelname.generic.y	  = 54;
	s_playermodel.modelname.string	      = modelname;
	s_playermodel.modelname.style		  = UI_CENTER;
	s_playermodel.modelname.color         = text_color_normal;

	s_playermodel.skinname.generic.type   = MTYPE_PTEXT;
	s_playermodel.skinname.generic.flags  = QMF_CENTER_JUSTIFY|QMF_INACTIVE;
	s_playermodel.skinname.generic.x	  = 497;
	s_playermodel.skinname.generic.y	  = 394;
	s_playermodel.skinname.string	      = skinname;
	s_playermodel.skinname.style		  = UI_CENTER;
	s_playermodel.skinname.color          = text_color_normal;
*/

	// ************************ PAINTS *****************************

	x = 24;
	y = 64;
	for (i=0; i<MAX_MODELSPERPAGE; i++)
	{
		s_playermodel.ports[i].generic.type		= MTYPE_BITMAP;
		s_playermodel.ports[i].generic.name		= MODEL_PORT;
		s_playermodel.ports[i].generic.flags	= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
		s_playermodel.ports[i].generic.x		= x;
		s_playermodel.ports[i].generic.y		= y;
		s_playermodel.ports[i].width			= 64;
		s_playermodel.ports[i].height			= 64;
		s_playermodel.ports[i].focuspic			= MODEL_SELECTED;
		s_playermodel.ports[i].focuscolor       = text_color_highlight;

		s_playermodel.pics[i].generic.type		= MTYPE_BITMAP;
		s_playermodel.pics[i].generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
		s_playermodel.pics[i].generic.x			= x;
		s_playermodel.pics[i].generic.y			= y;
		s_playermodel.pics[i].width  			= 64;
		s_playermodel.pics[i].height  			= 64;
		s_playermodel.pics[i].generic.name		= s_playermodel.modelIcons[i];

		s_playermodel.picbuttons[i].generic.type	 = MTYPE_BITMAP;
		s_playermodel.picbuttons[i].generic.flags    = QMF_LEFT_JUSTIFY|QMF_NODEFAULTINIT|QMF_PULSEIFFOCUS;
		s_playermodel.picbuttons[i].generic.id	     = ID_PLAYERPIC0+i;
		s_playermodel.picbuttons[i].generic.callback = PlayerModel_PicEvent;
		s_playermodel.picbuttons[i].generic.x    	 = x;
		s_playermodel.picbuttons[i].generic.left	 = x;
		s_playermodel.picbuttons[i].generic.right	 = x + 64;
		s_playermodel.picbuttons[i].generic.y		 = y;
		s_playermodel.picbuttons[i].generic.top		 = y;
		s_playermodel.picbuttons[i].generic.bottom   = y + 64;
		s_playermodel.picbuttons[i].width  		     = 64;
		s_playermodel.picbuttons[i].height  		 = 64;
		s_playermodel.picbuttons[i].focuspic  		 = MODEL_SELECT;
		s_playermodel.picbuttons[i].focuscolor  	 = text_color_highlight;

		x += 64 + 6;
	}

	y = 138;
	s_playermodel.up.generic.type			= MTYPE_BITMAP;
	s_playermodel.up.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playermodel.up.generic.name			= MODEL_ARROWSL0;
	s_playermodel.up.generic.callback		= PlayerModel_MenuEvent;
	s_playermodel.up.generic.id				= ID_PREVPAGE;
	s_playermodel.up.generic.x				= 40 - 16;
	s_playermodel.up.generic.y				= y;
	s_playermodel.up.width  				= 32;
	s_playermodel.up.height  				= 32;
	s_playermodel.up.focuspic				= MODEL_ARROWSL;

	s_playermodel.down.generic.type			= MTYPE_BITMAP;
	s_playermodel.down.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playermodel.down.generic.name			= MODEL_ARROWSR0;
	s_playermodel.down.generic.callback		= PlayerModel_MenuEvent;
	s_playermodel.down.generic.id			= ID_NEXTPAGE;
	s_playermodel.down.generic.x			= 280 - 16;
	s_playermodel.down.generic.y			= y;
	s_playermodel.down.width  				= 32;
	s_playermodel.down.height  				= 32;
	s_playermodel.down.focuspic				= MODEL_ARROWSR;

	s_playermodel.paintname.generic.type  = MTYPE_PTEXT;
	s_playermodel.paintname.generic.flags = QMF_CENTER_JUSTIFY|QMF_INACTIVE;
	s_playermodel.paintname.generic.x	  = 160;
	s_playermodel.paintname.generic.y	  = y + 8;
	s_playermodel.paintname.string	      = skinname;
	s_playermodel.paintname.style		  = UI_CENTER | UI_SMALLFONT;
	s_playermodel.paintname.color         = text_color_normal;

	// ************************ HEADS *****************************

	x = 482;
	y = 403;
	for (i=0; i<MAX_HEADSPERPAGE; i++)
	{
		s_playermodel.headports[i].generic.type		= MTYPE_BITMAP;
		s_playermodel.headports[i].generic.name		= MODEL_PORT;
		s_playermodel.headports[i].generic.flags	= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
		s_playermodel.headports[i].generic.x		= x;
		s_playermodel.headports[i].generic.y		= y;
		s_playermodel.headports[i].width			= 64;
		s_playermodel.headports[i].height			= 64;
		s_playermodel.headports[i].focuspic			= MODEL_SELECTED;
		s_playermodel.headports[i].focuscolor       = text_color_highlight;

		s_playermodel.headpics[i].generic.type		= MTYPE_BITMAP;
		s_playermodel.headpics[i].generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
		s_playermodel.headpics[i].generic.x			= x;
		s_playermodel.headpics[i].generic.y			= y;
		s_playermodel.headpics[i].width  			= 64;
		s_playermodel.headpics[i].height  			= 64;
		s_playermodel.headpics[i].generic.name		= s_playermodel.headIcons[i];

		s_playermodel.headpicbuttons[i].generic.type	 = MTYPE_BITMAP;
		s_playermodel.headpicbuttons[i].generic.flags    = QMF_LEFT_JUSTIFY|QMF_NODEFAULTINIT|QMF_PULSEIFFOCUS;
		s_playermodel.headpicbuttons[i].generic.id	     = ID_HEADPIC0+i;
		s_playermodel.headpicbuttons[i].generic.callback = PlayerModel_HeadPicEvent;
		s_playermodel.headpicbuttons[i].generic.x    	 = x;
		s_playermodel.headpicbuttons[i].generic.left	 = x;
		s_playermodel.headpicbuttons[i].generic.right	 = x + 64;
		s_playermodel.headpicbuttons[i].generic.y		 = y;
		s_playermodel.headpicbuttons[i].generic.top		 = y;
		s_playermodel.headpicbuttons[i].generic.bottom   = y + 64;
		s_playermodel.headpicbuttons[i].width  		     = 64;
		s_playermodel.headpicbuttons[i].height  		 = 64;
		s_playermodel.headpicbuttons[i].focuspic  		 = MODEL_SELECT;
		s_playermodel.headpicbuttons[i].focuscolor  	 = text_color_highlight;

		x += 64 + 6;
	}

	y = 362;
	s_playermodel.headup.generic.type			= MTYPE_BITMAP;
	s_playermodel.headup.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playermodel.headup.generic.name			= MODEL_ARROWSL0;
	s_playermodel.headup.generic.callback		= PlayerModel_MenuEvent;
	s_playermodel.headup.generic.id				= ID_PREVHEADPAGE;
	s_playermodel.headup.generic.x				= 516 - 16;
	s_playermodel.headup.generic.y				= y;
	s_playermodel.headup.width  				= 32;
	s_playermodel.headup.height  				= 32;
	s_playermodel.headup.focuspic				= MODEL_ARROWSL;

	s_playermodel.headdown.generic.type			= MTYPE_BITMAP;
	s_playermodel.headdown.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playermodel.headdown.generic.name			= MODEL_ARROWSR0;
	s_playermodel.headdown.generic.callback		= PlayerModel_MenuEvent;
	s_playermodel.headdown.generic.id			= ID_NEXTHEADPAGE;
	s_playermodel.headdown.generic.x			= 584 - 16;
	s_playermodel.headdown.generic.y			= y;
	s_playermodel.headdown.width  				= 32;
	s_playermodel.headdown.height  				= 32;
	s_playermodel.headdown.focuspic				= MODEL_ARROWSR;

	// ************************ RIMS *****************************

	x = 344;
	y = 64;
	for (i=0; i<MAX_RIMSPERPAGE; i++)
	{
		s_playermodel.rimports[i].generic.type		= MTYPE_BITMAP;
		s_playermodel.rimports[i].generic.name		= MODEL_PORT;
		s_playermodel.rimports[i].generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
		s_playermodel.rimports[i].generic.x			= x;
		s_playermodel.rimports[i].generic.y			= y;
		s_playermodel.rimports[i].width				= 64;
		s_playermodel.rimports[i].height			= 64;
		s_playermodel.rimports[i].focuspic			= MODEL_SELECTED;
		s_playermodel.rimports[i].focuscolor        = text_color_highlight;

		s_playermodel.rimpics[i].generic.type		= MTYPE_BITMAP;
		s_playermodel.rimpics[i].generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
		s_playermodel.rimpics[i].generic.x			= x;
		s_playermodel.rimpics[i].generic.y			= y;
		s_playermodel.rimpics[i].width  			= 64;
		s_playermodel.rimpics[i].height  			= 64;
		s_playermodel.rimpics[i].generic.name		= s_playermodel.rimIcons[i];

		s_playermodel.rimpicbuttons[i].generic.type		= MTYPE_BITMAP;
		s_playermodel.rimpicbuttons[i].generic.flags    = QMF_LEFT_JUSTIFY|QMF_NODEFAULTINIT|QMF_PULSEIFFOCUS;
		s_playermodel.rimpicbuttons[i].generic.id	    = ID_RIMPIC0+i;
		s_playermodel.rimpicbuttons[i].generic.callback = PlayerModel_RimPicEvent;
		s_playermodel.rimpicbuttons[i].generic.x    	= x;
		s_playermodel.rimpicbuttons[i].generic.left		= x;
		s_playermodel.rimpicbuttons[i].generic.right	= x + 64;
		s_playermodel.rimpicbuttons[i].generic.y		= y;
		s_playermodel.rimpicbuttons[i].generic.top		= y;
		s_playermodel.rimpicbuttons[i].generic.bottom   = y + 64;
		s_playermodel.rimpicbuttons[i].width  		    = 64;
		s_playermodel.rimpicbuttons[i].height  			= 64;
		s_playermodel.rimpicbuttons[i].focuspic  		= MODEL_SELECT;
		s_playermodel.rimpicbuttons[i].focuscolor  		= text_color_highlight;

		x += 64 + 6;
	}

	y = 138;
	s_playermodel.rimup.generic.type			= MTYPE_BITMAP;
	s_playermodel.rimup.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playermodel.rimup.generic.name			= MODEL_ARROWSL0;
	s_playermodel.rimup.generic.callback		= PlayerModel_MenuEvent;
	s_playermodel.rimup.generic.id				= ID_PREVRIMPAGE;
	s_playermodel.rimup.generic.x				= 360 - 16;
	s_playermodel.rimup.generic.y				= y;
	s_playermodel.rimup.width  					= 32;
	s_playermodel.rimup.height  				= 32;
	s_playermodel.rimup.focuspic				= MODEL_ARROWSL;

	s_playermodel.rimdown.generic.type			= MTYPE_BITMAP;
	s_playermodel.rimdown.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playermodel.rimdown.generic.name			= MODEL_ARROWSR0;
	s_playermodel.rimdown.generic.callback		= PlayerModel_MenuEvent;
	s_playermodel.rimdown.generic.id			= ID_NEXTRIMPAGE;
	s_playermodel.rimdown.generic.x				= 600 - 16;
	s_playermodel.rimdown.generic.y				= y;
	s_playermodel.rimdown.width  				= 32;
	s_playermodel.rimdown.height  				= 32;
	s_playermodel.rimdown.focuspic				= MODEL_ARROWSR;

	s_playermodel.rimname.generic.type  = MTYPE_PTEXT;
	s_playermodel.rimname.generic.flags = QMF_CENTER_JUSTIFY | QMF_INACTIVE;
	s_playermodel.rimname.generic.x		= 480;
	s_playermodel.rimname.generic.y		= y + 8;
	s_playermodel.rimname.string	    = s_playermodel.rimskin;
	s_playermodel.rimname.style			= UI_CENTER | UI_SMALLFONT;
	s_playermodel.rimname.color         = text_color_normal;

	// ************************ FAVORITES *****************************

	s_playermodel.favorites.generic.type   = MTYPE_PTEXT;
	s_playermodel.favorites.generic.flags  = QMF_CENTER_JUSTIFY|QMF_INACTIVE;
	s_playermodel.favorites.generic.x	   = 320;
	s_playermodel.favorites.generic.y	   = 378;
	s_playermodel.favorites.string	       = "ADD TO FAVORITES";
	s_playermodel.favorites.style		   = UI_CENTER | UI_SMALLFONT;
	s_playermodel.favorites.color          = text_color_normal;

	x =	183;
	y = 403;
	for (i=0; i < NUM_FAVORITES; i++)
	{
		s_playermodel.favports[i].generic.type		= MTYPE_BITMAP;
		s_playermodel.favports[i].generic.name		= MODEL_PORT;
		s_playermodel.favports[i].generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
		s_playermodel.favports[i].generic.x			= x;
		s_playermodel.favports[i].generic.y			= y;
		s_playermodel.favports[i].width  			= 64;
		s_playermodel.favports[i].height  			= 64;

		s_playermodel.favpics[i].generic.type	= MTYPE_BITMAP;
		s_playermodel.favpics[i].generic.flags	= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
		s_playermodel.favpics[i].generic.x		= x;
		s_playermodel.favpics[i].generic.y		= y;
		s_playermodel.favpics[i].width  		= 64;
		s_playermodel.favpics[i].height  		= 64;

		s_playermodel.favpicbuttons[i].generic.type			= MTYPE_BITMAP;
		s_playermodel.favpicbuttons[i].generic.flags		= QMF_LEFT_JUSTIFY|QMF_NODEFAULTINIT|QMF_PULSEIFFOCUS;
		s_playermodel.favpicbuttons[i].generic.id			= ID_FAVORITE1 + i;
		s_playermodel.favpicbuttons[i].generic.callback		= PlayerModel_PicEvent;
		s_playermodel.favpicbuttons[i].generic.x    		= x;
		s_playermodel.favpicbuttons[i].generic.y			= y;
		s_playermodel.favpicbuttons[i].generic.left			= x;
		s_playermodel.favpicbuttons[i].generic.top			= y;
		s_playermodel.favpicbuttons[i].generic.right		= x + 64;
		s_playermodel.favpicbuttons[i].generic.bottom		= y + 64;
		s_playermodel.favpicbuttons[i].width  				= 64;
		s_playermodel.favpicbuttons[i].height  				= 64;
		s_playermodel.favpicbuttons[i].focuspic  			= MODEL_SELECT;
		s_playermodel.favpicbuttons[i].focuscolor  			= text_color_highlight;

		x += 64 + 6;
	}
// END

	s_playermodel.player.generic.type      = MTYPE_BITMAP;
// STONELANCE
//	s_playermodel.player.generic.flags     = QMF_INACTIVE;
	s_playermodel.player.generic.flags      = QMF_LEFT_JUSTIFY | QMF_NODEFAULTINIT | QMF_SILENT;
// END
	s_playermodel.player.generic.ownerdraw = PlayerModel_DrawPlayer;
// STONELANCE
/*
	s_playermodel.player.generic.x	       = 400;
	s_playermodel.player.generic.y	       = -40;
	s_playermodel.player.width	           = 32*10;
	s_playermodel.player.height            = 56*10;
*/
	s_playermodel.player.generic.callback	= PlayerModel_RendererEvent;
	s_playermodel.player.generic.id			= ID_SPINMODEL;
	s_playermodel.player.generic.x	        = 40;
	s_playermodel.player.generic.y	        = 0;
	s_playermodel.player.generic.left		= 100;
	s_playermodel.player.generic.right		= 540;
	s_playermodel.player.generic.top		= 170;
	s_playermodel.player.generic.bottom		= 362;
	s_playermodel.player.width	            = 560;
	s_playermodel.player.height             = 480;

/*
	s_playermodel.arrows.generic.type		= MTYPE_BITMAP;
	s_playermodel.arrows.generic.name		= MODEL_ARROWS;
	s_playermodel.arrows.generic.flags		= QMF_INACTIVE;
	s_playermodel.arrows.generic.x			= 125;
	s_playermodel.arrows.generic.y			= 340;
	s_playermodel.arrows.width				= 128;
	s_playermodel.arrows.height				= 32;

	s_playermodel.left.generic.type			= MTYPE_BITMAP;
	s_playermodel.left.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playermodel.left.generic.callback		= PlayerModel_MenuEvent;
	s_playermodel.left.generic.id			= ID_PREVPAGE;
	s_playermodel.left.generic.x			= 125;
	s_playermodel.left.generic.y			= 340;
	s_playermodel.left.width  				= 64;
	s_playermodel.left.height  				= 32;
	s_playermodel.left.focuspic				= MODEL_ARROWSL;

	s_playermodel.right.generic.type	    = MTYPE_BITMAP;
	s_playermodel.right.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playermodel.right.generic.callback	= PlayerModel_MenuEvent;
	s_playermodel.right.generic.id			= ID_NEXTPAGE;
	s_playermodel.right.generic.x			= 125+61;
	s_playermodel.right.generic.y			= 340;
	s_playermodel.right.width  				= 64;
	s_playermodel.right.height  		    = 32;
	s_playermodel.right.focuspic			= MODEL_ARROWSR;

	s_playermodel.back.generic.type	    = MTYPE_BITMAP;
	s_playermodel.back.generic.name     = MODEL_BACK0;
	s_playermodel.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playermodel.back.generic.callback = PlayerModel_MenuEvent;
	s_playermodel.back.generic.id	    = ID_BACK;
	s_playermodel.back.generic.x		= 0;
	s_playermodel.back.generic.y		= 480-64;
	s_playermodel.back.width  		    = 128;
	s_playermodel.back.height  		    = 64;
	s_playermodel.back.focuspic         = MODEL_BACK1;
*/

	s_playermodel.back.generic.type				= MTYPE_PTEXT;
	s_playermodel.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playermodel.back.generic.x				= 20;
	s_playermodel.back.generic.y				= 480 - 50;
	s_playermodel.back.generic.id				= ID_BACK;
	s_playermodel.back.generic.callback			= PlayerModel_MenuEvent; 
	s_playermodel.back.string					= "< BACK";
	s_playermodel.back.color					= text_color_normal;
	s_playermodel.back.style					= UI_LEFT | UI_SMALLFONT;
// END

	PlayerModel_ApplyLayout();

	Menu_AddItem( &s_playermodel.menu, &s_playermodel.player );
	for (i = 0; i < MAX_MODELSPERPAGE; i++) {
		Menu_AddItem( &s_playermodel.menu, &s_playermodel.picbuttons[i] );
		Menu_AddItem( &s_playermodel.menu, &s_playermodel.pics[i] );
	}
	Menu_AddItem( &s_playermodel.menu, &s_playermodel.paintPrev );
	Menu_AddItem( &s_playermodel.menu, &s_playermodel.paintNext );

	for (i = 0; i < MAX_RIMSPERPAGE; i++) {
		Menu_AddItem( &s_playermodel.menu, &s_playermodel.rimpicbuttons[i] );
		Menu_AddItem( &s_playermodel.menu, &s_playermodel.rimpics[i] );
	}
	Menu_AddItem( &s_playermodel.menu, &s_playermodel.rimPrev );
	Menu_AddItem( &s_playermodel.menu, &s_playermodel.rimNext );

	for (i = 0; i < MAX_HEADSPERPAGE; i++) {
		Menu_AddItem( &s_playermodel.menu, &s_playermodel.headpicbuttons[i] );
		Menu_AddItem( &s_playermodel.menu, &s_playermodel.headpics[i] );
	}
	Menu_AddItem( &s_playermodel.menu, &s_playermodel.headPrev );
	Menu_AddItem( &s_playermodel.menu, &s_playermodel.headNext );

	for (i = 0; i < NUM_FAVORITES; i++) {
		Menu_AddItem( &s_playermodel.menu, &s_playermodel.favpicbuttons[i] );
		Menu_AddItem( &s_playermodel.menu, &s_playermodel.favpics[i] );
	}
	Menu_AddItem( &s_playermodel.menu,	&s_playermodel.back );

	// find all available models
//	PlayerModel_BuildList();

	// set initial states
	PlayerModel_SetMenuItems();

	// update user interface
	PlayerModel_UpdateGrid();
	PlayerModel_UpdateModel();
// STONELANCE
	PlayerModel_UpdateRimGrid();
	PlayerModel_UpdateHeadGrid();
	PlayerModel_UpdateFavorites();

	uis.transitionIn = uis.realtime;
// END
}

/*
=================
PlayerModel_Cache
=================
*/
void PlayerModel_Cache( void )
{
	int	i;

	for( i = 0; playermodel_artlist[i]; i++ ) {
		trap_R_RegisterShaderNoMip( playermodel_artlist[i] );
	}

// STONELANCE
	if (s_playermodel.modelname[0] == 0){
		trap_Cvar_VariableStringBuffer( "model", s_playermodel.modelname, sizeof(s_playermodel.modelname) );
	}
// END

	PlayerModel_BuildList();
	for( i = 0; i < s_playermodel.nummodels; i++ ) {
// STONELANCE
//		trap_R_RegisterShaderNoMip( s_playermodel.modelnames[i] );
		trap_R_RegisterShaderNoMip( va("models/players/%s/icon_%s", s_playermodel.modelname, s_playermodel.modelList[i]) );
// END
	}

// STONELANCE
	PlayerModel_BuildHeadList();
	for( i = 0; i < s_playermodel.numHeads; i++ ) {
		trap_R_RegisterShaderNoMip( va("models/players/heads/icon_%s", s_playermodel.headList[i]) );
	}

	PlayerModel_BuildRimList();
	for( i = 0; i < s_playermodel.numRims; i++ ) {
		trap_R_RegisterShaderNoMip( va("models/players/wheels/icon_%s", s_playermodel.rimList[i]) );
	}
// END
}

// STONELANCE
// void UI_PlayerModelMenu(void)
void UI_PlayerModelMenu( const char *modelName )
// END
{
// STONELANCE
	memset( &s_playermodel, 0 ,sizeof(playermodel_t) );

	Q_strncpyz(s_playermodel.modelname, modelName, sizeof(s_playermodel.modelname));
// END

	PlayerModel_MenuInit();

	UI_PushMenu( &s_playermodel.menu );

	Menu_SetCursorToItem( &s_playermodel.menu,
		&s_playermodel.picbuttons[s_playermodel.selectedmodel % MAX_MODELSPERPAGE] );
}


