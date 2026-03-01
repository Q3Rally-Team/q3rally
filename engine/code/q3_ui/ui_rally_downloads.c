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
/*
=======================================================================

DOWNLOAD MANAGER MENU

Communicates with the native engine via CVars:

  ui_dl_state     (int, read)   - 0=idle, 1=fetching index, 2=ready, 3=downloading, 4=done, 5=error
  ui_dl_progress  (float, read) - 0.0 to 100.0 during active download
  ui_dl_filename  (str,   read) - currently downloading file name
  ui_dl_error     (str,   read) - human-readable error message (state 5)
  ui_dl_action    (str,   write)- commands sent to engine:
                                   "fetch_index"        - start fetching content index
                                   "download:<id>"      - queue a specific item for download
                                   "cancel"             - cancel ongoing operation

The index is stored as a temporary file by the engine and the path is
written to ui_dl_indexpath so the UI can read it via trap_FS_FOpenFile.

=======================================================================
*/

#include "ui_local.h"


// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

#define DL_MAX_ITEMS            64
#define DL_ITEMS_PER_PAGE       8
#define DL_ITEM_HEIGHT          32
#define DL_LIST_X               20
#define DL_LIST_Y               112
#define DL_LIST_WIDTH           468
#define DL_TAB_COUNT            4
#define DL_TAB_WIDTH            ( DL_LIST_WIDTH / DL_TAB_COUNT )
#define DL_TAB_TOP              68
#define DL_TAB_HEIGHT           22
#define DL_TAB_LABEL_Y          ( DL_TAB_TOP + 1 )
#define DL_PROGRESSBAR_Y        410
#define DL_PROGRESSBAR_WIDTH    ( DL_LIST_WIDTH + DL_PREVIEW_WIDTH + 8 )
#define DL_PROGRESSBAR_HEIGHT   14
#define DL_STATUS_Y             425
#define DL_PREVIEW_X            ( DL_LIST_X + DL_LIST_WIDTH + 8 )
#define DL_PREVIEW_Y            DL_LIST_Y
#define DL_PREVIEW_WIDTH        144
#define DL_PREVIEW_HEIGHT       184

// Column X offsets within the list (relative to DL_LIST_X)
#define DL_COL_NAME_X           8
#define DL_COL_AUTHOR_X         232
#define DL_COL_SIZE_X           ( DL_LIST_WIDTH - 136 )
#define DL_COL_STATUS_X         ( DL_LIST_WIDTH - 8 )

// Download states (mirrored from native side via ui_dl_state CVar)
#define DL_STATE_IDLE           0
#define DL_STATE_FETCHING       1
#define DL_STATE_READY          2
#define DL_STATE_DOWNLOADING    3
#define DL_STATE_DONE           4
#define DL_STATE_ERROR          5

// Content types
#define DL_TYPE_ALL             0
#define DL_TYPE_TRACKS          1
#define DL_TYPE_VEHICLES        2
#define DL_TYPE_SKINS           3

// Widget IDs
#define ID_DL_BACK              10
#define ID_DL_TAB_ALL           11
#define ID_DL_TAB_TRACKS        12
#define ID_DL_TAB_VEHICLES      13
#define ID_DL_TAB_SKINS         14
#define ID_DL_SCROLL_UP         15
#define ID_DL_SCROLL_DOWN       16
#define ID_DL_DOWNLOAD          17
#define ID_DL_REFRESH           18
#define ID_DL_ITEM_BASE         100  // items: ID_DL_ITEM_BASE + index


// ---------------------------------------------------------------------------
// Data structures
// ---------------------------------------------------------------------------

typedef struct {
    char    id[64];
    char    name[80];
    char    author[64];
    char    version[16];
    char    type[16];       // "track", "vehicle", "skin"
    char    preview[128];
    int     size_kb;
    qboolean installed;
} dlItem_t;

typedef struct {
    menuframework_s menu;

    menutext_s      banner;
    menutext_s      back;
    menutext_s      tabAll;
    menutext_s      tabTracks;
    menutext_s      tabVehicles;
    menutext_s      tabSkins;
    menutext_s      scrollUp;
    menutext_s      scrollDown;
    menutext_s      downloadBtn;
    menutext_s      refreshBtn;

    // State
    int             activeTab;
    int             scrollOffset;
    int             selectedItem;
    int             dlState;
    float           dlProgress;
    char            dlFilename[MAX_QPATH];
    char            dlError[256];

    // Item list (filtered)
    dlItem_t        items[DL_MAX_ITEMS];
    int             numItems;

    // All items from index
    dlItem_t        allItems[DL_MAX_ITEMS];
    int             numAllItems;

    int             lastStateCheck;
    qboolean        showPopup;
    char            popupText[128];
} downloadsMenu_t;

static downloadsMenu_t s_dl;


// ---------------------------------------------------------------------------
// Minimal JSON parser for the content index
// ---------------------------------------------------------------------------

