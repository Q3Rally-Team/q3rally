#include "ui_local.h"

#define MAX_PROFILE_FILES   64
#define PROFILE_STATUS_LINES 1

#define ID_PROFILE_LIST      200
#define ID_PROFILE_CREATE    201
#define ID_PROFILE_DELETE    202
#define ID_PROFILE_SELECT    203
#define ID_PROFILE_NAME      204

#define PROFILE_OVERLAY_SCREEN_WIDTH   640
#define PROFILE_OVERLAY_SCREEN_HEIGHT  480
#define PROFILE_OVERLAY_TOP_MARGIN     10
#define PROFILE_OVERLAY_BOTTOM_MARGIN  10
#define PROFILE_OVERLAY_AVAILABLE_HEIGHT \
    (PROFILE_OVERLAY_SCREEN_HEIGHT - PROFILE_OVERLAY_TOP_MARGIN - PROFILE_OVERLAY_BOTTOM_MARGIN)

#define PROFILE_OVERLAY_TITLE_OFFSET            0
#define PROFILE_OVERLAY_LIST_OFFSET             82
#define PROFILE_OVERLAY_NAMEFIELD_OFFSET       218
#define PROFILE_OVERLAY_BUTTON_ROW_OFFSET      266
#define PROFILE_OVERLAY_STATUS_OFFSET          330
#define PROFILE_OVERLAY_GUIDE_PRIMARY_OFFSET   354
#define PROFILE_OVERLAY_GUIDE_SECONDARY_OFFSET 374
#define PROFILE_OVERLAY_GUIDE_EMPTY_OFFSET     360
#define PROFILE_OVERLAY_GUIDE_HINT_OFFSET      410
#define PROFILE_OVERLAY_CONTENT_SPAN           PROFILE_OVERLAY_GUIDE_HINT_OFFSET

static vec4_t overlayBackgroundColor = { 0.2f, 0.2f, 0.2f, 0.8f };
static vec4_t statusNormalColor = { 1.0f, 1.0f, 1.0f, 1.0f };
static vec4_t statusErrorColor  = { 1.0f, 0.3f, 0.3f, 1.0f };
static vec4_t statusInfoColor   = { 1.0f, 0.8f, 0.3f, 1.0f };

static const char *emptyProfileList[] = { "No profiles", NULL };

typedef struct {
    menuframework_s menu;
    menutext_s      title;
    menulist_s      list;
    menufield_s     nameField;
    menutext_s      createButton;
    menutext_s      deleteButton;
    menutext_s      selectButton;
    char            profileNames[MAX_PROFILE_FILES][PROFILE_MAX_NAME];
    const char     *listItems[MAX_PROFILE_FILES + 1];
    int             profileCount;
    int             contentBaseY;

    char            statusLine[128];
    vec4_t          statusColor;
    qboolean        forcingSelection;
} profileOverlay_t;

static profileOverlay_t s_profileOverlay;
//static qboolean s_profileOverlaySessionInitialized = qfalse;

static void UI_ProfileOverlay_Draw( void );
static sfxHandle_t UI_ProfileOverlay_Key( int key );
static void UI_ProfileOverlay_FocusNameField( void );
static void UI_ProfileOverlay_DrawNameField( void *self );
static void UI_ProfileOverlay_EnsureSelectionVisible( void );
static qboolean UI_Profile_WriteFile( const char *name, const profile_info_t *info, const profile_stats_t *stats );
static void UI_Profile_ParseString( const char *buffer, const char *key, char *out, int outSize, const char *defaultValue );
static qboolean UI_Profile_FavoritesEmpty( const profile_info_t *info ) {
    int i;

    if ( !info ) {
        return qtrue;
    }

    for ( i = 0; i < PROFILE_MAX_FAVORITE_CARS; ++i ) {
        if ( info->favoriteCars[i].model[0] && info->favoriteCars[i].skin[0] ) {
            return qfalse;
        }
    }

    return qtrue;
}

static qboolean UI_Profile_ParseFavoriteCvar( int index, profile_favorite_car_t *outFavorite ) {
    char cvarName[16];
    char favoriteValue[MAX_CVAR_VALUE_STRING];
    char *cursor;
    char *slash;

    if ( !outFavorite ) {
        return qfalse;
    }

    Com_Memset( outFavorite, 0, sizeof( *outFavorite ) );

    Com_sprintf( cvarName, sizeof( cvarName ), "favoritecar%d", index + 1 );
    trap_Cvar_VariableStringBuffer( cvarName, favoriteValue, sizeof( favoriteValue ) );

    if ( !favoriteValue[0] ) {
        return qfalse;
    }

    cursor = favoriteValue;

    slash = strchr( cursor, '/' );
    if ( !slash ) {
        return qfalse;
    }
    *slash = '\0';
    Q_strncpyz( outFavorite->model, cursor, sizeof( outFavorite->model ) );
    cursor = slash + 1;

    slash = strchr( cursor, '/' );
    if ( !slash ) {
        return qfalse;
    }
    *slash = '\0';
    Q_strncpyz( outFavorite->skin, cursor, sizeof( outFavorite->skin ) );
    cursor = slash + 1;

    slash = strchr( cursor, '/' );
    if ( slash ) {
        *slash = '\0';
        Q_strncpyz( outFavorite->rim, cursor, sizeof( outFavorite->rim ) );
        Q_strncpyz( outFavorite->head, slash + 1, sizeof( outFavorite->head ) );
    } else {
        Q_strncpyz( outFavorite->rim, cursor, sizeof( outFavorite->rim ) );
    }

    return (qboolean)( outFavorite->model[0] && outFavorite->skin[0] );
}

