/*
 * Racing line (cg_rally_racingline.c): resampling, ground trace, colours
 * from inputs and from the speed change, resets, source choice, circuit
 * wrap-around, drawing range and the server route request.
 */
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../engine/code/cgame/cg_rally_racingline.c"

cg_t cg;
cgs_t cgs;
vmCvar_t cg_developer;
vmCvar_t cg_racingLine;
vmCvar_t cg_racingLineDistance;

static int s_sprint;
static int s_race = 1;
static int s_routeRequests;
static int s_personalLoads;
static int s_polys;
static int s_polyCalls;
static int s_groundZ = 0;

void QDECL Com_Error( int level, const char *fmt, ... ) { (void)level; (void)fmt; assert( 0 ); }
void QDECL Com_Printf( const char *fmt, ... ) { (void)fmt; }
void QDECL CG_Printf( const char *fmt, ... ) { (void)fmt; }
qboolean CG_IsSprintTrack( void ) { return s_sprint; }
qboolean isRallyRace( void ) { return s_race; }
void CG_LoadPersonalGhost( void ) { s_personalLoads++; cg.personalGhostSearchValid = qtrue; }
void CG_GhostRoute_Request( void ) { s_routeRequests++; }
qhandle_t trap_R_RegisterShader( const char *name ) { (void)name; return 7; }
void trap_R_AddPolysToScene( qhandle_t shader, int numVerts, const polyVert_t *verts, int numPolys ) {
	(void)verts;
	assert( shader == 7 && numVerts == 4 );
	s_polys += numPolys;
	s_polyCalls++;
}
/* flat road at z = s_groundZ */
void trap_CM_BoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
		const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask ) {
	(void)mins; (void)maxs; (void)model; (void)brushmask;
	memset( results, 0, sizeof( *results ) );
	results->fraction = 1.0f;
	if ( start[2] >= s_groundZ && end[2] <= s_groundZ ) {
		results->fraction = ( start[2] - s_groundZ ) / ( start[2] - end[2] );
		VectorCopy( start, results->endpos );
		results->endpos[2] = (float)s_groundZ;
		VectorSet( results->plane.normal, 0, 0, 1 );
	}
}

static void AddFrame( ghostRecording_t *r, int t, float x, float y, int fm ) {
	ghostFrame_t *f = &r->frames[r->frameCount++];
	memset( f, 0, sizeof( *f ) );
	f->timeOffset = t;
	VectorSet( f->origin, x, y, 40.0f );
	f->forwardmove = fm;
	r->duration = t;
	r->valid = qtrue;
}

/* 0-3000 throttle, 3000-4000 coast, 4000-5000 brake, rest throttle */
static void StraightWithInputs( ghostRecording_t *r ) {
	int t;
	memset( r, 0, sizeof( *r ) );
	for ( t = 0; t <= 8000; t += 50 ) {
		int fm = 127;
		if ( t >= 3000 && t < 4000 ) fm = 0;
		else if ( t >= 4000 && t < 5000 ) fm = -127;
		AddFrame( r, t, t * 1.0f, 0.0f, fm );	// 1000 u/s
	}
}

static int KindAtX( float x ) {
	int i, best = 0;
	for ( i = 0; i < rl.count; i++ ) {
		if ( fabs( rl.points[i].origin[0] - x ) < fabs( rl.points[best].origin[0] - x ) ) best = i;
	}
	return rl.points[best].kind;
}

static void TestResampleGroundInputs( void ) {
	static ghostRecording_t r;
	int i;

	StraightWithInputs( &r );
	assert( RL_Build( &r ) );
	assert( rl.count > 140 && rl.count < 152 );	// 8000 units / 54
	for ( i = 1; i < rl.count - 1; i++ ) {
		float d = rl.points[i].origin[0] - rl.points[i - 1].origin[0];
		assert( fabs( d - RL_SPACING ) < 0.01f );
		assert( fabs( rl.points[i].origin[2] - RL_LIFT ) < 0.01f );	// dropped onto the road
		assert( !rl.points[i].breakBefore );
		/* band across the line: +-y */
		assert( fabs( fabs( rl.points[i].right[1] ) - RL_HALF_WIDTH ) < 0.01f );
	}
	assert( KindAtX( 1500 ) == RL_THROTTLE );
	assert( KindAtX( 3500 ) == RL_LIFT_OFF );
	assert( KindAtX( 4500 ) == RL_BRAKE );
	assert( KindAtX( 6500 ) == RL_THROTTLE );
	assert( !rl.loop );
}