/*
=================
DL_ParseString
Extracts the string value after "key": "..." from buf starting at pos.
Returns pointer past the parsed value, or NULL on failure.
=================
*/
static const char *DL_ParseString( const char *buf, const char *key, char *out, int outSize ) {
    char    searchKey[128];
    const char *p;
    int     i;

    Com_sprintf( searchKey, sizeof( searchKey ), "\"%s\"", key );
    p = strstr( buf, searchKey );
    if ( !p ) return NULL;

    p += strlen( searchKey );

    // skip whitespace and colon
    while ( *p && ( *p == ' ' || *p == '\t' || *p == ':' || *p == '\n' || *p == '\r' ) ) p++;
    if ( *p != '"' ) return NULL;
    p++; // skip opening quote

    for ( i = 0; i < outSize - 1 && *p && *p != '"'; i++, p++ ) {
        out[i] = *p;
    }
    out[i] = '\0';

    if ( *p == '"' ) p++;
    return p;
}

/*
=================
DL_ParseInt
=================
*/
static int DL_ParseInt( const char *buf, const char *key ) {
    char    searchKey[128];
    const char *p;

    Com_sprintf( searchKey, sizeof( searchKey ), "\"%s\"", key );
    p = strstr( buf, searchKey );
    if ( !p ) return 0;

    p += strlen( searchKey );
    while ( *p && ( *p == ' ' || *p == '\t' || *p == ':' ) ) p++;
    return atoi( p );
}

/*
=================
DL_IsInstalled
Checks whether a .pk3 file for this item already exists in the game path.
=================
*/
static qboolean DL_IsInstalled( const char *itemId ) {
    fileHandle_t    f;
    char            path[MAX_QPATH];
    int             len;

    Com_sprintf( path, sizeof( path ), "%s.pk3", itemId );
    len = trap_FS_FOpenFile( path, &f, FS_READ );
    if ( len >= 0 ) {
        trap_FS_FCloseFile( f );
        return qtrue;
    }
    return qfalse;
}

/*
=================
DL_ParseIndex

Parses a minimal JSON content index from a buffer.
Expected format (array of objects):
[
  { "id": "...", "name": "...", "author": "...", "version": "...",
    "type": "track|vehicle|skin", "size": <bytes> },
  ...
]
=================
*/
static void DL_ParseIndex( const char *buf ) {
    const char  *p = buf;
    const char  *objEnd;
    dlItem_t    *item;

    s_dl.numAllItems = 0;

    // Walk through the array looking for object starts
    while ( *p && s_dl.numAllItems < DL_MAX_ITEMS ) {
        // Find next object opening brace
        p = strchr( p, '{' );
        if ( !p ) break;

        item = &s_dl.allItems[s_dl.numAllItems];
        memset( item, 0, sizeof( *item ) );

        // Find closing brace to limit our parse scope
        objEnd = strchr( p, '}' );
        if ( !objEnd ) break;

        // We parse within this object range only
        // (crude but functional for a flat object structure)
        DL_ParseString( p, "id",      item->id,      sizeof( item->id ) );
        DL_ParseString( p, "name",    item->name,    sizeof( item->name ) );
        DL_ParseString( p, "author",  item->author,  sizeof( item->author ) );
        DL_ParseString( p, "version", item->version, sizeof( item->version ) );
        DL_ParseString( p, "type",    item->type,    sizeof( item->type ) );
        DL_ParseString( p, "preview", item->preview, sizeof( item->preview ) );
        item->size_kb = DL_ParseInt( p, "size" ) / 1024;

        if ( item->id[0] && item->name[0] && item->type[0] ) {
            item->installed = DL_IsInstalled( item->id );
            s_dl.numAllItems++;
        }

        p = objEnd + 1;
    }
}

/*
=================
DL_LoadIndex
Tries to read the cached index file written by the engine.
=================
*/
static void DL_LoadIndex( void ) {
    fileHandle_t    f;
    char            indexPath[MAX_QPATH];
    char            buf[16384];
    int             len;

    trap_Cvar_VariableStringBuffer( "ui_dl_indexpath", indexPath, sizeof( indexPath ) );
    if ( !indexPath[0] ) return;

    len = trap_FS_FOpenFile( indexPath, &f, FS_READ );
    if ( len <= 0 || len > (int)sizeof( buf ) - 1 ) {
        if ( len > 0 ) trap_FS_FCloseFile( f );
        return;
    }

    trap_FS_Read( buf, len, f );
    trap_FS_FCloseFile( f );
    buf[len] = '\0';

    DL_ParseIndex( buf );
}


// ---------------------------------------------------------------------------
// Filtering
// ---------------------------------------------------------------------------

