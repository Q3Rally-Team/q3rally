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
#define DRIVERINFO_ACTION_WIDTH	UI_FRONTEND_ACTION_WIDTH
#define DRIVERINFO_ACTION_HEIGHT	UI_FRONTEND_BUTTON_HEIGHT

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

Q3Rally: first of the three graphics pages (see ui_advanced_graphics.c).
Preset, renderer, display mode and texture quality.

=======================================================================
*/

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

	if( mode < 0 || mode >= ARRAY_LEN( builtinResolutions ) - 1 )
		return -1;

	for( i = 0; detectedResolutions[ i ]; i++ )
	{
		if( !Q_stricmp( builtinResolutions[ mode ], detectedResolutions[ i ] ) )
			return i;
	}

	return -1;
}

static void GraphicsOptions_ParseResolution( const char *res, int *w, int *h )
{
	const char *x = strchr( res, 'x' );

	*w = atoi( res );
	*h = x ? atoi( x + 1 ) : 0;
}

/*
=================
GraphicsOptions_GetAspectRatios

Builds the list of distinct aspect ratios and which ratio each resolution has.
=================
*/
static void GraphicsOptions_GetAspectRatios( void )
{
	int i, r, numRatios = 0;

	memset( ratioBuf, 0, sizeof( ratioBuf ) );

	for( r = 0; resolutions[r] && r < MAX_RESOLUTIONS; r++ )
	{
		int w, h;
		char str[ sizeof(ratioBuf[0]) ];

		GraphicsOptions_ParseResolution( resolutions[r], &w, &h );
		if ( h <= 0 )
			h = 1;
		Com_sprintf( str, sizeof(str), "%.2f:1", (float)w / (float)h );

		// rename common ratios ("1.33:1" -> "4:3")
		for( i = 0; knownRatios[i][0]; i++ ) {
			if( !Q_stricmp( str, knownRatios[i][0] ) ) {
				Q_strncpyz( str, knownRatios[i][1], sizeof( str ) );
				break;
			}
		}

		for( i = 0; i < numRatios; i++ )
		{
			if( !Q_stricmp( str, ratioBuf[i] ) )
				break;
		}
		if( i == numRatios && numRatios < MAX_RESOLUTIONS - 1 )
		{
			Q_strncpyz( ratioBuf[i], str, sizeof(ratioBuf[i]) );
			ratios[i] = ratioBuf[i];
			numRatios++;
		}

		resToRatio[r] = i;
	}

	ratios[numRatios] = NULL;
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
GraphicsOptions_ReadResolution

Index into resolutions[] of the mode the cvars ask for.
=================
*/
static int GraphicsOptions_ReadResolution( void )
{
	int mode = GraphicsOptions_FindDetectedResolution( UI_GfxCvarValue( "r_mode" ) );

	if ( mode < 0 )
	{
		if( resolutionsDetected )
		{
			int i;
			char buf[64];

			Com_sprintf( buf, sizeof( buf ), "%dx%d",
				(int)UI_GfxCvarValue( "r_customwidth" ),
				(int)UI_GfxCvarValue( "r_customheight" ) );

			for( i = 0; detectedResolutions[i]; ++i )
			{
				if( !Q_stricmp( buf, detectedResolutions[i] ) )
					return i;
			}
			return 0;
		}
		return 3;
	}
	return mode;
}

static void GraphicsOptions_ApplyResolution( int choice )
{
	if( resolutionsDetected )
	{
		// search for builtin mode that matches the detected mode
		int mode;

		if ( choice < 0 || choice >= ARRAY_LEN( detectedResolutions ) || !detectedResolutions[ choice ] )
			choice = 0;

		mode = GraphicsOptions_FindBuiltinResolution( choice );
		if( mode == -1 )
		{
			int w, h;

			GraphicsOptions_ParseResolution( detectedResolutions[ choice ], &w, &h );
			UI_GfxSetCvarValue( "r_customwidth", w );
			UI_GfxSetCvarValue( "r_customheight", h );
		}

		UI_GfxSetCvarValue( "r_mode", mode );
	}
	else
		UI_GfxSetCvarValue( "r_mode", choice );
}

static int GraphicsOptions_ReadRatio( void )
{
	return resToRatio[ GraphicsOptions_ReadResolution() ];
}

// switching the aspect ratio picks the biggest resolution with that ratio
static void GraphicsOptions_ApplyRatio( int choice )
{
	int r, best = -1, bestArea = 0;

	for ( r = 0; resolutions[r] && r < MAX_RESOLUTIONS; r++ )
	{
		int w, h;

		if ( resToRatio[r] != choice )
			continue;
		GraphicsOptions_ParseResolution( resolutions[r], &w, &h );
		if ( w * h > bestArea )
		{
			bestArea = w * h;
			best = r;
		}
	}

	if ( best >= 0 )
		GraphicsOptions_ApplyResolution( best );
}

static int GraphicsOptions_ReadPreset( void )
{
	int level = UI_GfxPreset_Detect();

	return ( level >= GFX_NUM_PRESETS ) ? -1 : level;
}

void UI_DriverInfoMenu( void )
{
	UI_DriverInfo_Menu();
}

static const char *renderer_names[] = { "OpenGL 2", "OpenGL 1", NULL };
static const char *renderer_sets[] = { "cl_renderer=opengl2", "cl_renderer=opengl1" };
static const char *fullscreen_names[] = { "Off", "On", NULL };
static const char *fullscreen_sets[] = { "r_fullscreen=0", "r_fullscreen=1" };
static const char *aa_names[] = { "Off", "FXAA", "2x + FXAA", "4x + FXAA", "8x + FXAA", NULL };
static const int aa_samples[] = { 0, 0, 2, 4, 8 };

static int GraphicsOptions_ReadAA( void )
{
	int i, samples = UI_GfxMsaa_Read();
	qboolean fxaa = UI_GfxCvarValue( "r_fxaa" ) != 0;

	if ( !fxaa )
		return samples ? -1 : 0;

	for ( i = 1; i < ARRAY_LEN( aa_samples ); i++ )
	{
		if ( aa_samples[i] == samples )
			return i;
	}
	return -1;
}

static void GraphicsOptions_ApplyAA( int choice )
{
	if ( choice < 0 || choice >= ARRAY_LEN( aa_samples ) )
		choice = 0;
	UI_GfxSetCvarValue( "r_fxaa", choice ? 1 : 0 );
	UI_GfxMsaa_Set( aa_samples[choice] );
}

static const char *texdetail_names[] = { "Low", "Medium", "High", NULL };
static const char *texdetail_sets[] = { "r_picmip=2", "r_picmip=1", "r_picmip=0" };
static const char *filter_names[] = { "Bilinear", "Trilinear", "Aniso 4x", "Aniso 8x", "Aniso 16x", NULL };
static const char *filter_sets[] = {
	"r_textureMode=GL_LINEAR_MIPMAP_NEAREST;r_ext_texture_filter_anisotropic=0",
	"r_textureMode=GL_LINEAR_MIPMAP_LINEAR;r_ext_texture_filter_anisotropic=0",
	"r_textureMode=GL_LINEAR_MIPMAP_LINEAR;r_ext_texture_filter_anisotropic=1;r_ext_max_anisotropy=4",
	"r_textureMode=GL_LINEAR_MIPMAP_LINEAR;r_ext_texture_filter_anisotropic=1;r_ext_max_anisotropy=8",
	"r_textureMode=GL_LINEAR_MIPMAP_LINEAR;r_ext_texture_filter_anisotropic=1;r_ext_max_anisotropy=16"
};
static const char *geometry_names[] = { "Low", "Medium", "High", NULL };
static const char *geometry_sets[] = {
	"r_lodBias=2;r_subdivisions=20", "r_lodBias=1;r_subdivisions=12", "r_lodBias=0;r_subdivisions=4"
};

enum {
	GRAPHICSROW_PRESET,
	GRAPHICSROW_RENDERER,
	GRAPHICSROW_RESOLUTION,
	GRAPHICSROW_RATIO,
	GRAPHICSROW_FULLSCREEN,
	GRAPHICSROW_AA,
	GRAPHICSROW_TEXDETAIL,
	GRAPHICSROW_FILTER,
	GRAPHICSROW_GEOMETRY,
	NUM_GRAPHICSROWS
};

// not const: the resolution lists are only known at run time
static gfxRow_t graphicsRows[NUM_GRAPHICSROWS] = {
	{ "Preset", gfx_preset_names, NULL, 0, GraphicsOptions_ReadPreset, UI_GfxPreset_Apply },
	{ "Renderer", renderer_names, renderer_sets, GFXROW_RESTART },
	{ "Resolution", NULL, NULL, GFXROW_RESTART, GraphicsOptions_ReadResolution, GraphicsOptions_ApplyResolution },
	{ "Aspect ratio", NULL, NULL, GFXROW_RESTART, GraphicsOptions_ReadRatio, GraphicsOptions_ApplyRatio },
	{ "Fullscreen", fullscreen_names, fullscreen_sets, GFXROW_RESTART },
	{ "Anti-aliasing", aa_names, NULL, GFXROW_RESTART, GraphicsOptions_ReadAA, GraphicsOptions_ApplyAA },
	{ "Texture detail", texdetail_names, texdetail_sets, GFXROW_RESTART },
	{ "Texture filter", filter_names, filter_sets, GFXROW_RESTART },
	{ "Geometry", geometry_names, geometry_sets, GFXROW_RESTART }
};

static const gfxPageDef_t graphicsPage = {
	"Graphics", "Quality preset, display mode and textures", "Display & quality",
	graphicsRows, NUM_GRAPHICSROWS, 5, 0, qfalse
};


/*
=================
GraphicsOptions_Cache
=================
*/
void GraphicsOptions_Cache( void ) {
}


/*
=================
UI_GraphicsOptionsMenu
=================
*/
void UI_GraphicsOptionsMenu( void ) {
	GraphicsOptions_GetResolutions();
	GraphicsOptions_GetAspectRatios();

	graphicsRows[GRAPHICSROW_RESOLUTION].names = resolutions;
	graphicsRows[GRAPHICSROW_RATIO].names = ratios;

	UI_GfxPage_Push( &graphicsPage );
}
