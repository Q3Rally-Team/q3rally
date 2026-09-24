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
//
// cg_draw.c -- draw all of the graphical elements during
// active (after loading) gameplay

#include "cg_local.h"
#include "cg_hud_elements.h"

#ifdef MISSIONPACK
#include "../ui/ui_shared.h"

// used for scoreboard
extern displayContextDef_t cgDC;
menuDef_t *menuScoreboard = NULL;
#else
int drawTeamOverlayModificationCount = -1;
#endif

int sortedTeamPlayers[TEAM_MAXOVERLAY];
int	numSortedTeamPlayers;

char systemChat[256];
char teamChat1[256];
char teamChat2[256];

static float CG_DrawRallyPowerups( float y );

#ifdef MISSIONPACK

int CG_Text_Width(const char *text, float scale, int limit) {
  int count,len;
	float out;
	glyphInfo_t *glyph;
	float useScale;
	const char *s = text;
	fontInfo_t *font = &cgDC.Assets.textFont;
	if (scale <= cg_smallFont.value) {
		font = &cgDC.Assets.smallFont;
	} else if (scale > cg_bigFont.value) {
		font = &cgDC.Assets.bigFont;
	}
	useScale = scale * font->glyphScale;
  out = 0;
  if (text) {
    len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[*s & 255];
				out += glyph->xSkip;
				s++;
				count++;
			}
    }
  }
  return out * useScale;
	}

int CG_Text_Height(const char *text, float scale, int limit) {
  int len, count;
	float max;
	glyphInfo_t *glyph;
	float useScale;
	const char *s = text;
	fontInfo_t *font = &cgDC.Assets.textFont;
	if (scale <= cg_smallFont.value) {
		font = &cgDC.Assets.smallFont;
	} else if (scale > cg_bigFont.value) {
		font = &cgDC.Assets.bigFont;
	}
	useScale = scale * font->glyphScale;
  max = 0;
  if (text) {
    len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[*s & 255];
	      if (max < glyph->height) {
		      max = glyph->height;
			  }
				s++;
				count++;
			}
    }
  }
  return max * useScale;
	}

void CG_Text_PaintChar(float x, float y, float width, float height, float scale, float s, float t, float s2, float t2, qhandle_t hShader) {
  float w, h;
  w = width * scale;
  h = height * scale;
  CG_AdjustFrom640( &x, &y, &w, &h );
  trap_R_DrawStretchPic( x, y, w, h, s, t, s2, t2, hShader );
}

void CG_Text_Paint(float x, float y, float scale, vec4_t color, const char *text, float adjust, int limit, int style) {
  int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph;
	float useScale;
	fontInfo_t *font = &cgDC.Assets.textFont;
	if (scale <= cg_smallFont.value) {
		font = &cgDC.Assets.smallFont;
	} else if (scale > cg_bigFont.value) {
		font = &cgDC.Assets.bigFont;
	}
	useScale = scale * font->glyphScale;
  if (text) {
		const char *s = text;
		trap_R_SetColor( color );
		memcpy(&newColor[0], &color[0], sizeof(vec4_t));
    len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			glyph = &font->glyphs[*s & 255];
      //int yadj = Assets.textFont.glyphs[text[i]].bottom + Assets.textFont.glyphs[text[i]].top;
      //float yadj = scale * (Assets.textFont.glyphs[text[i]].imageHeight - Assets.textFont.glyphs[text[i]].height);
			if ( Q_IsColorString( s ) ) {
				memcpy( newColor, g_color_table[ColorIndex(*(s+1))], sizeof( newColor ) );
				newColor[3] = color[3];
				trap_R_SetColor( newColor );
				s += 2;
				continue;
			} else {
				float yadj = useScale * glyph->top;
				if (style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE) {
					int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
					colorBlack[3] = newColor[3];
					trap_R_SetColor( colorBlack );
					CG_Text_PaintChar(x + ofs, y - yadj + ofs, 
														glyph->imageWidth,
														glyph->imageHeight,
														useScale, 
														glyph->s,
														glyph->t,
														glyph->s2,
														glyph->t2,
														glyph->glyph);
					colorBlack[3] = 1.0;
					trap_R_SetColor( newColor );
				}
				CG_Text_PaintChar(x, y - yadj, 
													glyph->imageWidth,
													glyph->imageHeight,
													useScale, 
													glyph->s,
													glyph->t,
													glyph->s2,
													glyph->t2,
													glyph->glyph);
				// CG_DrawPic(x, y - yadj, scale * cgDC.Assets.textFont.glyphs[text[i]].imageWidth, scale * cgDC.Assets.textFont.glyphs[text[i]].imageHeight, cgDC.Assets.textFont.glyphs[text[i]].glyph);
				x += (glyph->xSkip * useScale) + adjust;
				s++;
				count++;
			}
    }
	  trap_R_SetColor( NULL );
  }
}


#endif

/*
==============================================================
CG_DrawField - Draws large numbers for status bar and powerups
==============================================================
*/
#ifndef MISSIONPACK
static void CG_DrawField (int x, int y, int width, int value) {
	char	num[16], *ptr;
	int		l;
	int		frame;

	if ( width < 1 ) {
		return;
	}

	// draw number string
	if ( width > 5 ) {
		width = 5;
	}

	switch ( width ) {
	case 1:
		value = value > 9 ? 9 : value;
		value = value < 0 ? 0 : value;
		break;
	case 2:
		value = value > 99 ? 99 : value;
		value = value < -9 ? -9 : value;
		break;
	case 3:
		value = value > 999 ? 999 : value;
		value = value < -99 ? -99 : value;
		break;
	case 4:
		value = value > 9999 ? 9999 : value;
		value = value < -999 ? -999 : value;
		break;
	}

	Com_sprintf (num, sizeof(num), "%i", value);
	l = strlen(num);
	if (l > width)
		l = width;
	x += 2 + CHAR_WIDTH*(width - l);

	// center x, move y to bottom.
	x += (1.0f-cg_statusScale.value)*l*CHAR_WIDTH*0.5f;
	y += (1.0f-cg_statusScale.value)*CHAR_HEIGHT;

	ptr = num;
	while (*ptr && l)
	{
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr -'0';

		CG_DrawPic( x,y, CHAR_WIDTH*cg_statusScale.value, CHAR_HEIGHT*cg_statusScale.value, cgs.media.numberShaders[frame] );
		x += CHAR_WIDTH*cg_statusScale.value;
		ptr++;
		l--;
	}
}
#endif // MISSIONPACK

/*
==============
CG_Draw3DModel
==============
*/
void CG_Draw3DModel( float x, float y, float w, float h, qhandle_t model, qhandle_t skin, vec3_t origin, vec3_t angles ) {
	refdef_t		refdef;
	refEntity_t		ent;

	if ( !cg_draw3dIcons.integer || !cg_drawIcons.integer ) {
		return;
	}

	CG_AdjustFrom640( &x, &y, &w, &h );

	memset( &refdef, 0, sizeof( refdef ) );

	memset( &ent, 0, sizeof( ent ) );
	AnglesToAxis( angles, ent.axis );
	VectorCopy( origin, ent.origin );
	ent.hModel = model;
	ent.customSkin = skin;
	ent.renderfx = RF_NOSHADOW;		// no stencil shadows

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	refdef.fov_x = 30;
	refdef.fov_y = 30;

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.time = cg.time;

	trap_R_ClearScene();
	trap_R_AddRefEntityToScene( &ent );
	trap_R_RenderScene( &refdef );
}

/*
==================
CG_DrawHealthModel
==================
*/
void CG_DrawHealthModel( float x, float y, float w, float h, qhandle_t model, qhandle_t skin, qhandle_t model2, vec3_t origin, vec3_t angles, float yaw2 ) {
	refdef_t		refdef;
	refEntity_t		ent;

	if ( !cg_draw3dIcons.integer || !cg_drawIcons.integer ) {
		return;
	}

	CG_AdjustFrom640( &x, &y, &w, &h );

	memset( &refdef, 0, sizeof( refdef ) );

	memset( &ent, 0, sizeof( ent ) );
	AnglesToAxis( angles, ent.axis );
	VectorCopy( origin, ent.origin );
	ent.hModel = model;
	ent.customSkin = skin;
	ent.renderfx = RF_NOSHADOW;		// no stencil shadows

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	refdef.fov_x = 30;
	refdef.fov_y = 30;

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.time = cg.time;

	trap_R_ClearScene();
	trap_R_AddRefEntityToScene( &ent );

	if ( model2 ) {
		ent.hModel = model2;
		angles[YAW] = yaw2;
		AnglesToAxis( angles, ent.axis );
		trap_R_AddRefEntityToScene( &ent );
	}

	trap_R_RenderScene( &refdef );
}

/*
==================================================================
CG_DrawFlagModel - Used for both the status bar and the scoreboard
==================================================================
*/
void CG_DrawFlagModel( float x, float y, float w, float h, int team, qboolean force2D ) {
	qhandle_t		cm;
	float			len;
	vec3_t			origin, angles;
	vec3_t			mins, maxs;
	qhandle_t		handle;

	if ( !force2D && cg_draw3dIcons.integer ) {

		VectorClear( angles );

		cm = cgs.media.redFlagModel;

		// offset the origin y and z to center the flag
		trap_R_ModelBounds( cm, mins, maxs );

		origin[2] = -0.5 * ( mins[2] + maxs[2] );
		origin[1] = 0.5 * ( mins[1] + maxs[1] );

		// calculate distance so the flag nearly fills the box
		// assume heads are taller than wide
		len = 0.5 * ( maxs[2] - mins[2] );		
		origin[0] = len / 0.268;	// len / tan( fov/2 )

		angles[YAW] = 60 * sin( cg.time / 2000.0 );;

		if( team == TEAM_RED ) {
			handle = cgs.media.redFlagModel;
		} else if( team == TEAM_BLUE ) {
			handle = cgs.media.blueFlagModel;
		} else if( team == TEAM_GREEN ) {
			handle = cgs.media.greenFlagModel;
		} else if( team == TEAM_YELLOW ) {
			handle = cgs.media.yellowFlagModel;
		} else if( team == TEAM_FREE ) {
			handle = cgs.media.neutralFlagModel;
		} else {
			return;
		}
		CG_Draw3DModel( x, y, w, h, handle, 0, origin, angles );
	} else if ( cg_drawIcons.integer ) {
		gitem_t *item;

		if( team == TEAM_RED ) {
			item = BG_FindItemForPowerup( PW_REDFLAG );
		} else if( team == TEAM_BLUE ) {
			item = BG_FindItemForPowerup( PW_BLUEFLAG );
		} else if( team == TEAM_GREEN ) {
			item = BG_FindItemForPowerup( PW_GREENFLAG );
		} else if( team == TEAM_YELLOW ) {
			item = BG_FindItemForPowerup( PW_YELLOWFLAG );
		} else if( team == TEAM_FREE ) {
			item = BG_FindItemForPowerup( PW_NEUTRALFLAG );
		} else {
			return;
		}
		if (item) {
		  CG_DrawPic( x, y, w, h, cg_items[ ITEM_INDEX(item) ].icon );
		}
	}
}

/*
====================
CG_DrawStatusBarHead
====================
*/
#ifndef MISSIONPACK
#if 0 // ZTM: Not used by Q3Rally
static void CG_DrawStatusBarHead( float x ) {
	vec3_t		angles;
	float		size, stretch;
	float		frac;

	VectorClear( angles );

	if ( cg.damageTime && cg.time - cg.damageTime < DAMAGE_TIME ) {
		frac = (float)(cg.time - cg.damageTime ) / DAMAGE_TIME;
		size = ICON_SIZE * 1.25 * ( 1.5 - frac * 0.5 );

		stretch = size - ICON_SIZE * 1.25;
		// kick in the direction of damage
		x -= stretch * 0.5 + cg.damageX * stretch * 0.5;

		cg.headStartYaw = 180 + cg.damageX * 45;

		cg.headEndYaw = 180 + 20 * cos( crandom()*M_PI );
		cg.headEndPitch = 5 * cos( crandom()*M_PI );

		cg.headStartTime = cg.time;
		cg.headEndTime = cg.time + 100 + random() * 2000;
	} else {
		if ( cg.time >= cg.headEndTime ) {
			// select a new head angle
			cg.headStartYaw = cg.headEndYaw;
			cg.headStartPitch = cg.headEndPitch;
			cg.headStartTime = cg.headEndTime;
			cg.headEndTime = cg.time + 100 + random() * 2000;

			cg.headEndYaw = 180 + 20 * cos( crandom()*M_PI );
			cg.headEndPitch = 5 * cos( crandom()*M_PI );
		}

		size = ICON_SIZE * 1.25;
	}

	// if the server was frozen for a while we may have a bad head start time
	if ( cg.headStartTime > cg.time ) {
		cg.headStartTime = cg.time;
	}

	frac = ( cg.time - cg.headStartTime ) / (float)( cg.headEndTime - cg.headStartTime );
	frac = frac * frac * ( 3 - 2 * frac );
	angles[YAW] = cg.headStartYaw + ( cg.headEndYaw - cg.headStartYaw ) * frac;
	angles[PITCH] = cg.headStartPitch + ( cg.headEndPitch - cg.headStartPitch ) * frac;

}
#endif
#endif // MISSIONPACK

/*
====================
CG_DrawStatusBarFlag
====================
*/
#ifndef MISSIONPACK
static void CG_DrawStatusBarFlag( float x, int team ) {
	int iconSize = ICON_SIZE*cg_statusScale.value;
	CG_DrawFlagModel( x+(1.0f-cg_statusScale.value)*ICON_SIZE*0.5f, 480 - iconSize, iconSize, iconSize, team, qfalse );
}
#endif // MISSIONPACK

/*
=====================
CG_DrawTeamBackground
=====================
*/
void CG_DrawTeamBackground( int x, int y, int w, int h, float alpha, int team )
{
	vec4_t		hcolor;

	hcolor[3] = alpha;
	if ( team == TEAM_RED ) {
		hcolor[0] = 1;
		hcolor[1] = 0;
		hcolor[2] = 0;
	} else if ( team == TEAM_BLUE ) {
		hcolor[0] = 0;
		hcolor[1] = 0;
		hcolor[2] = 1;
	}
// Q3Rally Code Start
	else if ( team == TEAM_GREEN ) {
		hcolor[0] = 0;
		hcolor[1] = 1;
		hcolor[2] = 0;
	}
	else if ( team == TEAM_YELLOW ) {
		hcolor[0] = 1;
		hcolor[1] = 1;
		hcolor[2] = 0;
	}
// END
	else {
		return;
	}
	trap_R_SetColor( hcolor );
    CG_SetScreenPlacement(PLACE_STRETCH, CG_GetScreenVerticalPlacement());
	CG_DrawPic( x, y, w, h, cgs.media.teamStatusBar );
    CG_PopScreenPlacement();
	trap_R_SetColor( NULL );
}

// Q3Rally Code Start - KOTH
/*
===================
CG_DrawKOTH_HillStatus

Draws the KOTH hill status panel:
  - Owner name / Neutral / Contested
  - Capture progress bar
Called from cg_hud_core.c GT_KOTH case.
===================
*/
void CG_DrawKOTH_HillStatus( void ) {
	float		x, y, w, h, barW;
	const char	*statusText;
	vec4_t		statusColor;
	vec4_t		barBgColor;
	char		captureText[24];
	int		capturePct;

	if ( cgs.gametype != GT_KOTH ) return;

	CG_SetScreenPlacement( PLACE_CENTER, PLACE_BOTTOM );
	x = 190.0f;
	y = 431.0f;
	w = 104.0f;
	h = 4.0f;
	capturePct = cgs.kothCapturePct;
	if ( capturePct < 0 ) capturePct = 0;
	if ( capturePct > 100 ) capturePct = 100;
	Q_strncpyz( captureText, "HILL NEUTRAL", sizeof( captureText ) );

	if ( cgs.kothContested ) {
		statusText = "CONTESTED";
		Vector4Copy( colorYellow, statusColor );
	} else if ( capturePct > 0 && capturePct < 100 ) {
		Com_sprintf( captureText, sizeof( captureText ), "CAPTURING %d%%", capturePct );
		statusText = captureText;
		Vector4Copy( colorYellow, statusColor );
	} else if ( cgs.kothOwner == TEAM_RED ) {
		statusText = "RED HOLDS HILL";
		Vector4Copy( colorRed, statusColor );
	} else if ( cgs.kothOwner == TEAM_BLUE ) {
		statusText = "BLUE HOLDS HILL";
		Vector4Copy( colorBlue, statusColor );
	} else {
		statusText = "HILL NEUTRAL";
		Vector4Copy( colorWhite, statusColor );
	}

	CG_DrawIngameString( (int)x, (int)y, statusText,
	                      UI_SMALLFONT, 0.43f, statusColor );

	barBgColor[0] = 0.075f;
	barBgColor[1] = 0.095f;
	barBgColor[2] = 0.105f;
	barBgColor[3] = 0.92f;
	CG_FillRect( x, y + 13.0f, w, h, barBgColor );
	if ( !cgs.kothContested && capturePct > 0 ) {
		barW = w * ( capturePct / 100.0f );
		CG_FillRect( x, y + 13.0f, barW, h, statusColor );
	}

	CG_PopScreenPlacement();
}
static void CG_DrawKOTH_RespawnWave_Internal( void ) {
	int waveMs;
	int respawnAt;
	int remainingMs;
	char msg[32];
	vec4_t panelColor = { 0.008f, 0.012f, 0.016f, 0.78f };
	vec4_t lineColor = { 0.24f, 0.34f, 0.36f, 0.85f };
	vec4_t accentColor = { 0.72f, 1.00f, 0.06f, 1.00f };

	if ( cgs.gametype != GT_KOTH ) return;
	if ( !cg_hudShowKothRespawnWave.integer ) return;
	if ( cg.snap->ps.stats[STAT_HEALTH] > 0 ) return;
	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) return;

	waveMs = cg_kothRespawnWave.integer;
	if ( waveMs <= 0 ) return;
	if ( waveMs < 1000 ) {
		waveMs *= 1000;
	}

	respawnAt = cg.kothRespawnAt;
	if ( respawnAt <= 0 ) {
		int deathBase = cg.kothDeathTime;
		if ( deathBase <= 0 ) {
			deathBase = cg.snap->serverTime;
			cg.kothDeathTime = deathBase;
		}

		// Server gates KOTH respawn by: respawnTime = deathTime + 1700, then align to wave
		respawnAt = ( ( deathBase + 1700 + waveMs - 1 ) / waveMs ) * waveMs;
		cg.kothRespawnAt = respawnAt;
	}

	remainingMs = respawnAt - cg.time;
	if ( remainingMs < 0 ) remainingMs = 0;

	Com_sprintf( msg, sizeof(msg), "RESPAWN IN %.1fS", remainingMs / 1000.0f );
	CG_SetScreenPlacement( PLACE_CENTER, PLACE_TOP );
	CG_FillRect( 238, 382, 164, 24, panelColor );
	CG_FillRect( 238, 382, 164, 2, accentColor );
	CG_DrawRect( 238, 382, 164, 24, 1.0f, lineColor );
	CG_DrawIngameString( 320, 390, msg, UI_CENTER | UI_SMALLFONT,
	                      0.70f, colorWhite );
	CG_PopScreenPlacement();
}

static void CG_DrawKOTH_LossFlash( void ) {
	vec4_t flashColor;
	float remain;

	if ( cgs.gametype != GT_KOTH ) {
		return;
	}

	if ( cg.kothLossFlashUntil <= cg.time ) {
		return;
	}

	remain = ( cg.kothLossFlashUntil - cg.time ) / 600.0f;
	if ( remain < 0.0f ) {
		remain = 0.0f;
	}

	flashColor[0] = 0.9f;
	flashColor[1] = 0.1f;
	flashColor[2] = 0.1f;
	flashColor[3] = 0.25f * remain;

	CG_SetScreenPlacement( PLACE_CENTER, PLACE_CENTER );
	CG_FillRect( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, flashColor );
	CG_PopScreenPlacement();
}
// Q3Rally Code END - KOTH

/*
===============
CG_DrawSigilHUD
===============
*/

void CG_DrawSigilHUD( void ) {

                      int i, x=440, y=0;
                      for (i=0; i<MAX_SIGILS; i++) {
                      switch ( cgs.sigil[i] )
                      
                {

                case SIGIL_ISRED:
                    CG_DrawPic( x, y, 18, 18, cgs.media.redsigilShader );
                    break;
        
                case SIGIL_ISBLUE:
                    CG_DrawPic( x, y, 18, 18, cgs.media.bluesigilShader );
                    break;
                  
                case SIGIL_ISGREEN:
                    CG_DrawPic( x, y, 18, 18, cgs.media.greensigilShader );
                    break;
                    
                case SIGIL_ISYELLOW:
                    CG_DrawPic( x, y, 18, 18, cgs.media.yellowsigilShader );
                    break;
                
                case SIGIL_ISWHITE:
                    CG_DrawPic( x, y, 18, 18, cgs.media.sigilShader );
                    break;
                    
                case SIGIL_NONE:
                    break;
                }
                
              x+= 19;
            }
}

