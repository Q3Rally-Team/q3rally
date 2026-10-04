/*
 * Ghost Race ladder ghost picker (cg_ghost_picker.c): list and ghost
 * transfer parsing, own-vehicle filter, picking, remembered pick and keys.
 */
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../engine/code/cgame/cg_ghost_picker.c"

cg_t cg;
cgs_t cgs;
centity_t cg_entities[MAX_GENTITIES];
vmCvar_t cg_developer;
vmCvar_t cg_ghostPlayback;
vmCvar_t cg_centertime;
static float s_fadeColor[4] = { 1, 1, 1, 1 };
float *CG_FadeColor( int startMsec, int totalMsec ) {
	return ( startMsec && cg.time - startMsec < totalMsec ) ? s_fadeColor : NULL;
}
static char s_lastDrawn[256];
static snapshot_t s_snap;

/* ---- command tokenizer (quotes like Cmd_TokenizeString) ----------------- */
static char s_args[64][MAX_STRING_CHARS];
static int s_argc;

static void Tokenize( const char *text ) {
	s_argc = 0;
	while ( *text ) {
		int n = 0;
		while ( *text == ' ' ) text++;
		if ( !*text ) break;
		if ( *text == '"' ) {
			text++;
			while ( *text && *text != '"' ) s_args[s_argc][n++] = *text++;
			if ( *text == '"' ) text++;
		} else {
			while ( *text && *text != ' ' ) s_args[s_argc][n++] = *text++;
		}
		s_args[s_argc++][n] = '\0';
	}
}
int trap_Argc( void ) { return s_argc; }
const char *CG_Argv( int arg ) { return ( arg >= 0 && arg < s_argc ) ? s_args[arg] : ""; }

static void Server( const char *text ) {
	Tokenize( text );
	assert( CG_LadderGhost_ServerCommand( s_args[0] ) );
}

/* ---- stubs ---------------------------------------------------------------- */
static char s_sent[16][256];
static int s_sentCount;
static char s_lastPick[256] = "";
static int s_catcher;
static int s_localFileExists;
static int s_localLoads;

void QDECL Com_Error( int level, const char *fmt, ... ) { (void)level; (void)fmt; assert( 0 ); }
void QDECL Com_Printf( const char *fmt, ... ) { (void)fmt; }
void QDECL CG_Printf( const char *fmt, ... ) { (void)fmt; }
void trap_SendClientCommand( const char *s ) { Q_strncpyz( s_sent[s_sentCount++ & 15], s, 256 ); }
void trap_Cvar_Set( const char *name, const char *value ) {
	if ( !strcmp( name, "cg_ladderGhostLast" ) ) Q_strncpyz( s_lastPick, value, sizeof( s_lastPick ) );
}
void trap_Cvar_VariableStringBuffer( const char *name, char *buffer, int size ) {
	Q_strncpyz( buffer, !strcmp( name, "cg_ladderGhostLast" ) ? s_lastPick : "", size );
}
int trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode ) {
	(void)qpath; (void)mode;
	*f = s_localFileExists ? 1 : 0;
	return s_localFileExists ? 100 : -1;
}
void trap_FS_FCloseFile( fileHandle_t f ) { (void)f; }
qboolean CG_LoadLadderGhostFile( const char *path, int lapMs ) {
	(void)path; (void)lapMs;
	s_localLoads++;
	cg.ladderGhost.valid = qtrue;
	cg.ladderGhost.frameCount = 2;
	return qtrue;
}
int trap_Key_GetCatcher( void ) { return s_catcher; }
void trap_Key_SetCatcher( int catcher ) { s_catcher = catcher; }
qboolean CG_HUDOptionsIsOpen( void ) { return qfalse; }
void CG_SetScreenPlacement( screenPlacement_e hpos, screenPlacement_e vpos ) { (void)hpos; (void)vpos; }
void CG_FillRect( float x, float y, float w, float h, const float *color ) { (void)x; (void)y; (void)w; (void)h; (void)color; }
void CG_DrawRect( float x, float y, float w, float h, float size, const float *color ) { (void)x; (void)y; (void)w; (void)h; (void)size; (void)color; }
static int s_drawn;
int CG_IngameStringWidth( const char *text, int style, float scale ) {
	(void)style;
	return (int)( strlen( text ) * 20 * scale );
}
static int s_playbackMode = 3;
int CG_GhostPlaybackMode( void ) { return s_playbackMode; }
static char s_precached[64];
void CG_PrecacheGhostVehicle( const char *vehicle ) { Q_strncpyz( s_precached, vehicle, sizeof( s_precached ) ); }
void CG_DrawIngameString( int x, int y, const char *text, int style, float scale, const float *color ) {
	(void)x; (void)y; (void)style; (void)scale; (void)color;
	Q_strncpyz( s_lastDrawn, text, sizeof( s_lastDrawn ) );
	s_drawn++;
}