static void DL_ApplyFilter( void ) {
    int     i;
    const char *filterType = NULL;

    switch ( s_dl.activeTab ) {
        case DL_TYPE_TRACKS:   filterType = "track";   break;
        case DL_TYPE_VEHICLES: filterType = "vehicle"; break;
        case DL_TYPE_SKINS:    filterType = "skin";    break;
        default:               filterType = NULL;      break; // ALL
    }

    s_dl.numItems = 0;
    for ( i = 0; i < s_dl.numAllItems; i++ ) {
        if ( !filterType || !Q_stricmp( s_dl.allItems[i].type, filterType ) ) {
            s_dl.items[s_dl.numItems++] = s_dl.allItems[i];
            if ( s_dl.numItems >= DL_MAX_ITEMS ) break;
        }
    }

    s_dl.scrollOffset = 0;
    s_dl.selectedItem = ( s_dl.numItems > 0 ) ? 0 : -1;
}


// ---------------------------------------------------------------------------
// State polling
// ---------------------------------------------------------------------------

static void DL_PollState( void ) {
    int prevState = s_dl.dlState;
    s_dl.dlState    = (int)trap_Cvar_VariableValue( "ui_dl_state" );
    s_dl.dlProgress = trap_Cvar_VariableValue( "ui_dl_progress" );
    trap_Cvar_VariableStringBuffer( "ui_dl_filename", s_dl.dlFilename, sizeof( s_dl.dlFilename ) );
    trap_Cvar_VariableStringBuffer( "ui_dl_error",    s_dl.dlError,    sizeof( s_dl.dlError ) );

    // When the engine transitions to READY, parse the index exactly once
    if ( s_dl.dlState == DL_STATE_READY && prevState != DL_STATE_READY ) {
        DL_LoadIndex();
        DL_ApplyFilter();
    }
    // When a download finishes, reload index and show restart popup
    if ( s_dl.dlState == DL_STATE_DONE && prevState == DL_STATE_DOWNLOADING ) {
        DL_LoadIndex();
        DL_ApplyFilter();
        s_dl.showPopup = qtrue;
        Q_strncpyz( s_dl.popupText, "Restart required to use new content.", sizeof( s_dl.popupText ) );

    }
    if ( s_dl.dlState == DL_STATE_IDLE && prevState != DL_STATE_IDLE &&
         prevState != DL_STATE_FETCHING ) {
        DL_LoadIndex();
        DL_ApplyFilter();
    }
}


// ---------------------------------------------------------------------------
// Draw helpers
// ---------------------------------------------------------------------------

static void DL_DrawProgressBar( float progress ) {
    float   filledW;
    int     x = DL_LIST_X;
    int     y = DL_PROGRESSBAR_Y;
    int     w = DL_PROGRESSBAR_WIDTH;
    int     h = DL_PROGRESSBAR_HEIGHT;
    vec4_t  bgColor   = { 0.1f, 0.1f, 0.1f, 0.8f };
    vec4_t  fillColor = { 0.8f, 0.5f, 0.0f, 1.0f };  // orange, Q3-style
    vec4_t  borderCol = { 0.5f, 0.5f, 0.5f, 1.0f };

    UI_FillRect( x, y, w, h, bgColor );
    filledW = ( progress / 100.0f ) * (float)w;
    if ( filledW > 0 ) {
        UI_FillRect( x, y, (int)filledW, h, fillColor );
    }
    UI_DrawRect( x, y, w, h, borderCol );
}

static void DL_DrawItemRow( int index, int y, qboolean selected ) {
    dlItem_t    *item;
    vec4_t      rowBg       = { 0.1f, 0.1f, 0.15f, 0.6f };
    vec4_t      rowSelected = { 0.3f, 0.2f, 0.0f,  0.8f };
    vec4_t      *bg;
    vec4_t      nameColor   = { 1.0f, 1.0f, 1.0f, 1.0f };
    vec4_t      infoColor   = { 0.7f, 0.7f, 0.7f, 1.0f };
    vec4_t      installedColor = { 0.3f, 0.8f, 0.3f, 1.0f };
    char        sizeStr[32];
    int         rowH        = DL_ITEM_HEIGHT;

    if ( index < 0 || index >= s_dl.numItems ) return;
    item = &s_dl.items[index];
    bg = selected ? &rowSelected : &rowBg;

    UI_FillRect( DL_LIST_X, y, DL_LIST_WIDTH, rowH - 2, *bg );

    // Name (left, normal font)
    UI_DrawString( DL_LIST_X + DL_COL_NAME_X, y + 8, item->name,
                   UI_LEFT | UI_SMALLFONT, nameColor );

    // Author (center area)
    UI_DrawString( DL_LIST_X + DL_COL_AUTHOR_X, y + 8, item->author,
                   UI_LEFT | UI_SMALLFONT, infoColor );

    // Size
    if ( item->size_kb > 1024 ) {
        Com_sprintf( sizeStr, sizeof( sizeStr ), "%.1f MB", item->size_kb / 1024.0f );
    } else {
        Com_sprintf( sizeStr, sizeof( sizeStr ), "%d KB", item->size_kb );
    }
    UI_DrawString( DL_LIST_X + DL_COL_SIZE_X, y + 8, sizeStr,
                   UI_LEFT | UI_SMALLFONT, infoColor );

    // Status column
    if ( item->installed ) {
        UI_DrawString( DL_LIST_X + DL_COL_STATUS_X, y + 8, "INSTALLED",
                       UI_RIGHT | UI_SMALLFONT, installedColor );
    } else {
        UI_DrawString( DL_LIST_X + DL_COL_STATUS_X, y + 8, "available",
                       UI_RIGHT | UI_SMALLFONT, infoColor );
    }
}