/*
================
CG_DrawStatusBar
================
*/
#ifndef MISSIONPACK
#if 0
static void CG_DrawStatusBar( void ) {
	int			color;
	centity_t	*cent;
	playerState_t	*ps;
	int			value;
	vec4_t		hcolor;
	vec3_t		angles;
	vec3_t		origin;
    float		scale, iconSize;

	static float colors[4][4] = { 
//		{ 0.2, 1.0, 0.2, 1.0 } , { 1.0, 0.2, 0.2, 1.0 }, {0.5, 0.5, 0.5, 1} };
		{ 1.0f, 0.69f, 0.0f, 1.0f },    // normal
		{ 1.0f, 0.2f, 0.2f, 1.0f },     // low health
		{ 0.5f, 0.5f, 0.5f, 1.0f },     // weapon firing
		{ 1.0f, 1.0f, 1.0f, 1.0f } };   // health > 100

	if ( cg_drawStatus.integer == 0 ) {
		return;
	}

	CG_SetScreenPlacement(PLACE_CENTER, PLACE_BOTTOM);

	scale = Com_Clamp( 0.1f, 2, cg_statusScale.value);

	iconSize = scale * ICON_SIZE;

	// draw the team background
	CG_DrawTeamBackground( 0, 480 - 60*scale, 640, 60*scale, 0.33f, cg.snap->ps.persistant[PERS_TEAM] );

	cent = &cg_entities[cg.snap->ps.clientNum];
	ps = &cg.snap->ps;

	VectorClear( angles );

	// draw any 3D icons first, so the changes back to 2D are minimized
	if ( cent->currentState.weapon && cg_weapons[ cent->currentState.weapon ].ammoModel ) {
		origin[0] = 70;
		origin[1] = 0;
		origin[2] = 0;
		angles[YAW] = 90 + 20 * sin( cg.time / 1000.0 );
		CG_Draw3DModel( CHAR_WIDTH*3 + TEXT_ICON_SPACE, 480-iconSize, iconSize, iconSize,
					   cg_weapons[ cent->currentState.weapon ].ammoModel, 0, origin, angles );
	}

		if ( cg_drawStatusHead.integer == 2 ) {
		origin[0] = 60;
		origin[1] = 0;
		origin[2] = -5;
		angles[YAW] = ( cg.time & 2047 ) * 360 / 4096.0;
		CG_DrawHealthModel( 185 + CHAR_WIDTH*3 + TEXT_ICON_SPACE, 480-iconSize, iconSize, iconSize,
					   cg_items[ 6 /*item_health_large*/].models[0], 0, cg_items[ 6 /*item_health_large*/].models[1], origin, angles, 0 );

		// if we didn't draw a 3D icon, draw a 2D icon for health
		if ( !cg_draw3dIcons.integer && cg_drawIcons.integer ) {
			CG_DrawPic( 185 + CHAR_WIDTH*3 + TEXT_ICON_SPACE, 480 - iconSize, iconSize, iconSize, cg_items[6/*item_health_large*/].icon );
		}
	}
	else if ( cg_drawStatusHead.integer == 1 )
		CG_DrawStatusBarHead( 185 + CHAR_WIDTH*3 + TEXT_ICON_SPACE );

	if( cg.predictedPlayerState.powerups[PW_REDFLAG] ) {
		CG_DrawStatusBarFlag( 185 + CHAR_WIDTH*3 + TEXT_ICON_SPACE + ICON_SIZE, TEAM_RED );
	} else if( cg.predictedPlayerState.powerups[PW_BLUEFLAG] ) {
		CG_DrawStatusBarFlag( 185 + CHAR_WIDTH*3 + TEXT_ICON_SPACE + ICON_SIZE, TEAM_BLUE );
	} else if( cg.predictedPlayerState.powerups[PW_NEUTRALFLAG] ) {
		CG_DrawStatusBarFlag( 185 + CHAR_WIDTH*3 + TEXT_ICON_SPACE + ICON_SIZE, TEAM_FREE );
	}

	if ( ps->stats[ STAT_ARMOR ] ) {
		origin[0] = 90;
		origin[1] = 0;
		origin[2] = -10;
		angles[YAW] = ( cg.time & 2047 ) * 360 / 2048.0;
        CG_Draw3DModel( 370 + CHAR_WIDTH*3 + TEXT_ICON_SPACE, 480 - iconSize, iconSize, iconSize,
					   cgs.media.armorModel, 0, origin, angles );
	}
	//
	// ammo
	//
	if ( cent->currentState.weapon ) {
		value = ps->ammo[cent->currentState.weapon];
		if ( value > -1 ) {
			if ( cg.predictedPlayerState.weaponstate == WEAPON_FIRING

				&& (cg.predictedPlayerState.weaponTime & NORMAL_WEAPON_TIME_MASK) > 100 ) {

				// draw as dark grey when reloading
				color = 2;	// dark grey
			} else {
				if ( value >= 0 ) {
					color = 0;	// green
				} else {
					color = 1;	// red
				}
			}
			trap_R_SetColor( colors[color] );
			
			CG_DrawField (0, 432, 3, value);
			trap_R_SetColor( NULL );

			// if we didn't draw a 3D icon, draw a 2D icon for ammo
			if ( !cg_draw3dIcons.integer && cg_drawIcons.integer ) {
				qhandle_t	icon;

				icon = cg_weapons[ cg.predictedPlayerState.weapon ].ammoIcon;
				if ( icon ) {
					CG_DrawPic( CHAR_WIDTH*3 + TEXT_ICON_SPACE, 480 - iconSize, iconSize, iconSize, icon );
				}
			}
		}

		// draw green
		if (cgs.gametype == GT_CTF4) {
			color[0] = 0.0f;
			color[1] = 1.0f;
			color[2] = 0.0f;
			color[3] = 0.33f;
			s = va( "%2i", s3 );
			w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
			x -= w;
			CG_FillRect( x+1, y+1, w-2, 16, color );
			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_GREEN ) {
				CG_DrawPic( x+1, y+1, w-2, 16, cgs.media.selectShader );
			}
			CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

			if( cgs.greenflag >= 0 && cgs.greenflag <= 2 ) {
				item = BG_FindItemForPowerup( PW_GREENFLAG );
				if (item) {
					y1 = y + TINYCHAR_HEIGHT + 8;
					CG_DrawPic( x+1, y1+1, w-2, 16, cgs.media.greenFlagShader[cgs.greenflag] );
				}
			}
		}

		// draw yellow
		if (cgs.gametype == GT_CTF4) {
			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 0.0f;
			color[3] = 0.33f;
			s = va( "%2i", s4 );
			w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
			x -= w;
			CG_FillRect( x+1, y+1, w-2, 16, color );
			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_YELLOW ) {
				CG_DrawPic( x+1, y+1, w-2, 16, cgs.media.selectShader );
			}
			CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

			if( cgs.yellowflag >= 0 && cgs.yellowflag <= 2 ) {
				item = BG_FindItemForPowerup( PW_YELLOWFLAG );
				if (item) {
					y1 = y + TINYCHAR_HEIGHT + 8;
					CG_DrawPic( x+1, y1+1, w-2, 16, cgs.media.yellowFlagShader[cgs.yellowflag] );
				}
			}
		}
	}

	//
	// health
	//
	value = ps->stats[STAT_HEALTH];
	if ( value > 100 ) {
		trap_R_SetColor( colors[3] );		// white
	} else if (value > 25) {
		trap_R_SetColor( colors[0] );	// green
	} else if (value > 0) {
		color = (cg.time >> 8) & 1;	// flash
		trap_R_SetColor( colors[color] );
	} else {
		trap_R_SetColor( colors[1] );	// red
	}

	// stretch the health up when taking damage
	CG_DrawField ( 185, 432, 3, value);
	CG_ColorForHealth( hcolor );
	trap_R_SetColor( hcolor );


	//
	// armor
	//
	value = ps->stats[STAT_ARMOR];
	if (value > 0 ) {
		trap_R_SetColor( colors[0] );
		CG_DrawField (370, 432, 3, value);
		trap_R_SetColor( NULL );
		// if we didn't draw a 3D icon, draw a 2D icon for armor
		if ( !cg_draw3dIcons.integer && cg_drawIcons.integer ) {
			CG_DrawPic( 370 + CHAR_WIDTH*3 + TEXT_ICON_SPACE, 480 - iconSize, iconSize, iconSize, cgs.media.armorIcon );
		}

	}
}
#endif


/*
=====================
CG_DrawRallyStatusBar
=====================
*/
static void CG_ShiftHudToastQueue( void ) {
	int i;

	if ( cg.hudToastQueueCount <= 0 ) {
		cg.hudToastQueueCount = 0;
		return;
	}
	for ( i = 1; i < cg.hudToastQueueCount; i++ ) {
		cg.hudToastQueue[i - 1] = cg.hudToastQueue[i];
	}
	cg.hudToastQueueCount--;
}

static cgHudToast_t *CG_AddHudToastEntry( cgHudToastType_t type, int duration ) {
	cgHudToast_t *toast;
	int startTime;
	int previousEnd;
	int previousDuration;

	if ( !cg_drawStatus.integer ||
	     ( cgs.gametype == GT_DERBY && !cg_hudShowDerbyVehicle.integer ) ) {
		return NULL;
	}
	while ( cg.hudToastQueueCount > 0 ) {
		toast = &cg.hudToastQueue[0];
		if ( cg.time < toast->startTime + toast->duration ) {
			break;
		}
		CG_ShiftHudToastQueue();
	}
	if ( cg.hudToastQueueCount >= HUD_TOAST_QUEUE_SIZE ) {
		return NULL;
	}

	startTime = cg.time;
	if ( cg.hudToastQueueCount > 0 ) {
		toast = &cg.hudToastQueue[cg.hudToastQueueCount - 1];
		previousDuration = toast->duration > 0 ? toast->duration : HUD_TOAST_DISPLAY_TIME;
		previousEnd = toast->startTime + previousDuration;
		if ( previousEnd > startTime ) {
			startTime = previousEnd;
		}
	}

	toast = &cg.hudToastQueue[cg.hudToastQueueCount++];
	Com_Memset( toast, 0, sizeof( *toast ) );
	toast->type = type;
	toast->startTime = startTime;
	toast->eventTime = cg.time;
	toast->duration = duration;
	return toast;
}

void CG_QueueHudToast( int itemNum ) {
	cgHudToastType_t type;
	cgHudToast_t *toast;
	int i;
	int delta;

	if ( itemNum == HUD_TOAST_CLEAN_SECTOR_ITEM ) {
		type = CG_HUD_TOAST_CLEAN_SECTOR;
	} else {
		if ( itemNum < 1 || itemNum >= bg_numItems ||
		     cg_drawPickups.value <= 0.0f ) {
			return;
		}
		type = CG_HUD_TOAST_PICKUP;
	}

	/* Ignore the duplicate local/global event copy, but retain later pickups. */
	for ( i = 0; i < cg.hudToastQueueCount; i++ ) {
		if ( cg.hudToastQueue[i].type == type &&
		     cg.hudToastQueue[i].itemNum == itemNum ) {
			delta = cg.time - cg.hudToastQueue[i].eventTime;
			if ( delta < 0 ) delta = -delta;
			if ( delta <= 200 ) {
				return;
			}
		}
	}

	toast = CG_AddHudToastEntry( type, HUD_TOAST_DISPLAY_TIME );
	if ( toast ) {
		toast->itemNum = itemNum;
	}
}

void CG_QueueRewardToast( qhandle_t icon, sfxHandle_t sound, int rewardCount ) {
	cgHudToast_t *toast;

	if ( !cg_drawRewards.integer ) {
		return;
	}
	toast = CG_AddHudToastEntry( CG_HUD_TOAST_REWARD,
	                             HUD_TOAST_ANNOUNCEMENT_TIME );
	if ( toast ) {
		toast->iconShader = icon;
		toast->sound = sound;
		toast->rewardCount = rewardCount;
	}
}

void CG_QueueAchievementToast( bgAchievementCategory_t category, int tierIndex ) {
	cgHudToast_t *toast;

	if ( category < 0 || category >= BG_ACHIEVEMENT_CATEGORY_COUNT ||
	     !BG_AchievementGetTier( category, tierIndex ) ) {
		return;
	}
	toast = CG_AddHudToastEntry( CG_HUD_TOAST_ACHIEVEMENT,
	                             HUD_TOAST_ANNOUNCEMENT_TIME );
	if ( toast ) {
		toast->achievementCategory = category;
		toast->achievementTier = tierIndex;
		toast->sound = cgs.media.achievementUnlockSound;
	}
}

void CG_QueueRankToast( int rankIndex, const char *name, qboolean rankUp ) {
	cgHudToast_t *toast;

	if ( rankIndex < 0 || !name || !name[0] ) {
		return;
	}
	toast = CG_AddHudToastEntry( CG_HUD_TOAST_RANK,
	                             HUD_TOAST_ANNOUNCEMENT_TIME );
	if ( toast ) {
		toast->rankIndex = rankIndex;
		toast->rankUp = rankUp;
		Q_strncpyz( toast->name, name, sizeof( toast->name ) );
	}
}

void CG_QueueMessageToast( const char *message ) {
	cgHudToast_t *toast;
	int i;

	if ( !message || !message[0] ) {
		return;
	}
	toast = CG_AddHudToastEntry( CG_HUD_TOAST_MESSAGE,
	                             HUD_TOAST_DISPLAY_TIME + 300 );
	if ( toast ) {
		Q_strncpyz( toast->message, message, sizeof( toast->message ) );
		for ( i = 0; toast->message[i]; i++ ) {
			if ( toast->message[i] == '\n' || toast->message[i] == '\r' ) {
				toast->message[i] = ' ';
			}
		}
	}
}

static const char *CG_GetRewardToastLabel( qhandle_t icon ) {
	if ( icon == cgs.media.medalImpressive ) return "IMPRESSIVE";
	if ( icon == cgs.media.medalImpressiveTelefrag ) return "TELEFRAG";
	if ( icon == cgs.media.medalExcellent ) return "EXCELLENT";
	if ( icon == cgs.media.medalGauntlet ) return "HUMILIATION";
	if ( icon == cgs.media.medalDefend ) return "DEFEND";
	if ( icon == cgs.media.medalAssist ) return "ASSIST";
	if ( icon == cgs.media.medalCapture ) return "CAPTURE";
	if ( icon == cgs.media.medalKothCapture ) return "HILL CAPTURE";
	if ( icon == cgs.media.medalKothDefend ) return "HILL DEFEND";
	return "MEDAL EARNED";
}

static void CG_DrawHudToast( void ) {
	cgHudToast_t *toast;
	gitem_t *item;
	const bgAchievementCategoryDef_t *category;
	const bgAchievementTierDef_t *tier;
	int elapsed;
	int duration;
	int itemNum;
	int textWidth;
	int panelWidth;
	int panelX;
	int textX;
	int i;
	float alpha;
	float textScale;
	qhandle_t icon;
	qboolean hasIcon;
	char message[128];
	vec4_t panelColor = { 0.008f, 0.012f, 0.016f, 0.76f };
	vec4_t borderColor = { 0.24f, 0.34f, 0.36f, 0.78f };
	vec4_t accentColor = { 0.72f, 1.00f, 0.06f, 1.00f };
	vec4_t textColor = { 0.88f, 0.95f, 0.97f, 1.00f };
	vec4_t iconColor = { 1.00f, 1.00f, 1.00f, 1.00f };

	while ( cg.hudToastQueueCount > 0 ) {
		toast = &cg.hudToastQueue[0];
		duration = toast->duration > 0 ? toast->duration : HUD_TOAST_DISPLAY_TIME;
		elapsed = cg.time - toast->startTime;
		if ( elapsed < duration &&
		     !( toast->type == CG_HUD_TOAST_PICKUP && cg_drawPickups.value <= 0.0f ) ) {
			break;
		}
		CG_ShiftHudToastQueue();
	}
	if ( cg.hudToastQueueCount <= 0 ) {
		return;
	}
	toast = &cg.hudToastQueue[0];
	duration = toast->duration > 0 ? toast->duration : HUD_TOAST_DISPLAY_TIME;
	elapsed = cg.time - toast->startTime;
	if ( elapsed < 0 ) {
		return;
	}
	if ( toast->sound && !toast->soundPlayed ) {
		trap_S_StartLocalSound( toast->sound, CHAN_ANNOUNCER );
		toast->soundPlayed = qtrue;
	}

	alpha = 1.0f;
	if ( elapsed > duration - HUD_TOAST_FADE_TIME ) {
		alpha = (float)( duration - elapsed ) / HUD_TOAST_FADE_TIME;
	}
	if ( alpha < 0.0f ) alpha = 0.0f;
	icon = 0;
	itemNum = toast->itemNum;
	hasIcon = qtrue;
	Q_strncpyz( message, "NOTICE", sizeof( message ) );

	switch ( toast->type ) {
	case CG_HUD_TOAST_CLEAN_SECTOR:
		Q_strncpyz( message, "CLEAN SECTOR +0.5S NOS", sizeof( message ) );
		item = BG_FindItemForPowerup( PW_TURBO );
		if ( item ) itemNum = item - bg_itemlist;
		accentColor[0] = 0.30f; accentColor[1] = 0.60f; accentColor[2] = 1.00f;
		break;
	case CG_HUD_TOAST_PICKUP:
		if ( itemNum < 1 || itemNum >= bg_numItems ) {
			CG_ShiftHudToastQueue();
			return;
		}
		item = &bg_itemlist[itemNum];
		Q_strncpyz( message, item->pickup_name ? item->pickup_name : "PICKUP",
		            sizeof( message ) );
		break;
	case CG_HUD_TOAST_REWARD:
		if ( toast->rewardCount > 1 ) {
			Com_sprintf( message, sizeof( message ), "%s  %d",
			             CG_GetRewardToastLabel( toast->iconShader ), toast->rewardCount );
		} else {
			Q_strncpyz( message, CG_GetRewardToastLabel( toast->iconShader ),
			            sizeof( message ) );
		}
		icon = toast->iconShader;
		accentColor[0] = 1.00f; accentColor[1] = 0.78f; accentColor[2] = 0.20f;
		break;
	case CG_HUD_TOAST_ACHIEVEMENT:
		category = BG_AchievementGetCategory( toast->achievementCategory );
		tier = BG_AchievementGetTier( toast->achievementCategory, toast->achievementTier );
		if ( !category || !tier ) {
			CG_ShiftHudToastQueue();
			return;
		}
		Com_sprintf( message, sizeof( message ), "ACHIEVEMENT: %s", tier->name );
		if ( category->icon >= 0 && category->icon < BG_ACHIEVEMENT_ICON_COUNT ) {
			icon = ( elapsed < ACHIEVEMENT_LOCKED_TIME ) ?
			       cgs.media.achievementMedalLocked[category->icon] :
			       cgs.media.achievementMedalUnlocked[category->icon];
		}
		accentColor[0] = 0.72f; accentColor[1] = 1.00f; accentColor[2] = 0.06f;
		break;
	case CG_HUD_TOAST_RANK:
		Com_sprintf( message, sizeof( message ), "%s: %s",
		             toast->rankUp ? "RANK UP" : "RANK DOWN", toast->name );
		if ( toast->rankUp ) {
			accentColor[0] = 0.30f; accentColor[1] = 0.88f; accentColor[2] = 0.42f;
		} else {
			accentColor[0] = 1.00f; accentColor[1] = 0.38f; accentColor[2] = 0.28f;
		}
		hasIcon = qfalse;
		break;
	case CG_HUD_TOAST_MESSAGE:
		Q_strncpyz( message, toast->message, sizeof( message ) );
		hasIcon = qfalse;
		break;
	default:
		CG_ShiftHudToastQueue();
		return;
	}

	if ( !icon && itemNum > 0 && itemNum < bg_numItems &&
	     ( toast->type == CG_HUD_TOAST_PICKUP ||
	       toast->type == CG_HUD_TOAST_CLEAN_SECTOR ) ) {
		CG_RegisterItemVisuals( itemNum );
		icon = cg_items[itemNum].icon;
	}

	textScale = 0.75f;
	textWidth = CG_IngameStringWidth( message, UI_SMALLFONT, textScale );
	if ( textWidth > 278 ) {
		textScale = 0.75f * 278.0f / textWidth;
	}
	if ( textScale < 0.42f ) textScale = 0.42f;
	while ( CG_IngameStringWidth( message, UI_SMALLFONT, textScale ) > 278 ) {
		i = strlen( message );
		if ( i <= 1 ) break;
		message[i - 1] = '\0';
	}
	textWidth = CG_IngameStringWidth( message, UI_SMALLFONT, textScale );
	panelWidth = textWidth + ( hasIcon && icon ? 34 : 20 );
	panelX = ( 640 - panelWidth ) / 2;
	textX = panelX + ( hasIcon && icon ? 28 : 10 );
	panelColor[3] *= alpha;
	borderColor[3] *= alpha;
	accentColor[3] *= alpha;
	textColor[3] *= alpha;
	iconColor[3] *= alpha;

	CG_FillRect( panelX, 386, panelWidth, 24, panelColor );
	CG_FillRect( panelX, 386, panelWidth, 2, accentColor );
	CG_DrawRect( panelX, 386, panelWidth, 24, 1.0f, borderColor );
	if ( hasIcon && icon ) {
		trap_R_SetColor( iconColor );
		CG_DrawPic( panelX + 7, 390, 16, 16, icon );
		trap_R_SetColor( NULL );
	}
	CG_DrawIngameString( textX, 393, message, UI_SMALLFONT, textScale, textColor );
}

