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

ADVANCED GRAPHICS OPTIONS MENU

=======================================================================
*/

#include "ui_local.h"
#include "ui_rally_frontend.h"

#define ID_GRAPHICS                 10
#define ID_ADVANCED_GRAPHICS        17
#define ID_DISPLAY                  11
#define ID_SOUND                    12
#define ID_NETWORK                  13
#define ID_HDR                      14
#define ID_POSTPROCESS              15
#define ID_BACK                     16
#define ID_TONEMAP                  18
#define ID_AUTOEXPOSURE             19
#define ID_SSAO                     20
#define ID_SUNRAYS                  21
#define ID_DYNAMIC_REFLECTIONS      22
#define ID_PRESET                   23
#define ID_SUNSHADOWS               24
#define ID_SHADOWQUALITY            25
#define ID_SHADOWBLUR               26
#define ID_SHADOWCASCADE_NEAR       27
#define ID_SHADOWCASCADE_FAR        28
#define ID_SHADOWCASCADE_BIAS       29
#define ID_PSHADOWDIST              30
#define ID_SUNLIGHTMODE             31

#define ADV_PRESET_CUSTOM           0
#define ADV_PRESET_LOW              1
#define ADV_PRESET_MIDDLE           2
#define ADV_PRESET_HIGH             3
#define ADV_PRESET_ULTRA            4

#define ADV_FRAME_X                24
#define ADV_FRAME_Y                20
#define ADV_FRAME_WIDTH            592
#define ADV_FRAME_HEIGHT           440
#define ADV_NAV_X                  40
#define ADV_NAV_Y                  104
#define ADV_NAV_WIDTH              164
#define ADV_NAV_HEIGHT             292
#define ADV_DETAIL_X               220
#define ADV_DETAIL_Y               104
#define ADV_DETAIL_WIDTH           376
#define ADV_DETAIL_HEIGHT          292
#define ADV_ROW_HEIGHT             24
#define ADV_ROW_GAP                4
#define ADV_ROW_START_Y            148
#define ADV_COLUMN_WIDTH           176
#define ADV_COLUMN_LEFT_X          236
#define ADV_COLUMN_RIGHT_X         420
#define ADV_VALUE_OFFSET           112
#define ADV_ACTION_Y               420
#define ADV_ACTION_WIDTH           120
#define ADV_ACTION_HEIGHT          24

static vec4_t advancedScrimColor = UI_FRONTEND_COLOR_SCRIM;
static vec4_t advancedAccentColor = UI_FRONTEND_COLOR_ACCENT;
static vec4_t advancedTextColor = UI_FRONTEND_COLOR_TEXT;
static vec4_t advancedMutedColor = UI_FRONTEND_COLOR_MUTED;
static vec4_t advancedBorderColor = UI_FRONTEND_COLOR_BORDER;
static vec4_t advancedFocusColor = UI_FRONTEND_COLOR_FOCUS_BG;

static const char *enabled_items[] = {
	"Off", "On", NULL
};

static const char *preset_items[] = {
	"Custom", "Low", "Middle", "High", "Ultra", NULL
};

static const char *shadow_quality_items[] = {
	"Low", "Medium", "High", NULL
};

static const char *shadow_cascade_near_items[] = {
	"4", "8", "16", NULL
};

static const char *shadow_cascade_far_items[] = {
	"512", "1024", "2048", NULL
};

static const char *shadow_cascade_bias_items[] = {
	"0.0", "0.25", "0.5", "1.0", NULL
};

static const char *pshadow_dist_items[] = {
	"64", "128", "256", NULL
};

static const char *sunlight_mode_items[] = {
	"Off", "Mode 1", "Mode 2", NULL
};

typedef struct {
	menuframework_s menu;

	menutext_s      banner;
	menutext_s      graphics;
	menutext_s      advanced_graphics;
	menutext_s      display;
	menutext_s      sound;
	menutext_s      network;

	menulist_s      preset;
	menulist_s      hdr;
	menulist_s      postprocess;
	menulist_s      tonemap;
	menulist_s      autoexposure;
	menulist_s      ssao;
	menulist_s      sunrays;
	menulist_s      dynamic_reflections;
	menulist_s      sunshadows;
	menulist_s      shadowquality;
	menulist_s      shadowblur;
	menulist_s      shadowcascade_near;
	menulist_s      shadowcascade_far;
	menulist_s      shadowcascade_bias;
	menulist_s      pshadowdist;
	menulist_s      sunlightmode;

	menutext_s      back;
} advancedGraphicsOptionsInfo_t;

static advancedGraphicsOptionsInfo_t advancedGraphicsOptionsInfo;

static void UI_AdvancedGraphicsOptionsMenu_ApplyShadowCvars( void ) {
	static const int shadowFilterValues[] = { 0, 1, 2 };
	static const int shadowMapSizeValues[] = { 512, 1024, 2048 };
	static const int shadowCascadeNearValues[] = { 4, 8, 16 };
	static const int shadowCascadeFarValues[] = { 512, 1024, 2048 };
	static const float shadowCascadeBiasValues[] = { 0.0f, 0.25f, 0.5f, 1.0f };
	static const int pshadowDistValues[] = { 64, 128, 256 };
	int qualityIndex = Com_Clamp( 0, 2, advancedGraphicsOptionsInfo.shadowquality.curvalue );
	int cascadeNearIndex = Com_Clamp( 0, 2, advancedGraphicsOptionsInfo.shadowcascade_near.curvalue );
	int cascadeFarIndex = Com_Clamp( 0, 2, advancedGraphicsOptionsInfo.shadowcascade_far.curvalue );
	int cascadeBiasIndex = Com_Clamp( 0, 3, advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue );
	int pshadowDistIndex = Com_Clamp( 0, 2, advancedGraphicsOptionsInfo.pshadowdist.curvalue );
	int sunlightMode = Com_Clamp( 0, 2, advancedGraphicsOptionsInfo.sunlightmode.curvalue );

	trap_Cvar_SetValue( "r_sunShadows", advancedGraphicsOptionsInfo.sunshadows.curvalue );
	trap_Cvar_SetValue( "r_shadowFilter", shadowFilterValues[qualityIndex] );
	trap_Cvar_SetValue( "r_shadowMapSize", shadowMapSizeValues[qualityIndex] );
	trap_Cvar_SetValue( "r_shadowBlur", advancedGraphicsOptionsInfo.shadowblur.curvalue != 0 );
	trap_Cvar_SetValue( "r_shadowCascadeZNear", shadowCascadeNearValues[cascadeNearIndex] );
	trap_Cvar_SetValue( "r_shadowCascadeZFar", shadowCascadeFarValues[cascadeFarIndex] );
	trap_Cvar_SetValue( "r_shadowCascadeZBias", shadowCascadeBiasValues[cascadeBiasIndex] );
	trap_Cvar_SetValue( "r_pshadowDist", pshadowDistValues[pshadowDistIndex] );
	trap_Cvar_SetValue( "r_sunlightMode", sunlightMode );
}