static qboolean UI_Profile_MigrateFavoriteCvars( const char *profileName, profile_info_t *info, const profile_stats_t *stats ) {
    int i;
    qboolean cvarFound = qfalse;
    qboolean migrated = qfalse;

    if ( !profileName || !profileName[0] || !info || UI_Profile_FavoritesEmpty( info ) == qfalse ) {
        return qfalse;
    }

    for ( i = 0; i < PROFILE_MAX_FAVORITE_CARS; ++i ) {
        char cvarName[16];
        char favoriteValue[MAX_CVAR_VALUE_STRING];

        Com_sprintf( cvarName, sizeof( cvarName ), "favoritecar%d", i + 1 );
        trap_Cvar_VariableStringBuffer( cvarName, favoriteValue, sizeof( favoriteValue ) );

        if ( favoriteValue[0] ) {
            cvarFound = qtrue;
            break;
        }
    }

    if ( !cvarFound ) {
        return qfalse;
    }

    for ( i = 0; i < PROFILE_MAX_FAVORITE_CARS; ++i ) {
        profile_favorite_car_t migratedFavorite;

        if ( UI_Profile_ParseFavoriteCvar( i, &migratedFavorite ) ) {
            info->favoriteCars[i] = migratedFavorite;
            migrated = qtrue;
        } else {
            Com_Memset( &info->favoriteCars[i], 0, sizeof( info->favoriteCars[i] ) );
        }
    }

    if ( migrated ) {
        UI_Profile_WriteFile( profileName, info, stats );

        for ( i = 0; i < PROFILE_MAX_FAVORITE_CARS; ++i ) {
            char cvarName[16];

            Com_sprintf( cvarName, sizeof( cvarName ), "favoritecar%d", i + 1 );
            trap_Cvar_Set( cvarName, "" );
        }
    }

    return migrated;
}
static int UI_Profile_ParseFavoriteCars( const char *buffer, profile_info_t *info ) {
    const char *favoritesStart;
    const char *cursor;
    int parsedFavorites = 0;

    if ( !buffer || !info ) {
        return 0;
    }

    favoritesStart = strstr( buffer, "\"favoriteCars\"" );
    if ( !favoritesStart ) {
        return 0;
    }

    favoritesStart = strchr( favoritesStart, '[' );
    if ( !favoritesStart ) {
        return 0;
    }

    cursor = favoritesStart + 1;

    while ( *cursor && *cursor != ']' && parsedFavorites < PROFILE_MAX_FAVORITE_CARS ) {
        const char *objectStart = strchr( cursor, '{' );
        const char *objectEnd;
        char objectBuffer[512];
        int objectLength;

        if ( !objectStart ) {
            break;
        }

        objectEnd = strchr( objectStart, '}' );
        if ( !objectEnd || objectEnd <= objectStart ) {
            break;
        }

        objectLength = objectEnd - objectStart + 1;
        if ( objectLength >= (int)sizeof( objectBuffer ) ) {
            objectLength = sizeof( objectBuffer ) - 1;
        }

        Com_Memcpy( objectBuffer, objectStart, objectLength );
        objectBuffer[objectLength] = '\0';

        UI_Profile_ParseString( objectBuffer, "model", info->favoriteCars[parsedFavorites].model, sizeof( info->favoriteCars[parsedFavorites].model ), "" );
        UI_Profile_ParseString( objectBuffer, "skin", info->favoriteCars[parsedFavorites].skin, sizeof( info->favoriteCars[parsedFavorites].skin ), "" );
        UI_Profile_ParseString( objectBuffer, "rim", info->favoriteCars[parsedFavorites].rim, sizeof( info->favoriteCars[parsedFavorites].rim ), "" );
        UI_Profile_ParseString( objectBuffer, "head", info->favoriteCars[parsedFavorites].head, sizeof( info->favoriteCars[parsedFavorites].head ), "" );

        parsedFavorites++;
        cursor = objectEnd + 1;
    }

    return parsedFavorites;
}

static void UI_Profile_SetFavoriteCvars( const profile_info_t *info ) {
    int i;

    if ( !info ) {
        return;
    }

    for ( i = 0; i < PROFILE_MAX_FAVORITE_CARS; ++i ) {
        char cvarName[16];

        Com_sprintf( cvarName, sizeof( cvarName ), "favoritecar%d", i + 1 );

        if ( info->favoriteCars[i].model[0] && info->favoriteCars[i].skin[0] ) {
            char favoriteValue[MAX_QPATH * 4];

            Com_sprintf( favoriteValue, sizeof( favoriteValue ), "%s/%s/%s/%s",
                         info->favoriteCars[i].model,
                         info->favoriteCars[i].skin,
                         info->favoriteCars[i].rim,
                         info->favoriteCars[i].head );
            trap_Cvar_Set( cvarName, favoriteValue );
        } else {
            trap_Cvar_Set( cvarName, "" );
        }
    }
}

static void UI_ProfileOverlay_SetStatus( const char *text, const vec4_t color ) {
    if ( text ) {
        Q_strncpyz( s_profileOverlay.statusLine, text, sizeof( s_profileOverlay.statusLine ) );
    } else {
        s_profileOverlay.statusLine[0] = '\0';
    }
    if ( color ) {
        Vector4Copy( color, s_profileOverlay.statusColor );
    } else {
        Vector4Copy( statusNormalColor, s_profileOverlay.statusColor );
    }
}

static void UI_ProfileOverlay_TrimName( char *name ) {
    char *start;
    char *end;

    if ( !name ) {
        return;
    }

    start = name;
    while ( *start && Q_IsColorString( start ) ) {
        start += 2;
    }

    while ( *start == ' ' || *start == '\t' ) {
        start++;
    }

    if ( start != name ) {
        memmove( name, start, strlen( start ) + 1 );
    }

    end = name + strlen( name );
    while ( end > name && ( end[-1] == ' ' || end[-1] == '\t' ) ) {
        *--end = '\0';
    }
}

static qboolean UI_Profile_NameIsValid( const char *name, char *error, int errorSize ) {
    int length;
    int i;

    if ( !name ) {
        if ( error && errorSize > 0 ) {
            Q_strncpyz( error, "Invalid profile name", errorSize );
        }
        return qfalse;
    }

    length = strlen( name );
    if ( length <= 0 ) {
        if ( error && errorSize > 0 ) {
            Q_strncpyz( error, "Name cannot be empty", errorSize );
        }
        return qfalse;
    }

    if ( length >= PROFILE_MAX_NAME ) {
        if ( error && errorSize > 0 ) {
            Q_strncpyz( error, "Name is too long", errorSize );
        }
        return qfalse;
    }

    for ( i = 0; i < length; ++i ) {
        char c = name[i];
        if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) ) {
            continue;
        }
        if ( c == '_' || c == '-' ) {
            continue;
        }
        if ( error && errorSize > 0 ) {
            Q_strncpyz( error, "Use letters, numbers, '-' or '_' only", errorSize );
        }
        return qfalse;
    }

    return qtrue;
}