static qhandle_t DL_GetPreviewShader( const dlItem_t *item ) {
    qhandle_t    shader;
    char         shaderPath[MAX_QPATH];
    char         ext[8];
    const char  *dot;
    const char  *lastSlash;
    const char  *filename;

    if ( !item ) return 0;

    // 1. Check the preview cache: q3rally_previews/<id>.<ext>
    //    The engine stores images there after fetching them.
    if ( item->preview[0] ) {
        // Derive extension from the URL
        lastSlash = strrchr( item->preview, '/' );
        filename  = lastSlash ? lastSlash + 1 : item->preview;
        dot       = strrchr( filename, '.' );
        if ( dot && strlen( dot ) <= 5 ) {
            Q_strncpyz( ext, dot, sizeof( ext ) );
        } else {
            Q_strncpyz( ext, ".jpg", sizeof( ext ) );
        }

        // Strip extension for RegisterShaderNoMip (engine appends its own)
        Com_sprintf( shaderPath, sizeof( shaderPath ),
                     "q3rally_previews/%s", item->id );
        // shaderPath now ends without extension – correct for Q3 shader lookup
        shader = trap_R_RegisterShaderNoMip( shaderPath );
        if ( shader ) return shader;

        // Also try with the explicit extension path stripped
        Com_sprintf( shaderPath, sizeof( shaderPath ),
                     "q3rally_previews/%s%s", item->id, ext );
        COM_StripExtension( shaderPath, shaderPath, sizeof( shaderPath ) );
        shader = trap_R_RegisterShaderNoMip( shaderPath );
        if ( shader ) return shader;
    }

    // 2. Fallback: levelshots/<id> (already packed in a pk3)
    Com_sprintf( shaderPath, sizeof( shaderPath ), "levelshots/%s", item->id );
    shader = trap_R_RegisterShaderNoMip( shaderPath );
    if ( shader ) return shader;

    // 3. Fallback: levelshots/<basename> (strip any path prefix from id)
    {
        const char *namePart = strrchr( item->id, '/' );
        if ( namePart ) {
            Com_sprintf( shaderPath, sizeof( shaderPath ),
                         "levelshots/%s", namePart + 1 );
            shader = trap_R_RegisterShaderNoMip( shaderPath );
            if ( shader ) return shader;
        }
    }

    return 0;
}

static void DL_DrawPreviewPane( void ) {
    qhandle_t    previewShader = 0;
    vec4_t       paneBg        = { 0.08f, 0.08f, 0.10f, 0.82f };
    vec4_t       paneBorder    = { 0.5f, 0.5f, 0.5f, 0.8f };
    vec4_t       textColor     = { 0.85f, 0.85f, 0.85f, 1.0f };
    vec4_t       hintColor     = { 0.55f, 0.55f, 0.55f, 1.0f };
    int          imageX        = DL_PREVIEW_X + 10;
    int          imageY        = DL_PREVIEW_Y + 10;
    int          imageW        = DL_PREVIEW_WIDTH - 20;
    int          imageH        = 94;

    UI_FillRect( DL_PREVIEW_X, DL_PREVIEW_Y, DL_PREVIEW_WIDTH, DL_PREVIEW_HEIGHT, paneBg );
    UI_DrawRect( DL_PREVIEW_X, DL_PREVIEW_Y, DL_PREVIEW_WIDTH, DL_PREVIEW_HEIGHT, paneBorder );

    UI_DrawString( DL_PREVIEW_X + 8, DL_PREVIEW_Y - 20, "PREVIEW",
                   UI_LEFT | UI_SMALLFONT, hintColor );

    if ( s_dl.selectedItem >= 0 && s_dl.selectedItem < s_dl.numItems ) {
        dlItem_t *item = &s_dl.items[s_dl.selectedItem];
        previewShader = DL_GetPreviewShader( item );

        if ( previewShader ) {
            UI_DrawHandlePic( imageX, imageY, imageW, imageH, previewShader );
        } else {
            UI_FillRect( imageX, imageY, imageW, imageH, colorBlack );
            UI_DrawString( DL_PREVIEW_X + 26, DL_PREVIEW_Y + 50, "NO PREVIEW",
                           UI_LEFT | UI_SMALLFONT, hintColor );
        }

        UI_DrawString( DL_PREVIEW_X + 8, DL_PREVIEW_Y + 116, item->name,
                       UI_LEFT | UI_SMALLFONT, textColor );
        UI_DrawString( DL_PREVIEW_X + 8, DL_PREVIEW_Y + 132, item->author,
                       UI_LEFT | UI_SMALLFONT, hintColor );
        UI_DrawString( DL_PREVIEW_X + 8, DL_PREVIEW_Y + 148, item->type,
                       UI_LEFT | UI_SMALLFONT, hintColor );
        UI_DrawString( DL_PREVIEW_X + DL_PREVIEW_WIDTH - 8, DL_PREVIEW_Y + 148, item->version,
                       UI_RIGHT | UI_SMALLFONT, hintColor );
    } else {
        UI_FillRect( imageX, imageY, imageW, imageH, colorBlack );
        UI_DrawString( DL_PREVIEW_X + 18, DL_PREVIEW_Y + 50, "SELECT AN ITEM",
                       UI_LEFT | UI_SMALLFONT, hintColor );
    }
}

