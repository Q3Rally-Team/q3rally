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

#include "q_shared.h"

#define RALLY_PLATE_POOLSIZE	(512 * 1024)

static char	rallyPlateMemoryPool[RALLY_PLATE_POOLSIZE];
static int	rallyPlateAllocPoint = 0;

extern int propMap[128][3];

int trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void trap_FS_Read( void *buffer, int len, fileHandle_t f );
void trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void trap_FS_FCloseFile( fileHandle_t f );

static void *RallyPlate_Alloc( int size ) {
	char	*p;

	if ( rallyPlateAllocPoint + size > RALLY_PLATE_POOLSIZE ) {
		Com_Printf( S_COLOR_YELLOW "Q3R Warning: RallyPlate_Alloc failed on allocation of %i bytes\n", size );
		return NULL;
	}

	p = &rallyPlateMemoryPool[rallyPlateAllocPoint];

	rallyPlateAllocPoint += ( size + 31 ) & ~31;

	return p;
}

typedef	struct {
	byte	*imageData;
	int		bpp;
	int		width;
	int		height;
} rallyPlateTexture_t;

/*
This function is based on code in the Tokens, Extensions, Scissor Testing And TGA Loading
tutorial by Jeff Molofee at NeHe.

NeHe URL: http://nehe.gamedev.net
Tutorial URL: http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=24
*/
static qboolean RallyPlate_LoadTGA( rallyPlateTexture_t *texture, const char *filename ) {
	byte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
	byte		TGAcompare[12];
	byte		header[6];
	int			bytesPerPixel;
	int			imageSize;
	fileHandle_t	imageFile;

	trap_FS_FOpenFile( filename, &imageFile, FS_READ );

	if ( !imageFile ) {
		Com_Printf( S_COLOR_YELLOW "Q3R Warning: Could not open %s for license plate.\n", filename );
		return qfalse;
	}

	trap_FS_Read( TGAcompare, sizeof( TGAcompare ), imageFile );

	if ( memcmp( TGAheader, TGAcompare, sizeof( TGAheader ) ) != 0 ) {
		trap_FS_FCloseFile( imageFile );
		if ( TGAcompare[2] == 10 ) {
			Com_Printf( S_COLOR_YELLOW "Q3R Warning: Cannot load %s, Run-Length Encoded TGAs are unsupported.\n", filename );
		} else {
			Com_Printf( S_COLOR_YELLOW "Q3R Warning: Header of %s does not match known header format.\n", filename );
		}
		return qfalse;
	}

	trap_FS_Read( header, sizeof( header ), imageFile );

	texture->width = header[1] * 256 + header[0];
	texture->height = header[3] * 256 + header[2];

	if ( texture->width <= 0 || texture->height <= 0 || ( header[4] != 24 && header[4] != 32 ) ) {
		trap_FS_FCloseFile( imageFile );
		Com_Printf( S_COLOR_YELLOW "Q3R Warning: %s has invalid dimensions or bpps.\n", filename );
		return qfalse;
	}

	texture->bpp = header[4];
	bytesPerPixel = texture->bpp / 8;
	imageSize = texture->width * texture->height * bytesPerPixel;

	texture->imageData = (byte *)RallyPlate_Alloc( imageSize );

	if ( texture->imageData == NULL ) {
		trap_FS_FCloseFile( imageFile );
		Com_Printf( S_COLOR_YELLOW "Q3R Warning: Not enough memory to load %s.\n", filename );
		return qfalse;
	}

	trap_FS_Read( texture->imageData, imageSize, imageFile );
	trap_FS_FCloseFile( imageFile );

	return qtrue;
}

