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

#ifdef UNIT_TEST
#define TESTABLE_STATIC
#else
#define TESTABLE_STATIC static
#endif

#define RALLY_INTRO_CAM_DURATION_MS 3000

static int CountRaceGridStarts( void );

static vec3_t rallyIntroGridOrigin[MAX_CLIENTS];
static vec3_t rallyIntroGridAngles[MAX_CLIENTS];
static qboolean rallyIntroGridSaved[MAX_CLIENTS];

static void G_DebugRaceStateTransitionRally( gentity_t *ent, const char *context, int oldState, int newState ) {
	if ( !g_debugIntroCam.integer ) {
		return;
	}

	G_Printf( "RaceStateFlip[%s]: clientNum=%d sessionTeam=%d spectatorState=%d pm_flags=%d raceState=%d->%d level.time=%d raceIntroEndTime=%d\n",
		context ? context : "unknown",
		ent ? ent->s.number : -1,
		( ent && ent->client ) ? ent->client->sess.sessionTeam : -1,
		( ent && ent->client ) ? ent->client->sess.spectatorState : -1,
		( ent && ent->client ) ? ent->client->ps.pm_flags : -1,
		oldState,
		newState,
		level.time,
		level.raceIntroEndTime );
}

static void G_RallyClearIntroGridSnapshots( void ) {
	int i;

	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		rallyIntroGridSaved[i] = qfalse;
	}
}

static void G_RallySnapshotIntroGridPositions( void ) {
	int i;

	G_RallyClearIntroGridSnapshots();

	for ( i = 0; i < level.maxclients; i++ ) {
		gentity_t *player = &g_entities[i];

		if ( !player->inuse || !player->client ) {
			continue;
		}

		if ( player->client->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}

		VectorCopy( player->client->ps.origin, rallyIntroGridOrigin[i] );
		VectorCopy( player->client->ps.viewangles, rallyIntroGridAngles[i] );
		rallyIntroGridSaved[i] = qtrue;
	}
}

void G_RallyIntroCountdownHandover( void ) {
	int i;
	qboolean hasSnapshots = qfalse;

	for ( i = 0; i < level.maxclients; i++ ) {
		if ( rallyIntroGridSaved[i] ) {
			hasSnapshots = qtrue;
			break;
		}
	}

	if ( !hasSnapshots ) {
		return;
	}

	for ( i = 0; i < level.maxclients; i++ ) {
		gentity_t *player = &g_entities[i];
		gclient_t *client;
		int beforeSessionTeam;
		int beforeSpectatorState;
		int beforePmFlags;

		if ( !player->inuse || !player->client ) {
			continue;
		}

		client = player->client;
		beforeSessionTeam = client->sess.sessionTeam;
		beforeSpectatorState = client->sess.spectatorState;
		beforePmFlags = client->ps.pm_flags;

		G_Printf( "IntroHandover before: clientNum=%d sessionTeam=%d spectatorState=%d pm_flags=%d\n",
			i, beforeSessionTeam, beforeSpectatorState, beforePmFlags );

		if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
			G_Printf( "IntroHandover after: clientNum=%d sessionTeam=%d spectatorState=%d pm_flags=%d\n",
				i, client->sess.sessionTeam, client->sess.spectatorState, client->ps.pm_flags );
			continue;
		}

		client->ps.pm_flags &= ~( PMF_FOLLOW | PMF_OBSERVE );
		client->sess.spectatorState = SPECTATOR_NOT;
		client->ps.pm_type = PM_NORMAL;

		if ( rallyIntroGridSaved[i] ) {
			VectorCopy( rallyIntroGridOrigin[i], client->ps.origin );
			G_SetOrigin( player, rallyIntroGridOrigin[i] );
			SetClientViewAngle( player, rallyIntroGridAngles[i] );
			VectorClear( client->ps.velocity );
			VectorClear( client->ps.angularMomentum );
		}

		G_Printf( "IntroHandover after: clientNum=%d sessionTeam=%d spectatorState=%d pm_flags=%d\n",
			i, client->sess.sessionTeam, client->sess.spectatorState, client->ps.pm_flags );
	}

	G_RallyClearIntroGridSnapshots();
}


