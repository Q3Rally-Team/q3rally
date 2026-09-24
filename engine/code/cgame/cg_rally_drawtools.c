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

#include "cg_local.h"


/*
===============
CG_DrawDigitalChar

Coordinates and size in 640*480 virtual screen size
===============
*/
void CG_DrawDigitalChar( int x, int y, int width, int height, int ch ) {
	int row, col;
	float frow, fcol;
	float size;
	float	ax, ay, aw, ah;

	ch &= 255;

	if ( ch == ' ' ) {
		return;
	}

	ax = x;
	ay = y;
	aw = width;
	ah = height;
	CG_AdjustFrom640( &ax, &ay, &aw, &ah );

	row = ch>>4;
	col = ch&15;

	frow = 0.5 + row*0.0625;
	fcol = col*0.0625;
	size = 0.0625;

	trap_R_DrawStretchPic( ax, ay, aw, ah,
					   fcol, frow, 
					   fcol + size, frow + size, 
					   cgs.media.charsetShader );
}


/*
==================
CG_DrawDigitalStringExt

Draws a multi-colored string with a drop shadow, optionally forcing
to a fixed color.

Coordinates are at 640 by 480 virtual resolution
==================
*/
void CG_DrawDigitalStringExt( int x, int y, const char *string, const float *setColor, 
		qboolean forceColor, qboolean shadow, int charWidth, int charHeight, int maxChars ) {
	vec4_t		color;
	const char	*s;
	int			xx;
	int			cnt;

	if (maxChars <= 0)
		maxChars = 32767; // do them all!

// Q3Rally Code Start - move font closer together
	x -= 1;
// END

	// draw the drop shadow
	if (shadow) {
		color[0] = color[1] = color[2] = 0;
		color[3] = setColor[3];
		trap_R_SetColor( color );
		s = string;
		xx = x;
		cnt = 0;
		while ( *s && cnt < maxChars) {
			if ( Q_IsColorString( s ) ) {
				s += 2;
				continue;
			}
			CG_DrawDigitalChar( xx + 2, y + 2, charWidth, charHeight, *s );
			cnt++;
			xx += charWidth - 2;
			s++;
		}
	}

	// draw the colored text
	s = string;
	xx = x;
	cnt = 0;
	trap_R_SetColor( setColor );
	while ( *s && cnt < maxChars) {
		if ( Q_IsColorString( s ) ) {
			if ( !forceColor ) {
				memcpy( color, g_color_table[ColorIndex(*(s+1))], sizeof( color ) );
				color[3] = setColor[3];
				trap_R_SetColor( color );
			}
			s += 2;
			continue;
		}

		CG_DrawDigitalChar( xx, y, charWidth, charHeight, *s );
		xx += charWidth - 2;
		cnt++;
		s++;
	}
	trap_R_SetColor( NULL );
}


void CG_DrawGiantDigitalString( int x, int y, const char *s, float alpha ) {
	float	color[4];

	color[0] = color[1] = color[2] = 1.0;
	color[3] = alpha;
	CG_DrawDigitalStringExt( x, y, s, color, qfalse, qfalse, GIANTCHAR_WIDTH, GIANTCHAR_HEIGHT, 0 );
}

void CG_DrawGiantDigitalStringColor( int x, int y, const char *s, vec4_t color ) {
	CG_DrawDigitalStringExt( x, y, s, color, qtrue, qfalse, GIANTCHAR_WIDTH, GIANTCHAR_HEIGHT, 0 );
}

void CG_DrawSmallDigitalString( int x, int y, const char *s, float alpha ) {
	float	color[4];

	color[0] = color[1] = color[2] = 1.0;
	color[3] = alpha;
	CG_DrawDigitalStringExt( x, y, s, color, qfalse, qfalse, SMALLCHAR_WIDTH+2, SMALLCHAR_HEIGHT, 0 );
}

void CG_DrawSmallDigitalStringColor( int x, int y, const char *s, vec4_t color ) {
	CG_DrawDigitalStringExt( x, y, s, color, qtrue, qfalse, SMALLCHAR_WIDTH+2, SMALLCHAR_HEIGHT, 0 );
}

