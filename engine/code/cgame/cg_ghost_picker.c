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
// cg_ghost_picker.c -- Ghost Race: pick a ladder ghost as opponent
//
// The server sends the ladder ranking for this map (lghostlist/lghostents,
// see g_ghost_ladder.c). Before the race starts an overlay lets the player
// pick one ghost; by default only ghosts of the player's own car are listed,
// TAB shows all cars. The picked ghost is loaded from the local ghosts/ladder/
// cache when possible, otherwise the server streams it (lghostmeta/data/done).
//
// The pick is remembered in cg_ladderGhostLast for the rest of the session,
// so a vid_restart or a restart of the same map selects it again.
//
// Client commands to the server (lghostlistreq, lghostpick, ghostopp) are
// repeated until the server answers: a dedicated server drops every client
// command that arrives within a second of the previous one (sv_floodProtect),
// and the HUD asks for "score" every two seconds. A command is only sent when
// the last client command is GHOSTCMD_GAP_MS old; the next "score" request is
// pushed back by the same amount.

#include "cg_local.h"
#include "cg_hud_elements.h"
#include "../client/keycodes.h"

#define LADDER_PICKER_ROWS      9
#define LADDER_PICKER_X         150.0f
#define LADDER_PICKER_Y         96.0f
#define LADDER_PICKER_W         340.0f
#define LADDER_PICKER_ROW_H     18.0f

#define GHOSTCMD_GAP_MS         1200    // distance to the previous client command
#define GHOSTCMD_RETRY_MS       3000    // resend when there is no answer by then
#define GHOSTCMD_PICK_WAIT_MS   45000   // pick taken: ghost data has to start by then

static const int ghostCmdMaxTries[GHOSTCMD_KINDS] = { 5, 5, 8, 3 };

static void CG_LadderGhost_FailTransfer( const char *reason );

/* Time since an earlier cg.time; a cg.time that went backwards counts as long ago. */
static int CG_GhostCmd_Since( int then ) {
	return cg.time >= then ? cg.time - then : 0x7fffffff;
}

static qboolean CG_GhostCmd_GapFree( void ) {
	if ( cg.ghostCmdAnySent && CG_GhostCmd_Since( cg.ghostCmdLastSent ) < GHOSTCMD_GAP_MS ) {
		return qfalse;
	}
	/* "score" from the HUD, the scoreboard or +scores */
	return CG_GhostCmd_Since( cg.scoresRequestTime ) >= GHOSTCMD_GAP_MS;
}

/* Sends the first due ghost command, at most one per call. */
static void CG_GhostCmd_Pump( void ) {
	int kind;

	if ( !CG_GhostCmd_GapFree() ) {
		return;
	}
	for ( kind = 0; kind < GHOSTCMD_KINDS; kind++ ) {
		qboolean due;

		if ( !cg.ghostCmds[kind].pending ) {
			continue;
		}
		due = !cg.ghostCmds[kind].sent || CG_GhostCmd_Since( cg.ghostCmds[kind].sentAt ) >= GHOSTCMD_RETRY_MS;
		if ( !due ) {
			continue;
		}
		if ( cg.ghostCmds[kind].tries >= ghostCmdMaxTries[kind] ) {
			cg.ghostCmds[kind].pending = qfalse;
			if ( kind == GHOSTCMD_PICK && cg.ladderGhostPending ) {
				CG_LadderGhost_FailTransfer( "no answer from the server" );
			}
			continue;
		}
		trap_SendClientCommand( cg.ghostCmds[kind].text );
		cg.ghostCmds[kind].sent = qtrue;
		cg.ghostCmds[kind].sentAt = cg.time;
		cg.ghostCmds[kind].tries++;
		cg.ghostCmdAnySent = qtrue;
		cg.ghostCmdLastSent = cg.time;
		/* The next "score" request waits until this one is through. */
		cg.scoresRequestTime = cg.time - 2000 + GHOSTCMD_GAP_MS;
		return;
	}
}

/* Queues a command (replacing an older one of the same kind) and sends it
 * right away when the gap to the last client command allows. */
static void CG_GhostCmd_Set( int kind, const char *text ) {
	Q_strncpyz( cg.ghostCmds[kind].text, text, sizeof( cg.ghostCmds[kind].text ) );
	cg.ghostCmds[kind].pending = qtrue;
	cg.ghostCmds[kind].sent = qfalse;
	cg.ghostCmds[kind].sentAt = 0;
	cg.ghostCmds[kind].tries = 0;
	CG_GhostCmd_Pump();
}

static void CG_GhostCmd_Done( int kind ) {
	cg.ghostCmds[kind].pending = qfalse;
}

/*
=================
CG_LadderGhost_NetFrame

Called every frame from CG_DrawActiveFrame: resends unanswered ghost
commands (in every race mode: the racing line asks for the server route)
and, in Ghost Race, gives up on a pick whose ghost data never comes.
=================
*/
void CG_LadderGhost_NetFrame( void ) {
	if ( cgs.gametype == GT_GHOST && cg.ladderGhostPending && cg.ladderGhostPickAcked && !cg.ladderGhostTransferExpected &&
	     CG_GhostCmd_Since( cg.ladderGhostPickAckedAt ) >= GHOSTCMD_PICK_WAIT_MS ) {
		CG_LadderGhost_FailTransfer( "timeout" );
	}
	CG_GhostCmd_Pump();
}

/*
=================
CG_GhostRoute_Request

Racing line outside Ghost Race: asks the server for its ghost route, which
it otherwise only sends in Ghost Race. The answer is "ghostmeta".
=================
*/
void CG_GhostRoute_Request( void ) {
	CG_GhostCmd_Set( GHOSTCMD_ROUTE, "ghostroutereq" );
}

void CG_GhostRoute_Answered( void ) {
	CG_GhostCmd_Done( GHOSTCMD_ROUTE );
}