static void SendList( void ) {
	Server( "lghostlist 3 0" );
	Server( "lghostents 0 3 59000 evo \"Alpha\" ghosts/ladder/m/a.ghost"
		" 59500 sidepipe \"Gamma Ray\" ghosts/ladder/m/c.ghost 61000 evo \"Beta\" ghosts/ladder/m/b.ghost" );
	Server( "lghostlistdone" );
}

int main( void ) {
	int i;

	cgs.gametype = GT_GHOST;
	Q_strncpyz( cgs.mapname, "maps/q3r_testtrack.bsp", sizeof( cgs.mapname ) );
	cg.clientNum = 0;
	cg.snap = &s_snap;
	cg.time = 100000;
	Q_strncpyz( cgs.clientinfo[0].modelName, "Evo", sizeof( cgs.clientinfo[0].modelName ) );
	CG_LadderGhost_Reset();

	SendList();
	assert( cg.ladderGhostListReady && cg.ladderGhostEntryCount == 3 );
	assert( !strcmp( cg.ladderGhostEntries[1].name, "Gamma Ray" ) );
	assert( !strcmp( cg.ladderGhostEntries[1].cacheFile, "ghosts/ladder/m/c.ghost" ) );

	/* The picker opens by itself before the race; own car first. */
	CG_LadderGhost_DrawPicker();
	assert( cg.ladderPickerOpen && ( s_catcher & KEYCATCH_CGAME ) );
	{
		int visible[MAX_LADDER_GHOST_ENTRIES];
		assert( CG_LadderGhost_Visible( visible, MAX_LADDER_GHOST_ENTRIES ) == 2 );
		cg.ladderPickerAllVehicles = qtrue;
		assert( CG_LadderGhost_Visible( visible, MAX_LADDER_GHOST_ENTRIES ) == 3 );
		cg.ladderPickerAllVehicles = qfalse;
	}
	s_drawn = 0;
	CG_LadderGhost_DrawPicker();
	assert( s_drawn > 5 );
	/* The "ready" centre print moves into the panel, above the key help. */
	{
		int withoutHint = s_drawn;
		cg_centertime.value = 3;
		cg.centerPrintTime = cg.time;
		Q_strncpyz( cg.centerPrint, "Press FIRE or USE when ready to race.\n", sizeof( cg.centerPrint ) );
		s_drawn = 0;
		CG_LadderGhost_DrawPicker();
		assert( s_drawn == withoutHint + 1 );
		cg.centerPrintTime = 0;
	}

	/* Down to "Beta" (row 2 of own car), ENTER: not cached -> server pick. */
	assert( CG_LadderGhost_KeyEvent( K_DOWNARROW ) );
	assert( CG_LadderGhost_KeyEvent( K_DOWNARROW ) );
	assert( CG_LadderGhost_KeyEvent( K_DOWNARROW ) );   /* clamped */
	assert( cg.ladderPickerCursor == 2 );
	assert( CG_LadderGhost_KeyEvent( K_ENTER ) );
	assert( !cg.ladderPickerOpen && !( s_catcher & KEYCATCH_CGAME ) );
	assert( cg.ladderGhostSelected == 2 && cg.ladderGhostPending );
	assert( !strcmp( s_sent[s_sentCount - 1], "lghostpick 2" ) );
	assert( !strcmp( s_precached, "evo" ) );
	assert( !strcmp( s_lastPick, "q3r_testtrack|61000|evo|Beta" ) );
	assert( !CG_LadderGhost_KeyEvent( K_ENTER ) );

	/* Streamed ghost: meta, data in order, done. */
	Server( "lghostmeta 2 61000 3" );
	Server( "lghostdata 0 2 0 10.0 20.0 30.0 1.0 90.0 0.0 100 20.0 20.0 30.0 2.0 91.0 0.0" );
	Server( "lghostdata 2 1 61000 30.0 20.0 30.0 3.0 92.0 0.0" );
	Server( "lghostdone 2" );
	assert( cg.ladderGhostAvailable && cg.ladderGhost.valid && !cg.ladderGhostPending );
	assert( cg.ladderGhost.frameCount == 3 && cg.ladderGhost.duration == 61000 );
	assert( cg.ladderGhost.frames[1].angles[YAW] == 91.0f );
	assert( cg.ladderGhost.frames[0].velocity[0] == 100.0f );
	{
		qboolean err;
		assert( !strcmp( CG_LadderGhost_StatusText( &err ), "VS BETA" ) && !err );
	}

	/* Out-of-order chunk fails the transfer. */
	CG_LadderGhost_Pick( 1 );
	Server( "lghostmeta 1 59500 3" );
	Server( "lghostdata 1 1 0 0 0 0 0 0 0" );
	assert( cg.ladderGhostFailed && !cg.ladderGhost.valid );
	{
		qboolean err;
		assert( !strcmp( CG_LadderGhost_StatusText( &err ), "GHOST FAILED" ) && err );
	}
	/* Data for another pick is ignored. */
	Server( "lghostmeta 0 59000 3" );
	assert( !cg.ladderGhostPending );
	Server( "lghostfail 1 download" );
	assert( cg.ladderGhostFailed );

	/* Cached locally: loaded without asking the server. */
	s_localFileExists = 1;
	cg.time += 1300;
	i = s_sentCount;
	CG_LadderGhost_Pick( 0 );
	assert( s_localLoads == 1 && cg.ladderGhostAvailable );
	/* No pick command, only the opponent report for the server (with the
	 * ladder entry, so the server can check the lap time). */
	assert( s_sentCount == i + 1 && !strcmp( s_sent[( s_sentCount - 1 ) & 15], "ghostopp 59000 \"Alpha\" 0" ) );
	s_localFileExists = 0;

	/* map_restart: list again, the loaded ghost stays without a new transfer. */
	i = s_sentCount;
	SendList();
	assert( cg.ladderGhostSelected == 0 && cg.ladderGhostAvailable && s_sentCount == i && s_localLoads == 1 );

	/* cgame restart: the remembered pick is selected again, no picker. */
	CG_LadderGhost_Reset();
	SendList();
	assert( cg.ladderGhostSelected == 0 && cg.ladderPickerAutoShown );
	CG_LadderGhost_DrawPicker();
	assert( !cg.ladderPickerOpen );

	/* Row 0 drops the ladder ghost and is remembered as "none". */
	CG_LadderGhost_TogglePicker_f();
	assert( cg.ladderPickerOpen );
	assert( CG_LadderGhost_KeyEvent( K_ENTER ) );
	assert( cg.ladderGhostSelected == -1 && !strcmp( s_lastPick, "none|q3r_testtrack" ) );
	CG_LadderGhost_Reset();
	SendList();
	CG_LadderGhost_DrawPicker();
	assert( !cg.ladderPickerOpen );
	/* ... but only for this map. */
	Q_strncpyz( cgs.mapname, "maps/q3r_other.bsp", sizeof( cgs.mapname ) );
	CG_LadderGhost_Reset();
	SendList();
	CG_LadderGhost_DrawPicker();
	assert( cg.ladderPickerOpen );
	CG_LadderGhost_CatcherCleared();
	s_catcher = 0;
	Q_strncpyz( cgs.mapname, "maps/q3r_testtrack.bsp", sizeof( cgs.mapname ) );

	/* ESC (catcher cleared by the engine) closes; race start closes too. */
	s_lastPick[0] = '\0';
	CG_LadderGhost_Reset();
	SendList();
	CG_LadderGhost_DrawPicker();
	assert( cg.ladderPickerOpen );
	CG_LadderGhost_CatcherCleared();
	assert( !cg.ladderPickerOpen );
	s_catcher = 0;
	CG_LadderGhost_Reset();
	SendList();
	CG_LadderGhost_DrawPicker();
	assert( cg.ladderPickerOpen && !cg.ladderPickerManual );
	cg_entities[0].startRaceTime = 5000;
	CG_LadderGhost_DrawPicker();
	assert( !cg.ladderPickerOpen && !( s_catcher & KEYCATCH_CGAME ) );
	/* Opened by hand during the race: stays open. */
	CG_LadderGhost_TogglePicker_f();
	CG_LadderGhost_DrawPicker();
	assert( cg.ladderPickerOpen && cg.ladderPickerManual );
	CG_LadderGhost_TogglePicker_f();
	assert( !cg.ladderPickerOpen );

	/* Explicit personal / base playback: no automatic picker. */
	cg_entities[0].startRaceTime = 0;
	cg_ghostPlayback.integer = 1;
	CG_LadderGhost_Reset();
	SendList();
	CG_LadderGhost_DrawPicker();
	assert( !cg.ladderPickerOpen );

	assert( !CG_LadderGhost_ServerCommand( "ghostmeta" ) );

	/* Result against the ghost: best lap vs. the ghost's lap. */
	cg_ghostPlayback.integer = 0;
	CG_LadderGhost_Reset();
	SendList();
	CG_LadderGhost_Pick( 1 );              /* Gamma Ray, 59500 */
	Server( "lghostmeta 1 59500 2" );
	Server( "lghostdata 0 2 0 0 0 0 0 0 0 59500 10 0 0 0 0 0" );
	Server( "lghostdone 1" );
	assert( cg.ladderGhostAvailable );
	{
		char name[40];
		int ghostMs = 0, playerMs = 0;
		qboolean finished = qtrue;

		/* Live, before the finish: current best lap. */
		cg_entities[0].bestLapTime = 60100;
		assert( CG_GhostRace_ScoreboardGhost( name, sizeof( name ), &ghostMs, &playerMs, &finished ) );
		assert( !strcmp( name, "Gamma Ray" ) && ghostMs == 59500 && playerMs == 60100 && !finished );

		CG_GhostRace_EvaluateFinish( 59321 );
		assert( cg.ghostResultValid && cg.ghostResultWon );
		assert( cg.ghostResultPlayerMs == 59321 && cg.ghostResultGhostMs == 59500 );
		assert( CG_GhostRace_ScoreboardGhost( name, sizeof( name ), &ghostMs, &playerMs, &finished ) );
		assert( finished && playerMs == 59321 );

		s_drawn = 0;
		s_snap.ps.pm_type = PM_INTERMISSION;
		CG_GhostRace_DrawResultBanner();
		assert( s_drawn == 2 );

		CG_GhostRace_EvaluateFinish( 59500 );   /* equal is not a win */
		assert( cg.ghostResultValid && !cg.ghostResultWon );

		/* Banner disappears after a while outside intermission. */
		s_snap.ps.pm_type = PM_NORMAL;
		cg.time = cg.ghostResultTime + 9000;
		s_drawn = 0;
		CG_GhostRace_DrawResultBanner();
		assert( s_drawn == 0 );

		/* Personal / server ghost and no ghost. */
		s_playbackMode = 1;
		cg.personalGhostAvailable = qtrue;
		cg.personalGhostBestTime = 61000;
		CG_GhostRace_EvaluateFinish( 60000 );
		assert( cg.ghostResultWon && !strcmp( cg.ghostResultName, "Personal Ghost" ) );
		s_playbackMode = 0;
		CG_GhostRace_EvaluateFinish( 60000 );
		assert( !cg.ghostResultValid );
		/* No ghost in Ghost Race: neutral banner, no ghost row, no result. */
		assert( cg.ghostFinishOnly && cg.ghostResultPlayerMs == 60000 );
		s_snap.ps.pm_type = PM_INTERMISSION;
		s_drawn = 0;
		CG_GhostRace_DrawResultBanner();
		assert( s_drawn == 2 );
		assert( !CG_GhostRace_ScoreboardGhost( name, sizeof( name ), &ghostMs, &playerMs, &finished ) );
		{
			qboolean won;
			assert( !CG_GhostRace_ClientResult( 0, &won ) );
		}
		s_snap.ps.pm_type = PM_NORMAL;

		/* Opponent report: once per change, again after a new race start. */
		s_playbackMode = 3;
		s_sentCount = 0;
		CG_GhostRace_ResetRace();      /* race start */
		cg.time += 1300;
		CG_GhostRace_ReportOpponent();
		assert( s_sentCount == 1 && !strcmp( s_sent[0], "ghostopp 59500 \"Gamma Ray\" 1" ) );
		CG_GhostRace_ReportOpponent();
		assert( s_sentCount == 1 );
		CG_GhostRace_ResetRace();
		cg.time += 1300;
		CG_GhostRace_ReportOpponent();
		assert( s_sentCount == 2 );
		s_playbackMode = 0;
		cg.time += 1300;
		CG_GhostRace_ReportOpponent();
		assert( s_sentCount == 3 && !strcmp( s_sent[2], "ghostopp 0 \"-\" -1" ) );

		/* Server results for every driver; own result is overwritten. */
		Server( "lghostresult 3 1 58000 59000 \"Other Ghost\"" );
		Server( "lghostresult 0 0 61000 60000 \"Gamma Ray\"" );
		{
			qboolean won = qfalse;
			assert( CG_GhostRace_ClientResult( 3, &won ) && won );
			assert( CG_GhostRace_ClientResult( 0, &won ) && !won );
			assert( !CG_GhostRace_ClientResult( 4, &won ) );
		}
		assert( cg.ghostResultValid && !cg.ghostResultWon && cg.ghostResultPlayerMs == 61000 );
		CG_GhostRace_ResetRace();
		{
			qboolean won;
			assert( !CG_GhostRace_ClientResult( 3, &won ) );
		}

		cgs.gametype = GT_RACING;
		s_playbackMode = 3;
		CG_GhostRace_EvaluateFinish( 1000 );
		assert( !cg.ghostResultValid && !cg.ghostFinishOnly );
		assert( !CG_GhostRace_ScoreboardGhost( name, sizeof( name ), &ghostMs, &playerMs, &finished ) );
	}

	/* Dedicated servers drop a client command within a second of the
	 * previous one: one ghost command per gap, repeated until answered. */
	cgs.gametype = GT_GHOST;
	s_playbackMode = 3;
	cg_ghostPlayback.integer = 0;
	s_lastPick[0] = '\0';
	s_localFileExists = 0;
	CG_LadderGhost_Reset();
	cg.time += 5000;
	SendList();
	s_sentCount = 0;
	cg.scoresRequestTime = cg.time - 300;      /* the HUD just asked for "score" */
	CG_LadderGhost_Pick( 2 );
	assert( s_sentCount == 0 && cg.ladderGhostPending );
	cg.time += 600;
	CG_LadderGhost_NetFrame();
	assert( s_sentCount == 0 );
	cg.time += 400;                             /* 1300 ms after "score" */
	CG_LadderGhost_NetFrame();
	assert( s_sentCount == 1 && !strcmp( s_sent[0], "lghostpick 2" ) );
	/* The next "score" request waits for the gap as well. */
	assert( cg.scoresRequestTime + 2000 >= cg.time + 1200 );
	/* No answer: repeated after the retry time. */
	cg.time += 2000;
	CG_LadderGhost_NetFrame();
	assert( s_sentCount == 1 );
	cg.time += 1000;
	CG_LadderGhost_NetFrame();
	assert( s_sentCount == 2 && !strcmp( s_sent[1], "lghostpick 2" ) );
	/* Taken by the server: no more repeats while the download runs ... */
	Server( "lghostpickok 2" );
	cg.time += 10000;
	CG_LadderGhost_NetFrame();
	assert( s_sentCount == 2 && cg.ladderGhostPending && !cg.ladderGhostFailed );
	/* ... but the ghost has to come at some point. */
	cg.time += 40000;
	CG_LadderGhost_NetFrame();
	assert( cg.ladderGhostFailed && !cg.ladderGhostPending );
	/* Never answered: the pick fails after five tries. */
	CG_LadderGhost_Pick( 2 );
	assert( s_sentCount == 3 );
	for ( i = 0; i < 10; i++ ) {
		cg.time += 3000;
		CG_LadderGhost_NetFrame();
	}
	assert( s_sentCount == 7 && cg.ladderGhostFailed && !cg.ladderGhostPending );
	/* The streamed ghost answers a pick as well; a repeated pick that crosses
	 * the finished transfer does not throw the ghost away. */
	cg.time += 3000;
	CG_LadderGhost_Pick( 2 );
	assert( s_sentCount == 8 );
	Server( "lghostmeta 2 61000 2" );
	Server( "lghostdata 0 2 0 0 0 0 0 0 0 61000 10 0 0 0 0 0" );
	Server( "lghostdone 2" );
	assert( cg.ladderGhostAvailable );
	for ( i = 0; i < 4; i++ ) {
		cg.time += 3000;
		CG_LadderGhost_NetFrame();
	}
	/* Only the opponent report went out (and is repeated, no answer yet). */
	assert( !strcmp( s_sent[8 & 15], "ghostopp 61000 \"Beta\" 2" ) );
	Server( "lghostmeta 2 61000 2" );
	assert( cg.ladderGhostAvailable && cg.ladderGhost.valid && cg.ladderGhost.frameCount == 2 );
	/* Opponent report: only the matching answer stops the repeats. */
	i = s_sentCount;
	Server( "lghostoppok 1234" );
	cg.time += 3000;
	CG_LadderGhost_NetFrame();
	assert( s_sentCount == i + 1 && !strcmp( s_sent[( s_sentCount - 1 ) & 15], "ghostopp 61000 \"Beta\" 2" ) );
	Server( "lghostoppok 61000" );
	cg.time += 3000;
	CG_LadderGhost_NetFrame();
	assert( s_sentCount == i + 1 );
	/* List request after a cgame restart, answered by the list. */
	cg.time += 3000;
	i = s_sentCount;
	CG_LadderGhost_RequestList();
	assert( s_sentCount == i + 1 && !strcmp( s_sent[( s_sentCount - 1 ) & 15], "lghostlistreq" ) );
	s_lastPick[0] = '\0';
	SendList();
	cg.time += 3000;
	CG_LadderGhost_NetFrame();
	assert( s_sentCount == i + 1 );
	/* Outside Ghost Race nothing is sent. */
	cgs.gametype = GT_RACING;
	CG_LadderGhost_RequestList();
	CG_LadderGhost_NetFrame();
	assert( s_sentCount == i + 1 );
	puts( "ok" );
	return 0;
}