int GetTeamAtRank( int rank ){
	int		i, j, count;
	int		ranks[4];
	int		counts[4];

	for (i = 0; i < 4; i++){
		counts[i] = TeamCount( -1, TEAM_RED + i );
		ranks[i] = 0;
	}

	for (i = 0; i < 4; i++){
		if (!counts[i]) continue;

		count = 0;
		for (j = 0; j < 4; j++){
			if (!counts[j]) continue;

			if (isRallyRace()){
				if (level.teamTimes[i + TEAM_RED] > level.teamTimes[j + TEAM_RED]) count++;
			}
			else if (level.teamScores[i + TEAM_RED] < level.teamScores[j + TEAM_RED]) count++;
		}

		while( count < 4 && ranks[count] ) count++; // rank is taken so move to the next one
		if (count < 4)
			ranks[count] = TEAM_RED + i;
	}

	// Guard against invalid rank values to prevent out-of-bounds array access.
	// rank is 1-based; ranks[] has 4 entries, so valid range is [1, 4].
	if ( rank < 1 || rank > 4 ) {
		return -1;
	}

	if (g_gametype.integer == GT_CTF && rank > 2){
		return -1;
	}
	else {
		return ranks[rank-1];
	}
}


// UPDATE - send as command string instead?
void Cmd_RacePositions_f( void ) {
	// Each entry is " <clientNum> <position>": at most " 63 64" = 6 chars + NUL.
	// 64 clients * 7 bytes = 448 bytes worst case, well within STRING_SIZE.
	// entry[] only needs to hold one formatted entry at a time.
#define ENTRY_SIZE  32
#define STRING_SIZE 512
	char			entry[ENTRY_SIZE];
	char			string[STRING_SIZE];
	gentity_t		*player;
	int				i, count, j, stringlength;

	string[0] = 0;
	stringlength = 0;

	for(i = 0, count = 0; i < level.maxclients; i++){
		player = &g_entities[i];
		if (!player->inuse) continue;
		if (!player->client) continue;

		Com_sprintf (entry, sizeof(entry)," %i %i", player->s.clientNum, player->client->ps.stats[STAT_POSITION]);
		j = strlen(entry);
		// Guard: ensure entry fits including the NUL terminator Q_strncpyz writes
		if (stringlength + j >= (int)sizeof(string) - 1)
			break;
		Q_strncpyz(string + stringlength, entry, sizeof(string) - stringlength);
		stringlength += j;

		count++;
	}
#undef ENTRY_SIZE
#undef STRING_SIZE

	G_LogPrintf("%s\n", va("positions %i%s", count, string));
	trap_SendServerCommand( -1, va("positions %i%s\n", count, string) );
}


void Cmd_Times_f( gentity_t *ent ) {
	// Send current team race times to the requesting client.
	// level.teamTimes[] is kept up-to-date by G_RallyUpdateTeamTime() in
	// g_rally_mapents.c whenever a player crosses the finish line.
	// For non-team or non-race modes the values are 0, which is harmless.
	trap_SendServerCommand( ent - g_entities, va( "times %i %i %i %i",
		level.teamTimes[TEAM_RED],
		level.teamTimes[TEAM_BLUE],
		level.teamTimes[TEAM_GREEN],
		level.teamTimes[TEAM_YELLOW] ) );
}


/*
================================================================================
GetDistanceToMarker

 Used to calculate how far a player is from the marker.
 Called to find out race positions of players.
================================================================================
*/
float GetDistanceToMarker( gentity_t *player, float markerNumber )
{
	gentity_t		*ent = NULL;
	vec3_t			dist;

	if ( !markerNumber )
		return 1<<30;

	while ( (ent = G_Find (ent, FOFS(classname), "rally_checkpoint")) != NULL )
	{
		if( ent->number == markerNumber )
			break;
	}

	if ( ent )
	{
		VectorSubtract(player->r.currentOrigin, ent->s.origin, dist);
		return VectorLength(dist);
	}
	else
		return 1<<30;
}