static void TestSpeedColours( void ) {
	static ghostRecording_t r;
	float x = 0.0f, v = 600.0f;
	int t;

	/* no inputs (server route): accelerate, hold, lift (-200), brake (-500), hold */
	memset( &r, 0, sizeof( r ) );
	for ( t = 0; t <= 10000; t += 50 ) {
		float a = 0.0f;
		if ( t < 2000 ) a = 300.0f;
		else if ( t < 4000 ) a = 0.0f;
		else if ( t < 6000 ) a = -200.0f;
		else if ( t < 7000 ) a = -500.0f;
		AddFrame( &r, t, x, 0.0f, 0 );
		v += a * 0.05f;
		x += v * 0.05f;
	}
	assert( !RL_HasInputs( &r ) );
	assert( RL_Build( &r ) );
	{
		int i, seenLift = 0, seenBrake = 0, brakeAfterSix = 1;
		for ( i = 0; i < rl.count; i++ ) {
			float tm = rl.points[i].time;
			if ( tm > 500 && tm < 1800 ) assert( rl.points[i].kind == RL_THROTTLE );
			if ( tm > 2400 && tm < 3600 ) assert( rl.points[i].kind == RL_THROTTLE );
			if ( tm > 4400 && tm < 5600 ) { assert( rl.points[i].kind == RL_LIFT_OFF ); seenLift = 1; }
			if ( tm > 6300 && tm < 6700 ) { assert( rl.points[i].kind == RL_BRAKE ); seenBrake = 1; }
			if ( tm > 7500 && tm < 9500 ) assert( rl.points[i].kind == RL_THROTTLE );
			(void)brakeAfterSix;
		}
		assert( seenLift && seenBrake );
	}
}

static void TestTeleportBreak( void ) {
	static ghostRecording_t r;
	int t, breaks = 0, i;

	memset( &r, 0, sizeof( r ) );
	for ( t = 0; t <= 2000; t += 50 ) AddFrame( &r, t, t * 1.0f, 0.0f, 127 );
	for ( t = 2050; t <= 4000; t += 50 ) AddFrame( &r, t, t * 1.0f, 5000.0f, 127 );	// reset: jump sideways
	assert( RL_Build( &r ) );
	for ( i = 0; i < rl.count; i++ ) breaks += rl.points[i].breakBefore;
	assert( breaks == 1 );
}

static void SetupCar( float x, float y ) {
	static snapshot_t snap;
	memset( &snap, 0, sizeof( snap ) );
	cg.snap = &snap;
	memset( &cg.predictedPlayerState, 0, sizeof( cg.predictedPlayerState ) );
	cg.predictedPlayerState.pm_type = PM_NORMAL;
	cg.predictedPlayerState.persistant[PERS_TEAM] = TEAM_FREE;
	VectorSet( cg.predictedPlayerState.origin, x, y, 40.0f );
}

static void TestSourceAndDraw( void ) {
	int t;

	CG_RacingLine_Reset();
	CG_RacingLine_Register();
	memset( &cg.ghostPlayback, 0, sizeof( cg.ghostPlayback ) );
	memset( &cg.baseGhost, 0, sizeof( cg.baseGhost ) );
	StraightWithInputs( &cg.ghostPlayback );
	cg.personalGhostAvailable = qtrue;
	cg.personalGhostBestTime = 8000;
	/* server route: same road, 2000 units further left, slower */
	for ( t = 0; t <= 9000; t += 100 ) AddFrame( &cg.baseGhost, t, t * 0.9f, 2000.0f, 0 );
	cg.baseGhostAvailable = qtrue;
	cg.baseGhostBestTime = 9000;
	cg.baseGhostStatusKnown = qtrue;

	cgs.gametype = GT_RACING;
	cg_racingLine.integer = 0;
	SetupCar( 1000, 0 );
	s_polys = 0;
	CG_AddRacingLine();
	assert( s_polys == 0 && s_routeRequests == 0 );	// off

	cg_racingLine.integer = 1;
	cg_racingLineDistance.value = 50.0f;	// metres
	CG_AddRacingLine();
	assert( rl.source == RL_SOURCE_PERSONAL );
	/* about 50 m ahead + 4 m behind at 1.5 m spacing */
	assert( s_polys > 30 && s_polys < 40 );
	assert( s_routeRequests == 0 );	// route status already known

	/* a faster server route wins */
	cg.baseGhostBestTime = 7000;
	s_polys = 0;
	CG_AddRacingLine();
	assert( rl.source == RL_SOURCE_BASE );
	assert( s_polys > 0 );

	/* spectators and intermission: nothing */
	s_polys = 0;
	cg.predictedPlayerState.persistant[PERS_TEAM] = TEAM_SPECTATOR;
	CG_AddRacingLine();
	assert( s_polys == 0 );
	SetupCar( 1000, 0 );
	cg.predictedPlayerState.pm_type = PM_INTERMISSION;
	CG_AddRacingLine();
	assert( s_polys == 0 );

	/* not a race: nothing */
	SetupCar( 1000, 0 );
	s_race = 0;
	CG_AddRacingLine();
	assert( s_polys == 0 );
	s_race = 1;
}

