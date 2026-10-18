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
/*
=======================================================================

GRAPHICS SETTINGS PAGES

Q3Rally: the three graphics pages (Graphics in ui_video.c, Effects and
Lighting & shadows here) are built from tables. A row lists its choices;
each choice sets some cvars ("cvar=value;cvar=value"), and the row shows the
first choice whose cvars all match (else "Custom"). Rows with a read/apply
function do their own thing (preset, resolution). Changes take effect at
once; when a changed cvar needs a renderer restart, the pages show "Apply"
(vid_restart). One preset (Low ... Ultra) sets everything.

=======================================================================
*/

#include "ui_local.h"
#include "ui_rally_frontend.h"

#define GFX_FRAME_X				24
#define GFX_FRAME_Y				20
#define GFX_FRAME_WIDTH			592
#define GFX_FRAME_HEIGHT		440
#define GFX_NAV_X				40
#define GFX_NAV_Y				104
#define GFX_NAV_WIDTH			164
#define GFX_NAV_HEIGHT			292
#define GFX_DETAIL_X			220
#define GFX_DETAIL_Y			104
#define GFX_DETAIL_WIDTH		376
#define GFX_DETAIL_HEIGHT		292
#define GFX_ROW_HEIGHT			24
#define GFX_ROW_GAP				4
#define GFX_ROW_START_Y			148
#define GFX_COLUMN_WIDTH		176
#define GFX_COLUMN_LEFT_X		236
#define GFX_COLUMN_RIGHT_X		420
#define GFX_VALUE_OFFSET		100
#define GFX_ACTION_Y			420
#define GFX_ACTION_WIDTH		UI_FRONTEND_ACTION_WIDTH
#define GFX_ACTION_HEIGHT		UI_FRONTEND_BUTTON_HEIGHT

#define GFX_MAX_PAGES			3
#define GFX_MAX_ROWS			16

#define ID_GFX_ROW				100
#define ID_GFX_NAV				200
#define ID_GFX_BACK				300
#define ID_GFX_APPLY			301

static vec4_t gfxPageScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t gfxPageAccentColor = UI_FRONTEND_COLOR_ACCENT;
static vec4_t gfxPageTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t gfxPageMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t gfxPageBorderColor = UI_FRONTEND_COLOR_BORDER;
static vec4_t gfxPageFocusColor = UI_FRONTEND_COLOR_FOCUS_BG;

typedef struct {
	menuframework_s		menu;		// first, menu items point back to it
	const gfxPageDef_t	*def;
	menulist_s			rows[GFX_MAX_ROWS];
	menutext_s			nav[4];		// gfxNav
	menutext_s			back;
	menutext_s			apply;
} gfxPage_t;

static void UI_GfxEffects_Open( void );
static void UI_GfxLighting_Open( void );

typedef struct {
	const char	*label;
	void		(*open)( void );
	qboolean	replace;		// a sibling page: replaces the current one
} gfxNav_t;

// the sub menu, the same on every page; the first entries in page slot order
static const gfxNav_t gfxNav[] = {
	{ "Graphics", UI_GraphicsOptionsMenu, qtrue },
	{ "Effects", UI_GfxEffects_Open, qtrue },
	{ "Lighting & shadows", UI_GfxLighting_Open, qtrue },
	{ "Driver info", UI_DriverInfoMenu, qfalse }
};

#define GFX_NUM_NAV		ARRAY_LEN( gfxNav )

static gfxPage_t	gfxPages[GFX_MAX_PAGES];
static qboolean		gfxRestart;		// a changed cvar needs vid_restart

static qboolean Gfx_RendererIsGL2( void ) {
	char buf[32];

	// the renderer that runs, not the one chosen for the next restart
	trap_Cvar_VariableStringBuffer( "cl_renderer", buf, sizeof( buf ) );
	return !Q_stricmp( buf, "opengl2" );
}

/*
=======================================================================

PRESET

=======================================================================
*/

typedef struct {
	const char	*cvar;
	const char	*value[GFX_NUM_PRESETS];	// low, medium, high, ultra
} gfxPresetCvar_t;