static void CG_LadderGhost_OwnVehicle( char *out, int outSize ) {
	const char *model = "";
	int i;

	if ( cg.clientNum >= 0 && cg.clientNum < MAX_CLIENTS ) {
		model = cgs.clientinfo[cg.clientNum].modelName;
	}
	for ( i = 0; i < outSize - 1 && model[i] && model[i] != '/'; i++ ) {
		out[i] = tolower( model[i] );
	}
	out[i] = '\0';
}

/* Indices of the entries shown with the current filter. */
static int CG_LadderGhost_Visible( int *indices, int maxIndices ) {
	char ownVehicle[MAX_QPATH];
	int count = 0;
	int i;

	CG_LadderGhost_OwnVehicle( ownVehicle, sizeof( ownVehicle ) );
	for ( i = 0; i < cg.ladderGhostEntryCount && count < maxIndices; i++ ) {
		if ( !cg.ladderPickerAllVehicles && ownVehicle[0] &&
		     Q_stricmp( cg.ladderGhostEntries[i].vehicle, ownVehicle ) ) {
			continue;
		}
		indices[count++] = i;
	}
	return count;
}

static qboolean CG_LadderGhost_RaceStarted( void ) {
	if ( !cg.snap || cg.snap->ps.clientNum >= MAX_CLIENTS ) {
		return qfalse;
	}
	return cg_entities[cg.snap->ps.clientNum].startRaceTime != 0;
}

static void CG_LadderGhost_ClearRecording( void ) {
	cg.ladderGhost.valid = qfalse;
	cg.ladderGhost.frameCount = 0;
	cg.ladderGhost.startIndex = 0;
	cg.ladderGhost.writeIndex = 0;
	cg.ladderGhost.duration = 0;
	cg.ladderGhostAvailable = qfalse;
	cg.ladderGhostPending = qfalse;
	cg.ladderGhostFailed = qfalse;
	cg.ladderGhostTransferExpected = 0;
	cg.ladderGhostTransferReceived = 0;
	/* A running pick is answered or abandoned with the recording. */
	CG_GhostCmd_Done( GHOSTCMD_PICK );
	cg.ladderGhostPickAcked = qfalse;
}

/*
=================
CG_LadderGhost_Reset

Called from CG_Init.
=================
*/
void CG_LadderGhost_Reset( void ) {
	cg.ladderGhostEntryCount = 0;
	cg.ladderGhostListExpected = 0;
	cg.ladderGhostListReady = qfalse;
	cg.ladderGhostListFromCache = qfalse;
	cg.ladderGhostSelected = -1;
	cg.ladderPickerOpen = qfalse;
	cg.ladderPickerAutoShown = qfalse;
	cg.ladderPickerAllVehicles = qfalse;
	cg.ladderPickerCursor = 0;
	cg.ladderPickerScroll = 0;
	CG_LadderGhost_ClearRecording();
	memset( cg.ghostCmds, 0, sizeof( cg.ghostCmds ) );
	cg.ghostOpponentPendingMs = 0;
}

/* Asks the server for the list again; covers a cgame restart (vid_restart). */
void CG_LadderGhost_RequestList( void ) {
	if ( cgs.gametype == GT_GHOST ) {
		CG_GhostCmd_Set( GHOSTCMD_LIST, "lghostlistreq" );
	}
}

static void CG_LadderGhost_Remember( int entryIndex ) {
	char mapname[MAX_QPATH];

	COM_StripExtension( COM_SkipPath( cgs.mapname ), mapname, sizeof( mapname ) );
	if ( entryIndex < 0 ) {
		trap_Cvar_Set( "cg_ladderGhostLast", va( "none|%s", mapname ) );
		return;
	}
	trap_Cvar_Set( "cg_ladderGhostLast", va( "%s|%d|%s|%s", mapname,
		cg.ladderGhostEntries[entryIndex].lapMs, cg.ladderGhostEntries[entryIndex].vehicle,
		cg.ladderGhostEntries[entryIndex].name ) );
}

/* Entry that matches the remembered pick, -1 none, -2 "no ladder ghost". */
static int CG_LadderGhost_RememberedEntry( void ) {
	char last[256];
	char mapname[MAX_QPATH];
	int i;

	trap_Cvar_VariableStringBuffer( "cg_ladderGhostLast", last, sizeof( last ) );
	if ( !last[0] ) {
		return -1;
	}
	COM_StripExtension( COM_SkipPath( cgs.mapname ), mapname, sizeof( mapname ) );
	if ( !Q_stricmp( last, va( "none|%s", mapname ) ) ) {
		return -2;
	}
	for ( i = 0; i < cg.ladderGhostEntryCount; i++ ) {
		const ladderGhostEntry_t *entry = &cg.ladderGhostEntries[i];
		if ( !Q_stricmp( last, va( "%s|%d|%s|%s", mapname, entry->lapMs, entry->vehicle, entry->name ) ) ) {
			return i;
		}
	}
	return -1;
}

/*
=================
CG_LadderGhost_Pick

entryIndex -1 drops the ladder ghost (back to personal / server base).
=================
*/
void CG_LadderGhost_Pick( int entryIndex ) {
	const ladderGhostEntry_t *entry;
	fileHandle_t f;
	int length;

	CG_LadderGhost_ClearRecording();
	if ( entryIndex < 0 || entryIndex >= cg.ladderGhostEntryCount ) {
		cg.ladderGhostSelected = -1;
		CG_LadderGhost_Remember( -1 );
		return;
	}

	entry = &cg.ladderGhostEntries[entryIndex];
	cg.ladderGhostSelected = entryIndex;
	CG_LadderGhost_Remember( entryIndex );
	/* Load the ghost's car now, not in the middle of the race. */
	CG_PrecacheGhostVehicle( entry->vehicle );

	/* Local server or picked before: the cache already holds the ghost. */
	length = trap_FS_FOpenFile( entry->cacheFile, &f, FS_READ );
	if ( f ) {
		trap_FS_FCloseFile( f );
	}
	if ( length > 0 && CG_LoadLadderGhostFile( entry->cacheFile, entry->lapMs ) ) {
		cg.ladderGhostAvailable = qtrue;
		CG_GhostRace_ReportOpponent();
		return;
	}

	cg.ladderGhostPending = qtrue;
	CG_GhostCmd_Set( GHOSTCMD_PICK, va( "lghostpick %d", entryIndex ) );
}

