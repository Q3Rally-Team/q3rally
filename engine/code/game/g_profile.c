#include "g_local.h"
#include "g_profile.h"

#define PROFILE_AUTOSAVE_INTERVAL 30000
#define PROFILE_DISPLAY_L_PER_100KM 9.0f

static struct {
    qboolean        loaded;
    qboolean        dirty;
    char            name[PROFILE_MAX_NAME];
    profile_stats_t stats;
    profile_info_t  info;
    int             nextAutosaveTime;
} s_profileState;

static void G_Profile_ParseString( const char *buffer, const char *key, char *out, int outSize, const char *defaultValue );

static qboolean G_Profile_ShouldTrackClient( const gclient_t *client ) {
    if ( !s_profileState.loaded ) {
        return qfalse;
    }

    if ( !client ) {
        return qfalse;
    }

    return client->pers.localClient ? qtrue : qfalse;
}

typedef struct {
    char name[PROFILE_MAX_VEHICLE];
    int  timeMs;
} profile_vehicle_usage_t;

#define PROFILE_MAX_TRACKED_VEHICLES 8
static profile_vehicle_usage_t s_profileVehicleUsage[PROFILE_MAX_TRACKED_VEHICLES];
static const char *G_Profile_FindSectionEnd( const char *start, char endChar ) {
    const char *end = strchr( start, endChar );
    return end ? end : start;
}

static int G_Profile_ParseFavoriteCars( const char *buffer, profile_info_t *info ) {
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
        profile_info_t tempInfo;

        if ( !objectStart ) {
            break;
        }

        objectEnd = G_Profile_FindSectionEnd( objectStart, '}' );
        if ( !objectEnd || objectEnd <= objectStart ) {
            break;
        }
        objectLength = objectEnd - objectStart + 1;
        if ( objectLength >= (int)sizeof( objectBuffer ) ) {
            objectLength = sizeof( objectBuffer ) - 1;
        }

        Com_Memcpy( objectBuffer, objectStart, objectLength );
        objectBuffer[objectLength] = '\0';

        Com_Memset( &tempInfo.favoriteCars[parsedFavorites], 0, sizeof( tempInfo.favoriteCars[parsedFavorites] ) );
        G_Profile_ParseString( objectBuffer, "model", tempInfo.favoriteCars[parsedFavorites].model, sizeof( tempInfo.favoriteCars[parsedFavorites].model ), "" );
        G_Profile_ParseString( objectBuffer, "skin", tempInfo.favoriteCars[parsedFavorites].skin, sizeof( tempInfo.favoriteCars[parsedFavorites].skin ), "" );
        G_Profile_ParseString( objectBuffer, "rim", tempInfo.favoriteCars[parsedFavorites].rim, sizeof( tempInfo.favoriteCars[parsedFavorites].rim ), "" );
        G_Profile_ParseString( objectBuffer, "head", tempInfo.favoriteCars[parsedFavorites].head, sizeof( tempInfo.favoriteCars[parsedFavorites].head ), "" );

        info->favoriteCars[parsedFavorites] = tempInfo.favoriteCars[parsedFavorites];
        parsedFavorites++;

        cursor = objectEnd + 1;
    }

    return parsedFavorites;
}

static qboolean G_Profile_IsValidName( const char *name ) {
    int i;
    int len;

    if ( !name ) {
        return qfalse;
    }

    len = strlen( name );
    if ( len <= 0 || len >= PROFILE_MAX_NAME ) {
        return qfalse;
    }

    for ( i = 0; i < len; ++i ) {
        char c = name[i];
        if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) ) {
            continue;
        }
        if ( c == '_' || c == '-' ) {
            continue;
        }
        return qfalse;
    }

    return qtrue;
}

static void G_Profile_BuildPath( const char *name, char *buffer, int bufferSize ) {
    Com_sprintf( buffer, bufferSize, "profiles/%s.json", name );
}