/*
================================================================================
IsCarAhead

 Returns true if player one is ahead of two.
================================================================================
*/
qboolean IsCarAhead(gentity_t *one, gentity_t *two){
	float		dist1, dist2;
	int			time1, time2;

	if (one->client->finishRaceTime && two->client->finishRaceTime){
		time1 = one->client->finishRaceTime - level.startRaceTime;
		if (one->client->ps.persistant[PERS_SCORE] > 0 && !isRallyNonDMRace()){
			time1 -= (one->client->ps.persistant[PERS_SCORE] * TIME_BONUS_PER_FRAG);
		}

		time2 = two->client->finishRaceTime - level.startRaceTime;
		if (two->client->ps.persistant[PERS_SCORE] > 0 && !isRallyNonDMRace()){
			time2 -= (two->client->ps.persistant[PERS_SCORE] * TIME_BONUS_PER_FRAG);
		}

		if (time1 < time2){ // use frag modified times
//			Com_Printf("Car 1 finished the race with less time than car 2\n");
			return qtrue;
		}
		else {
//			Com_Printf("Car 2 finished the race with less time than car 1\n");
			return qfalse;
		}
	}
	else if (one->client->finishRaceTime){
//		Com_Printf("Car 1 finished the race, car 2 hasn't\n");
		return qtrue;
	}
	else if (two->client->finishRaceTime){
//		Com_Printf("Car 2 finished the race, car 1 hasn't\n");
		return qfalse;
	}
	else if (one->currentLap < two->currentLap){
//		Com_Printf("Car 1 is a lap behind car 2\n");
		return qfalse;
	}
	else if (one->currentLap == two->currentLap && one->number < two->number){
//		Com_Printf("Car 1 hat a target marker that is behind car 2's\n");
		return qfalse;
	}
	else if (one->currentLap == two->currentLap && one->number == two->number){
		dist1 = GetDistanceToMarker( one, one->number );
		dist2 = GetDistanceToMarker( two, two->number );

		if (dist1 > dist2){
//			Com_Printf("Car 1 is %f to marker %i and car 2 is %f\n", dist1, one->number, dist2);
			return qfalse;
		}
	}

	return qtrue;
}


/*
================================================================================
CalculatePlayerPositions

 Calculates the order of all racers
================================================================================
*/
void CalculatePlayerPositions( void )
{
	gentity_t	*ent, *leader, *cur, *last;
	int			position;
	qboolean	positionChanged;

//	if (level.startRaceTime + FRAMETIME > level.time || level.startRaceTime == 0){
//		return;
//	}
	if (!isRallyRace()){
		return;
	}

	positionChanged = qfalse;
	leader = ent = last = NULL;
	while ( (ent = G_Find (ent, FOFS(classname), "player")) != NULL )
	{
		if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) continue;
//		if ( isRaceObserver(ent->s.number) ) continue;

		if ( g_gametype.integer == GT_ELIMINATION && ent->client->finishRaceTime ) {
			ent->carBehind = NULL;
			continue;
		}

		ent->carBehind = NULL;

		if ( leader == NULL )
		{
			leader = ent;
			continue;
		}

		cur = leader;
		if ( IsCarAhead( ent, cur ) )
		{
			ent->carBehind = cur;
			leader = ent;
			continue;
		}

		while ( cur->carBehind != NULL )
		{
			if ( IsCarAhead( ent, cur->carBehind ) )
			{
//				ent->carBehind = cur->carBehind;
//				cur->carBehind = ent;
				last = cur;
				cur = cur->carBehind;
				break;
			}

			last = cur;
			cur = cur->carBehind;
		}

		if ( IsCarAhead( ent, cur ) )
		{
//			cur->carBehind = NULL;
			ent->carBehind = cur;
			if (last) {
				last->carBehind = ent;
			}
		}
		else {
			cur->carBehind = ent;
			ent->carBehind = NULL;
		}
	}

	if ( leader == NULL )
		return;

	cur = leader;
	position = 1;

	while( cur->carBehind != NULL )
	{
		if ( position != cur->client->ps.stats[STAT_POSITION] && cur->client ){
			cur->client->ps.stats[STAT_POSITION] = position;

			positionChanged = qtrue;
		}

		cur = cur->carBehind;
		position++;
	}

	if ( position != cur->client->ps.stats[STAT_POSITION] && cur->client ){
		cur->client->ps.stats[STAT_POSITION] = position;

		positionChanged = qtrue;
	}

	if ( positionChanged )
	{
		Cmd_RacePositions_f();
		CalculateRanks();
	}

	// Keep team times live during the race so scoreboard and HUD
	// always show the correct team ranking, not just at finish events.
	G_RallyUpdateAllTeamTimes();
}


void RallyRace_Think( gentity_t *ent ){
	ent->nextthink = level.time + 200;

	CalculatePlayerPositions();
}

void RaceCountdown( char *s, int secondsLeft ){
	trap_SendServerCommand( -1, va("rc \"%s\" %d", s, secondsLeft) );
}