/* -------------------------------------------------------------------------
   Server commands
   ------------------------------------------------------------------------- */

static void CG_LadderGhost_FailTransfer( const char *reason ) {
	CG_LadderGhost_ClearRecording();
	cg.ladderGhostFailed = qtrue;
	CG_Printf( "Ladder ghost transfer failed: %s.\n", reason );
}

static void CG_LadderGhost_ListDone( void ) {
	int remembered;

	cg.ladderGhostListReady = qtrue;
	if ( cg_developer.integer ) {
		CG_Printf( "Received %d ladder ghosts%s.\n", cg.ladderGhostEntryCount,
			cg.ladderGhostListFromCache ? " (server cache)" : "" );
	}

	remembered = CG_LadderGhost_RememberedEntry();
	if ( remembered >= 0 && cg.ladderGhostAvailable && cg.ladderGhost.valid ) {
		/* map_restart: the loaded ghost is still the remembered one. */
		cg.ladderGhostSelected = remembered;
		cg.ladderPickerAutoShown = qtrue;
	} else if ( remembered >= 0 ) {
		CG_LadderGhost_Pick( remembered );
		cg.ladderPickerAutoShown = qtrue;
	} else if ( remembered == -2 ) {
		cg.ladderPickerAutoShown = qtrue;
	} else if ( cg.ladderGhostSelected >= 0 ) {
		/* The list changed under an active pick. */
		CG_LadderGhost_Pick( -1 );
	}
}

