/*
===========================================================================
Copyright (C) 2002-2026 Q3Rally Team

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.
===========================================================================
*/
// g_ladder_rules.c -- ladder results only count with the standard rules
//
// Match reports and lap ghosts reach the ladder only when the server runs
// the standard rules:
//   - no cheats: sv_cheats 0 (also covers god, noclip, give and every
//     cheat-protected cvar),
//   - real time: timescale 1 and fixedtime 0 (a lap driven in slow motion
//     on an offline server would otherwise count with its game time),
//   - default physics: the movement and car physics cvars below at the
//     defaults of the game cvar table (g_main.c).
// The check runs in the game module of the reporting server, so it covers
// dedicated servers and offline games (listen server) alike.

#include "g_local.h"

static const char *s_ladderPhysicsCvars[] = {
	"g_speed",
	"g_gravity",
	"pmove_fixed",
	"pmove_msec",
	"car_spring",
	"car_shock_up",
	"car_shock_down",
	"car_swaybar",
	"car_wheel",
	"car_wheel_damp",
	"car_frontweight_dist",
	"car_IT_xScale",
	"car_IT_yScale",
	"car_IT_zScale",
	"car_body_elasticity",
	"car_air_cof",
	"car_air_frac_to_df",
	"car_friction_scale",
	"g_carImpactTransfer",
	"g_carImpactElasticity"
};

#define LADDER_PHYSICS_CVARS    ( (int)ARRAY_LEN( s_ladderPhysicsCvars ) )
#define LADDER_RULES_EPSILON    0.0001f

static char         s_rulesReason[96];
static qboolean     s_rulesCached;
static int          s_rulesCacheFrame;
static int          s_rulesCacheTime;
static const char   *s_rulesCacheResult;

static float G_LadderRules_Value( const char *name ) {
	char buffer[64];

	trap_Cvar_VariableStringBuffer( name, buffer, sizeof( buffer ) );
	return (float)atof( buffer );
}

/* "0.5", "1400", "1.1": the same text for the QVM (no %g there) and native. */
static void G_LadderRules_Format( float value, char *out, int outSize ) {
	int length;

	Com_sprintf( out, outSize, "%.4f", value );
	length = (int)strlen( out );
	while ( length > 0 && out[length - 1] == '0' ) {
		out[--length] = '\0';
	}
	if ( length > 0 && out[length - 1] == '.' ) {
		out[--length] = '\0';
	}
}

static qboolean G_LadderRules_Differs( float value, float expected ) {
	float diff = value - expected;

	return ( diff > LADDER_RULES_EPSILON || diff < -LADDER_RULES_EPSILON ) ? qtrue : qfalse;
}

static const char *G_LadderRules_Check( void ) {
	char valueText[24];
	char expectedText[24];
	float value;
	int i;

	if ( trap_Cvar_VariableIntegerValue( "sv_cheats" ) ) {
		return "sv_cheats 1";
	}
	value = G_LadderRules_Value( "timescale" );
	if ( G_LadderRules_Differs( value, 1.0f ) ) {
		G_LadderRules_Format( value, valueText, sizeof( valueText ) );
		Com_sprintf( s_rulesReason, sizeof( s_rulesReason ), "timescale %s", valueText );
		return s_rulesReason;
	}
	if ( trap_Cvar_VariableIntegerValue( "fixedtime" ) ) {
		return "fixedtime";
	}
	for ( i = 0; i < LADDER_PHYSICS_CVARS; i++ ) {
		const char *defaultString = G_CvarDefault( s_ladderPhysicsCvars[i] );
		float expected;

		if ( !defaultString ) {
			continue;
		}
		expected = (float)atof( defaultString );
		value = G_LadderRules_Value( s_ladderPhysicsCvars[i] );
		if ( G_LadderRules_Differs( value, expected ) ) {
			G_LadderRules_Format( value, valueText, sizeof( valueText ) );
			G_LadderRules_Format( expected, expectedText, sizeof( expectedText ) );
			Com_sprintf( s_rulesReason, sizeof( s_rulesReason ), "%s %s instead of %s",
				s_ladderPhysicsCvars[i], valueText, expectedText );
			return s_rulesReason;
		}
	}
	return NULL;
}

/*
=================
G_LadderRulesViolation

NULL when ladder results count, otherwise a short reason for the log.
Evaluated at most once per server frame.
=================
*/
const char *G_LadderRulesViolation( void ) {
	if ( s_rulesCached && s_rulesCacheFrame == level.framenum && s_rulesCacheTime == level.time ) {
		return s_rulesCacheResult;
	}
	s_rulesCacheResult = G_LadderRules_Check();
	s_rulesCacheFrame = level.framenum;
	s_rulesCacheTime = level.time;
	s_rulesCached = qtrue;
	return s_rulesCacheResult;
}

/*
=================
G_LadderRules_Init

Called from G_InitGame: tells the server console once per map when the
results of this map do not reach the ladder.
=================
*/
void G_LadderRules_Init( void ) {
	const char *reason;

	s_rulesCached = qfalse;
	if ( !trap_Cvar_VariableIntegerValue( "sv_ladderEnabled" ) ) {
		return;
	}
	reason = G_LadderRulesViolation();
	if ( reason ) {
		G_Printf( "Ladder: results of this map are not reported (%s)\n", reason );
	}
}

/*
=================
G_LadderRules_ClientBegin

Tells a joining driver when results on this server do not count.
=================
*/
void G_LadderRules_ClientBegin( int clientNum ) {
	const char *reason;

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS || ( g_entities[clientNum].r.svFlags & SVF_BOT ) ) {
		return;
	}
	if ( !trap_Cvar_VariableIntegerValue( "sv_ladderEnabled" ) ) {
		return;
	}
	reason = G_LadderRulesViolation();
	if ( reason ) {
		trap_SendServerCommand( clientNum,
			va( "print \"^3Ladder: results on this map are not reported (%s).\n\"", reason ) );
	}
}