TESTABLE_STATIC void G_RallyConfigureElimination( int participantCount ) {
	if ( g_gametype.integer != GT_ELIMINATION ) {
		return;
	}

	if ( participantCount < 1 ) {
		participantCount = 1;
	}

	if ( level.eliminationSetupComplete ) {
		level.eliminationPlayersRemaining = participantCount;
		return;
	}

	level.eliminationInitialPlayers = participantCount;
	level.eliminationPlayersRemaining = participantCount;
	level.eliminationRound = 0;
	level.eliminationSetupComplete = qtrue;

	if ( participantCount < 1 ) {
		participantCount = 1;
	}

	{
		int laps = participantCount - 1;
		if ( laps < 1 ) {
			laps = 1;
		}
		level.numberOfLaps = laps;
		trap_Cvar_Set( "laplimit", va( "%d", level.numberOfLaps ) );
	}
}

static void G_RallyInitializeLapTimersAtRaceStart( int startTime ) {
	gentity_t *player;
	int i;

	if ( startTime <= 0 ) {
		return;
	}

	for ( i = 0; i < MAX_CLIENTS; ++i ) {
		player = &g_entities[i];
		if ( !player->inuse || !player->client ) {
			continue;
		}
		if ( player->client->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		if ( isRaceObserver( i ) ) {
			continue;
		}
		if ( player->client->finishRaceTime ) {
			continue;
		}

		player->client->lapStartTime = startTime;
		player->client->lastCheckpointTime = startTime;
	}
}

void RallyStarter_Think( gentity_t *ent ){
	gentity_t		*player, *t;
	int				i, count;
	int				activePlayers;
	qboolean	start;
	qboolean	enforceReady;
	qboolean	ignoreBots;
	qboolean	useIntroRaceState;
	int			introDurationMs;

	useIntroRaceState = ( g_gametype.integer == GT_RACING
		|| g_gametype.integer == GT_RACING_DM
		|| g_gametype.integer == GT_TEAM_RACING
		|| g_gametype.integer == GT_TEAM_RACING_DM
		|| g_gametype.integer == GT_SPRINT
		|| g_gametype.integer == GT_ELIMINATION ) ? qtrue : qfalse;
	introDurationMs = level.raceIntroDurationMs > 0 ? level.raceIntroDurationMs : RALLY_INTRO_CAM_DURATION_MS;
	ignoreBots = g_rallyIgnoreBots.integer;

	if ( !ent->count ) {
		int gridStarts = CountRaceGridStarts();
		ent->count = 1;
		if ( gridStarts < level.maxclients ) {
			G_Printf( "Warning: Map has %i info_player_start entities, but sv_maxClients is %i; temporary grid slots may be required\n", gridStarts, level.maxclients );
		}
	}

	if (level.startRaceTime){
		{
			int oldRaceState = level.raceState;
			level.raceState = RACE_STATE_RUNNING;
			level.raceIntroEndTime = 0;
			G_DebugRaceStateTransitionRally( ent, "RallyRace_Stage countdown finished -> RUNNING", oldRaceState, level.raceState );
		}
		return;
	}

	/* Fire intro camera as early as possible -- before the 7500ms grid guard --
	   so players see the track preview instead of sitting on the grid.
	   raceIntroFallback is pre-set to qtrue in G_InitGame when restart=1,
	   so the intro only plays on the initial map load, not after every race.
	   We count connected non-spectator clients independently of the
	   main ready-check loop below. */
	if ( useIntroRaceState && level.raceIntroHasSequence
		&& level.raceState != RACE_STATE_INTRO_CAM
		&& !level.raceIntroFallback ) {
		int introCount = 0;
		for ( i = 0; i < MAX_CLIENTS; i++ ) {
			player = &g_entities[i];
			if ( !player->inuse || !player->client ) continue;
			if ( player->client->sess.sessionTeam == TEAM_SPECTATOR ) continue;
			if ( (player->r.svFlags & SVF_BOT) && ignoreBots ) continue;
			introCount++;
		}
		if ( introCount > 0 ) {
			int oldRaceState = level.raceState;
			level.raceState = RACE_STATE_INTRO_CAM;
			level.raceIntroEndTime = level.time + introDurationMs;
			level.raceIntroFallback = qtrue;  /* prevent re-trigger after expiry */
			G_DebugRaceStateTransitionRally( ent, "RallyRace_Stage early intro -> INTRO_CAM", oldRaceState, level.raceState );
			trap_SendServerCommand( -1, va( "introCamStart %d", level.time ) );
			G_RallySnapshotIntroGridPositions();
			ent->number = 3;
			ent->pain_debounce_time = 0;
			G_RallyConfigureElimination( introCount );
		}
	}

	// if no checkpoints dont do start sequence
	if (isRallyRace()){
		t = NULL;
		t = G_Find (t, FOFS(classname), "rally_checkpoint");
		if (t == NULL){
			activePlayers = 0;
			for ( i = 0; i < MAX_CLIENTS; i++ ) {
				player = &g_entities[i];
				if ( !player->inuse ) continue;
				if ( !player->client ) continue;
				if ( player->client->sess.sessionTeam == TEAM_SPECTATOR ) continue;

				activePlayers++;
				break;
			}

			if ( !activePlayers ) {
				return;
			}

			// start race right away
			level.raceState = RACE_STATE_RUNNING;
			level.raceIntroEndTime = 0;
			level.startRaceTime = level.time;
			G_RallyInitializeLapTimersAtRaceStart( level.startRaceTime );
			trap_SendServerCommand( -1, va("raceTime %i", level.startRaceTime) );
			CenterPrint_All("GO..");

			G_FreeEntity( ent );
			return;
		}
	}
	ent->nextthink = level.time + 1000;
	t = NULL;

	enforceReady = g_gametype.integer != GT_SINGLE_PLAYER && g_rallyReadyCheck.integer;

	if ( ent->number == 0 ){

		if( level.time - level.startTime < 7500 )
			return;

		start = qtrue;
		for (i = 0, count = 0; i < MAX_CLIENTS; i++){
			player = &g_entities[i];
			if (!player->inuse) continue;
			if (!player->client) continue;
			if (player->client->sess.sessionTeam == TEAM_SPECTATOR) continue;

			if ( (player->r.svFlags & SVF_BOT) && ignoreBots ) {
				continue;
			}

			count++;

			if (player->r.svFlags & SVF_BOT) continue;

			if ( !enforceReady ){
				continue;
			}

			if ( !player->ready ){
				start = qfalse;
				break;
			}
		}

		if ( !count ){
			return;
		}
		// Option A lobby: Derby and LCS need at least g_derbyMinPlayers real players
		// before the countdown begins. Show a waiting message and hold here.
		else if ( (g_gametype.integer == GT_DERBY || g_gametype.integer == GT_LCS)
			&& count < g_derbyMinPlayers.integer ) {
			CenterPrint_All( va("Waiting for players... (%i/%i)",
				count, g_derbyMinPlayers.integer) );
			return;
		}
		/* Intro is now triggered early (before the 7500ms grid guard) if a
		   sequence exists. If we reach here and INTRO_CAM is already active,
		   do nothing -- the intro expiry block below handles the handover. */
		if ( level.raceState == RACE_STATE_INTRO_CAM ) {
			/* waiting for intro to finish */
		}
		else if ( start && count ){
			ent->number = 3;
				{
					int oldRaceState = level.raceState;

					if ( useIntroRaceState && !level.raceIntroHasSequence ) {
						level.raceIntroFallback = qtrue;
					}
					level.raceState = RACE_STATE_COUNTDOWN;
					level.raceIntroEndTime = 0;
					G_DebugRaceStateTransitionRally( ent, "RallyRace_Stage start -> COUNTDOWN", oldRaceState, level.raceState );
					G_RallyClearIntroGridSnapshots();
				}
			ent->pain_debounce_time = 0;
			G_RallyConfigureElimination( count );
		}
		else if ( level.time >= level.startTime + (g_forceEngineStart.integer * 1000) ) {
			ent->number = 3; // force race start
				{
					int oldRaceState = level.raceState;

					if ( useIntroRaceState && !level.raceIntroHasSequence ) {
						level.raceIntroFallback = qtrue;
					}
					level.raceState = RACE_STATE_COUNTDOWN;
					level.raceIntroEndTime = 0;
					G_DebugRaceStateTransitionRally( ent, "RallyRace_Stage forced start -> COUNTDOWN", oldRaceState, level.raceState );
					G_RallyClearIntroGridSnapshots();
				}
			ent->pain_debounce_time = 0;
			G_RallyConfigureElimination( count );
		}
		else if (ent->number == 0 && level.time > level.startTime + (g_forceEngineStart.integer * 1000) - 10000){
			CenterPrint_All( va("Forced engine start in %i...", 10 - ((level.time - (level.startTime + (g_forceEngineStart.integer * 1000) - 10000)) / 1000)) );
			return;
		}
		else {
			return;
		}
	}

	if ( level.raceState == RACE_STATE_INTRO_CAM ) {
		if ( level.time < level.raceIntroEndTime ) {
			return;
		}

		{
			int oldRaceState = level.raceState;
			level.raceState = RACE_STATE_COUNTDOWN;
			G_DebugRaceStateTransitionRally( ent, "RallyRace_Stage intro expired -> COUNTDOWN", oldRaceState, level.raceState );
		}
		G_RallyIntroCountdownHandover();
		ent->pain_debounce_time = level.time;
	}

	if ( ent->pain_debounce_time == 0 ) {
		{
			int oldRaceState = level.raceState;
			level.raceState = RACE_STATE_COUNTDOWN;
			G_DebugRaceStateTransitionRally( ent, "RallyRace_Stage countdown bootstrap", oldRaceState, level.raceState );
		}
		G_RallyIntroCountdownHandover();
		ent->pain_debounce_time = level.time;
	}

	if ( level.time > ent->pain_debounce_time + 5000 ){
		{
			int oldRaceState = level.raceState;
			level.raceState = RACE_STATE_RUNNING;
			level.raceIntroEndTime = 0;
			G_DebugRaceStateTransitionRally( ent, "RallyRace_Stage countdown finished -> RUNNING", oldRaceState, level.raceState );
		}
		level.startRaceTime = level.time;
		// Snapshot how many players were present at race start.
		// Used by CheckExitRules to prevent a solo-start instant win.
		if ( g_gametype.integer == GT_DERBY || g_gametype.integer == GT_LCS ) {
			int pi;
			level.derbyStartPlayerCount = 0;
			for ( pi = 0; pi < MAX_CLIENTS; pi++ ) {
				gentity_t *pl = &g_entities[pi];
				if ( !pl->inuse || !pl->client ) continue;
				if ( pl->client->sess.sessionTeam == TEAM_SPECTATOR ) continue;
				level.derbyStartPlayerCount++;
			}
		}
		G_RallyInitializeLapTimersAtRaceStart( level.startRaceTime );

		trap_SendServerCommand( -1, va("raceTime %i", level.startRaceTime) );
		RaceCountdown("GO!", 0);

		Rally_Sound( ent, EV_GLOBAL_SOUND, CHAN_ANNOUNCER, G_SoundIndex("sound/rally/race/go.ogg") );

		if (g_gametype.integer != GT_DERBY)
			ent->think = RallyRace_Think;
	}
	else if ( level.time > ent->pain_debounce_time + 4000 ){
		RaceCountdown("1", 1);

		Rally_Sound( ent, EV_GLOBAL_SOUND, CHAN_ANNOUNCER, G_SoundIndex("sound/rally/race/one.ogg") );
		ent->number = -1;
	}
	else if ( level.time > ent->pain_debounce_time + 3000 ){
		RaceCountdown("2", 2);

		Rally_Sound( ent, EV_GLOBAL_SOUND, CHAN_ANNOUNCER, G_SoundIndex("sound/rally/race/two.ogg") );
		ent->number = 1;
	}
	else if ( level.time > ent->pain_debounce_time + 2000 ){
		RaceCountdown("3", 3);

		Rally_Sound( ent, EV_GLOBAL_SOUND, CHAN_ANNOUNCER, G_SoundIndex("sound/rally/race/three.ogg") );
		ent->number = 2;
	}
	else {
		CenterPrint_All("Starting Race...");
	}
}

void CreateRallyStarter( void ) {
	gentity_t		*ent;

	ent = G_Spawn();

	ent->think = RallyStarter_Think;
	ent->nextthink = level.time + 2000;
	ent->number = 0;
	ent->count = 0;
	ent->classname = "rally_starter";
}


/*
===========
SelectLastMarkerForSpawn

  Places cars at the last marker they visited during a race

============
*/
gentity_t *SelectLastMarkerForSpawn( gentity_t *ent, vec3_t origin, vec3_t angles, qboolean isbot ) {
	gentity_t	*spot;
	int			lastMarker;

	spot = NULL;
	lastMarker = ent->number - 1;
	if (lastMarker <= 0){
		lastMarker = level.numCheckpoints;
	}

	while ((spot = G_Find (spot, FOFS(classname), "rally_checkpoint")) != NULL) {
		if ( spot->number == lastMarker) {
			break;
		}
	}

	if ( !spot ) {
		return SelectSpawnPoint( vec3_origin, origin, angles, isbot );
	}

	// spawn at last checkpoint
	VectorCopy (spot->s.origin, origin);
	VectorCopy (spot->s.angles, angles);

	return spot;
}

/*
===========
SelectGridPositionSpawn

  Places cars at the start line in order, so that no one is telefragged

============
*/
static gentity_t overflowSpot;
static qboolean missingGridStartsNotified;

#define OVERFLOW_GRID_COLUMNS			4
#define OVERFLOW_GRID_SPACING			192.0f
#define TEMP_GRID_GROUND_TRACE_UP		128.0f
#define TEMP_GRID_GROUND_TRACE_DOWN		4096.0f
#define TEMP_GRID_MIN_GROUND_NORMAL		0.3f
#define TEMP_GRID_GROUND_CLEARANCE		1.0f

static int CountRaceGridStarts( void ) {
	gentity_t *spot;
	int count;

	spot = NULL;
	count = 0;
	while ( ( spot = G_Find( spot, FOFS(classname), "info_player_start" ) ) != NULL ) {
		count++;
	}

	return count;
}

static qboolean RaceGridHasNumberedStarts( void ) {
	gentity_t *spot;

	spot = NULL;
	while ( ( spot = G_Find( spot, FOFS(classname), "info_player_start" ) ) != NULL ) {
		if ( spot->number ) {
			return qtrue;
		}
	}

	return qfalse;
}

static void BuildTemporaryGridSlot( vec3_t baseOrigin, vec3_t baseAngles, int slotIndex, vec3_t outOrigin, vec3_t outAngles ) {
	vec3_t forward, right;
	float row;
	float column;

	AngleVectors( baseAngles, forward, right, NULL );
	row = (float)( slotIndex / OVERFLOW_GRID_COLUMNS );
	column = (float)( slotIndex % OVERFLOW_GRID_COLUMNS );

	VectorCopy( baseOrigin, outOrigin );
	VectorMA( outOrigin, -(( row + 1.0f ) * OVERFLOW_GRID_SPACING), forward, outOrigin );
	VectorMA( outOrigin, ( column - ( ( OVERFLOW_GRID_COLUMNS - 1 ) * 0.5f ) ) * OVERFLOW_GRID_SPACING, right, outOrigin );

	VectorCopy( baseAngles, outAngles );
}

static qboolean SnapTemporaryGridSlotToGround( vec3_t candidateOrigin, vec3_t outOrigin ) {
	trace_t tr;
	vec3_t start, end;
	vec3_t mins, maxs;

	VectorSet( mins, -CAR_WIDTH / 2.0f, -CAR_WIDTH / 2.0f, -CAR_HEIGHT / 2.0f );
	VectorSet( maxs, CAR_WIDTH / 2.0f, CAR_WIDTH / 2.0f, CAR_HEIGHT / 2.0f );

	VectorCopy( candidateOrigin, start );
	start[2] += TEMP_GRID_GROUND_TRACE_UP;
	VectorCopy( candidateOrigin, end );
	end[2] -= TEMP_GRID_GROUND_TRACE_DOWN;

	trap_Trace( &tr, start, mins, maxs, end, ENTITYNUM_NONE, MASK_PLAYERSOLID & ~CONTENTS_BODY );
	if ( tr.allsolid || tr.startsolid || tr.fraction == 1.0f ) {
		return qfalse;
	}

	if ( tr.plane.normal[2] < TEMP_GRID_MIN_GROUND_NORMAL ) {
		return qfalse;
	}

	VectorCopy( tr.endpos, outOrigin );
	outOrigin[2] += TEMP_GRID_GROUND_CLEARANCE;

	trap_Trace( &tr, outOrigin, mins, maxs, outOrigin, ENTITYNUM_NONE, MASK_PLAYERSOLID & ~CONTENTS_BODY );
	if ( tr.allsolid || tr.startsolid ) {
		return qfalse;
	}

	return qtrue;
}

static qboolean FindTemporaryGridAnchor( vec3_t baseOrigin, vec3_t baseAngles ) {
	gentity_t *spot;

	spot = G_Find( NULL, FOFS(classname), "info_player_start" );
	if ( spot ) {
		VectorCopy( spot->s.origin, baseOrigin );
		VectorCopy( spot->s.angles, baseAngles );
		return qtrue;
	}

	spot = G_Find( NULL, FOFS(classname), "rally_checkpoint" );
	if ( spot ) {
		VectorCopy( spot->s.origin, baseOrigin );
		VectorCopy( spot->s.angles, baseAngles );
		return qfalse;
	}

	VectorClear( baseOrigin );
	VectorClear( baseAngles );
	return qfalse;
}

TESTABLE_STATIC gentity_t *SelectOverflowGridPosition( gentity_t *baseSpot, int overflowIndex, gentity_t *ent, vec3_t origin, vec3_t angles ) {
	vec3_t			tempOrigin, tempAngles, groundedOrigin;
	int				attempts = 0;
	int				currentIndex = overflowIndex;

	while ( attempts < 16 ) {
		BuildTemporaryGridSlot( baseSpot->s.origin, baseSpot->s.angles, currentIndex, tempOrigin, tempAngles );
		if ( !SnapTemporaryGridSlotToGround( tempOrigin, groundedOrigin ) ) {
			attempts++;
			currentIndex++;
			continue;
		}

		VectorCopy( groundedOrigin, overflowSpot.s.origin );
		VectorCopy( tempAngles, overflowSpot.s.angles );

		if ( !SpotWouldTelefrag( &overflowSpot ) ) {
			VectorCopy( overflowSpot.s.origin, origin );
			VectorCopy( overflowSpot.s.angles, angles );

			if ( ent && ent->client ) {
				trap_SendServerCommand( ent - g_entities, va( "cp \"Grid overflow slot %d assigned\"", currentIndex + 1 ) );
			}

			return &overflowSpot;
		}

		attempts++;
		currentIndex++;
	}

	return NULL;
}

gentity_t *SelectGridPositionSpawn( gentity_t *ent, vec3_t origin, vec3_t angles, qboolean isbot ) {
	gentity_t		*spot;
	gentity_t		*firstGridSpot;
	vec3_t			anchorOrigin;
	vec3_t			anchorAngles;
	qboolean		hasNumberedGridSpots;
	int				gridPosition;
	int				fallbackIndex;
	vec3_t			groundedOrigin;

	spot = NULL;
	firstGridSpot = NULL;
	hasNumberedGridSpots = RaceGridHasNumberedStarts();
	gridPosition = 1;
	while ( gridPosition <= level.maxclients ) {
		gentity_t *matchedSpot = NULL;
		int unnumberedGridPosition = 1;
		spot = NULL;

		while ((spot = G_Find (spot, FOFS(classname), "info_player_start")) != NULL) {
			if ( !firstGridSpot ) {
				firstGridSpot = spot;
			}

			if ( ( spot->number == gridPosition ) ||
				( !spot->number && !hasNumberedGridSpots && unnumberedGridPosition == gridPosition ) ||
				( !spot->number && hasNumberedGridSpots && gridPosition == 1 ) ) {
				matchedSpot = spot;
				if ( !SpotWouldTelefrag( spot ) ) {
					VectorCopy (spot->s.origin, origin);
					origin[2] += 9;
					VectorCopy (spot->s.angles, angles);

					return spot;
				}

				break;
			}

			if ( !spot->number ) {
				unnumberedGridPosition++;
			}
		}

		if ( !matchedSpot && firstGridSpot ) {
			break;
		}

		if ( matchedSpot && SpotWouldTelefrag( matchedSpot ) ) {
			// try the next numbered grid position
			gridPosition++;
			continue;
		}

		if ( !matchedSpot ) {
			gridPosition++;
		}
	}

	fallbackIndex = gridPosition - 1;
	if ( firstGridSpot ) {
		gentity_t *overflow = SelectOverflowGridPosition( firstGridSpot, fallbackIndex, ent, origin, angles );
		if ( overflow ) {
			return overflow;
		}
		fallbackIndex += 16;
	}

	if ( !missingGridStartsNotified ) {
		missingGridStartsNotified = qtrue;
		G_Printf("Warning: Missing info_player_start for race grid; assigning temporary safe grid slots (telefrag protection active)\n");
		CenterPrint_All("Operator notice: missing info_player_start, using temporary safe grid slots");
	}

	if ( !FindTemporaryGridAnchor( anchorOrigin, anchorAngles ) ) {
		if ( !hasNumberedGridSpots ) {
			G_Printf("Warning: Race map has no numbered info_player_start entities for full grid coverage\n");
		}
	}

	for ( gridPosition = fallbackIndex; gridPosition < fallbackIndex + 256; gridPosition++ ) {
		BuildTemporaryGridSlot( anchorOrigin, anchorAngles, gridPosition, overflowSpot.s.origin, overflowSpot.s.angles );
		if ( !SnapTemporaryGridSlotToGround( overflowSpot.s.origin, groundedOrigin ) ) {
			continue;
		}

		VectorCopy( groundedOrigin, overflowSpot.s.origin );
		if ( SpotWouldTelefrag( &overflowSpot ) ) {
			continue;
		}

		VectorCopy( overflowSpot.s.origin, origin );
		VectorCopy( overflowSpot.s.angles, angles );
		return &overflowSpot;
	}

	G_Printf("Warning: Unable to find telefrag-safe temporary race grid slot, using emergency spawn fallback\n");
	return SelectSpawnPoint( vec3_origin, origin, angles, isbot );
}