static void DL_DrawStatusLine( void ) {
    vec4_t  normalColor  = { 0.7f, 0.7f, 0.7f, 1.0f };
    vec4_t  activeColor  = { 1.0f, 0.8f, 0.2f, 1.0f };
    vec4_t  doneColor    = { 0.3f, 0.9f, 0.3f, 1.0f };
    vec4_t  errorColor   = { 0.9f, 0.2f, 0.2f, 1.0f };
    char    statusText[256];

    switch ( s_dl.dlState ) {
    case DL_STATE_IDLE:
        Com_sprintf( statusText, sizeof( statusText ),
                     "%d item(s) in list. Select and press DOWNLOAD.", s_dl.numItems );
        UI_DrawProportionalString( 320, DL_STATUS_Y, statusText,
                                   UI_CENTER | UI_SMALLFONT, normalColor );
        break;

    case DL_STATE_DONE:
        UI_DrawProportionalString( 320, DL_STATUS_Y, "Download complete. Restart the game to use new content.",
                                   UI_CENTER | UI_SMALLFONT, doneColor );
        break;

    case DL_STATE_FETCHING:
        UI_DrawProportionalString( 320, DL_STATUS_Y, "Fetching content index...",
                                   UI_CENTER | UI_SMALLFONT, activeColor );
        DL_DrawProgressBar( s_dl.dlProgress );
        break;

    case DL_STATE_READY:
        Com_sprintf( statusText, sizeof( statusText ),
                     "%d item(s) available. Select and press DOWNLOAD.", s_dl.numItems );
        UI_DrawProportionalString( 320, DL_STATUS_Y, statusText,
                                   UI_CENTER | UI_SMALLFONT, normalColor );
        break;

    case DL_STATE_DOWNLOADING:
        DL_DrawProgressBar( s_dl.dlProgress );
        if ( s_dl.dlFilename[0] ) {
            Com_sprintf( statusText, sizeof( statusText ),
                         "Downloading: %s  (%.0f%%)", s_dl.dlFilename, s_dl.dlProgress );
        } else {
            Com_sprintf( statusText, sizeof( statusText ),
                         "Downloading...  (%.0f%%)", s_dl.dlProgress );
        }
        UI_DrawProportionalString( 320, DL_STATUS_Y + 20, statusText,
                                   UI_CENTER | UI_SMALLFONT, activeColor );
        break;


    case DL_STATE_ERROR:
        if ( s_dl.dlError[0] ) {
            Com_sprintf( statusText, sizeof( statusText ), "Error: %s", s_dl.dlError );
        } else {
            Q_strncpyz( statusText, "An error occurred. Please try again.", sizeof( statusText ) );
        }
        UI_DrawProportionalString( 320, DL_STATUS_Y, statusText,
                                   UI_CENTER | UI_SMALLFONT, errorColor );
        break;
    }
}


// ---------------------------------------------------------------------------
// Menu callbacks
// ---------------------------------------------------------------------------