static int CG_RallyTelemetryX( float x, float spreadFactor ) {
	/* Spread anchors into the pillarboxed side margins without stretching
	 * glyphs or icons; their own draw sizes remain aspect-correct. */
	return (int)( ( SCREEN_WIDTH * 0.5f ) +
	               ( x - SCREEN_WIDTH * 0.5f ) * spreadFactor + 0.5f );
}

static void CG_DrawRallyTelemetryHud( void ) {
	playerState_t *ps;
	centity_t *cent;
	vec4_t panelColor = { 0.008f, 0.012f, 0.016f, 0.50f };
	vec4_t lineColor = { 0.300f, 0.390f, 0.430f, 0.78f };
	vec4_t mutedColor = { 0.480f, 0.610f, 0.650f, 1.00f };
	vec4_t accentColor = { 0.720f, 1.000f, 0.060f, 1.00f };
	vec4_t blueColor = { 0.380f, 0.650f, 1.000f, 1.00f };
	vec4_t dangerColor = { 1.000f, 0.150f, 0.080f, 1.00f };
	vec4_t healthColor;
	vec4_t darkSegment = { 0.075f, 0.095f, 0.105f, 0.92f };
	float healthFrac;
	float fuelFrac;
	float rpmFrac;
	float turboFrac;
	float damageFrac;
	float speed;
	float raceProgress;
	float raceDistanceTotal;
	float barX;
	float segmentW;
	float maxHealth;
	float centerX;
	float centerY;
	float zoneColorAlpha;
	float derbyHitAlpha;
	float turboBarWidth;
	float fuelBarWidth;
	float spreadFactor;
	int health;
	int armor;
	int fuel;
	int rpm;
	int turboMs;
	int turboValue;
	int speedValue;
	int speedRight;
	int vehicleInfoRight;
	int gearValueWidth;
	int gearPrefixWidth;
	int damageTaken;
	int damageDealt;
	int gear;
	int position;
	int racers;
	int lap;
	int playersRemaining;
	int team;
	int teamScore;
	int lapTime;
	int totalTime;
	int raceDistanceRemain;
	int raceLaps;
	int i;
	int derbyHitElapsed;
	int zoneCounts[5];
	int zoneCount;
	int zoneIndex;
	int zoneChip;
	const char *derbyHitLabel;
	int frontWeapon, rearWeapon, fallbackRearWeapon, weaponIndex;
	int frontAmmo, rearAmmo;
	float zoneChipX;
	float zoneChipW;
	qboolean raceMode;
	qboolean raceCombatMode;
	qboolean showWeaponTelemetry;
	qboolean showRaceProgress;
	qboolean teamMode;
	qboolean turboActive;
	screenPlacement_e savedHorizontalPlacement;
	screenPlacement_e savedVerticalPlacement;
	char healthText[16];
	char speedText[16];
	char gearText[16];
	char fuelText[16];
	char raceText[32];
	char modeTitle[24];
	char modeValue[32];
	char modeExtra[32];
	char timeText[24];
	char bestText[24];
	char ammoText[12];
	char zoneChipText[8];
	vec4_t zoneTeamColor;
	vec3_t forward;

#define HUD_X(x) CG_RallyTelemetryX( (float)(x), spreadFactor )

	if ( !cg.snap || !cg_drawStatus.integer ) {
		return;
	}
	if ( cgs.gametype == GT_DERBY && !cg_hudShowDerbyVehicle.integer ) {
		return;
	}
	savedHorizontalPlacement = CG_GetScreenHorizontalPlacement();
	savedVerticalPlacement = CG_GetScreenVerticalPlacement();

	ps = &cg.snap->ps;
	cent = &cg_entities[ps->clientNum];
	raceMode = ( cgs.gametype == GT_RACING || cgs.gametype == GT_RACING_DM ||
	             cgs.gametype == GT_SPRINT || cgs.gametype == GT_TEAM_RACING ||
	             cgs.gametype == GT_TEAM_RACING_DM || cgs.gametype == GT_SINGLE_PLAYER );
	raceCombatMode = ( cgs.gametype == GT_RACING_DM ||
	                   cgs.gametype == GT_TEAM_RACING_DM );
	showWeaponTelemetry = raceCombatMode || cgs.gametype == GT_LCS ||
	                      cgs.gametype == GT_DEATHMATCH ||
	                      cgs.gametype == GT_CTF ||
	                      cgs.gametype == GT_CTF4 ||
	                      cgs.gametype == GT_DOMINATION ||
	                      cgs.gametype == GT_KOTH;
	spreadFactor = 1.0f;
	if ( raceCombatMode && !cg_stretch.integer && cgs.screenXScale > 0.0f ) {
		spreadFactor = cgs.screenXScaleStretch / cgs.screenXScale;
		if ( spreadFactor < 1.0f )
			spreadFactor = 1.0f;
	}
	health = ps->stats[STAT_HEALTH];
	armor = ps->stats[STAT_ARMOR];
	fuel = ps->stats[STAT_FUEL];
	rpm = ps->stats[STAT_RPM];
	turboValue = ps->powerups[PW_TURBO];
	damageTaken = ps->stats[STAT_DAMAGE_TAKEN];
	damageDealt = ps->stats[STAT_DAMAGE_DEALT];
	gear = ps->stats[STAT_GEAR];
	position = cent->currentPosition;
	racers = cgs.numRacers > 0 ? cgs.numRacers : 1;
	lap = cent->currentLap;

	maxHealth = (float)ps->stats[STAT_MAX_HEALTH];
	if ( maxHealth <= 0.0f ) {
		maxHealth = 100.0f;
	}
	healthFrac = health / maxHealth;
	if ( healthFrac < 0.0f ) healthFrac = 0.0f;
	if ( healthFrac > 1.0f ) healthFrac = 1.0f;
	if ( fuel < 0 ) fuel = 0;
	if ( fuel > 100 ) fuel = 100;
	fuelFrac = fuel / 100.0f;
	rpmFrac = rpm / (float)CP_RPM_MAX;
	if ( rpmFrac < 0.0f ) rpmFrac = 0.0f;
	if ( rpmFrac > 1.0f ) rpmFrac = 1.0f;
	turboActive = turboValue > cg.time;
	if ( turboValue < 0 ) {
		turboMs = -turboValue;
	} else if ( turboActive ) {
		turboMs = turboValue - cg.time;
	} else {
		turboMs = 0;
	}
	/* Five pickups fill the continuous gauge (25 seconds total). */
	turboFrac = turboMs / (float)RALLY_TURBO_MAX_MSEC;
	if ( turboFrac < 0.0f ) turboFrac = 0.0f;
	if ( turboFrac > 1.0f ) turboFrac = 1.0f;

	AngleVectors( ps->viewangles, forward, NULL, NULL );
	speed = fabs( Q3VelocityToRL( DotProduct( ps->velocity, forward ) ) );
	speedValue = (int)speed;

	if ( healthFrac > 0.50f ) {
		healthColor[0] = 0.90f; healthColor[1] = 0.96f;
		healthColor[2] = 0.98f; healthColor[3] = 1.00f;
	} else if ( healthFrac > 0.25f ) {
		Vector4Copy( accentColor, healthColor );
	} else {
		Vector4Copy( dangerColor, healthColor );
	}

	/* One continuous telemetry strip.  It is deliberately quiet so the
	   world, mirror and minimap remain the visual focus. */
	CG_SetScreenPlacement( PLACE_STRETCH, PLACE_BOTTOM );
	CG_FillRect( 0, 414, 640, 66, panelColor );
	CG_FillRect( 0, 414, 640, 1, lineColor );
	CG_FillRect( 0, 479, 640, 1, lineColor );
	CG_SetScreenPlacement( PLACE_CENTER, PLACE_BOTTOM );
	CG_FillRect( HUD_X(174), 422, 1, 50, lineColor );
	CG_FillRect( HUD_X(432), 422, 1, 50, lineColor );

	/* Left: integrity and the two combat counters. */
	CG_DrawIngameString( HUD_X(74), 414, "INTEGRITY",
	                     UI_CENTER | UI_SMALLFONT, 0.75f, mutedColor );
	Com_sprintf( healthText, sizeof(healthText), "%d", health );
	CG_DrawIngameString( HUD_X(74), 426, healthText, UI_CENTER,
	                     1.1f, healthColor );
	if ( health <= 25 && cgs.media.derbyHudWarningShader ) {
		trap_R_SetColor( dangerColor );
		CG_DrawPic( HUD_X(136), 414, 14, 14, cgs.media.derbyHudWarningShader );
		trap_R_SetColor( NULL );
	}
	CG_DrawIngameString( HUD_X(114), 432, "ARMOR", UI_SMALLFONT, 0.45f, mutedColor );
	CG_DrawIngameString( HUD_X(166), 432, va("%d", armor), UI_RIGHT | UI_SMALLFONT,
	                     0.45f, blueColor );
	barX = 22.0f;
	CG_FillRect( HUD_X(barX), 469, 104 * spreadFactor, 5, darkSegment );
	CG_FillRect( HUD_X(barX), 469, 104 * healthFrac * spreadFactor, 5, healthColor );
	CG_FillRect( HUD_X(barX + 104 * healthFrac), 469, 1, 5, colorWhite );

	/* Centre: use the mode's objective as the primary telemetry. */
	team = ps->persistant[PERS_TEAM];
	teamMode = ( cgs.gametype == GT_TEAM || cgs.gametype == GT_CTF ||
	             cgs.gametype == GT_CTF4 || cgs.gametype == GT_DOMINATION );

	/* STAT_DISTANCE_REMAIN and CS_TRACKLENGTH are both expressed in metres.
	 * Loop races use full-lap distance; Sprint publishes its open start-finish
	 * route length so both values describe the same course. */
	raceProgress = 0.0f;
	raceDistanceTotal = 0.0f;
	raceDistanceRemain = ps->stats[STAT_DISTANCE_REMAIN];
	raceLaps = cgs.laplimit;
	if ( cgs.gametype == GT_SPRINT && raceLaps <= 0 ) {
		raceLaps = 1;
	}
	raceDistanceTotal = cgs.trackLength * raceLaps;
	showRaceProgress = raceMode && raceDistanceTotal > 0.0f;
	if ( showRaceProgress ) {
		if ( cent->finishRaceTime ) {
			raceProgress = 1.0f;
		} else if ( raceDistanceRemain > 0 ) {
			raceProgress = 1.0f - raceDistanceRemain / raceDistanceTotal;
		}
		if ( raceProgress < 0.0f ) raceProgress = 0.0f;
		if ( raceProgress > 1.0f ) raceProgress = 1.0f;
	}

	if ( cgs.gametype == GT_DERBY ) {
		CG_DrawIngameSmallString( HUD_X(190), 414, "ZONES", mutedColor );
		centerX = 208.0f;
		centerY = 431.0f;
		zoneColorAlpha = 0.82f;
		{
			vec4_t zoneColor;
			vec4_t hitLabelColor;
			Vector4Copy( blueColor, zoneColor );
			zoneColor[3] = zoneColorAlpha;
			CG_FillRect( centerX + 11, centerY, 40, 7, zoneColor );
			CG_FillRect( centerX + 11, centerY + 26, 40, 7, zoneColor );
			CG_FillRect( centerX, centerY + 7, 8, 19, zoneColor );
			CG_FillRect( centerX + 54, centerY + 7, 8, 19, zoneColor );
			CG_FillRect( centerX + 11, centerY + 10, 40, 13, darkSegment );
			CG_DrawRect( centerX, centerY, 62, 33, 1.0f, lineColor );

			/* Keep directional feedback visible long enough to read; undirected
			 * world impacts light the whole diagram instead of silently vanishing. */
			derbyHitElapsed = cg.time - cg.derbyHitFxTime;
			derbyHitLabel = NULL;
			if ( cg.derbyHitFxTime > 0 && derbyHitElapsed >= 0 && derbyHitElapsed < 1100 ) {
				derbyHitAlpha = 0.96f * ( 1.0f - (float)derbyHitElapsed / 1100.0f );
				zoneColor[0] = 1.0f;
				zoneColor[1] = 0.36f;
				zoneColor[2] = 0.08f;
				zoneColor[3] = derbyHitAlpha;
				Vector4Copy( zoneColor, hitLabelColor );
				switch ( cg.derbyHitFxDir ) {
				case 0:
					CG_FillRect( centerX + 11, centerY, 40, 7, zoneColor );
					derbyHitLabel = "FRONT HIT";
					break;
				case 1:
					CG_FillRect( centerX, centerY + 7, 8, 19, zoneColor );
					derbyHitLabel = "LEFT HIT";
					break;
				case 2:
					CG_FillRect( centerX + 54, centerY + 7, 8, 19, zoneColor );
					derbyHitLabel = "RIGHT HIT";
					break;
				case 3:
					CG_FillRect( centerX + 11, centerY + 26, 40, 7, zoneColor );
					derbyHitLabel = "REAR HIT";
					break;
				default:
					CG_FillRect( centerX + 11, centerY, 40, 7, zoneColor );
					CG_FillRect( centerX + 11, centerY + 26, 40, 7, zoneColor );
					CG_FillRect( centerX, centerY + 7, 8, 19, zoneColor );
					CG_FillRect( centerX + 54, centerY + 7, 8, 19, zoneColor );
					derbyHitLabel = "HIT";
					break;
				}
				CG_DrawIngameString( centerX + 31, 462, derbyHitLabel,
				                     UI_CENTER | UI_SMALLFONT, 0.36f, hitLabelColor );
			}
		}
		damageFrac = damageTaken / 100.0f;
		if ( damageFrac < 0.0f ) damageFrac = 0.0f;
		if ( damageFrac > 1.0f ) damageFrac = 1.0f;
		CG_DrawIngameSmallString( HUD_X(284), 414, "DAMAGE", mutedColor );
		CG_FillRect( HUD_X(284), 432, 126 * spreadFactor, 5, darkSegment );
		CG_FillRect( HUD_X(284), 432, 126 * damageFrac * spreadFactor, 5, dangerColor );
		CG_DrawIngameString( HUD_X(284), 441, va("IN %d", damageTaken),
		                     UI_SMALLFONT, 0.6f, colorWhite );
		CG_DrawIngameString( HUD_X(410), 441, va("OUT %d", damageDealt),
		                     UI_RIGHT | UI_SMALLFONT, 0.6f, dangerColor );
		if ( cg_hudShowScores.integer ) {
			Com_sprintf( raceText, sizeof(raceText), "SCORE %d", ps->persistant[PERS_SCORE] );
			CG_DrawIngameString( HUD_X(347), 459, raceText,
			                     UI_CENTER | UI_SMALLFONT, 0.75f, accentColor );
		}
	} else if ( cgs.gametype == GT_DOMINATION ) {
		static const char *zoneCodes[5] = { "R", "B", "G", "Y", "N" };
		zoneCounts[0] = zoneCounts[1] = zoneCounts[2] = 0;
		zoneCounts[3] = zoneCounts[4] = 0;
		zoneCount = 0;
		for ( zoneIndex = 0; zoneIndex < MAX_SIGILS; zoneIndex++ ) {
			switch ( cgs.sigil[zoneIndex] ) {
			case SIGIL_ISRED:    zoneCounts[0]++; zoneCount++; break;
			case SIGIL_ISBLUE:   zoneCounts[1]++; zoneCount++; break;
			case SIGIL_ISGREEN:  zoneCounts[2]++; zoneCount++; break;
			case SIGIL_ISYELLOW: zoneCounts[3]++; zoneCount++; break;
			case SIGIL_ISWHITE:  zoneCounts[4]++; zoneCount++; break;
			default: break;
			}
		}
		CG_DrawIngameSmallString( HUD_X(190), 414, "DOMINATION", mutedColor );
		CG_DrawIngameSmallString( HUD_X(190), 428, "ZONES", mutedColor );
		if ( zoneCount > 0 ) {
			zoneChipW = 18.0f;
			for ( zoneChip = 0; zoneChip < 5; zoneChip++ ) {
				switch ( zoneChip ) {
				case 0: Vector4Copy( colorRed, zoneTeamColor ); break;
				case 1: Vector4Copy( colorBlue, zoneTeamColor ); break;
				case 2: Vector4Copy( colorGreen, zoneTeamColor ); break;
				case 3: Vector4Copy( colorYellow, zoneTeamColor ); break;
				default: Vector4Copy( colorWhite, zoneTeamColor ); break;
				}
				zoneTeamColor[3] = 0.24f;
				zoneChipX = HUD_X(190 + zoneChip * 20);
				CG_FillRect( zoneChipX, 439, zoneChipW, 13, zoneTeamColor );
				zoneTeamColor[3] = 0.95f;
				CG_FillRect( zoneChipX, 439, 2, 13, zoneTeamColor );
				CG_DrawRect( zoneChipX, 439, zoneChipW, 13, 1.0f, lineColor );
				Com_sprintf( zoneChipText, sizeof(zoneChipText), "%s%d",
				             zoneCodes[zoneChip], zoneCounts[zoneChip] );
				CG_DrawIngameString( zoneChipX + zoneChipW * 0.5f, 442,
				                     zoneChipText, UI_CENTER | UI_SMALLFONT,
				                     0.32f, colorWhite );
			}
		} else {
			CG_DrawIngameString( HUD_X(294), 439, "NO ZONES",
			                     UI_RIGHT | UI_SMALLFONT, 0.38f, mutedColor );
		}
		if ( cg_hudShowScores.integer ) {
			Com_sprintf( modeValue, sizeof(modeValue), "SCORE %d", ps->persistant[PERS_SCORE] );
			CG_DrawIngameString( HUD_X(216), 457, modeValue,
			                     UI_CENTER | UI_SMALLFONT, 0.34f, colorWhite );
			CG_FillRect( HUD_X(242), 456, 1, 12, lineColor );
			if ( team >= TEAM_RED && team <= TEAM_YELLOW ) {
				teamScore = cg.teamScores[team - TEAM_RED];
				Com_sprintf( modeValue, sizeof(modeValue), "TEAM %d", teamScore );
				CG_DrawIngameString( HUD_X(268), 457, modeValue,
				                     UI_CENTER | UI_SMALLFONT, 0.34f, accentColor );
			}
		}
	} else if ( cgs.gametype == GT_KOTH ) {
		CG_DrawIngameString( HUD_X(190), 414, "KING OF THE HILL",
		                     UI_SMALLFONT, 0.42f, mutedColor );
		if ( cg_hudShowScores.integer ) {
			Com_sprintf( modeValue, sizeof(modeValue), "SCORE %d", ps->persistant[PERS_SCORE] );
			CG_DrawIngameString( HUD_X(216), 457, modeValue,
			                     UI_CENTER | UI_SMALLFONT, 0.34f, colorWhite );
			CG_FillRect( HUD_X(242), 456, 1, 12, lineColor );
			if ( team >= TEAM_RED && team <= TEAM_YELLOW ) {
				teamScore = cg.teamScores[team - TEAM_RED];
				Com_sprintf( modeValue, sizeof(modeValue), "TEAM %d", teamScore );
				CG_DrawIngameString( HUD_X(268), 457, modeValue,
				                     UI_CENTER | UI_SMALLFONT, 0.34f, accentColor );
			}
		}
	} else if ( cgs.gametype == GT_LCS ) {
		playersRemaining = CG_GetPlayersRemaining( NULL );
		CG_DrawIngameSmallString( HUD_X(190), 414, "LCS MATCH", mutedColor );
		if ( playersRemaining > 0 )
			Com_sprintf( modeValue, sizeof(modeValue), "LEFT %d", playersRemaining );
		else
			Q_strncpyz( modeValue, "LEFT --", sizeof(modeValue) );
		CG_DrawIngameString( HUD_X(190), 432, modeValue,
		                     UI_SMALLFONT, 0.52f, accentColor );
		if ( cg_hudShowScores.integer ) {
			Com_sprintf( modeExtra, sizeof(modeExtra), "SCORE %d", ps->persistant[PERS_SCORE] );
			CG_DrawIngameString( HUD_X(294), 432, modeExtra,
			                     UI_RIGHT | UI_SMALLFONT, 0.42f, accentColor );
		}
		if ( cent->finishRaceTime )
			totalTime = cent->finishRaceTime - cent->startRaceTime;
		else if ( cent->startRaceTime )
			totalTime = cg.time - cent->startRaceTime;
		else
			totalTime = 0;
		if ( totalTime < 0 ) totalTime = 0;
		Q_strncpyz( timeText, getStringForTime( totalTime ), sizeof(timeText) );
		if ( cg_hudShowTimes.integer ) {
			Com_sprintf( modeValue, sizeof(modeValue), "MATCH %s", timeText );
			CG_DrawIngameString( HUD_X(190), 450, modeValue,
			                     UI_SMALLFONT, 0.42f, colorWhite );
		}
	} else if ( cgs.gametype == GT_ELIMINATION ) {
		playersRemaining = CG_GetPlayersRemaining( NULL );
		Q_strncpyz( modeTitle, "ELIMINATION", sizeof(modeTitle) );
		CG_DrawIngameSmallString( HUD_X(190), 414, modeTitle, mutedColor );
		if ( cg_hudShowPosition.integer && position > 0 ) {
			Com_sprintf( modeValue, sizeof(modeValue), "POS %d/%d", position, racers );
		} else if ( cg_hudShowPosition.integer ) {
			Q_strncpyz( modeValue, "POS --", sizeof(modeValue) );
		} else {
			Com_sprintf( modeValue, sizeof(modeValue), "PLAYERS %d", playersRemaining );
		}
		if ( cg_hudShowPosition.integer && playersRemaining > 0 ) {
			Com_sprintf( modeExtra, sizeof(modeExtra), "LEFT %d", playersRemaining );
		} else if ( cg_hudShowPosition.integer ) {
			Q_strncpyz( modeExtra, "LEFT --", sizeof(modeExtra) );
		}
		CG_DrawIngameSmallString( HUD_X(190), 432, modeValue, colorWhite );
		if ( cg_hudShowPosition.integer ) {
			CG_DrawIngameString( HUD_X(410), 432, modeExtra,
			                     UI_RIGHT | UI_SMALLFONT, 0.75f, accentColor );
		}
		if ( cent->finishRaceTime ) {
			lapTime = cent->finishRaceTime - cent->startLapTime;
			totalTime = cent->finishRaceTime - cent->startRaceTime;
		} else if ( cent->startRaceTime ) {
			lapTime = cg.time - cent->startLapTime;
			totalTime = cg.time - cent->startRaceTime;
		} else {
			lapTime = totalTime = 0;
		}
		Q_strncpyz( timeText, getStringForTime( totalTime ), sizeof(timeText) );
		if ( cg_hudShowTimes.integer ) {
			Com_sprintf( modeValue, sizeof(modeValue), "TIME %s", timeText );
			CG_DrawIngameSmallString( HUD_X(190), 450, modeValue, colorWhite );
		}
		if ( cg_hudShowScores.integer ) {
			Com_sprintf( modeExtra, sizeof(modeExtra), "SCORE %d", ps->persistant[PERS_SCORE] );
			CG_DrawIngameString( HUD_X(410), 450, modeExtra,
			                     UI_RIGHT | UI_SMALLFONT, 0.55f, accentColor );
		}
	} else if ( raceCombatMode ) {
		if ( cent->finishRaceTime ) {
			lapTime = cent->finishRaceTime - cent->startLapTime;
			totalTime = cent->finishRaceTime - cent->startRaceTime;
		} else if ( cent->startRaceTime ) {
			lapTime = cg.time - cent->startLapTime;
			totalTime = cg.time - cent->startRaceTime;
		} else {
			lapTime = totalTime = 0;
		}
		CG_DrawIngameSmallString( HUD_X(190), 414, "RACE COMBAT", mutedColor );
		if ( cg_hudShowLaps.integer ) {
			if ( cgs.laplimit > 1 )
				Com_sprintf( modeValue, sizeof(modeValue), "LAP %d/%d", lap, cgs.laplimit );
			else
				Com_sprintf( modeValue, sizeof(modeValue), "LAP %d", lap );
			CG_DrawIngameString( HUD_X(190), 432, modeValue,
			                     UI_SMALLFONT, 0.48f, colorWhite );
		}
		if ( cg_hudShowPosition.integer ) {
			if ( position > 0 )
				Com_sprintf( modeExtra, sizeof(modeExtra), "POS %d/%d", position, racers );
			else
				Q_strncpyz( modeExtra, "POS --", sizeof(modeExtra) );
			CG_DrawIngameString( HUD_X(294), 432, modeExtra,
			                     UI_RIGHT | UI_SMALLFONT, 0.48f, accentColor );
		}
		if ( cg_hudShowTimes.integer ) {
			Q_strncpyz( timeText, getStringForTime( lapTime ), sizeof(timeText) );
			Com_sprintf( modeValue, sizeof(modeValue), "TIME %s", timeText );
			CG_DrawIngameString( HUD_X(190), 450, modeValue,
			                     UI_SMALLFONT, 0.42f, colorWhite );
		}
		if ( cg_hudShowScores.integer ) {
			Com_sprintf( modeExtra, sizeof(modeExtra), "SCORE %d", ps->persistant[PERS_SCORE] );
			CG_DrawIngameString( HUD_X(294), 450, modeExtra,
			                     UI_RIGHT | UI_SMALLFONT, 0.42f, accentColor );
		}
	} else if ( raceMode ) {
		if ( cent->finishRaceTime ) {
			lapTime = cent->finishRaceTime - cent->startLapTime;
			totalTime = cent->finishRaceTime - cent->startRaceTime;
		} else if ( cent->startRaceTime ) {
			lapTime = cg.time - cent->startLapTime;
			totalTime = cg.time - cent->startRaceTime;
		} else {
			lapTime = totalTime = 0;
		}
		if ( cgs.gametype == GT_SPRINT ) {
			Q_strncpyz( modeTitle, "SPRINT", sizeof(modeTitle) );
			Com_sprintf( modeValue, sizeof(modeValue), "DIST %dM",
			             (int)ps->stats[STAT_DISTANCE_REMAIN] );
		} else {
			Q_strncpyz( modeTitle,
			            ( cgs.gametype == GT_RACING_DM || cgs.gametype == GT_TEAM_RACING_DM )
			            ? "RACE COMBAT" : "RACE STATUS", sizeof(modeTitle) );
			if ( cgs.laplimit > 1 )
				Com_sprintf( modeValue, sizeof(modeValue), "LAP %d/%d", lap, cgs.laplimit );
			else
				Com_sprintf( modeValue, sizeof(modeValue), "LAP %d", lap );
		}
		CG_DrawIngameSmallString( 190, 414, modeTitle, mutedColor );
		if ( cg_ghostPlayback.integer ) {
			const char *ghostStatus;
			vec4_t ghostStatusColor;

			Vector4Copy( mutedColor, ghostStatusColor );
			if ( cg_ghostPlayback.integer == 1 ) {
				CG_LoadPersonalGhost();
				if ( cg.personalGhostAvailable ) {
					ghostStatus = "PERSONAL GHOST";
					Vector4Copy( blueColor, ghostStatusColor );
				} else if ( cg.personalGhostSearchValid && !cg.personalGhostSearchFound ) {
					ghostStatus = "NO GHOST FOR CAR";
					Vector4Copy( dangerColor, ghostStatusColor );
				} else {
					ghostStatus = "SEARCHING GHOST";
				}
			} else if ( cg_ghostPlayback.integer == 2 ) {
				if ( cg.baseGhostAvailable ) {
					ghostStatus = "SERVER BASE";
					Vector4Copy( blueColor, ghostStatusColor );
				} else if ( cg.baseGhostTransferFailed ) {
					ghostStatus = "BASE TRANSFER FAILED";
					Vector4Copy( dangerColor, ghostStatusColor );
				} else if ( cg.baseGhostTransferPending ) {
					ghostStatus = "RECEIVING BASE";
				} else if ( cg.baseGhostStatusKnown ) {
					ghostStatus = "NO BASE GHOST";
					Vector4Copy( dangerColor, ghostStatusColor );
				} else {
					ghostStatus = "WAITING FOR BASE";
				}
			} else {
				ghostStatus = "GHOST OFF";
			}

			CG_DrawIngameString( HUD_X(410), 414, ghostStatus,
			                     UI_RIGHT | UI_SMALLFONT, 0.42f, ghostStatusColor );
		}
		if ( position > 0 )
			Com_sprintf( modeExtra, sizeof(modeExtra), "POS %d/%d", position, racers );
		else
			Q_strncpyz( modeExtra, "POS --", sizeof(modeExtra) );
		if ( cgs.gametype == GT_SPRINT ? cg_hudShowDistToFinish.integer
		                               : cg_hudShowLaps.integer ) {
			CG_DrawIngameSmallString( 190, 432, modeValue, colorWhite );
		}
		if ( cg_hudShowPosition.integer ) {
			CG_DrawIngameString( 410, 432, modeExtra,
			                     UI_RIGHT | UI_SMALLFONT, 0.75f, accentColor );
		}
		if ( cgs.gametype == GT_SPRINT ) {
			Q_strncpyz( timeText, getStringForTime( totalTime ), sizeof(timeText) );
			Q_strncpyz( bestText, getStringForTime( cent->bestLapTime ), sizeof(bestText) );
			Com_sprintf( modeValue, sizeof(modeValue), "TIME %s", timeText );
			Com_sprintf( modeExtra, sizeof(modeExtra), "BEST %s", bestText );
		} else {
			Q_strncpyz( timeText, getStringForTime( lapTime ), sizeof(timeText) );
			Com_sprintf( modeValue, sizeof(modeValue), "LAP %s", timeText );
			if ( cgs.gametype == GT_RACING_DM || cgs.gametype == GT_TEAM_RACING_DM ) {
				Com_sprintf( modeExtra, sizeof(modeExtra), "SCORE %d", ps->persistant[PERS_SCORE] );
			} else {
				Q_strncpyz( bestText, getStringForTime( cent->bestLapTime ), sizeof(bestText) );
				Com_sprintf( modeExtra, sizeof(modeExtra), "BEST %s", bestText );
			}
		}
		if ( cg_hudShowTimes.integer ) {
			CG_DrawIngameSmallString( 190, 450, modeValue, colorWhite );
		}
		if ( ( cgs.gametype == GT_RACING_DM || cgs.gametype == GT_TEAM_RACING_DM )
		     ? cg_hudShowScores.integer : cg_hudShowTimes.integer ) {
			CG_DrawIngameString( 410, 450, modeExtra,
			                     UI_RIGHT | UI_SMALLFONT, 0.55f, accentColor );
		}
	} else {
		switch ( cgs.gametype ) {
		case GT_CTF:
			Q_strncpyz( modeTitle, "CAPTURE THE FLAG", sizeof(modeTitle) );
			break;
		case GT_CTF4:
			Q_strncpyz( modeTitle, "FOUR TEAM CTF", sizeof(modeTitle) );
			break;
		case GT_DOMINATION:
			Q_strncpyz( modeTitle, "DOMINATION", sizeof(modeTitle) );
			break;
		case GT_TEAM:
			Q_strncpyz( modeTitle, "TEAM COMBAT", sizeof(modeTitle) );
			break;
		default:
			Q_strncpyz( modeTitle, "DEATHMATCH", sizeof(modeTitle) );
			break;
		}
		CG_DrawIngameSmallString( 190, 414, modeTitle, mutedColor );
		if ( cg_hudShowScores.integer ) {
			Com_sprintf( modeValue, sizeof(modeValue), "SCORE %d", ps->persistant[PERS_SCORE] );
			CG_DrawIngameSmallString( 190, 432, modeValue, colorWhite );
			if ( teamMode && cgs.gametype != GT_CTF &&
			     cgs.gametype != GT_CTF4 &&
			     team >= TEAM_RED && team <= TEAM_YELLOW ) {
				teamScore = cg.teamScores[team - TEAM_RED];
				Com_sprintf( modeExtra, sizeof(modeExtra), "TEAM %d", teamScore );
				CG_DrawIngameString( 410, 432, modeExtra,
				                     UI_RIGHT | UI_SMALLFONT, 0.55f, accentColor );
			}
		}
		Com_sprintf( modeExtra, sizeof(modeExtra), "PLAYERS %d", cgs.numRacers );
		CG_DrawIngameSmallString( 190, 450, modeExtra, mutedColor );
	}

	/* Compact race progress ruler at the bottom of the Race Status segment. */
	if ( showRaceProgress ) {
		float progressX;
		float progressWidth;
		vec4_t progressColor;
		vec4_t markerColor;
		int marker;

		progressX = HUD_X(190);
		progressWidth = ( raceCombatMode ? 100.0f : 220.0f ) * spreadFactor;
		Vector4Copy( accentColor, progressColor );
		progressColor[3] = 0.82f;
		Vector4Copy( lineColor, markerColor );
		markerColor[3] = 0.72f;
		CG_FillRect( progressX, 469, progressWidth, 4, darkSegment );
		if ( raceProgress > 0.0f ) {
			CG_FillRect( progressX, 469, progressWidth * raceProgress,
			             4, progressColor );
		}
		for ( marker = 1; marker < 4; marker++ ) {
			CG_FillRect( HUD_X(190 + ( raceCombatMode ? 100 : 220 ) * marker * 0.25f),
			             468, 1, 6, markerColor );
		}
	}

	/* Combat modes, CTF and LCS use a dedicated loadout area in the telemetry
	 * strip. The selection preview lives here instead of in the old carousel. */
	if ( showWeaponTelemetry ) {
		vec4_t weaponColor;
		qhandle_t weaponIcon;
		qboolean previewingWeapon;

		CG_FillRect( HUD_X(302), 422, 1, 50, lineColor );
		CG_DrawIngameSmallString( HUD_X(310), 414, "WEAPONS", mutedColor );
		Vector4Copy( colorWhite, weaponColor );

		frontWeapon = ps->weapon;
		previewingWeapon = ( cg.weaponSelectTime > 0 &&
		                     cg.time >= cg.weaponSelectTime &&
		                     cg.time - cg.weaponSelectTime < WEAPON_SELECT_TIME );
		if ( previewingWeapon && cg.weaponSelect > WP_NONE &&
		     cg.weaponSelect < RWP_SMOKE &&
		     ( ps->stats[STAT_WEAPONS] & ( 1u << cg.weaponSelect ) ) ) {
			frontWeapon = cg.weaponSelect;
			Vector4Copy( accentColor, weaponColor );
		}
		if ( frontWeapon <= WP_NONE || frontWeapon >= RWP_SMOKE ) {
			frontWeapon = 0;
		}

		CG_DrawIngameString( HUD_X(310), 430, "FRONT",
		                     UI_SMALLFONT, 0.40f, mutedColor );
		weaponIcon = 0;
		frontAmmo = -1;
		if ( frontWeapon > WP_NONE ) {
			CG_RegisterWeapon( frontWeapon );
			weaponIcon = cg_weapons[frontWeapon].weaponIcon;
			frontAmmo = ps->ammo[frontWeapon];
		}
		if ( weaponIcon ) {
			trap_R_SetColor( weaponColor );
			CG_DrawPic( HUD_X(350), 426, 16, 16, weaponIcon );
			trap_R_SetColor( NULL );
		}
		if ( frontAmmo >= 0 )
			Com_sprintf( ammoText, sizeof(ammoText), "%d", frontAmmo );
		else
			Q_strncpyz( ammoText, "--", sizeof(ammoText) );
		CG_DrawIngameString( HUD_X(424), 430, ammoText,
		                     UI_RIGHT | UI_SMALLFONT, 0.62f, weaponColor );

		rearWeapon = 0;
		fallbackRearWeapon = 0;
		for ( weaponIndex = RWP_SMOKE; weaponIndex < WP_NUM_WEAPONS; weaponIndex++ ) {
			if ( ps->stats[STAT_WEAPONS] & ( 1u << weaponIndex ) ) {
				if ( !fallbackRearWeapon )
					fallbackRearWeapon = weaponIndex;
				if ( ps->ammo[weaponIndex] > 0 ) {
					rearWeapon = weaponIndex;
					break;
				}
			}
		}
		if ( !rearWeapon )
			rearWeapon = fallbackRearWeapon;

		CG_DrawIngameString( HUD_X(310), 446, "REAR",
		                     UI_SMALLFONT, 0.40f, mutedColor );
		weaponIcon = 0;
		rearAmmo = -1;
		if ( rearWeapon > 0 ) {
			CG_RegisterWeapon( rearWeapon );
			weaponIcon = cg_weapons[rearWeapon].weaponIcon;
			rearAmmo = ps->ammo[rearWeapon];
		}
		if ( weaponIcon ) {
			trap_R_SetColor( colorWhite );
			CG_DrawPic( HUD_X(350), 442, 16, 16, weaponIcon );
			trap_R_SetColor( NULL );
		}
		if ( rearWeapon > 0 && rearAmmo >= 0 )
			Com_sprintf( ammoText, sizeof(ammoText), "%d", rearAmmo );
		else
			Q_strncpyz( ammoText, "--", sizeof(ammoText) );
		CG_DrawIngameString( HUD_X(424), 446, ammoText,
		                     UI_RIGHT | UI_SMALLFONT, 0.62f, colorWhite );

		CG_DrawIngameString( HUD_X(310), 462, "PWRUP",
		                     UI_SMALLFONT, 0.40f, mutedColor );
	}

	/* Serialize pickups and clean-sector rewards in one toast lane. */
	CG_DrawHudToast();

	/* Right: speed, a right-aligned unit/gear pair, and RPM. */
	if ( gear < 0 ) Q_strncpyz( gearText, "R", sizeof(gearText) );
	else if ( gear == 0 ) Q_strncpyz( gearText, "N", sizeof(gearText) );
	else Com_sprintf( gearText, sizeof(gearText), "%d", gear );
	Com_sprintf( fuelText, sizeof(fuelText), "FUEL %d%%", fuel );
	Com_sprintf( speedText, sizeof(speedText), "%d", speedValue );
	speedRight = HUD_X(594);
	vehicleInfoRight = HUD_X(630);
	CG_DrawIngameString( speedRight, 415, speedText, UI_RIGHT,
	                     1.2f, colorWhite );
	CG_DrawIngameString( vehicleInfoRight, 433,
	                     cg_metricUnits.integer ? "KPH" : "MPH",
	                     UI_RIGHT | UI_SMALLFONT, 0.75f, blueColor );
	gearValueWidth = CG_IngameStringWidth( gearText, UI_SMALLFONT, 0.75f );
	gearPrefixWidth = CG_IngameStringWidth( "GEAR ", UI_SMALLFONT, 0.75f );
	CG_DrawIngameString( vehicleInfoRight - gearValueWidth - gearPrefixWidth,
	                     451, "GEAR", UI_SMALLFONT, 0.75f, mutedColor );
	CG_DrawIngameString( vehicleInfoRight, 451, gearText,
	                     UI_RIGHT | UI_SMALLFONT, 0.75f, blueColor );
	CG_DrawIngameSmallString( HUD_X(450), 418, "NOS",
	                          turboActive ? accentColor : mutedColor );
	turboBarWidth = 72.0f;
	CG_FillRect( HUD_X(450), 431, turboBarWidth * spreadFactor, 5, darkSegment );
	if ( turboFrac > 0.0f ) {
		CG_FillRect( HUD_X(450), 431, turboBarWidth * turboFrac * spreadFactor, 5,
		             turboActive ? accentColor : blueColor );
	}
	CG_DrawIngameString( HUD_X(450), 441, fuelText, UI_SMALLFONT, 0.75f, mutedColor );
	fuelBarWidth = 72.0f;
	CG_FillRect( HUD_X(450), 454, fuelBarWidth * spreadFactor, 5, darkSegment );
	if ( fuelFrac > 0.0f ) {
		CG_FillRect( HUD_X(450), 454, fuelBarWidth * fuelFrac * spreadFactor, 5, blueColor );
	}
	CG_DrawIngameString( HUD_X(450), 466, "RPM", UI_SMALLFONT,
	                     0.75f, mutedColor );
	segmentW = 8.0f * spreadFactor;
	for ( i = 0; i < 15; i++ ) {
		vec4_t segmentColor;
		if ( i < rpmFrac * 15.0f ) {
			Vector4Copy( i >= 12 ? dangerColor : colorWhite, segmentColor );
		} else {
			Vector4Copy( darkSegment, segmentColor );
		}
		CG_FillRect( HUD_X(484 + i * 10), 469,
		             segmentW, 5, segmentColor );
	}

	/* Keep active powerups above the strip rather than under its background. */
	CG_DrawRallyPowerups( 399 );
	CG_SetScreenPlacement( savedHorizontalPlacement, savedVerticalPlacement );
#undef HUD_X
}