static double UI_Profile_ParseDouble( const char *buffer, const char *key, double defaultValue ) {
    char pattern[64];
    const char *cursor;

    Com_sprintf( pattern, sizeof( pattern ), "\"%s\"", key );
    cursor = strstr( buffer, pattern );
    if ( !cursor ) {
        return defaultValue;
    }

    cursor = strchr( cursor, ':' );
    if ( !cursor ) {
        return defaultValue;
    }

    cursor++;
    while ( *cursor == ' ' || *cursor == '\t' ) {
        cursor++;
    }

    return atof( cursor );
}

static int UI_Profile_ParseInt( const char *buffer, const char *key, int defaultValue ) {
    char pattern[64];
    const char *cursor;

    Com_sprintf( pattern, sizeof( pattern ), "\"%s\"", key );
    cursor = strstr( buffer, pattern );
    if ( !cursor ) {
        return defaultValue;
    }

    cursor = strchr( cursor, ':' );
    if ( !cursor ) {
        return defaultValue;
    }

    cursor++;
    while ( *cursor == ' ' || *cursor == '\t' ) {
        cursor++;
    }

    return atoi( cursor );
}

static void UI_Profile_ParseString( const char *buffer, const char *key, char *out, int outSize, const char *defaultValue ) {
    char pattern[64];
    const char *cursor;
    const char *start;
    int length;

    if ( !out || outSize <= 0 ) {
        return;
    }

    out[0] = '\0';
    if ( defaultValue ) {
        Q_strncpyz( out, defaultValue, outSize );
    }

    if ( !buffer || !key ) {
        return;
    }

    Com_sprintf( pattern, sizeof( pattern ), "\"%s\"", key );
    cursor = strstr( buffer, pattern );
    if ( !cursor ) {
        return;
    }

    cursor = strchr( cursor, ':' );
    if ( !cursor ) {
        return;
    }
    cursor++;

    while ( *cursor == ' ' || *cursor == '\t' ) {
        cursor++;
    }

    if ( *cursor != '"' ) {
        return;
    }

    cursor++;
    start = cursor;

    while ( *cursor && *cursor != '"' ) {
        if ( *cursor == '\\' && cursor[1] ) {
            cursor += 2;
            continue;
        }
        cursor++;
    }

    length = cursor - start;
    if ( length >= outSize ) {
        length = outSize - 1;
    }

    Com_Memcpy( out, start, length );
    out[length] = '\0';

    // unescape in-place
    {
        char *dst = out;
        char *src = out;

        while ( *src ) {
            if ( *src == '\\' && src[1] ) {
                src++;
            }
            *dst++ = *src++;
        }
        *dst = '\0';
    }
}

static void UI_Profile_FormatJsonString( char *out, int outSize, const char *value ) {
    const char *src;
    char *dst;

    if ( !out || outSize <= 0 ) {
        return;
    }

    if ( !value ) {
        value = "";
    }

    dst = out;
    src = value;

    while ( *src && ( dst - out ) < outSize - 1 ) {
        if ( (*src == '"' || *src == '\\') && ( dst - out ) < outSize - 2 ) {
            *dst++ = '\\';
        }

        if ( ( dst - out ) >= outSize - 1 ) {
            break;
        }

        *dst++ = *src++;
    }

    *dst = '\0';
}

static qboolean UI_Profile_ReadData( const char *name, profile_info_t *outInfo, profile_stats_t *outStats ) {
    fileHandle_t file;
    char path[MAX_QPATH];
    char buffer[2048];
    int length;

    if ( !name || !name[0] ) {
        return qfalse;
    }

    Com_sprintf( path, sizeof( path ), "profiles/%s.json", name );
    length = trap_FS_FOpenFile( path, &file, FS_READ );
    if ( length <= 0 ) {
        if ( file ) {
            trap_FS_FCloseFile( file );
        }
        return qfalse;
    }

    if ( length >= (int)sizeof( buffer ) ) {
        length = sizeof( buffer ) - 1;
    }

    trap_FS_Read( buffer, length, file );
    buffer[length] = '\0';
    trap_FS_FCloseFile( file );

        if ( outStats ) {
        Com_Memset( outStats, 0, sizeof( *outStats ) );
            outStats->distanceKm = UI_Profile_ParseDouble( buffer, "distanceKm", 0.0 );
            outStats->fuelUsed = UI_Profile_ParseDouble( buffer, "fuelUsed", 0.0 );
            outStats->bestLapMs = UI_Profile_ParseInt( buffer, "bestLapMs", 0 );
            outStats->kills = UI_Profile_ParseInt( buffer, "kills", 0 );
            outStats->deaths = UI_Profile_ParseInt( buffer, "deaths", 0 );
            outStats->wins = UI_Profile_ParseInt( buffer, "wins", 0 );
            outStats->sprintWins = UI_Profile_ParseInt( buffer, "sprintWins", 0 );
            outStats->losses = UI_Profile_ParseInt( buffer, "losses", 0 );
            outStats->flagCaptures = UI_Profile_ParseInt( buffer, "flagCaptures", 0 );
            outStats->flagAssists = UI_Profile_ParseInt( buffer, "flagAssists", 0 );
            outStats->accuracyAwards = UI_Profile_ParseInt( buffer, "accuracyAwards", 0 );
            outStats->excellentAwards = UI_Profile_ParseInt( buffer, "excellentAwards", 0 );
            outStats->impressiveAwards = UI_Profile_ParseInt( buffer, "impressiveAwards", 0 );
            outStats->perfectAwards = UI_Profile_ParseInt( buffer, "perfectAwards", 0 );
            outStats->topSpeedKph = UI_Profile_ParseDouble( buffer, "topSpeedKph", 0.0 );
            outStats->damageDealt = UI_Profile_ParseInt( buffer, "damageDealt", 0 );
            outStats->damageTaken = UI_Profile_ParseInt( buffer, "damageTaken", 0 );
            UI_Profile_ParseString( buffer, "mostUsedVehicle", outStats->mostUsedVehicle, sizeof( outStats->mostUsedVehicle ), "" );
            outStats->mostUsedVehicleTimeMs = UI_Profile_ParseInt( buffer, "mostUsedVehicleTimeMs", 0 );
        }

    if ( outInfo ) {
        Com_Memset( outInfo, 0, sizeof( *outInfo ) );
        UI_Profile_ParseString( buffer, "name", outInfo->name, sizeof( outInfo->name ), name );
        UI_Profile_ParseString( buffer, "gender", outInfo->gender, sizeof( outInfo->gender ), "" );
        UI_Profile_ParseString( buffer, "birthDate", outInfo->birthDate, sizeof( outInfo->birthDate ), "" );
        UI_Profile_ParseString( buffer, "avatar", outInfo->avatar, sizeof( outInfo->avatar ), "" );
        UI_Profile_ParseString( buffer, "country", outInfo->country, sizeof( outInfo->country ), "" );
        UI_Profile_ParseFavoriteCars( buffer, outInfo );
    }

    return qtrue;
}