void CG_DrawTinyDigitalString( int x, int y, const char *s, float alpha ) {
	float	color[4];

	color[0] = color[1] = color[2] = 1.0;
	color[3] = alpha;
	CG_DrawDigitalStringExt( x, y, s, color, qfalse, qfalse, TINYCHAR_WIDTH+2, TINYCHAR_HEIGHT, 0 );
}

void CG_DrawTinyDigitalStringColor( int x, int y, const char *s, vec4_t color ) {
	CG_DrawDigitalStringExt( x, y, s, color, qtrue, qfalse, TINYCHAR_WIDTH+2, TINYCHAR_HEIGHT, 0 );
}

void CG_DrawTinyString( int x, int y, const char *s, float alpha ) {
	float	color[4];

	color[0] = color[1] = color[2] = 1.0;
	color[3] = alpha;
	CG_DrawStringExt( x, y, s, color, qfalse, qfalse, TINYCHAR_WIDTH+2, TINYCHAR_HEIGHT, 0 );
}

void CG_DrawTinyStringColor( int x, int y, const char *s, vec4_t color ) {
	CG_DrawStringExt( x, y, s, color, qtrue, qfalse, TINYCHAR_WIDTH+2, TINYCHAR_HEIGHT, 0 );
}


/*
==============================
CG_FrontendStringWidth

Measure text using the same compact atlas and tracking as the modern
frontend.  Keeping this in CGame lets the in-game HUD share the menu's
visual language without pulling UI code into the renderer.
==============================
*/
int CG_FrontendStringWidth( const char *text, int style, float scale ) {
	const char *s;
	int advance;
	int width;

	if ( !text ) {
		return 0;
	}

	advance = ( style & UI_SMALLFONT ) ? 7 : 12;
	width = 0;
	for ( s = text; *s; s++ ) {
		if ( Q_IsColorString( s ) ) {
			s++;
			continue;
		}

		if ( *s == ' ' ) {
			width += ( advance + 1 ) / 2;
		} else if ( *s == 'I' || *s == 'i' || *s == 'l' || *s == 'L' ||
		           *s == '!' || *s == '|' || *s == '.' || *s == ',' ||
		           *s == ':' || *s == ';' ) {
			width += advance - 2;
		} else {
			width += advance;
		}
	}

	return (int)( width * scale + 0.5f );
}


/*
==============================
CG_DrawFrontendString

Draw the compact Q3Rally frontend atlas in virtual 640x480 coordinates.
UI_LEFT/UI_CENTER/UI_RIGHT are honoured so HUD labels can use the same
alignment rules as the menu.  The screen placement selected by the caller
is respected through CG_AdjustFrom640, which keeps the HUD correct on
wide-screen displays as well.
==============================
*/
void CG_DrawFrontendString( int x, int y, const char *text, int style,
	                        float scale, const float *color ) {
	const char *s;
	int cursorX;
	int textWidth;
	int advance;
	int charHeight;
	int quadWidth;
	vec4_t shadowColor;

	if ( !text || !text[0] || !color || !cgs.media.frontendCharset ) {
		return;
	}

	advance = ( style & UI_SMALLFONT ) ? 7 : 12;
	charHeight = ( style & UI_SMALLFONT ) ? 14 : 20;
	textWidth = CG_FrontendStringWidth( text, style, scale );
	cursorX = x;

	if ( ( style & UI_FORMATMASK ) == UI_CENTER ) {
		cursorX -= textWidth / 2;
	} else if ( ( style & UI_FORMATMASK ) == UI_RIGHT ) {
		cursorX -= textWidth;
	}

	if ( style & UI_DROPSHADOW ) {
		shadowColor[0] = 0.0f;
		shadowColor[1] = 0.0f;
		shadowColor[2] = 0.0f;
		shadowColor[3] = color[3];
		CG_DrawFrontendString( cursorX + 2, y + 2, text,
		                       UI_LEFT, scale, shadowColor );
	}

	trap_R_SetColor( color );
	for ( s = text; *s; s++ ) {
		int ch;
		float drawX, drawY, drawW, drawH;
		float frow, fcol;

		if ( Q_IsColorString( s ) ) {
			s++;
			continue;
		}

		ch = *s & 255;
		if ( ch == ' ' ) {
			cursorX += (int)( ( ( advance + 1 ) / 2 ) * scale + 0.5f );
			continue;
		}

		if ( ch == 'I' || ch == 'i' || ch == 'l' || ch == 'L' ||
		     ch == '!' || ch == '|' || ch == '.' || ch == ',' ||
		     ch == ':' || ch == ';' ) {
			quadWidth = ( ch == 'l' || ch == 'L' ) ? advance - 1 : advance - 2;
		} else {
			quadWidth = advance;
		}

		drawX = (float)cursorX;
		drawY = (float)y;
		drawW = (float)quadWidth * scale;
		drawH = (float)charHeight * scale;
		CG_AdjustFrom640( &drawX, &drawY, &drawW, &drawH );

		frow = (float)( ch >> 4 ) * 0.0625f;
		fcol = (float)( ch & 15 ) * 0.0625f;
		trap_R_DrawStretchPic( drawX, drawY, drawW, drawH,
		                       fcol, frow, fcol + 0.0625f,
		                       frow + 0.0625f, cgs.media.frontendCharset );
		cursorX += (int)( advance * scale + 0.5f );
	}
	trap_R_SetColor( NULL );
}