static qboolean RallyPlate_WriteNameOnTexture( rallyPlateTexture_t *texture, const char *name, int maxChars ) {
	vec4_t			color;
	const char		*s;
	unsigned char	ch;
	float			ax, ay, aw, ah;
	float			frow, fcol, fwidth, fheight;
	int				i, j;
	int				bytesPerPixelF, bytesPerPixelT;
	float			a;
	int				t, f, len, cnt;
	rallyPlateTexture_t	font;

	if ( !RallyPlate_LoadTGA( &font, "gfx/2d/bigchars_plates.tga" ) )
		return qfalse;

	bytesPerPixelF = font.bpp / 8;
	bytesPerPixelT = texture->bpp / 8;

	len = (int)( texture->width / SMALLCHAR_WIDTH ) - 1;
	if ( len < maxChars ) {
		maxChars = len;
	}

	len = strlen( name );
	if ( len > maxChars ) {
		len = maxChars;
	}

	ax = (int)( ( texture->width / 2.0f ) - ( len * ( SMALLCHAR_WIDTH + 1 ) / 2.0f ) - 3 );
	if ( ax < -3 ) {
		ax = -3;
	}
	ay = 11;
	ah = SMALLCHAR_HEIGHT;

	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	color[3] = 1.0f;

	s = name;
	cnt = 0;
	while ( *s && cnt < maxChars ) {
		if ( Q_IsColorString( s ) ) {
			memcpy( color, g_color_table[ColorIndex( *( s + 1 ) )], sizeof( color ) );
			s += 2;
			continue;
		}

		ch = *s & 127;

		if ( ch == ' ' ) {
			aw = SMALLCHAR_WIDTH + 8;
		} else if ( propMap[ch][2] != -1 ) {
			frow = ( ch >> 4 ) * 16.0f;
			fcol = ( ch & 15 ) * 16.0f;
			fwidth = 16.0f;
			fheight = 16.0f;
			aw = SMALLCHAR_WIDTH + 8;

			for ( i = 0; i < ah; i++ ) {
				t = (int)( ( texture->height - ( ay + ah - i ) ) * texture->width + ax ) * bytesPerPixelT;
				f = (int)( font.height - ( frow + fheight - fheight * ( i / (float)ah ) ) ) * font.width;

				for ( j = 0; j < aw; j++ ) {
					a = font.imageData[(int)( ( f + (int)( fcol + fwidth * ( j / (float)aw ) ) ) * bytesPerPixelF + 3 )];

					texture->imageData[t] = (byte)( texture->imageData[t] * ( 1.0f - ( a / 255.0f ) ) + color[2] * a );
					texture->imageData[t+1] = (byte)( texture->imageData[t+1] * ( 1.0f - ( a / 255.0f ) ) + color[1] * a );
					texture->imageData[t+2] = (byte)( texture->imageData[t+2] * ( 1.0f - ( a / 255.0f ) ) + color[0] * a );

					t += bytesPerPixelT;
				}
			}
		} else {
			aw = 0;
		}

		ax += aw - 7;
		cnt++;
		s++;
	}

	return qtrue;
}

static qboolean RallyPlate_SaveTGA( rallyPlateTexture_t *texture, const char *filename ) {
	byte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
	byte		header[6];
	int			bytesPerPixel;
	int			imageSize;
	fileHandle_t	imageFile;

	trap_FS_FOpenFile( filename, &imageFile, FS_WRITE );

	if ( !imageFile ) {
		Com_Printf( S_COLOR_YELLOW "Q3R Warning: Could not open %s for texture output.\n", filename );
		return qfalse;
	}

	bytesPerPixel = texture->bpp / 8;
	imageSize = texture->width * texture->height * bytesPerPixel;

	header[0] = texture->width % 256;
	header[1] = texture->width / 256;
	header[2] = texture->height % 256;
	header[3] = texture->height / 256;
	header[4] = texture->bpp;
	header[5] = 0;

	trap_FS_Write( TGAheader, sizeof( TGAheader ), imageFile );
	trap_FS_Write( header, sizeof( header ), imageFile );
	trap_FS_Write( texture->imageData, imageSize, imageFile );

	trap_FS_FCloseFile( imageFile );

	return qtrue;
}

void CreateLicensePlateImage( const char *input, const char *output, const char *name, int maxChars ) {
	rallyPlateTexture_t	tga;

	rallyPlateAllocPoint = 0;

	if ( !RallyPlate_LoadTGA( &tga, input ) ) {
		return;
	}
	if ( !RallyPlate_WriteNameOnTexture( &tga, name, maxChars ) ) {
		return;
	}
	RallyPlate_SaveTGA( &tga, output );
}