static qboolean UI_Profile_WriteFile( const char *name, const profile_info_t *info, const profile_stats_t *stats ) {
    fileHandle_t file;
    char path[MAX_QPATH];
    char buffer[1024];
    char gender[PROFILE_MAX_GENDER * 2];
    char birthDate[PROFILE_MAX_BIRTHDATE * 2];
    char avatar[PROFILE_MAX_AVATAR * 2];
    char country[PROFILE_MAX_COUNTRY * 2];
    char favoriteCarsJson[1024];
    char favoriteField[PROFILE_MAX_FAVORITE_FIELD * 2];
    profile_stats_t zeroStats;
    profile_info_t emptyInfo;
    int length;
    int favoriteIndex;

    if ( !name || !name[0] ) {
        return qfalse;
    }

    Com_sprintf( path, sizeof( path ), "profiles/%s.json", name );

    if ( !info ) {
        Com_Memset( &emptyInfo, 0, sizeof( emptyInfo ) );
        info = &emptyInfo;
    }

    if ( !stats ) {
        Com_Memset( &zeroStats, 0, sizeof( zeroStats ) );
        stats = &zeroStats;
    }

    UI_Profile_FormatJsonString( gender, sizeof( gender ), info->gender );
    UI_Profile_FormatJsonString( birthDate, sizeof( birthDate ), info->birthDate );
    UI_Profile_FormatJsonString( avatar, sizeof( avatar ), info->avatar );
    UI_Profile_FormatJsonString( country, sizeof( country ), info->country );

    length = 0;
    length += Com_sprintf( favoriteCarsJson + length, sizeof( favoriteCarsJson ) - length, "[\n" );
    for ( favoriteIndex = 0; favoriteIndex < PROFILE_MAX_FAVORITE_CARS; ++favoriteIndex ) {
        if ( favoriteIndex > 0 ) {
            length += Com_sprintf( favoriteCarsJson + length, sizeof( favoriteCarsJson ) - length, ",\n" );
        }

        UI_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), info->favoriteCars[favoriteIndex].model );
        length += Com_sprintf( favoriteCarsJson + length, sizeof( favoriteCarsJson ) - length,
                               "\t\t\t{\"model\": \"%s\", ", favoriteField );

        UI_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), info->favoriteCars[favoriteIndex].skin );
        length += Com_sprintf( favoriteCarsJson + length, sizeof( favoriteCarsJson ) - length,
                               "\"skin\": \"%s\", ", favoriteField );

        UI_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), info->favoriteCars[favoriteIndex].rim );
        length += Com_sprintf( favoriteCarsJson + length, sizeof( favoriteCarsJson ) - length,
                               "\"rim\": \"%s\", ", favoriteField );

        UI_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), info->favoriteCars[favoriteIndex].head );
        length += Com_sprintf( favoriteCarsJson + length, sizeof( favoriteCarsJson ) - length,
                               "\"head\": \"%s\"}", favoriteField );
    }
    Com_sprintf( favoriteCarsJson + length, sizeof( favoriteCarsJson ) - length, "\n\t\t]" );

    length = Com_sprintf( buffer, sizeof( buffer ),
        "{\n"
        "\t\"name\": \"%s\",\n"
        "\t\"info\": {\n"
        "\t\t\"gender\": \"%s\",\n"
        "\t\t\"birthDate\": \"%s\",\n"
        "\t\t\"avatar\": \"%s\",\n"
        "\t\t\"country\": \"%s\",\n"
        "\t\t\"favoriteCars\": %s\n"
        "\t},\n"
        "\t\"stats\": {\n"
        "\t\t\"distanceKm\": %.6f,\n"
        "\t\t\"fuelUsed\": %.3f,\n"
        "\t\t\"bestLapMs\": %d,\n"
        "\t\t\"kills\": %d,\n"
        "\t\t\"deaths\": %d,\n"
        "\t\t\"wins\": %d,\n"
        "\t\t\"sprintWins\": %d,\n"
        "\t\t\"losses\": %d,\n"
        "\t\t\"flagCaptures\": %d,\n"
        "\t\t\"flagAssists\": %d,\n"
        "\t\t\"accuracyAwards\": %d,\n"
        "\t\t\"excellentAwards\": %d,\n"
        "\t\t\"impressiveAwards\": %d,\n"
        "\t\t\"perfectAwards\": %d,\n"
        "\t\t\"topSpeedKph\": %.2f,\n"
        "\t\t\"damageDealt\": %d,\n"
        "\t\t\"damageTaken\": %d,\n"
        "\t\t\"mostUsedVehicle\": \"%s\",\n"
        "\t\t\"mostUsedVehicleTimeMs\": %d\n"
        "\t}\n"
        "}\n",
        info->name[0] ? info->name : name,
        gender,
        birthDate,
        avatar,
        country,
        favoriteCarsJson,
        stats->distanceKm,
        stats->fuelUsed,
        stats->bestLapMs,
        stats->kills,
        stats->deaths,
        stats->wins,
        stats->sprintWins,
        stats->losses,
        stats->flagCaptures,
        stats->flagAssists,
        stats->accuracyAwards,
        stats->excellentAwards,
        stats->impressiveAwards,
        stats->perfectAwards,
        stats->topSpeedKph,
        stats->damageDealt,
        stats->damageTaken,
        stats->mostUsedVehicle,
        stats->mostUsedVehicleTimeMs );

    trap_FS_FOpenFile( path, &file, FS_WRITE );
    if ( file < 0 ) {
        return qfalse;
    }

    trap_FS_Write( buffer, length, file );
    trap_FS_FCloseFile( file );
    return qtrue;
}

static qboolean UI_Profile_WriteDefaultFile( const char *name ) {
    profile_info_t info;
    profile_stats_t stats;

    if ( !name || !name[0] ) {
        return qfalse;
    }

    Com_Memset( &info, 0, sizeof( info ) );
    Com_Memset( &stats, 0, sizeof( stats ) );

    return UI_Profile_WriteFile( name, &info, &stats );
}