qboolean CG_LadderGhost_ServerCommand( const char *cmd ) {
	if ( !Q_stricmp( cmd, "lghostlist" ) ) {
		int total = atoi( CG_Argv( 1 ) );

		CG_GhostCmd_Done( GHOSTCMD_LIST );
		cg.ladderGhostEntryCount = 0;
		cg.ladderGhostListExpected = total < 0 ? 0 : ( total > MAX_LADDER_GHOST_ENTRIES ? MAX_LADDER_GHOST_ENTRIES : total );
		cg.ladderGhostListFromCache = atoi( CG_Argv( 2 ) ) ? qtrue : qfalse;
		cg.ladderGhostListReady = qfalse;
		cg.ladderPickerCursor = 0;
		cg.ladderPickerScroll = 0;
		return qtrue;
	}

	if ( !Q_stricmp( cmd, "lghostents" ) ) {
		int first = atoi( CG_Argv( 1 ) );
		int count = atoi( CG_Argv( 2 ) );
		int i;

		if ( first != cg.ladderGhostEntryCount || count < 1 || trap_Argc() != 3 + count * 4 ) {
			return qtrue;
		}
		for ( i = 0; i < count && cg.ladderGhostEntryCount < MAX_LADDER_GHOST_ENTRIES; i++ ) {
			ladderGhostEntry_t *entry = &cg.ladderGhostEntries[cg.ladderGhostEntryCount];
			int arg = 3 + i * 4;

			entry->lapMs = atoi( CG_Argv( arg ) );
			Q_strncpyz( entry->vehicle, CG_Argv( arg + 1 ), sizeof( entry->vehicle ) );
			Q_strncpyz( entry->name, CG_Argv( arg + 2 ), sizeof( entry->name ) );
			Q_strncpyz( entry->cacheFile, CG_Argv( arg + 3 ), sizeof( entry->cacheFile ) );
			cg.ladderGhostEntryCount++;
		}
		return qtrue;
	}

	if ( !Q_stricmp( cmd, "lghostlistdone" ) ) {
		CG_LadderGhost_ListDone();
		return qtrue;
	}

	if ( !Q_stricmp( cmd, "lghostmeta" ) ) {
		int entry = atoi( CG_Argv( 1 ) );
		int count = atoi( CG_Argv( 3 ) );

		if ( entry != cg.ladderGhostSelected ) {
			return qtrue;
		}
		/* A repeated pick crossed the finished transfer: keep the ghost. */
		if ( !cg.ladderGhostPending && cg.ladderGhostAvailable && cg.ladderGhost.valid ) {
			return qtrue;
		}
		CG_LadderGhost_ClearRecording();
		if ( count < 2 || count > MAX_GHOST_FRAMES ) {
			CG_LadderGhost_FailTransfer( "invalid size" );
			return qtrue;
		}
		cg.ladderGhostPending = qtrue;
		cg.ladderGhostTransferExpected = count;
		return qtrue;
	}

	if ( !Q_stricmp( cmd, "lghostdata" ) ) {
		int first = atoi( CG_Argv( 1 ) );
		int count = atoi( CG_Argv( 2 ) );
		int i;

		if ( !cg.ladderGhostPending || !cg.ladderGhostTransferExpected ) {
			return qtrue;
		}
		if ( count < 1 || count > 32 || first != cg.ladderGhostTransferReceived ||
		     first + count > cg.ladderGhostTransferExpected || trap_Argc() != 3 + count * 7 ) {
			CG_LadderGhost_FailTransfer( "invalid chunk sequence" );
			return qtrue;
		}
		for ( i = 0; i < count; i++ ) {
			int arg = 3 + i * 7;
			ghostFrame_t *frame = &cg.ladderGhost.frames[cg.ladderGhostTransferReceived];
			int timeOffset = atoi( CG_Argv( arg ) );

			if ( cg.ladderGhostTransferReceived > 0 &&
			     timeOffset < cg.ladderGhost.frames[cg.ladderGhostTransferReceived - 1].timeOffset ) {
				CG_LadderGhost_FailTransfer( "timestamps are not ordered" );
				return qtrue;
			}
			memset( frame, 0, sizeof( *frame ) );
			frame->timeOffset = timeOffset;
			frame->origin[0] = atof( CG_Argv( arg + 1 ) );
			frame->origin[1] = atof( CG_Argv( arg + 2 ) );
			frame->origin[2] = atof( CG_Argv( arg + 3 ) );
			frame->angles[0] = atof( CG_Argv( arg + 4 ) );
			frame->angles[1] = atof( CG_Argv( arg + 5 ) );
			frame->angles[2] = atof( CG_Argv( arg + 6 ) );
			cg.ladderGhostTransferReceived++;
		}
		return qtrue;
	}

	if ( !Q_stricmp( cmd, "lghostdone" ) ) {
		int entry = atoi( CG_Argv( 1 ) );
		int i;

		if ( entry != cg.ladderGhostSelected || !cg.ladderGhostPending ) {
			return qtrue;
		}
		if ( cg.ladderGhostTransferReceived != cg.ladderGhostTransferExpected ||
		     cg.ladderGhostTransferReceived < 2 ) {
			CG_LadderGhost_FailTransfer( "ghost is incomplete" );
			return qtrue;
		}
		/* Velocity from neighbouring samples, for the wheel animation. */
		for ( i = 0; i < cg.ladderGhostTransferReceived; i++ ) {
			ghostFrame_t *a = &cg.ladderGhost.frames[i > 0 ? i - 1 : 0];
			ghostFrame_t *b = &cg.ladderGhost.frames[i + 1 < cg.ladderGhostTransferReceived ? i + 1 : i];
			int dt = b->timeOffset - a->timeOffset;

			if ( dt > 0 ) {
				VectorSubtract( b->origin, a->origin, cg.ladderGhost.frames[i].velocity );
				VectorScale( cg.ladderGhost.frames[i].velocity, 1000.0f / dt, cg.ladderGhost.frames[i].velocity );
			}
		}
		cg.ladderGhost.frameCount = cg.ladderGhostTransferReceived;
		cg.ladderGhost.startIndex = 0;
		cg.ladderGhost.writeIndex = cg.ladderGhost.frameCount % MAX_GHOST_FRAMES;
		cg.ladderGhost.duration = cg.ladderGhost.frames[cg.ladderGhost.frameCount - 1].timeOffset;
		cg.ladderGhost.valid = qtrue;
		cg.ladderGhostAvailable = qtrue;
		cg.ladderGhostPending = qfalse;
		cg.ladderGhostFailed = qfalse;
		if ( cg_developer.integer ) {
			CG_Printf( "Received ladder ghost (%d samples).\n", cg.ladderGhost.frameCount );
		}
		CG_GhostRace_ReportOpponent();
		return qtrue;
	}

	if ( !Q_stricmp( cmd, "lghostpickok" ) ) {
		if ( atoi( CG_Argv( 1 ) ) == cg.ladderGhostSelected && cg.ladderGhostPending ) {
			CG_GhostCmd_Done( GHOSTCMD_PICK );
			cg.ladderGhostPickAcked = qtrue;
			cg.ladderGhostPickAckedAt = cg.time;
		}
		return qtrue;
	}

	if ( !Q_stricmp( cmd, "lghostoppok" ) ) {
		if ( cg.ghostCmds[GHOSTCMD_OPP].pending && atoi( CG_Argv( 1 ) ) == cg.ghostOpponentPendingMs ) {
			CG_GhostCmd_Done( GHOSTCMD_OPP );
		}
		return qtrue;
	}

	if ( !Q_stricmp( cmd, "lghostresult" ) ) {
		int client = atoi( CG_Argv( 1 ) );

		if ( client >= 0 && client < MAX_CLIENTS && trap_Argc() >= 6 ) {
			cg.ghostRaceResults[client].valid = qtrue;
			cg.ghostRaceResults[client].won = atoi( CG_Argv( 2 ) ) ? qtrue : qfalse;
			cg.ghostRaceResults[client].playerMs = atoi( CG_Argv( 3 ) );
			cg.ghostRaceResults[client].ghostMs = atoi( CG_Argv( 4 ) );
			Q_strncpyz( cg.ghostRaceResults[client].name, CG_Argv( 5 ), sizeof( cg.ghostRaceResults[client].name ) );
			/* The server's lap times are authoritative for the own banner. */
			if ( cg.snap && client == cg.snap->ps.clientNum ) {
				if ( !cg.ghostResultValid ) {
					cg.ghostResultTime = cg.time;
				}
				cg.ghostResultValid = qtrue;
				cg.ghostFinishOnly = qfalse;
				cg.ghostResultWon = cg.ghostRaceResults[client].won;
				cg.ghostResultPlayerMs = cg.ghostRaceResults[client].playerMs;
				cg.ghostResultGhostMs = cg.ghostRaceResults[client].ghostMs;
				Q_strncpyz( cg.ghostResultName, cg.ghostRaceResults[client].name, sizeof( cg.ghostResultName ) );
			}
		}
		return qtrue;
	}

	if ( !Q_stricmp( cmd, "lghostfail" ) ) {
		int entry = atoi( CG_Argv( 1 ) );

		if ( entry == cg.ladderGhostSelected ) {
			CG_LadderGhost_FailTransfer( CG_Argv( 2 ) );
		}
		return qtrue;
	}

	return qfalse;
}

/* -------------------------------------------------------------------------
   Picker overlay
   ------------------------------------------------------------------------- */

qboolean CG_LadderGhost_PickerIsOpen( void ) {
	return cg.ladderPickerOpen;
}

static void CG_LadderGhost_SetPickerOpen( qboolean open ) {
	if ( open == cg.ladderPickerOpen ) {
		return;
	}
	cg.ladderPickerOpen = open;
	if ( open ) {
		cg.ladderPickerCursor = 0;
		cg.ladderPickerScroll = 0;
		trap_Key_SetCatcher( trap_Key_GetCatcher() | KEYCATCH_CGAME );
	} else if ( !CG_HUDOptionsIsOpen() ) {
		trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_CGAME );
	}
}

void CG_LadderGhost_ClosePicker( void ) {
	CG_LadderGhost_SetPickerOpen( qfalse );
}