static void UI_AdvancedGraphicsOptionsMenu_SyncPreset( void ) {
	if (advancedGraphicsOptionsInfo.hdr.curvalue == 0 &&
		advancedGraphicsOptionsInfo.postprocess.curvalue == 0 &&
		advancedGraphicsOptionsInfo.tonemap.curvalue == 0 &&
		advancedGraphicsOptionsInfo.autoexposure.curvalue == 0 &&
		advancedGraphicsOptionsInfo.ssao.curvalue == 0 &&
		advancedGraphicsOptionsInfo.sunrays.curvalue == 0 &&
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue == 0 &&
		advancedGraphicsOptionsInfo.sunshadows.curvalue == 0 &&
		advancedGraphicsOptionsInfo.shadowquality.curvalue == 0 &&
		advancedGraphicsOptionsInfo.shadowblur.curvalue == 0 &&
		advancedGraphicsOptionsInfo.shadowcascade_near.curvalue == 0 &&
		advancedGraphicsOptionsInfo.shadowcascade_far.curvalue == 0 &&
		advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue == 0 &&
		advancedGraphicsOptionsInfo.pshadowdist.curvalue == 0 &&
		advancedGraphicsOptionsInfo.sunlightmode.curvalue == 0) {
		advancedGraphicsOptionsInfo.preset.curvalue = ADV_PRESET_LOW;
		return;
	}

	if (advancedGraphicsOptionsInfo.hdr.curvalue == 1 &&
		advancedGraphicsOptionsInfo.postprocess.curvalue == 1 &&
		advancedGraphicsOptionsInfo.tonemap.curvalue == 1 &&
		advancedGraphicsOptionsInfo.autoexposure.curvalue == 1 &&
		advancedGraphicsOptionsInfo.ssao.curvalue == 0 &&
		advancedGraphicsOptionsInfo.sunrays.curvalue == 0 &&
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue == 0 &&
		advancedGraphicsOptionsInfo.sunshadows.curvalue == 0 &&
		advancedGraphicsOptionsInfo.shadowquality.curvalue == 0 &&
		advancedGraphicsOptionsInfo.shadowblur.curvalue == 0 &&
		advancedGraphicsOptionsInfo.shadowcascade_near.curvalue == 0 &&
		advancedGraphicsOptionsInfo.shadowcascade_far.curvalue == 0 &&
		advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue == 0 &&
		advancedGraphicsOptionsInfo.pshadowdist.curvalue == 0 &&
		advancedGraphicsOptionsInfo.sunlightmode.curvalue == 0) {
		advancedGraphicsOptionsInfo.preset.curvalue = ADV_PRESET_MIDDLE;
		return;
	}

	if (advancedGraphicsOptionsInfo.hdr.curvalue == 1 &&
		advancedGraphicsOptionsInfo.postprocess.curvalue == 1 &&
		advancedGraphicsOptionsInfo.tonemap.curvalue == 1 &&
		advancedGraphicsOptionsInfo.autoexposure.curvalue == 1 &&
		advancedGraphicsOptionsInfo.ssao.curvalue == 1 &&
		advancedGraphicsOptionsInfo.sunrays.curvalue == 0 &&
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue == 0 &&
		advancedGraphicsOptionsInfo.sunshadows.curvalue == 1 &&
		advancedGraphicsOptionsInfo.shadowquality.curvalue == 1 &&
		advancedGraphicsOptionsInfo.shadowblur.curvalue == 0 &&
		advancedGraphicsOptionsInfo.shadowcascade_near.curvalue == 1 &&
		advancedGraphicsOptionsInfo.shadowcascade_far.curvalue == 1 &&
		advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue == 0 &&
		advancedGraphicsOptionsInfo.pshadowdist.curvalue == 1 &&
		advancedGraphicsOptionsInfo.sunlightmode.curvalue == 1) {
		advancedGraphicsOptionsInfo.preset.curvalue = ADV_PRESET_HIGH;
		return;
	}

	if (advancedGraphicsOptionsInfo.hdr.curvalue == 1 &&
		advancedGraphicsOptionsInfo.postprocess.curvalue == 1 &&
		advancedGraphicsOptionsInfo.tonemap.curvalue == 1 &&
		advancedGraphicsOptionsInfo.autoexposure.curvalue == 1 &&
		advancedGraphicsOptionsInfo.ssao.curvalue == 1 &&
		advancedGraphicsOptionsInfo.sunrays.curvalue == 1 &&
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue == 1 &&
		advancedGraphicsOptionsInfo.sunshadows.curvalue == 1 &&
		advancedGraphicsOptionsInfo.shadowquality.curvalue == 2 &&
		advancedGraphicsOptionsInfo.shadowblur.curvalue == 1 &&
		advancedGraphicsOptionsInfo.shadowcascade_near.curvalue == 2 &&
		advancedGraphicsOptionsInfo.shadowcascade_far.curvalue == 2 &&
		advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue == 2 &&
		advancedGraphicsOptionsInfo.pshadowdist.curvalue == 2 &&
		advancedGraphicsOptionsInfo.sunlightmode.curvalue == 2) {
		advancedGraphicsOptionsInfo.preset.curvalue = ADV_PRESET_ULTRA;
		return;
	}

	advancedGraphicsOptionsInfo.preset.curvalue = ADV_PRESET_CUSTOM;
}