static void CG_DrawRallyStatusBar( void ) {
	int			color;
	centity_t	*cent;
	playerState_t	*ps;
	int			value;
	vec4_t		hcolor;
	vec3_t		angles;
	vec3_t		origin;
	qhandle_t	healthModel;
	vec4_t		bg_color;
	vec4_t		statusPanelColor;
	vec4_t		statusCellColor;
	vec4_t		statusAccentColor;
	int			weapon, i;

	if ( !cg_drawStatus.integer ) {
		return;
	}

	CG_DrawRallyTelemetryHud();
	return;

#if 0 /* legacy card status bar replaced by CG_DrawRallyTelemetryHud */
	CG_SetScreenPlacement(PLACE_CENTER, PLACE_BOTTOM);

// draw the dtf sigils

    if ( cgs.gametype == GT_DOMINATION )

    CG_DrawSigilHUD();

	cent = &cg_entities[cg.snap->ps.clientNum];
	ps = &cg.snap->ps;

	VectorClear( angles );

	statusPanelColor[0] = 0.018f;
	statusPanelColor[1] = 0.025f;
	statusPanelColor[2] = 0.030f;
	statusPanelColor[3] = 0.86f;
	statusCellColor[0] = 0.050f;
	statusCellColor[1] = 0.070f;
	statusCellColor[2] = 0.070f;
	statusCellColor[3] = 0.78f;

	switch (cgs.clientinfo[cg.snap->ps.clientNum].team){
	case TEAM_RED:
		Vector4Copy(colorRed, statusAccentColor);
		break;

	case TEAM_BLUE:
		Vector4Copy(colorBlue, statusAccentColor);
		break;

	case TEAM_GREEN:
		Vector4Copy(colorGreen, statusAccentColor);
		break;

	case TEAM_YELLOW:
		Vector4Copy(colorYellow, statusAccentColor);
		break;
	
	default:
		statusAccentColor[0] = 0.720f;
		statusAccentColor[1] = 1.000f;
		statusAccentColor[2] = 0.060f;
		statusAccentColor[3] = 1.00f;
	}

	CG_FillRect( 16, 440, 386, 34, statusPanelColor );
	CG_DrawRect( 16, 440, 386, 34, 1.0f, statusAccentColor );
	CG_FillRect( 16, 440, 386, 2, statusAccentColor );
	CG_FillRect( 20, 446, 90, 24, statusCellColor );
	CG_FillRect( 115, 446, 90, 24, statusCellColor );
	CG_FillRect( 210, 446, 90, 24, statusCellColor );
	CG_FillRect( 305, 446, 90, 24, statusCellColor );
	Vector4Copy( statusCellColor, bg_color );

	// draw ammo background
	value = ps->ammo[cent->currentState.weapon];
	if ( cent->currentState.weapon && value > -1 )
		CG_FillRect( 20, 476 - 30, 90, 24, bg_color );

	// health background
	   CG_FillRect( 210, 476 - 30, 90, 24, bg_color );

       // armor background
       if ( ps->stats[ STAT_ARMOR ] )

       CG_FillRect( 305, 476 - 30, 90, 24, bg_color );

       // rearammo background
       weapon = 0;
for (i = RWP_SMOKE; i < WP_NUM_WEAPONS; i++){
if (ps->stats[STAT_WEAPONS] & ( 1u << i )){
			if (ps->ammo[ i ]){
				weapon = i;
				break;
			}
		}
	}

	if ( weapon )
		CG_FillRect( 115, 476 - 30, 90, 24, bg_color );

	// draw any 3D icons now, so the changes back to 2D are minimized
	if ( cent->currentState.weapon && cg_weapons[ cent->currentState.weapon ].ammoModel ) {
		origin[0] = 70;
		origin[1] = 0;
		origin[2] = 0;
		angles[YAW] = 270 * sin( cg.time / 1000.0 );
		CG_Draw3DModel( 26, 476 - 28, 19, 19,
					   cg_weapons[ cent->currentState.weapon ].ammoModel, 0, origin, angles );
	}

	// Replace head with '+' sign.
	VectorClear( angles );
	if (ps->stats[STAT_HEALTH] < 25)
		healthModel = trap_R_RegisterModel( "models/powerups/health/large_cross.md3" );
	else if (ps->stats[STAT_HEALTH] < 50)
		healthModel = trap_R_RegisterModel( "models/powerups/health/medium_cross.md3" );
	else
		healthModel = trap_R_RegisterModel( "models/powerups/health/small_cross.md3" );

	if ( healthModel ) {
		origin[0] = 68;
		origin[1] = 0;
		origin[2] = 0;
		angles[YAW] = 270 * sin( cg.time / 1000.0 );
		CG_Draw3DModel( 212, 476 - 26, 19, 19,
					   healthModel, 0, origin, angles );
	}

	CG_DrawRallyPowerups( 476 );

	if (cg.predictedPlayerState.powerups[PW_REDFLAG])
		CG_DrawStatusBarFlag( 495, TEAM_RED);
	else if (cg.predictedPlayerState.powerups[PW_BLUEFLAG])
		CG_DrawStatusBarFlag( 495, TEAM_BLUE);
	else if (cg.predictedPlayerState.powerups[PW_GREENFLAG])
		CG_DrawStatusBarFlag( 495, TEAM_GREEN);
	else if (cg.predictedPlayerState.powerups[PW_YELLOWFLAG])
		CG_DrawStatusBarFlag( 495, TEAM_YELLOW);
	else if (cg_entities[cg.snap->ps.clientNum].finishRaceTime &&
		cg_entities[cg.snap->ps.clientNum].currentPosition == 1){

		CG_DrawStatusBarFlag( 495, 15);
	}

	if ( ps->stats[ STAT_ARMOR ] ) {
		origin[0] = 45;
		origin[1] = 0;
		origin[2] = -10;
		angles[YAW] = 270 * sin( cg.time / 1000.0 );
//		CG_Draw3DModel( 196, 476 - 64, 26, 26,
        CG_Draw3DModel( 311, 476 - 28, 19, 19,
					   cgs.media.armorModel, 0, origin, angles );
	}


	//
	// Draw numbers and 2D icons now
	//
	CG_DrawFrontendString( 48, 450, "AMMO",
	                       UI_SMALLFONT | UI_DROPSHADOW, 1.0f, colorWhite );
	CG_DrawFrontendString( 143, 450, "REAR",
	                       UI_SMALLFONT | UI_DROPSHADOW, 1.0f, colorWhite );
	CG_DrawFrontendString( 238, 450, "HEALTH",
	                       UI_SMALLFONT | UI_DROPSHADOW, 1.0f, colorWhite );
	CG_DrawFrontendString( 333, 450, "ARMOR",
	                       UI_SMALLFONT | UI_DROPSHADOW, 1.0f, colorWhite );

	//
	// ammo
	//
	if ( cent->currentState.weapon ) {
		value = ps->ammo[cent->currentState.weapon];
		if ( value > -1 ) {
			if ( cg.predictedPlayerState.weaponstate == WEAPON_FIRING
				&& (cg.predictedPlayerState.weaponTime & NORMAL_WEAPON_TIME_MASK) > 100 ) {
				// draw as dark grey when reloading
				color = 2;	// dark grey
			} else {
				if ( value >= 0 ) {
					color = 0;	// green
				} else {
					color = 1;	// red
				}
			}
			trap_R_SetColor( colors[color] );
			
			CG_DrawFrontendString( 102, 450, va("%i", value),
			                       UI_RIGHT | UI_SMALLFONT | UI_DROPSHADOW,
			                       1.0f, colors[color] );
			trap_R_SetColor( NULL );

			// if we didn't draw a 3D icon, draw a 2D icon for ammo
			if ( !cg_draw3dIcons.integer && cg_drawIcons.integer ) {
				qhandle_t	icon;

				icon = cg_weapons[ cg.predictedPlayerState.weapon ].ammoIcon;
				if ( icon ) {
					CG_DrawPic( 10, 476 - 28, 26, 26, icon );
				}
			}
		}
	}


	//
	// rear ammo
	//
	if ( weapon && cg_weapons[ weapon ].weaponModel ) {
		origin[0] = 70;
		origin[1] = 0;
		origin[2] = 0;
		angles[YAW] = 270 * sin( cg.time / 1000.0 );
		CG_Draw3DModel( 120, 476 - 28, 19, 19,
					   cg_weapons[ weapon ].weaponModel, 0, origin, angles );
	}

	if ( weapon ) {
		value = ps->ammo[weapon];
		if ( value > -1 ) {
			if ( cg.predictedPlayerState.weaponstate == WEAPON_REARFIRING
				&& ((cg.predictedPlayerState.weaponTime & REAR_WEAPON_TIME_MASK) >> 16) > 100 ) {
				// draw as dark grey when reloading
				color = 2;	// dark grey
			} else {
				if ( value >= 0 ) {
					color = 0;	// green
				} else {
					color = 1;	// red
				}
			}
			trap_R_SetColor( colors[color] );
			
			CG_DrawFrontendString( 197, 450, va("%i", value),
			                       UI_RIGHT | UI_SMALLFONT | UI_DROPSHADOW,
			                       1.0f, colors[color] );
			trap_R_SetColor( NULL );

			// if we didn't draw a 3D icon, draw a 2D icon for ammo
			if ( !cg_draw3dIcons.integer && cg_drawIcons.integer ) {
				qhandle_t	icon;

				icon = cg_weapons[ weapon ].weaponIcon;
				if ( icon ) {
					CG_DrawPic( 6, 480 - 67, 26, 26, icon );
				}
			}
		}
	}


	//
	// health
	//
	value = ps->stats[STAT_HEALTH];
	if ( value > 100 ) {
		trap_R_SetColor( colors[3] );		// white
	} else if (value > 25) {
		trap_R_SetColor( colors[0] );	// green
	} else if (value > 0) {
		color = (cg.time >> 8) & 1;	// flash
		trap_R_SetColor( colors[color] );
	} else {
		trap_R_SetColor( colors[1] );	// red
	}

	// stretch the health up when taking damage
	CG_ColorForHealth( hcolor );
	trap_R_SetColor( hcolor );
	CG_DrawFrontendString( 292, 450, va("%i", value),
	                       UI_RIGHT | UI_SMALLFONT | UI_DROPSHADOW,
	                       1.0f, hcolor );


	//
	// armor
	//
	value = ps->stats[STAT_ARMOR];
	if (value > 0 ) {
		trap_R_SetColor( colors[0] );
//		CG_DrawField ( 242, 476 - 64, 3, value);
		CG_DrawFrontendString( 387, 450, va("%i", value),
		                       UI_RIGHT | UI_SMALLFONT | UI_DROPSHADOW,
		                       1.0f, colors[0] );
		trap_R_SetColor( NULL );
		// if we didn't draw a 3D icon, draw a 2D icon for armor
		if ( !cg_draw3dIcons.integer && cg_drawIcons.integer ) {
			CG_DrawPic( 196, 476 - 64, 26, 26, cgs.media.armorIcon );
		}
	}
}
#endif
}
#endif