static double G_Profile_ParseDouble( const char *buffer, const char *key, double defaultValue ) {
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

static int G_Profile_ParseInt( const char *buffer, const char *key, int defaultValue ) {
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

static void G_Profile_ParseString( const char *buffer, const char *key, char *out, int outSize, const char *defaultValue ) {
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
        if ( *cursor == '\\' && cursor[1] != '\0' ) {
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

    // unescape quotes and backslashes in-place
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

static void G_Profile_FormatJsonString( char *out, int outSize, const char *value ) {
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

static profile_vehicle_usage_t *G_Profile_FindVehicleUsage( const char *vehicle, qboolean allowCreate ) {
    int i;
    profile_vehicle_usage_t *empty = NULL;

    if ( !vehicle || !vehicle[0] ) {
        return NULL;
    }

    for ( i = 0; i < PROFILE_MAX_TRACKED_VEHICLES; ++i ) {
        if ( !s_profileVehicleUsage[i].name[0] ) {
            if ( !empty ) {
                empty = &s_profileVehicleUsage[i];
            }
            continue;
        }

        if ( !Q_stricmp( s_profileVehicleUsage[i].name, vehicle ) ) {
            return &s_profileVehicleUsage[i];
        }
    }

    if ( allowCreate && empty ) {
        Q_strncpyz( empty->name, vehicle, sizeof( empty->name ) );
        empty->timeMs = 0;
        return empty;
    }

    return NULL;
}

static void G_Profile_AddVehicleTime( const char *vehicle, int timeMs ) {
    profile_vehicle_usage_t *usage;

    if ( timeMs <= 0 ) {
        return;
    }

    usage = G_Profile_FindVehicleUsage( vehicle, qtrue );
    if ( !usage ) {
        Com_Printf( "G_Profile: Failed to find/create vehicle usage for '%s'\n", vehicle );
        return;
    }

    usage->timeMs += timeMs;
    
    Com_Printf( "G_Profile: Vehicle '%s' now has %d ms (added %d ms)\n", 
               usage->name, usage->timeMs, timeMs );
    
    if ( usage->timeMs > s_profileState.stats.mostUsedVehicleTimeMs ) {
        s_profileState.stats.mostUsedVehicleTimeMs = usage->timeMs;
        Q_strncpyz( s_profileState.stats.mostUsedVehicle, usage->name, sizeof( s_profileState.stats.mostUsedVehicle ) );
        s_profileState.dirty = qtrue;
        
        Com_Printf( "G_Profile: NEW most used vehicle: '%s' with %d ms\n", 
                   s_profileState.stats.mostUsedVehicle,
                   s_profileState.stats.mostUsedVehicleTimeMs );
    }
}

static void G_Profile_UpdateVehicleUsage( gentity_t *ent, int frameMsec ) {
    char userinfo[MAX_INFO_STRING];
    const char *vehicle;

    if ( frameMsec <= 0 ) {
        return;
    }

    trap_GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );
    
    // Versuche zuerst "chassis", dann "vehicle", dann "model"
    vehicle = Info_ValueForKey( userinfo, "chassis" );
    if ( !vehicle || !vehicle[0] ) {
        vehicle = Info_ValueForKey( userinfo, "vehicle" );
    }
    if ( !vehicle || !vehicle[0] ) {
        vehicle = Info_ValueForKey( userinfo, "model" );
    }

    if ( vehicle && vehicle[0] ) {
        G_Profile_AddVehicleTime( vehicle, frameMsec );
    }
}

static qboolean G_Profile_LoadFromDisk( void ) {
    fileHandle_t file;
    char path[MAX_QPATH];
    char buffer[2048];
    int length;
    const char *vehiclesStart;
    const char *cursor;

    if ( !s_profileState.name[0] ) {
        return qfalse;
    }

    G_Profile_BuildPath( s_profileState.name, path, sizeof( path ) );

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

    Com_Memset( &s_profileState.stats, 0, sizeof( s_profileState.stats ) );
    Com_Memset( &s_profileState.info, 0, sizeof( s_profileState.info ) );
    Com_Memset( &s_profileVehicleUsage, 0, sizeof( s_profileVehicleUsage ) );

    s_profileState.stats.distanceKm = G_Profile_ParseDouble( buffer, "distanceKm", 0.0 );
    s_profileState.stats.fuelUsed = G_Profile_ParseDouble( buffer, "fuelUsed", 0.0 );
    s_profileState.stats.bestLapMs = G_Profile_ParseInt( buffer, "bestLapMs", 0 );
    s_profileState.stats.kills = G_Profile_ParseInt( buffer, "kills", 0 );
    s_profileState.stats.deaths = G_Profile_ParseInt( buffer, "deaths", 0 );
    s_profileState.stats.wins = G_Profile_ParseInt( buffer, "wins", 0 );
    s_profileState.stats.losses = G_Profile_ParseInt( buffer, "losses", 0 );
    s_profileState.stats.flagCaptures = G_Profile_ParseInt( buffer, "flagCaptures", 0 );
    s_profileState.stats.flagAssists = G_Profile_ParseInt( buffer, "flagAssists", 0 );
    s_profileState.stats.accuracyAwards = G_Profile_ParseInt( buffer, "accuracyAwards", 0 );
    s_profileState.stats.excellentAwards = G_Profile_ParseInt( buffer, "excellentAwards", 0 );
    s_profileState.stats.impressiveAwards = G_Profile_ParseInt( buffer, "impressiveAwards", 0 );
    s_profileState.stats.perfectAwards = G_Profile_ParseInt( buffer, "perfectAwards", 0 );
    s_profileState.stats.topSpeedKph = G_Profile_ParseDouble( buffer, "topSpeedKph", 0.0 );
    s_profileState.stats.damageDealt = G_Profile_ParseInt( buffer, "damageDealt", 0 );
    s_profileState.stats.damageTaken = G_Profile_ParseInt( buffer, "damageTaken", 0 );
    G_Profile_ParseString( buffer, "mostUsedVehicle", s_profileState.stats.mostUsedVehicle, sizeof( s_profileState.stats.mostUsedVehicle ), "" );
    s_profileState.stats.mostUsedVehicleTimeMs = G_Profile_ParseInt( buffer, "mostUsedVehicleTimeMs", 0 );

    G_Profile_ParseString( buffer, "gender", s_profileState.info.gender, sizeof( s_profileState.info.gender ), "" );
    G_Profile_ParseString( buffer, "birthDate", s_profileState.info.birthDate, sizeof( s_profileState.info.birthDate ), "" );
    G_Profile_ParseString( buffer, "avatar", s_profileState.info.avatar, sizeof( s_profileState.info.avatar ), "" );
    G_Profile_ParseString( buffer, "country", s_profileState.info.country, sizeof( s_profileState.info.country ), "" );
    G_Profile_ParseFavoriteCars( buffer, &s_profileState.info );

    // Lade Vehicle-Array aus JSON
    vehiclesStart = strstr( buffer, "\"vehicles\"" );
    if ( vehiclesStart ) {
        vehiclesStart = strchr( vehiclesStart, '[' );
        if ( vehiclesStart ) {
            int vehicleIndex = 0;
            cursor = vehiclesStart + 1;
            
            while ( *cursor && *cursor != ']' && vehicleIndex < PROFILE_MAX_TRACKED_VEHICLES ) {
                const char *nameStart;
                const char *nameEnd;
                const char *timeStr;
                char vehicleName[PROFILE_MAX_VEHICLE];
                int vehicleTime;
                int nameLength;
                
                // Finde "name"
                cursor = strstr( cursor, "\"name\"" );
                if ( !cursor ) break;
                
                cursor = strchr( cursor, ':' );
                if ( !cursor ) break;
                cursor++;
                
                while ( *cursor == ' ' || *cursor == '\t' ) cursor++;
                if ( *cursor != '"' ) break;
                cursor++;
                
                nameStart = cursor;
                nameEnd = strchr( cursor, '"' );
                if ( !nameEnd ) break;
                
                nameLength = nameEnd - nameStart;
                if ( nameLength >= PROFILE_MAX_VEHICLE ) nameLength = PROFILE_MAX_VEHICLE - 1;
                Com_Memcpy( vehicleName, nameStart, nameLength );
                vehicleName[nameLength] = '\0';
                
                // Finde "timeMs"
                cursor = strstr( nameEnd, "\"timeMs\"" );
                if ( !cursor ) break;
                
                cursor = strchr( cursor, ':' );
                if ( !cursor ) break;
                cursor++;
                
                while ( *cursor == ' ' || *cursor == '\t' ) cursor++;
                timeStr = cursor;
                vehicleTime = atoi( timeStr );
                
                // Speichere im Array
                Q_strncpyz( s_profileVehicleUsage[vehicleIndex].name, vehicleName, sizeof( s_profileVehicleUsage[vehicleIndex].name ) );
                s_profileVehicleUsage[vehicleIndex].timeMs = vehicleTime;
                
                vehicleIndex++;
                
                // Nächstes Objekt
                cursor = strchr( cursor, '}' );
                if ( !cursor ) break;
                cursor++;
            }
        }
    }

    // Falls keine Vehicle-Liste vorhanden (Legacy), lade das mostUsedVehicle-Feld
    if ( s_profileState.stats.mostUsedVehicle[0] && s_profileVehicleUsage[0].name[0] == '\0' ) {
        Q_strncpyz( s_profileVehicleUsage[0].name, s_profileState.stats.mostUsedVehicle, sizeof( s_profileVehicleUsage[0].name ) );
        s_profileVehicleUsage[0].timeMs = s_profileState.stats.mostUsedVehicleTimeMs;
    }

    return qtrue;
}

static void G_Profile_WriteToDisk( void ) {
    fileHandle_t file;
    char path[MAX_QPATH];
    char buffer[4096];
    char gender[PROFILE_MAX_GENDER * 2];
    char birthDate[PROFILE_MAX_BIRTHDATE * 2];
    char avatar[PROFILE_MAX_AVATAR * 2];
    char country[PROFILE_MAX_COUNTRY * 2];
    char vehicleJson[1024];
    char favoriteCarsJson[1024];
    char favoriteField[PROFILE_MAX_FAVORITE_FIELD * 2];
    fileHandle_t readFile;
    char readBuffer[1024];
    int readLength;
    int length;
    int i;
    int vehicleJsonPos;

    if ( !s_profileState.loaded || !s_profileState.name[0] ) {
        return;
    }

    G_Profile_BuildPath( s_profileState.name, path, sizeof( path ) );

    // Lese bestehende Info-Felder aus der Datei
    readLength = trap_FS_FOpenFile( path, &readFile, FS_READ );
    if ( readLength > 0 ) {
        if ( readLength >= (int)sizeof( readBuffer ) ) {
            readLength = sizeof( readBuffer ) - 1;
        }

        trap_FS_Read( readBuffer, readLength, readFile );
        readBuffer[readLength] = '\0';
        trap_FS_FCloseFile( readFile );

        G_Profile_ParseString( readBuffer, "gender", s_profileState.info.gender, sizeof( s_profileState.info.gender ), s_profileState.info.gender );
        G_Profile_ParseString( readBuffer, "birthDate", s_profileState.info.birthDate, sizeof( s_profileState.info.birthDate ), s_profileState.info.birthDate );
        G_Profile_ParseString( readBuffer, "avatar", s_profileState.info.avatar, sizeof( s_profileState.info.avatar ), s_profileState.info.avatar );
        G_Profile_ParseString( readBuffer, "country", s_profileState.info.country, sizeof( s_profileState.info.country ), s_profileState.info.country );
    } else if ( readFile ) {
        trap_FS_FCloseFile( readFile );
    }

    // Escape Sonderzeichen für JSON
    G_Profile_FormatJsonString( gender, sizeof( gender ), s_profileState.info.gender );
    G_Profile_FormatJsonString( birthDate, sizeof( birthDate ), s_profileState.info.birthDate );
    G_Profile_FormatJsonString( avatar, sizeof( avatar ), s_profileState.info.avatar );
    G_Profile_FormatJsonString( country, sizeof( country ), s_profileState.info.country );

    // Build favorite cars JSON
    vehicleJsonPos = 0;
    vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos, "[\n" );

    for ( i = 0; i < PROFILE_MAX_FAVORITE_CARS; ++i ) {
        if ( i > 0 ) {
            vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos, ",\n" );
        }

        G_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), s_profileState.info.favoriteCars[i].model );
        vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos,
                                      "\t\t\t{\"model\": \"%s\", ", favoriteField );

        G_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), s_profileState.info.favoriteCars[i].skin );
        vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos,
                                      "\"skin\": \"%s\", ", favoriteField );

        G_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), s_profileState.info.favoriteCars[i].rim );
        vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos,
                                      "\"rim\": \"%s\", ", favoriteField );

        G_Profile_FormatJsonString( favoriteField, sizeof( favoriteField ), s_profileState.info.favoriteCars[i].head );
        vehicleJsonPos += Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos,
                                      "\"head\": \"%s\"}", favoriteField );
    }

    Com_sprintf( favoriteCarsJson + vehicleJsonPos, sizeof( favoriteCarsJson ) - vehicleJsonPos, "\n\t\t]" );

    // Baue Fahrzeug-Array für JSON - manuell ohne Q_strcat
    vehicleJsonPos = 0;
    vehicleJsonPos += Com_sprintf( vehicleJson + vehicleJsonPos, sizeof( vehicleJson ) - vehicleJsonPos, "[\n" );
    
    for ( i = 0; i < PROFILE_MAX_TRACKED_VEHICLES; ++i ) {
        if ( !s_profileVehicleUsage[i].name[0] ) {
            continue;
        }
        
        // Füge Komma hinzu, wenn nicht das erste Element
        if ( vehicleJsonPos > 2 ) {  // mehr als nur "[\n"
            vehicleJsonPos += Com_sprintf( vehicleJson + vehicleJsonPos, sizeof( vehicleJson ) - vehicleJsonPos, ",\n" );
        }
        
        vehicleJsonPos += Com_sprintf( vehicleJson + vehicleJsonPos, sizeof( vehicleJson ) - vehicleJsonPos, 
                                      "\t\t\t{\"name\": \"%s\", \"timeMs\": %d}", 
                                      s_profileVehicleUsage[i].name, 
                                      s_profileVehicleUsage[i].timeMs );
    }
    
    Com_sprintf( vehicleJson + vehicleJsonPos, sizeof( vehicleJson ) - vehicleJsonPos, "\n\t\t]" );

    // Debug-Ausgabe
    Com_Printf( "G_Profile: Writing vehicles array:\n%s\n", vehicleJson );
    Com_Printf( "G_Profile: mostUsedVehicle='%s', timeMs=%d\n", 
               s_profileState.stats.mostUsedVehicle, 
               s_profileState.stats.mostUsedVehicleTimeMs );

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
        "\t\t\"mostUsedVehicleTimeMs\": %d,\n"
        "\t\t\"vehicles\": %s\n"
        "\t}\n"
        "}\n",
        s_profileState.name,
        gender,
        birthDate,
        avatar,
        country,
        favoriteCarsJson,
        s_profileState.stats.distanceKm,
        s_profileState.stats.fuelUsed,
        s_profileState.stats.bestLapMs,
        s_profileState.stats.kills,
        s_profileState.stats.deaths,
        s_profileState.stats.wins,
        s_profileState.stats.losses,
        s_profileState.stats.flagCaptures,
        s_profileState.stats.flagAssists,
        s_profileState.stats.accuracyAwards,
        s_profileState.stats.excellentAwards,
        s_profileState.stats.impressiveAwards,
        s_profileState.stats.perfectAwards,
        s_profileState.stats.topSpeedKph,
        s_profileState.stats.damageDealt,
        s_profileState.stats.damageTaken,
        s_profileState.stats.mostUsedVehicle,
        s_profileState.stats.mostUsedVehicleTimeMs,
        vehicleJson );

    if ( length < 0 ) {
        Com_Printf( "G_Profile: Com_sprintf failed\n" );
        return;
    }

    // Debug-Ausgabe des gesamten Buffers
    Com_Printf( "G_Profile: Writing profile file:\n%s\n", buffer );

    trap_FS_FOpenFile( path, &file, FS_WRITE );
    if ( file <= 0 ) {
        Com_Printf( "G_Profile: Failed to open %s for writing\n", path );
        return;
    }

    trap_FS_Write( buffer, length, file );
    trap_FS_FCloseFile( file );

    Com_Printf( "G_Profile: Successfully wrote %d bytes to %s\n", length, path );

    s_profileState.dirty = qfalse;
    s_profileState.nextAutosaveTime = level.time + PROFILE_AUTOSAVE_INTERVAL;
}

