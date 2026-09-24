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

void GraphicsOptions_MenuInit( void );
static void GraphicsOptions_Event( void *ptr, int event );

/*
=======================================================================

DRIVER INFORMATION MENU

=======================================================================
*/


// STONELANCE
/*
#define DRIVERINFO_FRAMEL	"menu/art/frame2_l"
#define DRIVERINFO_FRAMER	"menu/art/frame1_r"
#define DRIVERINFO_BACK0	"menu/art/back_0"
#define DRIVERINFO_BACK1	"menu/art/back_1"

static char* driverinfo_artlist[] = 
{
	DRIVERINFO_FRAMEL,
	DRIVERINFO_FRAMER,
	DRIVERINFO_BACK0,
	DRIVERINFO_BACK1,
	NULL,
};
*/
// END

#define ID_DRIVERINFOBACK	100

#define DRIVERINFO_FRAME_X		24
#define DRIVERINFO_FRAME_Y		20
#define DRIVERINFO_FRAME_WIDTH	592
#define DRIVERINFO_FRAME_HEIGHT	440
#define DRIVERINFO_SUMMARY_X	40
#define DRIVERINFO_SUMMARY_Y	104
#define DRIVERINFO_SUMMARY_WIDTH	560
#define DRIVERINFO_SUMMARY_HEIGHT	72
#define DRIVERINFO_PIXEL_X		40
#define DRIVERINFO_PIXEL_Y		196
#define DRIVERINFO_PIXEL_WIDTH	200
#define DRIVERINFO_PIXEL_HEIGHT	190
#define DRIVERINFO_EXT_X		256
#define DRIVERINFO_EXT_Y		196
#define DRIVERINFO_EXT_WIDTH	344
#define DRIVERINFO_EXT_HEIGHT	190
#define DRIVERINFO_EXT_LIST_X	272
#define DRIVERINFO_EXT_LIST_Y	248
#define DRIVERINFO_EXT_LIST_WIDTH	36
#define DRIVERINFO_EXT_LIST_HEIGHT	7
#define DRIVERINFO_ACTION_Y	420
#define DRIVERINFO_ACTION_WIDTH	120
#define DRIVERINFO_ACTION_HEIGHT	24

typedef struct
{
	menuframework_s	menu;
	menutext_s		banner;
// STONELANCE
/*
	menubitmap_s	back;
	menubitmap_s	framel;
	menubitmap_s	framer;
*/
	menutext_s		back;
	menulist_s		extensionList;

//	char			strings[64][128];
//	char*			stringitems[64];
// END
	char			stringbuff[1024];
	char*			strings[64];
	int				numstrings;
} driverinfo_t;

static driverinfo_t	s_driverinfo;
static vec4_t driverInfoScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t driverInfoTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t driverInfoMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t driverInfoAccentColor = UI_FRONTEND_COLOR_ACCENT;

static void DriverInfo_FitText( char *out, int outSize, const char *text,
	int maxWidth )
{
	int len;

	Q_strncpyz( out, text ? text : "-", outSize );
	if ( Frontend_TextWidth( out, UI_SMALLFONT ) <= maxWidth ) {
		return;
	}

	len = (int)strlen( out );
	while ( len > 3 && Frontend_TextWidth( out, UI_SMALLFONT ) > maxWidth ) {
		len--;
		out[len] = '\0';
	}
	if ( len >= 3 ) {
		out[len - 3] = '.';
		out[len - 2] = '.';
		out[len - 1] = '.';
	}
}

static void DriverInfo_DrawField( int x, int y, const char *label,
	const char *value, int maxWidth )
{
	char fitted[128];

	DriverInfo_FitText( fitted, sizeof( fitted ), value, maxWidth );
	Frontend_DrawText( x, y, label, UI_LEFT | UI_SMALLFONT,
		driverInfoMutedColor );
	Frontend_DrawText( x, y + 16, fitted, UI_LEFT | UI_SMALLFONT,
		driverInfoTextColor );
}

static void DriverInfo_DrawValueRow( int x, int y, const char *label,
	const char *value )
{
	Frontend_DrawText( x, y, label, UI_LEFT | UI_SMALLFONT,
		driverInfoMutedColor );
	Frontend_DrawText( x + 104, y, value, UI_LEFT | UI_BIGFONT,
		driverInfoTextColor );
}

static void DriverInfo_DrawExtensions( void *self )
{
	menulist_s *list;
	int i;

	list = (menulist_s *)self;
	if ( s_driverinfo.numstrings <= 0 ) {
		Frontend_DrawText( DRIVERINFO_EXT_LIST_X, DRIVERINFO_EXT_LIST_Y,
			"No extensions reported", UI_LEFT | UI_SMALLFONT,
			driverInfoMutedColor );
		return;
	}

	for ( i = 0; i < DRIVERINFO_EXT_LIST_HEIGHT; i++ ) {
		int index;
		int y;
		qboolean active;
		vec4_t textColor;
		char fitted[128];

		index = list->top + i;
		if ( index < 0 || index >= s_driverinfo.numstrings ) {
			break;
		}

		y = DRIVERINFO_EXT_LIST_Y + i * SMALLCHAR_HEIGHT;
		active = ( index == list->curvalue ) ? qtrue : qfalse;
		if ( active ) {
			UI_FillRect( DRIVERINFO_EXT_LIST_X, y, 2, SMALLCHAR_HEIGHT,
				driverInfoAccentColor );
			Vector4Copy( driverInfoAccentColor, textColor );
		} else {
			Vector4Copy( driverInfoMutedColor, textColor );
		}

		DriverInfo_FitText( fitted, sizeof( fitted ),
			s_driverinfo.strings[index], 288 );
		Frontend_DrawText( DRIVERINFO_EXT_LIST_X + 10, y, fitted,
			UI_LEFT | UI_SMALLFONT, textColor );
	}
}

static void DriverInfo_DrawAction( void *self )
{
	menutext_s *button;
	qboolean focus;

	button = (menutext_s *)self;
	focus = ( Menu_ItemAtCursor( button->generic.parent ) == button );
	Frontend_DrawButton( button->generic.left, button->generic.top,
		button->generic.right - button->generic.left,
		button->generic.bottom - button->generic.top,
		button->string, 1.0f, focus, UI_FRONTEND_TEXT_LEFT );
}

/*
=================
DriverInfo_Event
=================
*/
static void DriverInfo_Event( void* ptr, int event )
{
	if (event != QM_ACTIVATED)
		return;

	switch (((menucommon_s*)ptr)->id)
	{
		case ID_DRIVERINFOBACK:
			UI_PopMenu();
			break;
	}
}