static const gfxPresetCvar_t gfxPresetTable[] = {
	// textures and geometry
	{ "r_picmip",							{ "1", "0", "0", "0" } },
	{ "r_textureMode",						{ "GL_LINEAR_MIPMAP_LINEAR", "GL_LINEAR_MIPMAP_LINEAR", "GL_LINEAR_MIPMAP_LINEAR", "GL_LINEAR_MIPMAP_LINEAR" } },
	{ "r_ext_texture_filter_anisotropic",	{ "0", "1", "1", "1" } },
	{ "r_ext_max_anisotropy",				{ "1", "4", "8", "16" } },
	{ "r_lodBias",							{ "1", "0", "0", "0" } },
	{ "r_subdivisions",						{ "12", "4", "4", "4" } },
	// anti-aliasing
	{ "r_fxaa",								{ "1", "1", "1", "1" } },
	// tone mapping and post-processing
	{ "r_hdr",								{ "0", "1", "1", "1" } },
	{ "r_postProcess",						{ "1", "1", "1", "1" } },
	{ "r_toneMap",							{ "0", "1", "1", "1" } },
	{ "r_autoExposure",						{ "0", "1", "1", "1" } },
	{ "r_toneMapCurve",						{ "1", "1", "1", "1" } },
	{ "r_colorGrade",						{ "1", "1", "1", "1" } },
	{ "r_bloom",							{ "0", "1", "1", "1" } },
	{ "r_motionBlur",						{ "0", "0.5", "0.5", "0.5" } },
	{ "r_drawSunRays",						{ "0", "0", "0", "0" } },
	// effects
	{ "r_carReflections",					{ "0", "1", "1", "2" } },
	{ "r_carReflectionSize",				{ "128", "256", "256", "512" } },
	{ "r_water",							{ "0", "1", "1", "1" } },
	{ "r_wetGround",						{ "0", "1", "1", "2" } },
	{ "r_softParticles",					{ "0", "24", "24", "24" } },
	{ "r_ssao",								{ "0", "0", "1", "2" } },
	{ "r_haze",								{ "0", "1", "1", "1" } },
	{ "r_genNormalMaps",					{ "0", "0", "1", "1" } },
	{ "r_marksOnTriangleMeshes",			{ "1", "1", "1", "1" } },
	// lighting and shadows
	{ "r_sunlightMode",						{ "0", "1", "1", "1" } },
	{ "r_sunShadows",						{ "0", "1", "1", "1" } },
	{ "r_shadowMapSize",					{ "512", "1024", "1024", "2048" } },
	{ "r_shadowFilter",						{ "0", "1", "1", "2" } },
	{ "r_lampShadows",						{ "0", "1", "2", "4" } },
	{ "r_lampShadowMapSize",				{ "256", "512", "512", "1024" } },
	{ "r_headlights",						{ "1", "1", "1", "1" } },
	{ "r_headlightShadows",					{ "0", "1", "2", "2" } },
	{ "r_headlightShadowMapSize",			{ "512", "1024", "1024", "2048" } },
	{ "r_dynamiclight",						{ "1", "1", "1", "1" } }
};

// multisampling per preset; see UI_GfxMsaa_Set
static const int gfxPresetMsaa[GFX_NUM_PRESETS] = { 0, 0, 2, 4 };

const char *gfx_preset_names[] = {
	"Low", "Medium", "High", "Ultra", NULL
};

/*
=================
Pending cvar values

A latched cvar (r_picmip, r_mode, ...) keeps reporting its old value until
the renderer restarts. The pages remember what they set, so they show the
new choice right away. The list is lost with the UI on vid_restart, when
the cvars hold the new values anyway.
=================
*/
#define GFX_MAX_PENDING		96

typedef struct {
	char	name[32];
	char	value[64];
} gfxPending_t;

static gfxPending_t	gfxPending[GFX_MAX_PENDING];
static int			gfxNumPending;

void UI_GfxGetCvar( const char *name, char *buf, int size ) {
	int i;

	for ( i = 0; i < gfxNumPending; i++ ) {
		if ( !Q_stricmp( gfxPending[i].name, name ) ) {
			Q_strncpyz( buf, gfxPending[i].value, size );
			return;
		}
	}
	trap_Cvar_VariableStringBuffer( name, buf, size );
}

float UI_GfxCvarValue( const char *name ) {
	char buf[64];

	UI_GfxGetCvar( name, buf, sizeof( buf ) );
	return atof( buf );
}

