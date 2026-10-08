/*
 * Online version check helpers (client/cl_update_util.h): version
 * comparison and URL validation.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "q_shared.h"
#include "../engine/code/client/cl_update_util.h"

void QDECL Com_Error( int level, const char *fmt, ... ) {
	(void)level;
	(void)fmt;
	assert( 0 );
}

void QDECL Com_Printf( const char *fmt, ... ) {
	(void)fmt;
}

static int Sign( int v ) {
	return ( v > 0 ) - ( v < 0 );
}

static void ExpectCompare( const char *a, const char *b, int expected ) {
	int got = Sign( CL_UpdateVersion_Compare( a, b ) );
	int back = Sign( CL_UpdateVersion_Compare( b, a ) );
	if ( got != expected || back != -expected ) {
		fprintf( stderr, "FAIL compare(%s, %s) = %d / reverse %d, expected %d\n", a, b, got, back, expected );
		assert( 0 );
	}
}

int main( void ) {
	/* equal, prefix and build metadata are ignored */
	ExpectCompare( "v0.7d", "v0.7d", 0 );
	ExpectCompare( "v0.7d", "0.7d", 0 );
	ExpectCompare( "V0.7D", "v0.7d", 0 );
	ExpectCompare( "v0.7d+20261004+abc1234", "v0.7d", 0 );
	ExpectCompare( "v0.7d_IOQ3+r3456", "v0.7d", 0 );
	ExpectCompare( "0.7", "0.7.0", 0 );
	ExpectCompare( " v0.7d ", "v0.7d", 0 );

	/* ordering */
	ExpectCompare( "v0.7d", "v0.7e", -1 );
	ExpectCompare( "v0.7", "v0.7d", -1 );
	ExpectCompare( "v0.7d", "v0.8", -1 );
	ExpectCompare( "v0.9", "v0.10", -1 );
	ExpectCompare( "v0.7d", "v1.0.15", -1 );
	ExpectCompare( "v1.0.9", "v1.0.15", -1 );
	ExpectCompare( "v0.8-rc1", "v0.8", -1 );
	ExpectCompare( "v0.8-rc1", "v0.8-rc2", -1 );
	ExpectCompare( "v0.8-rc2", "v0.8-rc10", -1 );
	ExpectCompare( "v0.7z", "v0.8-rc1", -1 );
	ExpectCompare( "", "v0.1", -1 );

	/* safe URLs */
	assert( CL_IsSafeURL( "https://www.q3rally.com/downloads" ) );
	assert( CL_IsSafeURL( "http://example.com/a?b=c&d=%20" ) );
	assert( !CL_IsSafeURL( "" ) );
	assert( !CL_IsSafeURL( NULL ) );
	assert( !CL_IsSafeURL( "ftp://www.q3rally.com/" ) );
	assert( !CL_IsSafeURL( "file:///etc/passwd" ) );
	assert( !CL_IsSafeURL( "https://x.com/\"; rm -rf ~; \"" ) );
	assert( !CL_IsSafeURL( "https://x.com/$(id)" ) );
	assert( !CL_IsSafeURL( "https://x.com/`id`" ) );
	assert( !CL_IsSafeURL( "https://x.com/a b" ) );
	assert( !CL_IsSafeURL( "https://x.com/a\nb" ) );
	assert( !CL_IsSafeURL( "https://x.com/a\\b" ) );
	assert( !CL_IsSafeURL( "https://x.com/'a'" ) );

	/* official download URLs */
	assert( CL_IsOfficialDownloadURL( "https://www.q3rally.com/downloads" ) );
	assert( CL_IsOfficialDownloadURL( "https://q3rally.com/downloads" ) );
	assert( !CL_IsOfficialDownloadURL( "http://www.q3rally.com/downloads" ) );
	assert( !CL_IsOfficialDownloadURL( "https://www.q3rally.com.evil.example/" ) );
	assert( !CL_IsOfficialDownloadURL( "https://evil.example/www.q3rally.com/" ) );
	assert( !CL_IsOfficialDownloadURL( "https://www.q3rally.com" ) );

	printf( "ok\n" );
	return 0;
}
