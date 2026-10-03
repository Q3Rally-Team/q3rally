/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2021 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

q3rally source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with q3rally; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "g_local.h"

#define INTRO_GHOST_ROUTE_MAX_FILE_SIZE ( 64 * 1024 )
#define INTRO_GHOST_ROUTE_MAX_FRAMES 256
/* Fallback duration by track class, used only if checkpoint distance is missing. */
#define INTRO_GHOST_CAM_BASE_DURATION_MS 10000
#define INTRO_GHOST_CAM_DURATION_STEP_MS 5000
#define INTRO_GHOST_CAM_MIN_DURATION_MS 10000
#define INTRO_GHOST_CAM_MAX_DURATION_MS 30000
#define INTRO_GHOST_CAM_MS_PER_METER 30.0f

/*
 * Intro routes are camera-only derivatives of Ghost recordings. The client
 * loads the selected route and evaluates it locally at full render rate.
 */
static qboolean G_RallyIntroRoute_Find( void ) {
	char mapName[MAX_QPATH];
	char routePath[MAX_QPATH];
	char routeHeader[256];
	char *headerCursor;
	char *token;
	fileHandle_t routeFile;
	int routeFileLength;
	int trackReversed;
	int headerReadLength;
	int routeTrackLength, routeTrackReversed, routeFrames;
	int i;

	level.raceIntroHasRoute = qfalse;
	level.raceIntroRoute[0] = '\0';

	if ( g_trackLength.integer < 0 || g_trackLength.integer > 2 ) {
		return qfalse;
	}

	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );
	Q_strlwr( mapName );
	if ( !mapName[0] ) {
		return qfalse;
	}

	/* Keep the cvar-derived filename inside the intro_routes directory. */
	for ( i = 0; mapName[i]; i++ ) {
		if ( ( mapName[i] < 'a' || mapName[i] > 'z' )
			&& ( mapName[i] < '0' || mapName[i] > '9' )
			&& mapName[i] != '_' && mapName[i] != '-' ) {
			return qfalse;
		}
	}
	trackReversed = ( g_trackReversed.integer && level.trackIsReversable ) ? 1 : 0;

	if ( Com_sprintf( routePath, sizeof( routePath ),
		"intro_routes/%s_tl%d_rev%d.route", mapName,
		g_trackLength.integer, trackReversed ) >= sizeof( routePath ) ) {
		return qfalse;
	}

	routeFileLength = trap_FS_FOpenFile( routePath, &routeFile, FS_READ );
	if ( routeFileLength < 0 ) {
		return qfalse;
	}
	if ( routeFileLength == 0 ) {
		trap_FS_FCloseFile( routeFile );
		return qfalse;
	}
	if ( routeFileLength > INTRO_GHOST_ROUTE_MAX_FILE_SIZE ) {
		trap_FS_FCloseFile( routeFile );
		G_Printf( "Warning: intro Ghost route '%s' is too large (%d bytes); ignoring it.\n",
			routePath, routeFileLength );
		return qfalse;
	}
	headerReadLength = ( routeFileLength < sizeof( routeHeader ) - 1 ) ? routeFileLength : sizeof( routeHeader ) - 1;
	trap_FS_Read( routeHeader, headerReadLength, routeFile );
	trap_FS_FCloseFile( routeFile );
	routeHeader[headerReadLength] = '\0';
	headerCursor = routeHeader;
	token = COM_ParseExt( &headerCursor, qtrue );
	if ( !token[0] || Q_stricmp( token, "Q3RALLY_INTRO_ROUTE" ) ) return qfalse;
	token = COM_ParseExt( &headerCursor, qtrue );
	if ( !token[0] || atoi( token ) != 1 ) return qfalse;
	token = COM_ParseExt( &headerCursor, qtrue );
	if ( !token[0] || Q_stricmp( token, "map" ) ) return qfalse;
	token = COM_ParseExt( &headerCursor, qtrue );
	if ( !token[0] || Q_stricmp( token, mapName ) ) return qfalse;
	token = COM_ParseExt( &headerCursor, qtrue );
	if ( !token[0] || Q_stricmp( token, "track_length" ) ) return qfalse;
	token = COM_ParseExt( &headerCursor, qtrue );
	if ( !token[0] ) return qfalse;
	routeTrackLength = atoi( token );
	token = COM_ParseExt( &headerCursor, qtrue );
	if ( !token[0] || Q_stricmp( token, "track_reversed" ) ) return qfalse;
	token = COM_ParseExt( &headerCursor, qtrue );
	if ( !token[0] ) return qfalse;
	routeTrackReversed = atoi( token ) ? 1 : 0;
	token = COM_ParseExt( &headerCursor, qtrue );
	if ( !token[0] || Q_stricmp( token, "frames" ) ) return qfalse;
	token = COM_ParseExt( &headerCursor, qtrue );
	if ( !token[0] ) return qfalse;
	routeFrames = atoi( token );
	if ( routeTrackLength != g_trackLength.integer || routeTrackReversed != trackReversed
		|| routeFrames < 2 || routeFrames > INTRO_GHOST_ROUTE_MAX_FRAMES ) {
		return qfalse;
	}

	Q_strncpyz( level.raceIntroRoute, routePath, sizeof( level.raceIntroRoute ) );
	level.raceIntroDurationMs = INTRO_GHOST_CAM_BASE_DURATION_MS
		+ g_trackLength.integer * INTRO_GHOST_CAM_DURATION_STEP_MS;
	return qtrue;
}

