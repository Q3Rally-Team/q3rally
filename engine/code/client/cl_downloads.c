/*
===========================================================================
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
// cl_downloads.c -- Q3Rally in-game content download manager
//
// Architecture:
//   The QVM UI cannot make HTTP requests directly (sandbox).
//   This module bridges the gap via CVars:
//
//   QVM writes:  ui_dl_action   "fetch_index" | "download:<id>" | "cancel"
//   Engine writes: ui_dl_state    0=idle 1=fetching 2=ready 3=downloading 4=done 5=error
//                  ui_dl_progress 0-100 (download progress)
//                  ui_dl_filename current filename being transferred
//                  ui_dl_error    error message if state==5
//                  ui_dl_indexpath absolute path to fetched index JSON (temp file)
//
// Flow:
//   1. UI sets ui_dl_action = "fetch_index"
//   2. Engine GETs https://content.q3rally.com/index.json
//   3. Engine writes JSON to temp file, sets ui_dl_indexpath, state=ready
//   4. UI parses index, user picks content, UI sets ui_dl_action = "download:<id>"
//   5. Engine GETs the pk3 URL for that id, streams to downloads/<id>.pk3
//   6. Engine updates ui_dl_progress, sets state=done on success

#include "client.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef USE_CURL
#ifdef USE_LOCAL_HEADERS
#include "../curl-7.54.0/include/curl/curl.h"
#else
#include <curl/curl.h>
#endif
#ifdef USE_CURL_DLOPEN
#include "../sys/sys_loadlib.h"
#endif
#endif

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

#define DL_INDEX_URL            "https://content.q3rally.com/index.json"
#define DL_INDEX_MAX_SIZE       (256 * 1024)    // 256 KB – index should never exceed this
#define DL_PK3_MAX_SIZE         (512 * 1024 * 1024) // 512 MB max pk3
#define DL_CONNECT_TIMEOUT      15L
#define DL_TRANSFER_TIMEOUT     300L            // 5 min for large pk3s
#define DL_PROGRESS_INTERVAL_MS 200             // update CVar at most every 200ms
#define DL_ACTION_POLL_MS       100             // poll ui_dl_action every 100ms
#define DL_INDEX_TMPNAME        "q3rally_dl_index.json"

// State values – must match DL_STATE_* in ui_rally_downloads.c
#define DL_STATE_IDLE           0
#define DL_STATE_FETCHING       1
#define DL_STATE_READY          2
#define DL_STATE_DOWNLOADING    3
#define DL_STATE_DONE           4
#define DL_STATE_ERROR          5

// ---------------------------------------------------------------------------
// curl dlopen shim – identical pattern to sv_ladder.c
// ---------------------------------------------------------------------------

#ifdef USE_CURL
#ifdef USE_CURL_DLOPEN
static void *cl_dl_curlLib;
static CURL*             (*cl_curl_easy_init)(void);
static CURLcode          (*cl_curl_easy_setopt)(CURL*, CURLoption, ...);
static CURLcode          (*cl_curl_easy_getinfo)(CURL*, CURLINFO, ...);
static void              (*cl_curl_easy_cleanup)(CURL*);
static const char*       (*cl_curl_easy_strerror)(CURLcode);
static CURLM*            (*cl_curl_multi_init)(void);
static CURLMcode         (*cl_curl_multi_add_handle)(CURLM*, CURL*);
static CURLMcode         (*cl_curl_multi_remove_handle)(CURLM*, CURL*);
static CURLMcode         (*cl_curl_multi_perform)(CURLM*, int*);
static CURLMcode         (*cl_curl_multi_cleanup)(CURLM*);
static CURLMsg*          (*cl_curl_multi_info_read)(CURLM*, int*);
static const char*       (*cl_curl_multi_strerror)(CURLMcode);
static struct curl_slist*(*cl_curl_slist_append)(struct curl_slist*, const char*);
static void              (*cl_curl_slist_free_all)(struct curl_slist*);
#else
#define cl_curl_easy_init           curl_easy_init
#define cl_curl_easy_setopt         curl_easy_setopt
#define cl_curl_easy_getinfo        curl_easy_getinfo
#define cl_curl_easy_cleanup        curl_easy_cleanup
#define cl_curl_easy_strerror       curl_easy_strerror
#define cl_curl_multi_init          curl_multi_init
#define cl_curl_multi_add_handle    curl_multi_add_handle
#define cl_curl_multi_remove_handle curl_multi_remove_handle
#define cl_curl_multi_perform       curl_multi_perform
#define cl_curl_multi_cleanup       curl_multi_cleanup
#define cl_curl_multi_info_read     curl_multi_info_read
#define cl_curl_multi_strerror      curl_multi_strerror
#define cl_curl_slist_append        curl_slist_append
#define cl_curl_slist_free_all      curl_slist_free_all
#endif
#endif

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

typedef enum {
    CLREQ_INDEX,        // fetch content index JSON
    CLREQ_DOWNLOAD,     // download a pk3
    CLREQ_PREVIEW       // fetch a preview image
} clDlRequestType_t;

typedef struct {
    clDlRequestType_t   type;

    // For CLREQ_DOWNLOAD
    char                id[64];
    char                url[512];
    char                destPath[MAX_OSPATH];   // absolute path on disk

    // Receive buffer (index fetch)
    char                *buf;
    size_t              bufLen;
    size_t              bufCap;

    // File handle (pk3 download)
    FILE                *file;
    curl_off_t          totalBytes;
    int                 lastProgressUpdate;

    // curl handles
#ifdef USE_CURL
    CURL                *easy;
    struct curl_slist   *headers;
    char                errorBuf[CURL_ERROR_SIZE];
#endif
} clDlRequest_t;

// Preview queue – up to 64 pending preview fetches after index load
#define DL_MAX_PREVIEW_QUEUE    64

typedef struct {
    char    id[64];
    char    url[512];
} clDlPreviewEntry_t;

typedef struct {
    qboolean            initialized;
    qboolean            curlLoaded;
    qboolean            warnedNoCurl;
    int                 state;              // DL_STATE_*
    int                 lastActionPoll;
    char                pendingAction[256]; // copy of ui_dl_action before clear

    clDlRequest_t       *active;

    // Preview image queue – drained one at a time after index is ready
    clDlPreviewEntry_t  previewQueue[DL_MAX_PREVIEW_QUEUE];
    int                 previewQueueHead;   // next entry to fetch
    int                 previewQueueCount;  // total entries enqueued
    qboolean            previewActive;      // a CLREQ_PREVIEW is in flight

#ifdef USE_CURL
    CURLM               *multi;
#endif
} clDlState_t;

static clDlState_t cl_dl;

// CVars – declared extern in client.h / registered in cl_main.c
// We access them directly via Cvar_* here to avoid QVM linkage.

// ---------------------------------------------------------------------------
// CVar helpers
// ---------------------------------------------------------------------------

static void CL_DL_SetState( int state ) {
    char buf[8];
    cl_dl.state = state;
    Com_sprintf( buf, sizeof( buf ), "%d", state );
    Cvar_Set( "ui_dl_state", buf );
}

static void CL_DL_SetProgress( float pct ) {
    char buf[16];
    Com_sprintf( buf, sizeof( buf ), "%.1f", pct );
    Cvar_Set( "ui_dl_progress", buf );
}

static void CL_DL_SetFilename( const char *name ) {
    Cvar_Set( "ui_dl_filename", name ? name : "" );
}

static void CL_DL_SetError( const char *msg ) {
    Cvar_Set( "ui_dl_error", msg ? msg : "" );
    CL_DL_SetState( DL_STATE_ERROR );
    CL_DL_SetFilename( "" );
    CL_DL_SetProgress( 0.0f );
}

static void CL_DL_SetIndexPath( const char *path ) {
    Cvar_Set( "ui_dl_indexpath", path ? path : "" );
}

// ---------------------------------------------------------------------------
// curl dlopen loader – mirrors sv_ladder.c
// ---------------------------------------------------------------------------

#ifdef USE_CURL
#ifdef USE_CURL_DLOPEN
#ifdef WIN32
#define CL_DL_CURL_LIB      "libcurl-4.dll"
#define CL_DL_CURL_LIB_ALT  "libcurl-3.dll"
#elif defined(__APPLE__)
#define CL_DL_CURL_LIB      "libcurl.dylib"
#else
#define CL_DL_CURL_LIB      "libcurl.so.4"
#define CL_DL_CURL_LIB_ALT  "libcurl.so.3"
#endif

static void *CL_DL_GPA( const char *sym ) {
    void *f = Sys_LoadFunction( cl_dl_curlLib, sym );
    if ( !f ) {
        Com_Printf( "CL_Downloads: missing curl symbol: %s\n", sym );
    }
    return f;
}

static qboolean CL_DL_LoadCurl( void ) {
    if ( cl_dl.curlLoaded ) return qtrue;

    if ( !cl_dl_curlLib ) {
        cl_dl_curlLib = Sys_LoadDll( CL_DL_CURL_LIB, qtrue );
#ifdef CL_DL_CURL_LIB_ALT
        if ( !cl_dl_curlLib ) {
            cl_dl_curlLib = Sys_LoadDll( CL_DL_CURL_LIB_ALT, qtrue );
        }
#endif
        if ( !cl_dl_curlLib ) {
            if ( !cl_dl.warnedNoCurl ) {
                Com_Printf( "CL_Downloads: unable to load %s\n", CL_DL_CURL_LIB );
                cl_dl.warnedNoCurl = qtrue;
            }
            return qfalse;
        }
    }

    cl_curl_easy_init           = CL_DL_GPA( "curl_easy_init" );
    cl_curl_easy_setopt         = CL_DL_GPA( "curl_easy_setopt" );
    cl_curl_easy_getinfo        = CL_DL_GPA( "curl_easy_getinfo" );
    cl_curl_easy_cleanup        = CL_DL_GPA( "curl_easy_cleanup" );
    cl_curl_easy_strerror       = CL_DL_GPA( "curl_easy_strerror" );
    cl_curl_multi_init          = CL_DL_GPA( "curl_multi_init" );
    cl_curl_multi_add_handle    = CL_DL_GPA( "curl_multi_add_handle" );
    cl_curl_multi_remove_handle = CL_DL_GPA( "curl_multi_remove_handle" );
    cl_curl_multi_perform       = CL_DL_GPA( "curl_multi_perform" );
    cl_curl_multi_cleanup       = CL_DL_GPA( "curl_multi_cleanup" );
    cl_curl_multi_info_read     = CL_DL_GPA( "curl_multi_info_read" );
    cl_curl_multi_strerror      = CL_DL_GPA( "curl_multi_strerror" );
    cl_curl_slist_append        = CL_DL_GPA( "curl_slist_append" );
    cl_curl_slist_free_all      = CL_DL_GPA( "curl_slist_free_all" );

    if ( !cl_curl_easy_init || !cl_curl_easy_setopt || !cl_curl_easy_getinfo ||
         !cl_curl_easy_cleanup || !cl_curl_easy_strerror || !cl_curl_multi_init ||
         !cl_curl_multi_add_handle || !cl_curl_multi_remove_handle ||
         !cl_curl_multi_perform || !cl_curl_multi_cleanup ||
         !cl_curl_multi_info_read || !cl_curl_multi_strerror ||
         !cl_curl_slist_append || !cl_curl_slist_free_all ) {
        Com_Printf( "CL_Downloads: failed to resolve required curl symbols\n" );
        cl_dl.warnedNoCurl = qtrue;
        return qfalse;
    }

    cl_dl.curlLoaded = qtrue;
    cl_dl.warnedNoCurl = qfalse;
    return qtrue;
}
#else
static qboolean CL_DL_LoadCurl( void ) {
    cl_dl.curlLoaded = qtrue;
    return qtrue;
}
#endif // USE_CURL_DLOPEN

static qboolean CL_DL_EnsureCurl( void ) {
    if ( !CL_DL_LoadCurl() ) return qfalse;

    if ( !cl_dl.multi ) {
        cl_dl.multi = cl_curl_multi_init();
        if ( !cl_dl.multi ) {
            Com_Printf( "CL_Downloads: curl_multi_init failed\n" );
            return qfalse;
        }
    }
    return qtrue;
}

static void CL_DL_ShutdownCurl( void ) {
    if ( cl_dl.multi ) {
        cl_curl_multi_cleanup( cl_dl.multi );
        cl_dl.multi = NULL;
    }
#ifdef USE_CURL_DLOPEN
    if ( cl_dl_curlLib ) {
        Sys_UnloadLibrary( cl_dl_curlLib );
        cl_dl_curlLib = NULL;
    }
    cl_curl_easy_init           = NULL;
    cl_curl_easy_setopt         = NULL;
    cl_curl_easy_getinfo        = NULL;
    cl_curl_easy_cleanup        = NULL;
    cl_curl_easy_strerror       = NULL;
    cl_curl_multi_init          = NULL;
    cl_curl_multi_add_handle    = NULL;
    cl_curl_multi_remove_handle = NULL;
    cl_curl_multi_perform       = NULL;
    cl_curl_multi_cleanup       = NULL;
    cl_curl_multi_info_read     = NULL;
    cl_curl_multi_strerror      = NULL;
    cl_curl_slist_append        = NULL;
    cl_curl_slist_free_all      = NULL;
#endif
    cl_dl.curlLoaded = qfalse;
}

// ---------------------------------------------------------------------------
// curl callbacks
// ---------------------------------------------------------------------------

// Write callback for index fetch – accumulates into heap buffer
static size_t CL_DL_WriteIndex( void *ptr, size_t size, size_t nmemb, void *userdata ) {
    clDlRequest_t   *req = (clDlRequest_t *)userdata;
    size_t           bytes = size * nmemb;
    size_t           needed;
    char            *newBuf;

    if ( !req ) return bytes;

    needed = req->bufLen + bytes + 1;
    if ( needed > DL_INDEX_MAX_SIZE ) {
        Com_Printf( "CL_Downloads: index exceeds size limit, aborting\n" );
        return 0; // causes CURLE_WRITE_ERROR → abort
    }

    if ( needed > req->bufCap ) {
        req->bufCap = needed * 2;
        newBuf = realloc( req->buf, req->bufCap );
        if ( !newBuf ) {
            Com_Printf( "CL_Downloads: out of memory reading index\n" );
            return 0;
        }
        req->buf = newBuf;
    }

    Com_Memcpy( req->buf + req->bufLen, ptr, bytes );
    req->bufLen += bytes;
    req->buf[req->bufLen] = '\0';
    return bytes;
}

// Write callback for pk3 download – streams directly to file
static size_t CL_DL_WriteFile( void *ptr, size_t size, size_t nmemb, void *userdata ) {
    clDlRequest_t *req = (clDlRequest_t *)userdata;
    size_t bytes = size * nmemb;
    size_t written;

    if ( !req || !req->file ) return 0;

    written = fwrite( ptr, 1, bytes, req->file );
    return written;
}

// Progress callback – updates ui_dl_progress CVar throttled by DL_PROGRESS_INTERVAL_MS
static int CL_DL_ProgressCallback( void *userdata,
                                    curl_off_t dltotal, curl_off_t dlnow,
                                    curl_off_t ultotal, curl_off_t ulnow ) {
    clDlRequest_t   *req = (clDlRequest_t *)userdata;
    int              now;
    float            pct;

    (void)ultotal; (void)ulnow;

    if ( !req ) return 0;
    if ( cl_dl.state != DL_STATE_DOWNLOADING ) return 0;

    now = Sys_Milliseconds();
    if ( now - req->lastProgressUpdate < DL_PROGRESS_INTERVAL_MS ) return 0;
    req->lastProgressUpdate = now;

    if ( dltotal > 0 ) {
        pct = (float)dlnow / (float)dltotal * 100.0f;
        if ( pct > 100.0f ) pct = 100.0f;
    } else {
        pct = 0.0f;
    }

    req->totalBytes = dltotal;
    CL_DL_SetProgress( pct );
    return 0;
}

#endif // USE_CURL

// ---------------------------------------------------------------------------
// Request lifecycle
// ---------------------------------------------------------------------------

static clDlRequest_t *CL_DL_AllocRequest( void ) {
    clDlRequest_t *req = (clDlRequest_t *)Z_Malloc( sizeof( clDlRequest_t ) );
    if ( req ) {
        Com_Memset( req, 0, sizeof( *req ) );
    }
    return req;
}

static void CL_DL_FreeRequest( clDlRequest_t *req ) {
    if ( !req ) return;

#ifdef USE_CURL
    if ( req->headers ) {
        cl_curl_slist_free_all( req->headers );
        req->headers = NULL;
    }
    if ( req->easy ) {
        if ( cl_dl.multi ) {
            cl_curl_multi_remove_handle( cl_dl.multi, req->easy );
        }
        cl_curl_easy_cleanup( req->easy );
        req->easy = NULL;
    }
#endif

    if ( req->buf ) {
        free( req->buf );
        req->buf = NULL;
    }

    if ( req->file ) {
        fclose( req->file );
        req->file = NULL;
    }

    Z_Free( req );
}


// ---------------------------------------------------------------------------
// Start requests
// ---------------------------------------------------------------------------

#ifdef USE_CURL
static qboolean CL_DL_StartIndexFetch( clDlRequest_t *req ) {
    CURL    *easy;
    CURLcode code;

    if ( !CL_DL_EnsureCurl() ) return qfalse;

    easy = cl_curl_easy_init();
    if ( !easy ) return qfalse;

    req->easy = easy;
    req->errorBuf[0] = '\0';
    req->buf    = (char *)malloc( 4096 );
    req->bufLen = 0;
    req->bufCap = 4096;
    if ( !req->buf ) {
        cl_curl_easy_cleanup( easy );
        req->easy = NULL;
        return qfalse;
    }
    req->buf[0] = '\0';

    code  = cl_curl_easy_setopt( easy, CURLOPT_URL,           DL_INDEX_URL );
    code |= cl_curl_easy_setopt( easy, CURLOPT_WRITEFUNCTION, CL_DL_WriteIndex );
    code |= cl_curl_easy_setopt( easy, CURLOPT_WRITEDATA,     req );
    code |= cl_curl_easy_setopt( easy, CURLOPT_ERRORBUFFER,   req->errorBuf );
    code |= cl_curl_easy_setopt( easy, CURLOPT_FOLLOWLOCATION,1L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_MAXREDIRS,     5L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_CONNECTTIMEOUT,DL_CONNECT_TIMEOUT );
    code |= cl_curl_easy_setopt( easy, CURLOPT_TIMEOUT,       60L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_NOSIGNAL,      1L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_USERAGENT,     Q3_VERSION );
    code |= cl_curl_easy_setopt( easy, CURLOPT_PRIVATE,       req );

    if ( code != CURLE_OK ) {
        Com_Printf( "CL_Downloads: curl_easy_setopt failed for index fetch\n" );
        return qfalse;
    }

    if ( cl_curl_multi_add_handle( cl_dl.multi, easy ) != CURLM_OK ) {
        return qfalse;
    }

    return qtrue;
}

static qboolean CL_DL_StartPk3Download( clDlRequest_t *req ) {
    CURL        *easy;
    CURLcode     code;
    char         dirPath[MAX_OSPATH];
    char         pathCopy[MAX_OSPATH];

    if ( !CL_DL_EnsureCurl() ) return qfalse;

    // Destination: <fs_homepath>/<basegame>/<id>.pk3
    // Must land in basegame so the engine finds it on next map load.
    {
        char homePath[MAX_OSPATH];
        char basegame[MAX_OSPATH];
        Q_strncpyz( homePath, Cvar_VariableString( "fs_homepath" ), sizeof( homePath ) );
        Cvar_VariableStringBuffer( "fs_basegame", basegame, sizeof( basegame ) );
        if ( !basegame[0] ) Q_strncpyz( basegame, "baseq3r", sizeof( basegame ) );
        Com_sprintf( dirPath, sizeof( dirPath ), "%s%c%s", homePath, PATH_SEP, basegame );
        if ( !Sys_Mkdir( dirPath ) ) {
            Com_Printf( "CL_Downloads: failed to create '%s'\n", dirPath );
            return qfalse;
        }
    }

    // Destination: <fs_homepath>/<basegame>/<id>.pk3
    Com_sprintf( req->destPath, sizeof( req->destPath ),
                 "%s%c%s.pk3", dirPath, PATH_SEP, req->id );

    // Create intermediate directories (FS_CreatePath needs a copy)
    Q_strncpyz( pathCopy, req->destPath, sizeof( pathCopy ) );
    if ( FS_CreatePath( pathCopy ) ) {
        Com_Printf( "CL_Downloads: failed to create path for '%s'\n", req->destPath );
        return qfalse;
    }

    req->file = Sys_FOpen( req->destPath, "wb" );
    if ( !req->file ) {
        Com_Printf( "CL_Downloads: failed to open '%s' for writing (%s)\n",
                    req->destPath, strerror( errno ) );
        return qfalse;
    }

    easy = cl_curl_easy_init();
    if ( !easy ) {
        fclose( req->file );
        req->file = NULL;
        return qfalse;
    }

    req->easy = easy;
    req->errorBuf[0] = '\0';
    req->lastProgressUpdate = 0;

    code  = cl_curl_easy_setopt( easy, CURLOPT_URL,              req->url );
    code |= cl_curl_easy_setopt( easy, CURLOPT_WRITEFUNCTION,    CL_DL_WriteFile );
    code |= cl_curl_easy_setopt( easy, CURLOPT_WRITEDATA,        req );
    code |= cl_curl_easy_setopt( easy, CURLOPT_XFERINFOFUNCTION, CL_DL_ProgressCallback );
    code |= cl_curl_easy_setopt( easy, CURLOPT_XFERINFODATA,     req );
    code |= cl_curl_easy_setopt( easy, CURLOPT_NOPROGRESS,       0L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_ERRORBUFFER,      req->errorBuf );
    code |= cl_curl_easy_setopt( easy, CURLOPT_FOLLOWLOCATION,   1L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_MAXREDIRS,        5L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_CONNECTTIMEOUT,   DL_CONNECT_TIMEOUT );
    code |= cl_curl_easy_setopt( easy, CURLOPT_TIMEOUT,          DL_TRANSFER_TIMEOUT );
    code |= cl_curl_easy_setopt( easy, CURLOPT_NOSIGNAL,         1L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_USERAGENT,        Q3_VERSION );
    code |= cl_curl_easy_setopt( easy, CURLOPT_PRIVATE,          req );

    if ( code != CURLE_OK ) {
        Com_Printf( "CL_Downloads: curl_easy_setopt failed for pk3 download\n" );
        return qfalse;
    }

    if ( cl_curl_multi_add_handle( cl_dl.multi, easy ) != CURLM_OK ) {
        return qfalse;
    }

    return qtrue;
}
#endif // USE_CURL

// ---------------------------------------------------------------------------
// Index JSON → temp file
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Preview cache helpers
// ---------------------------------------------------------------------------

#define DL_PREVIEW_DIR          "q3rally_previews"
#define DL_PREVIEW_MAX_SIZE     (2 * 1024 * 1024)   // 2 MB per preview image

/*
 * CL_DL_PreviewLocalPath
 * Builds the VFS-relative path where a preview image is (or will be) stored.
 * e.g. id="tracks/mytrack", ext=".jpg"  →  "q3rally_previews/tracks/mytrack.jpg"
 */
