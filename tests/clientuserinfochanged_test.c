#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "g_client_userinfo.h"

static char g_configstring[MAX_INFO_STRING];
static int g_last_config_index;
static int g_set_call_count;

void trap_GetConfigstring( int num, char *buffer, int bufferSize ) {
    assert( num == CS_PLAYERS + g_last_config_index );
    (void)bufferSize;
    strncpy( buffer, g_configstring, (size_t)bufferSize );
    buffer[bufferSize - 1] = '\0';
}

void trap_SetConfigstring( int num, const char *string ) {
    assert( num == CS_PLAYERS + g_last_config_index );
    strncpy( g_configstring, string, sizeof( g_configstring ) );
    g_configstring[sizeof( g_configstring ) - 1] = '\0';
    g_set_call_count++;
}

int main( void ) {
    static const char first_value[] = "n\\Player\\t\\1";
    static const char second_value[] = "n\\Player\\t\\2";

    memset( g_configstring, 0, sizeof( g_configstring ) );
    g_last_config_index = 0;
    g_set_call_count = 0;

    assert( G_SetClientConfigstringIfChanged( g_last_config_index, first_value ) );
    assert( g_set_call_count == 1 );
    assert( strcmp( g_configstring, first_value ) == 0 );

    assert( !G_SetClientConfigstringIfChanged( g_last_config_index, first_value ) );
    assert( g_set_call_count == 1 );
    assert( strcmp( g_configstring, first_value ) == 0 );

    assert( G_SetClientConfigstringIfChanged( g_last_config_index, second_value ) );
    assert( g_set_call_count == 2 );
    assert( strcmp( g_configstring, second_value ) == 0 );

    puts( "ok" );
    return 0;
}