static void UI_ProfileOverlay_EnsureSelectionVisible( void ) {
    int viewSize;
    int count;

    count = s_profileOverlay.profileCount;
    if ( count <= 0 ) {
        s_profileOverlay.list.top = 0;
        s_profileOverlay.list.curvalue = 0;
        s_profileOverlay.list.oldvalue = 0;
        return;
    }

    if ( s_profileOverlay.list.curvalue < 0 ) {
        s_profileOverlay.list.curvalue = 0;
    } else if ( s_profileOverlay.list.curvalue >= count ) {
        s_profileOverlay.list.curvalue = count - 1;
    }

    viewSize = s_profileOverlay.list.height;
    if ( viewSize <= 0 ) {
        viewSize = 1;
    }
    if ( viewSize > count ) {
        viewSize = count;
    }

    if ( s_profileOverlay.list.top < 0 ) {
        s_profileOverlay.list.top = 0;
    }
    if ( s_profileOverlay.list.top > count - viewSize ) {
        s_profileOverlay.list.top = count - viewSize;
    }
    if ( s_profileOverlay.list.top < 0 ) {
        s_profileOverlay.list.top = 0;
    }

    if ( s_profileOverlay.list.curvalue < s_profileOverlay.list.top ) {
        s_profileOverlay.list.top = s_profileOverlay.list.curvalue;
    } else if ( s_profileOverlay.list.curvalue >= s_profileOverlay.list.top + viewSize ) {
        s_profileOverlay.list.top = s_profileOverlay.list.curvalue - ( viewSize - 1 );
    }

    if ( s_profileOverlay.list.top < 0 ) {
        s_profileOverlay.list.top = 0;
    }
    if ( s_profileOverlay.list.top > count - viewSize ) {
        s_profileOverlay.list.top = count - viewSize;
        if ( s_profileOverlay.list.top < 0 ) {
            s_profileOverlay.list.top = 0;
        }
    }

    s_profileOverlay.list.oldvalue = s_profileOverlay.list.curvalue;
}

static void UI_ProfileOverlay_LoadProfiles( void ) {
    char fileBuffer[4096];
    char activeName[PROFILE_MAX_NAME];
    char *ptr;
    int total;
    int index;
    int i;

    s_profileOverlay.profileCount = 0;
    trap_Cvar_VariableStringBuffer( "profile_active", activeName, sizeof( activeName ) );

    total = trap_FS_GetFileList( "profiles", ".json", fileBuffer, sizeof( fileBuffer ) );
    ptr = fileBuffer;

    for ( i = 0; i < total && s_profileOverlay.profileCount < MAX_PROFILE_FILES; ++i ) {
        char name[MAX_QPATH];
        int len = strlen( ptr );
        int fileLen;
        fileHandle_t file;

        if ( len <= 0 ) {
            ptr++;
            continue;
        }

        COM_StripExtension( ptr, name, sizeof( name ) );
        if ( !UI_Profile_NameIsValid( name, NULL, 0 ) ) {
            ptr += len + 1;
            continue;
        }

        // ignore zero-length files (treated as deleted)
        fileLen = trap_FS_FOpenFile( va( "profiles/%s.json", name ), &file, FS_READ );
        if ( fileLen <= 0 ) {
            if ( file ) {
                trap_FS_FCloseFile( file );
            }
            ptr += len + 1;
            continue;
        }
        trap_FS_FCloseFile( file );

        Q_strncpyz( s_profileOverlay.profileNames[s_profileOverlay.profileCount], name, PROFILE_MAX_NAME );
        s_profileOverlay.listItems[s_profileOverlay.profileCount] = s_profileOverlay.profileNames[s_profileOverlay.profileCount];
        s_profileOverlay.profileCount++;
        ptr += len + 1;
    }

    s_profileOverlay.listItems[s_profileOverlay.profileCount] = NULL;

    s_profileOverlay.list.generic.flags &= ~QMF_GRAYED;
    s_profileOverlay.list.generic.flags &= ~QMF_HIDDEN;
    s_profileOverlay.deleteButton.generic.flags &= ~QMF_GRAYED;
    s_profileOverlay.selectButton.generic.flags &= ~QMF_GRAYED;

    if ( s_profileOverlay.profileCount <= 0 ) {
        s_profileOverlay.list.itemnames = emptyProfileList;
        s_profileOverlay.list.numitems = 1;
        s_profileOverlay.list.curvalue = 0;
        s_profileOverlay.list.top = 0;
        s_profileOverlay.list.oldvalue = 0;
        s_profileOverlay.list.generic.flags |= QMF_GRAYED;
        s_profileOverlay.deleteButton.generic.flags |= QMF_GRAYED;
        s_profileOverlay.selectButton.generic.flags |= QMF_GRAYED;
        s_profileOverlay.forcingSelection = qtrue;
        UI_ProfileOverlay_SetStatus( "Create a new profile to continue", statusInfoColor );
        if ( uis.profileOverlayShown ) {
            UI_ProfileOverlay_FocusNameField();
        }
        return;
    }

    s_profileOverlay.list.itemnames = s_profileOverlay.listItems;
    s_profileOverlay.list.numitems = s_profileOverlay.profileCount;

    index = 0;
    if ( activeName[0] ) {
        for ( i = 0; i < s_profileOverlay.profileCount; ++i ) {
            if ( !Q_stricmp( s_profileOverlay.profileNames[i], activeName ) ) {
                index = i;
                break;
            }
        }
    }
    s_profileOverlay.list.curvalue = index;
    UI_ProfileOverlay_EnsureSelectionVisible();
    s_profileOverlay.forcingSelection = qfalse;
    UI_ProfileOverlay_SetStatus( "Select a profile to continue", statusNormalColor );
}

static void UI_ProfileOverlay_MenuEvent( void *ptr, int event );
static qboolean UI_ProfileOverlay_HandleCreate( void );
static qboolean UI_ProfileOverlay_HandleDelete( void );
static qboolean UI_ProfileOverlay_HandleSelect( void );