static void CL_DL_PreviewLocalPath( const char *id, const char *ext,
                                    char *out, int outSize ) {
    Com_sprintf( out, outSize, "%s/%s%s", DL_PREVIEW_DIR, id, ext );
}

/*
 * CL_DL_PreviewExists
 * Returns qtrue if the preview file is already present on disk.
 */
static qboolean CL_DL_PreviewExists( const char *id, const char *ext ) {
    char        relPath[MAX_OSPATH];
    char        absPath[MAX_OSPATH];
    char        homePath[MAX_OSPATH];
    char        basegame[MAX_OSPATH];
    struct stat st;

    CL_DL_PreviewLocalPath( id, ext, relPath, sizeof( relPath ) );

    Q_strncpyz( homePath, Cvar_VariableString( "fs_homepath" ), sizeof( homePath ) );
    Cvar_VariableStringBuffer( "fs_basegame", basegame, sizeof( basegame ) );
    if ( !basegame[0] ) Q_strncpyz( basegame, "baseq3r", sizeof( basegame ) );

    Com_sprintf( absPath, sizeof( absPath ), "%s%c%s%c%s",
                 homePath, PATH_SEP, basegame, PATH_SEP, relPath );

    // Replace forward slashes with platform separator
#ifdef _WIN32
    {
        char *p;
        for ( p = absPath; *p; p++ ) if ( *p == '/' ) *p = PATH_SEP;
    }
#endif

    return ( stat( absPath, &st ) == 0 && st.st_size > 0 ) ? qtrue : qfalse;
}

