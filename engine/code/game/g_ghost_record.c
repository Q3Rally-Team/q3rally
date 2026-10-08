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
// g_ghost_record.c -- server-side lap ghost recording for the ladder
//
// The server samples every eligible driver's position during a lap (the same
// sampling rules as the client-side personal ghost) and, when a lap is a new
// session best for that driver on this map, sends it to the ladder via
// trap_LadderSubmitGhost(). Recording on the server keeps the ghost as
// trustworthy as the lap time itself and needs no upload path from clients.
//
// The ghost is serialised in the regular .ghost text format, so the same file
// can later be played back by cgame or used as a bot route by g_ghost.c.
// Extra header keys (physics_version, map_checksum, player, player_id) are
// ignored by both existing parsers.

#include "g_local.h"

#define GHOST_REC_SLOTS                 MAX_CLIENTS
#define GHOST_REC_MAX_FRAMES            4096    // per lap
#define GHOST_REC_CHUNK_FRAMES          256
#define GHOST_REC_MAX_CHUNKS            ( GHOST_REC_MAX_FRAMES / GHOST_REC_CHUNK_FRAMES )
// Frames come from a shared pool in chunks of 256 instead of a fixed 4096
// frames per slot: a typical lap needs 2-4 chunks, so every client can record
// while the pool (about 2.5 MB) stays below the former 16 fixed slots
// (3.4 MB). An exhausted pool only invalidates that lap's upload.
#define GHOST_REC_POOL_CHUNKS           192
#define GHOST_REC_MIN_DISTANCE          96.0f   // units between samples on straights
#define GHOST_REC_MAX_INTERVAL          200     // ms, sample at least this often
#define GHOST_REC_MIN_TURN_INTERVAL     40      // ms
#define GHOST_REC_TURN_DEGREES          8.0f
#define GHOST_REC_TELEPORT_SPEED        6000.0f // units/s between samples => reset/teleport
#define GHOST_REC_MAX_SLIPSTREAM        5       // of SLIPSTREAM_CURRENT_MAX (~2 % less drag)
#define GHOST_REC_TELEPORT_MIN_DISTANCE 256.0f
#define GHOST_REC_MIN_LAP_MS            5000
#define GHOST_REC_MAX_START_OFFSET      1000    // ms; the ladder refuses later first samples

typedef struct {
	int             timeOffset;
	vec3_t          origin;
	vec3_t          angles;
	vec3_t          velocity;
	int             buttons;
	int             forwardmove;
	int             upmove;
} ghostRecFrame_t;

typedef struct {
	int             clientNum;              // -1 = free
	int             lapStartTime;
	int             lastSampleTime;         // offset of the last stored frame
	int             lastCheckpoint;
	vec3_t          lastOrigin;
	float           lastYaw;
	qboolean        invalid;                // teleport/reset/overflow: lap is not uploaded
	int             frameCount;
	int             chunkCount;
	int             chunks[GHOST_REC_MAX_CHUNKS];   // indices into s_recFramePool
} ghostRecSlot_t;

static ghostRecSlot_t   s_recSlots[GHOST_REC_SLOTS];
static ghostRecFrame_t  s_recFramePool[GHOST_REC_POOL_CHUNKS][GHOST_REC_CHUNK_FRAMES];
static qboolean         s_recChunkUsed[GHOST_REC_POOL_CHUNKS];
static int              s_recClientSlot[MAX_CLIENTS];
static int              s_recUploadedBestMs[MAX_CLIENTS];
static char             s_recMapName[MAX_QPATH];
#define s_recGhostText  g_ghostTextBuffer       // shared, see g_local.h
static ladderGhostMeta_t s_recMeta;

static ghostRecFrame_t *G_GhostRecord_Frame( const ghostRecSlot_t *slot, int index ) {
	return &s_recFramePool[slot->chunks[index / GHOST_REC_CHUNK_FRAMES]][index % GHOST_REC_CHUNK_FRAMES];
}

static void G_GhostRecord_FreeFrames( ghostRecSlot_t *slot ) {
	int i;

	for ( i = 0; i < slot->chunkCount; i++ ) {
		s_recChunkUsed[slot->chunks[i]] = qfalse;
	}
	slot->chunkCount = 0;
	slot->frameCount = 0;
}