/*
==============================
CG_IngameStringWidth / CG_DrawIngameString

The driving HUD intentionally uses a separate bitmap face.  It is a little
more technical and less letter-spaced than the frontend face, which keeps
large telemetry values readable while the menu can retain its own identity.
==============================
*/
int CG_IngameStringWidth( const char *text, int style, float scale ) {
	const char *s;
	int advance;
	int width;

	if ( !text ) {
		return 0;
	}

	/* Keep destination glyphs square like the 16x16 atlas cells, but leave
	 * a little breathing room around the source pixels. */
	advance = ( style & UI_SMALLFONT ) ? 14 : 24;
	width = 0;
	for ( s = text; *s; s++ ) {
		if ( Q_IsColorString( s ) ) {
			s++;
			continue;
		}

		if ( *s == ' ' ) {
			width += advance / 2;
		} else {
			width += advance;
		}
	}

	return (int)( width * scale + 0.5f );
}

void CG_DrawIngameString( int x, int y, const char *text, int style,
	                       float scale, const float *color ) {
	const char *s;
	int cursorX;
	int textWidth;
	int advance;
	int charHeight;
	vec4_t shadowColor;

	if ( !text || !text[0] || !color || !cgs.media.ingameCharset ) {
		return;
	}

	advance = ( style & UI_SMALLFONT ) ? 14 : 24;
	charHeight = advance;
	textWidth = CG_IngameStringWidth( text, style, scale );
	cursorX = x;

	if ( ( style & UI_FORMATMASK ) == UI_CENTER ) {
		cursorX -= textWidth / 2;
	} else if ( ( style & UI_FORMATMASK ) == UI_RIGHT ) {
		cursorX -= textWidth;
	}

	if ( style & UI_DROPSHADOW ) {
		shadowColor[0] = 0.0f;
		shadowColor[1] = 0.0f;
		shadowColor[2] = 0.0f;
		shadowColor[3] = color[3];
		CG_DrawIngameString( cursorX + 2, y + 2, text,
		                     UI_LEFT | ( style & UI_SMALLFONT ),
		                     scale, shadowColor );
	}

	trap_R_SetColor( color );
	for ( s = text; *s; s++ ) {
		int ch;
		float drawX, drawY, drawW, drawH;
		float frow, fcol;

		if ( Q_IsColorString( s ) ) {
			s++;
			continue;
		}

		ch = *s & 255;
		if ( ch == ' ' ) {
			cursorX += (int)( ( advance / 2 ) * scale + 0.5f );
			continue;
		}

		drawX = (float)cursorX;
		drawY = (float)y;
		drawW = (float)advance * scale;
		drawH = (float)charHeight * scale;
		CG_AdjustFrom640( &drawX, &drawY, &drawW, &drawH );

		frow = (float)( ch >> 4 ) * 0.0625f;
		fcol = (float)( ch & 15 ) * 0.0625f;
		trap_R_DrawStretchPic( drawX, drawY, drawW, drawH,
		                       fcol, frow, fcol + 0.0625f,
		                       frow + 0.0625f, cgs.media.ingameCharset );
		cursorX += (int)( advance * scale + 0.5f );
	}
	trap_R_SetColor( NULL );
}

void CG_DrawIngameSmallString( int x, int y, const char *text,
	                            const float *color ) {
	CG_DrawIngameString( x, y, text, UI_SMALLFONT, 0.75f, color );
}
