/*
 * Ladder results only with standard rules (g_ladder_rules.c): sv_cheats,
 * timescale, fixedtime and the physics cvars against their defaults.
 */
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../engine/code/game/g_ladder_rules.c"

level_locals_t level;
gentity_t g_entities[MAX_GENTITIES];

void QDECL Com_Error( int level, const char *fmt, ... ) { (void)level; (void)fmt; assert( 0 ); }
void QDECL Com_Printf( const char *fmt, ... ) { (void)fmt; }
static int s_printed;
void QDECL G_Printf( const char *fmt, ... ) { (void)fmt; s_printed++; }

/* ---- cvars ---------------------------------------------------------------- */
typedef struct { const char *name; const char *defaultString; char value[64]; } testCvar_t;
static testCvar_t s_cvars[] = {
	{ "sv_cheats", NULL, "0" },
	{ "timescale", NULL, "1" },
	{ "fixedtime", NULL, "0" },
	{ "sv_ladderEnabled", NULL, "1" },
	{ "g_speed", "320", "320" },
	{ "g_gravity", "1400", "1400" },
	{ "pmove_fixed", "1", "1" },
	{ "pmove_msec", "18", "18" },
	{ "car_spring", "120", "120" },
	{ "car_frontweight_dist", "0.5", "0.5" },
	{ "car_friction_scale", "1.1", "1.1" },
	{ "g_carImpactTransfer", "1.0", "1.0" },
};

static testCvar_t *FindCvar( const char *name ) {
	size_t i;
	for ( i = 0; i < ARRAY_LEN( s_cvars ); i++ ) {
		if ( !Q_stricmp( s_cvars[i].name, name ) ) {
			return &s_cvars[i];
		}
	}
	return NULL;
}
static void SetCvar( const char *name, const char *value ) {
	testCvar_t *cvar = FindCvar( name );
	assert( cvar );
	Q_strncpyz( cvar->value, value, sizeof( cvar->value ) );
	level.framenum++;       /* new frame: no cached result */
}
const char *G_CvarDefault( const char *name ) {
	testCvar_t *cvar = FindCvar( name );
	return cvar ? cvar->defaultString : NULL;
}
void trap_Cvar_VariableStringBuffer( const char *name, char *buffer, int size ) {
	testCvar_t *cvar = FindCvar( name );
	Q_strncpyz( buffer, cvar ? cvar->value : "", size );
}
int trap_Cvar_VariableIntegerValue( const char *name ) {
	testCvar_t *cvar = FindCvar( name );
	return cvar ? atoi( cvar->value ) : 0;
}

static char s_command[MAX_STRING_CHARS];
static int s_commandCount;
void trap_SendServerCommand( int clientNum, const char *text ) {
	(void)clientNum;
	Q_strncpyz( s_command, text, sizeof( s_command ) );
	s_commandCount++;
}

int main( void ) {
	const char *reason;

	/* Standard rules: results count, nothing printed. */
	assert( G_LadderRulesViolation() == NULL );
	G_LadderRules_Init();
	assert( s_printed == 0 );
	G_LadderRules_ClientBegin( 0 );
	assert( s_commandCount == 0 );

	/* Same value written differently is still the default. */
	SetCvar( "car_frontweight_dist", "0.50" );
	SetCvar( "g_carImpactTransfer", "1" );
	assert( G_LadderRulesViolation() == NULL );

	SetCvar( "sv_cheats", "1" );
	assert( !strcmp( G_LadderRulesViolation(), "sv_cheats 1" ) );
	SetCvar( "sv_cheats", "0" );

	/* Slow motion on an offline server. */
	SetCvar( "timescale", "0.5" );
	assert( !strcmp( G_LadderRulesViolation(), "timescale 0.5" ) );
	SetCvar( "timescale", "1" );
	SetCvar( "fixedtime", "8" );
	assert( !strcmp( G_LadderRulesViolation(), "fixedtime" ) );
	SetCvar( "fixedtime", "0" );

	/* Physics. */
	SetCvar( "g_gravity", "1000" );
	assert( !strcmp( G_LadderRulesViolation(), "g_gravity 1000 instead of 1400" ) );
	SetCvar( "g_gravity", "1400" );
	SetCvar( "pmove_msec", "8" );
	assert( !strcmp( G_LadderRulesViolation(), "pmove_msec 8 instead of 18" ) );
	SetCvar( "pmove_msec", "18" );
	SetCvar( "car_friction_scale", "1.5" );
	reason = G_LadderRulesViolation();
	assert( !strcmp( reason, "car_friction_scale 1.5 instead of 1.1" ) );

	/* Cached for the frame, evaluated again in the next one. */
	Q_strncpyz( FindCvar( "car_friction_scale" )->value, "1.1", 64 );
	assert( G_LadderRulesViolation() == reason );
	level.time += 50;
	assert( G_LadderRulesViolation() == NULL );

	/* Told once on the console at map start and to joining drivers. */
	SetCvar( "car_spring", "200" );
	G_LadderRules_Init();
	assert( s_printed == 1 );
	G_LadderRules_ClientBegin( 0 );
	assert( s_commandCount == 1 );
	assert( !strcmp( s_command, "print \"^3Ladder: results on this map are not reported (car_spring 200 instead of 120).\n\"" ) );
	/* Not to bots, not without ladder. */
	g_entities[1].r.svFlags = SVF_BOT;
	G_LadderRules_ClientBegin( 1 );
	assert( s_commandCount == 1 );
	SetCvar( "sv_ladderEnabled", "0" );
	G_LadderRules_ClientBegin( 0 );
	G_LadderRules_Init();
	assert( s_commandCount == 1 && s_printed == 1 );

	puts( "ok" );
	return 0;
}