/*
================
CG_DrawPowerups
================

#ifndef MISSIONPACK
#if 0 // ZTM: Not used by Q3Rally
static float CG_DrawPowerups( float y ) {
	int		sorted[MAX_POWERUPS];
	int		sortedTime[MAX_POWERUPS];
	int		i, j, k;
	int		active;
	playerState_t	*ps;
	int		t;
	gitem_t	*item;
	int		x;
	int		color;
	float	size;
	float	f;
	static float colors[2][4] = { 
    { 0.2f, 1.0f, 0.2f, 1.0f } , 
    { 1.0f, 0.2f, 0.2f, 1.0f } 
  };

	ps = &cg.snap->ps;

	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		return y;
	}

	// sort the list by time remaining
	active = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( !ps->powerups[ i ] ) {
			continue;
		}

		// ZOID--don't draw if the power up has unlimited time
		// This is true of the CTF flags
		if ( ps->powerups[ i ] == INT_MAX ) {
			continue;
		}

		t = ps->powerups[ i ] - cg.time;
		if ( t <= 0 ) {
			continue;
		}

		// insert into the list
		for ( j = 0 ; j < active ; j++ ) {
			if ( sortedTime[j] >= t ) {
				for ( k = active - 1 ; k >= j ; k-- ) {
					sorted[k+1] = sorted[k];
					sortedTime[k+1] = sortedTime[k];
				}
				break;
			}
		}
		sorted[j] = i;
		sortedTime[j] = t;
		active++;
	}

	// draw the icons and timers
	x = 640 - ICON_SIZE - CHAR_WIDTH * 2;
	for ( i = 0 ; i < active ; i++ ) {
		item = BG_FindItemForPowerup( sorted[i] );

    if (item) {

		  color = 1;

		  y -= ICON_SIZE;

		  trap_R_SetColor( colors[color] );
		  CG_DrawField( x, y, 2, sortedTime[ i ] / 1000 );

		  t = ps->powerups[ sorted[i] ];
		  if ( t - cg.time >= POWERUP_BLINKS * POWERUP_BLINK_TIME ) {
			  trap_R_SetColor( NULL );
		  } else {
			  vec4_t	modulate;

			  f = (float)( t - cg.time ) / POWERUP_BLINK_TIME;
			  f -= (int)f;
			  modulate[0] = modulate[1] = modulate[2] = modulate[3] = f;
			  trap_R_SetColor( modulate );
		  }

		  if ( cg.powerupActive == sorted[i] && 
			  cg.time - cg.powerupTime < PULSE_TIME ) {
			  f = 1.0 - ( ( (float)cg.time - cg.powerupTime ) / PULSE_TIME );
			  size = ICON_SIZE * ( 1.0 + ( PULSE_SCALE - 1.0 ) * f );
		  } else {
			  size = ICON_SIZE;
		  }

		  CG_DrawPic( 640 - size, y + ICON_SIZE / 2 - size / 2, 
			  size, size, trap_R_RegisterShader( item->icon ) );
    }
	}
	trap_R_SetColor( NULL );

	return y;
}
#endif

*/

/*
====================
CG_DrawRallyPowerups
====================
*/
static float CG_DrawRallyPowerups( float y ) {
	int		sorted[MAX_POWERUPS];
	int		sortedTime[MAX_POWERUPS];
	int		i, j, k;
	int		displayPowerupIndex;
	int		active;
	int		telemetryRows;
	playerState_t	*ps;
	int		t;
	gitem_t	*item;
	int		color;
	float	size;
	float	f;
	float	powerupY;
	float	powerupX;
	qboolean raceTelemetryMode;
	qboolean loadoutTelemetryMode;
	float	spreadFactor;
    float	x;
	char	powerupTimeText[8];
	vec3_t	origin;
	vec3_t	angles;
	vec4_t	bg_color;

	static float colors[2][4] = { 
		{ 0.2f, 1.0f, 0.2f, 1.0f } ,
		{ 1.0f, 0.2f, 0.2f, 1.0f }
	};

	ps = &cg.snap->ps;

	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		return y;
	}

	switch (cgs.clientinfo[cg.snap->ps.clientNum].team){
	case TEAM_RED:
		Vector4Copy(colorRed, bg_color);
		bg_color[3] = 0.5f;
		break;

	case TEAM_BLUE:
		Vector4Copy(colorBlue, bg_color);
		bg_color[3] = 0.5f;
		break;

	case TEAM_GREEN:
		Vector4Copy(colorGreen, bg_color);
		bg_color[3] = 0.5f;
		break;

	case TEAM_YELLOW:
		Vector4Copy(colorYellow, bg_color);
		bg_color[3] = 0.5f;
		break;
	
	default:
		Vector4Copy(bgColor, bg_color);
	}

	// sort the list by time remaining
	active = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( !ps->powerups[ i ] ) {
			continue;
		}
		t = ps->powerups[ i ] - cg.time;

		if (i == PW_TURBO && ps->powerups[ i ] < 0)
			t = -ps->powerups[ i ];

		// ZOID--don't draw if the power up has unlimited time (999 seconds)
		// This is true of the CTF flags
		if ( t < 0 || t > 999000 ) {
			continue;
		}

		// insert into the list
		for ( j = 0 ; j < active ; j++ ) {
			if ( sortedTime[j] <= t ) {
				for ( k = active - 1 ; k >= j ; k-- ) {
					sorted[k+1] = sorted[k];
					sortedTime[k+1] = sortedTime[k];
				}
				break;
			}
		}
		sorted[j] = i;
		sortedTime[j] = t;
		active++;
	}

	// draw the icons and timers
	raceTelemetryMode = ( cgs.gametype == GT_RACING ||
		cgs.gametype == GT_RACING_DM || cgs.gametype == GT_SPRINT ||
		cgs.gametype == GT_TEAM_RACING ||
		cgs.gametype == GT_TEAM_RACING_DM ||
		cgs.gametype == GT_SINGLE_PLAYER );
	loadoutTelemetryMode = ( cgs.gametype == GT_DEATHMATCH ||
		cgs.gametype == GT_RACING_DM ||
		cgs.gametype == GT_TEAM_RACING_DM ||
		cgs.gametype == GT_CTF ||
		cgs.gametype == GT_CTF4 ||
		cgs.gametype == GT_DOMINATION ||
		cgs.gametype == GT_KOTH );
	spreadFactor = 1.0f;
	if ( ( cgs.gametype == GT_RACING_DM || cgs.gametype == GT_TEAM_RACING_DM ) &&
	     !cg_stretch.integer && cgs.screenXScale > 0.0f ) {
		spreadFactor = cgs.screenXScaleStretch / cgs.screenXScale;
		if ( spreadFactor < 1.0f )
			spreadFactor = 1.0f;
	}
	telemetryRows = 0;
	displayPowerupIndex = -1;
	if ( loadoutTelemetryMode ) {
		/* The compact HUD has one powerup slot; prioritize the effect nearest
		 * expiration so the most time-sensitive active powerup stays visible. */
		for ( i = active - 1; i >= 0; i-- ) {
			if ( sorted[i] == PW_TURBO )
				continue;
			item = BG_FindItemForPowerup( sorted[i] );
			if ( item && item->icon ) {
				displayPowerupIndex = i;
				break;
			}
		}
	}
    x = 402;  // Start X position for horizontal layout
	for ( i = 0 ; i < active ; i++ ) {
		/* Turbo is represented by the telemetry gauge; don't draw the legacy
		 * powerup icon and timer a second time. */
		if ( sorted[i] == PW_TURBO ) {
			continue;
		}
		item = BG_FindItemForPowerup( sorted[i] );
		if ( loadoutTelemetryMode ) {
			/* Render the selected effect in the dedicated weapon/loadout row. */
			if ( i != displayPowerupIndex || !item || !item->icon ) {
				continue;
			}
			powerupX = CG_RallyTelemetryX( 350.0f, spreadFactor );
			powerupY = 458.0f;
			trap_R_SetColor( NULL );
			CG_DrawPic( powerupX, powerupY, 16.0f, 16.0f,
					trap_R_RegisterShader( item->icon ) );
			Com_sprintf( powerupTimeText, sizeof( powerupTimeText ), "%dS",
							( sortedTime[i] + 999 ) / 1000 );
			CG_DrawIngameString( CG_RallyTelemetryX( 424.0f, spreadFactor ),
					462.0f, powerupTimeText, UI_RIGHT | UI_SMALLFONT,
					0.62f, colorWhite );
			telemetryRows++;
			continue;
		}
		if ( raceTelemetryMode ) {
			/* Non-combat racing rotations include Quad and Auto Repair.
			 * Keep their compact indicators beneath Armor in the new HUD. */
			if ( sorted[i] < PW_QUAD || sorted[i] > PW_SHIELD ||
			     telemetryRows >= 2 || !item || !item->icon ) {
				continue;
			}
			powerupY = 448.0f + telemetryRows * 11.0f;
			trap_R_SetColor( NULL );
			CG_DrawPic( CG_RallyTelemetryX( 114, spreadFactor ), powerupY, 8, 8,
			            trap_R_RegisterShader( item->icon ) );
			Com_sprintf( powerupTimeText, sizeof(powerupTimeText), "%dS",
			             ( sortedTime[i] + 999 ) / 1000 );
			CG_DrawIngameString( CG_RallyTelemetryX( 166, spreadFactor ), powerupY, powerupTimeText,
			                     UI_RIGHT | UI_SMALLFONT, 0.55f, colorWhite );
			telemetryRows++;
			continue;
		}

		color = 1;

		CG_FillRect( x, y - 30, 90, 24, bg_color );

		trap_R_SetColor( colors[color] );
		CG_DrawField( x + 22 + CHAR_WIDTH, y - 28, 2, sortedTime[ i ] / 1000 );

		t = ps->powerups[ sorted[i] ];
		if ( t - cg.time >= POWERUP_BLINKS * POWERUP_BLINK_TIME ) {
			trap_R_SetColor( NULL );
		}
		// dont blink turbo when not on
		else if (item->giTag == PW_TURBO && ps->powerups[ item->giTag ] < 0){
			trap_R_SetColor( NULL );
		}
		else {
			vec4_t	modulate;

			f = (float)( t - cg.time ) / POWERUP_BLINK_TIME;
			f -= (int)f;
			modulate[0] = modulate[1] = modulate[2] = modulate[3] = f;
			trap_R_SetColor( modulate );
		}

		if ( cg.powerupActive == sorted[i] && 
			cg.time - cg.powerupTime < PULSE_TIME ) {
			f = 1.0 - ( ( (float)cg.time - cg.powerupTime ) / PULSE_TIME );
			size = 19 * ( 1.0 + ( PULSE_SCALE - 1.0 ) * f );
		} else {
			size = 19;
		}

		// Setup 3D model positioning and rotation
		origin[0] = 68;
		origin[1] = 0;
		origin[2] = 0;
		VectorClear( angles );
		angles[YAW] = 270 * sin( cg.time / 1000.0 );
		
		CG_Draw3DModel( x + 6, y - 27, size, size, trap_R_RegisterModel( item->world_model[0] ), 0, origin, angles );
		x += 96;  // Move to next horizontal position (90 width + 6 spacing)
	}
	trap_R_SetColor( NULL );
    return y;
}


/*
===========================================================================================

 UPPER RIGHT CORNER

===========================================================================================
*/

/*
================
CG_DrawAttacker
================
*/
static float CG_DrawAttacker( float y ) {
	int			t;
	float		size;
// Q3Rally Code
	//vec3_t		angles;
// Q3Rally Code END
	const char	*info;
	const char	*name;
	int			clientNum;

	if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 ) {
		return y;
	}

	if ( !cg.attackerTime ) {
		return y;
	}

	clientNum = cg.predictedPlayerState.persistant[PERS_ATTACKER];
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS || clientNum == cg.snap->ps.clientNum ) {
		return y;
	}

	if ( !cgs.clientinfo[clientNum].infoValid ) {
		cg.attackerTime = 0;
		return y;
	}

	t = cg.time - cg.attackerTime;
	if ( t > ATTACKER_HEAD_TIME ) {
		cg.attackerTime = 0;
		return y;
	}

	size = ICON_SIZE * 1.25;

