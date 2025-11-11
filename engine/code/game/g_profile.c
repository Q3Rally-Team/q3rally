#include "g_local.h"
#include "g_profile.h"

#define PROFILE_AUTOSAVE_INTERVAL 30000

static struct {
    qboolean        loaded;
    qboolean        dirty;
    char            name[PROFILE_MAX_NAME];
    profile_stats_t stats;
    int             nextAutosaveTime;
} s_profileState;

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

static qboolean G_Profile_LoadFromDisk( void ) {
    fileHandle_t file;
    char path[MAX_QPATH];
    char buffer[1024];
    int length;

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

    s_profileState.stats.distanceKm = G_Profile_ParseDouble( buffer, "distanceKm", 0.0 );
    s_profileState.stats.fuelUsed = G_Profile_ParseDouble( buffer, "fuelUsed", 0.0 );
    s_profileState.stats.bestLapMs = G_Profile_ParseInt( buffer, "bestLapMs", 0 );
    s_profileState.stats.kills = G_Profile_ParseInt( buffer, "kills", 0 );
    s_profileState.stats.deaths = G_Profile_ParseInt( buffer, "deaths", 0 );
    s_profileState.stats.wins = G_Profile_ParseInt( buffer, "wins", 0 );
    s_profileState.stats.losses = G_Profile_ParseInt( buffer, "losses", 0 );
    s_profileState.stats.flagCaptures = G_Profile_ParseInt( buffer, "flagCaptures", 0 );

    return qtrue;
}

static void G_Profile_WriteToDisk( void ) {
    fileHandle_t file;
    char path[MAX_QPATH];
    char buffer[512];
    int length;

    if ( !s_profileState.loaded || !s_profileState.name[0] ) {
        return;
    }

    G_Profile_BuildPath( s_profileState.name, path, sizeof( path ) );

    length = Com_sprintf( buffer, sizeof( buffer ),
        "{\n"
        "\t\"name\": \"%s\",\n"
        "\t\"stats\": {\n"
        "\t\t\"distanceKm\": %.6f,\n"
        "\t\t\"fuelUsed\": %.3f,\n"
        "\t\t\"bestLapMs\": %d,\n"
        "\t\t\"kills\": %d,\n"
        "\t\t\"deaths\": %d,\n"
        "\t\t\"wins\": %d,\n"
        "\t\t\"losses\": %d,\n"
        "\t\t\"flagCaptures\": %d\n"
        "\t}\n"
        "}\n",
        s_profileState.name,
        s_profileState.stats.distanceKm,
        s_profileState.stats.fuelUsed,
        s_profileState.stats.bestLapMs,
        s_profileState.stats.kills,
        s_profileState.stats.deaths,
        s_profileState.stats.wins,
        s_profileState.stats.losses,
        s_profileState.stats.flagCaptures );

    if ( length < 0 ) {
        return;
    }

    trap_FS_FOpenFile( path, &file, FS_WRITE );
    if ( file <= 0 ) {
        Com_Printf( "G_Profile: Failed to open %s for writing\n", path );
        return;
    }

    trap_FS_Write( buffer, length, file );
    trap_FS_FCloseFile( file );

    s_profileState.dirty = qfalse;
    s_profileState.nextAutosaveTime = level.time + PROFILE_AUTOSAVE_INTERVAL;
}

static void G_Profile_ClearState( void ) {
    Com_Memset( &s_profileState, 0, sizeof( s_profileState ) );
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

void G_Profile_TrackClientSpawn( gclient_t *client ) {
    if ( !client ) {
        return;
    }

    client->profileHasLastOrigin = qfalse;
    client->profileLastFuel = client->car.fuel;
}

void G_Profile_UpdateClientFrame( gentity_t *ent ) {
    gclient_t *client;
    vec3_t delta;
    double distanceQu;

    if ( !s_profileState.loaded || !s_profileState.name[0] ) {
        return;
    }

    if ( !ent || !( client = ent->client ) ) {
        return;
    }

    if ( !client->pers.localClient ) {
        return;
    }

    if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
        return;
    }

    if ( !client->profileHasLastOrigin ) {
        VectorCopy( ent->r.currentOrigin, client->profileLastOrigin );
        client->profileHasLastOrigin = qtrue;
    } else {
        VectorSubtract( ent->r.currentOrigin, client->profileLastOrigin, delta );
        distanceQu = VectorLength( delta );
        if ( distanceQu > 0.0 ) {
            s_profileState.stats.distanceKm += distanceQu / CP_M_2_QU / 1000.0;
            s_profileState.dirty = qtrue;
        }
        VectorCopy( ent->r.currentOrigin, client->profileLastOrigin );
    }

    if ( client->profileLastFuel <= 0.0f ) {
        client->profileLastFuel = client->car.fuel;
    } else {
        float diff = client->profileLastFuel - client->car.fuel;
        if ( diff > 0.0f ) {
            s_profileState.stats.fuelUsed += diff;
            s_profileState.dirty = qtrue;
        }
        client->profileLastFuel = client->car.fuel;
    }

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