/*
 * CL_DL_ExtFromUrl
 * Extracts the file extension from a URL (e.g. ".jpg", ".png").
 * Returns ".jpg" as default if none found.
 */
static void CL_DL_ExtFromUrl( const char *url, char *ext, int extSize ) {
    const char *lastSlash = strrchr( url, '/' );
    const char *filename  = lastSlash ? lastSlash + 1 : url;
    const char *dot       = strrchr( filename, '.' );

    if ( dot && strlen( dot ) <= 5 ) {
        Q_strncpyz( ext, dot, extSize );
    } else {
        Q_strncpyz( ext, ".jpg", extSize );
    }
}

/*
 * CL_DL_EnqueuePreviews
 * Parses the index JSON and populates the preview queue with entries
 * whose preview images are not yet cached on disk.
 */
static void CL_DL_EnqueuePreviews( const char *jsonBuf ) {
    const char  *p = jsonBuf;
    const char  *objEnd;
    char         id[64];
    char         previewUrl[512];
    char         ext[8];

    cl_dl.previewQueueHead  = 0;
    cl_dl.previewQueueCount = 0;

    while ( *p && cl_dl.previewQueueCount < DL_MAX_PREVIEW_QUEUE ) {
        p = strchr( p, '{' );
        if ( !p ) break;

        objEnd = strchr( p, '}' );
        if ( !objEnd ) break;

        id[0]         = '\0';
        previewUrl[0] = '\0';

        // Reuse the same minimal parser pattern as the QVM side
        {
            // id
            const char *kp;
            char searchKey[128];
            int  i;

            Com_sprintf( searchKey, sizeof( searchKey ), "\"id\"" );
            kp = strstr( p, searchKey );
            if ( kp && kp < objEnd ) {
                kp += strlen( searchKey );
                while ( *kp && ( *kp == ' ' || *kp == '\t' || *kp == ':' ) ) kp++;
                if ( *kp == '"' ) {
                    kp++;
                    for ( i = 0; i < (int)sizeof(id) - 1 && *kp && *kp != '"'; i++ )
                        id[i] = *kp++;
                    id[i] = '\0';
                }
            }

            // preview
            Com_sprintf( searchKey, sizeof( searchKey ), "\"preview\"" );
            kp = strstr( p, searchKey );
            if ( kp && kp < objEnd ) {
                kp += strlen( searchKey );
                while ( *kp && ( *kp == ' ' || *kp == '\t' || *kp == ':' ) ) kp++;
                if ( *kp == '"' ) {
                    kp++;
                    for ( i = 0; i < (int)sizeof(previewUrl) - 1 && *kp && *kp != '"'; i++ )
                        previewUrl[i] = *kp++;
                    previewUrl[i] = '\0';
                }
            }
        }

        if ( id[0] && previewUrl[0] ) {
            CL_DL_ExtFromUrl( previewUrl, ext, sizeof( ext ) );

            if ( !CL_DL_PreviewExists( id, ext ) ) {
                clDlPreviewEntry_t *entry = &cl_dl.previewQueue[cl_dl.previewQueueCount];
                Q_strncpyz( entry->id,  id,         sizeof( entry->id ) );
                Q_strncpyz( entry->url, previewUrl, sizeof( entry->url ) );
                cl_dl.previewQueueCount++;
            }
        }

        p = objEnd + 1;
    }

    Com_Printf( "CL_Downloads: %d preview(s) queued for download\n",
                cl_dl.previewQueueCount );
}