void UI_GfxSetCvar( const char *name, const char *value ) {
	int i;

	trap_Cvar_Set( name, value );

	for ( i = 0; i < gfxNumPending; i++ ) {
		if ( !Q_stricmp( gfxPending[i].name, name ) ) {
			break;
		}
	}
	if ( i == gfxNumPending ) {
		if ( gfxNumPending == GFX_MAX_PENDING ) {
			return;
		}
		gfxNumPending++;
	}
	Q_strncpyz( gfxPending[i].name, name, sizeof( gfxPending[i].name ) );
	Q_strncpyz( gfxPending[i].value, value, sizeof( gfxPending[i].value ) );
}

void UI_GfxSetCvarValue( const char *name, float value ) {
	char buf[32];

	if ( value == (int)value ) {
		Com_sprintf( buf, sizeof( buf ), "%i", (int)value );
	} else {
		Com_sprintf( buf, sizeof( buf ), "%g", value );
	}
	UI_GfxSetCvar( name, buf );
}

static qboolean Gfx_CvarIs( const char *cvar, const char *value ) {
	char current[MAX_CVAR_VALUE_STRING];
	char *end1, *end2;
	double a, b;

	UI_GfxGetCvar( cvar, current, sizeof( current ) );

	// numbers compare as numbers ("1" == "1.0")
	a = strtod( current, &end1 );
	b = strtod( value, &end2 );
	if ( current[0] && value[0] && *end1 == '\0' && *end2 == '\0' ) {
		return fabs( a - b ) < 0.0001;
	}

	return Q_stricmp( current, value ) == 0;
}

/*
=================
UI_GfxMsaa_Read / UI_GfxMsaa_Set

OpenGL 2 renders into a framebuffer object and multisamples that
(r_ext_framebuffer_multisample); a multisampled window would only cost
memory. OpenGL 1 can only multisample the window (r_ext_multisample).
=================
*/
int UI_GfxMsaa_Read( void ) {
	if ( Gfx_RendererIsGL2() ) {
		return (int)UI_GfxCvarValue( "r_ext_framebuffer_multisample" );
	}
	return (int)UI_GfxCvarValue( "r_ext_multisample" );
}

void UI_GfxMsaa_Set( int samples ) {
	if ( Gfx_RendererIsGL2() ) {
		UI_GfxSetCvarValue( "r_ext_framebuffer_multisample", samples );
		UI_GfxSetCvarValue( "r_ext_multisample", 0 );
	} else {
		UI_GfxSetCvarValue( "r_ext_multisample", samples );
	}
}

int UI_GfxPreset_Detect( void ) {
	int level, i;
	int msaa = UI_GfxMsaa_Read();

	for ( level = 0; level < GFX_NUM_PRESETS; level++ ) {
		if ( msaa != gfxPresetMsaa[level] ) {
			continue;
		}
		for ( i = 0; i < ARRAY_LEN( gfxPresetTable ); i++ ) {
			if ( !Gfx_CvarIs( gfxPresetTable[i].cvar, gfxPresetTable[i].value[level] ) ) {
				break;
			}
		}
		if ( i == ARRAY_LEN( gfxPresetTable ) ) {
			return level;
		}
	}

	return GFX_NUM_PRESETS;		// custom
}

void UI_GfxPreset_Apply( int level ) {
	int i;

	if ( level < 0 || level >= GFX_NUM_PRESETS ) {
		return;
	}

	for ( i = 0; i < ARRAY_LEN( gfxPresetTable ); i++ ) {
		if ( !Gfx_CvarIs( gfxPresetTable[i].cvar, gfxPresetTable[i].value[level] ) ) {
			UI_GfxSetCvar( gfxPresetTable[i].cvar, gfxPresetTable[i].value[level] );
		}
	}
	UI_GfxMsaa_Set( gfxPresetMsaa[level] );
	gfxRestart = qtrue;
}

void UI_GfxMarkRestart( void ) {
	gfxRestart = qtrue;
}

/*
=======================================================================

ROWS

=======================================================================
*/

// calls func for each "cvar=value" of a choice; stops when it returns qfalse
static qboolean Gfx_ForEachSet( const char *sets, qboolean (*func)( const char *cvar, const char *value ) ) {
	char buf[512], *s, *next, *eq;

	Q_strncpyz( buf, sets, sizeof( buf ) );
	for ( s = buf; s && *s; s = next ) {
		next = strchr( s, ';' );
		if ( next ) {
			*next++ = '\0';
		}
		eq = strchr( s, '=' );
		if ( !eq ) {
			continue;
		}
		*eq = '\0';
		if ( !func( s, eq + 1 ) ) {
			return qfalse;
		}
	}
	return qtrue;
}