void G_RallyIntroRoute_UpdateDuration( void ) {
	float trackLengthMeters;
	int durationMs;

	if ( !level.raceIntroHasRoute || level.trackLength <= 0.0f ) {
		return;
	}

	trackLengthMeters = level.trackLength / CP_M_2_QU;
	durationMs = (int)( trackLengthMeters * INTRO_GHOST_CAM_MS_PER_METER + 0.5f );
	if ( durationMs < INTRO_GHOST_CAM_MIN_DURATION_MS ) {
		durationMs = INTRO_GHOST_CAM_MIN_DURATION_MS;
	} else if ( durationMs > INTRO_GHOST_CAM_MAX_DURATION_MS ) {
		durationMs = INTRO_GHOST_CAM_MAX_DURATION_MS;
	}
	level.raceIntroDurationMs = durationMs;
	G_Printf( "Info: intro Ghost route duration %d ms for %.0f m track.\n",
		durationMs, trackLengthMeters );
}

static qboolean G_RallyIntroRoute_UsesRaceState( void ) {
	return ( g_gametype.integer == GT_RACING
		|| g_gametype.integer == GT_RACING_DM
		|| g_gametype.integer == GT_TEAM_RACING
		|| g_gametype.integer == GT_TEAM_RACING_DM
		|| g_gametype.integer == GT_SPRINT
		|| g_gametype.integer == GT_ELIMINATION
		|| g_gametype.integer == GT_GHOST ) ? qtrue : qfalse;
}

void G_RallyIntroRoute_SetPending( qboolean pending ) {
	char configString[MAX_INFO_STRING];
	int configLength;
	int trackReversed;

	if ( !level.raceIntroHasRoute ) {
		return;
	}

	trackReversed = ( g_trackReversed.integer && level.trackIsReversable ) ? 1 : 0;
	configLength = Com_sprintf( configString, sizeof( configString ), "ghost %s %d %d %d",
		level.raceIntroRoute, level.raceIntroDurationMs, trackReversed, pending ? 1 : 0 );
	if ( configLength < 0 || configLength >= sizeof( configString ) ) {
		return;
	}

	trap_SetConfigstring( CS_INTRO_ROUTE, configString );
}

void G_RallyIntroRoute_Init( void ) {
	level.raceIntroDurationMs = 0;
	level.raceIntroHasRoute = qfalse;
	level.raceIntroRoute[0] = '\0';
	trap_SetConfigstring( CS_INTRO_ROUTE, "" );

	if ( !G_RallyIntroRoute_Find() ) {
		level.raceIntroFallback = qtrue;
		G_Printf( "Info: No packaged intro Ghost route found; using countdown fallback.\n" );
		return;
	}

	level.raceIntroHasRoute = qtrue;
	G_RallyIntroRoute_SetPending( !level.raceIntroFallback && G_RallyIntroRoute_UsesRaceState() );
	G_Printf( "Info: using packaged Ghost route '%s' for the automatic track preview.\n",
		level.raceIntroRoute );
}