#ifdef USE_CURL
/*
 * CL_DL_StartPreviewFetch
 * Kicks off a curl transfer for the next preview in the queue.
 * The file is written directly to disk (VFS homepath).
 */
static qboolean CL_DL_StartPreviewFetch( clDlRequest_t *req ) {
    CURL        *easy;
    CURLcode     code;
    char         relPath[MAX_OSPATH];
    char         absPath[MAX_OSPATH];
    char         pathCopy[MAX_OSPATH];
    char         ext[8];
    char         homePath[MAX_OSPATH];
    char         basegame[MAX_OSPATH];

    if ( !CL_DL_EnsureCurl() ) return qfalse;

    CL_DL_ExtFromUrl( req->url, ext, sizeof( ext ) );
    CL_DL_PreviewLocalPath( req->id, ext, relPath, sizeof( relPath ) );

    Q_strncpyz( homePath, Cvar_VariableString( "fs_homepath" ), sizeof( homePath ) );
    Cvar_VariableStringBuffer( "fs_basegame", basegame, sizeof( basegame ) );
    if ( !basegame[0] ) Q_strncpyz( basegame, "baseq3r", sizeof( basegame ) );

    Com_sprintf( absPath, sizeof( absPath ), "%s%c%s%c%s",
                 homePath, PATH_SEP, basegame, PATH_SEP, relPath );

#ifdef _WIN32
    {
        char *p;
        for ( p = absPath; *p; p++ ) if ( *p == '/' ) *p = PATH_SEP;
    }
#endif

    Q_strncpyz( req->destPath, absPath, sizeof( req->destPath ) );

    // Ensure subdirectory exists
    Q_strncpyz( pathCopy, absPath, sizeof( pathCopy ) );
    if ( FS_CreatePath( pathCopy ) ) {
        Com_Printf( "CL_Downloads: failed to create preview path '%s'\n", absPath );
        return qfalse;
    }

    req->file = Sys_FOpen( absPath, "wb" );
    if ( !req->file ) {
        Com_Printf( "CL_Downloads: failed to open preview file '%s' (%s)\n",
                    absPath, strerror( errno ) );
        return qfalse;
    }

    easy = cl_curl_easy_init();
    if ( !easy ) {
        fclose( req->file );
        req->file = NULL;
        return qfalse;
    }

    req->easy        = easy;
    req->errorBuf[0] = '\0';

    code  = cl_curl_easy_setopt( easy, CURLOPT_URL,            req->url );
    code |= cl_curl_easy_setopt( easy, CURLOPT_WRITEFUNCTION,  CL_DL_WriteFile );
    code |= cl_curl_easy_setopt( easy, CURLOPT_WRITEDATA,      req );
    code |= cl_curl_easy_setopt( easy, CURLOPT_ERRORBUFFER,    req->errorBuf );
    code |= cl_curl_easy_setopt( easy, CURLOPT_FOLLOWLOCATION, 1L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_MAXREDIRS,      5L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_CONNECTTIMEOUT, DL_CONNECT_TIMEOUT );
    code |= cl_curl_easy_setopt( easy, CURLOPT_TIMEOUT,        30L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_NOSIGNAL,       1L );
    code |= cl_curl_easy_setopt( easy, CURLOPT_USERAGENT,      Q3_VERSION );
    code |= cl_curl_easy_setopt( easy, CURLOPT_PRIVATE,        req );
    // Abort if server tries to send more than the limit
    code |= cl_curl_easy_setopt( easy, CURLOPT_MAXFILESIZE_LARGE,
                                 (curl_off_t)DL_PREVIEW_MAX_SIZE );

    if ( code != CURLE_OK ) {
        Com_Printf( "CL_Downloads: curl_easy_setopt failed for preview fetch\n" );
        fclose( req->file );
        req->file = NULL;
        cl_curl_easy_cleanup( easy );
        req->easy = NULL;
        return qfalse;
    }

    if ( cl_curl_multi_add_handle( cl_dl.multi, easy ) != CURLM_OK ) {
        fclose( req->file );
        req->file = NULL;
        cl_curl_easy_cleanup( easy );
        req->easy = NULL;
        return qfalse;
    }

    return qtrue;
}