/* Console command "ghostpicker". */
void CG_LadderGhost_TogglePicker_f( void ) {
	if ( cgs.gametype != GT_GHOST ) {
		CG_Printf( "The ghost picker is only available in Ghost Race.\n" );
		return;
	}
	if ( !cg.ladderGhostListReady ) {
		CG_Printf( "No ladder ghost list from the server yet.\n" );
		return;
	}
	CG_LadderGhost_SetPickerOpen( !cg.ladderPickerOpen );
	cg.ladderPickerManual = cg.ladderPickerOpen;
}

/* The ESC key clears the catcher in the engine and lands here. */
void CG_LadderGhost_CatcherCleared( void ) {
	cg.ladderPickerOpen = qfalse;
}

static void CG_LadderGhost_MoveCursor( int delta, int rows ) {
	cg.ladderPickerCursor += delta;
	if ( cg.ladderPickerCursor < 0 ) {
		cg.ladderPickerCursor = 0;
	}
	if ( cg.ladderPickerCursor > rows - 1 ) {
		cg.ladderPickerCursor = rows - 1;
	}
	if ( cg.ladderPickerCursor < cg.ladderPickerScroll ) {
		cg.ladderPickerScroll = cg.ladderPickerCursor;
	}
	if ( cg.ladderPickerCursor >= cg.ladderPickerScroll + LADDER_PICKER_ROWS ) {
		cg.ladderPickerScroll = cg.ladderPickerCursor - LADDER_PICKER_ROWS + 1;
	}
}

/* Returns qtrue when the key was used by the picker. Row 0 is "no ladder ghost". */
qboolean CG_LadderGhost_KeyEvent( int key ) {
	int visible[MAX_LADDER_GHOST_ENTRIES];
	int count;
	int rows;

	if ( !cg.ladderPickerOpen ) {
		return qfalse;
	}

	count = CG_LadderGhost_Visible( visible, MAX_LADDER_GHOST_ENTRIES );
	rows = count + 1;

	switch ( key ) {
	case K_UPARROW:
	case K_KP_UPARROW:
	case K_MWHEELUP:
	case K_PAD0_DPAD_UP:
		CG_LadderGhost_MoveCursor( -1, rows );
		break;
	case K_DOWNARROW:
	case K_KP_DOWNARROW:
	case K_MWHEELDOWN:
	case K_PAD0_DPAD_DOWN:
		CG_LadderGhost_MoveCursor( 1, rows );
		break;
	case K_PGUP:
		CG_LadderGhost_MoveCursor( -LADDER_PICKER_ROWS, rows );
		break;
	case K_PGDN:
		CG_LadderGhost_MoveCursor( LADDER_PICKER_ROWS, rows );
		break;
	case K_TAB:
	case K_PAD0_X:
		cg.ladderPickerAllVehicles = !cg.ladderPickerAllVehicles;
		cg.ladderPickerCursor = 0;
		cg.ladderPickerScroll = 0;
		break;
	case K_ENTER:
	case K_KP_ENTER:
	case K_MOUSE1:
	case K_PAD0_A:
		if ( cg.ladderPickerCursor <= 0 || cg.ladderPickerCursor > count ) {
			CG_LadderGhost_Pick( -1 );
		} else {
			CG_LadderGhost_Pick( visible[cg.ladderPickerCursor - 1] );
		}
		CG_LadderGhost_SetPickerOpen( qfalse );
		break;
	case K_BACKSPACE:
	case K_PAD0_B:
		CG_LadderGhost_SetPickerOpen( qfalse );
		break;
	default:
		break;
	}
	return qtrue;
}

static void CG_LadderGhost_FormatTime( int ms, char *out, int outSize ) {
	Com_sprintf( out, outSize, "%d:%02d.%03d", ms / 60000, ( ms / 1000 ) % 60, ms % 1000 );
}

/*
=================
CG_LadderGhost_Frame

Opens the picker once per map in Ghost Race and closes it when the race
starts. Called every frame before drawing.
=================
*/
static void CG_LadderGhost_Frame( void ) {
	int mode;

	if ( cgs.gametype != GT_GHOST ) {
		return;
	}
	if ( cg.ladderPickerOpen ) {
		if ( !cg.ladderPickerManual && CG_LadderGhost_RaceStarted() ) {
			CG_LadderGhost_SetPickerOpen( qfalse );
		}
		return;
	}
	if ( cg.ladderPickerAutoShown || !cg.ladderGhostListReady || CG_LadderGhost_RaceStarted() ) {
		return;
	}
	mode = cg_ghostPlayback.integer;
	if ( cg.ladderGhostEntryCount <= 0 || ( mode != 0 && mode != 3 ) ) {
		cg.ladderPickerAutoShown = qtrue;
		return;
	}
	/* Wait while a menu, the console or the HUD options own the keys. */
	if ( trap_Key_GetCatcher() != 0 || CG_HUDOptionsIsOpen() ) {
		return;
	}
	if ( !cg.snap || cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_SPECTATOR ) {
		return;
	}
	cg.ladderPickerAutoShown = qtrue;
	cg.ladderPickerManual = qfalse;
	CG_LadderGhost_SetPickerOpen( qtrue );
}