static void UI_AdvancedGraphicsOptionsMenu_ApplyPreset( int preset ) {
	switch (preset) {
	case ADV_PRESET_LOW:
		advancedGraphicsOptionsInfo.hdr.curvalue = 0;
		advancedGraphicsOptionsInfo.postprocess.curvalue = 0;
		advancedGraphicsOptionsInfo.tonemap.curvalue = 0;
		advancedGraphicsOptionsInfo.autoexposure.curvalue = 0;
		advancedGraphicsOptionsInfo.ssao.curvalue = 0;
		advancedGraphicsOptionsInfo.sunrays.curvalue = 0;
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue = 0;
		advancedGraphicsOptionsInfo.sunshadows.curvalue = 0;
		advancedGraphicsOptionsInfo.shadowquality.curvalue = 0;
		advancedGraphicsOptionsInfo.shadowblur.curvalue = 0;
		advancedGraphicsOptionsInfo.shadowcascade_near.curvalue = 0;
		advancedGraphicsOptionsInfo.shadowcascade_far.curvalue = 0;
		advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue = 0;
		advancedGraphicsOptionsInfo.pshadowdist.curvalue = 0;
		advancedGraphicsOptionsInfo.sunlightmode.curvalue = 0;
		break;
	case ADV_PRESET_MIDDLE:
		advancedGraphicsOptionsInfo.hdr.curvalue = 1;
		advancedGraphicsOptionsInfo.postprocess.curvalue = 1;
		advancedGraphicsOptionsInfo.tonemap.curvalue = 1;
		advancedGraphicsOptionsInfo.autoexposure.curvalue = 1;
		advancedGraphicsOptionsInfo.ssao.curvalue = 0;
		advancedGraphicsOptionsInfo.sunrays.curvalue = 0;
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue = 0;
		advancedGraphicsOptionsInfo.sunshadows.curvalue = 0;
		advancedGraphicsOptionsInfo.shadowquality.curvalue = 0;
		advancedGraphicsOptionsInfo.shadowblur.curvalue = 0;
		advancedGraphicsOptionsInfo.shadowcascade_near.curvalue = 0;
		advancedGraphicsOptionsInfo.shadowcascade_far.curvalue = 0;
		advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue = 0;
		advancedGraphicsOptionsInfo.pshadowdist.curvalue = 0;
		advancedGraphicsOptionsInfo.sunlightmode.curvalue = 0;
		break;
	case ADV_PRESET_HIGH:
		advancedGraphicsOptionsInfo.hdr.curvalue = 1;
		advancedGraphicsOptionsInfo.postprocess.curvalue = 1;
		advancedGraphicsOptionsInfo.tonemap.curvalue = 1;
		advancedGraphicsOptionsInfo.autoexposure.curvalue = 1;
		advancedGraphicsOptionsInfo.ssao.curvalue = 1;
		advancedGraphicsOptionsInfo.sunrays.curvalue = 0;
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue = 0;
		advancedGraphicsOptionsInfo.sunshadows.curvalue = 1;
		advancedGraphicsOptionsInfo.shadowquality.curvalue = 1;
		advancedGraphicsOptionsInfo.shadowblur.curvalue = 0;
		advancedGraphicsOptionsInfo.shadowcascade_near.curvalue = 1;
		advancedGraphicsOptionsInfo.shadowcascade_far.curvalue = 1;
		advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue = 0;
		advancedGraphicsOptionsInfo.pshadowdist.curvalue = 1;
		advancedGraphicsOptionsInfo.sunlightmode.curvalue = 1;
		break;
	case ADV_PRESET_ULTRA:
		advancedGraphicsOptionsInfo.hdr.curvalue = 1;
		advancedGraphicsOptionsInfo.postprocess.curvalue = 1;
		advancedGraphicsOptionsInfo.tonemap.curvalue = 1;
		advancedGraphicsOptionsInfo.autoexposure.curvalue = 1;
		advancedGraphicsOptionsInfo.ssao.curvalue = 1;
		advancedGraphicsOptionsInfo.sunrays.curvalue = 1;
		advancedGraphicsOptionsInfo.dynamic_reflections.curvalue = 1;
		advancedGraphicsOptionsInfo.sunshadows.curvalue = 1;
		advancedGraphicsOptionsInfo.shadowquality.curvalue = 2;
		advancedGraphicsOptionsInfo.shadowblur.curvalue = 1;
		advancedGraphicsOptionsInfo.shadowcascade_near.curvalue = 2;
		advancedGraphicsOptionsInfo.shadowcascade_far.curvalue = 2;
		advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue = 2;
		advancedGraphicsOptionsInfo.pshadowdist.curvalue = 2;
		advancedGraphicsOptionsInfo.sunlightmode.curvalue = 2;
		break;
	default:
		return;
	}

	trap_Cvar_SetValue( "r_hdr", advancedGraphicsOptionsInfo.hdr.curvalue );
	trap_Cvar_SetValue( "r_postProcess", advancedGraphicsOptionsInfo.postprocess.curvalue );
	trap_Cvar_SetValue( "r_toneMap", advancedGraphicsOptionsInfo.tonemap.curvalue );
	trap_Cvar_SetValue( "r_autoExposure", advancedGraphicsOptionsInfo.autoexposure.curvalue );
	trap_Cvar_SetValue( "r_ssao", advancedGraphicsOptionsInfo.ssao.curvalue );
	trap_Cvar_SetValue( "r_drawSunRays", advancedGraphicsOptionsInfo.sunrays.curvalue );
	trap_Cvar_SetValue( "r_dynamicReflections", advancedGraphicsOptionsInfo.dynamic_reflections.curvalue );
	UI_AdvancedGraphicsOptionsMenu_ApplyShadowCvars();
}