/* Observer spots remain for live spectating; race intros use packaged routes. */
void SP_info_observer_spot( gentity_t *ent ) {
	G_SetOrigin( ent, ent->s.origin );
	if ( ent->target ) {
		ent->spawnflags |= OBSERVERCAM_FIXED;
	}
}
gentity_t *FindBestObserverSpot( gentity_t *self, gentity_t *target, vec3_t spot, vec3_t angles){
	gentity_t		*ent;
	trace_t			tr;
	vec3_t			delta;
	vec3_t			targetOrigin;
	static vec3_t	mins = { -4, -4, -4 };
	static vec3_t	maxs = { 4, 4, 4 };
	float			dist, bestDist;
	gentity_t		*foundSpot;

	// Use ps.origin as the target reference for both trace and distance checks
	// so observer spot selection stays consistent and more deterministic.
	VectorCopy(target->client->ps.origin, targetOrigin);

	foundSpot = NULL;
	dist = 0;
	bestDist = 0;
	ent = NULL;
	while ( (ent = G_Find (ent, FOFS(classname), "info_observer_spot")) != NULL )
	{
//		if ( !trap_InPVS( ent->s.origin, target->s.origin) ) continue;

//		Com_Printf("Found an observer spot in PVS\n");
//		VectorCopy(ent->s.origin, spot);
//		foundSpot = ent;
//		return foundSpot;
		
		trap_Trace(&tr, ent->r.currentOrigin, mins, maxs, targetOrigin, target->s.number, CONTENTS_SOLID);

		if (tr.startsolid || tr.allsolid || tr.fraction < 1.0) continue;

		VectorSubtract(targetOrigin, ent->s.origin, delta);
		dist = VectorNormalize(delta);

		// check for spot with locked angles
		if (ent->spawnflags & OBSERVERCAM_FIXED)
		{
			vec3_t	forward;

			AngleVectors(ent->s.angles, forward, NULL, NULL);
			if (DotProduct(delta, forward) < -0.40)
			{
				VectorCopy(ent->s.origin, spot);
				VectorCopy(ent->s.angles, angles);

				self->spotflags = ent->spawnflags;

				// use this one
				return ent;
			}
		}

		if (dist < bestDist || bestDist == 0)
		{
			bestDist = dist;
			VectorCopy(ent->s.origin, spot);
			VectorCopy(ent->s.angles, angles);

//			Com_Printf("Found a valid observer spot\n");
			self->spotflags = ent->spawnflags;
			foundSpot = ent;
		}
	}

	return foundSpot;
}

void UpdateObserverSpot( gentity_t *ent, qboolean forceUpdate ){
	vec3_t			origin, angles;
	trace_t			tr;
	int				clientNum;
	gclient_t		*targetClient;
	static vec3_t	mins = { -4, -4, -4 };
	static vec3_t	maxs = { 4, 4, 4 };

	clientNum = ent->client->sess.spectatorClient;
	if ( clientNum == -1 )
		clientNum = level.follow1;
	else if ( clientNum == -2 )
		clientNum = level.follow2;

	if (clientNum < 0)
	{
//		ent->client->sess.spectatorState = SPECTATOR_FREE;
//		G_DebugLogPrintf( "UpdateObserverSpot: drop back to free\n" );
		StopFollowing( ent );
//		ClientSpawn( ent );
		return;
	}

	if ( clientNum < 0 || clientNum >= level.maxclients )
	{
		StopFollowing( ent );
		return;
	}

	targetClient = &level.clients[clientNum];
	if ( targetClient->pers.connected != CON_CONNECTED || targetClient->sess.sessionTeam == TEAM_SPECTATOR )
	{
		ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		return;
	}

	trap_Trace( &tr, ent->client->ps.origin, mins, maxs, targetClient->ps.origin, ent->s.number, CONTENTS_SOLID );
	if ( forceUpdate || tr.fraction < 1 )
	{
		if ( !FindBestObserverSpot(ent, &g_entities[clientNum], origin, angles) )
		{
			if (ent->updateTime + 500 < level.time){
				ent->updateTime = level.time;
				trap_SendServerCommand( ent - g_entities, "print \"Couldnt find valid observer spot, dropping back to follow mode.\n\"" );
				ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
				return;
			}
		}
		else
		{
//			Com_Printf( "Updating observer position" );

			G_SetOrigin(ent, origin);
			VectorCopy(origin, ent->client->ps.origin);
			VectorCopy(angles, ent->client->ps.viewangles);
			ent->updateTime = level.time;
		}
	}
	else {
		ent->updateTime = level.time;
	}
}