void CG_LadderGhost_DrawPicker( void ) {
	static vec4_t bgColor     = { 0.008f, 0.012f, 0.016f, 0.90f };
	static vec4_t bandColor   = { 0.008f, 0.012f, 0.016f, 0.96f };
	static vec4_t borderColor = { 0.24f, 0.34f, 0.36f, 0.72f };
	static vec4_t accentColor = Q3RALLY_ACCENT_COLOR;
	static vec4_t titleColor  = { 0.90f, 0.95f, 0.94f, 1.00f };
	static vec4_t mutedColor  = { 0.47f, 0.62f, 0.61f, 1.00f };
	static vec4_t hoverColor  = { 0.07f, 0.11f, 0.17f, 0.90f };
	static vec4_t timeColor   = { 0.30f, 0.66f, 0.96f, 1.00f };
	int visible[MAX_LADDER_GHOST_ENTRIES];
	char ownVehicle[MAX_QPATH];
	char text[96];
	int count;
	int row;
	float x = LADDER_PICKER_X;
	float y = LADDER_PICKER_Y;
	float w = LADDER_PICKER_W;
	float h;

	CG_LadderGhost_Frame();
	if ( !cg.ladderPickerOpen ) {
		return;
	}

	count = CG_LadderGhost_Visible( visible, MAX_LADDER_GHOST_ENTRIES );
	CG_LadderGhost_OwnVehicle( ownVehicle, sizeof( ownVehicle ) );
	if ( cg.ladderPickerCursor > count ) {
		cg.ladderPickerCursor = count;
	}

	CG_SetScreenPlacement( PLACE_CENTER, PLACE_CENTER );
	h = 62.0f + LADDER_PICKER_ROWS * LADDER_PICKER_ROW_H + 38.0f;
	CG_FillRect( x, y, w, h, bgColor );
	CG_FillRect( x, y, w, 26.0f, bandColor );
	CG_FillRect( x, y, w, 2.0f, accentColor );
	CG_DrawRect( x, y, w, h, 1.0f, borderColor );

	CG_DrawIngameString( (int)( x + w * 0.5f ), (int)( y + 6 ), "LADDER GHOSTS",
		UI_CENTER | UI_DROPSHADOW, 0.72f, titleColor );

	if ( cg.ladderPickerAllVehicles ) {
		Com_sprintf( text, sizeof( text ), "ALL CARS  (%d)", count );
	} else {
		Com_sprintf( text, sizeof( text ), "YOUR CAR: %s  (%d)", ownVehicle[0] ? ownVehicle : "?", count );
	}
	Q_strupr( text );
	CG_DrawIngameString( (int)( x + 8 ), (int)( y + 32 ), text, UI_SMALLFONT, 0.50f, accentColor );
	if ( cg.ladderGhostListFromCache ) {
		CG_DrawIngameString( (int)( x + w - 8 ), (int)( y + 32 ), "OFFLINE CACHE",
			UI_RIGHT | UI_SMALLFONT, 0.50f, mutedColor );
	}

	for ( row = 0; row < LADDER_PICKER_ROWS; row++ ) {
		int line = cg.ladderPickerScroll + row;
		float rowY = y + 50.0f + row * LADDER_PICKER_ROW_H;
		const float *color = ( line == cg.ladderPickerCursor ) ? accentColor : titleColor;

		if ( line > count ) {
			break;
		}
		if ( line == cg.ladderPickerCursor ) {
			CG_FillRect( x + 4, rowY - 2, w - 8, LADDER_PICKER_ROW_H, hoverColor );
		}
		if ( line == 0 ) {
			CG_DrawIngameString( (int)( x + 10 ), (int)rowY, "NO LADDER GHOST",
				UI_SMALLFONT, 0.50f, color );
			CG_DrawIngameString( (int)( x + w - 10 ), (int)rowY, "PERSONAL / BASE",
				UI_RIGHT | UI_SMALLFONT, 0.45f, mutedColor );
			continue;
		}
		{
			const ladderGhostEntry_t *entry = &cg.ladderGhostEntries[visible[line - 1]];
			char name[24];
			char lapText[24];

			Q_strncpyz( name, entry->name, sizeof( name ) );
			Com_sprintf( text, sizeof( text ), "%2d. %s", line, name );
			CG_DrawIngameString( (int)( x + 10 ), (int)rowY, text, UI_SMALLFONT, 0.50f, color );
			if ( cg.ladderPickerAllVehicles ) {
				Q_strncpyz( text, entry->vehicle, 16 );
				Q_strupr( text );
				CG_DrawIngameString( (int)( x + 214 ), (int)rowY, text, UI_RIGHT | UI_SMALLFONT, 0.42f, mutedColor );
			}
			CG_LadderGhost_FormatTime( entry->lapMs, lapText, sizeof( lapText ) );
			CG_DrawIngameString( (int)( x + w - 10 ), (int)rowY, lapText,
				UI_RIGHT | UI_SMALLFONT, 0.50f, timeColor );
		}
	}

	if ( count == 0 ) {
		CG_DrawIngameString( (int)( x + w * 0.5f ), (int)( y + 50.0f + LADDER_PICKER_ROW_H * 1.5f ),
			cg.ladderPickerAllVehicles ? "NO LADDER GHOSTS FOR THIS TRACK" : "NO GHOSTS FOR YOUR CAR - TAB: ALL CARS",
			UI_CENTER | UI_SMALLFONT, 0.45f, mutedColor );
	}

	/* The centre print ("Press FIRE or USE when ready to race.") would sit
	 * on top of the panel: while the picker is open it is shown here, small,
	 * above the key help (CG_DrawCenterString skips it). */
	if ( cg.centerPrintTime && CG_FadeColor( cg.centerPrintTime, (int)( 1000 * cg_centertime.value ) ) ) {
		char hint[96];
		int i;

		Q_strncpyz( hint, cg.centerPrint, sizeof( hint ) );
		for ( i = 0; hint[i]; i++ ) {
			if ( hint[i] == '\n' ) {
				hint[i] = '\0';
				break;
			}
		}
		Q_strupr( hint );
		if ( hint[0] ) {
			CG_DrawIngameString( (int)( x + w * 0.5f ), (int)( y + h - 34 ), hint,
				UI_CENTER | UI_SMALLFONT, 0.45f, accentColor );
		}
	}

	CG_DrawIngameString( (int)( x + w * 0.5f ), (int)( y + h - 16 ),
		"UP/DOWN SELECT  |  ENTER PICK  |  TAB CARS  |  ESC CLOSE",
		UI_CENTER | UI_SMALLFONT, 0.42f, mutedColor );
}

/*
=================
CG_LadderGhost_StatusText

HUD line for the ghost status: opponent name, loading or failure.
=================
*/
const char *CG_LadderGhost_StatusText( qboolean *isError ) {
	*isError = qfalse;
	if ( cg.ladderGhostAvailable && cg.ladderGhostSelected >= 0 ) {
		char name[16];

		Q_strncpyz( name, cg.ladderGhostEntries[cg.ladderGhostSelected].name, sizeof( name ) );
		Q_strupr( name );
		return va( "VS %s", name );
	}
	if ( cg.ladderGhostFailed ) {
		*isError = qtrue;
		return "GHOST FAILED";
	}
	return "LOADING GHOST";
}