static void G_Profile_ClearState( void ) {
    Com_Memset( &s_profileState, 0, sizeof( s_profileState ) );
    Com_Memset( &s_profileVehicleUsage, 0, sizeof( s_profileVehicleUsage ) );
}

void G_Profile_Init( void ) {
    char activeName[PROFILE_MAX_NAME];

    G_Profile_ClearState();

    trap_Cvar_VariableStringBuffer( "profile_active", activeName, sizeof( activeName ) );
    if ( !activeName[0] ) {
        return;
    }

    if ( !G_Profile_IsValidName( activeName ) ) {
        Com_Printf( "G_Profile: Ignoring invalid profile name '%s'\n", activeName );
        return;
    }

    Q_strncpyz( s_profileState.name, activeName, sizeof( s_profileState.name ) );
    if ( !G_Profile_LoadFromDisk() ) {
        Com_Memset( &s_profileState.stats, 0, sizeof( s_profileState.stats ) );
        Com_Memset( &s_profileState.info, 0, sizeof( s_profileState.info ) );
        s_profileState.dirty = qtrue;
        G_Profile_WriteToDisk();
    }

    s_profileState.loaded = qtrue;
    s_profileState.nextAutosaveTime = level.time + PROFILE_AUTOSAVE_INTERVAL;
}