static void DownloadsMenu_Event( void *ptr, int event ) {
    int     id;
    char    cmd[128];
    int     itemIndex;

    if ( event != QM_ACTIVATED ) return;

    id = ( (menucommon_s *)ptr )->id;

    if ( id >= ID_DL_ITEM_BASE ) {
        // Item row selected
        itemIndex = id - ID_DL_ITEM_BASE + s_dl.scrollOffset;
        if ( itemIndex >= 0 && itemIndex < s_dl.numItems ) {
            s_dl.selectedItem = itemIndex;
        }
        return;
    }

    switch ( id ) {
    case ID_DL_BACK:
        UI_PopMenu();
        break;

    case ID_DL_TAB_ALL:
        s_dl.activeTab = DL_TYPE_ALL;
        DL_ApplyFilter();
        break;
    case ID_DL_TAB_TRACKS:
        s_dl.activeTab = DL_TYPE_TRACKS;
        DL_ApplyFilter();
        break;
    case ID_DL_TAB_VEHICLES:
        s_dl.activeTab = DL_TYPE_VEHICLES;
        DL_ApplyFilter();
        break;
    case ID_DL_TAB_SKINS:
        s_dl.activeTab = DL_TYPE_SKINS;
        DL_ApplyFilter();
        break;

    case ID_DL_SCROLL_UP:
        if ( s_dl.scrollOffset > 0 ) {
            s_dl.scrollOffset--;
        }
        break;

    case ID_DL_SCROLL_DOWN:
        if ( s_dl.scrollOffset + DL_ITEMS_PER_PAGE < s_dl.numItems ) {
            s_dl.scrollOffset++;
        }
        break;

    case ID_DL_REFRESH:
        trap_Cvar_Set( "ui_dl_action", "fetch_index" );
        break;

    case ID_DL_DOWNLOAD:
        if ( s_dl.selectedItem >= 0 && s_dl.selectedItem < s_dl.numItems ) {
            if ( s_dl.dlState != DL_STATE_DOWNLOADING &&
                 s_dl.dlState != DL_STATE_FETCHING ) {
                if ( !s_dl.items[s_dl.selectedItem].installed ) {
                    Com_sprintf( cmd, sizeof( cmd ), "download:%s",
                                 s_dl.items[s_dl.selectedItem].id );
                    trap_Cvar_Set( "ui_dl_action", cmd );
                }
            }
        }
        break;
    }
}

/*
=================
DownloadsMenu_Draw
=================
*/
static void DownloadsMenu_Draw( void ) {
    int     i;
    int     visibleItems;
    int     itemY;
    vec4_t  headerColor    = { 1.0f, 0.8f, 0.0f, 1.0f };
    vec4_t  dividerColor   = { 0.4f, 0.4f, 0.4f, 0.5f };
    vec4_t  colHeaderColor = { 0.6f, 0.6f, 0.6f, 1.0f };
    vec4_t  popBg          = { 0.0f, 0.0f, 0.0f, 0.85f };
    vec4_t  popBdr         = { 0.8f, 0.5f, 0.0f, 1.0f  };
    vec4_t  popTxt         = { 1.0f, 1.0f, 1.0f, 1.0f  };
    vec4_t  popOk          = { 0.3f, 0.9f, 0.3f, 1.0f  };
    int     now = trap_Milliseconds();

    // Poll state every ~100ms
    if ( now - s_dl.lastStateCheck > 100 ) {
        DL_PollState();
        s_dl.lastStateCheck = now;
    }

    // Banner
    UI_DrawProportionalString( 320, 16, "CONTENT MANAGER",
                               UI_CENTER | UI_DROPSHADOW, headerColor );

    // Divider below title
    UI_FillRect( DL_LIST_X, 64, DL_LIST_WIDTH, 2, dividerColor );

    // Tab highlight (background only – text drawn by menu framework)
    {
        vec4_t activeBg   = { 0.2f, 0.15f, 0.0f, 0.85f };
        vec4_t inactiveBg = { 0.1f, 0.1f,  0.1f, 0.6f  };
        int t;

        for ( t = 0; t < DL_TAB_COUNT; t++ ) {
            vec4_t *bg = ( s_dl.activeTab == t ) ? &activeBg : &inactiveBg;
            UI_FillRect( DL_LIST_X + t * DL_TAB_WIDTH, DL_TAB_TOP,
                         DL_TAB_WIDTH - 2, DL_TAB_HEIGHT, *bg );
        }
    }

    DL_DrawPreviewPane();

    // Column headers
    if ( s_dl.dlState == DL_STATE_READY ||
         s_dl.dlState == DL_STATE_IDLE  ||
         s_dl.dlState == DL_STATE_DONE  ||
         s_dl.dlState == DL_STATE_ERROR ) {

        UI_DrawString( DL_LIST_X + DL_COL_NAME_X,                  92, "NAME",
                       UI_LEFT | UI_SMALLFONT, colHeaderColor );
        UI_DrawString( DL_LIST_X + DL_COL_AUTHOR_X,                92, "AUTHOR",
                       UI_LEFT | UI_SMALLFONT, colHeaderColor );
        UI_DrawString( DL_LIST_X + DL_COL_SIZE_X,                  92, "SIZE",
                       UI_LEFT | UI_SMALLFONT, colHeaderColor );
        UI_DrawString( DL_LIST_X + DL_COL_STATUS_X,                92, "STATUS",
                       UI_RIGHT | UI_SMALLFONT, colHeaderColor );
    }

    // Item list
    visibleItems = DL_ITEMS_PER_PAGE;
    if ( visibleItems > s_dl.numItems - s_dl.scrollOffset ) {
        visibleItems = s_dl.numItems - s_dl.scrollOffset;
    }

    for ( i = 0; i < visibleItems; i++ ) {
        int         listIndex;
        qboolean    selected;
        listIndex = s_dl.scrollOffset + i;
        selected  = ( listIndex == s_dl.selectedItem );
        itemY = DL_LIST_Y + i * DL_ITEM_HEIGHT;
        DL_DrawItemRow( listIndex, itemY, selected );
    }

    // Empty-state messages
    {
        vec4_t hintColor   = { 0.5f, 0.5f, 0.5f, 1.0f };
        vec4_t scrollColor = { 0.8f, 0.8f, 0.8f, 1.0f };

        if ( 0 && s_dl.numAllItems == 0 &&
             s_dl.dlState != DL_STATE_FETCHING &&
             s_dl.dlState != DL_STATE_DOWNLOADING ) {
            UI_DrawProportionalString( 320, DL_LIST_Y + 60,
                                       "Press REFRESH to load available content.",
                                       UI_CENTER | UI_SMALLFONT, hintColor );
        }

        // Scroll indicators
        if ( s_dl.scrollOffset > 0 ) {
            UI_DrawString( DL_LIST_X + DL_LIST_WIDTH + 8,
                           DL_LIST_Y,
                           "^", UI_LEFT | UI_SMALLFONT, scrollColor );
        }
        if ( s_dl.scrollOffset + DL_ITEMS_PER_PAGE < s_dl.numItems ) {
            UI_DrawString( DL_LIST_X + DL_LIST_WIDTH + 8,
                           DL_LIST_Y + ( DL_ITEMS_PER_PAGE - 1 ) * DL_ITEM_HEIGHT,
                           "v", UI_LEFT | UI_SMALLFONT, scrollColor );
        }
    }

    // Divider above controls
    UI_FillRect( DL_LIST_X, DL_PROGRESSBAR_Y - 8, DL_LIST_WIDTH, 2, dividerColor );

    // Status line + progress bar
    DL_DrawStatusLine();

    // Inline info popup
    if ( s_dl.showPopup ) {
        UI_FillRect( 120, 190, 400, 80, popBg );
        UI_DrawRect( 120, 190, 400, 80, popBdr );
        UI_DrawProportionalString( 320, 204, s_dl.popupText,
                                   UI_CENTER | UI_SMALLFONT, popTxt );
        UI_DrawProportionalString( 320, 236, "[ OK ]",
                                   UI_CENTER | UI_SMALLFONT, popOk );
    }

    // Draw menu items (BACK, DOWNLOAD, REFRESH buttons)
    Menu_Draw( &s_dl.menu );
}

