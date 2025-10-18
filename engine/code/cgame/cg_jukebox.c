/*
===========================================================================
Copyright (C) 2025 Q3Rally Team (Per Thormann - q3rally@gmail.com)

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

typedef struct {
    qboolean    scanned;
    qboolean    active;
    int         trackCount;
    int         currentTrack;
    int         trackDurations[CG_JUKEBOX_MAX_TRACKS];
    char        trackPaths[CG_JUKEBOX_MAX_TRACKS][MAX_QPATH];
    char        trackNames[CG_JUKEBOX_MAX_TRACKS][MAX_QPATH];
    int         trackStartTime;
    int         displayExpireTime;
    char        statusLine[128];
    char        subtitleLine[64];
} cgJukeboxState_t;

static cgJukeboxState_t cg_jukebox;

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

static void CG_JukeboxScan( void ) {
    char fileList[CG_JUKEBOX_FILELIST_SIZE];
    const char *entry;
    int listed;
    int i;

    cg_jukebox.scanned = qtrue;
    cg_jukebox.trackCount = 0;

    listed = trap_FS_GetFileList( CG_JUKEBOX_DIRECTORY, "ogg", fileList, sizeof( fileList ) );
    entry = fileList;

    for ( i = 0 ; i < listed && cg_jukebox.trackCount < CG_JUKEBOX_MAX_TRACKS ; i++ ) {
        int len = strlen( entry );

        if ( len > 0 ) {
            int totalLen = strlen( CG_JUKEBOX_DIRECTORY ) + 1 + len;

            if ( totalLen >= MAX_QPATH ) {
                CG_Printf( "Jukebox: Title path too long: %s/%s\n", CG_JUKEBOX_DIRECTORY, entry );
            } else {
                char *path = cg_jukebox.trackPaths[cg_jukebox.trackCount];
                char *name = cg_jukebox.trackNames[cg_jukebox.trackCount];

                Com_sprintf( path, MAX_QPATH, "%s/%s", CG_JUKEBOX_DIRECTORY, entry );
                CG_JukeboxFormatName( entry, name, MAX_QPATH );
                cg_jukebox.trackDurations[cg_jukebox.trackCount] = trap_S_GetStreamLength( path );
                cg_jukebox.trackCount++;
            }
        }

        entry += len + 1;
    }

    if ( listed > CG_JUKEBOX_MAX_TRACKS ) {
        CG_Printf( "Jukebox: Only the first %i Titles will be used\n", CG_JUKEBOX_MAX_TRACKS );
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
    Com_sprintf( statusBuffer, sizeof( statusBuffer ), "%s  %s", durationBuffer, cg_jukebox.trackNames[index] );
    Com_sprintf( subtitleBuffer, sizeof( subtitleBuffer ), "Track %i/%i", index + 1, cg_jukebox.trackCount );

    CG_JukeboxSetDisplay( statusBuffer, subtitleBuffer );
    CG_Printf( "Jukebox: %s\n", cg_jukebox.trackNames[index] );
}

static void CG_JukeboxAdvance( int step ) {
    int nextIndex;

    if ( !CG_JukeboxEnsureTracks() ) {
        return;
    }

    if ( cg_jukebox.trackCount <= 0 ) {
        return;
    }

    nextIndex = cg_jukebox.currentTrack;

    if ( step > 0 ) {
        nextIndex = ( nextIndex + 1 ) % cg_jukebox.trackCount;
    } else if ( step < 0 ) {
        nextIndex = ( nextIndex - 1 + cg_jukebox.trackCount ) % cg_jukebox.trackCount;
    }

    CG_JukeboxPlayIndex( nextIndex );
}

static void CG_JukeboxStopInternal( qboolean showOverlay ) {
    if ( !cg_jukebox.active ) {
        if ( showOverlay ) {
            if ( cg_jukebox.trackCount > 0 ) {
                char subtitle[32];
                Com_sprintf( subtitle, sizeof( subtitle ), "Ready: %i Title", cg_jukebox.trackCount );
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
        char subtitle[32];
        if ( cg_jukebox.trackCount > 0 ) {
            Com_sprintf( subtitle, sizeof( subtitle ), "Ready: %i Title", cg_jukebox.trackCount );
            CG_JukeboxSetDisplay( "Jukebox stopped", subtitle );
        } else {
            CG_JukeboxSetDisplay( "Jukebox stopped", NULL );
        }
    }

    CG_Printf( "Jukebox stopped\n" );
}

void CG_JukeboxInit( void ) {
    memset( &cg_jukebox, 0, sizeof( cg_jukebox ) );
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
        if ( cg_jukebox.trackCount > 1 ) {
            CG_JukeboxAdvance( 1 );
        } else {
            cg_jukebox.trackStartTime = cg.time;
        }
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

