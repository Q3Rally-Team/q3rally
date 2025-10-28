#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "server.h"

cvar_t *sv_ladderUrl = NULL;
cvar_t *sv_ladderApiKey = NULL;
cvar_t *sv_ladderEnabled = NULL;
cvar_t *sv_telemetryMaxBatch = NULL;

#include "sv_ladder_for_test.c"

int main( void ) {
        ladderMatchPayload_t payload;
        char *json;
        size_t length;

        Com_Memset( &payload, 0, sizeof( payload ) );
        payload.valid = qtrue;
        payload.playerCount = 1;
        payload.players[0].clientNum = 7;
        payload.players[0].team = TEAM_RED;
        payload.players[0].lapCount = 2;
        payload.players[0].lapTimes[0] = 1234;
        payload.players[0].lapTimes[1] = 1100;
        payload.players[0].bestLapMs = 1100;
        Q_strncpyz( payload.players[0].name, "Tester", sizeof( payload.players[0].name ) );

        json = SV_LadderSerializeMatch( &payload, &length );
        assert( json != NULL );
        assert( strstr( json, "\"bestLapMs\":1100" ) != NULL );
        assert( strstr( json, "\"lapTimes\":[1234,1100]" ) != NULL );

        Z_Free( json );
        puts( "ok" );
        return 0;
}