static qboolean Gfx_SetCvar( const char *cvar, const char *value ) {
	UI_GfxSetCvar( cvar, value );
	return qtrue;
}

static int Gfx_ReadRow( const gfxRow_t *row ) {
	int i;

	if ( row->read ) {
		return row->read();
	}
	for ( i = 0; row->names[i]; i++ ) {
		if ( row->sets && row->sets[i] && Gfx_ForEachSet( row->sets[i], Gfx_CvarIs ) ) {
			return i;
		}
	}
	return -1;		// custom
}

static void Gfx_RefreshPage( gfxPage_t *page ) {
	int i;
	qboolean gl2 = Gfx_RendererIsGL2();

	for ( i = 0; i < page->def->numRows; i++ ) {
		const gfxRow_t *row = &page->def->rows[i];

		page->rows[i].curvalue = Gfx_ReadRow( row );
		if ( ( row->flags & GFXROW_GL2 ) && !gl2 ) {
			page->rows[i].generic.flags |= QMF_GRAYED;
		} else {
			page->rows[i].generic.flags &= ~QMF_GRAYED;
		}
	}
}

static void Gfx_Event( void *ptr, int event ) {
	menucommon_s *item = (menucommon_s *)ptr;
	gfxPage_t *page = (gfxPage_t *)item->parent;
	int id = item->id;

	if ( event != QM_ACTIVATED ) {
		return;
	}

	if ( id >= ID_GFX_ROW && id < ID_GFX_ROW + page->def->numRows ) {
		const gfxRow_t *row = &page->def->rows[id - ID_GFX_ROW];
		int choice = page->rows[id - ID_GFX_ROW].curvalue;

		if ( choice == Gfx_ReadRow( row ) ) {
			return;		// activated without a change (Enter)
		}
		if ( choice < 0 ) {
			choice = 0;
		}
		if ( row->apply ) {
			row->apply( choice );
		} else if ( row->sets && row->sets[choice] ) {
			Gfx_ForEachSet( row->sets[choice], Gfx_SetCvar );
		}
		if ( row->flags & GFXROW_RESTART ) {
			gfxRestart = qtrue;
		}
		Gfx_RefreshPage( page );
		return;
	}

	if ( id >= ID_GFX_NAV && id < ID_GFX_NAV + GFX_NUM_NAV ) {
		int index = id - ID_GFX_NAV;

		if ( index == page->def->slot ) {
			return;		// already here
		}
		if ( gfxNav[index].replace && uis.menusp > 0 ) {
			// the pages replace each other, so Back always leaves the graphics
			// settings; take this page off the stack without UI_PopMenu, which
			// would play the back sound or close the UI
			uis.menusp--;
		}
		gfxNav[index].open();
		return;
	}

	if ( id == ID_GFX_BACK ) {
		UI_PopMenu();
		return;
	}

	if ( id == ID_GFX_APPLY ) {
		// a video restart reloads the UI; come back to the config hub
		trap_Cmd_ExecuteText( EXEC_APPEND, "set q3r_ui_return_config 1\nvid_restart\n" );
	}
}

/*
=======================================================================

DRAWING

=======================================================================
*/

static void Gfx_DrawSetting( void *self ) {
	menulist_s *list = (menulist_s *)self;
	menucommon_s *item = &list->generic;
	const char *value = "Custom";
	qboolean disabled, highlighted;
	vec4_t fillColor, lineColor, labelColor, valueColor;
	int i;

	for ( i = 0; list->itemnames && list->curvalue >= 0 && list->itemnames[i]; i++ ) {
		if ( i == list->curvalue ) {
			value = list->itemnames[i];
			break;
		}
	}

	disabled = ( item->flags & ( QMF_GRAYED | QMF_INACTIVE ) ) ? qtrue : qfalse;
	highlighted = !disabled && ( Menu_ItemAtCursor( item->parent ) == item
		|| ( uis.cursorx >= item->left && uis.cursorx <= item->right
		&& uis.cursory >= item->top && uis.cursory <= item->bottom ) );

	Vector4Copy( gfxPageFocusColor, fillColor );
	fillColor[3] = 0.72f;
	Vector4Copy( gfxPageBorderColor, lineColor );
	lineColor[3] = 0.55f;
	if ( highlighted ) {
		UI_FillRect( item->left, item->top, item->right - item->left, item->bottom - item->top, fillColor );
		Vector4Copy( gfxPageAccentColor, lineColor );
	}
	UI_FillRect( item->left, item->bottom - 1, item->right - item->left, 1, lineColor );

	if ( disabled ) {
		Vector4Copy( gfxPageMutedColor, labelColor );
		labelColor[3] = 0.45f;
		Vector4Copy( labelColor, valueColor );
	} else if ( highlighted ) {
		Vector4Copy( gfxPageAccentColor, labelColor );
		Vector4Copy( gfxPageTextColor, valueColor );
	} else {
		Vector4Copy( gfxPageMutedColor, labelColor );
		Vector4Copy( gfxPageTextColor, valueColor );
	}

	Frontend_DrawText( item->left + UI_FRONTEND_SPACE_SM,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		item->name, UI_LEFT | UI_SMALLFONT, labelColor );
	Frontend_DrawText( item->left + GFX_VALUE_OFFSET,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		value, UI_LEFT | UI_SMALLFONT, valueColor );
}