static void UI_AdvancedGraphicsOptionsMenu_Event( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_PRESET:
		UI_AdvancedGraphicsOptionsMenu_ApplyPreset( advancedGraphicsOptionsInfo.preset.curvalue );
		break;

	case ID_HDR:
		trap_Cvar_SetValue( "r_hdr", advancedGraphicsOptionsInfo.hdr.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_POSTPROCESS:
		trap_Cvar_SetValue( "r_postProcess", advancedGraphicsOptionsInfo.postprocess.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_TONEMAP:
		trap_Cvar_SetValue( "r_toneMap", advancedGraphicsOptionsInfo.tonemap.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_AUTOEXPOSURE:
		trap_Cvar_SetValue( "r_autoExposure", advancedGraphicsOptionsInfo.autoexposure.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_SSAO:
		trap_Cvar_SetValue( "r_ssao", advancedGraphicsOptionsInfo.ssao.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_SUNRAYS:
		trap_Cvar_SetValue( "r_drawSunRays", advancedGraphicsOptionsInfo.sunrays.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_DYNAMIC_REFLECTIONS:
		trap_Cvar_SetValue( "r_dynamicReflections", advancedGraphicsOptionsInfo.dynamic_reflections.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_SUNSHADOWS:
		trap_Cvar_SetValue( "r_sunShadows", advancedGraphicsOptionsInfo.sunshadows.curvalue );
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_SHADOWQUALITY:
		UI_AdvancedGraphicsOptionsMenu_ApplyShadowCvars();
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_SHADOWBLUR:
	case ID_SHADOWCASCADE_NEAR:
	case ID_SHADOWCASCADE_FAR:
	case ID_SHADOWCASCADE_BIAS:
	case ID_PSHADOWDIST:
	case ID_SUNLIGHTMODE:
		UI_AdvancedGraphicsOptionsMenu_ApplyShadowCvars();
		UI_AdvancedGraphicsOptionsMenu_SyncPreset();
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}

static const char *AdvancedGraphics_ListValue( menulist_s *item ) {
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

static void AdvancedGraphics_DrawNavItem( void *self ) {
	menutext_s *text;
	menucommon_s *item;
	qboolean focus;
	qboolean active;

	text = (menutext_s *)self;
	item = &text->generic;
	focus = ( Menu_ItemAtCursor( item->parent ) == item );
	active = ( item->id == ID_ADVANCED_GRAPHICS );
	Frontend_DrawNavButton( item->left, item->top,
		item->right - item->left, item->bottom - item->top,
		text->string, 1.0f, active || focus, UI_LEFT );
}

static void AdvancedGraphics_DrawSetting( void *self ) {
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
	value = AdvancedGraphics_ListValue( (menulist_s *)item );

	Vector4Copy( advancedFocusColor, fillColor );
	fillColor[3] = highlighted ? 0.72f : 0.0f;
	Vector4Copy( advancedBorderColor, lineColor );
	lineColor[3] = 0.55f;
	if ( highlighted ) {
		UI_FillRect( item->left, item->top,
			item->right - item->left, item->bottom - item->top, fillColor );
		Vector4Copy( advancedAccentColor, lineColor );
	}
	UI_FillRect( item->left, item->bottom - 1,
		item->right - item->left, 1, lineColor );

	if ( disabled ) {
		Vector4Copy( advancedMutedColor, labelColor );
		labelColor[3] = 0.65f;
		Vector4Copy( labelColor, valueColor );
	} else if ( highlighted ) {
		Vector4Copy( advancedAccentColor, labelColor );
		Vector4Copy( advancedTextColor, valueColor );
	} else {
		Vector4Copy( advancedMutedColor, labelColor );
		Vector4Copy( advancedTextColor, valueColor );
	}

	Frontend_DrawText( item->left + UI_FRONTEND_SPACE_SM,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		item->name, UI_LEFT | UI_SMALLFONT, labelColor );
	Frontend_DrawText( item->left + ADV_VALUE_OFFSET,
		item->top + ( item->bottom - item->top - SMALLCHAR_HEIGHT ) / 2,
		value, UI_LEFT | UI_SMALLFONT, valueColor );
}

static void AdvancedGraphics_DrawAction( void *self ) {
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

static void AdvancedGraphics_SetBounds( menucommon_s *item, int id,
	int x, int y, int width, int height, const char *label ) {
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

static void AdvancedGraphics_SetNavBounds( menutext_s *item, int id,
	const char *label, int y ) {
	item->generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
	item->generic.callback = UI_AdvancedGraphicsOptionsMenu_Event;
	item->string = (char *)label;
	item->style = UI_LEFT | UI_SMALLFONT;
	item->generic.ownerdraw = AdvancedGraphics_DrawNavItem;
	AdvancedGraphics_SetBounds( &item->generic, id, ADV_NAV_X + 16, y,
		ADV_NAV_WIDTH - 32, ADV_ROW_HEIGHT, NULL );
}

static void AdvancedGraphics_SetSettingBounds( menucommon_s *item, int id,
	const char *label, int x, int y ) {
	item->ownerdraw = AdvancedGraphics_DrawSetting;
	AdvancedGraphics_SetBounds( item, id, x, y, ADV_COLUMN_WIDTH,
		ADV_ROW_HEIGHT, label );
}

static void UI_AdvancedGraphicsOptionsMenu_Draw( void ) {
	Frontend_DrawBackground( advancedScrimColor );
	Frontend_DrawPanel( ADV_FRAME_X, ADV_FRAME_Y,
		ADV_FRAME_WIDTH, ADV_FRAME_HEIGHT, 1.0f,
		UI_FRONTEND_STYLE_FRAME );
	Frontend_DrawText( ADV_FRAME_X + 24, ADV_FRAME_Y + 24,
		"Advanced graphics", UI_LEFT | UI_BIGFONT, advancedTextColor );
	Frontend_DrawText( ADV_FRAME_X + 24, ADV_FRAME_Y + 48,
		"Tune lighting, shadows and post-processing",
		UI_LEFT | UI_SMALLFONT, advancedMutedColor );
	Frontend_DrawStatusChip( ADV_FRAME_X + ADV_FRAME_WIDTH - 104,
		ADV_FRAME_Y + 26, "Settings", advancedAccentColor, 1.0f );

	Frontend_DrawCard( ADV_NAV_X, ADV_NAV_Y,
		ADV_NAV_WIDTH, ADV_NAV_HEIGHT, 1.0f, qfalse );
	Frontend_DrawCard( ADV_DETAIL_X, ADV_DETAIL_Y,
		ADV_DETAIL_WIDTH, ADV_DETAIL_HEIGHT, 1.0f, qfalse );
	Frontend_DrawText( ADV_NAV_X + 16, ADV_NAV_Y + 22,
		"Parent", UI_LEFT | UI_SMALLFONT, advancedMutedColor );
	Frontend_DrawText( ADV_NAV_X + 16, ADV_NAV_Y + 54,
		"Graphics", UI_LEFT | UI_SMALLFONT, advancedTextColor );
	Frontend_DrawText( ADV_DETAIL_X + 16, ADV_DETAIL_Y + 22,
		"Lighting & effects", UI_LEFT | UI_SMALLFONT, advancedMutedColor );

	Menu_Draw( &advancedGraphicsOptionsInfo.menu );

	Frontend_DrawText( ADV_FRAME_X + 24, ADV_FRAME_Y + 384,
		"Select an option   Left / right adjust   Esc back",
		UI_LEFT | UI_SMALLFONT, advancedMutedColor );
}


static sfxHandle_t UI_AdvancedGraphicsOptionsMenu_Key( int key ) {
	if( key == K_MOUSE2 || key == K_ESCAPE ) {
		UI_PopMenu();
		return menu_out_sound;
	}

	return Menu_DefaultKey( &advancedGraphicsOptionsInfo.menu, key );
}

static void UI_AdvancedGraphicsOptionsMenu_Init( void ) {
	int y;

	memset( &advancedGraphicsOptionsInfo, 0, sizeof(advancedGraphicsOptionsInfo) );

	UI_AdvancedGraphicsOptionsMenu_Cache();
	advancedGraphicsOptionsInfo.menu.wrapAround = qtrue;
	advancedGraphicsOptionsInfo.menu.fullscreen = qtrue;
	advancedGraphicsOptionsInfo.menu.draw = UI_AdvancedGraphicsOptionsMenu_Draw;
	advancedGraphicsOptionsInfo.menu.key = UI_AdvancedGraphicsOptionsMenu_Key;

	advancedGraphicsOptionsInfo.banner.generic.type      = MTYPE_BTEXT;
	advancedGraphicsOptionsInfo.banner.generic.flags     = QMF_CENTER_JUSTIFY;
	advancedGraphicsOptionsInfo.banner.generic.x         = 320;
	advancedGraphicsOptionsInfo.banner.generic.y         = 16;
	advancedGraphicsOptionsInfo.banner.string            = "SYSTEM SETUP";
	advancedGraphicsOptionsInfo.banner.color             = color_white;
	advancedGraphicsOptionsInfo.banner.style             = UI_CENTER;

	advancedGraphicsOptionsInfo.graphics.generic.type        = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.graphics.generic.flags       = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	advancedGraphicsOptionsInfo.graphics.generic.id          = ID_GRAPHICS;
	advancedGraphicsOptionsInfo.graphics.generic.callback    = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.graphics.generic.x           = 216;
	advancedGraphicsOptionsInfo.graphics.generic.y           = 240 - 2 * PROP_HEIGHT;
	advancedGraphicsOptionsInfo.graphics.string              = "GRAPHICS";
	advancedGraphicsOptionsInfo.graphics.style               = UI_RIGHT;
	advancedGraphicsOptionsInfo.graphics.color               = text_color_normal;

	advancedGraphicsOptionsInfo.advanced_graphics.generic.type      = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.advanced_graphics.generic.flags     = QMF_RIGHT_JUSTIFY;
	advancedGraphicsOptionsInfo.advanced_graphics.generic.id        = ID_ADVANCED_GRAPHICS;
	advancedGraphicsOptionsInfo.advanced_graphics.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.advanced_graphics.generic.x         = 216;
	advancedGraphicsOptionsInfo.advanced_graphics.generic.y         = 240 - PROP_HEIGHT;
	advancedGraphicsOptionsInfo.advanced_graphics.string            = "ADVANCED GRAPHICS";
	advancedGraphicsOptionsInfo.advanced_graphics.style             = UI_RIGHT;
	advancedGraphicsOptionsInfo.advanced_graphics.color             = text_color_normal;

	advancedGraphicsOptionsInfo.display.generic.type        = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.display.generic.flags       = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	advancedGraphicsOptionsInfo.display.generic.id          = ID_DISPLAY;
	advancedGraphicsOptionsInfo.display.generic.callback    = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.display.generic.x           = 216;
	advancedGraphicsOptionsInfo.display.generic.y           = 240;
	advancedGraphicsOptionsInfo.display.string              = "DISPLAY";
	advancedGraphicsOptionsInfo.display.style               = UI_RIGHT;
	advancedGraphicsOptionsInfo.display.color               = text_color_normal;

	advancedGraphicsOptionsInfo.sound.generic.type      = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.sound.generic.flags     = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	advancedGraphicsOptionsInfo.sound.generic.id        = ID_SOUND;
	advancedGraphicsOptionsInfo.sound.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.sound.generic.x         = 216;
	advancedGraphicsOptionsInfo.sound.generic.y         = 240 + PROP_HEIGHT;
	advancedGraphicsOptionsInfo.sound.string            = "SOUND";
	advancedGraphicsOptionsInfo.sound.style             = UI_RIGHT;
	advancedGraphicsOptionsInfo.sound.color             = text_color_normal;

	advancedGraphicsOptionsInfo.network.generic.type        = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.network.generic.flags       = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	advancedGraphicsOptionsInfo.network.generic.id          = ID_NETWORK;
	advancedGraphicsOptionsInfo.network.generic.callback    = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.network.generic.x           = 216;
	advancedGraphicsOptionsInfo.network.generic.y           = 240 + 2 * PROP_HEIGHT;
	advancedGraphicsOptionsInfo.network.string              = "NETWORK";
	advancedGraphicsOptionsInfo.network.style               = UI_RIGHT;
	advancedGraphicsOptionsInfo.network.color               = text_color_normal;

	y = 240 - 7 * (BIGCHAR_HEIGHT + 2);
	advancedGraphicsOptionsInfo.preset.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.preset.generic.name      = "Quality Preset:";
	advancedGraphicsOptionsInfo.preset.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.preset.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.preset.generic.id        = ID_PRESET;
	advancedGraphicsOptionsInfo.preset.generic.x         = 400;
	advancedGraphicsOptionsInfo.preset.generic.y         = y;
	advancedGraphicsOptionsInfo.preset.itemnames         = preset_items;

	// extra spacing below preset so it reads as a separate group header
	y += 2 * (BIGCHAR_HEIGHT + 2);
	advancedGraphicsOptionsInfo.hdr.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.hdr.generic.name      = "HDR:";
	advancedGraphicsOptionsInfo.hdr.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.hdr.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.hdr.generic.id        = ID_HDR;
	advancedGraphicsOptionsInfo.hdr.generic.x         = 400;
	advancedGraphicsOptionsInfo.hdr.generic.y         = y;
	advancedGraphicsOptionsInfo.hdr.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.hdr.curvalue          = trap_Cvar_VariableValue( "r_hdr" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.postprocess.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.postprocess.generic.name      = "Post Process:";
	advancedGraphicsOptionsInfo.postprocess.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.postprocess.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.postprocess.generic.id        = ID_POSTPROCESS;
	advancedGraphicsOptionsInfo.postprocess.generic.x         = 400;
	advancedGraphicsOptionsInfo.postprocess.generic.y         = y;
	advancedGraphicsOptionsInfo.postprocess.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.postprocess.curvalue          = trap_Cvar_VariableValue( "r_postProcess" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.tonemap.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.tonemap.generic.name      = "Tone Mapping:";
	advancedGraphicsOptionsInfo.tonemap.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.tonemap.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.tonemap.generic.id        = ID_TONEMAP;
	advancedGraphicsOptionsInfo.tonemap.generic.x         = 400;
	advancedGraphicsOptionsInfo.tonemap.generic.y         = y;
	advancedGraphicsOptionsInfo.tonemap.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.tonemap.curvalue          = trap_Cvar_VariableValue( "r_toneMap" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.autoexposure.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.autoexposure.generic.name      = "Auto Exposure:";
	advancedGraphicsOptionsInfo.autoexposure.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.autoexposure.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.autoexposure.generic.id        = ID_AUTOEXPOSURE;
	advancedGraphicsOptionsInfo.autoexposure.generic.x         = 400;
	advancedGraphicsOptionsInfo.autoexposure.generic.y         = y;
	advancedGraphicsOptionsInfo.autoexposure.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.autoexposure.curvalue          = trap_Cvar_VariableValue( "r_autoExposure" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.ssao.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.ssao.generic.name      = "SSAO:";
	advancedGraphicsOptionsInfo.ssao.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.ssao.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.ssao.generic.id        = ID_SSAO;
	advancedGraphicsOptionsInfo.ssao.generic.x         = 400;
	advancedGraphicsOptionsInfo.ssao.generic.y         = y;
	advancedGraphicsOptionsInfo.ssao.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.ssao.curvalue          = trap_Cvar_VariableValue( "r_ssao" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.sunrays.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.sunrays.generic.name      = "Sun Rays:";
	advancedGraphicsOptionsInfo.sunrays.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.sunrays.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.sunrays.generic.id        = ID_SUNRAYS;
	advancedGraphicsOptionsInfo.sunrays.generic.x         = 400;
	advancedGraphicsOptionsInfo.sunrays.generic.y         = y;
	advancedGraphicsOptionsInfo.sunrays.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.sunrays.curvalue          = trap_Cvar_VariableValue( "r_drawSunRays" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.name      = "Dynamic Reflections:";
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.id        = ID_DYNAMIC_REFLECTIONS;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.x         = 400;
	advancedGraphicsOptionsInfo.dynamic_reflections.generic.y         = y;
	advancedGraphicsOptionsInfo.dynamic_reflections.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.dynamic_reflections.curvalue          = trap_Cvar_VariableValue( "r_dynamicReflections" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.sunshadows.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.sunshadows.generic.name      = "Sun Shadows:";
	advancedGraphicsOptionsInfo.sunshadows.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.sunshadows.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.sunshadows.generic.id        = ID_SUNSHADOWS;
	advancedGraphicsOptionsInfo.sunshadows.generic.x         = 400;
	advancedGraphicsOptionsInfo.sunshadows.generic.y         = y;
	advancedGraphicsOptionsInfo.sunshadows.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.sunshadows.curvalue          = trap_Cvar_VariableValue( "r_sunShadows" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.shadowquality.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.shadowquality.generic.name      = "Shadow Quality:";
	advancedGraphicsOptionsInfo.shadowquality.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.shadowquality.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.shadowquality.generic.id        = ID_SHADOWQUALITY;
	advancedGraphicsOptionsInfo.shadowquality.generic.x         = 400;
	advancedGraphicsOptionsInfo.shadowquality.generic.y         = y;
	advancedGraphicsOptionsInfo.shadowquality.itemnames         = shadow_quality_items;
	{
		int shadowMapSize = (int)trap_Cvar_VariableValue( "r_shadowMapSize" );
		if ( shadowMapSize >= 2048 )
			advancedGraphicsOptionsInfo.shadowquality.curvalue = 2;
		else if ( shadowMapSize >= 1024 )
			advancedGraphicsOptionsInfo.shadowquality.curvalue = 1;
		else
			advancedGraphicsOptionsInfo.shadowquality.curvalue = 0;
	}

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.shadowblur.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.shadowblur.generic.name      = "Shadow Blur:";
	advancedGraphicsOptionsInfo.shadowblur.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.shadowblur.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.shadowblur.generic.id        = ID_SHADOWBLUR;
	advancedGraphicsOptionsInfo.shadowblur.generic.x         = 400;
	advancedGraphicsOptionsInfo.shadowblur.generic.y         = y;
	advancedGraphicsOptionsInfo.shadowblur.itemnames         = enabled_items;
	advancedGraphicsOptionsInfo.shadowblur.curvalue          = trap_Cvar_VariableValue( "r_shadowBlur" ) != 0;

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.shadowcascade_near.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.shadowcascade_near.generic.name      = "Shadow Cascade Near:";
	advancedGraphicsOptionsInfo.shadowcascade_near.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.shadowcascade_near.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.shadowcascade_near.generic.id        = ID_SHADOWCASCADE_NEAR;
	advancedGraphicsOptionsInfo.shadowcascade_near.generic.x         = 400;
	advancedGraphicsOptionsInfo.shadowcascade_near.generic.y         = y;
	advancedGraphicsOptionsInfo.shadowcascade_near.itemnames         = shadow_cascade_near_items;
	{
		int nearValue = (int)trap_Cvar_VariableValue( "r_shadowCascadeZNear" );
		if ( nearValue >= 16 )
			advancedGraphicsOptionsInfo.shadowcascade_near.curvalue = 2;
		else if ( nearValue >= 8 )
			advancedGraphicsOptionsInfo.shadowcascade_near.curvalue = 1;
		else
			advancedGraphicsOptionsInfo.shadowcascade_near.curvalue = 0;
	}

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.shadowcascade_far.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.shadowcascade_far.generic.name      = "Shadow Cascade Far:";
	advancedGraphicsOptionsInfo.shadowcascade_far.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.shadowcascade_far.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.shadowcascade_far.generic.id        = ID_SHADOWCASCADE_FAR;
	advancedGraphicsOptionsInfo.shadowcascade_far.generic.x         = 400;
	advancedGraphicsOptionsInfo.shadowcascade_far.generic.y         = y;
	advancedGraphicsOptionsInfo.shadowcascade_far.itemnames         = shadow_cascade_far_items;
	{
		int farValue = (int)trap_Cvar_VariableValue( "r_shadowCascadeZFar" );
		if ( farValue >= 2048 )
			advancedGraphicsOptionsInfo.shadowcascade_far.curvalue = 2;
		else if ( farValue >= 1024 )
			advancedGraphicsOptionsInfo.shadowcascade_far.curvalue = 1;
		else
			advancedGraphicsOptionsInfo.shadowcascade_far.curvalue = 0;
	}

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.shadowcascade_bias.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.shadowcascade_bias.generic.name      = "Shadow Cascade Bias:";
	advancedGraphicsOptionsInfo.shadowcascade_bias.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.shadowcascade_bias.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.shadowcascade_bias.generic.id        = ID_SHADOWCASCADE_BIAS;
	advancedGraphicsOptionsInfo.shadowcascade_bias.generic.x         = 400;
	advancedGraphicsOptionsInfo.shadowcascade_bias.generic.y         = y;
	advancedGraphicsOptionsInfo.shadowcascade_bias.itemnames         = shadow_cascade_bias_items;
	{
		float biasValue = trap_Cvar_VariableValue( "r_shadowCascadeZBias" );
		if ( biasValue >= 0.75f )
			advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue = 3;
		else if ( biasValue >= 0.375f )
			advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue = 2;
		else if ( biasValue >= 0.125f )
			advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue = 1;
		else
			advancedGraphicsOptionsInfo.shadowcascade_bias.curvalue = 0;
	}

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.pshadowdist.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.pshadowdist.generic.name      = "Projected Shadow Dist:";
	advancedGraphicsOptionsInfo.pshadowdist.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.pshadowdist.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.pshadowdist.generic.id        = ID_PSHADOWDIST;
	advancedGraphicsOptionsInfo.pshadowdist.generic.x         = 400;
	advancedGraphicsOptionsInfo.pshadowdist.generic.y         = y;
	advancedGraphicsOptionsInfo.pshadowdist.itemnames         = pshadow_dist_items;
	{
		int pshadowDist = (int)trap_Cvar_VariableValue( "r_pshadowDist" );
		if ( pshadowDist >= 256 )
			advancedGraphicsOptionsInfo.pshadowdist.curvalue = 2;
		else if ( pshadowDist >= 128 )
			advancedGraphicsOptionsInfo.pshadowdist.curvalue = 1;
		else
			advancedGraphicsOptionsInfo.pshadowdist.curvalue = 0;
	}

	y += BIGCHAR_HEIGHT + 2;
	advancedGraphicsOptionsInfo.sunlightmode.generic.type      = MTYPE_SPINCONTROL;
	advancedGraphicsOptionsInfo.sunlightmode.generic.name      = "Sunlight Mode:";
	advancedGraphicsOptionsInfo.sunlightmode.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	advancedGraphicsOptionsInfo.sunlightmode.generic.callback  = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.sunlightmode.generic.id        = ID_SUNLIGHTMODE;
	advancedGraphicsOptionsInfo.sunlightmode.generic.x         = 400;
	advancedGraphicsOptionsInfo.sunlightmode.generic.y         = y;
	advancedGraphicsOptionsInfo.sunlightmode.itemnames         = sunlight_mode_items;
	advancedGraphicsOptionsInfo.sunlightmode.curvalue          = Com_Clamp( 0, 2, (int)trap_Cvar_VariableValue( "r_sunlightMode" ) );
	UI_AdvancedGraphicsOptionsMenu_SyncPreset();

	advancedGraphicsOptionsInfo.back.generic.type         = MTYPE_PTEXT;
	advancedGraphicsOptionsInfo.back.generic.flags        = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	advancedGraphicsOptionsInfo.back.generic.x            = 20;
	advancedGraphicsOptionsInfo.back.generic.y            = 480 - 50;
	advancedGraphicsOptionsInfo.back.generic.id           = ID_BACK;
	advancedGraphicsOptionsInfo.back.generic.callback     = UI_AdvancedGraphicsOptionsMenu_Event;
	advancedGraphicsOptionsInfo.back.string               = "< BACK";
	advancedGraphicsOptionsInfo.back.color                = text_color_normal;
	advancedGraphicsOptionsInfo.back.style                = UI_LEFT | UI_SMALLFONT;

	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.preset );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.hdr );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.postprocess );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.tonemap );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.autoexposure );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.ssao );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.sunrays );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.dynamic_reflections );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.sunshadows );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.shadowquality );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.shadowblur );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.shadowcascade_near );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.shadowcascade_far );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.shadowcascade_bias );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.pshadowdist );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.sunlightmode );
	Menu_AddItem( &advancedGraphicsOptionsInfo.menu, ( void * ) &advancedGraphicsOptionsInfo.back );

	/* Menu_AddItem initializes the legacy widgets and overwrites their
	 * default bounds. Apply the frontend layout after that initialization so
	 * the custom ownerdraw positions are the ones used for drawing and input. */
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.preset.generic,
		ID_PRESET, "Preset", ADV_COLUMN_LEFT_X, ADV_ROW_START_Y );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.hdr.generic,
		ID_HDR, "HDR", ADV_COLUMN_LEFT_X,
		ADV_ROW_START_Y + ADV_ROW_HEIGHT + ADV_ROW_GAP );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.postprocess.generic,
		ID_POSTPROCESS, "Post process", ADV_COLUMN_LEFT_X,
		ADV_ROW_START_Y + 2 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.tonemap.generic,
		ID_TONEMAP, "Tone map", ADV_COLUMN_LEFT_X,
		ADV_ROW_START_Y + 3 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.autoexposure.generic,
		ID_AUTOEXPOSURE, "Auto exposure", ADV_COLUMN_LEFT_X,
		ADV_ROW_START_Y + 4 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.ssao.generic,
		ID_SSAO, "SSAO", ADV_COLUMN_LEFT_X,
		ADV_ROW_START_Y + 5 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.sunrays.generic,
		ID_SUNRAYS, "Sun rays", ADV_COLUMN_LEFT_X,
		ADV_ROW_START_Y + 6 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.dynamic_reflections.generic,
		ID_DYNAMIC_REFLECTIONS, "Dynamic refl.", ADV_COLUMN_LEFT_X,
		ADV_ROW_START_Y + 7 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );

	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.sunshadows.generic,
		ID_SUNSHADOWS, "Sun shadows", ADV_COLUMN_RIGHT_X, ADV_ROW_START_Y );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.shadowquality.generic,
		ID_SHADOWQUALITY, "Shadow quality", ADV_COLUMN_RIGHT_X,
		ADV_ROW_START_Y + ADV_ROW_HEIGHT + ADV_ROW_GAP );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.shadowblur.generic,
		ID_SHADOWBLUR, "Shadow blur", ADV_COLUMN_RIGHT_X,
		ADV_ROW_START_Y + 2 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.shadowcascade_near.generic,
		ID_SHADOWCASCADE_NEAR, "Cascade near", ADV_COLUMN_RIGHT_X,
		ADV_ROW_START_Y + 3 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.shadowcascade_far.generic,
		ID_SHADOWCASCADE_FAR, "Cascade far", ADV_COLUMN_RIGHT_X,
		ADV_ROW_START_Y + 4 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.shadowcascade_bias.generic,
		ID_SHADOWCASCADE_BIAS, "Cascade bias", ADV_COLUMN_RIGHT_X,
		ADV_ROW_START_Y + 5 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.pshadowdist.generic,
		ID_PSHADOWDIST, "Projected shadow", ADV_COLUMN_RIGHT_X,
		ADV_ROW_START_Y + 6 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );
	AdvancedGraphics_SetSettingBounds( &advancedGraphicsOptionsInfo.sunlightmode.generic,
		ID_SUNLIGHTMODE, "Sunlight mode", ADV_COLUMN_RIGHT_X,
		ADV_ROW_START_Y + 7 * ( ADV_ROW_HEIGHT + ADV_ROW_GAP ) );

	AdvancedGraphics_SetBounds( &advancedGraphicsOptionsInfo.back.generic, ID_BACK,
		ADV_NAV_X, ADV_ACTION_Y, ADV_ACTION_WIDTH, ADV_ACTION_HEIGHT, NULL );
	advancedGraphicsOptionsInfo.back.generic.ownerdraw = AdvancedGraphics_DrawAction;
}

void UI_AdvancedGraphicsOptionsMenu_Cache( void ) {
}

void UI_AdvancedGraphicsOptionsMenu( void ) {
// STONELANCE FIXME: get rid of this after proper tansitions are added
	uis.transitionIn = 0;
// END
	UI_AdvancedGraphicsOptionsMenu_Init();
	UI_PushMenu( &advancedGraphicsOptionsInfo.menu );
	Menu_SetCursorToItem( &advancedGraphicsOptionsInfo.menu, &advancedGraphicsOptionsInfo.hdr );
}