void G_Profile_Shutdown( void ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( s_profileState.dirty ) {
        G_Profile_WriteToDisk();
    }

    G_Profile_ClearState();
}

void G_Profile_RecordDamage( gclient_t *attacker, gclient_t *victim, int damage ) {
    if ( damage <= 0 || !s_profileState.loaded ) {
        return;
    }

    if ( attacker && attacker->pers.localClient && attacker != victim ) {
        s_profileState.stats.damageDealt += damage;
        s_profileState.dirty = qtrue;
    }

    if ( victim && victim->pers.localClient ) {
        s_profileState.stats.damageTaken += damage;
        s_profileState.dirty = qtrue;
    }
}

void G_Profile_TrackClientSpawn( gclient_t *client ) {
    if ( !client ) {
        return;
    }

    client->profileHasLastOrigin = qfalse;
    client->profileLastTime = 0;
}

void G_Profile_UpdateClientFrame( gentity_t *ent ) {
    gclient_t *client;
    vec3_t delta;
    double distanceQu;
    double distanceKm;
    double speedQu;
    double speedKph;
    int frameMsec;
    int currentTime;

    if ( !s_profileState.loaded || !s_profileState.name[0] ) {
        return;
    }

    if ( !ent || !( client = ent->client ) ) {
        return;
    }

    if ( ent->r.svFlags & SVF_BOT || client->pers.connected != CON_CONNECTED ) {
        return;
    }

    if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
        return;
    }

    // Zeit-Tracking: Messe tatsächlich vergangene Zeit seit letztem Frame
    currentTime = level.time;
    
    if ( client->profileLastTime == 0 ) {
        // Erster Frame nach Spawn - initialisiere nur, addiere keine Zeit
        client->profileLastTime = currentTime;
        frameMsec = 0;
    } else {
        frameMsec = currentTime - client->profileLastTime;
        
        // Sicherheitscheck: Verhindere negative oder unrealistisch große Werte
        if ( frameMsec < 0 || frameMsec > 1000 ) {
            frameMsec = 0;
        }
        
        client->profileLastTime = currentTime;
    }

    // Distanz-Tracking
    if ( !client->profileHasLastOrigin ) {
        VectorCopy( ent->r.currentOrigin, client->profileLastOrigin );
        client->profileHasLastOrigin = qtrue;
    } else {
        VectorSubtract( ent->r.currentOrigin, client->profileLastOrigin, delta );
        distanceQu = VectorLength( delta );
        if ( distanceQu > 0.0 ) {
            distanceKm = distanceQu / CP_M_2_QU / 1000.0;
            s_profileState.stats.distanceKm += distanceKm;
            s_profileState.stats.fuelUsed += distanceKm * ( PROFILE_DISPLAY_L_PER_100KM / 100.0f );
            s_profileState.dirty = qtrue;
        }
        VectorCopy( ent->r.currentOrigin, client->profileLastOrigin );
    }

    G_Profile_UpdateVehicleUsage( ent, frameMsec );

    // Geschwindigkeits-Tracking
    speedQu = VectorLength( client->ps.velocity );
    speedKph = ( speedQu / CP_M_2_QU ) * 3.6;
    if ( speedKph > s_profileState.stats.topSpeedKph ) {
        s_profileState.stats.topSpeedKph = speedKph;
        s_profileState.dirty = qtrue;
    }

    // Auto-Save Check
    if ( s_profileState.dirty && level.time >= s_profileState.nextAutosaveTime ) {
        G_Profile_WriteToDisk();
    }
}