// Q3Rally Code ( removed function )
//	angles[PITCH] = 0;
//	angles[YAW] = 180;
//	angles[ROLL] = 0;
//	CG_DrawHead( 640 - size, y, size, size, clientNum, angles );
// Q3Rally Code END

	info = CG_ConfigString( CS_PLAYERS + clientNum );
	name = Info_ValueForKey(  info, "n" );
	y += size;
	CG_DrawBigString( 640 - ( Q_PrintStrlen( name ) * BIGCHAR_WIDTH), y, name, 0.5 );

	return y + BIGCHAR_HEIGHT + 2;
}

/*
===============
CG_DrawSnapshot
===============
*/
static float CG_DrawSnapshot( float y ) {
	char		*s;
	int			w;

	s = va( "time:%i snap:%i cmd:%i", cg.snap->serverTime, 
		cg.latestSnapshotNum, cgs.serverCommandSequence );
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

	CG_DrawBigString( 635 - w, y + 2, s, 1.0F);

	return y + BIGCHAR_HEIGHT + 4;
}

/*
==========
CG_DrawFPS
==========
*/
#define	FPS_FRAMES	4
static float CG_DrawFPS( float y ) {
	char		*s;
	int			w;
	static int	previousTimes[FPS_FRAMES];
	static int	index;
	int		i, total;
	int		fps;
	static	int	previous;
	int		t, frameTime;

	// don't use serverTime, because that will be drifting to
	// correct for internet lag changes, timescales, timedemos, etc
	t = trap_Milliseconds();
	frameTime = t - previous;
	previous = t;

	previousTimes[index % FPS_FRAMES] = frameTime;
	index++;
	if ( index > FPS_FRAMES ) {
		// average multiple frames together to smooth changes out a bit
		total = 0;
		for ( i = 0 ; i < FPS_FRAMES ; i++ ) {
			total += previousTimes[i];
		}
		if ( !total ) {
			total = 1;
		}
		fps = 1000 * FPS_FRAMES / total;

		s = va( "%ifps", fps );
		w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

		CG_DrawBigString( 621 - w, 355 - 25 + 2, s, 1.0F );
	}

	return y + BIGCHAR_HEIGHT + 4;
}

/*
============
CG_DrawTimer
============
*/
static float CG_DrawTimer( float y ) {
	char		*s;
	int			w;
	int			mins, seconds, tens;
	int			msec;

	msec = cg.time - cgs.levelStartTime;

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	s = va( "%i:%i%i", mins, tens, seconds );
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

	CG_DrawBigString( 635 - w, y + 2, s, 1.0F);

	return y + BIGCHAR_HEIGHT + 4;
}


/*
==================
CG_DrawTeamOverlay
==================
*/

static float CG_DrawTeamOverlay( float y, qboolean right, qboolean upper ) {
	int x, w, h, xx;
	int i, j, len;
	const char *p;
	vec4_t		hcolor;
	int pwidth, lwidth;
	int plyrs;
	char st[16];
	clientInfo_t *ci;
	gitem_t	*item;
	int ret_y, count;

	if ( !cg_drawTeamOverlay.integer ) {
		return y;
	}

	if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_RED && cg.snap->ps.persistant[PERS_TEAM] != TEAM_BLUE ) {
		return y; // Not on any team
	}

	plyrs = 0;

	// max player name width
	pwidth = 0;
	count = (numSortedTeamPlayers > 8) ? 8 : numSortedTeamPlayers;
	for (i = 0; i < count; i++) {
		ci = cgs.clientinfo + sortedTeamPlayers[i];
		if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM]) {
			plyrs++;
			len = CG_DrawStrlen(ci->name);
			if (len > pwidth)
				pwidth = len;
		}
	}

	if (!plyrs)
		return y;

	if (pwidth > TEAM_OVERLAY_MAXNAME_WIDTH)
		pwidth = TEAM_OVERLAY_MAXNAME_WIDTH;

	// max location name width
	lwidth = 0;
	for (i = 1; i < MAX_LOCATIONS; i++) {
		p = CG_ConfigString(CS_LOCATIONS + i);
		if (p && *p) {
			len = CG_DrawStrlen(p);
			if (len > lwidth)
				lwidth = len;
		}
	}

	if (lwidth > TEAM_OVERLAY_MAXLOCATION_WIDTH)
		lwidth = TEAM_OVERLAY_MAXLOCATION_WIDTH;

	w = (pwidth + lwidth + 4 + 7) * TINYCHAR_WIDTH;

	if ( right )
		x = 640 - w;
	else
		x = 0;

	h = plyrs * TINYCHAR_HEIGHT;

	if ( upper ) {
		ret_y = y + h;
	} else {
		y -= h;
		ret_y = y;
	}

	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
		hcolor[0] = 1.0f;
		hcolor[1] = 0.0f;
		hcolor[2] = 0.0f;
		hcolor[3] = 0.33f;
	} else { // if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE )
		hcolor[0] = 0.0f;
		hcolor[1] = 0.0f;
		hcolor[2] = 1.0f;
		hcolor[3] = 0.33f;
	}
	trap_R_SetColor( hcolor );
	CG_DrawPic( x, y, w, h, cgs.media.teamStatusBar );
	trap_R_SetColor( NULL );

	for (i = 0; i < count; i++) {
		ci = cgs.clientinfo + sortedTeamPlayers[i];
		if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM]) {

			hcolor[0] = hcolor[1] = hcolor[2] = hcolor[3] = 1.0;

			xx = x + TINYCHAR_WIDTH;

			CG_DrawStringExt( xx, y,
				ci->name, hcolor, qfalse, qfalse,
				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, TEAM_OVERLAY_MAXNAME_WIDTH);

			if (lwidth) {
				p = CG_ConfigString(CS_LOCATIONS + ci->location);
				if (!p || !*p)
					p = "unknown";
//				len = CG_DrawStrlen(p);
//				if (len > lwidth)
//					len = lwidth;

//				xx = x + TINYCHAR_WIDTH * 2 + TINYCHAR_WIDTH * pwidth + 
//					((lwidth/2 - len/2) * TINYCHAR_WIDTH);
				xx = x + TINYCHAR_WIDTH * 2 + TINYCHAR_WIDTH * pwidth;
				CG_DrawStringExt( xx, y,
					p, hcolor, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
					TEAM_OVERLAY_MAXLOCATION_WIDTH);
			}

			CG_GetColorForHealth( ci->health, ci->armor, hcolor );

			Com_sprintf (st, sizeof(st), "%3i %3i", ci->health,	ci->armor);

			xx = x + TINYCHAR_WIDTH * 3 + 
				TINYCHAR_WIDTH * pwidth + TINYCHAR_WIDTH * lwidth;

			CG_DrawStringExt( xx, y,
				st, hcolor, qfalse, qfalse,
				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );

			// draw weapon icon
			xx += TINYCHAR_WIDTH * 3;

			if ( cg_weapons[ci->curWeapon].weaponIcon ) {
				CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 
					cg_weapons[ci->curWeapon].weaponIcon );
			} else {
				CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 
					cgs.media.deferShader );
			}

			// Draw powerup icons
			if (right) {
				xx = x;
			} else {
				xx = x + w - TINYCHAR_WIDTH;
			}
			for (j = 0; j <= PW_NUM_POWERUPS; j++) {
				if (ci->powerups & (1 << j)) {

					item = BG_FindItemForPowerup( j );

					if (item) {
						CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 
						trap_R_RegisterShader( item->icon ) );
						if (right) {
							xx -= TINYCHAR_WIDTH;
						} else {
							xx += TINYCHAR_WIDTH;
						}
					}
				}
			}

			y += TINYCHAR_HEIGHT;
		}
	}

	return ret_y;
//#endif
}


/*
=================
CG_DrawUpperRight
=================
*/
static void CG_DrawUpperRight(stereoFrame_t stereoFrame)
{
	float	y;

	y = 0;
    
    CG_SetScreenPlacement(PLACE_RIGHT, PLACE_TOP);

	y = CG_DrawUpperRightHUD( y );

	if ( cgs.gametype != GT_CTF && cgs.gametype != GT_CTF4 &&
	     cgs.gametype != GT_DOMINATION &&
	     cgs.gametype != GT_KOTH &&
	     cgs.gametype > GT_TEAM &&
	     cg_drawTeamOverlay.integer == 1 ) {
		y = CG_DrawTeamOverlay( y, qtrue, qtrue );
	} 
	if ( cg_drawSnapshot.integer ) {
		y = CG_DrawSnapshot( y );
	}
	if (cg_drawFPS.integer && (stereoFrame == STEREO_CENTER || stereoFrame == STEREO_RIGHT)) {
		y = CG_DrawFPS( y );
	}
	if ( cg_drawTimer.integer ) {
		y = CG_DrawTimer( y );
	}
	if ( cg_drawAttacker.integer ) {
		CG_DrawAttacker( y );
	}

}

/*
===========================================================================================

  LOWER RIGHT CORNER

===========================================================================================
*/

/*
================================================
CG_DrawScores - Draw the small two score display
================================================
*/
#ifndef MISSIONPACK

float CG_DrawScores( float x, float y ) {

	const char	*s;
	int			s1, s2, score;
	int			s3, s4, w;
	int			v;
	vec4_t		color;
	float		y1;
	gitem_t		*item;

	if ( !cg_drawScores.integer ) {
		return y;
	}

	s1 = cgs.scores1;
	s2 = cgs.scores2;
	s3 = cgs.scores3;
	s4 = cgs.scores4;

	y1 = y + 18;

	if ( cgs.gametype == GT_SINGLE_PLAYER ) {
		const char	*totalString;
		const char	*bestLapString;
		int		finishTime;
		int		runningTime;
		centity_t	*cent;

		cent = &cg_entities[cg.clientNum];
		finishTime = cent->finishRaceTime ? cent->finishRaceTime : cg.time;
		runningTime = ( cent->startRaceTime && finishTime > cent->startRaceTime ) ? finishTime - cent->startRaceTime : 0;

		CG_FillRect( x - 80, y, 96, 18, bgColor );

		totalString = runningTime > 0 ? getStringForTimePrecise( runningTime ) : "--:--.--";
		bestLapString = cent->bestLapTime > 0 ? getStringForTimePrecise( cent->bestLapTime ) : "--:--.--";

		s = va( "TIME %s", totalString );
		w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
		x -= w;
		CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

		s = va( "BEST %s", bestLapString );
		w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
		x -= w;
		CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

		return y;
	}

	// draw from the right side to left
	if ( cgs.gametype >= GT_TEAM ) {

		CG_FillRect( x - 80, y, 96, 18, bgColor );

        if ( cgs.gametype >= GT_TEAM && cgs.gametype != GT_KOTH ) {
			// draw yellow
			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 0.0f;
			color[3] = 0.33f;
			s = va( "%2i", s4 );
			w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
			x -= w;
			CG_FillRect( x+1, y+1, w-2, 16, color );
			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_YELLOW ) {
				CG_DrawPic( x+1, y+1, w-2, 16, cgs.media.selectShader );
			}
			CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

			if( cgs.gametype == GT_CTF4 ) {
				if (cgs.yellowflag >= 0 && cgs.yellowflag <= 2) {
					item = BG_FindItemForPowerup(PW_YELLOWFLAG);
					if (item) {
						y1 = y + TINYCHAR_HEIGHT + 8;
						CG_DrawPic( x+1, y1+1, w-2, 16, cgs.media.yellowFlagShader[cgs.yellowflag] );
					}
				}
			}

			// draw green
			color[0] = 0.0f;
			color[1] = 1.0f;
			color[2] = 0.0f;
			color[3] = 0.33f;
			s = va( "%2i", s3 );
			w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
			x -= w;
			CG_FillRect( x+1, y+1, w-2, 16, color );
			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_GREEN ) {
				CG_DrawPic( x+1, y+1, w-2, 16, cgs.media.selectShader );
			}
			CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

			if( cgs.gametype == GT_CTF4 ) {
				if (cgs.greenflag >= 0 && cgs.greenflag <= 2) {
					item = BG_FindItemForPowerup(PW_GREENFLAG);
					if (item) {
						y1 = y + TINYCHAR_HEIGHT + 8;
						CG_DrawPic( x+1, y1+1, w-2, 16, cgs.media.greenFlagShader[cgs.greenflag] );
					}
				}
			}
		}

		color[0] = 0.0f;
		color[1] = 0.0f;
		color[2] = 1.0f;
		color[3] = 0.33f;
		s = va( "%2i", s2 );
		w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
		x -= w;
        
		CG_FillRect( x+1, y+1, w-2, 16, color );
		if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
			CG_DrawPic( x+1, y+1, w-2, 16, cgs.media.selectShader );

		}

		CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

		if ( cgs.gametype == GT_CTF || cgs.gametype == GT_CTF4 ) {
			if (cgs.blueflag >= 0 && cgs.blueflag <= 2) {
				item = BG_FindItemForPowerup(PW_BLUEFLAG);
				if (item) {
					y1 = y + TINYCHAR_HEIGHT + 8;
					CG_DrawPic( x+1, y1+1, w-2, 16, cgs.media.blueFlagShader[cgs.blueflag] );
				}
			}
		}

		color[0] = 1.0f;
		color[1] = 0.0f;
		color[2] = 0.0f;
		color[3] = 0.33f;
		s = va( "%2i", s1 );
		w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
		x -= w;

		CG_FillRect( x+1, y+1, w-2, 16, color );
		if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {

			CG_DrawPic( x+1, y+1, w-2, 16, cgs.media.selectShader );

		}

		CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

		if ( cgs.gametype == GT_CTF || cgs.gametype == GT_CTF4 ) {
			if (cgs.redflag >= 0 && cgs.redflag <= 2) {
				item = BG_FindItemForPowerup(PW_REDFLAG);
				if (item) {
					y1 = y + TINYCHAR_HEIGHT + 8;
					CG_DrawPic( x+1, y1+1, w-2, 16, cgs.media.redFlagShader[cgs.redflag] );
				}
			}
		}

#ifdef MISSIONPACK
		if ( cgs.gametype == GT_1FCTF ) {
			// Display flag status
			item = BG_FindItemForPowerup( PW_NEUTRALFLAG );

			if (item) {
				y1 = y - BIGCHAR_HEIGHT - 8;
				if( cgs.flagStatus >= 0 && cgs.flagStatus <= 3 ) {
					CG_DrawPic( x, y1-4, w, BIGCHAR_HEIGHT+8, cgs.media.flagShader[cgs.flagStatus] );
				}
			}
		}
#endif
		if ( cgs.gametype >= GT_CTF ) {
			v = cgs.capturelimit;
		} else {
			v = cgs.fraglimit;
		}

		if ( v && !isRallyRace()) {

			s = va( "%2i", v );
			w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
			x -= w;

			CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

		}

	} else {
		qboolean	spectator;

		CG_FillRect( x - 80, y, 96, 18, bgColor );

		score = cg.snap->ps.persistant[PERS_SCORE];
		spectator = ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR );

		// always show your score in the second box if not in first place
		if ( s1 != score ) {
			s2 = score;
		}
		if ( s2 != SCORE_NOT_PRESENT ) {
			s = va( "%2i", s2 );
			w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
			x -= w;
			if ( !spectator && score == s2 && score != s1 ) {
				color[0] = 1.0f;
				color[1] = 0.0f;
				color[2] = 0.0f;
				color[3] = 0.33f;

				CG_FillRect( x+1, y+1, w-2, 16, color );
				CG_DrawPic( x+1, y+1, w-2, 16, cgs.media.selectShader );

			} else {
				color[0] = 0.5f;
				color[1] = 0.5f;
				color[2] = 0.5f;
				color[3] = 0.33f;

				CG_FillRect( x+1, y+1, w-2, 16, color );

			}	

			CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

		}

		// first place
		if ( s1 != SCORE_NOT_PRESENT ) {
			s = va( "%2i", s1 );
			w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
			x -= w;
			if ( !spectator && score == s1 ) {
				color[0] = 0.0f;
				color[1] = 0.0f;
				color[2] = 1.0f;
				color[3] = 0.33f;

				CG_FillRect( x+1, y+1, w-2, 16, color );
				CG_DrawPic( x+1, y+1, w-2, 16, cgs.media.selectShader );

			} else {
				color[0] = 0.5f;
				color[1] = 0.5f;
				color[2] = 0.5f;
				color[3] = 0.33f;

				CG_FillRect( x+1, y+1, w-2, 16, color );

			}	

			CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

		}

		if ( cgs.fraglimit ) {
			s = va( "%2i", cgs.fraglimit );
			w = CG_DrawStrlen( s ) * TINYCHAR_WIDTH + 8;
			x -= w;

			CG_DrawTinyDigitalString( x + 4, y+4, s, 1.0F);

		}

	}

	return y1;

}
#endif // MISSIONPACK

/*
=================
CG_DrawLowerRight
=================
*/
#ifndef MISSIONPACK
static void CG_DrawLowerRight( void ) {
	float	y;

	y = 470;

	CG_SetScreenPlacement(PLACE_RIGHT, PLACE_BOTTOM);

	if ( isRaceObserver( cg.snap->ps.clientNum ) )
		return;

	y = CG_DrawLowerRightHUD( y );

	if ( cgs.gametype >= GT_TEAM && cg_drawTeamOverlay.integer == 2 ) {
		y = CG_DrawTeamOverlay( y, qtrue, qfalse );
	} 

}
#endif // MISSIONPACK

/*
=================
CG_DrawPickupItem
=================
*/
#ifndef MISSIONPACK
static int CG_DrawPickupItem( int y ) {
	int		value;
	float	*fadeColor;
    float	iconSize, charWidth, charHeight;

	if ( cg_drawPickups.value <= 0 ) {
		return y;
	}
	/* The telemetry HUD owns the centered, queued pickup toast. */
	if ( cg_drawStatus.integer &&
	     ( cgs.gametype != GT_DERBY || cg_hudShowDerbyVehicle.integer ) ) {
		return y;
	}

	if ( cg.snap->ps.stats[STAT_HEALTH] <= 0 ) {
		return y;
	}

	iconSize = ICON_SIZE*cg_drawPickups.value;
	charWidth = BIGCHAR_WIDTH*cg_drawPickups.value;
	charHeight = BIGCHAR_HEIGHT*cg_drawPickups.value;

	y -= iconSize;

	value = cg.itemPickup;
	if ( value ) {
		fadeColor = CG_FadeColor( cg.itemPickupTime, 3000 );
		if ( fadeColor ) {
			CG_RegisterItemVisuals( value );
			trap_R_SetColor( fadeColor );
			CG_DrawPic( 8, y, iconSize, iconSize, cg_items[ value ].icon );
			CG_DrawStringExt( iconSize + 16, y + (iconSize/2 - charHeight/2), bg_itemlist[ value ].pickup_name, fadeColor, qfalse, qtrue, charWidth, charHeight, 0 );
			trap_R_SetColor( NULL );
		}
	}
	
	return y;
}
#endif // MISSIONPACK

/*
================
CG_DrawLowerLeft
================
*/
#ifndef MISSIONPACK
static void CG_DrawLowerLeft( void ) {
	float	y;

	y = 480;
    
    CG_SetScreenPlacement(PLACE_LEFT, PLACE_BOTTOM);

	if (y > 404)
		y = 404;

	y = CG_DrawLowerLeftHUD( y );

	if ( cgs.gametype >= GT_TEAM && cg_drawTeamOverlay.integer == 3 ) {
		y = CG_DrawTeamOverlay( y, qfalse, qfalse );
	} 

	/* Derby: vehicle state panel sits at bottom-left (y=376);
	   move pickup overlay above it so they don't overlap. */
	if ( cgs.gametype == GT_DERBY && y > 376 ) {
		y = 376;
	}

	CG_DrawPickupItem( y );
}
#endif // MISSIONPACK


//===========================================================================================