static void UI_ProfileOverlay_SetupMenu( void ) {
    profileOverlay_t *overlay = &s_profileOverlay;

    Com_Memset( overlay, 0, sizeof( *overlay ) );

    overlay->menu.fullscreen = qtrue;
    overlay->menu.wrapAround = qfalse;
    overlay->menu.draw = UI_ProfileOverlay_Draw;
    overlay->menu.key = UI_ProfileOverlay_Key;

    overlay->contentBaseY = PROFILE_OVERLAY_TOP_MARGIN +
        (PROFILE_OVERLAY_AVAILABLE_HEIGHT - PROFILE_OVERLAY_CONTENT_SPAN) / 2;

    overlay->title.generic.type = MTYPE_BTEXT;
    overlay->title.generic.flags = QMF_INACTIVE;
    overlay->title.generic.x = 320;
    overlay->title.generic.y = overlay->contentBaseY + PROFILE_OVERLAY_TITLE_OFFSET;
    overlay->title.string = "PROFILE SELECTION";
    overlay->title.color = text_color_normal;
    overlay->title.style = UI_CENTER | UI_BIGFONT;

    overlay->list.generic.type = MTYPE_SCROLLLIST;
    overlay->list.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS | QMF_SMALLFONT;
    overlay->list.generic.id = ID_PROFILE_LIST;
    overlay->list.generic.callback = UI_ProfileOverlay_MenuEvent;
    overlay->list.generic.x = 320;
    overlay->list.generic.y = overlay->contentBaseY + PROFILE_OVERLAY_LIST_OFFSET;
    overlay->list.curvalue = 0;
    overlay->list.itemnames = overlay->listItems;
    overlay->list.width = 24;
    overlay->list.height = 6;
    overlay->list.columns = 1;
    overlay->list.separation = 0;

    overlay->nameField.generic.type = MTYPE_FIELD;
    overlay->nameField.generic.id = ID_PROFILE_NAME;
    overlay->nameField.generic.flags = QMF_SMALLFONT | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
    overlay->nameField.generic.x = 320;
    overlay->nameField.generic.y = overlay->contentBaseY + PROFILE_OVERLAY_NAMEFIELD_OFFSET;
    overlay->nameField.generic.name = "NEW PROFILE";
    overlay->nameField.generic.callback = NULL;
    overlay->nameField.generic.ownerdraw = UI_ProfileOverlay_DrawNameField;
    overlay->nameField.generic.statusbar = NULL;
    overlay->nameField.field.widthInChars = 20;
    overlay->nameField.field.maxchars = PROFILE_MAX_NAME - 1;

    MenuField_Init( &overlay->nameField );

    overlay->nameField.field.cursor = 0;
    overlay->nameField.field.scroll = 0;
    overlay->nameField.field.buffer[0] = '\0';

    overlay->createButton.generic.type = MTYPE_PTEXT;
    overlay->createButton.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    overlay->createButton.generic.id = ID_PROFILE_CREATE;
    overlay->createButton.generic.callback = UI_ProfileOverlay_MenuEvent;
    overlay->createButton.generic.x = 200;
    overlay->createButton.generic.y = overlay->contentBaseY + PROFILE_OVERLAY_BUTTON_ROW_OFFSET;
    overlay->createButton.string = "CREATE";
    overlay->createButton.style = UI_CENTER | UI_SMALLFONT;
    overlay->createButton.color = text_color_normal;

    overlay->deleteButton.generic.type = MTYPE_PTEXT;
    overlay->deleteButton.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    overlay->deleteButton.generic.id = ID_PROFILE_DELETE;
    overlay->deleteButton.generic.callback = UI_ProfileOverlay_MenuEvent;
    overlay->deleteButton.generic.x = 320;
    overlay->deleteButton.generic.y = overlay->contentBaseY + PROFILE_OVERLAY_BUTTON_ROW_OFFSET;
    overlay->deleteButton.string = "DELETE";
    overlay->deleteButton.style = UI_CENTER | UI_SMALLFONT;
    overlay->deleteButton.color = text_color_normal;

    overlay->selectButton.generic.type = MTYPE_PTEXT;
    overlay->selectButton.generic.flags = QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    overlay->selectButton.generic.id = ID_PROFILE_SELECT;
    overlay->selectButton.generic.callback = UI_ProfileOverlay_MenuEvent;
    overlay->selectButton.generic.x = 440;
    overlay->selectButton.generic.y = overlay->contentBaseY + PROFILE_OVERLAY_BUTTON_ROW_OFFSET;
    overlay->selectButton.string = "SELECT";
    overlay->selectButton.style = UI_CENTER | UI_SMALLFONT;
    overlay->selectButton.color = text_color_normal;

    UI_ProfileOverlay_LoadProfiles();

    Menu_AddItem( &overlay->menu, &overlay->title );
    Menu_AddItem( &overlay->menu, &overlay->list );
    Menu_AddItem( &overlay->menu, &overlay->nameField );
    Menu_AddItem( &overlay->menu, &overlay->createButton );
    Menu_AddItem( &overlay->menu, &overlay->deleteButton );
    Menu_AddItem( &overlay->menu, &overlay->selectButton );
}

static void UI_ProfileOverlay_MenuEvent( void *ptr, int event ) {
    menucommon_s *item;

    if ( event != QM_ACTIVATED ) {
        return;
    }

    item = (menucommon_s *)ptr;
    switch ( item->id ) {
        case ID_PROFILE_LIST:
            UI_ProfileOverlay_HandleSelect();
            break;
        case ID_PROFILE_CREATE:
            UI_ProfileOverlay_HandleCreate();
            break;
        case ID_PROFILE_DELETE:
            UI_ProfileOverlay_HandleDelete();
            break;
        case ID_PROFILE_SELECT:
            UI_ProfileOverlay_HandleSelect();
            break;
    }
}