void G_Profile_RecordKill( gclient_t *attacker, gclient_t *victim ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !attacker || !attacker->pers.localClient ) {
        return;
    }

    if ( attacker == victim ) {
        return;
    }

    s_profileState.stats.kills++;
    s_profileState.dirty = qtrue;
}

void G_Profile_RecordDeath( gclient_t *victim ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !victim || !victim->pers.localClient ) {
        return;
    }

    s_profileState.stats.deaths++;
    s_profileState.dirty = qtrue;
}

void G_Profile_RecordFlagCapture( gclient_t *client ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !client || !client->pers.localClient ) {
        return;
    }

    s_profileState.stats.flagCaptures++;
    s_profileState.dirty = qtrue;
}

void G_Profile_RecordFlagAssist( gclient_t *client ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !client || !client->pers.localClient ) {
        return;
    }

    s_profileState.stats.flagAssists++;
    s_profileState.dirty = qtrue;
}

void G_Profile_RecordWin( gclient_t *client ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !client || !client->pers.localClient ) {
        return;
    }

    s_profileState.stats.wins++;
    s_profileState.dirty = qtrue;
}

void G_Profile_RecordLoss( gclient_t *client ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !client || !client->pers.localClient ) {
        return;
    }

    s_profileState.stats.losses++;
    s_profileState.dirty = qtrue;
}