/* Room for one more frame; qfalse when the lap or the pool is full. */
static qboolean G_GhostRecord_ReserveFrame( ghostRecSlot_t *slot ) {
	int i;

	if ( slot->frameCount < slot->chunkCount * GHOST_REC_CHUNK_FRAMES ) {
		return qtrue;
	}
	if ( slot->chunkCount >= GHOST_REC_MAX_CHUNKS ) {
		return qfalse;
	}
	for ( i = 0; i < GHOST_REC_POOL_CHUNKS; i++ ) {
		if ( !s_recChunkUsed[i] ) {
			s_recChunkUsed[i] = qtrue;
			slot->chunks[slot->chunkCount++] = i;
			return qtrue;
		}
	}
	return qfalse;
}

/*
=================
G_GhostRecord_Init

Called from G_InitGame: forget all recordings and session bests.
=================
*/
void G_GhostRecord_Init( void ) {
	int i;

	Com_Memset( s_recSlots, 0, sizeof( s_recSlots ) );
	Com_Memset( s_recChunkUsed, 0, sizeof( s_recChunkUsed ) );
	for ( i = 0; i < GHOST_REC_SLOTS; i++ ) {
		s_recSlots[i].clientNum = -1;
	}
	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		s_recClientSlot[i] = -1;
		s_recUploadedBestMs[i] = 0;
	}
	trap_Cvar_VariableStringBuffer( "mapname", s_recMapName, sizeof( s_recMapName ) );
}

static void G_GhostRecord_ReleaseSlot( int clientNum ) {
	int slotIndex;

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		return;
	}
	slotIndex = s_recClientSlot[clientNum];
	if ( slotIndex >= 0 && slotIndex < GHOST_REC_SLOTS ) {
		s_recSlots[slotIndex].clientNum = -1;
		G_GhostRecord_FreeFrames( &s_recSlots[slotIndex] );
	}
	s_recClientSlot[clientNum] = -1;
}

void G_GhostRecord_ClientDisconnect( int clientNum ) {
	G_GhostRecord_ReleaseSlot( clientNum );
	if ( clientNum >= 0 && clientNum < MAX_CLIENTS ) {
		s_recUploadedBestMs[clientNum] = 0;
	}
}

/* Driver opted out with cg_ghostShare 0. Missing key (older clients) = shared. */
static qboolean G_GhostRecord_ClientShares( int clientNum ) {
	char userinfo[MAX_INFO_STRING];
	const char *value;

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );
	value = Info_ValueForKey( userinfo, "cg_ghostShare" );
	if ( value && value[0] && atoi( value ) == 0 ) {
		return qfalse;
	}
	return qtrue;
}

/* Recording only makes sense when the result can reach the ladder. */
static qboolean G_GhostRecord_ClientEligible( gentity_t *ent ) {
	gclient_t *client = ent->client;

	if ( !g_ghostUpload.integer ) {
		return qfalse;
	}
	if ( !trap_Cvar_VariableIntegerValue( "sv_ladderEnabled" ) ) {
		return qfalse;
	}
	/* Cheats, timescale or changed physics (g_ladder_rules.c). Checked
	 * every frame: a lap with a change anywhere in it is never uploaded. */
	if ( G_LadderRulesViolation() ) {
		return qfalse;
	}
	if ( !BG_GametypeIsTimedRace( g_gametype.integer ) ) {
		return qfalse;
	}
	if ( !client || ( ent->r.svFlags & SVF_BOT ) ) {
		return qfalse;
	}
	if ( client->pers.connected != CON_CONNECTED || !client->pers.uuid[0] ) {
		return qfalse;
	}
	if ( client->sess.sessionTeam == TEAM_SPECTATOR || client->finishRaceTime ) {
		return qfalse;
	}
	if ( !level.startRaceTime || client->lapStartTime <= 0 || level.time < client->lapStartTime ) {
		return qfalse;
	}
	return qtrue;
}