/*
 * CL_DL_PumpPreviewQueue
 * Called from CL_DL_Frame when no pk3 transfer is active.
 * Starts the next queued preview fetch if one is pending.
 */
static void CL_DL_PumpPreviewQueue( void ) {
    clDlRequest_t      *req;
    clDlPreviewEntry_t *entry;

    if ( cl_dl.previewActive ) return;
    if ( cl_dl.previewQueueHead >= cl_dl.previewQueueCount ) return;
    // Don't overlap with an active pk3 download
    if ( cl_dl.active ) return;

    entry = &cl_dl.previewQueue[cl_dl.previewQueueHead];

    req = CL_DL_AllocRequest();
    if ( !req ) return;

    req->type = CLREQ_PREVIEW;
    Q_strncpyz( req->id,  entry->id,  sizeof( req->id ) );
    Q_strncpyz( req->url, entry->url, sizeof( req->url ) );

    if ( !CL_DL_StartPreviewFetch( req ) ) {
        CL_DL_FreeRequest( req );
        // Skip broken entry and try next frame
        cl_dl.previewQueueHead++;
        return;
    }

    cl_dl.active        = req;
    cl_dl.previewActive = qtrue;
    cl_dl.previewQueueHead++;

    Com_DPrintf( "CL_Downloads: fetching preview %d/%d – %s\n",
                 cl_dl.previewQueueHead, cl_dl.previewQueueCount, entry->url );
}
#endif // USE_CURL