void G_Profile_RecordBestLap( gclient_t *client, int lapTime ) {
    if ( !s_profileState.loaded ) {
        return;
    }

    if ( !client || !client->pers.localClient ) {
        return;
    }

    if ( lapTime <= 0 ) {
        return;
    }

    if ( s_profileState.stats.bestLapMs == 0 || lapTime < s_profileState.stats.bestLapMs ) {
        s_profileState.stats.bestLapMs = lapTime;
        s_profileState.dirty = qtrue;
    }
}

void G_Profile_RecordExcellent( gclient_t *client ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    s_profileState.stats.excellentAwards++;
    s_profileState.dirty = qtrue;
}

void G_Profile_RecordImpressive( gclient_t *client ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    s_profileState.stats.impressiveAwards++;
    s_profileState.dirty = qtrue;
}

void G_Profile_RecordAccuracy( gclient_t *client, int accuracyPercent ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    if ( accuracyPercent < 75 ) {
        return;
    }

    s_profileState.stats.accuracyAwards++;
    s_profileState.dirty = qtrue;
}

void G_Profile_RecordPerfect( gclient_t *client ) {
    if ( !G_Profile_ShouldTrackClient( client ) ) {
        return;
    }

    s_profileState.stats.perfectAwards++;
    s_profileState.dirty = qtrue;
}