/*
=================
CG_DrawTeamInfo
=================
*/
#ifndef MISSIONPACK
static void CG_DrawTeamInfo( void ) {
	int h;
	int i;
	vec4_t		hcolor;
	int		chatHeight;

#define CHATLOC_Y 420 // bottom end
#define CHATLOC_X 0

	if (cg_teamChatHeight.integer < TEAMCHAT_HEIGHT)
		chatHeight = cg_teamChatHeight.integer;
	else
		chatHeight = TEAMCHAT_HEIGHT;
	if (chatHeight <= 0)
		return; // disabled

    CG_SetScreenPlacement( PLACE_LEFT, PLACE_BOTTOM );

	if (cgs.teamLastChatPos != cgs.teamChatPos) {
		if (cg.time - cgs.teamChatMsgTimes[cgs.teamLastChatPos % chatHeight] > cg_teamChatTime.integer) {
			cgs.teamLastChatPos++;
		}

		h = (cgs.teamChatPos - cgs.teamLastChatPos) * TINYCHAR_HEIGHT;

		if ( cgs.clientinfo[cg.clientNum].team == TEAM_RED ) {
			hcolor[0] = 1.0f;
			hcolor[1] = 0.0f;
			hcolor[2] = 0.0f;
			hcolor[3] = 0.33f;
		} else if ( cgs.clientinfo[cg.clientNum].team == TEAM_BLUE ) {
			hcolor[0] = 0.0f;
			hcolor[1] = 0.0f;
			hcolor[2] = 1.0f;
			hcolor[3] = 0.33f;
		} else {
			hcolor[0] = 0.0f;
			hcolor[1] = 1.0f;
			hcolor[2] = 0.0f;
			hcolor[3] = 0.33f;
		}

		trap_R_SetColor( hcolor );
		CG_DrawPic( CHATLOC_X, CHATLOC_Y - h, 640, h, cgs.media.teamStatusBar );
		trap_R_SetColor( NULL );

		hcolor[0] = hcolor[1] = hcolor[2] = 1.0f;
		hcolor[3] = 1.0f;

		for (i = cgs.teamChatPos - 1; i >= cgs.teamLastChatPos; i--) {
			CG_DrawStringExt( CHATLOC_X + TINYCHAR_WIDTH, 
				CHATLOC_Y - (cgs.teamChatPos - i)*TINYCHAR_HEIGHT, 
				cgs.teamChatMsgs[i % chatHeight], hcolor, qfalse, qfalse,
				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );
		}
	}
}
#endif // MISSIONPACK

/*
===================
CG_DrawHoldableItem
===================
*/
#ifndef MISSIONPACK
static void CG_DrawHoldableItem( void ) { 
	int		value;
	screenPlacement_e savedHorizontalPlacement;
	screenPlacement_e savedVerticalPlacement;
	vec4_t panelColor = { 0.008f, 0.012f, 0.016f, 0.50f };
	vec4_t useColor = { 0.650f, 0.780f, 0.810f, 0.90f };

	value = cg.snap->ps.stats[STAT_HOLDABLE_ITEM];
	if ( !value ) {
		return;
	}
	CG_RegisterItemVisuals( value );

	/* Keep the legacy placement as a fallback when the telemetry strip is off. */
	if ( !cg_drawStatus.integer ||
	     ( cgs.gametype == GT_DERBY && !cg_hudShowDerbyVehicle.integer ) ) {
		CG_SetScreenPlacement( PLACE_RIGHT, PLACE_CENTER );
		CG_DrawPic( 640 - ICON_SIZE, ( SCREEN_HEIGHT - ICON_SIZE ) / 2,
		            ICON_SIZE, ICON_SIZE, cg_items[value].icon );
		return;
	}

	savedHorizontalPlacement = CG_GetScreenHorizontalPlacement();
	savedVerticalPlacement = CG_GetScreenVerticalPlacement();
	CG_SetScreenPlacement( PLACE_CENTER, PLACE_BOTTOM );
	CG_FillRect( 570, 394, 60, 18, panelColor );
	trap_R_SetColor( NULL );
	CG_DrawPic( 576, 397, 12, 12, cg_items[value].icon );
	CG_DrawIngameString( 625, 398, "USE",
	                     UI_RIGHT | UI_SMALLFONT, 0.46f, useColor );
	CG_SetScreenPlacement( savedHorizontalPlacement, savedVerticalPlacement );

}
#endif // MISSIONPACK

#ifdef MISSIONPACK
/*
========================
CG_DrawPersistantPowerup
========================
*/
#if 0 // sos001208 - DEAD
static void CG_DrawPersistantPowerup( void ) { 
	int		value;

    CG_SetScreenPlacement(PLACE_RIGHT, PLACE_CENTER);

	value = cg.snap->ps.stats[STAT_PERSISTANT_POWERUP];
	if ( value ) {
		CG_RegisterItemVisuals( value );
		CG_DrawPic( 640-ICON_SIZE, (SCREEN_HEIGHT-ICON_SIZE)/2 - ICON_SIZE, ICON_SIZE, ICON_SIZE, cg_items[ value ].icon );
	}
}
#endif
#endif // MISSIONPACK


#if 0 /* Legacy center-frame banners; all announcements use the HUD toast lane. */
static void CG_ShiftAchievementQueue( void ) {
    int i;

    if ( cg.achievementQueueCount <= 0 ) {
        cg.achievementQueueCount = 0;
        return;
    }

    for ( i = 1; i < cg.achievementQueueCount; ++i ) {
        cg.achievementQueue[i - 1] = cg.achievementQueue[i];
    }

    cg.achievementQueueCount--;
}

static void CG_ShiftRankQueue( void ) {
    int i;

    if ( cg.rankQueueCount <= 0 ) {
        cg.rankQueueCount = 0;
        return;
    }

    for ( i = 1; i < cg.rankQueueCount; ++i ) {
        cg.rankQueue[i - 1] = cg.rankQueue[i];
    }

    cg.rankQueueCount--;
}

static qboolean CG_IsAchievementNotificationActive( void ) {
    const cgAchievementAnnouncement_t *announcement;
    const bgAchievementCategoryDef_t *category;
    const bgAchievementTierDef_t *tier;
    int elapsed;

    if ( cg.achievementQueueCount <= 0 ) {
        return qfalse;
    }

    announcement = &cg.achievementQueue[0];
    category = BG_AchievementGetCategory( announcement->category );
    tier = BG_AchievementGetTier( announcement->category, announcement->tierIndex );

    if ( !category || !tier ) {
        return qfalse;
    }

    elapsed = cg.time - announcement->startTime;
    return ( elapsed < ACHIEVEMENT_DISPLAY_TIME );
}

static void CG_DrawAchievementNotifications( void ) {
    const bgAchievementCategoryDef_t *category;
    const bgAchievementTierDef_t *tier;
    cgAchievementAnnouncement_t *announcement;
    int elapsed;
    float alpha;
    float x, y;
    float width = 260.0f;
    float height = 60.0f;
    float iconSize = 36.0f;
    vec4_t bgColor = { 0.0f, 0.0f, 0.0f, 0.45f };
    vec4_t borderColor = { 1.0f, 1.0f, 1.0f, 0.25f };
    vec4_t lockedColor = { 0.7f, 0.7f, 0.7f, 1.0f };
    vec4_t unlockedColor = { 0.6f, 1.0f, 0.6f, 1.0f };
    vec4_t textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    qhandle_t icon;

    if ( cg.achievementQueueCount <= 0 ) {
        return;
    }

    announcement = &cg.achievementQueue[0];
    category = BG_AchievementGetCategory( announcement->category );
    tier = BG_AchievementGetTier( announcement->category, announcement->tierIndex );

    if ( !category || !tier ) {
        CG_ShiftAchievementQueue();
        return;
    }

    elapsed = cg.time - announcement->startTime;
    if ( elapsed >= ACHIEVEMENT_DISPLAY_TIME ) {
        CG_ShiftAchievementQueue();
        return;
    }

    alpha = 1.0f;
    if ( elapsed > ACHIEVEMENT_DISPLAY_TIME - ACHIEVEMENT_FADE_TIME ) {
        alpha = (float)( ACHIEVEMENT_DISPLAY_TIME - elapsed ) / ACHIEVEMENT_FADE_TIME;
    }

    if ( alpha < 0.0f ) {
        alpha = 0.0f;
    }

    bgColor[3] *= alpha;
    borderColor[3] *= alpha;
    lockedColor[3] *= alpha;
    unlockedColor[3] *= alpha;
    textColor[3] *= alpha;

    // Position the achievement notification inside the rearview mirror frame
    x = 170.0f + ( 300.0f - width ) * 0.5f;
    y = 10.0f + ( 75.0f - height ) * 0.5f;

    CG_FillRect( x, y, width, height, bgColor );
    CG_DrawRect( x, y, width, height, 1.0f, borderColor );

    icon = ( elapsed < ACHIEVEMENT_LOCKED_TIME ) ? cgs.media.achievementMedalLocked[category->icon] : cgs.media.achievementMedalUnlocked[category->icon];
    if ( icon ) {
        CG_DrawPic( x + 10.0f, y + ( height - iconSize ) * 0.5f, iconSize, iconSize, icon );
    }

    CG_DrawStringExt( (int)( x + iconSize + 22.0f ), (int)( y + 12.0f ), tier->name,
                      ( elapsed < ACHIEVEMENT_LOCKED_TIME ) ? lockedColor : unlockedColor,
                      qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );

    CG_DrawStringExt( (int)( x + iconSize + 22.0f ), (int)( y + 30.0f ), tier->description,
                      textColor, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );

    CG_DrawStringExt( (int)( x + iconSize + 22.0f ), (int)( y + 44.0f ), category->title,
                      textColor, qfalse, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );

    trap_R_SetColor( NULL );
}

static void CG_DrawRankNotifications( void ) {
    cgRankAnnouncement_t *announcement;
    int elapsed;
    float alpha;
    float x, y;
    float width = 260.0f;
    float height = 60.0f;
    vec4_t bgColor = { 0.0f, 0.0f, 0.0f, 0.45f };
    vec4_t borderColor = { 1.0f, 1.0f, 1.0f, 0.25f };
    vec4_t highlightColor = { 0.6f, 1.0f, 0.6f, 1.0f };
    vec4_t demotionColor = { 1.0f, 0.4f, 0.4f, 1.0f };
    vec4_t textColor = { 1.0f, 1.0f, 1.0f, 1.0f };

    if ( cg.rankQueueCount <= 0 ) {
        return;
    }

    announcement = &cg.rankQueue[0];

    if ( !announcement->name[0] ) {
        CG_ShiftRankQueue();
        return;
    }

    elapsed = cg.time - announcement->startTime;
    if ( elapsed >= RANK_DISPLAY_TIME ) {
        CG_ShiftRankQueue();
        return;
    }

    alpha = 1.0f;
    if ( elapsed > RANK_DISPLAY_TIME - RANK_FADE_TIME ) {
        alpha = (float)( RANK_DISPLAY_TIME - elapsed ) / RANK_FADE_TIME;
    }

    if ( alpha < 0.0f ) {
        alpha = 0.0f;
    }

    bgColor[3] *= alpha;
    borderColor[3] *= alpha;
    highlightColor[3] *= alpha;
    demotionColor[3] *= alpha;
    textColor[3] *= alpha;

    x = 170.0f + ( 300.0f - width ) * 0.5f;
    y = 10.0f + ( 75.0f - height ) * 0.5f;

    CG_FillRect( x, y, width, height, bgColor );
    CG_DrawRect( x, y, width, height, 1.0f, borderColor );

    CG_DrawStringExt( (int)( x + 12.0f ), (int)( y + 10.0f ), announcement->rankUp ? "Rank Up!" : "Rank Down!",
                      announcement->rankUp ? highlightColor : demotionColor,
                      qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );

    CG_DrawStringExt( (int)( x + 12.0f ), (int)( y + 26.0f ), announcement->name,
                      announcement->rankUp ? highlightColor : demotionColor,
                      qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );

    if ( announcement->nextName[0] && Q_stricmp( announcement->nextName, announcement->name ) ) {
        CG_DrawStringExt( (int)( x + 12.0f ), (int)( y + 42.0f ), va( "Next: %s", announcement->nextName ),
                          textColor, qfalse, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );
    }

    trap_R_SetColor( NULL );
}
#endif

/*
=========
LAGOMETER
=========
*/

#define	LAG_SAMPLES		128


typedef struct {
	int		frameSamples[LAG_SAMPLES];
	int		frameCount;
	int		snapshotFlags[LAG_SAMPLES];
	int		snapshotSamples[LAG_SAMPLES];
	int		snapshotCount;
} lagometer_t;

lagometer_t		lagometer;

/*
========================================================================================
CG_AddLagometerFrameInfo - Adds the current interpolate / extrapolate bar for this frame
========================================================================================
*/
void CG_AddLagometerFrameInfo( void ) {
	int			offset;

	offset = cg.time - cg.latestSnapshotTime;
	lagometer.frameSamples[ lagometer.frameCount & ( LAG_SAMPLES - 1) ] = offset;
	lagometer.frameCount++;
}