static void CL_DL_SaveIndexToTemp( clDlRequest_t *req ) {
    // Write via FS_WriteFile so the file lands inside the VFS and
    // trap_FS_FOpenFile in the QVM can find it by relative name.
    FS_WriteFile( DL_INDEX_TMPNAME, req->buf, (int)req->bufLen );

    CL_DL_SetIndexPath( DL_INDEX_TMPNAME );
    CL_DL_SetState( DL_STATE_READY );
    Com_Printf( "CL_Downloads: index fetched (%u bytes) -> %s\n",
                (unsigned)req->bufLen, DL_INDEX_TMPNAME );

    // Queue preview images for background download
    CL_DL_EnqueuePreviews( req->buf );
}

// ---------------------------------------------------------------------------
// Parse "download:<id>" action and find URL in index JSON
// ---------------------------------------------------------------------------

static qboolean CL_DL_ParseDownloadAction( const char *action,
                                            char *idOut, int idSize,
                                            char *urlOut, int urlSize ) {
    const char  *prefix = "download:";
    const char  *id;
    char         indexPath[MAX_OSPATH];
    FILE        *f;
    char        *buf;
    long         size;
    char         needle[128];
    const char  *found;
    const char  *cursor;
    int          i;

    if ( Q_strncmp( action, prefix, strlen( prefix ) ) != 0 ) {
        return qfalse;
    }

    id = action + strlen( prefix );
    Q_strncpyz( idOut, id, idSize );

    // Build absolute path to index file – FS_WriteFile writes to fs_homepath/basegame/
    {
        char homePath[MAX_OSPATH];
        char basegame[MAX_OSPATH];
        Q_strncpyz( homePath, Cvar_VariableString( "fs_homepath" ), sizeof( homePath ) );
        Cvar_VariableStringBuffer( "fs_basegame", basegame, sizeof( basegame ) );
        if ( !basegame[0] ) Q_strncpyz( basegame, "baseq3r", sizeof( basegame ) );
        Com_sprintf( indexPath, sizeof( indexPath ), "%s%c%s%c%s",
                     homePath, PATH_SEP, basegame, PATH_SEP, DL_INDEX_TMPNAME );
    }

    f = Sys_FOpen( indexPath, "rb" );
    if ( !f ) {
        Com_Printf( "CL_Downloads: cannot open index '%s'\n", indexPath );
        return qfalse;
    }

    fseek( f, 0, SEEK_END );
    size = ftell( f );
    fseek( f, 0, SEEK_SET );

    if ( size <= 0 || size > DL_INDEX_MAX_SIZE ) {
        fclose( f );
        return qfalse;
    }

    buf = (char *)malloc( size + 1 );
    if ( !buf ) {
        fclose( f );
        return qfalse;
    }

    if ( (long)fread( buf, 1, size, f ) != size ) {
        fclose( f );
        free( buf );
        return qfalse;
    }
    fclose( f );
    buf[size] = '\0';

    // Search for "id": "<id>" – try both with and without space after colon
    Com_sprintf( needle, sizeof( needle ), "\"id\": \"%s\"", id );
    found = strstr( buf, needle );
    if ( !found ) {
        Com_sprintf( needle, sizeof( needle ), "\"id\":\"%s\"", id );
        found = strstr( buf, needle );
    }
    if ( !found ) {
        Com_Printf( "CL_Downloads: id '%s' not found in index\n", id );
        free( buf );
        return qfalse;
    }

    // From that position, find "url" – handle both "url": "..." and "url":"..."
    {
        const char *urlFound = strstr( found, "\"url\": \"" );
        if ( urlFound ) {
            found = urlFound + strlen( "\"url\": \"" );
        } else {
            urlFound = strstr( found, "\"url\":\"" );
            if ( !urlFound ) {
                free( buf );
                return qfalse;
            }
            found = urlFound + strlen( "\"url\":\"" );
        }
    }
    cursor = found;
    i = 0;
    while ( i < urlSize - 1 && *cursor && *cursor != '"' ) {
        urlOut[i++] = *cursor++;
    }
    urlOut[i] = '\0';

    free( buf );
    return qtrue;
}