static ghostRecSlot_t *G_GhostRecord_AllocSlot( int clientNum ) {
	int i;

	for ( i = 0; i < GHOST_REC_SLOTS; i++ ) {
		if ( s_recSlots[i].clientNum < 0 ) {
			s_recSlots[i].clientNum = clientNum;
			s_recClientSlot[clientNum] = i;
			return &s_recSlots[i];
		}
	}
	return NULL;
}

static void G_GhostRecord_StoreFrame( ghostRecSlot_t *slot, gclient_t *client, int timeOffset ) {
	ghostRecFrame_t *frame;

	if ( slot->frameCount >= GHOST_REC_MAX_FRAMES || !G_GhostRecord_ReserveFrame( slot ) ) {
		slot->invalid = qtrue;
		return;
	}
	if ( slot->frameCount > 0 && timeOffset < slot->lastSampleTime ) {
		timeOffset = slot->lastSampleTime;
	}

	frame = G_GhostRecord_Frame( slot, slot->frameCount++ );
	frame->timeOffset = timeOffset;
	VectorCopy( client->ps.origin, frame->origin );
	VectorCopy( client->ps.viewangles, frame->angles );
	VectorCopy( client->ps.velocity, frame->velocity );
	frame->buttons = client->pers.cmd.buttons;
	frame->forwardmove = client->pers.cmd.forwardmove;
	frame->upmove = client->pers.cmd.upmove;

	slot->lastSampleTime = timeOffset;
	slot->lastCheckpoint = client->ps.stats[STAT_NEXT_CHECKPOINT];
	VectorCopy( client->ps.origin, slot->lastOrigin );
	slot->lastYaw = client->ps.viewangles[YAW];
}

static void G_GhostRecord_BeginLap( ghostRecSlot_t *slot, gclient_t *client ) {
	int offset;

	slot->lapStartTime = client->lapStartTime;
	G_GhostRecord_FreeFrames( slot );
	slot->lastSampleTime = 0;
	slot->invalid = qfalse;

	offset = level.time - client->lapStartTime;
	if ( offset < 0 ) {
		offset = 0;
	}
	/* Recording started in the middle of the lap (driver became eligible
	 * late, e.g. after the rules went back to standard): never uploaded. */
	if ( offset > GHOST_REC_MAX_START_OFFSET ) {
		slot->invalid = qtrue;
	}
	G_GhostRecord_StoreFrame( slot, client, offset );
}

/*
=================
G_GhostRecord_ClientFrame

Called from ClientEndFrame for every active client.
=================
*/
void G_GhostRecord_ClientFrame( gentity_t *ent ) {
	gclient_t *client;
	ghostRecSlot_t *slot;
	int clientNum;
	int offset;
	int elapsed;
	float distance;
	float turn;
	qboolean checkpointChanged;

	if ( !ent || !ent->client ) {
		return;
	}
	client = ent->client;
	clientNum = ent - g_entities;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		return;
	}

	if ( !G_GhostRecord_ClientEligible( ent ) ) {
		G_GhostRecord_ReleaseSlot( clientNum );
		return;
	}

	slot = s_recClientSlot[clientNum] >= 0 ? &s_recSlots[s_recClientSlot[clientNum]] : NULL;
	if ( !slot || slot->lapStartTime != client->lapStartTime ) {
		// New lap (or first lap): check the opt-out once per lap.
		if ( !G_GhostRecord_ClientShares( clientNum ) ) {
			G_GhostRecord_ReleaseSlot( clientNum );
			return;
		}
		if ( !slot ) {
			slot = G_GhostRecord_AllocSlot( clientNum );
			if ( !slot ) {
				return;
			}
		}
		G_GhostRecord_BeginLap( slot, client );
		return;
	}

	if ( slot->invalid ) {
		return;
	}

	if ( BG_SlipstreamCurrent( &client->ps ) > GHOST_REC_MAX_SLIPSTREAM ) {
		// Towed along in another car's wake: not comparable to a solo lap.
		slot->invalid = qtrue;
		return;
	}

	offset = level.time - slot->lapStartTime;
	elapsed = offset - slot->lastSampleTime;
	if ( elapsed <= 0 ) {
		return;
	}

	distance = Distance( client->ps.origin, slot->lastOrigin );
	if ( distance > GHOST_REC_TELEPORT_MIN_DISTANCE &&
		distance * 1000.0f / (float)elapsed > GHOST_REC_TELEPORT_SPEED ) {
		// Reset to a checkpoint, respawn or other teleport: not a clean lap.
		slot->invalid = qtrue;
		return;
	}

	checkpointChanged = client->ps.stats[STAT_NEXT_CHECKPOINT] != slot->lastCheckpoint;
	turn = fabs( AngleSubtract( client->ps.viewangles[YAW], slot->lastYaw ) );

	if ( checkpointChanged || distance >= GHOST_REC_MIN_DISTANCE ||
		elapsed >= GHOST_REC_MAX_INTERVAL ||
		( elapsed >= GHOST_REC_MIN_TURN_INTERVAL && turn >= GHOST_REC_TURN_DEGREES ) ) {
		G_GhostRecord_StoreFrame( slot, client, offset );
	}
}