/*
=================
DriverInfo_MenuDraw
=================
*/
static void DriverInfo_MenuDraw( void )
{
	char colorBits[32];
	char depthBits[32];
	char stencilBits[32];
	char extensionCount[32];

	Frontend_DrawBackground( driverInfoScrimColor );
	Frontend_DrawPanel( DRIVERINFO_FRAME_X, DRIVERINFO_FRAME_Y,
		DRIVERINFO_FRAME_WIDTH, DRIVERINFO_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( DRIVERINFO_FRAME_X + 24, DRIVERINFO_FRAME_Y + 24,
		"Driver info", UI_LEFT | UI_BIGFONT, driverInfoTextColor );
	Frontend_DrawText( DRIVERINFO_FRAME_X + 24, DRIVERINFO_FRAME_Y + 48,
		"Renderer, display capabilities and extensions",
		UI_LEFT | UI_SMALLFONT, driverInfoMutedColor );
	Frontend_DrawStatusChip( DRIVERINFO_FRAME_X + DRIVERINFO_FRAME_WIDTH - 112,
		DRIVERINFO_FRAME_Y + 26, "Renderer", driverInfoAccentColor, 1.0f );

	Frontend_DrawCard( DRIVERINFO_SUMMARY_X, DRIVERINFO_SUMMARY_Y,
		DRIVERINFO_SUMMARY_WIDTH, DRIVERINFO_SUMMARY_HEIGHT, 1.0f, qfalse );
	DriverInfo_DrawField( DRIVERINFO_SUMMARY_X + 16,
		DRIVERINFO_SUMMARY_Y + 12, "Vendor", uis.glconfig.vendor_string, 150 );
	DriverInfo_DrawField( DRIVERINFO_SUMMARY_X + 200,
		DRIVERINFO_SUMMARY_Y + 12, "Renderer", uis.glconfig.renderer_string, 150 );
	DriverInfo_DrawField( DRIVERINFO_SUMMARY_X + 384,
		DRIVERINFO_SUMMARY_Y + 12, "Version", uis.glconfig.version_string, 150 );

	Frontend_DrawCard( DRIVERINFO_PIXEL_X, DRIVERINFO_PIXEL_Y,
		DRIVERINFO_PIXEL_WIDTH, DRIVERINFO_PIXEL_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( DRIVERINFO_PIXEL_X + 16, DRIVERINFO_PIXEL_Y + 20,
		"Pixel format", UI_LEFT | UI_SMALLFONT, driverInfoMutedColor );
	Com_sprintf( colorBits, sizeof( colorBits ), "%d bit", uis.glconfig.colorBits );
	Com_sprintf( depthBits, sizeof( depthBits ), "%d bit", uis.glconfig.depthBits );
	Com_sprintf( stencilBits, sizeof( stencilBits ), "%d bit", uis.glconfig.stencilBits );
	DriverInfo_DrawValueRow( DRIVERINFO_PIXEL_X + 16,
		DRIVERINFO_PIXEL_Y + 62, "Color", colorBits );
	DriverInfo_DrawValueRow( DRIVERINFO_PIXEL_X + 16,
		DRIVERINFO_PIXEL_Y + 94, "Depth", depthBits );
	DriverInfo_DrawValueRow( DRIVERINFO_PIXEL_X + 16,
		DRIVERINFO_PIXEL_Y + 126, "Stencil", stencilBits );

	Frontend_DrawCard( DRIVERINFO_EXT_X, DRIVERINFO_EXT_Y,
		DRIVERINFO_EXT_WIDTH, DRIVERINFO_EXT_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( DRIVERINFO_EXT_X + 16, DRIVERINFO_EXT_Y + 20,
		"Extensions", UI_LEFT | UI_SMALLFONT, driverInfoMutedColor );
	Com_sprintf( extensionCount, sizeof( extensionCount ), "%d detected",
		s_driverinfo.numstrings );
	Frontend_DrawText( DRIVERINFO_EXT_X + DRIVERINFO_EXT_WIDTH - 16,
		DRIVERINFO_EXT_Y + 20, extensionCount, UI_RIGHT | UI_SMALLFONT,
		driverInfoAccentColor );

	Menu_Draw( &s_driverinfo.menu );

	Frontend_DrawText( DRIVERINFO_FRAME_X + 24, DRIVERINFO_FRAME_Y + 384,
		"Select an extension   Up / down scroll   Esc back",
		UI_LEFT | UI_SMALLFONT, driverInfoMutedColor );
}

/*
=================
DriverInfo_Cache
=================
*/
void DriverInfo_Cache( void )
{
// STONELANCE
/*
	int	i;

	// touch all our pics
	for (i=0; ;i++)
	{
		if (!driverinfo_artlist[i])
			break;
		trap_R_RegisterShaderNoMip(driverinfo_artlist[i]);
	}
*/
// END
}

/*
=================
UI_DriverInfo_Menu
=================
*/
static void UI_DriverInfo_Menu( void )
{
	char*	eptr;
	int		i;
	int		len;

	// zero set all our globals
	memset( &s_driverinfo, 0 ,sizeof(driverinfo_t) );

	DriverInfo_Cache();

	s_driverinfo.menu.fullscreen = qtrue;
	s_driverinfo.menu.draw       = DriverInfo_MenuDraw;

	s_driverinfo.banner.generic.type  = MTYPE_BTEXT;
	s_driverinfo.banner.generic.x	  = 320;
	s_driverinfo.banner.generic.y	  = 16;
	s_driverinfo.banner.string		  = "DRIVER INFO";
	s_driverinfo.banner.color	      = color_white;
	s_driverinfo.banner.style	      = UI_CENTER;

// STONELANCE
/*
	s_driverinfo.framel.generic.type  = MTYPE_BITMAP;
	s_driverinfo.framel.generic.name  = DRIVERINFO_FRAMEL;
	s_driverinfo.framel.generic.flags = QMF_INACTIVE;
	s_driverinfo.framel.generic.x	  = 0;
	s_driverinfo.framel.generic.y	  = 78;
	s_driverinfo.framel.width  	      = 256;
	s_driverinfo.framel.height  	  = 329;

	s_driverinfo.framer.generic.type  = MTYPE_BITMAP;
	s_driverinfo.framer.generic.name  = DRIVERINFO_FRAMER;
	s_driverinfo.framer.generic.flags = QMF_INACTIVE;
	s_driverinfo.framer.generic.x	  = 376;
	s_driverinfo.framer.generic.y	  = 76;
	s_driverinfo.framer.width  	      = 256;
	s_driverinfo.framer.height  	  = 334;

	s_driverinfo.back.generic.type	   = MTYPE_BITMAP;
	s_driverinfo.back.generic.name     = DRIVERINFO_BACK0;
	s_driverinfo.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_driverinfo.back.generic.callback = DriverInfo_Event;
	s_driverinfo.back.generic.id	   = ID_DRIVERINFOBACK;
	s_driverinfo.back.generic.x		   = 0;
	s_driverinfo.back.generic.y		   = 480-64;
	s_driverinfo.back.width  		   = 128;
	s_driverinfo.back.height  		   = 64;
	s_driverinfo.back.focuspic         = DRIVERINFO_BACK1;
*/
	s_driverinfo.back.generic.type				= MTYPE_PTEXT;
	s_driverinfo.back.generic.flags				= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_driverinfo.back.generic.x					= DRIVERINFO_FRAME_X + 24;
	s_driverinfo.back.generic.y					= DRIVERINFO_ACTION_Y;
	s_driverinfo.back.generic.id				= ID_DRIVERINFOBACK;
	s_driverinfo.back.generic.callback			= DriverInfo_Event; 
	s_driverinfo.back.string					= "Back";
	s_driverinfo.back.color						= driverInfoTextColor;
	s_driverinfo.back.style						= UI_LEFT | UI_SMALLFONT;
// END

  // TTimo: overflow with particularly long GL extensions (such as the gf3)
  // https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=399
  // NOTE: could have pushed the size of stringbuff, but the list is already out of the screen
  // (no matter what your resolution)
  Q_strncpyz(s_driverinfo.stringbuff, uis.glconfig.extensions_string, 1024);

	// build null terminated extension strings
	eptr = s_driverinfo.stringbuff;
// STONELANCE
//	while ( s_driverinfo.numstrings<40 && *eptr )
	while ( s_driverinfo.numstrings < 64 && *eptr )
// END
	{
		while ( *eptr && *eptr == ' ' )
			*eptr++ = '\0';

		// track start of valid string
		if (*eptr && *eptr != ' ')
			s_driverinfo.strings[s_driverinfo.numstrings++] = eptr;

		while ( *eptr && *eptr != ' ' )
			eptr++;
	}

	// safety length strings for display
	for (i=0; i<s_driverinfo.numstrings; i++) {
		len = strlen(s_driverinfo.strings[i]);
// STONELANCE
//		if (len > 32) {
		if (len > 40) {
// END
			s_driverinfo.strings[i][len-1] = '>';
			s_driverinfo.strings[i][len]   = '\0';
		}
	}

// STONELANCE
	s_driverinfo.extensionList.generic.type			= MTYPE_LISTBOX;
	s_driverinfo.extensionList.generic.flags		= QMF_CENTER_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS | QMF_SCROLL_ONLY;
	s_driverinfo.extensionList.scrollbarAlignment	= SB_RIGHT | SB_HIDE;
	s_driverinfo.extensionList.generic.x			= DRIVERINFO_EXT_LIST_X +
				(DRIVERINFO_EXT_LIST_WIDTH * SMALLCHAR_WIDTH) / 2;
	s_driverinfo.extensionList.generic.y			= DRIVERINFO_EXT_LIST_Y;
	s_driverinfo.extensionList.width				= DRIVERINFO_EXT_LIST_WIDTH;
	s_driverinfo.extensionList.height				= DRIVERINFO_EXT_LIST_HEIGHT;
	s_driverinfo.extensionList.itemnames			= (const char **)s_driverinfo.strings;
	s_driverinfo.extensionList.numitems				= s_driverinfo.numstrings;
	s_driverinfo.extensionList.generic.ownerdraw		= DriverInfo_DrawExtensions;
// END

// The modern header is drawn by DriverInfo_MenuDraw.
// STONELANCE
/*
	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.framel );
	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.framer );
*/
	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.extensionList );
// END
	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.back );

	s_driverinfo.back.generic.ownerdraw = DriverInfo_DrawAction;
	s_driverinfo.back.generic.left = DRIVERINFO_FRAME_X + 24;
	s_driverinfo.back.generic.top = DRIVERINFO_ACTION_Y;
	s_driverinfo.back.generic.right = DRIVERINFO_FRAME_X + 24 +
		DRIVERINFO_ACTION_WIDTH;
	s_driverinfo.back.generic.bottom = DRIVERINFO_ACTION_Y +
		DRIVERINFO_ACTION_HEIGHT;

	UI_PushMenu( &s_driverinfo.menu );
	Menu_SetCursorToItem( &s_driverinfo.menu, &s_driverinfo.extensionList );
}

/*
=======================================================================

GRAPHICS OPTIONS MENU

=======================================================================
*/

// STONELANCE
/*
#define GRAPHICSOPTIONS_FRAMEL	"menu/art/frame2_l"
#define GRAPHICSOPTIONS_FRAMER	"menu/art/frame1_r"
#define GRAPHICSOPTIONS_BACK0	"menu/art/back_0"
#define GRAPHICSOPTIONS_BACK1	"menu/art/back_1"
#define GRAPHICSOPTIONS_ACCEPT0	"menu/art/accept_0"
#define GRAPHICSOPTIONS_ACCEPT1	"menu/art/accept_1"
*/
// END

#define ID_BACK2		101
#define ID_FULLSCREEN	102
#define ID_LIST			103
#define ID_MODE			104
#define ID_DRIVERINFO	105
#define ID_GRAPHICS		106
#define ID_ADVANCED_GRAPHICS	107
#define ID_DISPLAY		108
#define ID_SOUND		109
#define ID_NETWORK		110
#define ID_RATIO		111

#define ID_ANISOTROPY	112
#define ID_MSAA			113
#define ID_DRIVER		114
#define ID_EXTENSIONS	115
#define ID_COLORDEPTH	116
#define ID_LIGHTING		117
#define ID_GEOMETRY		118
#define ID_TEXTUREDETAIL	119
#define ID_TEXTUREQUALITY	120
#define ID_FILTER		121
#define ID_APPLY		122

#define GRAPHICS_FRAME_X		24
#define GRAPHICS_FRAME_Y		20
#define GRAPHICS_FRAME_WIDTH	592
#define GRAPHICS_FRAME_HEIGHT	440
#define GRAPHICS_NAV_X		40
#define GRAPHICS_NAV_Y		104
#define GRAPHICS_NAV_WIDTH	164
#define GRAPHICS_NAV_HEIGHT	292
#define GRAPHICS_DETAIL_X	220
#define GRAPHICS_DETAIL_Y	104
#define GRAPHICS_DETAIL_WIDTH	376
#define GRAPHICS_DETAIL_HEIGHT	292
#define GRAPHICS_ROW_HEIGHT	24
#define GRAPHICS_ROW_GAP		4
#define GRAPHICS_ROW_START_Y	148
#define GRAPHICS_COLUMN_WIDTH	176
#define GRAPHICS_COLUMN_GAP	8
#define GRAPHICS_COLUMN_LEFT_X	236
#define GRAPHICS_COLUMN_RIGHT_X	420
#define GRAPHICS_VALUE_OFFSET	112
#define GRAPHICS_ACTION_Y		420
#define GRAPHICS_ACTION_HEIGHT	24
#define GRAPHICS_ACTION_WIDTH	120

static vec4_t graphicsScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t graphicsAccentColor = UI_FRONTEND_COLOR_ACCENT;
static vec4_t graphicsTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t graphicsMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t graphicsBorderColor = UI_FRONTEND_COLOR_BORDER;
static vec4_t graphicsFocusColor = UI_FRONTEND_COLOR_FOCUS_BG;

typedef struct {
	menuframework_s	menu;

	menutext_s		banner;
// STONELANCE
/*
	menubitmap_s	framel;
	menubitmap_s	framer;
*/
// END

	menutext_s		graphics;
	menutext_s		advanced_graphics;
	menutext_s		display;
	menutext_s		sound;
	menutext_s		network;

	menulist_s		list;
	menulist_s		ratio;
	menulist_s		mode;
	menulist_s		driver;
	menuslider_s	tq;
	menulist_s  	fs;
	menulist_s  	lighting;
	menulist_s  	allow_extensions;
	menulist_s  	texturebits;
	menulist_s  	colordepth;
	menulist_s  	geometry;
	menulist_s  	filter;
	menulist_s		anisotropy;
	menulist_s		msaa;
	menutext_s		driverinfo;

// STONELANCE
//	menubitmap_s	apply;
//	menubitmap_s	back;
	menutext_s		apply;
	menutext_s		back;
// END
} graphicsoptions_t;

typedef struct
{
	int mode;
	qboolean fullscreen;
	int tq;
	int lighting;
	int colordepth;
	int texturebits;
	int geometry;
	int filter;
	int anisotropy;
	int msaa;
	int driver;
	qboolean extensions;
} InitialVideoOptions_s;

static InitialVideoOptions_s	s_ivo;
static graphicsoptions_t		s_graphicsoptions;	

static InitialVideoOptions_s s_ivo_templates[] =
{
	{
		6, qtrue, 3, 0, 2, 2, 2, 1, 0, 0, 0, qtrue
	},
	{
		4, qtrue, 2, 0, 2, 2, 1, 1, 0, 0, 0, qtrue	// JDC: this was tq 3
	},
	{
		3, qtrue, 2, 0, 0, 0, 1, 0, 0, 0, 0, qtrue
	},
	{
		2, qtrue, 1, 0, 1, 0, 0, 0, 0, 0, 0, qtrue
	},
	{
		2, qtrue, 1, 1, 1, 0, 0, 0, 0, 0, 0, qtrue
	},
	{
		3, qtrue, 1, 0, 0, 0, 1, 0, 0, 0, 0, qtrue
	}
};

#define NUM_IVO_TEMPLATES ( ARRAY_LEN( s_ivo_templates ) )

static const char *builtinResolutions[ ] =
{
	"320x240",
	"400x300",
	"512x384",
	"640x480",
	"800x600",
	"960x720",
	"1024x768",
	"1152x864",
	"1280x1024",
	"1600x1200",
	"2048x1536",
	"856x480",
	NULL
};

static const char *knownRatios[ ][2] =
{
	{ "1.25:1", "5:4"   },
	{ "1.33:1", "4:3"   },
	{ "1.50:1", "3:2"   },
	{ "1.56:1", "14:9"  },
	{ "1.60:1", "16:10" },
	{ "1.67:1", "5:3"   },
	{ "1.78:1", "16:9"  },
	{ NULL    , NULL    }
};

#define MAX_RESOLUTIONS	32

static const char* ratios[ MAX_RESOLUTIONS ];
static char ratioBuf[ MAX_RESOLUTIONS ][ 8 ];
static int ratioToRes[ MAX_RESOLUTIONS ];
static int resToRatio[ MAX_RESOLUTIONS ];

static char resbuf[ MAX_STRING_CHARS ];
static const char* detectedResolutions[ MAX_RESOLUTIONS ];
static char currentResolution[ 20 ];

static const char** resolutions = builtinResolutions;
static qboolean resolutionsDetected = qfalse;

/*
=================
GraphicsOptions_FindBuiltinResolution
=================
*/
static int GraphicsOptions_FindBuiltinResolution( int mode )
{
	int i;

	if( !resolutionsDetected )
		return mode;

	if( mode < 0 )
		return -1;

	for( i = 0; builtinResolutions[ i ]; i++ )
	{
		if( !Q_stricmp( builtinResolutions[ i ], detectedResolutions[ mode ] ) )
			return i;
	}

	return -1;
}

/*
=================
GraphicsOptions_FindDetectedResolution
=================
*/
static int GraphicsOptions_FindDetectedResolution( int mode )
{
	int i;

	if( !resolutionsDetected )
		return mode;

	if( mode < 0 )
		return -1;

	for( i = 0; detectedResolutions[ i ]; i++ )
	{
		if( !Q_stricmp( builtinResolutions[ mode ], detectedResolutions[ i ] ) )
			return i;
	}

	return -1;
}

/*
=================
GraphicsOptions_GetAspectRatios
=================
*/
static void GraphicsOptions_GetAspectRatios( void )
{
	int i, r;

	// build ratio list from resolutions
	for( r = 0; resolutions[r]; r++ )
	{
		int w, h;
		char *x;
		char str[ sizeof(ratioBuf[0]) ];

		// calculate resolution's aspect ratio
		x = strchr( resolutions[r], 'x' ) + 1;
		Q_strncpyz( str, resolutions[r], x-resolutions[r] );
		w = atoi( str );
		h = atoi( x );
		Com_sprintf( str, sizeof(str), "%.2f:1", (float)w / (float)h );

		// rename common ratios ("1.33:1" -> "4:3")
		for( i = 0; knownRatios[i][0]; i++ ) {
			if( !Q_stricmp( str, knownRatios[i][0] ) ) {
				Q_strncpyz( str, knownRatios[i][1], sizeof( str ) );
				break;
			}
		}

		// add ratio to list if it is new
		// establish res/ratio relationship
		for( i = 0; ratioBuf[i][0]; i++ )
		{
			if( !Q_stricmp( str, ratioBuf[i] ) )
				break;
		}
		if( !ratioBuf[i][0] )
		{
			Q_strncpyz( ratioBuf[i], str, sizeof(ratioBuf[i]) );
			ratioToRes[i] = r;
		}

		ratios[r] = ratioBuf[r]; 
		resToRatio[r] = i; 
	}

	ratios[r] = NULL;
}

/*
=================
GraphicsOptions_GetInitialVideo
=================
*/
static void GraphicsOptions_GetInitialVideo( void )
{
	s_ivo.colordepth  = s_graphicsoptions.colordepth.curvalue;
	s_ivo.driver      = s_graphicsoptions.driver.curvalue;
	s_ivo.mode        = s_graphicsoptions.mode.curvalue;
	s_ivo.fullscreen  = s_graphicsoptions.fs.curvalue;
	s_ivo.extensions  = s_graphicsoptions.allow_extensions.curvalue;
	s_ivo.tq          = s_graphicsoptions.tq.curvalue;
	s_ivo.lighting    = s_graphicsoptions.lighting.curvalue;
	s_ivo.geometry    = s_graphicsoptions.geometry.curvalue;
	s_ivo.filter      = s_graphicsoptions.filter.curvalue;
	s_ivo.anisotropy  = s_graphicsoptions.anisotropy.curvalue;
	s_ivo.msaa        = s_graphicsoptions.msaa.curvalue;
	s_ivo.texturebits = s_graphicsoptions.texturebits.curvalue;
}

/*
=================
GraphicsOptions_GetResolutions
=================
*/
static void GraphicsOptions_GetResolutions( void )
{
	trap_Cvar_VariableStringBuffer("r_availableModes", resbuf, sizeof(resbuf));
	if(*resbuf)
	{
		char* s = resbuf;
		unsigned int i = 0;
		while( s && i < ARRAY_LEN(detectedResolutions)-1 )
		{
			detectedResolutions[i++] = s;
			s = strchr(s, ' ');
			if( s )
				*s++ = '\0';
		}
		detectedResolutions[ i ] = NULL;

		// add custom resolution if not in mode list
		if ( i < ARRAY_LEN(detectedResolutions)-1 )
		{
			Com_sprintf( currentResolution, sizeof ( currentResolution ), "%dx%d", uis.glconfig.vidWidth, uis.glconfig.vidHeight );

			for( i = 0; detectedResolutions[ i ]; i++ )
			{
				if ( strcmp( detectedResolutions[ i ], currentResolution ) == 0 )
					break;
			}

			if ( detectedResolutions[ i ] == NULL )
			{
				detectedResolutions[ i++ ] = currentResolution;
				detectedResolutions[ i ] = NULL;
			}
		}

		resolutions = detectedResolutions;
		resolutionsDetected = qtrue;
	}
}

/*
=================
GraphicsOptions_CheckConfig
=================
*/
static void GraphicsOptions_CheckConfig( void )
{
	int i;

	for ( i = 0; i < NUM_IVO_TEMPLATES-1; i++ )
	{
		if ( s_ivo_templates[i].colordepth != s_graphicsoptions.colordepth.curvalue )
			continue;
		if ( s_ivo_templates[i].driver != s_graphicsoptions.driver.curvalue )
			continue;
		if ( GraphicsOptions_FindDetectedResolution(s_ivo_templates[i].mode) != s_graphicsoptions.mode.curvalue )
			continue;
		if ( s_ivo_templates[i].fullscreen != s_graphicsoptions.fs.curvalue )
			continue;
		if ( s_ivo_templates[i].tq != s_graphicsoptions.tq.curvalue )
			continue;
		if ( s_ivo_templates[i].lighting != s_graphicsoptions.lighting.curvalue )
			continue;
		if ( s_ivo_templates[i].geometry != s_graphicsoptions.geometry.curvalue )
			continue;
		if ( s_ivo_templates[i].filter != s_graphicsoptions.filter.curvalue )
			continue;
		if ( s_ivo_templates[i].anisotropy != s_graphicsoptions.anisotropy.curvalue )
			continue;
		if ( s_ivo_templates[i].msaa != s_graphicsoptions.msaa.curvalue )
			continue;
//		if ( s_ivo_templates[i].texturebits != s_graphicsoptions.texturebits.curvalue )
//			continue;
		s_graphicsoptions.list.curvalue = i;
		return;
	}

	// return 'Custom' ivo template
	s_graphicsoptions.list.curvalue = NUM_IVO_TEMPLATES - 1;
}

/*
=================
GraphicsOptions_UpdateMenuItems
=================
*/
static void GraphicsOptions_UpdateMenuItems( void )
{
	if ( s_graphicsoptions.driver.curvalue == 1 )
	{
		s_graphicsoptions.fs.curvalue = 1;
		s_graphicsoptions.fs.generic.flags |= QMF_GRAYED;
		s_graphicsoptions.colordepth.curvalue = 1;
	}
	else
	{
		s_graphicsoptions.fs.generic.flags &= ~QMF_GRAYED;
	}

	if ( s_graphicsoptions.fs.curvalue == 0 || s_graphicsoptions.driver.curvalue == 1 )
	{
		s_graphicsoptions.colordepth.curvalue = 0;
		s_graphicsoptions.colordepth.generic.flags |= QMF_GRAYED;
	}
	else
	{
		s_graphicsoptions.colordepth.generic.flags &= ~QMF_GRAYED;
	}

	if ( s_graphicsoptions.allow_extensions.curvalue == 0 )
	{
		if ( s_graphicsoptions.texturebits.curvalue == 0 )
		{
			s_graphicsoptions.texturebits.curvalue = 1;
		}
		s_graphicsoptions.anisotropy.curvalue = 0;
		s_graphicsoptions.msaa.curvalue = 0;
		s_graphicsoptions.anisotropy.generic.flags |= QMF_GRAYED;
		s_graphicsoptions.msaa.generic.flags |= QMF_GRAYED;
	}
	else
	{
		s_graphicsoptions.anisotropy.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.msaa.generic.flags &= ~QMF_GRAYED;
	}

	s_graphicsoptions.apply.generic.flags |= QMF_HIDDEN|QMF_INACTIVE;

	if ( s_ivo.mode != s_graphicsoptions.mode.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.fullscreen != s_graphicsoptions.fs.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.extensions != s_graphicsoptions.allow_extensions.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.tq != s_graphicsoptions.tq.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.lighting != s_graphicsoptions.lighting.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.colordepth != s_graphicsoptions.colordepth.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.driver != s_graphicsoptions.driver.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.texturebits != s_graphicsoptions.texturebits.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.geometry != s_graphicsoptions.geometry.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.filter != s_graphicsoptions.filter.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.anisotropy != s_graphicsoptions.anisotropy.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.msaa != s_graphicsoptions.msaa.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}

	GraphicsOptions_CheckConfig();
}	

static const char *GraphicsOptions_ListValue( menulist_s *item )
{
	int i;

	if ( !item->itemnames || item->curvalue < 0 ) {
		return "-";
	}

	for ( i = 0; item->itemnames[i]; i++ ) {
		if ( i == item->curvalue ) {
			return item->itemnames[i];
		}
	}

	return "-";
}

static const char *GraphicsOptions_CurrentValue( menucommon_s *item )
{
	if ( item->id == ID_LIST ) {
		switch ( s_graphicsoptions.list.curvalue ) {
		case 0:
			return "Very high";
		case 1:
			return "High";
		case 2:
			return "Normal";
		case 3:
			return "Fast";
		case 4:
			return "Fastest";
		default:
			return "Custom";
		}
	}

	if ( item->id == ID_TEXTUREDETAIL ) {
		switch ( (int)s_graphicsoptions.tq.curvalue ) {
		case 0:
			return "Low";
		case 1:
			return "Medium";
		case 2:
			return "High";
		default:
			return "Ultra";
		}
	}

	return GraphicsOptions_ListValue( (menulist_s *)item );
}

static void GraphicsOptions_DrawSetting( void *self )
{
	menucommon_s *item;
	const char *value;
	qboolean disabled;
	qboolean focus;
	qboolean hovered;
	qboolean highlighted;
	vec4_t fillColor;
	vec4_t lineColor;
	vec4_t labelColor;
	vec4_t valueColor;

	item = (menucommon_s *)self;
	disabled = ( item->flags & ( QMF_GRAYED | QMF_INACTIVE ) ) ? qtrue : qfalse;
	focus = ( !disabled && Menu_ItemAtCursor( item->parent ) == item ) ? qtrue : qfalse;
	hovered = ( !disabled && uis.cursorx >= item->left &&
		uis.cursorx <= item->right && uis.cursory >= item->top &&
		uis.cursory <= item->bottom ) ? qtrue : qfalse;
	highlighted = ( focus || hovered ) ? qtrue : qfalse;
	value = GraphicsOptions_CurrentValue( item );

	fillColor[0] = graphicsFocusColor[0];
	fillColor[1] = graphicsFocusColor[1];
	fillColor[2] = graphicsFocusColor[2];
	fillColor[3] = highlighted ? 0.72f : 0.0f;
	lineColor[0] = graphicsBorderColor[0];
	lineColor[1] = graphicsBorderColor[1];
	lineColor[2] = graphicsBorderColor[2];
	lineColor[3] = 0.55f;

	if ( highlighted ) {
		UI_FillRect( item->left, item->top,
			item->right - item->left, item->bottom - item->top, fillColor );
		lineColor[0] = graphicsAccentColor[0];
		lineColor[1] = graphicsAccentColor[1];
		lineColor[2] = graphicsAccentColor[2];
		lineColor[3] = 1.0f;
	}
	UI_FillRect( item->left, item->bottom - 1,
		item->right - item->left, 1, lineColor );

	if ( disabled ) {
		labelColor[0] = graphicsMutedColor[0];
		labelColor[1] = graphicsMutedColor[1];
		labelColor[2] = graphicsMutedColor[2];
		labelColor[3] = 0.65f;
		Vector4Copy( labelColor, valueColor );
	} else if ( highlighted ) {
		Vector4Copy( graphicsAccentColor, labelColor );
		Vector4Copy( graphicsTextColor, valueColor );
	} else {
		Vector4Copy( graphicsMutedColor, labelColor );
		Vector4Copy( graphicsTextColor, valueColor );
	}

	Frontend_DrawText( item->left + UI_FRONTEND_SPACE_SM,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		item->name, UI_LEFT | UI_SMALLFONT, labelColor );
	Frontend_DrawText( item->left + GRAPHICS_VALUE_OFFSET,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		value, UI_LEFT | UI_SMALLFONT, valueColor );
}

static void GraphicsOptions_DrawAction( void *self )
{
	menutext_s *text;
	menucommon_s *item;
	qboolean focus;

	text = (menutext_s *)self;
	item = &text->generic;
	focus = ( Menu_ItemAtCursor( item->parent ) == item );
	Frontend_DrawButton( item->left, item->top,
		item->right - item->left, item->bottom - item->top,
		text->string, 1.0f, focus, UI_CENTER );
}

/*
=================
GraphicsOptions_ApplyChanges
=================
*/
static void GraphicsOptions_ApplyChanges( void *unused, int notification )
{
	if (notification != QM_ACTIVATED)
		return;

	switch ( s_graphicsoptions.texturebits.curvalue  )
	{
	case 0:
		trap_Cvar_SetValue( "r_texturebits", 0 );
		break;
	case 1:
		trap_Cvar_SetValue( "r_texturebits", 16 );
		break;
	case 2:
		trap_Cvar_SetValue( "r_texturebits", 32 );
		break;
	}
	trap_Cvar_SetValue( "r_picmip", 3 - s_graphicsoptions.tq.curvalue );
	trap_Cvar_SetValue( "r_allowExtensions", s_graphicsoptions.allow_extensions.curvalue );

	if( resolutionsDetected )
	{
		// search for builtin mode that matches the detected mode
		int mode;
		if ( s_graphicsoptions.mode.curvalue == -1
			|| s_graphicsoptions.mode.curvalue >= ARRAY_LEN( detectedResolutions ) )
			s_graphicsoptions.mode.curvalue = 0;

		mode = GraphicsOptions_FindBuiltinResolution( s_graphicsoptions.mode.curvalue );
		if( mode == -1 )
		{
			char w[ 16 ], h[ 16 ];
			Q_strncpyz( w, detectedResolutions[ s_graphicsoptions.mode.curvalue ], sizeof( w ) );
			*strchr( w, 'x' ) = 0;
			Q_strncpyz( h,
					strchr( detectedResolutions[ s_graphicsoptions.mode.curvalue ], 'x' ) + 1, sizeof( h ) );
			trap_Cvar_Set( "r_customwidth", w );
			trap_Cvar_Set( "r_customheight", h );
		}

		trap_Cvar_SetValue( "r_mode", mode );
	}
	else
		trap_Cvar_SetValue( "r_mode", s_graphicsoptions.mode.curvalue );

	trap_Cvar_SetValue( "r_fullscreen", s_graphicsoptions.fs.curvalue );
	switch ( s_graphicsoptions.colordepth.curvalue )
	{
	case 0:
		trap_Cvar_SetValue( "r_colorbits", 0 );
		trap_Cvar_SetValue( "r_depthbits", 0 );
		trap_Cvar_Reset( "r_stencilbits" );
		break;
	case 1:
		trap_Cvar_SetValue( "r_colorbits", 16 );
		trap_Cvar_SetValue( "r_depthbits", 16 );
		trap_Cvar_SetValue( "r_stencilbits", 0 );
		break;
	case 2:
		trap_Cvar_SetValue( "r_colorbits", 32 );
		trap_Cvar_SetValue( "r_depthbits", 24 );
		trap_Cvar_SetValue( "r_stencilbits", 8 );
		break;
	}
	trap_Cvar_SetValue( "r_vertexLight", s_graphicsoptions.lighting.curvalue );

	if ( s_graphicsoptions.geometry.curvalue == 2 )
	{
		trap_Cvar_SetValue( "r_lodBias", 0 );
		trap_Cvar_SetValue( "r_subdivisions", 4 );
	}
	else if ( s_graphicsoptions.geometry.curvalue == 1 )
	{
		trap_Cvar_SetValue( "r_lodBias", 1 );
		trap_Cvar_SetValue( "r_subdivisions", 12 );
	}
	else
	{
		trap_Cvar_SetValue( "r_lodBias", 1 );
		trap_Cvar_SetValue( "r_subdivisions", 20 );
	}

	if ( s_graphicsoptions.filter.curvalue )
	{
		trap_Cvar_Set( "r_textureMode", "GL_LINEAR_MIPMAP_LINEAR" );
	}
	else
	{
		trap_Cvar_Set( "r_textureMode", "GL_LINEAR_MIPMAP_NEAREST" );
	}

	if ( s_graphicsoptions.anisotropy.curvalue == 0 )
	{
		trap_Cvar_SetValue( "r_ext_texture_filter_anisotropic", 0 );
		trap_Cvar_SetValue( "r_ext_max_anisotropy", 1 );
	}
	else
	{
		static const int anisoValues[] = { 2, 4, 8, 16 };
		int anisoIndex = s_graphicsoptions.anisotropy.curvalue - 1;
		if ( anisoIndex < 0 )
			anisoIndex = 0;
		if ( anisoIndex > 3 )
			anisoIndex = 3;
		trap_Cvar_SetValue( "r_ext_texture_filter_anisotropic", 1 );
		trap_Cvar_SetValue( "r_ext_max_anisotropy", anisoValues[anisoIndex] );
	}

	{
		static const int msaaValues[] = { 0, 2, 4 };
		int msaaIndex = s_graphicsoptions.msaa.curvalue;
		if ( msaaIndex < 0 )
			msaaIndex = 0;
		if ( msaaIndex > 2 )
			msaaIndex = 2;
		trap_Cvar_SetValue( "r_ext_multisample", msaaValues[msaaIndex] );
		if ( !Q_stricmp( UI_Cvar_VariableString( "cl_renderer" ), "opengl2" ) )
		{
			trap_Cvar_SetValue( "r_ext_framebuffer_multisample", msaaValues[msaaIndex] );
		}
	}

	/* A video restart reloads the UI VM and therefore clears the menu stack.
	 * Remember that this Apply came from Config so the freshly initialized
	 * frontend can restore the user to the Config hub instead of the main
	 * menu. */
	trap_Cmd_ExecuteText( EXEC_APPEND,
		"set q3r_ui_return_config 1\nvid_restart\n" );
}

/*
=================
GraphicsOptions_Event
=================
*/
static void GraphicsOptions_Event( void* ptr, int event ) {
	InitialVideoOptions_s *ivo;

	if( event != QM_ACTIVATED ) {
	 	return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_RATIO:
		s_graphicsoptions.mode.curvalue =
			ratioToRes[ s_graphicsoptions.ratio.curvalue ];
		// fall through to apply mode constraints
		
	case ID_MODE:
		// clamp 3dfx video modes
		if ( s_graphicsoptions.driver.curvalue == 1 )
		{
			if ( s_graphicsoptions.mode.curvalue < 2 )
				s_graphicsoptions.mode.curvalue = 2;
			else if ( s_graphicsoptions.mode.curvalue > 6 )
				s_graphicsoptions.mode.curvalue = 6;
		}
		s_graphicsoptions.ratio.curvalue =
			resToRatio[ s_graphicsoptions.mode.curvalue ];
		break;

	case ID_LIST:
		ivo = &s_ivo_templates[s_graphicsoptions.list.curvalue];

		s_graphicsoptions.mode.curvalue        = GraphicsOptions_FindDetectedResolution(ivo->mode);
		s_graphicsoptions.ratio.curvalue =
			resToRatio[ s_graphicsoptions.mode.curvalue ];
		s_graphicsoptions.tq.curvalue          = ivo->tq;
		s_graphicsoptions.lighting.curvalue    = ivo->lighting;
		s_graphicsoptions.colordepth.curvalue  = ivo->colordepth;
		s_graphicsoptions.texturebits.curvalue = ivo->texturebits;
		s_graphicsoptions.geometry.curvalue    = ivo->geometry;
		s_graphicsoptions.filter.curvalue      = ivo->filter;
		s_graphicsoptions.anisotropy.curvalue  = ivo->anisotropy;
		s_graphicsoptions.msaa.curvalue        = ivo->msaa;
		s_graphicsoptions.fs.curvalue          = ivo->fullscreen;
		break;

	case ID_DRIVERINFO:
		UI_DriverInfo_Menu();
		break;

	case ID_BACK2:
		UI_PopMenu();
		break;

	case ID_ADVANCED_GRAPHICS:
		UI_AdvancedGraphicsOptionsMenu();
		break;
	}
}


/*
================
GraphicsOptions_TQEvent
================
*/
static void GraphicsOptions_TQEvent( void *ptr, int event ) {
	if( event != QM_ACTIVATED ) {
	 	return;
	}
	s_graphicsoptions.tq.curvalue = (int)(s_graphicsoptions.tq.curvalue + 0.5);
}


/*
================
GraphicsOptions_MenuDraw
================
*/
void GraphicsOptions_MenuDraw (void)
{
	GraphicsOptions_UpdateMenuItems();

	Frontend_DrawBackground( graphicsScrimColor );
	Frontend_DrawPanel( GRAPHICS_FRAME_X, GRAPHICS_FRAME_Y,
		GRAPHICS_FRAME_WIDTH, GRAPHICS_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( GRAPHICS_FRAME_X + 24, GRAPHICS_FRAME_Y + 24,
		"Graphics", UI_LEFT | UI_BIGFONT, graphicsTextColor );
	Frontend_DrawText( GRAPHICS_FRAME_X + 24, GRAPHICS_FRAME_Y + 48,
		"Tune display mode, quality and rendering",
		UI_LEFT | UI_SMALLFONT, graphicsMutedColor );
	Frontend_DrawStatusChip( GRAPHICS_FRAME_X + GRAPHICS_FRAME_WIDTH - 104,
		GRAPHICS_FRAME_Y + 26, "Settings", graphicsAccentColor, 1.0f );

	Frontend_DrawCard( GRAPHICS_NAV_X, GRAPHICS_NAV_Y,
		GRAPHICS_NAV_WIDTH, GRAPHICS_NAV_HEIGHT, 1.0f, qfalse );
	Frontend_DrawCard( GRAPHICS_DETAIL_X, GRAPHICS_DETAIL_Y,
		GRAPHICS_DETAIL_WIDTH, GRAPHICS_DETAIL_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( GRAPHICS_NAV_X + 16, GRAPHICS_NAV_Y + 22,
		"Submenu", UI_LEFT | UI_SMALLFONT, graphicsMutedColor );
	Frontend_DrawText( GRAPHICS_DETAIL_X + 16, GRAPHICS_DETAIL_Y + 22,
		"Display & quality", UI_LEFT | UI_SMALLFONT,
		graphicsMutedColor );

	Menu_Draw( &s_graphicsoptions.menu );

	Frontend_DrawText( GRAPHICS_FRAME_X + 24, GRAPHICS_FRAME_Y + 384,
		"Select an option   Left / right adjust   Esc back",
		UI_LEFT | UI_SMALLFONT, graphicsMutedColor );
}

/*
=================
GraphicsOptions_SetMenuItems
=================
*/
static void GraphicsOptions_SetMenuItems( void )
{
	s_graphicsoptions.mode.curvalue =
		GraphicsOptions_FindDetectedResolution( trap_Cvar_VariableValue( "r_mode" ) );

	if ( s_graphicsoptions.mode.curvalue < 0 )
	{
		if( resolutionsDetected )
		{
			int i;
			char buf[MAX_STRING_CHARS];
			trap_Cvar_VariableStringBuffer("r_customwidth", buf, sizeof(buf)-2);
			buf[strlen(buf)+1] = 0;
			buf[strlen(buf)] = 'x';
			trap_Cvar_VariableStringBuffer("r_customheight", buf+strlen(buf), sizeof(buf)-strlen(buf));

			for(i = 0; detectedResolutions[i]; ++i)
			{
				if(!Q_stricmp(buf, detectedResolutions[i]))
				{
					s_graphicsoptions.mode.curvalue = i;
					break;
				}
			}
			if ( s_graphicsoptions.mode.curvalue < 0 )
				s_graphicsoptions.mode.curvalue = 0;
		}
		else
		{
			s_graphicsoptions.mode.curvalue = 3;
		}
	}
	s_graphicsoptions.ratio.curvalue =
		resToRatio[ s_graphicsoptions.mode.curvalue ];
	s_graphicsoptions.fs.curvalue = trap_Cvar_VariableValue("r_fullscreen");
	s_graphicsoptions.allow_extensions.curvalue = trap_Cvar_VariableValue("r_allowExtensions");
	s_graphicsoptions.tq.curvalue = 3-trap_Cvar_VariableValue( "r_picmip");
	if ( s_graphicsoptions.tq.curvalue < 0 )
	{
		s_graphicsoptions.tq.curvalue = 0;
	}
	else if ( s_graphicsoptions.tq.curvalue > 3 )
	{
		s_graphicsoptions.tq.curvalue = 3;
	}

	s_graphicsoptions.lighting.curvalue = trap_Cvar_VariableValue( "r_vertexLight" ) != 0;
	switch ( ( int ) trap_Cvar_VariableValue( "r_texturebits" ) )
	{
	default:
	case 0:
		s_graphicsoptions.texturebits.curvalue = 0;
		break;
	case 16:
		s_graphicsoptions.texturebits.curvalue = 1;
		break;
	case 32:
		s_graphicsoptions.texturebits.curvalue = 2;
		break;
	}

	if ( !Q_stricmp( UI_Cvar_VariableString( "r_textureMode" ), "GL_LINEAR_MIPMAP_NEAREST" ) )
	{
		s_graphicsoptions.filter.curvalue = 0;
	}
	else
	{
		s_graphicsoptions.filter.curvalue = 1;
	}

	if ( trap_Cvar_VariableValue( "r_ext_texture_filter_anisotropic" ) <= 0 )
	{
		s_graphicsoptions.anisotropy.curvalue = 0;
	}
	else
	{
		int aniso = (int)trap_Cvar_VariableValue( "r_ext_max_anisotropy" );
		if ( aniso >= 16 )
			s_graphicsoptions.anisotropy.curvalue = 4;
		else if ( aniso >= 8 )
			s_graphicsoptions.anisotropy.curvalue = 3;
		else if ( aniso >= 4 )
			s_graphicsoptions.anisotropy.curvalue = 2;
		else if ( aniso >= 2 )
			s_graphicsoptions.anisotropy.curvalue = 1;
		else
			s_graphicsoptions.anisotropy.curvalue = 0;
	}

	{
		int msaa = (int)trap_Cvar_VariableValue( "r_ext_multisample" );
		if ( msaa >= 4 )
			s_graphicsoptions.msaa.curvalue = 2;
		else if ( msaa >= 2 )
			s_graphicsoptions.msaa.curvalue = 1;
		else
			s_graphicsoptions.msaa.curvalue = 0;
	}

	if ( trap_Cvar_VariableValue( "r_lodBias" ) > 0 )
	{
		if ( trap_Cvar_VariableValue( "r_subdivisions" ) >= 20 )
		{
			s_graphicsoptions.geometry.curvalue = 0;
		}
		else
		{
			s_graphicsoptions.geometry.curvalue = 1;
		}
	}
	else
	{
		s_graphicsoptions.geometry.curvalue = 2;
	}

	switch ( ( int ) trap_Cvar_VariableValue( "r_colorbits" ) )
	{
	default:
	case 0:
		s_graphicsoptions.colordepth.curvalue = 0;
		break;
	case 16:
		s_graphicsoptions.colordepth.curvalue = 1;
		break;
	case 32:
		s_graphicsoptions.colordepth.curvalue = 2;
		break;
	}

	if ( s_graphicsoptions.fs.curvalue == 0 )
	{
		s_graphicsoptions.colordepth.curvalue = 0;
	}
	if ( s_graphicsoptions.driver.curvalue == 1 )
	{
		s_graphicsoptions.colordepth.curvalue = 1;
	}
}

static void GraphicsOptions_SetBounds( menucommon_s *item, int id,
	int x, int y, int width, int height, const char *label )
{
	item->id = id;
	item->x = x;
	item->y = y;
	item->left = x;
	item->top = y;
	item->right = x + width;
	item->bottom = y + height;
	if ( label ) {
		item->name = (char *)label;
	}
}

static void GraphicsOptions_SetChildActionBounds( menutext_s *item,
	int id, const char *label, int y )
{
	item->generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
	item->generic.callback = GraphicsOptions_Event;
	item->string = (char *)label;
	item->style = UI_CENTER | UI_SMALLFONT;
	item->generic.ownerdraw = GraphicsOptions_DrawAction;
	GraphicsOptions_SetBounds( &item->generic, id, GRAPHICS_NAV_X + 16, y,
		GRAPHICS_NAV_WIDTH - 32, GRAPHICS_ROW_HEIGHT, NULL );
}

static void GraphicsOptions_SetSettingBounds( menucommon_s *item, int id,
	const char *label, int x, int y )
{
	item->ownerdraw = GraphicsOptions_DrawSetting;
	GraphicsOptions_SetBounds( item, id, x, y, GRAPHICS_COLUMN_WIDTH,
		GRAPHICS_ROW_HEIGHT, label );
}

/*
================
GraphicsOptions_MenuInit
================
*/
void GraphicsOptions_MenuInit( void )
{
	static const char *s_driver_names[] =
	{
		"Default",
		"Voodoo",
		NULL
	};

	static const char *tq_names[] =
	{
		"Default",
		"16 bit",
		"32 bit",
		NULL
	};

	static const char *s_graphics_options_names[] =
	{
		"Very High Quality",
		"High Quality",
		"Normal",
		"Fast",
		"Fastest",
		"Custom",
		NULL
	};

	static const char *lighting_names[] =
	{
		"Lightmap",
		"Vertex",
		NULL
	};

	static const char *colordepth_names[] =
	{
		"Default",
		"16 bit",
		"32 bit",
		NULL
	};

	static const char *filter_names[] =
	{
		"Bilinear",
		"Trilinear",
		NULL
	};

	static const char *anisotropy_names[] =
	{
		"Off",
		"2x",
		"4x",
		"8x",
		"16x",
		NULL
	};

	static const char *msaa_names[] =
	{
		"Off",
		"2x",
		"4x",
		NULL
	};
	static const char *quality_names[] =
	{
		"Low",
		"Medium",
		"High",
		NULL
	};
	static const char *enabled_names[] =
	{
		"Off",
		"On",
		NULL
	};
	int y;

	// zero set all our globals
	memset( &s_graphicsoptions, 0 ,sizeof(graphicsoptions_t) );

	GraphicsOptions_GetResolutions();
	GraphicsOptions_GetAspectRatios();
	
	GraphicsOptions_Cache();

	s_graphicsoptions.menu.wrapAround = qtrue;
	s_graphicsoptions.menu.fullscreen = qtrue;
	s_graphicsoptions.menu.draw       = GraphicsOptions_MenuDraw;

	s_graphicsoptions.banner.generic.type  = MTYPE_BTEXT;
	s_graphicsoptions.banner.generic.x	   = 320;
	s_graphicsoptions.banner.generic.y	   = 16;
	s_graphicsoptions.banner.string  	   = "SYSTEM SETUP";
	s_graphicsoptions.banner.color         = color_white;
	s_graphicsoptions.banner.style         = UI_CENTER;

// STONELANCE
/*
	s_graphicsoptions.framel.generic.type  = MTYPE_BITMAP;
	s_graphicsoptions.framel.generic.name  = GRAPHICSOPTIONS_FRAMEL;
	s_graphicsoptions.framel.generic.flags = QMF_INACTIVE;
	s_graphicsoptions.framel.generic.x	   = 0;
	s_graphicsoptions.framel.generic.y	   = 78;
	s_graphicsoptions.framel.width  	   = 256;
	s_graphicsoptions.framel.height  	   = 329;

	s_graphicsoptions.framer.generic.type  = MTYPE_BITMAP;
	s_graphicsoptions.framer.generic.name  = GRAPHICSOPTIONS_FRAMER;
	s_graphicsoptions.framer.generic.flags = QMF_INACTIVE;
	s_graphicsoptions.framer.generic.x	   = 376;
	s_graphicsoptions.framer.generic.y	   = 76;
	s_graphicsoptions.framer.width  	   = 256;
	s_graphicsoptions.framer.height  	   = 334;
*/
// END

	s_graphicsoptions.graphics.generic.type		= MTYPE_PTEXT;
	s_graphicsoptions.graphics.generic.flags	= QMF_RIGHT_JUSTIFY;
	s_graphicsoptions.graphics.generic.id		= ID_GRAPHICS;
	s_graphicsoptions.graphics.generic.callback	= GraphicsOptions_Event;
	s_graphicsoptions.graphics.generic.x		= 216;
	s_graphicsoptions.graphics.generic.y		= 240 - 2 * PROP_HEIGHT;
	s_graphicsoptions.graphics.string			= "GRAPHICS";
	s_graphicsoptions.graphics.style			= UI_RIGHT;
// BAGPUSS
//	s_graphicsoptions.graphics.color			= color_red;
	s_graphicsoptions.graphics.color			= text_color_normal;
// END


	s_graphicsoptions.advanced_graphics.generic.type		= MTYPE_PTEXT;
	s_graphicsoptions.advanced_graphics.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.advanced_graphics.generic.id		= ID_ADVANCED_GRAPHICS;
	s_graphicsoptions.advanced_graphics.generic.callback	= GraphicsOptions_Event;
	s_graphicsoptions.advanced_graphics.generic.x			= 216;
	s_graphicsoptions.advanced_graphics.generic.y			= 240 - PROP_HEIGHT;
	s_graphicsoptions.advanced_graphics.string			= "ADVANCED GRAPHICS";
	s_graphicsoptions.advanced_graphics.style			= UI_RIGHT;
	s_graphicsoptions.advanced_graphics.color			= text_color_normal;

	s_graphicsoptions.display.generic.type		= MTYPE_PTEXT;
	s_graphicsoptions.display.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.display.generic.id		= ID_DISPLAY;
	s_graphicsoptions.display.generic.callback	= GraphicsOptions_Event;
	s_graphicsoptions.display.generic.x			= 216;
	s_graphicsoptions.display.generic.y			= 240;
	s_graphicsoptions.display.string			= "DISPLAY";
	s_graphicsoptions.display.style				= UI_RIGHT;
// BAGPUSS
//	s_graphicsoptions.display.color				= color_red;
	s_graphicsoptions.display.color				= text_color_normal;
// END

	s_graphicsoptions.sound.generic.type		= MTYPE_PTEXT;
	s_graphicsoptions.sound.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.sound.generic.id			= ID_SOUND;
	s_graphicsoptions.sound.generic.callback	= GraphicsOptions_Event;
	s_graphicsoptions.sound.generic.x			= 216;
	s_graphicsoptions.sound.generic.y			= 240 + PROP_HEIGHT;
	s_graphicsoptions.sound.string				= "SOUND";
	s_graphicsoptions.sound.style				= UI_RIGHT;
// BAGPUSS
//	s_graphicsoptions.sound.color				= color_red;
	s_graphicsoptions.sound.color				= text_color_normal;
// END

	s_graphicsoptions.network.generic.type		= MTYPE_PTEXT;
	s_graphicsoptions.network.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.network.generic.id		= ID_NETWORK;
	s_graphicsoptions.network.generic.callback	= GraphicsOptions_Event;
	s_graphicsoptions.network.generic.x			= 216;
	s_graphicsoptions.network.generic.y			= 240 + 2 * PROP_HEIGHT;
	s_graphicsoptions.network.string			= "NETWORK";
	s_graphicsoptions.network.style				= UI_RIGHT;
// BAGPUSS
//	s_graphicsoptions.network.color				= color_red;
	s_graphicsoptions.network.color				= text_color_normal;
// END

	y = 240 - 7 * (BIGCHAR_HEIGHT + 2);
	s_graphicsoptions.list.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.list.generic.name     = "Graphics Settings:";
	s_graphicsoptions.list.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.list.generic.x        = 400;
	s_graphicsoptions.list.generic.y        = y;
	s_graphicsoptions.list.generic.callback = GraphicsOptions_Event;
	s_graphicsoptions.list.generic.id       = ID_LIST;
	s_graphicsoptions.list.itemnames        = s_graphics_options_names;
	y += 2 * ( BIGCHAR_HEIGHT + 2 );

	s_graphicsoptions.driver.generic.type  = MTYPE_SPINCONTROL;
	s_graphicsoptions.driver.generic.name  = "GL Driver:";
	s_graphicsoptions.driver.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.driver.generic.x     = 400;
	s_graphicsoptions.driver.generic.y     = y;
	s_graphicsoptions.driver.itemnames     = s_driver_names;
	s_graphicsoptions.driver.curvalue      = (uis.glconfig.driverType == GLDRV_VOODOO);
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_allowExtensions"
	s_graphicsoptions.allow_extensions.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.allow_extensions.generic.name	    = "GL Extensions:";
	s_graphicsoptions.allow_extensions.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.allow_extensions.generic.x	    = 400;
	s_graphicsoptions.allow_extensions.generic.y	    = y;
	s_graphicsoptions.allow_extensions.itemnames        = enabled_names;
	y += BIGCHAR_HEIGHT+2;

	s_graphicsoptions.ratio.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.ratio.generic.name     = "Aspect Ratio:";
	s_graphicsoptions.ratio.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.ratio.generic.x        = 400;
	s_graphicsoptions.ratio.generic.y        = y;
	s_graphicsoptions.ratio.itemnames        = ratios;
	s_graphicsoptions.ratio.generic.callback = GraphicsOptions_Event;
	s_graphicsoptions.ratio.generic.id       = ID_RATIO;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_mode"
	s_graphicsoptions.mode.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.mode.generic.name     = "Resolution:";
	s_graphicsoptions.mode.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.mode.generic.x        = 400;
	s_graphicsoptions.mode.generic.y        = y;
	s_graphicsoptions.mode.itemnames        = resolutions;
	s_graphicsoptions.mode.generic.callback = GraphicsOptions_Event;
	s_graphicsoptions.mode.generic.id       = ID_MODE;
	y += BIGCHAR_HEIGHT+2;

	// references "r_colorbits"
	s_graphicsoptions.colordepth.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.colordepth.generic.name     = "Color Depth:";
	s_graphicsoptions.colordepth.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.colordepth.generic.x        = 400;
	s_graphicsoptions.colordepth.generic.y        = y;
	s_graphicsoptions.colordepth.itemnames        = colordepth_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_fullscreen"
	s_graphicsoptions.fs.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.fs.generic.name	  = "Fullscreen:";
	s_graphicsoptions.fs.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.fs.generic.x	      = 400;
	s_graphicsoptions.fs.generic.y	      = y;
	s_graphicsoptions.fs.itemnames	      = enabled_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_vertexLight"
	s_graphicsoptions.lighting.generic.type  = MTYPE_SPINCONTROL;
	s_graphicsoptions.lighting.generic.name	 = "Lighting:";
	s_graphicsoptions.lighting.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.lighting.generic.x	 = 400;
	s_graphicsoptions.lighting.generic.y	 = y;
	s_graphicsoptions.lighting.itemnames     = lighting_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_lodBias" & "subdivisions"
	s_graphicsoptions.geometry.generic.type  = MTYPE_SPINCONTROL;
	s_graphicsoptions.geometry.generic.name	 = "Geometric Detail:";
	s_graphicsoptions.geometry.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.geometry.generic.x	 = 400;
	s_graphicsoptions.geometry.generic.y	 = y;
	s_graphicsoptions.geometry.itemnames     = quality_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_picmip"
	s_graphicsoptions.tq.generic.type	= MTYPE_SLIDER;
	s_graphicsoptions.tq.generic.name	= "Texture Detail:";
	s_graphicsoptions.tq.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.tq.generic.x		= 400;
	s_graphicsoptions.tq.generic.y		= y;
	s_graphicsoptions.tq.minvalue       = 0;
	s_graphicsoptions.tq.maxvalue       = 3;
	s_graphicsoptions.tq.generic.callback = GraphicsOptions_TQEvent;
	s_graphicsoptions.tq.generic.ownerdraw = UI_RallySlider_Draw;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_textureBits"
	s_graphicsoptions.texturebits.generic.type  = MTYPE_SPINCONTROL;
	s_graphicsoptions.texturebits.generic.name	= "Texture Quality:";
	s_graphicsoptions.texturebits.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.texturebits.generic.x	    = 400;
	s_graphicsoptions.texturebits.generic.y	    = y;
	s_graphicsoptions.texturebits.itemnames     = tq_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_textureMode"
	s_graphicsoptions.filter.generic.type   = MTYPE_SPINCONTROL;
	s_graphicsoptions.filter.generic.name	= "Texture Filter:";
	s_graphicsoptions.filter.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.filter.generic.x	    = 400;
	s_graphicsoptions.filter.generic.y	    = y;
	s_graphicsoptions.filter.itemnames      = filter_names;
	y += BIGCHAR_HEIGHT+2;

	s_graphicsoptions.anisotropy.generic.type   = MTYPE_SPINCONTROL;
	s_graphicsoptions.anisotropy.generic.name	 = "Anisotropic:";
	s_graphicsoptions.anisotropy.generic.flags	 = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.anisotropy.generic.x	    = 400;
	s_graphicsoptions.anisotropy.generic.y	    = y;
	s_graphicsoptions.anisotropy.generic.id      = ID_ANISOTROPY;
	s_graphicsoptions.anisotropy.generic.callback= GraphicsOptions_Event;
	s_graphicsoptions.anisotropy.itemnames       = anisotropy_names;
	y += BIGCHAR_HEIGHT+2;

	s_graphicsoptions.msaa.generic.type		 = MTYPE_SPINCONTROL;
	s_graphicsoptions.msaa.generic.name		 = "MSAA:";
	s_graphicsoptions.msaa.generic.flags		 = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.msaa.generic.x		 = 400;
	s_graphicsoptions.msaa.generic.y		 = y;
	s_graphicsoptions.msaa.generic.id		 = ID_MSAA;
	s_graphicsoptions.msaa.generic.callback	 = GraphicsOptions_Event;
	s_graphicsoptions.msaa.itemnames		 = msaa_names;
	y += BIGCHAR_HEIGHT+2;

	y += BIGCHAR_HEIGHT + 8;

	s_graphicsoptions.driverinfo.generic.type     = MTYPE_PTEXT;
	s_graphicsoptions.driverinfo.generic.flags    = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.driverinfo.generic.callback = GraphicsOptions_Event;
	s_graphicsoptions.driverinfo.generic.id       = ID_DRIVERINFO;
	s_graphicsoptions.driverinfo.generic.x        = 320;
	s_graphicsoptions.driverinfo.generic.y        = y;
	s_graphicsoptions.driverinfo.string           = "Driver Info";
	s_graphicsoptions.driverinfo.style            = UI_CENTER|UI_SMALLFONT;
// BAGPUSS
//	s_graphicsoptions.driverinfo.color            = color_red;
	s_graphicsoptions.driverinfo.color            = text_color_normal;
// END
	y += BIGCHAR_HEIGHT+2;

// STONELANCE
/*
	s_graphicsoptions.back.generic.type	    = MTYPE_BITMAP;
	s_graphicsoptions.back.generic.name     = GRAPHICSOPTIONS_BACK0;
	s_graphicsoptions.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.back.generic.callback = GraphicsOptions_Event;
	s_graphicsoptions.back.generic.id	    = ID_BACK2;
	s_graphicsoptions.back.generic.x		= 0;
	s_graphicsoptions.back.generic.y		= 480-64;
	s_graphicsoptions.back.width  		    = 128;
	s_graphicsoptions.back.height  		    = 64;
	s_graphicsoptions.back.focuspic         = GRAPHICSOPTIONS_BACK1;

	s_graphicsoptions.apply.generic.type     = MTYPE_BITMAP;
	s_graphicsoptions.apply.generic.name     = GRAPHICSOPTIONS_ACCEPT0;
	s_graphicsoptions.apply.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_HIDDEN|QMF_INACTIVE;
	s_graphicsoptions.apply.generic.callback = GraphicsOptions_ApplyChanges;
	s_graphicsoptions.apply.generic.x        = 640;
	s_graphicsoptions.apply.generic.y        = 480-64;
	s_graphicsoptions.apply.width  		     = 128;
	s_graphicsoptions.apply.height  		 = 64;
	s_graphicsoptions.apply.focuspic         = GRAPHICSOPTIONS_ACCEPT1;
*/
	s_graphicsoptions.back.generic.type				= MTYPE_PTEXT;
	s_graphicsoptions.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.back.generic.x				= 20;
	s_graphicsoptions.back.generic.y				= 480 - 50;
	s_graphicsoptions.back.generic.id				= ID_BACK2;
	s_graphicsoptions.back.generic.callback			= GraphicsOptions_Event; 
	s_graphicsoptions.back.string					= "< BACK";
	s_graphicsoptions.back.color					= text_color_normal;
	s_graphicsoptions.back.style					= UI_LEFT | UI_SMALLFONT;

	s_graphicsoptions.apply.generic.type			= MTYPE_PTEXT;
	s_graphicsoptions.apply.generic.flags			= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.apply.generic.x				= 640 - 20;
	s_graphicsoptions.apply.generic.y				= 480 - 50;
	s_graphicsoptions.apply.generic.callback		= GraphicsOptions_ApplyChanges; 
	s_graphicsoptions.apply.string					= "APPLY";
	s_graphicsoptions.apply.color					= text_color_normal;
	s_graphicsoptions.apply.style					= UI_RIGHT | UI_SMALLFONT;
// END

// STONELANCE
/*
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.framel );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.framer );
*/
// END

	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.advanced_graphics );

	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.list );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.driver );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.allow_extensions );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.ratio );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.mode );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.colordepth );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.fs );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.lighting );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.geometry );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.tq );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.texturebits );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.filter );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.anisotropy );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.msaa );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.driverinfo );

	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.back );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.apply );

	/* Menu_AddItem initializes the legacy widgets and overwrites their
	 * default bounds. Apply the frontend layout after that initialization so
	 * the custom ownerdraw positions are the ones used for drawing and input. */
	GraphicsOptions_SetChildActionBounds( &s_graphicsoptions.advanced_graphics,
		ID_ADVANCED_GRAPHICS, "Advanced graphics", 152 );
	GraphicsOptions_SetChildActionBounds( &s_graphicsoptions.driverinfo,
		ID_DRIVERINFO, "Driver info", 184 );

	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.list.generic,
		ID_LIST, "Preset", GRAPHICS_COLUMN_LEFT_X, GRAPHICS_ROW_START_Y );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.driver.generic,
		ID_DRIVER, "Renderer", GRAPHICS_COLUMN_LEFT_X,
		GRAPHICS_ROW_START_Y + GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.ratio.generic,
		ID_RATIO, "Aspect ratio", GRAPHICS_COLUMN_LEFT_X,
		GRAPHICS_ROW_START_Y + 2 * ( GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP ) );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.mode.generic,
		ID_MODE, "Resolution", GRAPHICS_COLUMN_LEFT_X,
		GRAPHICS_ROW_START_Y + 3 * ( GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP ) );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.colordepth.generic,
		ID_COLORDEPTH, "Color depth", GRAPHICS_COLUMN_LEFT_X,
		GRAPHICS_ROW_START_Y + 4 * ( GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP ) );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.fs.generic,
		ID_FULLSCREEN, "Fullscreen", GRAPHICS_COLUMN_LEFT_X,
		GRAPHICS_ROW_START_Y + 5 * ( GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP ) );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.msaa.generic,
		ID_MSAA, "MSAA", GRAPHICS_COLUMN_LEFT_X,
		GRAPHICS_ROW_START_Y + 6 * ( GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP ) );

	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.allow_extensions.generic,
		ID_EXTENSIONS, "Extensions", GRAPHICS_COLUMN_RIGHT_X,
		GRAPHICS_ROW_START_Y );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.lighting.generic,
		ID_LIGHTING, "Lighting", GRAPHICS_COLUMN_RIGHT_X,
		GRAPHICS_ROW_START_Y + GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.geometry.generic,
		ID_GEOMETRY, "Geometry", GRAPHICS_COLUMN_RIGHT_X,
		GRAPHICS_ROW_START_Y + 2 * ( GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP ) );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.tq.generic,
		ID_TEXTUREDETAIL, "Texture detail", GRAPHICS_COLUMN_RIGHT_X,
		GRAPHICS_ROW_START_Y + 3 * ( GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP ) );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.texturebits.generic,
		ID_TEXTUREQUALITY, "Tex. quality", GRAPHICS_COLUMN_RIGHT_X,
		GRAPHICS_ROW_START_Y + 4 * ( GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP ) );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.filter.generic,
		ID_FILTER, "Tex. filter", GRAPHICS_COLUMN_RIGHT_X,
		GRAPHICS_ROW_START_Y + 5 * ( GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP ) );
	GraphicsOptions_SetSettingBounds( &s_graphicsoptions.anisotropy.generic,
		ID_ANISOTROPY, "Anisotropic", GRAPHICS_COLUMN_RIGHT_X,
		GRAPHICS_ROW_START_Y + 6 * ( GRAPHICS_ROW_HEIGHT + GRAPHICS_ROW_GAP ) );

	s_graphicsoptions.back.generic.ownerdraw = GraphicsOptions_DrawAction;
	GraphicsOptions_SetBounds( &s_graphicsoptions.back.generic, ID_BACK2,
		GRAPHICS_NAV_X, GRAPHICS_ACTION_Y, GRAPHICS_ACTION_WIDTH,
		GRAPHICS_ACTION_HEIGHT, NULL );
	s_graphicsoptions.apply.generic.ownerdraw = GraphicsOptions_DrawAction;
	GraphicsOptions_SetBounds( &s_graphicsoptions.apply.generic, ID_APPLY,
		GRAPHICS_FRAME_X + GRAPHICS_FRAME_WIDTH - GRAPHICS_ACTION_WIDTH,
		GRAPHICS_ACTION_Y, GRAPHICS_ACTION_WIDTH, GRAPHICS_ACTION_HEIGHT, NULL );

	GraphicsOptions_SetMenuItems();
	GraphicsOptions_GetInitialVideo();

	if ( uis.glconfig.driverType == GLDRV_ICD &&
		 uis.glconfig.hardwareType == GLHW_3DFX_2D3D )
	{
		s_graphicsoptions.driver.generic.flags |= QMF_HIDDEN|QMF_INACTIVE;
	}
}


/*
=================
GraphicsOptions_Cache
=================
*/
void GraphicsOptions_Cache( void ) {
// STONELANCE
/*
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_FRAMEL );
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_FRAMER );
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_BACK0 );
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_BACK1 );
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_ACCEPT0 );
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_ACCEPT1 );
*/
// END
}


/*
=================
UI_GraphicsOptionsMenu
=================
*/
void UI_GraphicsOptionsMenu( void ) {
// STONELANCE FIXME: get rid of this after proper tansitions are added
	uis.transitionIn = 0;
// END

	GraphicsOptions_MenuInit();
	UI_PushMenu( &s_graphicsoptions.menu );
	Menu_SetCursorToItem( &s_graphicsoptions.menu, &s_graphicsoptions.list );
}