/*
=================
DownloadsMenu_Key
=================
*/
static sfxHandle_t DownloadsMenu_Key( int key ) {
    char    cmd[128];
    int     row;
    // Dismiss popup on any key
    if ( s_dl.showPopup ) {
        s_dl.showPopup = qfalse;
        return 0;
    }
    if ( key & K_CHAR_FLAG ) return 0;

    if ( key == K_MOUSE1 ) {
        if ( uis.cursorx >= DL_LIST_X &&
             uis.cursorx <= DL_LIST_X + DL_LIST_WIDTH &&
             uis.cursory >= DL_LIST_Y &&
             uis.cursory < DL_LIST_Y + DL_ITEMS_PER_PAGE * DL_ITEM_HEIGHT ) {
            row = ( uis.cursory - DL_LIST_Y ) / DL_ITEM_HEIGHT;
            row += s_dl.scrollOffset;
            if ( row >= 0 && row < s_dl.numItems ) {
                s_dl.selectedItem = row;
                return menu_move_sound;
            }
        }
    }

    switch ( key ) {
    case K_ESCAPE:
        UI_PopMenu();
        return menu_out_sound;

    case K_UPARROW:
    case K_KP_UPARROW:
        if ( s_dl.selectedItem > 0 ) {
            s_dl.selectedItem--;
            if ( s_dl.selectedItem < s_dl.scrollOffset ) {
                s_dl.scrollOffset = s_dl.selectedItem;
            }
        }
        return menu_move_sound;

    case K_DOWNARROW:
    case K_KP_DOWNARROW:
        if ( s_dl.selectedItem < s_dl.numItems - 1 ) {
            s_dl.selectedItem++;
            if ( s_dl.selectedItem >= s_dl.scrollOffset + DL_ITEMS_PER_PAGE ) {
                s_dl.scrollOffset = s_dl.selectedItem - DL_ITEMS_PER_PAGE + 1;
            }
        }
        return menu_move_sound;

    case K_MOUSE4:
    case K_MWHEELUP:
        if ( s_dl.scrollOffset > 0 ) s_dl.scrollOffset--;
        return menu_move_sound;

    case K_MOUSE5:
    case K_MWHEELDOWN:
        if ( s_dl.scrollOffset + DL_ITEMS_PER_PAGE < s_dl.numItems ) s_dl.scrollOffset++;
        return menu_move_sound;

    case K_ENTER:
    case K_KP_ENTER:
        // Trigger download for selected item
        if ( s_dl.selectedItem >= 0 && s_dl.selectedItem < s_dl.numItems ) {
            /* cmd declared at function top */
            if ( s_dl.dlState != DL_STATE_DOWNLOADING &&
                 s_dl.dlState != DL_STATE_FETCHING ) {
                if ( !s_dl.items[s_dl.selectedItem].installed ) {
                    Com_sprintf( cmd, sizeof( cmd ), "download:%s",
                                 s_dl.items[s_dl.selectedItem].id );
                    trap_Cvar_Set( "ui_dl_action", cmd );
                }
            }
        }
        return menu_in_sound;
    }

    return Menu_DefaultKey( &s_dl.menu, key );
}