/* -------------------------------------------------------------------------
   Result against the ghost
   The ghost is one best lap, so the race counts as won when the player's
   best lap (A2B: the whole run) is faster than the ghost's lap.
   ------------------------------------------------------------------------- */

#define GHOST_RESULT_BANNER_MS  8000
#define BANNER_TITLE_SCALE      0.95f
#define BANNER_LINE_SCALE       0.50f

/*
=================
CG_GhostRace_Opponent

Name and lap time of the ghost the player races against in Ghost Race.
=================
*/
qboolean CG_GhostRace_Opponent( char *name, int nameSize, int *lapMs ) {
	if ( cgs.gametype != GT_GHOST ) {
		return qfalse;
	}

	switch ( CG_GhostPlaybackMode() ) {
	case 1:
		if ( !cg.personalGhostAvailable || cg.personalGhostBestTime <= 0 ) {
			return qfalse;
		}
		Q_strncpyz( name, "Personal Ghost", nameSize );
		*lapMs = cg.personalGhostBestTime;
		return qtrue;
	case 2:
		if ( !cg.baseGhostAvailable || cg.baseGhostBestTime <= 0 ) {
			return qfalse;
		}
		Q_strncpyz( name, "Server Ghost", nameSize );
		*lapMs = cg.baseGhostBestTime;
		return qtrue;
	case 3:
		if ( !cg.ladderGhostAvailable || cg.ladderGhostSelected < 0 ||
		     cg.ladderGhostSelected >= cg.ladderGhostEntryCount ) {
			return qfalse;
		}
		Q_strncpyz( name, cg.ladderGhostEntries[cg.ladderGhostSelected].name, nameSize );
		*lapMs = cg.ladderGhostEntries[cg.ladderGhostSelected].lapMs;
		return *lapMs > 0;
	default:
		return qfalse;
	}
}

/*
=================
CG_GhostRace_ReportOpponent

Tells the server which ghost this driver races (name + lap time, ladder
entry index or -1), so the server can rate every driver at the finish.
Sent on changes and repeated until the server answers with lghostoppok.
=================
*/
void CG_GhostRace_ReportOpponent( void ) {
	char name[40];
	char report[64];
	int lapMs = 0;
	int ladderEntry = -1;

	if ( cgs.gametype != GT_GHOST ) {
		return;
	}
	if ( !CG_GhostRace_Opponent( name, sizeof( name ), &lapMs ) ) {
		Q_strncpyz( name, "-", sizeof( name ) );
		lapMs = 0;
	} else if ( CG_GhostPlaybackMode() == 3 ) {
		/* Ladder ghost: the server takes the lap time from its own list. */
		ladderEntry = cg.ladderGhostSelected;
	}
	Com_sprintf( report, sizeof( report ), "%d %s %d", lapMs, name, ladderEntry );
	if ( !strcmp( report, cg.ghostOpponentReported ) ) {
		return;
	}
	Q_strncpyz( cg.ghostOpponentReported, report, sizeof( cg.ghostOpponentReported ) );
	cg.ghostOpponentPendingMs = lapMs;
	CG_GhostCmd_Set( GHOSTCMD_OPP, va( "ghostopp %d \"%s\" %d", lapMs, name, ladderEntry ) );
}

/* Result of one driver from the server; the local driver falls back to
 * the own evaluation until the server result arrives. */
qboolean CG_GhostRace_ClientResult( int clientNum, qboolean *won ) {
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS || cgs.gametype != GT_GHOST ) {
		return qfalse;
	}
	if ( cg.ghostRaceResults[clientNum].valid ) {
		*won = cg.ghostRaceResults[clientNum].won;
		return qtrue;
	}
	if ( cg.snap && clientNum == cg.snap->ps.clientNum && cg.ghostResultValid ) {
		*won = cg.ghostResultWon;
		return qtrue;
	}
	return qfalse;
}

/* Called from CG_StartRace: forget all results, report the ghost again. */
void CG_GhostRace_ResetRace( void ) {
	memset( cg.ghostRaceResults, 0, sizeof( cg.ghostRaceResults ) );
	cg.ghostOpponentReported[0] = '\0';
	CG_GhostRace_ResetResult();
}

/* Own result only. */
void CG_GhostRace_ResetResult( void ) {
	cg.ghostFinishOnly = qfalse;
	cg.ghostResultValid = qfalse;
	cg.ghostResultWon = qfalse;
	cg.ghostResultPlayerMs = 0;
	cg.ghostResultGhostMs = 0;
	cg.ghostResultTime = 0;
	cg.ghostResultName[0] = '\0';
}

/*
=================
CG_GhostRace_EvaluateFinish

Called from CG_FinishedRace for the local player after the best lap was
updated and before a new personal ghost is saved (which would replace the
personal ghost's lap time with this run).
=================
*/
void CG_GhostRace_EvaluateFinish( int bestLapMs ) {
	char name[40];
	int ghostMs;

	CG_GhostRace_ResetResult();
	if ( bestLapMs <= 0 ) {
		return;
	}
	if ( !CG_GhostRace_Opponent( name, sizeof( name ), &ghostMs ) ) {
		/* Ghost Race without a ghost: neutral finish banner. */
		if ( cgs.gametype == GT_GHOST ) {
			cg.ghostFinishOnly = qtrue;
			cg.ghostResultPlayerMs = bestLapMs;
			cg.ghostResultTime = cg.time;
		}
		return;
	}
	cg.ghostResultValid = qtrue;
	cg.ghostResultWon = bestLapMs < ghostMs ? qtrue : qfalse;
	cg.ghostResultPlayerMs = bestLapMs;
	cg.ghostResultGhostMs = ghostMs;
	cg.ghostResultTime = cg.time;
	Q_strncpyz( cg.ghostResultName, name, sizeof( cg.ghostResultName ) );
	{
		char playerText[24];

		Q_strncpyz( playerText, getStringForTimePrecise( bestLapMs ), sizeof( playerText ) );
		CG_Printf( "%s against %s: best lap %s vs %s\n",
			cg.ghostResultWon ? "Won" : "Lost", name, playerText,
			getStringForTimePrecise( ghostMs ) );
	}
}