// ---------------------------------------------------------------------------
// Action handler (called from CL_DL_Frame)
// ---------------------------------------------------------------------------

static void CL_DL_HandleAction( const char *action ) {
    clDlRequest_t   *req;
    char             id[64];
    char             url[512];

    if ( !action || !action[0] ) return;

    // Cancel any active transfer first
    if ( Q_stricmp( action, "cancel" ) == 0 ) {
        if ( cl_dl.active ) {
            CL_DL_FreeRequest( cl_dl.active );
            cl_dl.active = NULL;
        }
        CL_DL_SetState( DL_STATE_IDLE );
        CL_DL_SetProgress( 0.0f );
        CL_DL_SetFilename( "" );
        Com_Printf( "CL_Downloads: cancelled\n" );
        return;
    }

    // Don't start a new request if one is already active
    if ( cl_dl.active ) {
        Com_Printf( "CL_Downloads: ignoring action '%s' – transfer in progress\n", action );
        return;
    }

#ifndef USE_CURL
    if ( !cl_dl.warnedNoCurl ) {
        Com_Printf( "CL_Downloads: built without libcurl – cannot download content\n" );
        cl_dl.warnedNoCurl = qtrue;
    }
    CL_DL_SetError( "libcurl not available" );
    return;
#else

    // --- fetch_index ---
    if ( Q_stricmp( action, "fetch_index" ) == 0 ) {
        req = CL_DL_AllocRequest();
        if ( !req ) {
            CL_DL_SetError( "Out of memory" );
            return;
        }
        req->type = CLREQ_INDEX;
        CL_DL_SetState( DL_STATE_FETCHING );
        CL_DL_SetFilename( "index.json" );
        CL_DL_SetProgress( 0.0f );

        if ( !CL_DL_StartIndexFetch( req ) ) {
            CL_DL_FreeRequest( req );
            CL_DL_SetError( "Failed to start index fetch" );
            return;
        }

        cl_dl.active = req;
        Com_Printf( "CL_Downloads: fetching index from %s\n", DL_INDEX_URL );
        return;
    }

    // --- download:<id> ---
    if ( Q_strncmp( action, "download:", 9 ) == 0 ) {
        if ( !CL_DL_ParseDownloadAction( action, id, sizeof(id), url, sizeof(url) ) ) {
            CL_DL_SetError( "Content ID not found in index" );
            return;
        }

        req = CL_DL_AllocRequest();
        if ( !req ) {
            CL_DL_SetError( "Out of memory" );
            return;
        }

        req->type = CLREQ_DOWNLOAD;
        Q_strncpyz( req->id,  id,  sizeof( req->id ) );
        Q_strncpyz( req->url, url, sizeof( req->url ) );

        CL_DL_SetState( DL_STATE_DOWNLOADING );
        CL_DL_SetFilename( va( "%s.pk3", id ) );
        CL_DL_SetProgress( 0.0f );

        if ( !CL_DL_StartPk3Download( req ) ) {
            CL_DL_FreeRequest( req );
            CL_DL_SetError( "Failed to start download" );
            return;
        }

        cl_dl.active = req;
        Com_Printf( "CL_Downloads: downloading '%s' from %s\n", id, url );
        return;
    }


    Com_Printf( "CL_Downloads: unknown action '%s'\n", action );
#endif // USE_CURL
}

// ---------------------------------------------------------------------------
// Completion handler
// ---------------------------------------------------------------------------