static qboolean UI_ProfileOverlay_HandleCreate( void ) {
    char name[PROFILE_MAX_NAME];
    char error[64];
    int i;

    Q_strncpyz( name, s_profileOverlay.nameField.field.buffer, sizeof( name ) );
    UI_ProfileOverlay_TrimName( name );

    if ( !UI_Profile_NameIsValid( name, error, sizeof( error ) ) ) {
        UI_ProfileOverlay_SetStatus( error, statusErrorColor );
        return qfalse;
    }

    for ( i = 0; i < s_profileOverlay.profileCount; ++i ) {
        if ( !Q_stricmp( s_profileOverlay.profileNames[i], name ) ) {
            UI_ProfileOverlay_SetStatus( "Profile already exists", statusErrorColor );
            return qfalse;
        }
    }

    if ( !UI_Profile_WriteDefaultFile( name ) ) {
        UI_ProfileOverlay_SetStatus( "Failed to create profile", statusErrorColor );
        return qfalse;
    }

    s_profileOverlay.nameField.field.buffer[0] = '\0';
    s_profileOverlay.nameField.field.cursor = 0;
    s_profileOverlay.nameField.field.scroll = 0;
    UI_ProfileOverlay_LoadProfiles();

    if ( uis.profileOverlayShown ) {
        UI_ProfileOverlay_FocusNameField();
    }

    for ( i = 0; i < s_profileOverlay.profileCount; ++i ) {
        if ( !Q_stricmp( s_profileOverlay.profileNames[i], name ) ) {
            s_profileOverlay.list.curvalue = i;
            UI_ProfileOverlay_EnsureSelectionVisible();
            break;
        }
    }

    UI_ProfileOverlay_SetStatus( "Profile created", statusInfoColor );
    return qtrue;
}

static qboolean UI_ProfileOverlay_HandleDelete( void ) {
    const char *name;
    char path[MAX_QPATH];
    fileHandle_t file;

    if ( s_profileOverlay.profileCount <= 0 ) {
        UI_ProfileOverlay_SetStatus( "Nothing to delete", statusErrorColor );
        return qfalse;
    }

    name = s_profileOverlay.profileNames[ s_profileOverlay.list.curvalue ];
    if ( !name || !name[0] ) {
        UI_ProfileOverlay_SetStatus( "Invalid selection", statusErrorColor );
        return qfalse;
    }

    Com_sprintf( path, sizeof( path ), "profiles/%s.json", name );
    trap_FS_FOpenFile( path, &file, FS_WRITE );
    if ( file < 0 ) {
        UI_ProfileOverlay_SetStatus( "Unable to remove profile", statusErrorColor );
        return qfalse;
    }
    trap_FS_FCloseFile( file );

    if ( !Q_stricmp( uis.activeProfile, name ) ) {
        uis.activeProfile[0] = '\0';
        trap_Cvar_Set( "profile_active", "" );
        trap_Cvar_Update( &ui_profileActive );
        UI_Profile_MarkStatsDirty();
    }

    UI_ProfileOverlay_LoadProfiles();
    UI_ProfileOverlay_SetStatus( "Profile deleted", statusInfoColor );
    return qtrue;
}

static qboolean UI_ProfileOverlay_HandleSelect( void ) {
    const char *name;

    if ( s_profileOverlay.profileCount <= 0 ) {
        UI_ProfileOverlay_SetStatus( "Create a profile first", statusErrorColor );
        return qfalse;
    }

    name = s_profileOverlay.profileNames[ s_profileOverlay.list.curvalue ];
    if ( !name || !name[0] ) {
        UI_ProfileOverlay_SetStatus( "Invalid profile", statusErrorColor );
        return qfalse;
    }

    trap_Cvar_Set( "profile_active", name );
    trap_Cvar_Update( &ui_profileActive );
    Q_strncpyz( uis.activeProfile, name, sizeof( uis.activeProfile ) );
    uis.profileOverlayShown = qtrue;
    UI_Profile_MarkStatsDirty();

    UI_PopMenu();
    return qtrue;
}

static void UI_ProfileOverlay_Draw( void ) {
    trap_R_SetColor( overlayBackgroundColor );
    UI_FillRect( 0,
                 PROFILE_OVERLAY_TOP_MARGIN,
                 PROFILE_OVERLAY_SCREEN_WIDTH,
                 PROFILE_OVERLAY_SCREEN_HEIGHT - PROFILE_OVERLAY_TOP_MARGIN - PROFILE_OVERLAY_BOTTOM_MARGIN,
                 overlayBackgroundColor );
    trap_R_SetColor( NULL );

    Menu_Draw( &s_profileOverlay.menu );

    if ( s_profileOverlay.statusLine[0] ) {
        UI_DrawProportionalString( 320,
                                   s_profileOverlay.contentBaseY + PROFILE_OVERLAY_STATUS_OFFSET,
                                   s_profileOverlay.statusLine,
                                   UI_CENTER | UI_SMALLFONT,
                                   s_profileOverlay.statusColor );
    }

    if ( s_profileOverlay.profileCount > 0 ) {
        UI_DrawProportionalString( 320,
                                   s_profileOverlay.contentBaseY + PROFILE_OVERLAY_GUIDE_PRIMARY_OFFSET,
                                   "Highlight a profile and press ENTER or SELECT",
                                   UI_CENTER | UI_SMALLFONT,
                                   text_color_normal );
        UI_DrawProportionalString( 320,
                                   s_profileOverlay.contentBaseY + PROFILE_OVERLAY_GUIDE_SECONDARY_OFFSET,
                                   "Enter a name and press CREATE below to add another",
                                   UI_CENTER | UI_SMALLFONT,
                                   text_color_normal );
    } else {
        UI_DrawProportionalString( 320,
                                   s_profileOverlay.contentBaseY + PROFILE_OVERLAY_GUIDE_EMPTY_OFFSET,
                                   "Enter a name and press CREATE below",
                                   UI_CENTER | UI_SMALLFONT,
                                   text_color_normal );
    }
    UI_DrawProportionalString( 320,
                                   s_profileOverlay.contentBaseY + PROFILE_OVERLAY_GUIDE_HINT_OFFSET,
                                   "Edit profile details later under CONFIG -> PLAYER -> PROFILE",
                                   UI_CENTER | UI_SMALLFONT,
                                   text_color_normal );
}