/* Vehicle model without skin, lower case (same rule as the player profile). */
static void G_GhostRecord_VehicleName( int clientNum, char *out, int outSize ) {
	char userinfo[MAX_INFO_STRING];
	const char *model;
	int i;

	out[0] = '\0';
	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );
	model = Info_ValueForKey( userinfo, "model" );
	if ( !model ) {
		return;
	}
	for ( i = 0; i < outSize - 1 && model[i] && model[i] != '/'; i++ ) {
		char c = model[i];
		out[i] = ( c >= 'A' && c <= 'Z' ) ? c + ( 'a' - 'A' ) : c;
	}
	out[i] = '\0';
}

/* Printable ASCII only: the name ends up in a text header line and in JSON. */
static void G_GhostRecord_SafeName( const char *in, char *out, int outSize ) {
	char clean[MAX_NETNAME];
	int i, n = 0;

	Q_strncpyz( clean, in ? in : "", sizeof( clean ) );
	Q_CleanStr( clean );
	for ( i = 0; clean[i] && n < outSize - 1; i++ ) {
		unsigned char c = (unsigned char)clean[i];
		if ( c >= 0x20 && c < 0x7f ) {
			out[n++] = (char)c;
		}
	}
	out[n] = '\0';
	if ( !n ) {
		Q_strncpyz( out, "Player", outSize );
	}
}

static qboolean G_GhostRecord_Append( int *length, const char *text ) {
	int len = strlen( text );

	if ( *length + len >= LADDER_GHOST_MAX_DATA ) {
		return qfalse;
	}
	Com_Memcpy( s_recGhostText + *length, text, len );
	*length += len;
	s_recGhostText[*length] = '\0';
	return qtrue;
}

static qboolean G_GhostRecord_BuildText( const ghostRecSlot_t *slot, const ladderGhostMeta_t *meta, int *lengthOut ) {
	int length = 0;
	int i;

	s_recGhostText[0] = '\0';
	if ( !G_GhostRecord_Append( &length, "# Q3Rally server ghost\n" ) ||
		!G_GhostRecord_Append( &length, va( "map %s\n", meta->map ) ) ||
		!G_GhostRecord_Append( &length, va( "vehicle %s\n", meta->vehicle ) ) ||
		!G_GhostRecord_Append( &length, va( "track_length %d\n", meta->trackLength ) ) ||
		!G_GhostRecord_Append( &length, va( "track_reversed %d\n", meta->trackReversed ) ) ||
		!G_GhostRecord_Append( &length, va( "best_time_ms %d\n", meta->lapMs ) ) ||
		!G_GhostRecord_Append( &length, va( "physics_version %d\n", meta->physicsVersion ) ) ||
		!G_GhostRecord_Append( &length, va( "map_checksum %d\n", meta->mapChecksum ) ) ||
		!G_GhostRecord_Append( &length, va( "player %s\n", meta->playerName ) ) ||
		!G_GhostRecord_Append( &length, va( "player_id %s\n", meta->playerId ) ) ||
		!G_GhostRecord_Append( &length, va( "frames %d\n", slot->frameCount ) ) ) {
		return qfalse;
	}

	for ( i = 0; i < slot->frameCount; i++ ) {
		const ghostRecFrame_t *f = G_GhostRecord_Frame( slot, i );
		if ( !G_GhostRecord_Append( &length, va( "%d %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %d %d %d\n",
			f->timeOffset,
			f->origin[0], f->origin[1], f->origin[2],
			f->angles[0], f->angles[1], f->angles[2],
			f->velocity[0], f->velocity[1], f->velocity[2],
			f->buttons, f->forwardmove, f->upmove ) ) ) {
			return qfalse;
		}
	}

	*lengthOut = length;
	return qtrue;
}