#ifdef USE_CURL
static void CL_DL_CompleteRequest( clDlRequest_t *req, CURLcode result, long httpCode ) {
    if ( result != CURLE_OK ) {
        const char *errMsg = req->errorBuf[0]
            ? req->errorBuf
            : cl_curl_easy_strerror( result );

        Com_Printf( "CL_Downloads: transfer failed: %s\n", errMsg );

        if ( req->type == CLREQ_PREVIEW ) {
            // Non-fatal – just skip this preview and continue the queue
            Com_DPrintf( "CL_Downloads: preview fetch failed for '%s': %s\n",
                         req->id, errMsg );
            if ( req->destPath[0] ) remove( req->destPath );
            cl_dl.previewActive = qfalse;
        } else {
            CL_DL_SetError( errMsg );
            // Remove partial pk3 file on failure
            if ( req->type == CLREQ_DOWNLOAD && req->destPath[0] ) {
                remove( req->destPath );
            }
        }

        CL_DL_FreeRequest( req );
        cl_dl.active = NULL;
        return;
    }

    if ( httpCode < 200 || httpCode >= 300 ) {
        char errBuf[64];
        Com_sprintf( errBuf, sizeof( errBuf ), "HTTP %ld", httpCode );
        Com_Printf( "CL_Downloads: server returned %s\n", errBuf );

        if ( req->type == CLREQ_PREVIEW ) {
            Com_DPrintf( "CL_Downloads: preview HTTP %ld for '%s', skipping\n",
                         httpCode, req->id );
            if ( req->destPath[0] ) remove( req->destPath );
            cl_dl.previewActive = qfalse;
        } else {
            CL_DL_SetError( errBuf );
            if ( req->type == CLREQ_DOWNLOAD && req->destPath[0] ) {
                remove( req->destPath );
            }
        }

        CL_DL_FreeRequest( req );
        cl_dl.active = NULL;
        return;
    }

    // Success
    if ( req->type == CLREQ_INDEX ) {
        CL_DL_SaveIndexToTemp( req );
        // state set inside SaveIndexToTemp
    } else if ( req->type == CLREQ_PREVIEW ) {
        if ( req->file ) {
            fclose( req->file );
            req->file = NULL;
        }
        Com_DPrintf( "CL_Downloads: preview saved – %s\n", req->destPath );
        cl_dl.previewActive = qfalse;
    } else {
        // CLREQ_DOWNLOAD – close file before reporting done
        if ( req->file ) {
            fclose( req->file );
            req->file = NULL;
        }
        CL_DL_SetProgress( 100.0f );
        Com_Printf( "CL_Downloads: '%s.pk3' saved to %s\n", req->id, req->destPath );
        CL_DL_SetState( DL_STATE_DONE );
    }

    CL_DL_FreeRequest( req );
    cl_dl.active = NULL;
}
#endif // USE_CURL

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

static void CL_DL_CleanupUninstalled( void ) {
    char    homePath[MAX_OSPATH];
    char    basegame[MAX_OSPATH];
    char    dir[MAX_OSPATH];
    char    **files;
    int     count, i;

    Q_strncpyz( homePath, Cvar_VariableString( "fs_homepath" ), sizeof( homePath ) );
    Cvar_VariableStringBuffer( "fs_basegame", basegame, sizeof( basegame ) );
    if ( !basegame[0] ) Q_strncpyz( basegame, "baseq3r", sizeof( basegame ) );
    Com_sprintf( dir, sizeof( dir ), "%s%c%s", homePath, PATH_SEP, basegame );

    files = Sys_ListFiles( dir, ".uninstalled", NULL, &count, qfalse );
    if ( !files ) return;

    for ( i = 0; i < count; i++ ) {
        char path[MAX_OSPATH];
        Com_sprintf( path, sizeof( path ), "%s%c%s", dir, PATH_SEP, files[i] );
        if ( remove( path ) == 0 ) {
            Com_Printf( "CL_Downloads: cleaned up '%s'\n", files[i] );
        }
    }
    Sys_FreeFileList( files );
}

void CL_DL_Init( void ) {
    Com_Memset( &cl_dl, 0, sizeof( cl_dl ) );
    CL_DL_CleanupUninstalled();

    // Register CVars (CVAR_TEMP – not saved to config)
    Cvar_Get( "ui_dl_state",    "0",  CVAR_TEMP );
    Cvar_Get( "ui_dl_progress", "0",  CVAR_TEMP );
    Cvar_Get( "ui_dl_filename", "",   CVAR_TEMP );
    Cvar_Get( "ui_dl_error",    "",   CVAR_TEMP );
    Cvar_Get( "ui_dl_action",   "",   CVAR_TEMP );
    Cvar_Get( "ui_dl_indexpath","",   CVAR_TEMP );

    cl_dl.initialized   = qtrue;
    cl_dl.state         = DL_STATE_IDLE;
    cl_dl.lastActionPoll = 0;

    Com_Printf( "CL_Downloads: initialized\n" );
}

void CL_DL_Shutdown( void ) {
    if ( !cl_dl.initialized ) return;

#ifdef USE_CURL
    if ( cl_dl.active ) {
        CL_DL_FreeRequest( cl_dl.active );
        cl_dl.active = NULL;
    }
    CL_DL_ShutdownCurl();
#endif

    cl_dl.initialized = qfalse;
    Com_Printf( "CL_Downloads: shutdown\n" );
}

void CL_DL_Frame( void ) {
#ifdef USE_CURL
    CURLMsg *msg;
    int      messages;
    int      running;
    int      now;
    char     action[256];
#endif

    if ( !cl_dl.initialized ) return;

#ifdef USE_CURL
    now = Sys_Milliseconds();

    // Poll ui_dl_action CVar
    if ( now - cl_dl.lastActionPoll >= DL_ACTION_POLL_MS ) {
        cl_dl.lastActionPoll = now;
        Cvar_VariableStringBuffer( "ui_dl_action", action, sizeof( action ) );
        if ( action[0] ) {
            // Clear immediately so UI knows we saw it
            Cvar_Set( "ui_dl_action", "" );
            CL_DL_HandleAction( action );
        }
    }

    // Drive active transfer
    if ( !cl_dl.active || !cl_dl.multi ) return;

    cl_curl_multi_perform( cl_dl.multi, &running );

    // Harvest completed transfers
    while ( ( msg = cl_curl_multi_info_read( cl_dl.multi, &messages ) ) != NULL ) {
        if ( msg->msg == CURLMSG_DONE ) {
            clDlRequest_t   *req = NULL;
            long             httpCode = 0;
            CURL            *easy = msg->easy_handle;

            cl_curl_easy_getinfo( easy, CURLINFO_PRIVATE,       (char **)&req );
            cl_curl_easy_getinfo( easy, CURLINFO_RESPONSE_CODE, &httpCode );

            cl_curl_multi_remove_handle( cl_dl.multi, easy );
            cl_curl_easy_cleanup( easy );

            if ( req ) {
                req->easy = NULL;
                if ( req->headers ) {
                    cl_curl_slist_free_all( req->headers );
                    req->headers = NULL;
                }
                if ( cl_dl.active == req ) {
                    cl_dl.active = NULL;
                }
                CL_DL_CompleteRequest( req, msg->data.result, httpCode );
            }
        }
    }

    // Start the next preview image if nothing else is running
    CL_DL_PumpPreviewQueue();
#endif // USE_CURL
}