/*
=================
CG_GhostRace_ScoreboardGhost

Data for the ghost row of the scoreboard: the result after the finish,
otherwise the live comparison with the current best lap.
=================
*/
qboolean CG_GhostRace_ScoreboardGhost( char *name, int nameSize, int *ghostMs, int *playerMs, qboolean *finished ) {
	if ( cgs.gametype != GT_GHOST || !cg.snap ) {
		return qfalse;
	}
	if ( cg.ghostResultValid ) {
		Q_strncpyz( name, cg.ghostResultName, nameSize );
		*ghostMs = cg.ghostResultGhostMs;
		*playerMs = cg.ghostResultPlayerMs;
		*finished = qtrue;
		return qtrue;
	}
	if ( !CG_GhostRace_Opponent( name, nameSize, ghostMs ) ) {
		return qfalse;
	}
	*playerMs = cg_entities[cg.snap->ps.clientNum].bestLapTime;
	*finished = qfalse;
	return qtrue;
}

static void CG_GhostRace_FormatGap( int ms, char *out, int outSize ) {
	int absMs = ms < 0 ? -ms : ms;
	Com_sprintf( out, outSize, "%d.%03d", absMs / 1000, absMs % 1000 );
}

/*
=================
CG_GhostRace_DrawResultBanner

Big result line after the finish; stays visible on the intermission
scoreboard.
=================
*/
void CG_GhostRace_DrawResultBanner( void ) {
	static vec4_t wonColor  = { 0.35f, 0.90f, 0.45f, 1.00f };
	static vec4_t lostColor = { 1.00f, 0.32f, 0.22f, 1.00f };
	static vec4_t subColor  = { 0.90f, 0.95f, 0.94f, 1.00f };
	static vec4_t finishColor = Q3RALLY_ACCENT_COLOR;
	static vec4_t shade     = { 0.008f, 0.012f, 0.016f, 0.70f };
	vec4_t color;
	vec4_t subtitleColor;
	vec4_t shadeColor;
	char gap[24];
	char title[64];
	char line[128];
	char name[24];
	int titleWidth;
	int lineWidth;
	int boxWidth;
	int top;
	char playerText[24];
	char ghostText[24];
	float alpha = 1.0f;
	int elapsed;

	if ( ( !cg.ghostResultValid && !cg.ghostFinishOnly ) || cgs.gametype != GT_GHOST || !cg.snap ) {
		return;
	}
	elapsed = cg.time - cg.ghostResultTime;
	if ( cg.snap->ps.pm_type != PM_INTERMISSION ) {
		if ( elapsed < 0 || elapsed > GHOST_RESULT_BANNER_MS ) {
			return;
		}
		if ( elapsed > GHOST_RESULT_BANNER_MS - 1000 ) {
			alpha = (float)( GHOST_RESULT_BANNER_MS - elapsed ) / 1000.0f;
		}
	}

	if ( cg.ghostResultValid ) {
		Vector4Copy( cg.ghostResultWon ? wonColor : lostColor, color );
	} else {
		Vector4Copy( finishColor, color );
	}
	Vector4Copy( subColor, subtitleColor );
	Vector4Copy( shade, shadeColor );
	color[3] *= alpha;
	subtitleColor[3] *= alpha;
	shadeColor[3] *= alpha;

	Q_strncpyz( playerText, getStringForTimePrecise( cg.ghostResultPlayerMs ), sizeof( playerText ) );
	if ( cg.ghostResultValid ) {
		CG_GhostRace_FormatGap( cg.ghostResultGhostMs - cg.ghostResultPlayerMs, gap, sizeof( gap ) );
		Com_sprintf( title, sizeof( title ), cg.ghostResultWon ? "GHOST BEATEN BY %s" : "GHOST WINS BY %s", gap );

		Q_strncpyz( name, cg.ghostResultName, sizeof( name ) );
		Q_strupr( name );
		Q_strncpyz( ghostText, getStringForTimePrecise( cg.ghostResultGhostMs ), sizeof( ghostText ) );
		Com_sprintf( line, sizeof( line ), "BEST LAP %s  -  %s %s", playerText, name, ghostText );
	} else {
		/* Raced without a ghost: same banner, neutral. */
		Q_strncpyz( title, "FINISHED", sizeof( title ) );
		Com_sprintf( line, sizeof( line ), "BEST LAP %s  -  NO GHOST", playerText );
	}

	/* Box fits the longer line. While driving the banner sits below the
	 * rear-view mirror (y 10-85) and the minimap (up to y 120), at the height
	 * of the other center prints; with the scoreboard up it moves above the
	 * table (the mirror and minimap are not drawn then). */
	titleWidth = CG_IngameStringWidth( title, 0, BANNER_TITLE_SCALE );
	lineWidth = CG_IngameStringWidth( line, UI_SMALLFONT, BANNER_LINE_SCALE );
	boxWidth = ( titleWidth > lineWidth ? titleWidth : lineWidth ) + 32;
	if ( boxWidth > SCREEN_WIDTH - 16 ) {
		boxWidth = SCREEN_WIDTH - 16;
	}
	top = ( cg.snap->ps.pm_type == PM_INTERMISSION || cg.showScores ) ? 22 : 128;

	CG_SetScreenPlacement( PLACE_CENTER, PLACE_TOP );
	CG_FillRect( ( SCREEN_WIDTH - boxWidth ) * 0.5f, top, boxWidth, 52, shadeColor );
	CG_DrawIngameString( SCREEN_WIDTH / 2, top + 6, title, UI_CENTER | UI_DROPSHADOW, BANNER_TITLE_SCALE, color );
	CG_DrawIngameString( SCREEN_WIDTH / 2, top + 33, line, UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, BANNER_LINE_SCALE, subtitleColor );
}