/*
=================
G_GhostRecord_LapComplete

Called from G_RallyCompleteLap before the next lap starts. Uploads the lap
when it is a clean recording and the driver's best lap of this session.
=================
*/
void G_GhostRecord_LapComplete( gentity_t *ent, int lapStartTime, int timestamp ) {
	gclient_t *client;
	ghostRecSlot_t *slot;
	int clientNum;
	int lapMs;
	int length;
	char uuidShort[9];

	if ( !ent || !ent->client ) {
		return;
	}
	client = ent->client;
	clientNum = ent - g_entities;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS || s_recClientSlot[clientNum] < 0 ) {
		return;
	}
	slot = &s_recSlots[s_recClientSlot[clientNum]];
	if ( slot->invalid || slot->lapStartTime != lapStartTime || slot->frameCount < 2 ) {
		return;
	}

	lapMs = timestamp - lapStartTime;
	if ( lapMs < GHOST_REC_MIN_LAP_MS ) {
		return;
	}

	// Finish anchor at the exact lap time.
	G_GhostRecord_StoreFrame( slot, client, lapMs );
	if ( slot->invalid ) {
		return;
	}
	G_GhostRecord_Frame( slot, slot->frameCount - 1 )->timeOffset = lapMs;

	if ( s_recUploadedBestMs[clientNum] > 0 && lapMs >= s_recUploadedBestMs[clientNum] ) {
		return;
	}

	Com_Memset( &s_recMeta, 0, sizeof( s_recMeta ) );
	Q_strncpyz( s_recMeta.map, s_recMapName, sizeof( s_recMeta.map ) );
	G_GhostRecord_VehicleName( clientNum, s_recMeta.vehicle, sizeof( s_recMeta.vehicle ) );
	if ( !s_recMeta.map[0] || !s_recMeta.vehicle[0] ) {
		return;
	}
	Q_strncpyz( s_recMeta.playerId, client->pers.uuid, sizeof( s_recMeta.playerId ) );
	G_GhostRecord_SafeName( client->pers.netname, s_recMeta.playerName, sizeof( s_recMeta.playerName ) );
	s_recMeta.trackLength = ( g_trackLength.integer >= 0 && g_trackLength.integer <= 2 ) ? g_trackLength.integer : 0;
	s_recMeta.trackReversed = g_trackReversed.integer ? 1 : 0;
	s_recMeta.lapMs = lapMs;
	s_recMeta.frameCount = slot->frameCount;
	s_recMeta.gametype = g_gametype.integer;
	s_recMeta.physicsVersion = BG_PHYSICS_VERSION;
	s_recMeta.mapChecksum = trap_Cvar_VariableIntegerValue( "sv_mapChecksum" );
	s_recMeta.courseLengthUnits = (int)level.trackLength;
	s_recMeta.sprintTrack = G_IsSprintTrack() ? 1 : 0;

	Q_strncpyz( uuidShort, client->pers.uuid, sizeof( uuidShort ) );
	Com_sprintf( s_recMeta.ghostId, sizeof( s_recMeta.ghostId ), "%s-%s-%d",
		s_recMeta.map, uuidShort, lapMs );

	if ( !G_GhostRecord_BuildText( slot, &s_recMeta, &length ) ) {
		G_Printf( "Ghost: lap of %s too large to upload (%d frames)\n",
			s_recMeta.playerName, slot->frameCount );
		return;
	}
	s_recMeta.dataLength = length;
	s_recMeta.valid = 1;

	trap_LadderSubmitGhost( &s_recMeta, s_recGhostText );
	s_recUploadedBestMs[clientNum] = lapMs;
	G_Printf( "Ghost: queued %s lap %d ms on %s (%s, %d frames)\n",
		s_recMeta.playerName, lapMs, s_recMeta.map, s_recMeta.vehicle, slot->frameCount );
}
