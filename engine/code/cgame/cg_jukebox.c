/*
===========================================================================
Copyright (C) 2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

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

#define CG_JUKEBOX_MAX_TRACKS          64
#define CG_JUKEBOX_FILELIST_SIZE       8192
#define CG_JUKEBOX_DISPLAY_TIME        15000
#define CG_JUKEBOX_DIRECTORY           "music/jukebox"
#define CG_JUKEBOX_PROGRESS_MARGIN     8.0f
#define CG_JUKEBOX_PROGRESS_HEIGHT     6.0f
#define CG_JUKEBOX_TEXT_MARGIN         8.0f
#define CG_JUKEBOX_FOOTER_MARGIN       3.0f
#define CG_JUKEBOX_FOOTER_GAP          12.0f
#define CG_JUKEBOX_META_FIELD_LEN      64

typedef struct {
    qboolean    scanned;
    qboolean    active;
    int         trackCount;
    int         currentTrack;
    int         trackDurations[CG_JUKEBOX_MAX_TRACKS];
    char        trackPaths[CG_JUKEBOX_MAX_TRACKS][MAX_QPATH];
    char        trackNames[CG_JUKEBOX_MAX_TRACKS][MAX_QPATH];
    char        trackTitles[CG_JUKEBOX_MAX_TRACKS][CG_JUKEBOX_META_FIELD_LEN];
    char        trackArtists[CG_JUKEBOX_MAX_TRACKS][CG_JUKEBOX_META_FIELD_LEN];
    int         trackStartTime;
    int         displayExpireTime;
    char        statusLine[128];
    char        subtitleLine[64];
} cgJukeboxState_t;

static cgJukeboxState_t cg_jukebox;

typedef enum {
    JUKEBOX_REPEAT_OFF = 0,
    JUKEBOX_REPEAT_ALL,
    JUKEBOX_REPEAT_ONE
} cgJukeboxRepeatMode_t;

static void CG_JukeboxFormatName( const char *filename, char *out, int outSize ) {
    char *ext;

    Q_strncpyz( out, filename, outSize );
    ext = strrchr( out, '.' );
    if ( ext ) {
        *ext = '\0';
    }

    while ( *out ) {
        if ( *out == '_' ) {
            *out = ' ';
        }
        out++;
    }
}


static void CG_JukeboxBuildDisplayName( const char *fallbackName, const char *artist, const char *title, char *out, int outSize ) {
    if ( artist && *artist && title && *title ) {
        Com_sprintf( out, outSize, "%s - %s", artist, title );
        return;
    }

    if ( title && *title ) {
        Q_strncpyz( out, title, outSize );
        return;
    }

    Q_strncpyz( out, fallbackName ? fallbackName : "", outSize );
}

static void CG_JukeboxFormatDuration( int duration, char *out, int outSize ) {
    if ( duration <= 0 ) {
        Q_strncpyz( out, "--:--", outSize );
        return;
    }

    {
        int totalSeconds = ( duration + 500 ) / 1000;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        Com_sprintf( out, outSize, "%d:%02d", minutes, seconds );
    }
}

static cgJukeboxRepeatMode_t CG_JukeboxGetRepeatMode( void ) {
    if ( !Q_stricmp( cg_jukeboxRepeatMode.string, "off" ) ) {
        return JUKEBOX_REPEAT_OFF;
    }

    if ( !Q_stricmp( cg_jukeboxRepeatMode.string, "all" ) ) {
        return JUKEBOX_REPEAT_ALL;
    }

    if ( !Q_stricmp( cg_jukeboxRepeatMode.string, "one" ) ) {
        return JUKEBOX_REPEAT_ONE;
    }

    if ( cg_jukeboxRepeatMode.integer <= JUKEBOX_REPEAT_OFF ) {
        return JUKEBOX_REPEAT_OFF;
    }

    if ( cg_jukeboxRepeatMode.integer >= JUKEBOX_REPEAT_ONE ) {
        return JUKEBOX_REPEAT_ONE;
    }

    return (cgJukeboxRepeatMode_t)cg_jukeboxRepeatMode.integer;
}

static const char *CG_JukeboxGetRepeatModeLabel( void ) {
    switch ( CG_JukeboxGetRepeatMode() ) {
        case JUKEBOX_REPEAT_ONE:
            return "one";
        case JUKEBOX_REPEAT_ALL:
            return "all";
        default:
            return "off";
    }
}

static const char *CG_JukeboxGetShuffleLabel( void ) {
    return cg_jukeboxShuffle.integer ? "on" : "off";
}

static void CG_JukeboxDecorateSubtitle( const char *subtitle, char *out, int outSize ) {
    if ( subtitle && *subtitle ) {
        Com_sprintf( out, outSize, "%s  |  Shuffle: %s  Repeat: %s", subtitle,
            CG_JukeboxGetShuffleLabel(), CG_JukeboxGetRepeatModeLabel() );
    } else {
        Com_sprintf( out, outSize, "Shuffle: %s  Repeat: %s",
            CG_JukeboxGetShuffleLabel(), CG_JukeboxGetRepeatModeLabel() );
    }
}

static void CG_JukeboxSetDisplay( const char *status, const char *subtitle ) {
    if ( status && *status ) {
        Q_strncpyz( cg_jukebox.statusLine, status, sizeof( cg_jukebox.statusLine ) );
    } else {
        cg_jukebox.statusLine[0] = '\0';
    }

    if ( subtitle && *subtitle ) {
        Q_strncpyz( cg_jukebox.subtitleLine, subtitle, sizeof( cg_jukebox.subtitleLine ) );
    } else {
        cg_jukebox.subtitleLine[0] = '\0';
    }

    cg_jukebox.displayExpireTime = cg.time + CG_JUKEBOX_DISPLAY_TIME;
}

static int CG_JukeboxCompareEntries( const void *a, const void *b ) {
    const char * const *left = (const char * const *)a;
    const char * const *right = (const char * const *)b;

    return Q_stricmp( *left, *right );
}

static void CG_JukeboxScan( void ) {
    char fileList[CG_JUKEBOX_FILELIST_SIZE];
    const char *entry;
    const char *rawEntries[CG_JUKEBOX_MAX_TRACKS];
    int listed;
    int rawCount;
    int i;

    cg_jukebox.scanned = qtrue;
    cg_jukebox.trackCount = 0;

    listed = trap_FS_GetFileList( CG_JUKEBOX_DIRECTORY, "ogg", fileList, sizeof( fileList ) );
    entry = fileList;
    rawCount = 0;

    for ( i = 0 ; i < listed ; i++ ) {
        int len = strlen( entry );

        if ( len > 0 && rawCount < CG_JUKEBOX_MAX_TRACKS ) {
            rawEntries[rawCount++] = entry;
        }

        entry += len + 1;
    }

    qsort( rawEntries, rawCount, sizeof( rawEntries[0] ), CG_JukeboxCompareEntries );

    for ( i = 0 ; i < rawCount && cg_jukebox.trackCount < CG_JUKEBOX_MAX_TRACKS ; i++ ) {
        int len = strlen( rawEntries[i] );

        if ( len > 0 ) {
            int totalLen = strlen( CG_JUKEBOX_DIRECTORY ) + 1 + len;

            if ( totalLen >= MAX_QPATH ) {
                CG_Printf( "Jukebox: Title path too long: %s/%s\n", CG_JUKEBOX_DIRECTORY, rawEntries[i] );
            } else {
                char *path = cg_jukebox.trackPaths[cg_jukebox.trackCount];
                char *name = cg_jukebox.trackNames[cg_jukebox.trackCount];
                char *title = cg_jukebox.trackTitles[cg_jukebox.trackCount];
                char *artist = cg_jukebox.trackArtists[cg_jukebox.trackCount];

                Com_sprintf( path, MAX_QPATH, "%s/%s", CG_JUKEBOX_DIRECTORY, rawEntries[i] );
                CG_JukeboxFormatName( rawEntries[i], name, MAX_QPATH );
                trap_S_GetStreamMetadata( path, title, CG_JUKEBOX_META_FIELD_LEN, artist, CG_JUKEBOX_META_FIELD_LEN, NULL, 0 );
                cg_jukebox.trackDurations[cg_jukebox.trackCount] = trap_S_GetStreamLength( path );
                cg_jukebox.trackCount++;
            }
        }
    }

    if ( listed > CG_JUKEBOX_MAX_TRACKS ) {
        CG_Printf( "Jukebox: Only the first %i Titles after sorting will be used\n", CG_JUKEBOX_MAX_TRACKS );
    }

    if ( cg_jukebox.trackCount <= 0 ) {
        cg_jukebox.currentTrack = 0;
    } else if ( cg_jukebox.currentTrack >= cg_jukebox.trackCount ) {
        cg_jukebox.currentTrack = 0;
    }
}

static qboolean CG_JukeboxEnsureTracks( void ) {
    if ( !cg_jukebox.scanned || cg_jukebox.trackCount <= 0 ) {
        CG_JukeboxScan();
    }

    if ( cg_jukebox.trackCount <= 0 ) {
        CG_JukeboxSetDisplay( "Jukebox: no Titles found", "Drop .ogg-Files in baseq3r/music/jukebox" );
        CG_Printf( "Jukebox: No .ogg-Dateien in %s\n", CG_JUKEBOX_DIRECTORY );
        return qfalse;
    }

    return qtrue;
}

static void CG_JukeboxPlayIndex( int index ) {
    char durationBuffer[16];
    char statusBuffer[sizeof( cg_jukebox.statusLine )];
    char subtitleBuffer[sizeof( cg_jukebox.subtitleLine )];
    char displayName[MAX_QPATH];

    if ( !CG_JukeboxEnsureTracks() ) {
        return;
    }

    if ( index < 0 || index >= cg_jukebox.trackCount ) {
        index = 0;
    }

    cg_jukebox.currentTrack = index;
    cg_jukebox.active = qtrue;
    cg_jukebox.trackStartTime = cg.time;

    trap_S_StartBackgroundTrack( cg_jukebox.trackPaths[index], cg_jukebox.trackPaths[index] );

    CG_JukeboxFormatDuration( cg_jukebox.trackDurations[index], durationBuffer, sizeof( durationBuffer ) );
    CG_JukeboxBuildDisplayName( cg_jukebox.trackNames[index], cg_jukebox.trackArtists[index], cg_jukebox.trackTitles[index],
        displayName, sizeof( displayName ) );
    Com_sprintf( statusBuffer, sizeof( statusBuffer ), "%s  %s", durationBuffer, displayName );
    CG_JukeboxDecorateSubtitle( va( "Track %i/%i", index + 1, cg_jukebox.trackCount ), subtitleBuffer, sizeof( subtitleBuffer ) );

    CG_JukeboxSetDisplay( statusBuffer, subtitleBuffer );
    CG_Printf( "Jukebox: %s\n", displayName );
}

static void CG_JukeboxStopInternal( qboolean showOverlay );

static void CG_JukeboxAdvance( int step ) {
    int nextIndex;

    if ( !CG_JukeboxEnsureTracks() ) {
        return;
    }

    if ( cg_jukebox.trackCount <= 0 ) {
        return;
    }

    if ( step == 0 ) {
        CG_JukeboxPlayIndex( cg_jukebox.currentTrack );
        return;
    }

    if ( CG_JukeboxGetRepeatMode() == JUKEBOX_REPEAT_ONE ) {
        CG_JukeboxPlayIndex( cg_jukebox.currentTrack );
        return;
    }

    nextIndex = cg_jukebox.currentTrack;

    if ( cg_jukeboxShuffle.integer && cg_jukebox.trackCount > 1 ) {
        int attempts = 0;

        do {
            nextIndex = rand() % cg_jukebox.trackCount;
            attempts++;
        } while ( nextIndex == cg_jukebox.currentTrack && attempts < 8 );

        if ( nextIndex == cg_jukebox.currentTrack ) {
            nextIndex = ( nextIndex + 1 ) % cg_jukebox.trackCount;
        }
    } else if ( step > 0 ) {
        if ( cg_jukebox.currentTrack >= cg_jukebox.trackCount - 1 && CG_JukeboxGetRepeatMode() == JUKEBOX_REPEAT_OFF ) {
            CG_JukeboxStopInternal( qfalse );
            return;
        }

        nextIndex = ( nextIndex + 1 ) % cg_jukebox.trackCount;
    } else if ( step < 0 ) {
        if ( cg_jukebox.currentTrack <= 0 && CG_JukeboxGetRepeatMode() == JUKEBOX_REPEAT_OFF ) {
            CG_JukeboxStopInternal( qfalse );
            return;
        }

        nextIndex = ( nextIndex - 1 + cg_jukebox.trackCount ) % cg_jukebox.trackCount;
    }

    CG_JukeboxPlayIndex( nextIndex );
}

static void CG_JukeboxStopInternal( qboolean showOverlay ) {
    if ( !cg_jukebox.active ) {
        if ( showOverlay ) {
            if ( cg_jukebox.trackCount > 0 ) {
                char subtitle[64];
                CG_JukeboxDecorateSubtitle( va( "Ready: %i Title", cg_jukebox.trackCount ), subtitle, sizeof( subtitle ) );
                CG_JukeboxSetDisplay( "Jukebox paused", subtitle );
            } else {
                CG_JukeboxSetDisplay( "Jukebox paused", NULL );
            }
        }
        return;
    }

    trap_S_StopBackgroundTrack();
    cg_jukebox.active = qfalse;
    cg_jukebox.trackStartTime = 0;

    CG_StartMusic();

    if ( showOverlay ) {
        char subtitle[64];
        if ( cg_jukebox.trackCount > 0 ) {
            CG_JukeboxDecorateSubtitle( va( "Ready: %i Title", cg_jukebox.trackCount ), subtitle, sizeof( subtitle ) );
            CG_JukeboxSetDisplay( "Jukebox stopped", subtitle );
        } else {
            CG_JukeboxSetDisplay( "Jukebox stopped", NULL );
        }
    }

    CG_Printf( "Jukebox stopped\n" );
}

void CG_JukeboxInit( void ) {
    memset( &cg_jukebox, 0, sizeof( cg_jukebox ) );

    trap_Cvar_Update( &cg_jukeboxShuffle );
    trap_Cvar_Update( &cg_jukeboxRepeatMode );
}

void CG_JukeboxFrame( void ) {
    if ( !cg_jukebox.active ) {
        return;
    }

    if ( cg_jukebox.trackCount <= 0 ) {
        return;
    }

    if ( cg_jukebox.trackDurations[cg_jukebox.currentTrack] <= 0 ) {
        return;
    }

    if ( cg.time - cg_jukebox.trackStartTime >= cg_jukebox.trackDurations[cg_jukebox.currentTrack] ) {
        CG_JukeboxAdvance( 1 );
    }
}

void CG_JukeboxDraw( float x, float y, float w, float h ) {
    vec4_t backgroundColor = { 0.05f, 0.05f, 0.05f, 0.75f };
    vec4_t borderColor = { 1.0f, 1.0f, 1.0f, 0.25f };
    vec4_t subtitleColor = { 0.8f, 0.8f, 0.8f, 1.0f };
    vec4_t progressBackgroundColor = { 0.0f, 0.0f, 0.0f, 0.45f };
    vec4_t progressFillColor = { 0.23f, 0.68f, 0.95f, 0.9f };
    vec4_t progressHighlightColor = { 0.8f, 0.92f, 1.0f, 0.6f };
    vec4_t progressBorderColor = { 1.0f, 1.0f, 1.0f, 0.35f };
    int textX;
    int headerY;
    int infoY;
    qboolean showProgress = qfalse;
    float progress = 0.0f;
    char timeLine[32];
    qboolean haveTimeLine = qfalse;
    float progressY;

    if ( cg_jukebox.displayExpireTime <= cg.time || !cg_jukebox.statusLine[0] ) {
        return;
    }

    CG_FillRect( x, y, w, h, backgroundColor );
    CG_DrawRect( x, y, w, h, 1.0f, borderColor );

    textX = (int)( x + CG_JUKEBOX_TEXT_MARGIN );
    headerY = (int)( y + CG_JUKEBOX_TEXT_MARGIN );
    infoY = (int)( y + h - CG_JUKEBOX_FOOTER_MARGIN - SMALLCHAR_HEIGHT );

    if ( infoY < headerY + SMALLCHAR_HEIGHT + 2 ) {
        infoY = headerY + SMALLCHAR_HEIGHT + 2;
    }

    progressY = infoY - CG_JUKEBOX_FOOTER_GAP - CG_JUKEBOX_PROGRESS_HEIGHT;

    if ( progressY < headerY + SMALLCHAR_HEIGHT + 2.0f ) {
        progressY = headerY + SMALLCHAR_HEIGHT + 2.0f;
    }

    if ( cg_jukebox.active && cg_jukebox.trackCount > 0 ) {
        int duration = cg_jukebox.trackDurations[cg_jukebox.currentTrack];

        if ( duration > 0 ) {
            int elapsed = cg.time - cg_jukebox.trackStartTime;
            char elapsedBuffer[16];
            char durationBuffer[16];

            if ( elapsed < 0 ) {
                elapsed = 0;
            }

            if ( elapsed > duration ) {
                elapsed = duration;
            }

            progress = (float)elapsed / (float)duration;
            showProgress = qtrue;

            CG_JukeboxFormatDuration( elapsed, elapsedBuffer, sizeof( elapsedBuffer ) );
            CG_JukeboxFormatDuration( duration, durationBuffer, sizeof( durationBuffer ) );
            Com_sprintf( timeLine, sizeof( timeLine ), "%s / %s", elapsedBuffer, durationBuffer );
            haveTimeLine = qtrue;
        }
    }

    CG_DrawSmallStringColor( textX, headerY, cg_jukebox.statusLine, colorWhite );

    if ( haveTimeLine ) {
        int timeWidth = CG_DrawStrlen( timeLine ) * SMALLCHAR_WIDTH;
        int timeX = (int)( x + w - CG_JUKEBOX_TEXT_MARGIN - timeWidth );
        int timeY = infoY;
        int subtitleWidth = 0;

        if ( cg_jukebox.subtitleLine[0] ) {
            subtitleWidth = CG_DrawStrlen( cg_jukebox.subtitleLine ) * SMALLCHAR_WIDTH;
        }

        if ( timeX < textX ) {
            timeX = textX;
        }

        if ( cg_jukebox.subtitleLine[0] && timeX <= textX + subtitleWidth + 4 ) {
            timeY = infoY - SMALLCHAR_HEIGHT - 2;

            if ( timeY < headerY + SMALLCHAR_HEIGHT ) {
                timeY = headerY + SMALLCHAR_HEIGHT;
            }
        }

        CG_DrawSmallStringColor( timeX, timeY, timeLine, subtitleColor );
    }

    if ( cg_jukebox.subtitleLine[0] ) {
        CG_DrawSmallStringColor( textX, infoY, cg_jukebox.subtitleLine, subtitleColor );
    }

    if ( showProgress ) {
        float progressX = x + CG_JUKEBOX_PROGRESS_MARGIN;
        float progressW = w - ( CG_JUKEBOX_PROGRESS_MARGIN * 2.0f );
        float progressH = CG_JUKEBOX_PROGRESS_HEIGHT;

        if ( progressW < 0.0f ) {
            progressW = 0.0f;
        }

        CG_FillRect( progressX, progressY, progressW, progressH, progressBackgroundColor );

        if ( progress > 0.0f ) {
            float filledWidth = progressW * progress;

            if ( filledWidth > progressW ) {
                filledWidth = progressW;
            }

            CG_FillRect( progressX, progressY, filledWidth, progressH, progressFillColor );
            CG_FillRect( progressX, progressY, filledWidth, progressH * 0.5f, progressHighlightColor );
        }

        CG_DrawRect( progressX, progressY, progressW, progressH, 1.0f, progressBorderColor );
    }
}

void CG_JukeboxToggle_f( void ) {
    if ( cg_jukebox.active ) {
        CG_JukeboxStopInternal( qtrue );
        return;
    }

    if ( !CG_JukeboxEnsureTracks() ) {
        return;
    }

    CG_JukeboxPlayIndex( cg_jukebox.currentTrack );
}

void CG_JukeboxNext_f( void ) {
    if ( !CG_JukeboxEnsureTracks() ) {
        return;
    }

    if ( !cg_jukebox.active && cg_jukebox.trackCount > 0 ) {
        CG_JukeboxPlayIndex( ( cg_jukebox.currentTrack + 1 ) % cg_jukebox.trackCount );
        return;
    }

    CG_JukeboxAdvance( 1 );
}

void CG_JukeboxPrev_f( void ) {
    if ( !CG_JukeboxEnsureTracks() ) {
        return;
    }

    if ( !cg_jukebox.active && cg_jukebox.trackCount > 0 ) {
        CG_JukeboxPlayIndex( ( cg_jukebox.currentTrack - 1 + cg_jukebox.trackCount ) % cg_jukebox.trackCount );
        return;
    }

	CG_JukeboxAdvance( -1 );
}

void CG_JukeboxRescan_f( void ) {
	char subtitle[96];

	cg_jukebox.scanned = qfalse;
	CG_JukeboxScan();

	if ( cg_jukebox.trackCount <= 0 ) {
		cg_jukebox.currentTrack = 0;
	} else if ( cg_jukebox.currentTrack < 0 ) {
		cg_jukebox.currentTrack = 0;
	} else if ( cg_jukebox.currentTrack >= cg_jukebox.trackCount ) {
		cg_jukebox.currentTrack = cg_jukebox.trackCount - 1;
	}

	CG_JukeboxDecorateSubtitle( va( "%i Track%s gefunden", cg_jukebox.trackCount,
		( cg_jukebox.trackCount == 1 ) ? "" : "s" ), subtitle, sizeof( subtitle ) );
	CG_JukeboxSetDisplay( "Jukebox neu gescannt", subtitle );

	CG_Printf( "Jukebox: Scan abgeschlossen (%i Tracks)\n", cg_jukebox.trackCount );
}

void CG_JukeboxShuffleToggle_f( void ) {
    char subtitle[96];

    trap_Cvar_Set( "cg_jukeboxShuffle", cg_jukeboxShuffle.integer ? "0" : "1" );
    trap_Cvar_Update( &cg_jukeboxShuffle );

    CG_JukeboxDecorateSubtitle( cg_jukebox.active ? cg_jukebox.trackNames[cg_jukebox.currentTrack] : "Jukebox",
        subtitle, sizeof( subtitle ) );
    CG_JukeboxSetDisplay( cg_jukeboxShuffle.integer ? "Shuffle aktiviert" : "Shuffle deaktiviert", subtitle );
}

void CG_JukeboxRepeatCycle_f( void ) {
    cgJukeboxRepeatMode_t repeatMode = CG_JukeboxGetRepeatMode();
    char subtitle[96];

    repeatMode = (cgJukeboxRepeatMode_t)( ( repeatMode + 1 ) % 3 );

    switch ( repeatMode ) {
        case JUKEBOX_REPEAT_ONE:
            trap_Cvar_Set( "cg_jukeboxRepeatMode", "one" );
            break;
        case JUKEBOX_REPEAT_ALL:
            trap_Cvar_Set( "cg_jukeboxRepeatMode", "all" );
            break;
        default:
            trap_Cvar_Set( "cg_jukeboxRepeatMode", "off" );
            break;
    }

    trap_Cvar_Update( &cg_jukeboxRepeatMode );

    CG_JukeboxDecorateSubtitle( cg_jukebox.active ? cg_jukebox.trackNames[cg_jukebox.currentTrack] : "Jukebox",
        subtitle, sizeof( subtitle ) );
    CG_JukeboxSetDisplay( va( "Repeat: %s", CG_JukeboxGetRepeatModeLabel() ), subtitle );
}