static void TestRouteRequest( void ) {
	CG_RacingLine_Reset();
	memset( &cg.ghostPlayback, 0, sizeof( cg.ghostPlayback ) );
	memset( &cg.baseGhost, 0, sizeof( cg.baseGhost ) );
	cg.personalGhostAvailable = qfalse;
	cg.personalGhostSearchValid = qfalse;
	cg.baseGhostAvailable = qfalse;
	cg.baseGhostStatusKnown = qfalse;
	cg.baseGhostTransferPending = qfalse;
	cg_racingLine.integer = 1;
	SetupCar( 0, 0 );
	s_routeRequests = 0;
	s_personalLoads = 0;

	cgs.gametype = GT_RACING;
	CG_AddRacingLine();
	CG_AddRacingLine();
	assert( s_routeRequests == 1 );	// once per map
	assert( s_personalLoads == 1 );

	/* Ghost Race: the server sends the route on its own */
	CG_RacingLine_Reset();
	s_routeRequests = 0;
	cgs.gametype = GT_GHOST;
	CG_AddRacingLine();
	assert( s_routeRequests == 0 );
}

static void TestCircuitWrap( void ) {
	static ghostRecording_t r;
	int t;
	const float radius = 3000.0f;

	/* circle of ~18850 units, one lap in 18850 ms */
	CG_RacingLine_Reset();
	memset( &cg.ghostPlayback, 0, sizeof( cg.ghostPlayback ) );
	memset( &cg.baseGhost, 0, sizeof( cg.baseGhost ) );
	cg.baseGhostAvailable = qfalse;
	cg.baseGhostStatusKnown = qtrue;
	for ( t = 0; t <= 18850; t += 50 ) {
		float ang = (float)t / 18850.0f * 2.0f * (float)M_PI;
		AddFrame( &cg.ghostPlayback, t, radius * cosf( ang ), radius * sinf( ang ), 127 );
	}
	(void)r;
	cg.personalGhostAvailable = qtrue;
	cg.personalGhostSearchValid = qtrue;
	cg.personalGhostBestTime = 18850;
	cgs.gametype = GT_RACING;
	s_sprint = 0;
	cg_racingLine.integer = 1;
	cg_racingLineDistance.value = 100.0f;

	/* car just before the finish line: the line continues into the next lap */
	SetupCar( radius * cosf( -0.05f ), radius * sinf( -0.05f ) );
	s_polys = 0;
	CG_AddRacingLine();
	assert( rl.loop );
	assert( rl.nearest > rl.count - 10 );
	assert( s_polys > 60 && s_polys < 75 );	// 100 m + 4 m behind

	/* sprint track: no wrap-around past the finish */
	s_sprint = 1;
	cg.personalGhostBestTime = 18851;	// force a rebuild
	s_polys = 0;
	CG_AddRacingLine();
	assert( !rl.loop );
	assert( s_polys < 10 );
	s_sprint = 0;
}

int main( void ) {
	TestResampleGroundInputs();
	TestSpeedColours();
	TestTeleportBreak();
	TestSourceAndDraw();
	TestRouteRequest();
	TestCircuitWrap();
	printf( "ok\n" );
	return 0;
}