static void UI_ProfileOverlay_DrawNameField( void *self ) {
	menufield_s *f = (menufield_s *)self;
	qboolean focus = (f->generic.parent->cursor == f->generic.menuPosition);
	int style = UI_LEFT | UI_SMALLFONT;
	float *color = text_color_normal;

	if ( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawProportionalString( f->generic.x - 70, f->generic.y - 1, f->generic.name, style, color );

    MField_Draw( &f->field, f->generic.x - 72, f->generic.y + 18, style, color );
}

static qboolean UI_ProfileOverlay_CanDismiss( void ) {
    if ( s_profileOverlay.forcingSelection ) {
        return qfalse;
    }

    return UI_Profile_HasActiveProfile();
}

static sfxHandle_t UI_ProfileOverlay_Key( int key ) {
    menucommon_s *item;

    if ( key == K_ESCAPE ) {
        if ( UI_ProfileOverlay_CanDismiss() ) {
            uis.profileOverlayShown = qtrue;
            UI_PopMenu();
            return menu_out_sound;
        }
        return menu_buzz_sound;
    }

    item = Menu_ItemAtCursor( &s_profileOverlay.menu );

    if ( item == (menucommon_s *)&s_profileOverlay.nameField ) {
        if ( key == K_ENTER || key == K_KP_ENTER ) {
            return UI_ProfileOverlay_HandleCreate() ? menu_move_sound : menu_buzz_sound;
        }
    }

    // Spezial-Handling für ENTER auf der Liste
    if ( ( key == K_ENTER || key == K_KP_ENTER ) && item == (menucommon_s *)&s_profileOverlay.list ) {
        return UI_ProfileOverlay_HandleSelect() ? menu_move_sound : menu_buzz_sound;
    }

    // Spezial-Handling für DEL auf der Liste
    if ( ( key == K_DEL || key == K_KP_DEL ) && item == (menucommon_s *)&s_profileOverlay.list ) {
        return UI_ProfileOverlay_HandleDelete() ? menu_move_sound : menu_buzz_sound;
    }

    // Alle anderen Keys an das Standard-Menu-System übergeben
    return Menu_DefaultKey( &s_profileOverlay.menu, key );
}

void UI_ProfileOverlay_InitSession( void ) {
    trap_Cvar_Update( &ui_profileOverlaySeen );

    uis.profileOverlayShown = ( ui_profileOverlaySeen.integer != 0 );
    uis.activeProfile[0] = '\0';
    uis.activeProfileStatsValid = qfalse;
    uis.activeProfileLastRead = 0;
    uis.activeProfileInfoValid = qfalse;
    uis.activeProfileInfoLastRead = 0;

    trap_Cvar_Update( &ui_profileActive );
    if ( ui_profileActive.string && ui_profileActive.string[0] ) {
        Q_strncpyz( uis.activeProfile, ui_profileActive.string, sizeof( uis.activeProfile ) );
    } else {
        trap_Cvar_VariableStringBuffer( "profile_active", uis.activeProfile, sizeof( uis.activeProfile ) );
    }
}

void UI_ProfileOverlay_ClearState( void ) {
    Com_Memset( &s_profileOverlay, 0, sizeof( s_profileOverlay ) );
}

void UI_ProfileOverlay_MaybeShow( void ) {
    if ( uis.profileOverlayShown ) {
        return;
    }

    UI_ProfileOverlay_SetupMenu();
    trap_Cvar_Set( "ui_profileOverlaySeen", "1" );
    trap_Cvar_Update( &ui_profileOverlaySeen );
    uis.profileOverlayShown = qtrue;
    UI_PushMenu( &s_profileOverlay.menu );

    // The overlay does not use the regular menu transition system. When it is
    // shown while another menu is still transitioning in, the global
    // transition timer stays active and blocks all key input (see
    // UI_KeyEvent). Clear the timers explicitly so text input becomes
    // available immediately.
    uis.transitionIn = 0;
    uis.transitionOut = 0;

    UI_ProfileOverlay_FocusNameField();
}

static void UI_ProfileOverlay_FocusNameField( void ) {
    Menu_SetCursorToItem( &s_profileOverlay.menu, &s_profileOverlay.nameField );
}

void UI_Profile_MarkStatsDirty( void ) {
    uis.activeProfileStatsValid = qfalse;
    uis.activeProfileInfoValid = qfalse;
}

const char *UI_Profile_GetActiveName( void ) {
    return uis.activeProfile;
}

qboolean UI_Profile_HasActiveProfile( void ) {
    return ( qboolean )( uis.activeProfile[0] != '\0' );
}

static qboolean UI_Profile_EnsureDataFresh( void ) {
    profile_stats_t stats;
    profile_info_t info;

    if ( !uis.activeProfile[0] ) {
        return qfalse;
    }

    if ( !uis.activeProfileStatsValid || !uis.activeProfileInfoValid ||
         uis.realtime - uis.activeProfileLastRead > 1000 ||
         uis.realtime - uis.activeProfileInfoLastRead > 1000 ) {
        if ( !UI_Profile_ReadData( uis.activeProfile, &info, &stats ) ) {
            return qfalse;
        }

        UI_Profile_MigrateFavoriteCvars( uis.activeProfile, &info, &stats );

        uis.activeProfileStats = stats;
        uis.activeProfileInfo = info;
        uis.activeProfileStatsValid = qtrue;
        uis.activeProfileInfoValid = qtrue;
        uis.activeProfileLastRead = uis.realtime;
        uis.activeProfileInfoLastRead = uis.realtime;

        UI_Profile_SetFavoriteCvars( &uis.activeProfileInfo );

        if ( uis.activeProfileInfo.name[0] ) {
            trap_Cvar_Set( "name", uis.activeProfileInfo.name );
        } else if ( uis.activeProfile[0] ) {
            trap_Cvar_Set( "name", uis.activeProfile );
        }
    }

    return qtrue;
}

const profile_stats_t *UI_Profile_GetActiveStats( void ) {
    if ( !UI_Profile_EnsureDataFresh() ) {
        return NULL;
    }

    return &uis.activeProfileStats;
}

const profile_info_t *UI_Profile_GetActiveInfo( void ) {
    if ( !UI_Profile_EnsureDataFresh() ) {
        return NULL;
    }

    return &uis.activeProfileInfo;
}

qboolean UI_Profile_SaveActiveInfo( const profile_info_t *info ) {
    profile_stats_t statsCopy;
    profile_info_t infoCopy;
    const profile_stats_t *stats;

    if ( !UI_Profile_HasActiveProfile() || !info ) {
        return qfalse;
    }

    stats = UI_Profile_GetActiveStats();
    if ( stats ) {
        statsCopy = *stats;
    } else {
        Com_Memset( &statsCopy, 0, sizeof( statsCopy ) );
    }

    infoCopy = *info;

    if ( !UI_Profile_WriteFile( uis.activeProfile, &infoCopy, &statsCopy ) ) {
        return qfalse;
    }

    uis.activeProfileInfo = infoCopy;
    uis.activeProfileInfoValid = qtrue;
    uis.activeProfileInfoLastRead = uis.realtime;
    uis.activeProfileLastRead = uis.realtime;

    return qtrue;
}