// ---------------------------------------------------------------------------
// Menu init helpers
// ---------------------------------------------------------------------------

static void InitDLTabButton( menutext_s *item, int id, char *label, int x, int y ) {
    item->generic.type     = MTYPE_PTEXT;
    item->generic.flags    = QMF_PULSEIFFOCUS;
    item->generic.id       = id;
    item->generic.callback = DownloadsMenu_Event;
    item->generic.x        = x;
    item->generic.y        = y;
    item->style            = UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW;
    item->string           = label;
    item->color            = text_color_normal;
}

static void InitDLButton( menutext_s *item, int id, char *label, int x, int y ) {
    item->generic.type     = MTYPE_PTEXT;
    item->generic.flags    = QMF_RIGHT_JUSTIFY | QMF_PULSEIFFOCUS;
    item->generic.id       = id;
    item->generic.callback = DownloadsMenu_Event;
    item->generic.x        = x;
    item->generic.y        = y;
    item->style            = UI_RIGHT | UI_SMALLFONT | UI_DROPSHADOW;
    item->string           = label;
    item->color            = text_color_normal;
}


static int DL_TabCenterX( int tabIndex ) {
    return DL_LIST_X + tabIndex * DL_TAB_WIDTH + ( DL_TAB_WIDTH / 2 );
}

// ---------------------------------------------------------------------------
// Public entry point
// ---------------------------------------------------------------------------

/*
=================
UI_Rally_DownloadsMenu
=================
*/
void UI_Rally_DownloadsMenu( void ) {
    int btnY = 460;

    memset( &s_dl, 0, sizeof( s_dl ) );

    s_dl.menu.draw       = DownloadsMenu_Draw;
    s_dl.menu.key        = DownloadsMenu_Key;
    s_dl.menu.fullscreen = qtrue;

    s_dl.activeTab   = DL_TYPE_ALL;
    s_dl.selectedItem = -1;

    // Poll initial state
    DL_PollState();

    // If we already have a cached index, load it immediately
    if ( s_dl.dlState == DL_STATE_READY && s_dl.numAllItems == 0 ) {
        DL_LoadIndex();
        DL_ApplyFilter();
    }

    // Auto-fetch index when opening the menu fresh
    if ( s_dl.dlState == DL_STATE_IDLE ) {
        trap_Cvar_Set( "ui_dl_action", "fetch_index" );
    }

    // Tab buttons
    {
        InitDLTabButton( &s_dl.tabAll,      ID_DL_TAB_ALL,      "ALL",      DL_TabCenterX( 0 ), DL_TAB_LABEL_Y );
        InitDLTabButton( &s_dl.tabTracks,   ID_DL_TAB_TRACKS,   "TRACKS",   DL_TabCenterX( 1 ), DL_TAB_LABEL_Y );
        InitDLTabButton( &s_dl.tabVehicles, ID_DL_TAB_VEHICLES, "VEHICLES", DL_TabCenterX( 2 ), DL_TAB_LABEL_Y );
        InitDLTabButton( &s_dl.tabSkins,    ID_DL_TAB_SKINS,    "SKINS",    DL_TabCenterX( 3 ), DL_TAB_LABEL_Y );
    }

    Menu_AddItem( &s_dl.menu, &s_dl.tabAll );
    Menu_AddItem( &s_dl.menu, &s_dl.tabTracks );
    Menu_AddItem( &s_dl.menu, &s_dl.tabVehicles );
    Menu_AddItem( &s_dl.menu, &s_dl.tabSkins );

    // Bottom-row buttons
    InitDLButton( &s_dl.back,         ID_DL_BACK,        "BACK",        80,   btnY );
    InitDLButton( &s_dl.downloadBtn,  ID_DL_DOWNLOAD,    "DOWNLOAD",    356,  btnY );
    InitDLButton( &s_dl.refreshBtn,   ID_DL_REFRESH,     "REFRESH",     620,  btnY );
    Menu_AddItem( &s_dl.menu, &s_dl.back );
    Menu_AddItem( &s_dl.menu, &s_dl.downloadBtn );
    Menu_AddItem( &s_dl.menu, &s_dl.refreshBtn );


    trap_Key_SetCatcher( KEYCATCH_UI );

    UI_PushMenu( &s_dl.menu );
}