static void Gfx_DrawNav( void *self ) {
	menutext_s *text = (menutext_s *)self;
	menucommon_s *item = &text->generic;
	gfxPage_t *page = (gfxPage_t *)item->parent;
	qboolean active = ( item->id - ID_GFX_NAV == page->def->slot );

	Frontend_DrawNavButton( item->left, item->top, item->right - item->left, item->bottom - item->top,
		text->string, 1.0f, active || Menu_ItemAtCursor( item->parent ) == item, UI_LEFT );
}

static void Gfx_DrawAction( void *self ) {
	menutext_s *text = (menutext_s *)self;
	menucommon_s *item = &text->generic;

	Frontend_DrawButton( item->left, item->top, item->right - item->left, item->bottom - item->top,
		text->string, 1.0f, Menu_ItemAtCursor( item->parent ) == item, UI_CENTER );
}

static gfxPage_t *Gfx_CurrentPage( void ) {
	int i;

	for ( i = 0; i < GFX_MAX_PAGES; i++ ) {
		if ( uis.activemenu == &gfxPages[i].menu ) {
			return &gfxPages[i];
		}
	}
	return NULL;
}

static void Gfx_MenuDraw( void ) {
	gfxPage_t *page = Gfx_CurrentPage();
	const gfxPageDef_t *def;
	const char *hint;

	if ( !page ) {
		return;
	}
	def = page->def;

	// rows follow the cvars, also after changes made on another page
	Gfx_RefreshPage( page );

	// the Apply button only when something needs the renderer restarted
	if ( gfxRestart ) {
		page->apply.generic.flags &= ~( QMF_HIDDEN | QMF_INACTIVE );
	} else {
		page->apply.generic.flags |= QMF_HIDDEN | QMF_INACTIVE;
	}

	Frontend_DrawBackground( gfxPageScrimColor );
	Frontend_DrawPanel( GFX_FRAME_X, GFX_FRAME_Y, GFX_FRAME_WIDTH, GFX_FRAME_HEIGHT, 1.0f, UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( GFX_FRAME_X + 24, GFX_FRAME_Y + 24, def->title, UI_LEFT | UI_BIGFONT, gfxPageTextColor );
	Frontend_DrawText( GFX_FRAME_X + 24, GFX_FRAME_Y + 48, def->subtitle, UI_LEFT | UI_SMALLFONT, gfxPageMutedColor );
	Frontend_DrawStatusChip( GFX_FRAME_X + GFX_FRAME_WIDTH - 104, GFX_FRAME_Y + 26, "Settings", gfxPageAccentColor, 1.0f );

	Frontend_DrawCard( GFX_NAV_X, GFX_NAV_Y, GFX_NAV_WIDTH, GFX_NAV_HEIGHT, 1.0f, qfalse );
	Frontend_DrawCard( GFX_DETAIL_X, GFX_DETAIL_Y, GFX_DETAIL_WIDTH, GFX_DETAIL_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( GFX_NAV_X + 16, GFX_NAV_Y + 22, "Submenu", UI_LEFT | UI_SMALLFONT, gfxPageMutedColor );
	Frontend_DrawText( GFX_DETAIL_X + 16, GFX_DETAIL_Y + 22, def->section, UI_LEFT | UI_SMALLFONT, gfxPageMutedColor );

	Menu_Draw( &page->menu );

	if ( gfxRestart ) {
		hint = "Apply restarts the renderer for the changed settings";
	} else if ( def->gl2Only && !Gfx_RendererIsGL2() ) {
		hint = "These settings need the OpenGL 2 renderer";
	} else {
		hint = "Select an option   Left / right adjust   Esc back";
	}
	Frontend_DrawText( GFX_FRAME_X + 24, GFX_FRAME_Y + 384, hint, UI_LEFT | UI_SMALLFONT,
		gfxRestart ? gfxPageAccentColor : gfxPageMutedColor );
}

static sfxHandle_t Gfx_MenuKey( int key ) {
	gfxPage_t *page = Gfx_CurrentPage();

	if ( key == K_MOUSE2 || key == K_ESCAPE ) {
		UI_PopMenu();
		return menu_out_sound;
	}
	if ( !page ) {
		return 0;
	}
	return Menu_DefaultKey( &page->menu, key );
}

static void Gfx_SetBounds( menucommon_s *item, int id, int x, int y, int width, int height ) {
	item->id = id;
	item->x = x;
	item->y = y;
	item->left = x;
	item->top = y;
	item->right = x + width;
	item->bottom = y + height;
}

/*
=================
UI_GfxPage_Push

Builds a page from its table and opens it.
=================
*/
void UI_GfxPage_Push( const gfxPageDef_t *def ) {
	gfxPage_t *page;
	int i, column, index;

	if ( def->slot < 0 || def->slot >= GFX_MAX_PAGES ) {
		return;
	}
	page = &gfxPages[def->slot];

	memset( page, 0, sizeof( *page ) );
	page->def = def;
	page->menu.wrapAround = qtrue;
	page->menu.fullscreen = qtrue;
	page->menu.draw = Gfx_MenuDraw;
	page->menu.key = Gfx_MenuKey;

	// the same sub menu on every page, the current page highlighted
	for ( i = 0; i < GFX_NUM_NAV; i++ ) {
		menutext_s *nav = &page->nav[i];

		nav->generic.type = MTYPE_PTEXT;
		nav->generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
		nav->generic.callback = Gfx_Event;
		nav->string = (char *)gfxNav[i].label;
		nav->style = UI_LEFT | UI_SMALLFONT;
		nav->color = text_color_normal;
		Menu_AddItem( &page->menu, nav );
		nav->generic.ownerdraw = Gfx_DrawNav;
		Gfx_SetBounds( &nav->generic, ID_GFX_NAV + i, GFX_NAV_X + 16,
			GFX_NAV_Y + 48 + i * 32 + ( gfxNav[i].replace ? 0 : 16 ), GFX_NAV_WIDTH - 32, GFX_ROW_HEIGHT );
	}

	for ( i = 0; i < def->numRows && i < GFX_MAX_ROWS; i++ ) {
		menulist_s *list = &page->rows[i];

		list->generic.type = MTYPE_SPINCONTROL;
		list->generic.name = (char *)def->rows[i].label;
		list->generic.flags = QMF_PULSEIFFOCUS | QMF_SMALLFONT;
		list->generic.callback = Gfx_Event;
		list->itemnames = def->rows[i].names;
		Menu_AddItem( &page->menu, list );

		column = ( i < def->leftRows ) ? 0 : 1;
		index = column ? i - def->leftRows : i;
		list->generic.ownerdraw = Gfx_DrawSetting;
		Gfx_SetBounds( &list->generic, ID_GFX_ROW + i,
			column ? GFX_COLUMN_RIGHT_X : GFX_COLUMN_LEFT_X,
			GFX_ROW_START_Y + index * ( GFX_ROW_HEIGHT + GFX_ROW_GAP ), GFX_COLUMN_WIDTH, GFX_ROW_HEIGHT );
		list->generic.name = (char *)def->rows[i].label;
	}

	page->back.generic.type = MTYPE_PTEXT;
	page->back.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
	page->back.generic.callback = Gfx_Event;
	page->back.string = "Back";
	page->back.style = UI_LEFT | UI_SMALLFONT;
	page->back.color = text_color_normal;
	Menu_AddItem( &page->menu, &page->back );
	page->back.generic.ownerdraw = Gfx_DrawAction;
	Gfx_SetBounds( &page->back.generic, ID_GFX_BACK, GFX_NAV_X, GFX_ACTION_Y, GFX_ACTION_WIDTH, GFX_ACTION_HEIGHT );

	page->apply.generic.type = MTYPE_PTEXT;
	page->apply.generic.flags = QMF_RIGHT_JUSTIFY | QMF_PULSEIFFOCUS | QMF_HIDDEN | QMF_INACTIVE;
	page->apply.generic.callback = Gfx_Event;
	page->apply.string = "Apply";
	page->apply.style = UI_RIGHT | UI_SMALLFONT;
	page->apply.color = text_color_normal;
	Menu_AddItem( &page->menu, &page->apply );
	page->apply.generic.ownerdraw = Gfx_DrawAction;
	Gfx_SetBounds( &page->apply.generic, ID_GFX_APPLY,
		GFX_FRAME_X + GFX_FRAME_WIDTH - GFX_ACTION_WIDTH, GFX_ACTION_Y, GFX_ACTION_WIDTH, GFX_ACTION_HEIGHT );

	Gfx_RefreshPage( page );

	uis.transitionIn = 0;
	UI_PushMenu( &page->menu );
	if ( def->numRows > 0 ) {
		Menu_SetCursorToItem( &page->menu, &page->rows[0] );
	}
}

/*
=======================================================================

EFFECTS PAGE

=======================================================================
*/

static const char *gfx_onoff[] = { "Off", "On", NULL };
static const char *gfx_onoff_fine[] = { "Off", "On", "Fine", NULL };

static const char *bloom_sets[] = { "r_bloom=0", "r_bloom=1" };
static const char *motionblur_names[] = { "Off", "Low", "Medium", "High", NULL };
static const char *motionblur_sets[] = { "r_motionBlur=0", "r_motionBlur=0.3", "r_motionBlur=0.5", "r_motionBlur=0.8" };
static const char *carrefl_names[] = { "Static", "Live", "Every frame", NULL };
static const char *carrefl_sets[] = { "r_carReflections=0", "r_carReflections=1", "r_carReflections=2" };
static const char *water_sets[] = { "r_water=0", "r_water=1" };
static const char *wet_sets[] = { "r_wetGround=0", "r_wetGround=1", "r_wetGround=2" };
static const char *haze_sets[] = { "r_haze=0", "r_haze=1" };
static const char *ssao_sets[] = { "r_ssao=0", "r_ssao=1", "r_ssao=2" };
static const char *relief_sets[] = { "r_genNormalMaps=0", "r_genNormalMaps=1" };
static const char *smoke_names[] = { "Off", "Normal", "Thick", NULL };
static const char *smoke_sets[] = { "cg_tireSmoke=0", "cg_tireSmoke=1", "cg_tireSmoke=2" };
static const char *soft_sets[] = { "r_softParticles=0", "r_softParticles=24" };
static const char *marks_names[] = { "Roads only", "Everywhere", NULL };
static const char *marks_sets[] = { "r_marksOnTriangleMeshes=0", "r_marksOnTriangleMeshes=1" };
static const char *look_names[] = { "Map", "Off", "Warm", "Cold", "Night", "Vivid", "Overcast", "Dusk", NULL };
static const char *look_sets[] = {
	"r_colorGrade=1;r_colorLut=", "r_colorGrade=0",
	"r_colorGrade=1;r_colorLut=gfx/luts/warm", "r_colorGrade=1;r_colorLut=gfx/luts/cold",
	"r_colorGrade=1;r_colorLut=gfx/luts/night", "r_colorGrade=1;r_colorLut=gfx/luts/vivid",
	"r_colorGrade=1;r_colorLut=gfx/luts/overcast", "r_colorGrade=1;r_colorLut=gfx/luts/dusk"
};
static const char *curve_names[] = { "Filmic", "Classic", NULL };
static const char *curve_sets[] = { "r_toneMapCurve=1", "r_toneMapCurve=0" };

static const gfxRow_t gfxEffectsRows[] = {
	{ "Bloom", gfx_onoff, bloom_sets, GFXROW_RESTART | GFXROW_GL2 },
	{ "Motion blur", motionblur_names, motionblur_sets, GFXROW_GL2 },
	{ "Reflections", carrefl_names, carrefl_sets, GFXROW_GL2 },
	{ "Water", gfx_onoff, water_sets, GFXROW_RESTART | GFXROW_GL2 },
	{ "Wet roads", gfx_onoff_fine, wet_sets, GFXROW_RESTART | GFXROW_GL2 },
	{ "Haze", gfx_onoff, haze_sets, GFXROW_GL2 },
	{ "Colour look", look_names, look_sets, GFXROW_GL2 },
	{ "Ambient occl.", gfx_onoff_fine, ssao_sets, GFXROW_RESTART | GFXROW_GL2 },
	{ "Surface relief", gfx_onoff, relief_sets, GFXROW_RESTART | GFXROW_GL2 },
	{ "Tyre smoke", smoke_names, smoke_sets, 0 },
	{ "Soft particles", gfx_onoff, soft_sets, GFXROW_GL2 },
	{ "Skid marks", marks_names, marks_sets, 0 },
	{ "Tone curve", curve_names, curve_sets, GFXROW_GL2 }
};

/*
=======================================================================

LIGHTING & SHADOWS PAGE

=======================================================================
*/

static const char *hdr_sets[] = { "r_hdr=0;r_toneMap=0;r_autoExposure=0", "r_hdr=1;r_toneMap=1;r_autoExposure=1" };
static const char *sun_sets[] = { "r_sunlightMode=0;r_sunShadows=0", "r_sunlightMode=1;r_sunShadows=1" };
static const char *quality_names[] = { "Low", "Medium", "High", "Ultra", NULL };
static const char *shadowq_sets[] = {
	"r_shadowMapSize=512;r_shadowFilter=0", "r_shadowMapSize=1024;r_shadowFilter=1",
	"r_shadowMapSize=2048;r_shadowFilter=1", "r_shadowMapSize=2048;r_shadowFilter=2"
};
static const char *lamp_names[] = { "Off", "1 lamp", "2 lamps", "4 lamps", NULL };
static const char *lamp_sets[] = { "r_lampShadows=0", "r_lampShadows=1", "r_lampShadows=2", "r_lampShadows=4" };
static const char *headlight_names[] = { "Simple", "Light cones", NULL };
static const char *headlight_sets[] = { "r_headlights=0", "r_headlights=1" };
static const char *hlshadow_names[] = { "Off", "1 car", "2 cars", NULL };
static const char *hlshadow_sets[] = { "r_headlightShadows=0", "r_headlightShadows=1", "r_headlightShadows=2" };
static const char *detail_names[] = { "Low", "Medium", "High", NULL };
static const char *detail_sets[] = {
	"r_lampShadowMapSize=256;r_headlightShadowMapSize=512",
	"r_lampShadowMapSize=512;r_headlightShadowMapSize=1024",
	"r_lampShadowMapSize=1024;r_headlightShadowMapSize=2048"
};
static const char *dlight_sets[] = { "r_dynamiclight=0", "r_dynamiclight=1" };

static const gfxRow_t gfxLightingRows[] = {
	// left: picture and sun
	{ "HDR", gfx_onoff, hdr_sets, GFXROW_RESTART | GFXROW_GL2 },
	{ "Sun shadows", gfx_onoff, sun_sets, GFXROW_RESTART | GFXROW_GL2 },
	{ "Sun quality", quality_names, shadowq_sets, GFXROW_RESTART | GFXROW_GL2 },
	{ "Dynamic lights", gfx_onoff, dlight_sets, 0 },
	// right: headlights and lamps
	{ "Headlights", headlight_names, headlight_sets, GFXROW_GL2 },
	{ "Headlight shad.", hlshadow_names, hlshadow_sets, GFXROW_GL2 },
	{ "Lamp shadows", lamp_names, lamp_sets, GFXROW_GL2 },
	{ "Shadow detail", detail_names, detail_sets, GFXROW_RESTART | GFXROW_GL2 }
};

static const gfxPageDef_t gfxEffectsPage = {
	"Effects", "Reflections, weather, particles and the look of the picture", "Effects",
	gfxEffectsRows, ARRAY_LEN( gfxEffectsRows ), 7, 1, qtrue
};

static const gfxPageDef_t gfxLightingPage = {
	"Lighting & shadows", "HDR, sun, lamp and headlight shadows", "Lighting & shadows",
	gfxLightingRows, ARRAY_LEN( gfxLightingRows ), 4, 2, qtrue
};

static void UI_GfxEffects_Open( void ) {
	UI_GfxPage_Push( &gfxEffectsPage );
}

static void UI_GfxLighting_Open( void ) {
	UI_GfxPage_Push( &gfxLightingPage );
}

void UI_GfxEffectsMenu( void ) {
	UI_GfxEffects_Open();
}

void UI_GfxLightingMenu( void ) {
	UI_GfxLighting_Open();
}

void UI_AdvancedGraphicsOptionsMenu_Cache( void ) {
}

// the old entry point opens the effects page
void UI_AdvancedGraphicsOptionsMenu( void ) {
	UI_GfxEffects_Open();
}