/*
=======================================================
CG_AddLagometerSnapshotInfo

Each time a snapshot is received, log its ping time and
the number of snapshots that were dropped before it.

Pass NULL for a dropped packet.
=======================================================
*/
void CG_AddLagometerSnapshotInfo( snapshot_t *snap ) {
	// dropped packet
	if ( !snap ) {
		lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = -1;
		lagometer.snapshotCount++;
		return;
	}

	// add this snapshot's info
	lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = snap->ping;
	lagometer.snapshotFlags[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = snap->snapFlags;
	lagometer.snapshotCount++;
}

/*
==================================================================================
CG_DrawDisconnect - Should we draw something differnet for long lag vs no packets?
==================================================================================
*/
static void CG_DrawDisconnect( void ) {
	float		x, y;
	int			cmdNum;
	usercmd_t	cmd;
	const char		*s;
	int			w;

	// draw the phone jack if we are completely past our buffers
	cmdNum = trap_GetCurrentCmdNumber() - CMD_BACKUP + 1;
	trap_GetUserCmd( cmdNum, &cmd );
	if ( cmd.serverTime <= cg.snap->ps.commandTime
		|| cmd.serverTime > cg.time ) {	// special check for map_restart
		return;
	}

    CG_SetScreenPlacement(PLACE_CENTER, PLACE_CENTER);

	// also add text in center of screen
	s = "Connection Interrupted";
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
	CG_DrawBigString( 320 - w/2, 100, s, 1.0F);

	// blink the icon
	if ( ( cg.time >> 9 ) & 1 ) {
		return;
	}

    CG_SetScreenPlacement(PLACE_RIGHT, PLACE_BOTTOM);

#ifdef MISSIONPACK
	x = 640 - 48;
	y = 480 - 144;
#else
	x = 640 - 48;
	y = 480 - 48;
#endif

	CG_DrawPic( x, y, 48, 48, trap_R_RegisterShader("gfx/2d/net.tga" ) );
}


#define	MAX_LAGOMETER_PING	900
#define	MAX_LAGOMETER_RANGE	300

/*
================
CG_DrawLagometer
================
*/
static void CG_DrawLagometer( void ) {
	int		a, x, y, i;
	float	v;
	float	ax, ay, aw, ah, mid, range;
	int		color;
	float	vscale;

	if ( !cg_lagometer.integer || cgs.localServer ) {
		CG_DrawDisconnect();
		return;
	}

    CG_SetScreenPlacement(PLACE_RIGHT, PLACE_BOTTOM);

	//
	// draw the graph
	//
#ifdef MISSIONPACK
	x = 640 - 48;
	y = 480 - 144;
#else
	x = 640 - 48;
	y = 480 - 48;
#endif

	trap_R_SetColor( NULL );
	CG_DrawPic( x, y, 48, 48, cgs.media.lagometerShader );

	ax = x;
	ay = y;
	aw = 48;
	ah = 48;
	CG_AdjustFrom640( &ax, &ay, &aw, &ah );

	color = -1;
	range = ah / 3;
	mid = ay + range;

	vscale = range / MAX_LAGOMETER_RANGE;

	// draw the frame interpoalte / extrapolate graph
	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.frameCount - 1 - a ) & (LAG_SAMPLES - 1);
		v = lagometer.frameSamples[i];
		v *= vscale;
		if ( v > 0 ) {
			if ( color != 1 ) {
				color = 1;
				trap_R_SetColor( g_color_table[ColorIndex(COLOR_YELLOW)] );
			}
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic ( ax + aw - a, mid - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 2 ) {
				color = 2;
				trap_R_SetColor( g_color_table[ColorIndex(COLOR_BLUE)] );
			}
			v = -v;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, mid, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	// draw the snapshot latency / drop graph
	range = ah / 2;
	vscale = range / MAX_LAGOMETER_PING;

	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.snapshotCount - 1 - a ) & (LAG_SAMPLES - 1);
		v = lagometer.snapshotSamples[i];
		if ( v > 0 ) {
			if ( lagometer.snapshotFlags[i] & SNAPFLAG_RATE_DELAYED ) {
				if ( color != 5 ) {
					color = 5;	// YELLOW for rate delay
					trap_R_SetColor( g_color_table[ColorIndex(COLOR_YELLOW)] );
				}
			} else {
				if ( color != 3 ) {
					color = 3;
					trap_R_SetColor( g_color_table[ColorIndex(COLOR_GREEN)] );
				}
			}
			v = v * vscale;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 4 ) {
				color = 4;		// RED for dropped snapshots
				trap_R_SetColor( g_color_table[ColorIndex(COLOR_RED)] );
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - range, 1, range, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	trap_R_SetColor( NULL );

	if ( cg_nopredict.integer || cg_synchronousClients.integer ) {
		CG_DrawBigString( x, y, "snc", 1.0 );
	}

	CG_DrawDisconnect();
}



/*
===============================================================================

CENTER PRINTING

===============================================================================
*/


/*
==============
CG_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
void CG_CenterPrint( const char *str, int y, int charWidth ) {
	char	*s;

	Q_strncpyz( cg.centerPrint, str, sizeof(cg.centerPrint) );

	cg.centerPrintTime = cg.time;
	cg.centerPrintY = y;
	cg.centerPrintCharWidth = charWidth;

	// count the number of lines for centering
	cg.centerPrintLines = 1;
	s = cg.centerPrint;
	while( *s ) {
		if (*s == '\n')
			cg.centerPrintLines++;
		s++;
	}
}


/*
===================
CG_DrawCenterString
===================
*/
static void CG_DrawCenterString( void ) {
	char	*start;
	int		l;
	int		x, y, w;
#ifdef MISSIONPACK
	int h;
#endif
	float	*color;

	if ( !cg.centerPrintTime ) {
		return;
	}

	color = CG_FadeColor( cg.centerPrintTime, 1000 * cg_centertime.value );
	if ( !color ) {
		return;
	}

    CG_SetScreenPlacement(PLACE_CENTER, PLACE_CENTER);

	trap_R_SetColor( color );

	start = cg.centerPrint;

	y = cg.centerPrintY - cg.centerPrintLines * BIGCHAR_HEIGHT / 2;

	while ( 1 ) {
		char linebuffer[1024];

		for ( l = 0; l < 50; l++ ) {
			if ( !start[l] || start[l] == '\n' ) {
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

#ifdef MISSIONPACK
		w = CG_Text_Width(linebuffer, 0.5, 0);
		h = CG_Text_Height(linebuffer, 0.5, 0);
		x = (SCREEN_WIDTH - w) / 2;
		CG_Text_Paint(x, y + h, 0.5, color, linebuffer, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
		y += h + 6;
#else
		w = cg.centerPrintCharWidth * CG_DrawStrlen( linebuffer );

		x = ( SCREEN_WIDTH - w ) / 2;

		CG_DrawStringExt( x, y, linebuffer, color, qfalse, qtrue,
			cg.centerPrintCharWidth, (int)(cg.centerPrintCharWidth * 1.5), 0 );

		y += cg.centerPrintCharWidth * 1.5;
#endif
		while ( *start && ( *start != '\n' ) ) {
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	trap_R_SetColor( NULL );
}



/*
================================================================================

CROSSHAIR

================================================================================
*/


/*
=================
CG_DrawCrosshair
=================

static void CG_DrawCrosshair(void)
{
	float		w, h;
	qhandle_t	hShader;
	float		f;
	float		x, y;
	int			ca;

	if ( !cg_drawCrosshair.integer ) {
		return;
	}

	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR) {
		return;
	}

	if ( cg.renderingThirdPerson ) {
		return;
	}

    CG_SetScreenPlacement(PLACE_CENTER, PLACE_CENTER);

	// set color based on health
	if ( cg_crosshairHealth.integer ) {
		vec4_t		hcolor;

		CG_ColorForHealth( hcolor );
		trap_R_SetColor( hcolor );
	} else {
		trap_R_SetColor( NULL );
	}

	w = h = cg_crosshairSize.value;

	// pulse the size of the crosshair when picking up items
	f = cg.time - cg.itemPickupBlendTime;
	if ( f > 0 && f < ITEM_BLOB_TIME ) {
		f /= ITEM_BLOB_TIME;
		w *= ( 1 + f );
		h *= ( 1 + f );
	}

	x = cg_crosshairX.integer;
	y = cg_crosshairY.integer;

	ca = cg_drawCrosshair.integer;
	if (ca < 0) {
		ca = 0;
	}
	hShader = cgs.media.crosshairShader[ ca % NUM_CROSSHAIRS ];

	CG_DrawPic( ((SCREEN_WIDTH-w)*0.5f)+x, ((SCREEN_HEIGHT-h)*0.5f)+y, w, h, hShader );

	trap_R_SetColor( NULL );
}
*/
/*
==================
CG_DrawCrosshair3D
==================

static void CG_DrawCrosshair3D(void)
{
	float		w;
	qhandle_t	hShader;
	float		f;
	int			ca;

	trace_t trace;
	vec3_t endpos;
	float stereoSep, zProj, maxdist, xmax;
	char rendererinfos[128];
	refEntity_t ent;

	if ( !cg_drawCrosshair.integer ) {
		return;
	}

	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR) {
		return;
	}

	if ( cg.renderingThirdPerson ) {
		return;
	}

	w = cg_crosshairSize.value;

	// pulse the size of the crosshair when picking up items
	f = cg.time - cg.itemPickupBlendTime;
	if ( f > 0 && f < ITEM_BLOB_TIME ) {
		f /= ITEM_BLOB_TIME;
		w *= ( 1 + f );
	}

	ca = cg_drawCrosshair.integer;
	if (ca < 0) {
		ca = 0;
	}
	hShader = cgs.media.crosshairShader[ ca % NUM_CROSSHAIRS ];

	// Use a different method rendering the crosshair so players don't see two of them when
	// focusing their eyes at distant objects with high stereo separation
	// We are going to trace to the next shootable object and place the crosshair in front of it.

	// first get all the important renderer information
	trap_Cvar_VariableStringBuffer("r_zProj", rendererinfos, sizeof(rendererinfos));
	zProj = atof(rendererinfos);
	trap_Cvar_VariableStringBuffer("r_stereoSeparation", rendererinfos, sizeof(rendererinfos));
	stereoSep = zProj / atof(rendererinfos);
	
	xmax = zProj * tan(cg.refdef.fov_x * M_PI / 360.0f);
	
	// let the trace run through until a change in stereo separation of the crosshair becomes less than one pixel.
	maxdist = cgs.glconfig.vidWidth * stereoSep * zProj / (2 * xmax);
	VectorMA(cg.refdef.vieworg, maxdist, cg.refdef.viewaxis[0], endpos);
	CG_Trace(&trace, cg.refdef.vieworg, NULL, NULL, endpos, 0, MASK_SHOT);
	
	memset(&ent, 0, sizeof(ent));
	ent.reType = RT_SPRITE;
	ent.renderfx = RF_DEPTHHACK | RF_CROSSHAIR;
	
	VectorCopy(trace.endpos, ent.origin);
	
	// scale the crosshair so it appears the same size for all distances
	ent.radius = w / 640 * xmax * trace.fraction * maxdist / zProj;
	ent.customShader = hShader;

	trap_R_AddRefEntityToScene(&ent);
}

*/

/*
=========================
CG_ScanForCrosshairEntity
=========================
*/
static void CG_ScanForCrosshairEntity( void ) {
	trace_t		trace;
	vec3_t		start, end;
	int			content;

	VectorCopy( cg.refdef.vieworg, start );
	VectorMA( start, 131072, cg.refdef.viewaxis[0], end );

	CG_Trace( &trace, start, vec3_origin, vec3_origin, end, 
		cg.snap->ps.clientNum, CONTENTS_SOLID|CONTENTS_BODY );
	if ( trace.entityNum >= MAX_CLIENTS ) {
		return;
	}

	// if the player is in fog, don't show it
	content = CG_PointContents( trace.endpos, 0 );
	if ( content & CONTENTS_FOG ) {
		return;
	}

	// if the player is invisible, don't show it
	if ( cg_entities[ trace.entityNum ].currentState.powerups & ( 1 << PW_INVIS ) ) {
		return;
	}

	// update the fade timer
	cg.crosshairClientNum = trace.entityNum;
	cg.crosshairClientTime = cg.time;
}


/*
=====================
CG_DrawCrosshairNames
=====================
*/
static void CG_DrawCrosshairNames( void ) {
	float		*color;
	char		*name;
	float		w;

	if ( !cg_drawCrosshair.integer ) {
		return;
	}

	// only draw crosshair in hood view
	if (cg_thirdPersonAngle.value != 0 || cg_thirdPersonRange.value != -20 || cg_thirdPersonHeight.value != 10) {
		return;
	}

	if ( !cg_drawCrosshairNames.integer ) {
		return;
	}
	if ( cg.renderingThirdPerson ) {
		return;
	}

    CG_SetScreenPlacement(PLACE_CENTER, PLACE_CENTER);

	// scan the known entities to see if the crosshair is sighted on one
	CG_ScanForCrosshairEntity();

	// draw the name of the player being looked at
	color = CG_FadeColor( cg.crosshairClientTime, 1000 );
	if ( !color ) {
		trap_R_SetColor( NULL );
		return;
	}

	name = cgs.clientinfo[ cg.crosshairClientNum ].name;
#ifdef MISSIONPACK
	color[3] *= 0.5f;
	w = CG_Text_Width(name, 0.3f, 0);
	CG_Text_Paint( 320 - w / 2, 190, 0.3f, color, name, 0, 0, ITEM_TEXTSTYLE_SHADOWED);
#else
	w = CG_DrawStrlen( name ) * BIGCHAR_WIDTH;
	CG_DrawBigString( 320 - w / 2, 170, name, color[3] * 0.5f );
#endif
	trap_R_SetColor( NULL );
}


//==============================================================================

/*
=================
CG_DrawSpectator
=================
*/
static void CG_DrawSpectator(void) {
    CG_SetScreenPlacement(PLACE_CENTER, PLACE_BOTTOM);
	CG_DrawBigString(320 - 9 * 8, 440, "SPECTATOR", 1.0F);
// Q3Rally Code Start - removed gametype
/*
	if ( cgs.gametype == GT_TOURNAMENT ) {
		CG_DrawBigString(320 - 15 * 8, 460, "waiting to play", 1.0F);
	}
	else
*/
// Q3Rally Code END
	if ( cgs.gametype >= GT_TEAM ) {
		CG_DrawBigString(320 - 39 * 8, 460, "press ESC and use the START menu to play", 1.0F);
	}
}

/*
===========
CG_DrawVote
===========
*/
static void CG_DrawVote(void) {
	char	*s;
	int		sec;

	if ( !cgs.voteTime ) {
		return;
	}

    CG_SetScreenPlacement(PLACE_LEFT, PLACE_TOP);

	// play a talk beep whenever it is modified
	if ( cgs.voteModified ) {
		cgs.voteModified = qfalse;
		trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
	}

	sec = ( VOTE_TIME - ( cg.time - cgs.voteTime ) ) / 1000;
	if ( sec < 0 ) {
		sec = 0;
	}
#ifdef MISSIONPACK
	s = va("VOTE(%i):%s yes:%i no:%i", sec, cgs.voteString, cgs.voteYes, cgs.voteNo);
	CG_DrawSmallString( 0, 58, s, 1.0F );
	s = "or press ESC then click Vote";
	CG_DrawSmallString( 0, 58 + SMALLCHAR_HEIGHT + 2, s, 1.0F );
#else
	s = va("VOTE(%i):%s yes:%i no:%i", sec, cgs.voteString, cgs.voteYes, cgs.voteNo );
	CG_DrawSmallString( 0, 58, s, 1.0F );
#endif
}

/*
===============
CG_DrawTeamVote
===============
*/
static void CG_DrawTeamVote(void) {
	char	*s;
	int		sec, cs_offset;

	if ( cgs.clientinfo[cg.clientNum].team == TEAM_RED )
		cs_offset = 0;
	else if ( cgs.clientinfo[cg.clientNum].team == TEAM_BLUE )
		cs_offset = 1;
	else
		return;

	if ( !cgs.teamVoteTime[cs_offset] ) {
		return;
	}

    CG_SetScreenPlacement(PLACE_LEFT, PLACE_TOP);

	// play a talk beep whenever it is modified
	if ( cgs.teamVoteModified[cs_offset] ) {
		cgs.teamVoteModified[cs_offset] = qfalse;
		trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
	}

	sec = ( VOTE_TIME - ( cg.time - cgs.teamVoteTime[cs_offset] ) ) / 1000;
	if ( sec < 0 ) {
		sec = 0;
	}
	s = va("TEAMVOTE(%i):%s yes:%i no:%i", sec, cgs.teamVoteString[cs_offset],
							cgs.teamVoteYes[cs_offset], cgs.teamVoteNo[cs_offset] );
	CG_DrawSmallString( 0, 90, s, 1.0F );
}


static qboolean CG_DrawScoreboard( void ) {
#ifdef MISSIONPACK
	static qboolean firstTime = qtrue;

    CG_SetScreenPlacement(PLACE_CENTER, PLACE_CENTER);

	if (menuScoreboard) {
		menuScoreboard->window.flags &= ~WINDOW_FORCED;
	}
	if (cg_paused.integer) {
		cg.deferredPlayerLoading = 0;
		firstTime = qtrue;
		return qfalse;
	}

	// should never happen in Team Arena
	if (cgs.gametype == GT_SINGLE_PLAYER && cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		cg.deferredPlayerLoading = 0;
		firstTime = qtrue;
		return qfalse;
	}

	// don't draw scoreboard during death while warmup up
	if ( cg.warmup && !cg.showScores ) {
		return qfalse;
	}

	if ( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD || cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
	} else {
		if ( !CG_FadeColor( cg.scoreFadeTime, FADE_TIME ) ) {
			// next time scoreboard comes up, don't print killer
			cg.deferredPlayerLoading = 0;
			cg.killerName[0] = 0;
			firstTime = qtrue;
			return qfalse;
		}
	}

	if (menuScoreboard == NULL) {
		if ( cgs.gametype >= GT_TEAM ) {
			menuScoreboard = Menus_FindByName("teamscore_menu");
		} else {
			menuScoreboard = Menus_FindByName("score_menu");
		}
	}

	if (menuScoreboard) {
		if (firstTime) {
			CG_SetScoreSelection(menuScoreboard);
			firstTime = qfalse;
		}
		Menu_Paint(menuScoreboard, qtrue);
	}

	// load any models that have been deferred
	if ( ++cg.deferredPlayerLoading > 10 ) {
		CG_LoadDeferredPlayers();
	}

	return qtrue;
#else
	return CG_DrawOldScoreboard();
#endif
}

/*
CG_DrawIntermission
===================
*/
static void CG_DrawIntermission( stereoFrame_t stereoFrame ) {
//	int key;
#ifdef MISSIONPACK
	//if (cg_singlePlayer.integer) {
	//	CG_DrawCenterString();
	//	return;
	//}
#else
	if ( cgs.gametype == GT_SINGLE_PLAYER ) {
		CG_DrawCenterString();
		return;
	}
#endif
	cg.scoreFadeTime = cg.time;

// Q3Rally Code Start
	cg.scoreBoardShowing = CG_DrawHUD();
	CG_DrawHUDOptionsMenu();

	if ( stereoFrame == STEREO_CENTER ) {
		CG_JukeboxFrame();
		CG_JukeboxDraw( 170.0f, 90.0f, 300.0f, 54.0f );
	}

	if (!cg.scoreBoardShowing)
// Q3Rally Code END
		cg.scoreBoardShowing = CG_DrawScoreboard();
}

/*
=============
CG_DrawFollow
=============
*/
static qboolean CG_DrawFollow( void ) {
	float		x;
	vec4_t		color;
	const char	*name;

	if ( !(cg.snap->ps.pm_flags & PMF_FOLLOW) ) {
		return qfalse;
	}

	/* Don't show "following <player>" during the intro camera sequence */
	if ( CG_IntroCam_IsActive() ) {
		return qfalse;
	}
    
    CG_SetScreenPlacement(PLACE_CENTER, PLACE_TOP);
    
	color[0] = 1;
	color[1] = 1;
	color[2] = 1;
	color[3] = 1;


	CG_DrawBigString( 320 - 9 * BIGCHAR_WIDTH/2, 24, "following", 1.0F );

	name = cgs.clientinfo[ cg.snap->ps.clientNum ].name;

	x = 0.5 * ( 640 - GIANT_WIDTH * CG_DrawStrlen( name ) );

	CG_DrawStringExt( x, 40, name, color, qtrue, qtrue, GIANT_WIDTH, GIANT_HEIGHT, 0 );

	return qtrue;
}



/*
==================
CG_DrawAmmoWarning
==================
*/
static void CG_DrawAmmoWarning( void ) {
	const char	*s;
	int			w;

// Q3Rally Code Start
	if (isRallyNonDMRace() || cgs.gametype == GT_DERBY){
		return;
	}
// Q3Rally Code END

	if ( cg_drawAmmoWarning.integer == 0 ) {
		return;
	}

	if ( !cg.lowAmmoWarning ) {
		return;
	}

    CG_SetScreenPlacement(PLACE_CENTER, PLACE_TOP);

	if ( cg.lowAmmoWarning == 2 ) {
		s = "OUT OF AMMO";
	} else {
		s = "LOW AMMO WARNING";
	}
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
	CG_DrawBigString(320 - w / 2, 98, s, 1.0F);
	}


#ifdef MISSIONPACK
/*
==================
CG_DrawProxWarning
==================
*/
static void CG_DrawProxWarning( void ) {
	char s [32];
	int			w;
  static int proxTime;
  int proxTick;

	if( !(cg.snap->ps.eFlags & EF_TICKING ) ) {
    proxTime = 0;
		return;
	}

    CG_SetScreenPlacement(PLACE_CENTER, PLACE_TOP);

  if (proxTime == 0) {
    proxTime = cg.time;
  }

  proxTick = 10 - ((cg.time - proxTime) / 1000);

  if (proxTick > 0 && proxTick <= 5) {
    Com_sprintf(s, sizeof(s), "INTERNAL COMBUSTION IN: %i", proxTick);
  } else {
    Com_sprintf(s, sizeof(s), "YOU HAVE BEEN MINED");
  }

	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
	CG_DrawBigStringColor( 320 - w / 2, 64 + BIGCHAR_HEIGHT, s, g_color_table[ColorIndex(COLOR_RED)] );
}
#endif

//========================
//CG_DrawSigilLocationInfo
//========================

void CG_DrawSigilLocationInfo( vec3_t origin, vec3_t target, qhandle_t shader, vec4_t color )    {

          int x = 320, y = 240;
          int w = 320, h = 240;
          float angle, distance;
          vec3_t temp, angles;
          VectorSubtract(origin, target, temp);
          distance=VectorLength(temp);
          VectorNormalize(temp);
          vectoangles(temp,angles);
          
          angles[YAW]=AngleSubtract(cg.refdefViewAngles[YAW],angles[YAW]);
          angle=(angles[YAW] + 180.0f)/360.0f;
          angle -=0.25;
          angle *= (2*M_PI);
          w=sqrt((w*w)+(h*h));
          x +=cos(angle)*w;
          y +=sin(angle)*w;
          
          if (x<15)
                  x=15;
          else {
        
          if (x>605)
                  x=605;
        }
        if (y<20)
                y=20;
        else 
          {
            if (y>440)
                    y=440;
        }
        CG_DrawPic( x, y, 20, 20, shader );
        CG_DrawStringExt( x-50, y+20, va("%10.2f",distance/100.0), color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );
    }

//=====================
//CG_DrawSigilLocations
//=====================

static void CG_DrawSigilLocations( void ) {
          snapshot_t *snap;
          int i;
          vec3_t origin, end;
          int redSigil, blueSigil, greenSigil, yellowSigil, whiteSigil;

          if ( cgs.gametype != GT_DOMINATION)
            return;
          if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
            return; 
      
          if ( cg.nextSnap && (!cg.nextFrameTeleport && !cg.thisFrameTeleport))
            
            snap = cg.nextSnap;
          else
            snap = cg.snap;
      
      VectorCopy(cg.snap->ps.origin,origin);
      redSigil = ITEM_INDEX( BG_FindItemForPowerup( PW_SIGILRED ) );
      blueSigil = ITEM_INDEX( BG_FindItemForPowerup( PW_SIGILBLUE ) );
      greenSigil = ITEM_INDEX( BG_FindItemForPowerup( PW_SIGILGREEN ) );
      yellowSigil = ITEM_INDEX( BG_FindItemForPowerup( PW_SIGILYELLOW ) );
      whiteSigil = ITEM_INDEX( BG_FindItemForPowerup( PW_SIGILWHITE ) );
      
      for ( i = 0; i < snap->numEntities; i++ )
    {
    
    centity_t *target = &cg_entities[snap->entities[i].number];
    if (target->currentState.eType != ET_ITEM)
    continue;

    if ( target->currentState.modelindex != redSigil && target->currentState.modelindex != blueSigil && target->currentState.modelindex != greenSigil && target->currentState.modelindex != yellowSigil && target->currentState.modelindex != whiteSigil )
    continue;
    
    VectorCopy(target->lerpOrigin,end);
    
    if (target->currentState.modelindex == redSigil)
        CG_DrawSigilLocationInfo(origin, end, cgs.media.redsigilShader, colorRed);
    
    else if (target->currentState.modelindex == blueSigil)
        CG_DrawSigilLocationInfo(origin, end, cgs.media.bluesigilShader, colorBlue);
    
    else if (target->currentState.modelindex == greenSigil)
        CG_DrawSigilLocationInfo(origin, end, cgs.media.greensigilShader, colorGreen);
        
    else if (target->currentState.modelindex == yellowSigil)
        CG_DrawSigilLocationInfo(origin, end, cgs.media.yellowsigilShader, colorYellow);
        
    else if (target->currentState.modelindex == whiteSigil)
        CG_DrawSigilLocationInfo(origin, end, cgs.media.sigilShader, colorWhite);
    }
  }
//==================================================================================
#ifdef MISSIONPACK
/* 
=================
CG_DrawTimedMenus
=================
*/
void CG_DrawTimedMenus( void ) {
	if (cg.voiceTime) {
		int t = cg.time - cg.voiceTime;
		if ( t > 2500 ) {
			Menus_CloseByName("voiceMenu");
			trap_Cvar_Set("cl_conXOffset", "0");
			cg.voiceTime = 0;
		}
	}
}
#endif
/*
=========
CG_Draw2D
=========
*/
static void CG_Draw2D(stereoFrame_t stereoFrame)
{
#ifdef MISSIONPACK
	if (cgs.orderPending && cg.time > cgs.orderTime) {
		CG_CheckOrderPending();
	}
#endif
	// if we are taking a levelshot for the menu, don't draw anything
	if ( cg.levelShot ) {
		return;
	}

	if ( cg_draw2D.integer == 0 ) {
		return;
	}

	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		CG_DrawIntermission( stereoFrame );
		return;
	}

	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR 
		|| isRaceObserver( cg.snap->ps.clientNum ) ) {

		CG_DrawSpectator();

		if(stereoFrame == STEREO_CENTER)

		CG_DrawCrosshairNames();
	} else {
		// don't draw any status if dead or the scoreboard is being explicitly shown

		if ( !cg.showScores && cg.snap->ps.stats[STAT_HEALTH] > 0 ) {

#ifdef MISSIONPACK
			if ( cg_drawStatus.integer ) {
            
            CG_SetScreenPlacement(PLACE_CENTER, PLACE_BOTTOM);
            
				Menu_PaintAll();
				CG_DrawTimedMenus();
			}
#else

			CG_DrawRallyStatusBar();

#endif
      
			CG_DrawAmmoWarning();

#ifdef MISSIONPACK
			CG_DrawProxWarning();
#endif      
			if(stereoFrame == STEREO_CENTER)
			CG_DrawCrosshairNames();
			CG_DrawWeaponSelect();

#ifndef MISSIONPACK
                        CG_DrawHoldableItem();
#else

#endif
                        /* Rewards, achievements and rank changes draw from the
                         * same serialized HUD toast lane as pickups. */
                }
        }

	CG_DrawKOTH_RespawnWave_Internal();
	CG_DrawKOTH_LossFlash();

	if ( cgs.gametype >= GT_TEAM ) {
#ifndef MISSIONPACK
		CG_DrawTeamInfo();
#endif
	}

	CG_DrawVote();
	CG_DrawTeamVote();

	CG_DrawLagometer();

	if (cg_sigilLocator.integer == 1)
		CG_DrawSigilLocations();

	if ( stereoFrame == STEREO_CENTER ) {
		CG_JukeboxFrame();
		CG_JukeboxDraw( 170.0f, 90.0f, 300.0f, 54.0f );
	}

#ifdef MISSIONPACK
	if (!cg_paused.integer) {
		CG_DrawUpperRight(stereoFrame);
	}
#else
	CG_DrawUpperRight(stereoFrame);
#endif

#ifndef MISSIONPACK
	CG_DrawLowerRight();
	CG_DrawLowerLeft();
#endif

	CG_DrawFollow();

	cg.scoreBoardShowing = CG_DrawHUD();
	CG_DrawHUDOptionsMenu();

	// don't draw center string if scoreboard is up

	if (!cg.scoreBoardShowing)

		cg.scoreBoardShowing = CG_DrawScoreboard();

	if ( !cg.scoreBoardShowing) {
		CG_DrawCenterString();

		CG_DrawRaceCountDown();

	}

}


/*
========================================================================
CG_DrawActive - Perform all drawing needed to completely fill the screen
========================================================================
*/
void CG_DrawActive( stereoFrame_t stereoView ) {
	// optionally draw the info screen instead
	if ( !cg.snap ) {
		CG_DrawInformation();
		return;
	}

	// clear around the rendered view if sized down
	CG_TileClear();

	if(stereoView != STEREO_CENTER)
//		CG_DrawCrosshair3D();
	// apply earthquake effect
	CG_Earthquake();

	// draw 3D view
	trap_R_RenderScene( &cg.refdef );

// Q3Rally Code Start
	if ( !cg.scoreBoardShowing )
		CG_DrawRearviewMirror( 170, 10, 300, 75);
	
	CG_DrawMMap( 0, 0, 160, 120); //TBB draw minimap function
		
// Q3Rally Code END

	// draw status bar and other floating elements
 	CG_Draw2D(stereoView);
}
