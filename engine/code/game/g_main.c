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
//

#include "g_local.h"
#include "g_profile.h"
#include "bg_achievements.h"

level_locals_t	level;

typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
	int			modificationCount;  // for tracking changes
	qboolean	trackChange;	    // track this variable, and announce if changed
  qboolean teamShader;        // track and if changed, update shader state
} cvarTable_t;

gentity_t		g_entities[MAX_GENTITIES];
gclient_t               g_clients[MAX_CLIENTS];

static void G_LadderBuildMatchId( int randomSeed );
static const char *G_LadderModeForGametype( int gametype );
static void G_LadderFormatIsoTime( const qtime_t *qt, char *buffer, size_t size );
static qboolean G_LadderPopulatePlayer( ladderMatchPayload_t *payload, int clientNum );
static void G_LadderSubmitMatchReport( const char *reason );
static void G_ValidateDerbyDamageCvars( void );
static ladderMatchPayload_t s_ladderMatchPayload;

vmCvar_t	g_gametype;
vmCvar_t	g_dmflags;
vmCvar_t	g_fraglimit;
// STONELANCE
vmCvar_t	g_laplimit;
vmCvar_t	g_timeTrialLaps;
vmCvar_t	g_eliminationStartDelay;
vmCvar_t	g_eliminationInterval;
vmCvar_t	g_eliminationWarning;
vmCvar_t	g_eliminationWeapons;
// END
vmCvar_t	g_timelimit;
vmCvar_t	g_capturelimit;
vmCvar_t	g_friendlyFire;
vmCvar_t	g_password;
vmCvar_t	g_needpass;
vmCvar_t	g_maxclients;
vmCvar_t	g_maxGameClients;
vmCvar_t	g_dedicated;
vmCvar_t	g_speed;
vmCvar_t	g_gravity;
vmCvar_t	g_cheats;
vmCvar_t	g_knockback;
vmCvar_t	g_quadfactor;
vmCvar_t	g_forcerespawn;
vmCvar_t	g_inactivity;
vmCvar_t	g_debugMove;
vmCvar_t	g_debugDamage;
vmCvar_t	g_debugAlloc;
vmCvar_t	g_weaponRespawn;
vmCvar_t	g_weaponTeamRespawn;
vmCvar_t	g_motd;
vmCvar_t	g_synchronousClients;
vmCvar_t	g_warmup;
vmCvar_t	g_doWarmup;
vmCvar_t	g_restarted;
vmCvar_t	g_logfile;
vmCvar_t	g_logfileSync;
vmCvar_t	g_blood;
vmCvar_t	g_podiumDist;
vmCvar_t	g_podiumDrop;
vmCvar_t	g_allowVote;
vmCvar_t	g_teamAutoJoin;
vmCvar_t	g_teamForceBalance;
vmCvar_t	g_banIPs;
vmCvar_t	g_filterBan;
vmCvar_t	g_smoothClients;
vmCvar_t	pmove_fixed;
vmCvar_t	pmove_msec;
vmCvar_t	g_rankings;
vmCvar_t	g_listEntity;
vmCvar_t	g_localTeamPref;
vmCvar_t    g_enableSnow;
vmCvar_t    g_dominationScoreInterval;
vmCvar_t    g_dominationCaptureDelay;
vmCvar_t    g_dominationSpawnStyle;
// Q3Rally Code Start - KOTH
vmCvar_t    g_kothScoreWin;
vmCvar_t    g_kothCaptureTime;
vmCvar_t    g_kothRespawnWave;
vmCvar_t    g_kothPtsTick;
vmCvar_t    g_kothPtsCapture;
vmCvar_t    g_kothPtsDefend;
vmCvar_t    g_kothOvertime;
vmCvar_t    g_kothOvertimeHoldTime;
// Q3Rally Code END - KOTH
#ifdef MISSIONPACK
vmCvar_t	g_obeliskHealth;
vmCvar_t	g_obeliskRegenPeriod;
vmCvar_t	g_obeliskRegenAmount;
vmCvar_t	g_obeliskRespawnDelay;
vmCvar_t	g_cubeTimeout;
vmCvar_t	g_redteam;
vmCvar_t	g_blueteam;
vmCvar_t	g_singlePlayer;
vmCvar_t	g_enableDust;
vmCvar_t	g_enableBreath;
vmCvar_t	g_proxMineTimeout;
#endif
// STONELANCE
vmCvar_t	g_forceEngineStart;
vmCvar_t	g_finishRaceDelay;
vmCvar_t	g_timeTrialFinishDelay;
vmCvar_t	g_trackReversed;
vmCvar_t	g_trackLength;
vmCvar_t	g_developer;
vmCvar_t	g_rallyReadyCheck;
vmCvar_t	g_derbyMinPlayers;
vmCvar_t	g_rallyIntroCamClients;
vmCvar_t	g_debugIntroCam;
vmCvar_t	g_rallyIgnoreBots;
vmCvar_t	g_aiDmnetDebugExport;
vmCvar_t	g_aiDmnetDebugExportPath;

vmCvar_t	g_damageScale;
vmCvar_t	g_vehicleDamageScale;
vmCvar_t        g_vehicleDamageOffset;
vmCvar_t	g_vehicleHealth;
vmCvar_t        g_derbyDamageFactor;
vmCvar_t        g_derbyRammerDamageRatio;
vmCvar_t        g_derbyIgnoreDamageScale;
vmCvar_t        g_derbyRamRadius;
vmCvar_t        g_derbyRamDamage;
vmCvar_t        g_derbyRamDamageScale;
vmCvar_t        g_derbyRamDamageMax;
vmCvar_t        g_derbyCollisionFrontWeight;
vmCvar_t        g_derbyCollisionSideWeight;
vmCvar_t        g_derbyCollisionRearWeight;
vmCvar_t        g_derbyCollisionLog;
vmCvar_t  g_humanplayers;
vmCvar_t        g_fuelKillReward;
vmCvar_t        g_useFuel;

// car variables
vmCvar_t	car_spring;
vmCvar_t	car_shock_up;
vmCvar_t	car_shock_down;
vmCvar_t	car_swaybar;
vmCvar_t	car_wheel;
vmCvar_t	car_wheel_damp;

vmCvar_t	car_frontweight_dist;
vmCvar_t	car_IT_xScale;
vmCvar_t	car_IT_yScale;
vmCvar_t	car_IT_zScale;
vmCvar_t	car_body_elasticity;

vmCvar_t	car_air_cof;
vmCvar_t	car_air_frac_to_df;
vmCvar_t	car_friction_scale;
// END

// bk001129 - made static to avoid aliasing
static cvarTable_t		gameCvarTable[] = {
	// don't override the cheat state set by the system
	{ &g_cheats, "sv_cheats", "", 0, 0, qfalse },

	// noset vars
	{ NULL, "gamename", GAMEVERSION , CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },
	{ NULL, "gamedate", PRODUCT_DATE , CVAR_ROM, 0, qfalse  },
	{ &g_restarted, "g_restarted", "0", CVAR_ROM, 0, qfalse  },

	// latched vars
// STONELANCE - change default
//	{ &g_gametype, "g_gametype", "0", CVAR_SERVERINFO | CVAR_USERINFO | CVAR_LATCH, 0, qfalse  },
	{ &g_gametype, "g_gametype", "1", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse  },
// END

	{ &g_maxclients, "sv_maxclients", "8", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },
	{ &g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },

	// change anytime vars
	{ &g_dmflags, "dmflags", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue  },
	{ &g_fraglimit, "fraglimit", "20", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
// STONELANCE
	{ &g_laplimit, "laplimit", "5", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_timeTrialLaps, "g_timeTrialLaps", "3", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
        { &g_eliminationStartDelay, "g_eliminationStartDelay", "30000", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
        { &g_eliminationInterval, "g_eliminationInterval", "15000", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
        { &g_eliminationWarning, "g_eliminationWarning", "5000", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
{ &g_eliminationWeapons, "g_eliminationWeapons", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
// END
	{ &g_timelimit, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_capturelimit, "capturelimit", "8", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },

	{ &g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, 0, qfalse  },

	{ &g_friendlyFire, "g_friendlyFire", "0", CVAR_ARCHIVE, 0, qtrue  },

	{ &g_teamAutoJoin, "g_teamAutoJoin", "0", CVAR_ARCHIVE  },
	{ &g_teamForceBalance, "g_teamForceBalance", "0", CVAR_ARCHIVE  },

	{ &g_warmup, "g_warmup", "20", CVAR_ARCHIVE, 0, qtrue  },
	{ &g_doWarmup, "g_doWarmup", "0", CVAR_ARCHIVE, 0, qtrue  },
	{ &g_logfile, "g_log", "games.log", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_logfileSync, "g_logsync", "0", CVAR_ARCHIVE, 0, qfalse  },

	{ &g_password, "g_password", "", CVAR_USERINFO, 0, qfalse  },

	{ &g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, 0, qfalse  },

	{ &g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse },

	{ &g_dedicated, "dedicated", "0", 0, 0, qfalse  },

	{ &g_speed, "g_speed", "320", 0, 0, qtrue  },
// STONELANCE
//	{ &g_gravity, "g_gravity", "800", 0, 0, qtrue  },
	{ &g_gravity, "g_gravity", "1400", 0, 0, qtrue  },
// END
	{ &g_knockback, "g_knockback", "1000", 0, 0, qtrue  },
	{ &g_quadfactor, "g_quadfactor", "3", 0, 0, qtrue  },
	{ &g_weaponRespawn, "g_weaponrespawn", "5", 0, 0, qtrue  },
	{ &g_weaponTeamRespawn, "g_weaponTeamRespawn", "30", 0, 0, qtrue },
	{ &g_forcerespawn, "g_forcerespawn", "20", 0, 0, qtrue },
	{ &g_inactivity, "g_inactivity", "0", 0, 0, qtrue },
	{ &g_debugMove, "g_debugMove", "0", 0, 0, qfalse },
	{ &g_debugDamage, "g_debugDamage", "0", 0, 0, qfalse },
	{ &g_debugAlloc, "g_debugAlloc", "0", 0, 0, qfalse },
	{ &g_motd, "g_motd", "", 0, 0, qfalse },
	{ &g_blood, "com_blood", "1", 0, 0, qfalse },

	{ &g_podiumDist, "g_podiumDist", "80", 0, 0, qfalse },
	{ &g_podiumDrop, "g_podiumDrop", "70", 0, 0, qfalse },

	{ &g_allowVote, "g_allowVote", "1", CVAR_ARCHIVE, 0, qfalse },
	{ &g_listEntity, "g_listEntity", "0", 0, 0, qfalse },
    { &g_enableSnow, "g_enableSnow", "0", CVAR_SERVERINFO, 0, qtrue, qfalse },

#ifdef MISSIONPACK
	{ &g_obeliskHealth, "g_obeliskHealth", "2500", 0, 0, qfalse },
	{ &g_obeliskRegenPeriod, "g_obeliskRegenPeriod", "1", 0, 0, qfalse },
	{ &g_obeliskRegenAmount, "g_obeliskRegenAmount", "15", 0, 0, qfalse },
	{ &g_obeliskRespawnDelay, "g_obeliskRespawnDelay", "10", CVAR_SERVERINFO, 0, qfalse },

	{ &g_cubeTimeout, "g_cubeTimeout", "30", 0, 0, qfalse },
	{ &g_redteam, "g_redteam", "Stroggs", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO , 0, qtrue, qtrue },
	{ &g_blueteam, "g_blueteam", "Pagans", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO , 0, qtrue, qtrue  },
	{ &g_singlePlayer, "ui_singlePlayerActive", "", 0, 0, qfalse, qfalse  },

	{ &g_enableDust, "g_enableDust", "0", CVAR_SERVERINFO, 0, qtrue, qfalse },
	{ &g_enableBreath, "g_enableBreath", "0", CVAR_SERVERINFO, 0, qtrue, qfalse },
	{ &g_proxMineTimeout, "g_proxMineTimeout", "20000", 0, 0, qfalse },
#endif
	{ &g_smoothClients, "g_smoothClients", "1", 0, 0, qfalse},
// STONELANCE
//	{ &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO, 0, qfalse},
//	{ &pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO, 0, qfalse},

	{ &pmove_fixed, "pmove_fixed", "1", CVAR_SYSTEMINFO, 0, qfalse},
	{ &pmove_msec, "pmove_msec", "18", CVAR_SYSTEMINFO, 0, qfalse},

	// UPDATE : needs to be server info?
	{ &g_trackReversed, "g_trackReversed", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse  },
	{ &g_trackLength, "g_trackLength", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse  },

{ &g_forceEngineStart, "g_forceEngineStart", "60", CVAR_ARCHIVE, 0, qfalse },
{ &g_finishRaceDelay, "g_finishRaceDelay", "30", CVAR_ARCHIVE, 0, qfalse },
{ &g_timeTrialFinishDelay, "g_timeTrialFinishDelay", "10", CVAR_ARCHIVE, 0, qfalse },

{ &g_developer, "developer", "0", 0, 0, qfalse },
{ &g_rallyReadyCheck, "g_rallyReadyCheck", "1", CVAR_ARCHIVE, 0, qfalse },
{ &g_derbyMinPlayers, "g_derbyMinPlayers", "2", CVAR_ARCHIVE, 0, qfalse },
// 0 = spectators/race observers only, 1 = all human clients before race start
{ &g_rallyIntroCamClients, "g_rallyIntroCamClients", "1", CVAR_ARCHIVE, 0, qfalse },
{ &g_debugIntroCam, "g_debugIntroCam", "0", CVAR_ARCHIVE, 0, qfalse },
{ &g_rallyIgnoreBots, "g_rallyIgnoreBots", "0", CVAR_ARCHIVE, 0, qfalse },
{ &g_aiDmnetDebugExport, "g_aiDmnetDebugExport", "0", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
{ &g_aiDmnetDebugExportPath, "g_aiDmnetDebugExportPath", "logs/ai_dmnet_debug.csv", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
{ &g_humanplayers, "g_humanplayers", "0", CVAR_ROM | CVAR_NORESTART, 0, qfalse },
{ &g_fuelKillReward, "g_fuelKillReward", "10", CVAR_ARCHIVE, 0, qfalse },
{ &g_useFuel, "g_useFuel", "1", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },

        // car variables
	// vehicle handling values are latched so they stay deterministic during an active match;
	// all values are also published via serverinfo for cgame prediction setup.
	{ &car_spring, "car_spring", "120", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &car_shock_up, "car_shock_up", "12", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &car_shock_down, "car_shock_down", "11", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &car_swaybar, "car_swaybar", "20", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &car_wheel, "car_wheel", "2400", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &car_wheel_damp, "car_wheel_damp", "140", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },

	{ &car_frontweight_dist, "car_frontweight_dist", "0.5", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &car_IT_xScale, "car_IT_xScale", "1.0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &car_IT_yScale, "car_IT_yScale", "1.0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &car_IT_zScale, "car_IT_zScale", "1.0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &car_body_elasticity, "car_body_elasticity", "0.05", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },

	{ &car_air_cof, "car_air_cof", "0.31", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &car_air_frac_to_df, "car_air_frac_to_df", "0.5", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &car_friction_scale, "car_friction_scale", "1.1", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },

        { &g_damageScale, "g_damageScale", "0.3", CVAR_ARCHIVE, 0, qfalse },
        { &g_vehicleDamageScale, "g_vehicleDamageScale", "5.0", CVAR_ARCHIVE, 0, qfalse },
        { &g_vehicleDamageOffset, "g_vehicleDamageOffset", "0", CVAR_ARCHIVE, 0, qfalse },
        { &g_vehicleHealth, "g_vehicleHealth", "100", CVAR_ARCHIVE, 0, qfalse },
        { &g_derbyDamageFactor, "g_derbyDamageFactor", "1.0", CVAR_ARCHIVE, 0, qfalse },
        { &g_derbyRammerDamageRatio, "g_derbyRammerDamageRatio", "1.0", CVAR_ARCHIVE, 0, qfalse },
        { &g_derbyIgnoreDamageScale, "g_derbyIgnoreDamageScale", "0", CVAR_ARCHIVE, 0, qfalse },
        { &g_derbyRamRadius, "g_derbyRamRadius", "0", CVAR_ARCHIVE, 0, qfalse },
        { &g_derbyRamDamage, "g_derbyRamDamage", "100", CVAR_ARCHIVE, 0, qfalse },
        { &g_derbyRamDamageScale, "g_derbyRamDamageScale", "0.05", CVAR_ARCHIVE, 0, qfalse },
        { &g_derbyRamDamageMax, "g_derbyRamDamageMax", "50", CVAR_ARCHIVE, 0, qfalse },
        { &g_derbyCollisionFrontWeight, "g_derbyCollisionFrontWeight", "1.0", CVAR_ARCHIVE, 0, qfalse },
        { &g_derbyCollisionSideWeight, "g_derbyCollisionSideWeight", "0.65", CVAR_ARCHIVE, 0, qfalse },
        { &g_derbyCollisionRearWeight, "g_derbyCollisionRearWeight", "0.35", CVAR_ARCHIVE, 0, qfalse },
        { &g_derbyCollisionLog, "g_derbyCollisionLog", "0", 0, 0, qfalse },
        // END

        { &g_rankings, "g_rankings", "0", 0, 0, qfalse},
	{ &g_localTeamPref, "g_localTeamPref", "", 0, 0, qfalse },

	{ &g_dominationScoreInterval, "g_dominationScoreInterval", "10000", CVAR_ARCHIVE, 0, qfalse },
	{ &g_dominationCaptureDelay, "g_dominationCaptureDelay", "1500", CVAR_ARCHIVE, 0, qfalse },
	{ &g_dominationSpawnStyle, "g_dominationSpawnStyle", "0", CVAR_ARCHIVE, 0, qfalse },
		// Q3Rally Code Start - KOTH
		{ &g_kothScoreWin, "g_kothScoreWin", "100", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },
		{ &g_kothCaptureTime, "g_kothCaptureTime", "3000", CVAR_ARCHIVE, 0, qfalse },
		{ &g_kothRespawnWave, "g_kothRespawnWave", "5000", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },
		{ &g_kothPtsTick, "g_kothPtsTick", "1", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },
		{ &g_kothPtsCapture, "g_kothPtsCapture", "5", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },
		{ &g_kothPtsDefend, "g_kothPtsDefend", "3", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },
		{ &g_kothOvertime, "g_kothOvertime", "1", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },
		{ &g_kothOvertimeHoldTime, "g_kothOvertimeHoldTime", "10000", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse }
		// Q3Rally Code END - KOTH
};

static int gameCvarTableSize = ARRAY_LEN( gameCvarTable );


void G_InitGame( int levelTime, int randomSeed, int restart );
void G_RunFrame( int levelTime );
void G_ShutdownGame( int restart );
void CheckExitRules( void );


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
Q_EXPORT intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  ) {
	switch ( command ) {
	case GAME_INIT:
// STONELANCE
//		G_DebugLogPrintf("INIT\n");
// END
		G_InitGame( arg0, arg1, arg2 );
		return 0;
	case GAME_SHUTDOWN:
// STONELANCE
//		G_DebugLogPrintf("SHUTDOWN\n");
// END
		G_ShutdownGame( arg0 );
		return 0;
	case GAME_CLIENT_CONNECT:
// STONELANCE
//		G_DebugLogPrintf("CLIENT CONNECT\n");
// END
		return (intptr_t)ClientConnect( arg0, arg1, arg2 );
	case GAME_CLIENT_THINK:
// STONELANCE
//		if (level.intermissiontime)
//			G_DebugLogPrintf("CLIENT THINK %d\n", arg0);
// END
		ClientThink( arg0 );
		return 0;
	case GAME_CLIENT_USERINFO_CHANGED:
// STONELANCE
//		G_DebugLogPrintf("USERINFO_CHANGED\n");
// END
		ClientUserinfoChanged( arg0 );
		return 0;
	case GAME_CLIENT_DISCONNECT:
// STONELANCE
//		G_DebugLogPrintf("DISCONNECT\n");
// END
		ClientDisconnect( arg0 );
		return 0;
	case GAME_CLIENT_BEGIN:
// STONELANCE
//		G_DebugLogPrintf("CLIENT BEGIN\n");
// END
		ClientBegin( arg0 );
		return 0;
	case GAME_CLIENT_COMMAND:
// STONELANCE
//		G_DebugLogPrintf("CLIENT COMMAND\n");
// END
		ClientCommand( arg0 );
		return 0;
	case GAME_RUN_FRAME:
		G_RunFrame( arg0 );
		return 0;
	case GAME_CONSOLE_COMMAND:
// STONELANCE
//		G_DebugLogPrintf("CONSOLE COMMAND\n");
// END
		return ConsoleCommand();
	case BOTAI_START_FRAME:
// STONELANCE
//		if (level.intermissiontime)
//			G_DebugLogPrintf("BOTAI FRAME\n");
// END
		return BotAIStartFrame( arg0 );
	}

	return -1;
}


void QDECL G_Printf( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Print( text );
}

void QDECL G_Error( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Error( text );
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void ) {
	gentity_t	*e, *e2;
	int		i, j;
	int		c, c2;

	c = 0;
	c2 = 0;
	for ( i=MAX_CLIENTS, e=g_entities+i ; i < level.num_entities ; i++,e++ ) {
		if (!e->inuse)
			continue;
		if (!e->team)
			continue;
		if (e->flags & FL_TEAMSLAVE)
			continue;
		e->teammaster = e;
		c++;
		c2++;
		for (j=i+1, e2=e+1 ; j < level.num_entities ; j++,e2++)
		{
			if (!e2->inuse)
				continue;
			if (!e2->team)
				continue;
			if (e2->flags & FL_TEAMSLAVE)
				continue;
			if (!strcmp(e->team, e2->team))
			{
				c2++;
				e2->teamchain = e->teamchain;
				e->teamchain = e2;
				e2->teammaster = e;
				e2->flags |= FL_TEAMSLAVE;

				// make sure that targets only point at the master
				if ( e2->targetname ) {
					e->targetname = e2->targetname;
					e2->targetname = NULL;
				}
			}
		}
	}

	G_Printf ("%i teams with %i entities\n", c, c2);
}

void G_RemapTeamShaders( void ) {
#ifdef MISSIONPACK
	char string[1024];
	float f = level.time * 0.001;
	Com_sprintf( string, sizeof(string), "team_icon/%s_red", g_redteam.string );
	AddRemap("textures/ctf2/redteam01", string, f); 
	AddRemap("textures/ctf2/redteam02", string, f); 
	Com_sprintf( string, sizeof(string), "team_icon/%s_blue", g_blueteam.string );
	AddRemap("textures/ctf2/blueteam01", string, f); 
	AddRemap("textures/ctf2/blueteam02", string, f); 
	trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
#endif
}


/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void ) {
	int			i;
	cvarTable_t	*cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName,
			cv->defaultString, cv->cvarFlags );
		if ( cv->vmCvar )
			cv->modificationCount = cv->vmCvar->modificationCount;

		if (cv->teamShader) {
			remapped = qtrue;
		}
	}

	if (remapped) {
		G_RemapTeamShaders();
	}

	// check some things
	if ( g_gametype.integer < 0 || g_gametype.integer >= GT_MAX_GAME_TYPE ) {
// STONELANCE
/*
		G_Printf( "g_gametype %i is out of range, defaulting to 0\n", g_gametype.integer );
		trap_Cvar_Set( "g_gametype", "0" );
*/
		G_Printf( "g_gametype %i is out of range, defaulting to 1\n", g_gametype.integer );
		trap_Cvar_Set( "g_gametype", "1" );
// END
		trap_Cvar_Update( &g_gametype );
	}

	level.warmupModificationCount = g_warmup.modificationCount;

        G_ValidateDerbyDamageCvars();
}

/*
=================
G_UpdateCvars
=================
*/
static void G_ValidateDerbyDamageCvars( void ) {
        float clamped;

        clamped = Com_Clamp( 0.0f, 2.0f, g_derbyRammerDamageRatio.value );
        if ( clamped != g_derbyRammerDamageRatio.value ) {
                trap_Cvar_Set( "g_derbyRammerDamageRatio", va( "%.3f", clamped ) );
                trap_Cvar_Update( &g_derbyRammerDamageRatio );
        }

        if ( g_derbyRamDamage.value < 0.0f ) {
                trap_Cvar_Set( "g_derbyRamDamage", "0" );
                trap_Cvar_Update( &g_derbyRamDamage );
        }

        if ( g_derbyRamDamageScale.value < 0.0f ) {
                trap_Cvar_Set( "g_derbyRamDamageScale", "0" );
                trap_Cvar_Update( &g_derbyRamDamageScale );
        }

        if ( g_derbyRamDamageMax.value < 1.0f ) {
                trap_Cvar_Set( "g_derbyRamDamageMax", "1" );
                trap_Cvar_Update( &g_derbyRamDamageMax );
        }

        if ( g_derbyDamageFactor.value < 0.0f ) {
                trap_Cvar_Set( "g_derbyDamageFactor", "0" );
                trap_Cvar_Update( &g_derbyDamageFactor );
        }
        clamped = Com_Clamp( 0.0f, 2.0f, g_derbyCollisionFrontWeight.value );
        if ( clamped != g_derbyCollisionFrontWeight.value ) {
                trap_Cvar_Set( "g_derbyCollisionFrontWeight", va( "%.3f", clamped ) );
                trap_Cvar_Update( &g_derbyCollisionFrontWeight );
        }

        clamped = Com_Clamp( 0.0f, 2.0f, g_derbyCollisionSideWeight.value );
        if ( clamped != g_derbyCollisionSideWeight.value ) {
                trap_Cvar_Set( "g_derbyCollisionSideWeight", va( "%.3f", clamped ) );
                trap_Cvar_Update( &g_derbyCollisionSideWeight );
        }

        clamped = Com_Clamp( 0.0f, 2.0f, g_derbyCollisionRearWeight.value );
        if ( clamped != g_derbyCollisionRearWeight.value ) {
                trap_Cvar_Set( "g_derbyCollisionRearWeight", va( "%.3f", clamped ) );
                trap_Cvar_Update( &g_derbyCollisionRearWeight );
        }
}

void G_UpdateCvars( void ) {
	int			i;
	cvarTable_t	*cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		if ( cv->vmCvar ) {
			trap_Cvar_Update( cv->vmCvar );

			if ( cv->modificationCount != cv->vmCvar->modificationCount ) {
				cv->modificationCount = cv->vmCvar->modificationCount;

				if ( cv->trackChange ) {
					trap_SendServerCommand( -1, va("print \"Server: %s changed to %s\n\"", 
						cv->cvarName, cv->vmCvar->string ) );
				}

				if (cv->teamShader) {
					remapped = qtrue;
				}
			}
		}
	}

	if (remapped) {
		G_RemapTeamShaders();
	}

        G_ValidateDerbyDamageCvars();
}

static void G_LadderBuildMatchId( int randomSeed ) {
        unsigned int seed = (unsigned int)randomSeed;
        unsigned int hash = 0;
        char serverinfo[MAX_INFO_STRING];
        const char *value;

        trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );

        value = Info_ValueForKey( serverinfo, "mapname" );
        if ( value && value[0] ) {
                const unsigned char *cursor = (const unsigned char *)value;
                while ( *cursor ) {
                        hash = ( hash * 33u ) ^ *cursor++;
                }
        }

        value = Info_ValueForKey( serverinfo, "sv_hostname" );
        if ( value && value[0] ) {
                const unsigned char *cursor = (const unsigned char *)value;
                while ( *cursor ) {
                        hash = ( hash * 33u ) ^ *cursor++;
                }
        }

        if ( !level.ladderStartEpoch ) {
                qtime_t qt;
                level.ladderStartEpoch = trap_RealTime( &qt );
                level.ladderStartTime = qt;
        }

        if ( !hash ) {
                hash = seed ^ (unsigned int)level.time ^ (unsigned int)level.startTime;
        }

        Com_sprintf( level.ladderMatchId, sizeof( level.ladderMatchId ),
                "%08X-%08X-%08X", (unsigned int)level.ladderStartEpoch, seed, hash );
}

static void G_LadderFormatIsoTime( const qtime_t *qt, char *buffer, size_t size ) {
        if ( !buffer || size <= 0 ) {
                return;
        }

        if ( !qt ) {
                buffer[0] = '\0';
                return;
        }

        Com_sprintf( buffer, size, "%04i-%02i-%02iT%02i:%02i:%02i",
                qt->tm_year + 1900, qt->tm_mon + 1, qt->tm_mday,
                qt->tm_hour, qt->tm_min, qt->tm_sec );
}

static void G_RallyApplySprintDefaults( void ) {
        if ( g_gametype.integer != GT_SPRINT ) {
                return;
        }

        level.numberOfLaps = 1;
        trap_Cvar_Set( "laplimit", "1" );
        trap_Cvar_Update( &g_laplimit );
}

static const char *G_LadderModeForGametype( int gametype ) {
        switch ( gametype ) {
        case GT_RACING:
                return "GT_RACING";
        case GT_RACING_DM:
                return "GT_RACING_DM";
        case GT_SPRINT:
                return "GT_SPRINT";
        case GT_SINGLE_PLAYER:
                return "GT_SINGLE_PLAYER";
        case GT_DERBY:
                return "GT_DERBY";
        case GT_LCS:
                return "GT_LCS";
        case GT_ELIMINATION:
                return "GT_ELIMINATION";
        case GT_DEATHMATCH:
                return "GT_DEATHMATCH";
        case GT_TEAM:
                return "GT_TEAM";
        case GT_TEAM_RACING:
                return "GT_TEAM_RACING";
        case GT_TEAM_RACING_DM:
                return "GT_TEAM_RACING_DM";
        case GT_CTF:
                return "GT_CTF";
        case GT_CTF4:
                return "GT_CTF4";
        case GT_DOMINATION:
                return "GT_DOMINATION";
        case GT_KOTH:
                return "GT_KOTH";
        default:
                break;
        }

        return "GT_ELIMINATION";
}

static qboolean G_LadderGametypeHasRaceFields( int gametype ) {
        return ( gametype == GT_RACING
                || gametype == GT_RACING_DM
                || gametype == GT_SPRINT
                || gametype == GT_TEAM_RACING
                || gametype == GT_TEAM_RACING_DM
                || gametype == GT_ELIMINATION
                || gametype == GT_SINGLE_PLAYER
                || gametype == GT_DERBY
                || gametype == GT_LCS ) ? qtrue : qfalse;
}

static qboolean G_LadderGametypeUsesEliminationSettings( int gametype ) {
        return ( gametype == GT_ELIMINATION || gametype == GT_LCS ) ? qtrue : qfalse;
}

static qboolean G_LadderGametypeIsTeamMode( int gametype ) {
        return ( gametype == GT_TEAM
                || gametype == GT_TEAM_RACING
                || gametype == GT_TEAM_RACING_DM
                || gametype == GT_CTF
                || gametype == GT_CTF4
                || gametype == GT_DOMINATION
                || gametype == GT_KOTH ) ? qtrue : qfalse;
}

static int G_LadderWinnerForGametype( int gametype ) {
        int wi;

        switch ( gametype ) {
        case GT_DEATHMATCH:
                {
                        int dmWinner = -1;
                        int dmBestScore = -99999;
                        for ( wi = 0; wi < level.maxclients; ++wi ) {
                                gclient_t *wc = &level.clients[wi];
                                if ( wc->pers.connected != CON_CONNECTED ) {
                                        continue;
                                }
                                if ( wc->sess.sessionTeam == TEAM_SPECTATOR ) {
                                        continue;
                                }
                                if ( wc->ps.persistant[PERS_SCORE] > dmBestScore ) {
                                        dmBestScore = wc->ps.persistant[PERS_SCORE];
                                        dmWinner = wi;
                                }
                        }
                        return dmWinner;
                }
        case GT_TEAM:
        case GT_TEAM_RACING:
        case GT_TEAM_RACING_DM:
        case GT_CTF:
        case GT_CTF4:
        case GT_DOMINATION:
        case GT_KOTH:
                return -1;
        default:
                break;
        }

        return level.winnerNumber;
}

static qboolean G_LadderPopulatePlayer( ladderMatchPayload_t *payload, int clientNum ) {
        gclient_t *client;
        gentity_t *ent;
        ladderPlayerPayload_t *player;
        char userinfo[MAX_INFO_STRING];
        const char *value;
        int slot;
        int i;
        int deaths;
        qboolean killSemantics;
        qboolean raceSemantics;
        qboolean zoneSemantics;
        qboolean perfectSemantics;

        if ( !payload ) {
                return qfalse;
        }

        if ( clientNum < 0 || clientNum >= level.maxclients ) {
                return qfalse;
        }

        client = &level.clients[ clientNum ];
        ent = &g_entities[ clientNum ];

        if ( client->pers.connected != CON_CONNECTED ) {
                return qfalse;
        }

        if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
                return qfalse;
        }

        if ( payload->playerCount < 0 || payload->playerCount >= MAX_CLIENTS ) {
                return qfalse;
        }

        slot = payload->playerCount;
        player = &payload->players[ slot ];
        Com_Memset( player, 0, sizeof( *player ) );

        player->clientNum = clientNum;

        trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

        /* ── playerId: UUID des Spielers ────────────────────────────────────
         * Priorität (höchste zuerst):
         *   1. cl_uuid aus dem Userinfo — gesetzt vom Client der eine UUID
         *      im lokalen Profil hat. Stabil über Namensänderungen und IPs.
         *   2. Für den lokalen Client: UUID direkt aus dem Profil-State im RAM
         *      (Fallback falls cl_uuid noch nicht im Userinfo gesetzt wurde).
         *   3. cl_guid — Hardware-ID, instabil aber besser als Name.
         *   4. IP-Adresse — nur für Remote-Clients, letzter stabiler Anker.
         *   5. cleanName → "client-N" — absolute Notfallwerte.
         * ────────────────────────────────────────────────────────────────── */
        value = Info_ValueForKey( userinfo, "cl_uuid" );
        if ( value && value[0] ) {
                Q_strncpyz( player->playerId, value, sizeof( player->playerId ) );
        }

        if ( !player->playerId[0] && client->pers.localClient ) {
                /* Direktzugriff auf den Profil-State: vermeidet einen
                 * Userinfo-Round-Trip wenn der Cvar noch nicht propagiert ist. */
                G_Profile_GetUUID( player->playerId, sizeof( player->playerId ) );
        }

        value = Info_ValueForKey( userinfo, "cl_guid" );
        if ( value && value[0] ) {
                Q_strncpyz( player->guid, value, sizeof( player->guid ) );
                if ( !player->playerId[0] ) {
                        Q_strncpyz( player->playerId, value, sizeof( player->playerId ) );
                }
        }

        value = Info_ValueForKey( userinfo, "ip" );
        if ( value && value[0] ) {
                if ( !client->pers.localClient && !player->playerId[0] ) {
                        Q_strncpyz( player->playerId, value, sizeof( player->playerId ) );
                }
                if ( !player->guid[0] ) {
                        Q_strncpyz( player->guid, value, sizeof( player->guid ) );
                }
        }

        value = Info_ValueForKey( userinfo, "name" );
        if ( !value || !value[0] ) {
                value = client->pers.netname;
        }
        if ( value && value[0] ) {
                Q_strncpyz( player->name, value, sizeof( player->name ) );
        }
        Q_strncpyz( player->cleanName, player->name, sizeof( player->cleanName ) );
        Q_CleanStr( player->cleanName );

        if ( !player->playerId[0] ) {
                Q_strncpyz( player->playerId, player->cleanName, sizeof( player->playerId ) );
        }
        if ( !player->playerId[0] ) {
                Com_sprintf( player->playerId, sizeof( player->playerId ), "client-%i", clientNum );
        }

        value = Info_ValueForKey( userinfo, "model" );
        if ( value && value[0] ) {
                Q_strncpyz( player->model, value, sizeof( player->model ) );
        }

        value = Info_ValueForKey( userinfo, "chassis" );
        if ( value && value[0] ) {
                Q_strncpyz( player->vehicle, value, sizeof( player->vehicle ) );
        } else {
                value = Info_ValueForKey( userinfo, "vehicle" );
                if ( value && value[0] ) {
                        Q_strncpyz( player->vehicle, value, sizeof( player->vehicle ) );
                }
        }

        player->team = client->sess.sessionTeam;
        player->isBot = ( ent->r.svFlags & SVF_BOT ) ? qtrue : qfalse;
        player->score = client->ps.persistant[PERS_SCORE];
        player->ping = client->ps.ping;
        player->time = level.time - client->pers.enterTime;
        if ( player->time < 0 ) {
                player->time = 0;
        }

        player->scoreFlags = client->ps.persistant[PERS_PLAYEREVENTS];

        for ( i = 0; i < MIN( PW_NUM_POWERUPS, MAX_POWERUPS ); ++i ) {
                if ( client->ps.powerups[i] > level.time ) {
                        player->powerUps |= ( 1 << i );
                }
        }

        if ( client->accuracy_shots > 0 ) {
                player->accuracy = ( client->accuracy_hits * 100 ) / client->accuracy_shots;
        }

        player->impressiveCount = client->ps.persistant[PERS_IMPRESSIVE_COUNT];
        player->impressiveTelefragCount = client->ps.persistant[PERS_IMPRESSIVETELEFRAG_COUNT];
        player->excellentCount = client->ps.persistant[PERS_EXCELLENT_COUNT];
        player->gauntletCount = client->ps.persistant[PERS_GAUNTLET_FRAG_COUNT];
        player->defendCount = client->ps.persistant[PERS_DEFEND_COUNT];
        player->assistCount = client->ps.persistant[PERS_ASSIST_COUNT];
        player->captures = client->ps.persistant[PERS_CAPTURES];

        player->damageDealt = client->ps.stats[STAT_DAMAGE_DEALT];
        player->damageTaken = client->ps.stats[STAT_DAMAGE_TAKEN];
        player->position = client->ps.stats[STAT_POSITION];

        killSemantics = qfalse;
        raceSemantics = ( isRallyRace() || g_gametype.integer == GT_DERBY || g_gametype.integer == GT_LCS ) ? qtrue : qfalse;
        zoneSemantics = ( g_gametype.integer == GT_DOMINATION || g_gametype.integer == GT_KOTH ) ? qtrue : qfalse;

        switch ( g_gametype.integer ) {
        case GT_DEATHMATCH:
        case GT_TEAM:
        case GT_CTF:
        case GT_CTF4:
        case GT_DOMINATION:
        case GT_KOTH:
        case GT_RACING_DM:
        case GT_TEAM_RACING_DM:
        case GT_DERBY:
        case GT_LCS:
                killSemantics = qtrue;
                break;
        case GT_ELIMINATION:
                killSemantics = g_eliminationWeapons.integer ? qtrue : qfalse;
                break;
        default:
                break;
        }
        perfectSemantics = killSemantics;

        if ( raceSemantics && level.startRaceTime > 0 ) {
                if ( client->finishRaceTime > level.startRaceTime ) {
                        player->survivalMs = client->finishRaceTime - level.startRaceTime;
                } else if ( level.finishRaceTime > level.startRaceTime ) {
                        player->survivalMs = level.finishRaceTime - level.startRaceTime;
                }
        }
        player->eliminationRound = client->eliminationRound;
        player->eliminationPlayersRemaining = client->eliminationPlayersRemaining;
        player->eliminationMetric = client->eliminationMetric;

        if ( raceSemantics && level.startRaceTime > 0 && client->finishRaceTime > level.startRaceTime ) {
                player->totalRaceMs = client->finishRaceTime - level.startRaceTime;
        } else if ( raceSemantics && level.startRaceTime > 0 && level.time > level.startRaceTime ) {
                player->totalRaceMs = level.time - level.startRaceTime;
        }
        if ( raceSemantics ) {
                player->finishRaceTime = client->finishRaceTime;
                player->bestLapMs = client->bestLapMs;
        } else {
                player->finishRaceTime = 0;
                player->bestLapMs = 0;
                player->totalRaceMs = 0;
        }

        if ( raceSemantics ) {
                int lapCount = client->recordedLapCount;
                if ( lapCount < 0 ) {
                        lapCount = 0;
                }
                if ( lapCount > LADDER_MAX_LAP_TIMES ) {
                        lapCount = LADDER_MAX_LAP_TIMES;
                }
                for ( i = 0; i < lapCount; ++i ) {
                        player->lapTimes[i] = client->recordedLaps[i];
                }
                player->lapCount = lapCount;
        } else {
                player->lapCount = 0;
                for ( i = 0; i < LADDER_MAX_LAP_TIMES; ++i ) {
                        player->lapTimes[i] = 0;
                }
        }

        if ( killSemantics ) {
                player->kills = client->ps.persistant[PERS_SCORE];
                deaths = client->ps.persistant[PERS_KILLED];
        } else {
                player->kills = 0;
                deaths = 0;
        }
        player->deaths = deaths;
        if ( perfectSemantics && player->kills > 0 && deaths == 0 ) {
                player->perfect = qtrue;
        } else {
                player->perfect = qfalse;
        }
        if ( killSemantics && deaths > 0 ) {
                player->kdRatio = (float)player->kills / (float)deaths;
        } else if ( killSemantics && player->kills > 0 ) {
                player->kdRatio = (float)player->kills;
        } else {
                player->kdRatio = 0.0f;
        }

        if ( zoneSemantics ) {
                if ( g_gametype.integer == GT_KOTH ) {
                        player->zoneHoldMs = client->kothContestTimeMs;
                } else {
                        player->zoneHoldMs = 0;
                }
                player->zoneActiveSigil = -1;
        } else {
                player->zoneHoldMs = 0;
                player->zoneActiveSigil = -1;
        }

        /* Attach career profile snapshot for the local client only.
         * Prefer runtime state (g_profile), use file-read fallback only when needed. */
        if ( client->pers.localClient ) {
                ladderProfileSnapshot_t *snap = &player->profile;
                int snapshotRevision = 0;
                int snapshotEpoch = 0;
                char profileName[PROFILE_MAX_NAME];
                char profilePath[MAX_QPATH];
                fileHandle_t fh;
                int len;
                int i;

                player->profileAttached = qtrue;
                Com_Memset( snap, 0, sizeof( *snap ) );
                snap->valid = qfalse;

                if ( G_Profile_GetLadderSnapshot( snap, &snapshotRevision, &snapshotEpoch ) ) {
                        snap->valid = qtrue;
                        snap->snapshotRevision = snapshotRevision;
                        snap->snapshotEpoch = snapshotEpoch;
                } else {
                        trap_Cvar_VariableStringBuffer( "profile_active", profileName, sizeof( profileName ) );
                        if ( profileName[0] ) {
                                Com_sprintf( profilePath, sizeof( profilePath ), "profiles/%s.json", profileName );
                                len = trap_FS_FOpenFile( profilePath, &fh, FS_READ );
                                if ( len > 0 ) {
                                        static char profileBuf[8192];
                                        const char *statsSection;
                                        const char *infoSection;
                                        const char *statsBuf;
                                        const char *infoBuf;
                                        if ( len >= (int)sizeof( profileBuf ) ) len = sizeof( profileBuf ) - 1;
                                        trap_FS_Read( profileBuf, len, fh );
                                        profileBuf[len] = '\0';
                                        trap_FS_FCloseFile( fh );

                                        statsSection = strstr( profileBuf, "\"stats\"" );
                                        infoSection  = strstr( profileBuf, "\"info\"" );
                                        statsBuf = profileBuf;
                                        infoBuf  = profileBuf;
                                        if ( statsSection ) {
                                                const char *brace = strchr( statsSection, '{' );
                                                if ( brace ) statsBuf = brace;
                                        }
                                        if ( infoSection ) {
                                                const char *brace = strchr( infoSection, '{' );
                                                if ( brace ) infoBuf = brace;
                                        }

                                        snap->valid            = qtrue;
                                        snap->snapshotRevision = G_Profile_ParseIntPublic( profileBuf, "revision", 0 );
                                        snap->snapshotEpoch    = G_Profile_ParseIntPublic( profileBuf, "timestamp", 0 );
                                        snap->playerScore      = G_Profile_ParseIntPublic( statsBuf,  "playerScore",     0 );
                                        snap->currentRank      = G_Profile_ParseIntPublic( infoBuf,   "currentRank",     0 );
                                        snap->highestRank      = G_Profile_ParseIntPublic( infoBuf,   "highestRank",     0 );
                                        snap->wins             = G_Profile_ParseIntPublic( statsBuf,  "wins",            0 );
                                        snap->losses           = G_Profile_ParseIntPublic( statsBuf,  "losses",          0 );
                                        snap->kills            = G_Profile_ParseIntPublic( statsBuf,  "kills",           0 );
                                        snap->deaths           = G_Profile_ParseIntPublic( statsBuf,  "deaths",          0 );
                                        snap->flagCaptures     = G_Profile_ParseIntPublic( statsBuf,  "flagCaptures",    0 );
                                        snap->flagAssists      = G_Profile_ParseIntPublic( statsBuf,  "flagAssists",     0 );
                                        snap->bestLapMs        = G_Profile_ParseIntPublic( statsBuf,  "bestLapMs",       0 );
                                        snap->accuracyAwards   = G_Profile_ParseIntPublic( statsBuf,  "accuracyAwards",  0 );
                                        snap->excellentAwards  = G_Profile_ParseIntPublic( statsBuf,  "excellentAwards", 0 );
                                        snap->impressiveAwards = G_Profile_ParseIntPublic( statsBuf,  "impressiveAwards",0 );
                                        snap->perfectAwards    = G_Profile_ParseIntPublic( statsBuf,  "perfectAwards",   0 );
                                        snap->damageDealt      = G_Profile_ParseIntPublic( statsBuf,  "damageDealt",     0 );
                                        snap->damageTaken      = G_Profile_ParseIntPublic( statsBuf,  "damageTaken",     0 );
                                        snap->distanceKm       = (float)G_Profile_ParseDoublePublic( statsBuf, "distanceKm",  0.0 );
                                        snap->topSpeedKph      = (float)G_Profile_ParseDoublePublic( statsBuf, "topSpeedKph", 0.0 );
                                        snap->fuelUsed         = (float)G_Profile_ParseDoublePublic( statsBuf, "fuelUsed",    0.0 );
                                        snap->gamesPlayed      = G_Profile_ParseIntPublic( statsBuf,  "gamesPlayed",     0 );
                                        G_Profile_ParseStringPublic( statsBuf, "mostUsedVehicle",
                                                snap->mostUsedVehicle, sizeof( snap->mostUsedVehicle ), "" );
                                        snap->racingWins = G_Profile_ParseIntPublic( statsBuf, "racingWins", 0 );
                                        snap->racingPodiums = G_Profile_ParseIntPublic( statsBuf, "racingPodiums", 0 );
                                        snap->racingCompleted = G_Profile_ParseIntPublic( statsBuf, "racingCompleted", 0 );
                                        snap->racingTotalMs = G_Profile_ParseIntPublic( statsBuf, "racingTotalMs", 0 );
                                        snap->racingDmWins = G_Profile_ParseIntPublic( statsBuf, "racingDmWins", 0 );
                                        snap->racingDmPodiums = G_Profile_ParseIntPublic( statsBuf, "racingDmPodiums", 0 );
                                        snap->racingDmCompleted = G_Profile_ParseIntPublic( statsBuf, "racingDmCompleted", 0 );
                                        snap->racingDmTotalMs = G_Profile_ParseIntPublic( statsBuf, "racingDmTotalMs", 0 );
                                        snap->sprintWins = G_Profile_ParseIntPublic( statsBuf, "sprintWins", 0 );
                                        snap->sprintCompleted = G_Profile_ParseIntPublic( statsBuf, "sprintCompleted", 0 );
                                        snap->sprintBestMs = G_Profile_ParseIntPublic( statsBuf, "sprintBestMs", 0 );
                                        snap->eliminationWins = G_Profile_ParseIntPublic( statsBuf, "eliminationWins", 0 );
                                        snap->eliminationCompleted = G_Profile_ParseIntPublic( statsBuf, "eliminationCompleted", 0 );
                                        snap->eliminationTotalRoundsLasted = G_Profile_ParseIntPublic( statsBuf, "eliminationTotalRoundsLasted", 0 );
                                        snap->lcsWins = G_Profile_ParseIntPublic( statsBuf, "lcsWins", 0 );
                                        snap->lcsCompleted = G_Profile_ParseIntPublic( statsBuf, "lcsCompleted", 0 );
                                        snap->lcsTotalSurvivalMs = G_Profile_ParseIntPublic( statsBuf, "lcsTotalSurvivalMs", 0 );
                                        snap->derbyWins = G_Profile_ParseIntPublic( statsBuf, "derbyWins", 0 );
                                        snap->derbyCompleted = G_Profile_ParseIntPublic( statsBuf, "derbyCompleted", 0 );
                                        snap->derbyKills = G_Profile_ParseIntPublic( statsBuf, "derbyKills", 0 );
                                        snap->dmWins = G_Profile_ParseIntPublic( statsBuf, "dmWins", 0 );
                                        snap->dmCompleted = G_Profile_ParseIntPublic( statsBuf, "dmCompleted", 0 );
                                        snap->dmKills = G_Profile_ParseIntPublic( statsBuf, "dmKills", 0 );
                                        snap->ctfWins = G_Profile_ParseIntPublic( statsBuf, "ctfWins", 0 );
                                        snap->ctfCompleted = G_Profile_ParseIntPublic( statsBuf, "ctfCompleted", 0 );
                                        snap->ctfCaptures = G_Profile_ParseIntPublic( statsBuf, "ctfCaptures", 0 );
                                        snap->ctf4Wins = G_Profile_ParseIntPublic( statsBuf, "ctf4Wins", 0 );
                                        snap->ctf4Completed = G_Profile_ParseIntPublic( statsBuf, "ctf4Completed", 0 );
                                        snap->ctf4Captures = G_Profile_ParseIntPublic( statsBuf, "ctf4Captures", 0 );
                                        snap->teamWins = G_Profile_ParseIntPublic( statsBuf, "teamWins", 0 );
                                        snap->teamCompleted = G_Profile_ParseIntPublic( statsBuf, "teamCompleted", 0 );
                                        snap->teamKills = G_Profile_ParseIntPublic( statsBuf, "teamKills", 0 );
                                        snap->teamRacingWins = G_Profile_ParseIntPublic( statsBuf, "teamRacingWins", 0 );
                                        snap->teamRacingCompleted = G_Profile_ParseIntPublic( statsBuf, "teamRacingCompleted", 0 );
                                        snap->teamRacingPodiums = G_Profile_ParseIntPublic( statsBuf, "teamRacingPodiums", 0 );
                                        snap->teamRacingDmWins = G_Profile_ParseIntPublic( statsBuf, "teamRacingDmWins", 0 );
                                        snap->teamRacingDmCompleted = G_Profile_ParseIntPublic( statsBuf, "teamRacingDmCompleted", 0 );
                                        snap->teamRacingDmPodiums = G_Profile_ParseIntPublic( statsBuf, "teamRacingDmPodiums", 0 );
                                        snap->dominationWins = G_Profile_ParseIntPublic( statsBuf, "dominationWins", 0 );
                                        snap->dominationCompleted = G_Profile_ParseIntPublic( statsBuf, "dominationCompleted", 0 );
                                        snap->dominationZoneHoldMs = G_Profile_ParseIntPublic( statsBuf, "dominationZoneHoldMs", 0 );
                                        snap->kothWins = G_Profile_ParseIntPublic( statsBuf, "kothWins", 0 );
                                        snap->kothCompleted = G_Profile_ParseIntPublic( statsBuf, "kothCompleted", 0 );
                                        snap->kothZoneHoldMs = G_Profile_ParseIntPublic( statsBuf, "kothZoneHoldMs", 0 );

                                        {
                                                double progress_table[BG_ACHIEVEMENT_CATEGORY_COUNT];
                                                progress_table[BG_ACHIEVEMENT_DISTANCE]      = snap->distanceKm;
                                                progress_table[BG_ACHIEVEMENT_KILLS]         = (double)snap->kills;
                                                progress_table[BG_ACHIEVEMENT_WINS]          = (double)snap->wins;
                                                progress_table[BG_ACHIEVEMENT_SPRINT_WINS]   = (double)snap->sprintWins;
                                                progress_table[BG_ACHIEVEMENT_FLAG_CAPTURES] = (double)snap->flagCaptures;
                                                progress_table[BG_ACHIEVEMENT_FLAG_ASSISTS]  = (double)snap->flagAssists;
                                                progress_table[BG_ACHIEVEMENT_FUEL]          = snap->fuelUsed;
                                                progress_table[BG_ACHIEVEMENT_ACCURACY]      = (double)snap->accuracyAwards;
                                                progress_table[BG_ACHIEVEMENT_EXCELLENT]     = (double)snap->excellentAwards;
                                                progress_table[BG_ACHIEVEMENT_IMPRESSIVE]    = (double)snap->impressiveAwards;
                                                progress_table[BG_ACHIEVEMENT_PERFECT]       = (double)snap->perfectAwards;

                                                for ( i = 0; i < BG_ACHIEVEMENT_CATEGORY_COUNT; ++i ) {
                                                        const bgAchievementCategoryDef_t *cat = BG_AchievementGetCategory( i );
                                                        snap->achievementTiers[i] = BG_AchievementUnlockedTiers( cat, progress_table[i] );
                                                }
                                        }
                                } else if ( fh ) {
                                        trap_FS_FCloseFile( fh );
                                }
                        }
                }
        }

        payload->playerCount++;
        return qtrue;
}

static void G_LadderSubmitMatchReport( const char *reason ) {
        ladderMatchPayload_t *payload = &s_ladderMatchPayload;
        qtime_t endTime;
        int endEpoch;
        char serverinfo[MAX_INFO_STRING];
        const char *value;
        char buffer[MAX_INFO_STRING];
        int i;
        qboolean hasRaceFields;
        qboolean hasEliminationSettings;
        qboolean isTeamMode;

        if ( trap_Cvar_VariableIntegerValue( "sv_ladderEnabled" ) == 0 ) {
                return;
        }

        // Flag whether this is a dedicated server or a local/offline game.
        // Both are allowed – the ladder displays them in separate lists.

        Com_Memset( payload, 0, sizeof( *payload ) );

        if ( level.ladderMatchId[0] ) {
                Q_strncpyz( payload->matchId, level.ladderMatchId, sizeof( payload->matchId ) );
        }

        payload->valid = qtrue;
        payload->gametype = g_gametype.integer;
        Q_strncpyz( payload->mode, G_LadderModeForGametype( g_gametype.integer ), sizeof( payload->mode ) );

        trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );
        value = Info_ValueForKey( serverinfo, "mapname" );
        if ( value && value[0] ) {
                Q_strncpyz( payload->mapName, value, sizeof( payload->mapName ) );
        }

        G_LadderFormatIsoTime( &level.ladderStartTime, payload->startTimeIso, sizeof( payload->startTimeIso ) );
        payload->startEpoch = level.ladderStartEpoch;

        endEpoch = trap_RealTime( &endTime );
        G_LadderFormatIsoTime( &endTime, payload->endTimeIso, sizeof( payload->endTimeIso ) );
        payload->endEpoch = endEpoch;

        if ( payload->startEpoch > 0 && payload->endEpoch >= payload->startEpoch ) {
                payload->durationSeconds = payload->endEpoch - payload->startEpoch;
        }
        Com_sprintf( payload->durationIso, sizeof( payload->durationIso ), "PT%iS", payload->durationSeconds );

        trap_Cvar_VariableStringBuffer( "sv_hostname", payload->serverName, sizeof( payload->serverName ) );
        payload->isDedicated = g_dedicated.integer ? qtrue : qfalse;
        trap_Cvar_VariableStringBuffer( "net_ip", payload->serverHost, sizeof( payload->serverHost ) );
        if ( payload->serverHost[0] ) {
                trap_Cvar_VariableStringBuffer( "net_port", buffer, sizeof( buffer ) );
                if ( buffer[0] ) {
                        Q_strcat( payload->serverHost, sizeof( payload->serverHost ), ":" );
                        Q_strcat( payload->serverHost, sizeof( payload->serverHost ), buffer );
                }
        } else {
                trap_Cvar_VariableStringBuffer( "sv_fqdn", payload->serverHost, sizeof( payload->serverHost ) );
                if ( !payload->serverHost[0] ) {
                        Q_strncpyz( payload->serverHost, payload->serverName, sizeof( payload->serverHost ) );
                }
        }
        trap_Cvar_VariableStringBuffer( "version", payload->serverBuild, sizeof( payload->serverBuild ) );

        payload->levelStartTime = level.startTime;
        payload->levelEndTime = level.time;
        hasRaceFields = G_LadderGametypeHasRaceFields( payload->gametype );
        hasEliminationSettings = G_LadderGametypeUsesEliminationSettings( payload->gametype );
        isTeamMode = G_LadderGametypeIsTeamMode( payload->gametype );

        if ( hasRaceFields ) {
                payload->raceStartTime = level.startRaceTime;
                payload->raceEndTime = level.finishRaceTime;
                payload->finishRaceTime = level.finishRaceTime;
                payload->numberOfLaps = level.numberOfLaps;
                payload->trackReversed = g_trackReversed.integer ? qtrue : qfalse;
        } else {
                if ( level.startRaceTime || level.finishRaceTime || level.numberOfLaps || g_trackReversed.integer ) {
                        Com_Printf( "Ladder: warning - race fields set in non-race mode %s; neutralizing payload fields\n",
                                payload->mode );
                }
                payload->raceStartTime = 0;
                payload->raceEndTime = 0;
                payload->finishRaceTime = 0;
                payload->numberOfLaps = 0;
                payload->trackReversed = qfalse;
        }

        payload->winnerClientNum = G_LadderWinnerForGametype( payload->gametype );
        if ( payload->winnerClientNum != level.winnerNumber ) {
                Com_Printf( "Ladder: info - winnerClientNum adjusted for mode %s (%d -> %d)\n",
                        payload->mode, level.winnerNumber, payload->winnerClientNum );
        }

        if ( hasEliminationSettings ) {
                payload->eliminationStartDelay = g_eliminationStartDelay.integer;
                payload->eliminationInterval = g_eliminationInterval.integer;
                payload->eliminationWarning = g_eliminationWarning.integer;
        } else {
                if ( g_eliminationStartDelay.integer || g_eliminationInterval.integer || g_eliminationWarning.integer ) {
                        Com_Printf( "Ladder: warning - elimination settings present in mode %s; emitting optional neutral values\n",
                                payload->mode );
                }
                payload->eliminationStartDelay = 0;
                payload->eliminationInterval = 0;
                payload->eliminationWarning = 0;
        }

        if ( isTeamMode ) {
                for ( i = 0; i < TEAM_NUM_TEAMS; ++i ) {
                        payload->teamScores[i] = level.teamScores[i];
                        payload->teamTimes[i] = level.teamTimes[i];
                }
        } else {
                for ( i = 0; i < TEAM_NUM_TEAMS; ++i ) {
                        if ( level.teamScores[i] || level.teamTimes[i] ) {
                                Com_Printf( "Ladder: warning - team fields populated in non-team mode %s; neutralizing payload fields\n",
                                        payload->mode );
                                break;
                        }
                }
                for ( i = 0; i < TEAM_NUM_TEAMS; ++i ) {
                        payload->teamScores[i] = 0;
                        payload->teamTimes[i] = 0;
                }
        }

        for ( i = 0; i < level.maxclients; ++i ) {
                G_LadderPopulatePlayer( payload, i );
        }

        if ( reason && reason[0] ) {
                Com_Printf( "Ladder: submitting '%s' with reason '%s' (%d players)\n",
                        payload->matchId[0] ? payload->matchId : "<unknown>", reason, payload->playerCount );
        } else {
                Com_Printf( "Ladder: submitting '%s' (%d players)\n",
                        payload->matchId[0] ? payload->matchId : "<unknown>", payload->playerCount );
        }

        if ( payload->playerCount > 0 || reason ) {
                trap_LadderSubmit( payload );
        }
}


/*
============
G_InitGame

============
*/
void G_InitGame( int levelTime, int randomSeed, int restart ) {
	int					i;

	G_Printf ("------- Game Initialization -------\n");
	G_Printf ("gamename: %s\n", GAMEVERSION);
	G_Printf ("gamedate: %s\n", PRODUCT_DATE);

	srand( randomSeed );

	G_RegisterCvars();

	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		if ( g_timeTrialLaps.integer > 0 ) {
			trap_Cvar_Set( "laplimit", va( "%i", g_timeTrialLaps.integer ) );
			trap_Cvar_Update( &g_laplimit );
		}

		if ( g_timeTrialFinishDelay.integer > 0 ) {
			trap_Cvar_Set( "g_finishRaceDelay", va( "%i", g_timeTrialFinishDelay.integer ) );
			trap_Cvar_Update( &g_finishRaceDelay );
		}
	}

	G_ProcessIPBans();

	G_InitMemory();

	// set some level globals
	memset( &level, 0, sizeof( level ) );
	level.raceState = RACE_STATE_NONE;
	level.raceIntroEndTime = 0;
	/* Preserve restart flag so the intro camera knows not to play again.
	   raceIntroFallback=qtrue suppresses the early-intro trigger in
	   RallyStarter_Think for all map_restart calls within a session. */
	if ( restart ) {
		level.raceIntroFallback = qtrue;
	}
        level.time = levelTime;
        level.startTime = levelTime;
        level.ladderStartEpoch = trap_RealTime( &level.ladderStartTime );
        G_LadderBuildMatchId( randomSeed );

	level.snd_fry = G_SoundIndex("sound/player/fry.ogg");	// FIXME standing in lava / slime

	if ( g_gametype.integer != GT_SINGLE_PLAYER && g_logfile.string[0] ) {
		if ( g_logfileSync.integer ) {
			trap_FS_FOpenFile( g_logfile.string, &level.logFile, FS_APPEND_SYNC );
		} else {
			trap_FS_FOpenFile( g_logfile.string, &level.logFile, FS_APPEND );
		}
		if ( !level.logFile ) {
			G_Printf( "WARNING: Couldn't open logfile: %s\n", g_logfile.string );
		} else {
			char	serverinfo[MAX_INFO_STRING];

			trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );

			G_LogPrintf("------------------------------------------------------------\n" );
			G_LogPrintf("InitGame: %s\n", serverinfo );
		}
	} else {
		G_Printf( "Not logging to disk.\n" );
	}

	G_InitWorldSession();

	{
		char mapname[MAX_QPATH];

		trap_Cvar_VariableStringBuffer( "mapname", mapname, sizeof( mapname ) );
		G_Ghost_InitForMap( mapname );
	}

	// initialize all entities for this game
	memset( g_entities, 0, MAX_GENTITIES * sizeof(g_entities[0]) );
	level.gentities = g_entities;

	// initialize all clients for this game
	level.maxclients = g_maxclients.integer;
	memset( g_clients, 0, MAX_CLIENTS * sizeof(g_clients[0]) );
	level.clients = g_clients;

	// set client fields on player ents
	for ( i=0 ; i<level.maxclients ; i++ ) {
		g_entities[i].client = level.clients + i;
	}

	// always leave room for the max number of clients,
	// even if they aren't all used, so numbers inside that
	// range are NEVER anything but clients
	level.num_entities = MAX_CLIENTS;

	for ( i=0 ; i<MAX_CLIENTS ; i++ ) {
		g_entities[i].classname = "clientslot";
	}

	// let the server system know where the entites are
	trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ), 
		&level.clients[0].ps, sizeof( level.clients[0] ) );

	// reserve some spots for dead player bodies
	InitBodyQue();

        ClearRegisteredItems();

        G_RallyApplySprintDefaults();

        // parse the key/value pairs and spawn gentities
        G_SpawnEntitiesFromString();

	// general initialization
	G_FindTeams();

	// make sure we have flags for CTF, etc
	if( g_gametype.integer >= GT_TEAM ) {
		G_CheckTeamItems();
	}

        SaveRegisteredItems();

        G_Profile_Init();

	G_Printf ("-----------------------------------\n");

	if( g_gametype.integer == GT_SINGLE_PLAYER || trap_Cvar_VariableIntegerValue( "com_buildScript" ) ) {
		G_ModelIndex( SP_PODIUM_MODEL );
	}

	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) ) {
		BotAISetup( restart );
		BotAILoadMap( restart );
		G_InitBots( restart );
	}

	G_RemapTeamShaders();

	trap_SetConfigstring( CS_INTERMISSION, "" );

// STONELANCE
/*
	{
		gentity_t	*ent;
		vec3_t		origin;

		VectorClear( origin );

		ent = G_Spawn();
		G_SetOrigin( ent, origin );
		level.testModelID = ent->s.number;
		ent->classname = "testmodel";

		ent->r.svFlags = SVF_BROADCAST;
		ent->s.eType = ET_GENERAL;
		ent->s.modelindex = G_ModelIndex( "models/test/sphere01.md3" );
		trap_LinkEntity( ent );
	}
*/
// END
}



/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart ) {
	G_Printf ("==== ShutdownGame ====\n");

	if ( level.logFile ) {
		G_LogPrintf("ShutdownGame:\n" );
		G_LogPrintf("------------------------------------------------------------\n" );
		trap_FS_FCloseFile( level.logFile );
		level.logFile = 0;
	}

        // write all the client session data so we can get it back
        G_WriteSessionData();

        G_Profile_Shutdown();

	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) ) {
		BotAIShutdown( restart );
	}
}



//===================================================================

void QDECL Com_Error ( int level, const char *error, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	Q_vsnprintf (text, sizeof(text), error, argptr);
	va_end (argptr);

	trap_Error( text );
}

void QDECL Com_Printf( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	Q_vsnprintf (text, sizeof(text), msg, argptr);
	va_end (argptr);

	trap_Print( text );
}

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=============
AddTournamentPlayer

If there are less than two tournament players, put a
spectator in the game and restart
=============
*/
void AddTournamentPlayer( void ) {
	int			i;
	gclient_t	*client;
	gclient_t	*nextInLine;

	if ( level.numPlayingClients >= 2 ) {
		return;
	}

	// never change during intermission
	if ( level.intermissiontime ) {
		return;
	}

	nextInLine = NULL;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		client = &level.clients[i];
		if ( client->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
			continue;
		}
		// never select the dedicated follow or scoreboard clients
		if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD || 
			client->sess.spectatorClient < 0  ) {
			continue;
		}

		if(!nextInLine || client->sess.spectatorNum > nextInLine->sess.spectatorNum)
			nextInLine = client;
	}

	if ( !nextInLine ) {
		return;
	}

	level.warmupTime = -1;

	// set them to free-for-all team
	SetTeam( &g_entities[ nextInLine - level.clients ], "f" );
}

/*
=======================
AddTournamentQueue

Add client to end of tournament queue
=======================
*/

void AddTournamentQueue(gclient_t *client)
{
	int index;
	gclient_t *curclient;
	
	for(index = 0; index < level.maxclients; index++)
	{
		curclient = &level.clients[index];
		
		if(curclient->pers.connected != CON_DISCONNECTED)
		{
			if(curclient == client)
				curclient->sess.spectatorNum = 0;
			else if(curclient->sess.sessionTeam == TEAM_SPECTATOR)
				curclient->sess.spectatorNum++;
		}
	}
}

/*
=======================
RemoveTournamentLoser

Make the loser a spectator at the back of the line
=======================
*/
void RemoveTournamentLoser( void ) {
	int			clientNum;

	if ( level.numPlayingClients != 2 ) {
		return;
	}

	clientNum = level.sortedClients[1];

	if ( level.clients[ clientNum ].pers.connected != CON_CONNECTED ) {
		return;
	}

	// make them a spectator
	SetTeam( &g_entities[ clientNum ], "s" );
}

/*
=======================
RemoveTournamentWinner
=======================
*/
void RemoveTournamentWinner( void ) {
	int			clientNum;

	if ( level.numPlayingClients != 2 ) {
		return;
	}

	clientNum = level.sortedClients[0];

	if ( level.clients[ clientNum ].pers.connected != CON_CONNECTED ) {
		return;
	}

	// make them a spectator
	SetTeam( &g_entities[ clientNum ], "s" );
}

static void G_Profile_RecordMatchAchievements( gclient_t *client ) {
        int accuracy;

        if ( !client ) {
                return;
        }

        if ( client->accuracy_shots > 0 ) {
                accuracy = ( client->accuracy_hits * 100 ) / client->accuracy_shots;
                G_Profile_RecordAccuracy( client, accuracy );
        }

        if ( isRallyNonDMRace() ) {
                return;
        }

        if ( client->ps.persistant[PERS_KILLED] <= 0 ) {
                G_Profile_RecordPerfect( client );
        }
}

/*
=======================
G_RecordMatchOutcome
=======================
*/
static void G_RecordMatchOutcome( void ) {
        int                     clientNum;
        int                     winner;
        qboolean                teamGame;

        if ( level.numPlayingClients <= 0 ) {
                return;
        }

        teamGame = ( g_gametype.integer >= GT_TEAM );

        if ( teamGame ) {
                int winningTeam = GetTeamAtRank( 1 );

                if ( winningTeam < TEAM_RED || winningTeam >= TEAM_RED + 4 ) {
                        return;
                }

                for ( clientNum = 0; clientNum < level.maxclients; ++clientNum ) {
                        gclient_t *client = &level.clients[ clientNum ];

                        if ( client->pers.connected != CON_CONNECTED ) {
                                continue;
                        }

                        if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
                                continue;
                        }

                        if ( client->sess.sessionTeam == winningTeam ) {
                                client->sess.wins++;
                                G_Profile_RecordWin( client );
                        } else {
                                client->sess.losses++;
                                G_Profile_RecordLoss( client );
                        }

                        G_Profile_RecordMatchAchievements( client );

                        ClientUserinfoChanged( clientNum );
                }

                return;
        }

        winner = -1;
        for ( clientNum = 0; clientNum < level.numConnectedClients; ++clientNum ) {
                int sortedClientNum = level.sortedClients[ clientNum ];
                gclient_t *client = &level.clients[ sortedClientNum ];

                if ( client->pers.connected != CON_CONNECTED ) {
                        continue;
                }

                if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
                        continue;
                }

                winner = sortedClientNum;
                break;
        }

        if ( winner < 0 ) {
                return;
        }

        for ( clientNum = 0; clientNum < level.maxclients; ++clientNum ) {
                gclient_t *client = &level.clients[ clientNum ];

                if ( client->pers.connected != CON_CONNECTED ) {
                        continue;
                }

                if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
                        continue;
                }

                if ( clientNum == winner ) {
                        client->sess.wins++;
                        G_Profile_RecordWin( client );
                } else {
                        client->sess.losses++;
                        G_Profile_RecordLoss( client );
                }

                /* Modi-spezifische Lifetime-Stats */
                {
                        int position = client->ps.persistant[PERS_RANK] + 1; /* 1-basiert */
                        G_Profile_RecordRacePlacement( client, position );

                        if ( client->finishRaceTime > level.startRaceTime ) {
                                int totalMs = client->finishRaceTime - level.startRaceTime;
                                G_Profile_RecordRaceTime( client, totalMs );
                                G_Profile_RecordSprintTime( client, totalMs );
                                G_Profile_RecordSurvivalTime( client, totalMs );
                        } else if ( level.finishRaceTime > level.startRaceTime ) {
                                G_Profile_RecordSurvivalTime( client,
                                        level.finishRaceTime - level.startRaceTime );
                        }

                        G_Profile_RecordEliminationRound( client, client->eliminationRound );
                }

                G_Profile_RecordMatchAchievements( client );

                ClientUserinfoChanged( clientNum );
        }
}

/*
=============
SortRanks

=============
*/
int QDECL SortRanks( const void *a, const void *b ) {
	gclient_t	*ca, *cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// sort special clients last
	if ( ca->sess.spectatorState == SPECTATOR_SCOREBOARD || ca->sess.spectatorClient < 0 ) {
		return 1;
	}
	if ( cb->sess.spectatorState == SPECTATOR_SCOREBOARD || cb->sess.spectatorClient < 0  ) {
		return -1;
	}

	// then connecting clients
	if ( ca->pers.connected == CON_CONNECTING ) {
		return 1;
	}
	if ( cb->pers.connected == CON_CONNECTING ) {
		return -1;
	}


	// then spectators
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		if ( ca->sess.spectatorNum > cb->sess.spectatorNum ) {
			return -1;
		}
		if ( ca->sess.spectatorNum < cb->sess.spectatorNum ) {
			return 1;
		}
		return 0;
	}
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR ) {
		return 1;
	}
	if ( cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		return -1;
	}


// STONELANCE
/*
	// then sort by score
	if ( ca->ps.persistant[PERS_SCORE]
		> cb->ps.persistant[PERS_SCORE] ) {
		return -1;
	}
	if ( ca->ps.persistant[PERS_SCORE]
		< cb->ps.persistant[PERS_SCORE] ) {
		return 1;
	}
*/

	// sort by position if race
	if (isRallyRace()){
		if ( ca->ps.stats[STAT_POSITION] < cb->ps.stats[STAT_POSITION] ) {
			return -1;
		}
		if ( ca->ps.stats[STAT_POSITION] > cb->ps.stats[STAT_POSITION] ) {
			return 1;
		}
	}
	// sort by finish time if derby
	else if (g_gametype.integer == GT_DERBY || g_gametype.integer == GT_LCS ){
		if ( ca->finishRaceTime && cb->finishRaceTime ) {
			if (ca->finishRaceTime < cb->finishRaceTime)
				return 1;
			else if (ca->finishRaceTime > cb->finishRaceTime)
				return -1;
		}
		else if ( ca->finishRaceTime ) {
			return 1;
		}
		else if ( cb->finishRaceTime ) {
			return -1;
		}
		else {
			// if still alive sort by health
			if ( ca->ps.stats[STAT_HEALTH] > cb->ps.stats[STAT_HEALTH] ) {
				return -1;
			}
			if ( ca->ps.stats[STAT_HEALTH] < cb->ps.stats[STAT_HEALTH] ) {
				return 1;
			}
		}
	}
	// then sort by score
	else {
		if ( ca->ps.persistant[PERS_SCORE]
			> cb->ps.persistant[PERS_SCORE] ) {
			return -1;
		}
		if ( ca->ps.persistant[PERS_SCORE]
			< cb->ps.persistant[PERS_SCORE] ) {
			return 1;
		}
	}
// END

	return 0;
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void ) {
	int		i;
	int		rank;
	int		score;
	int		newScore;
	gclient_t	*cl;

	level.follow1 = -1;
	level.follow2 = -1;
	level.numConnectedClients = 0;
	level.numNonSpectatorClients = 0;
	level.numPlayingClients = 0;
	level.numVotingClients = 0;		// don't count bots

	for (i = 0; i < ARRAY_LEN(level.numteamVotingClients); i++)
		level.numteamVotingClients[i] = 0;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_DISCONNECTED ) {
			level.sortedClients[level.numConnectedClients] = i;
			level.numConnectedClients++;

			if ( level.clients[i].sess.sessionTeam != TEAM_SPECTATOR ) {
				level.numNonSpectatorClients++;
			
				// decide if this should be auto-followed
				if ( level.clients[i].pers.connected == CON_CONNECTED ) {
					level.numPlayingClients++;
					if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
						level.numVotingClients++;
						if ( level.clients[i].sess.sessionTeam == TEAM_RED )
							level.numteamVotingClients[0]++;
						else if ( level.clients[i].sess.sessionTeam == TEAM_BLUE )
							level.numteamVotingClients[1]++;
					}
					if ( level.follow1 == -1 ) {
						level.follow1 = i;
					} else if ( level.follow2 == -1 ) {
						level.follow2 = i;
					}
				}
			}
		}
	}

	qsort( level.sortedClients, level.numConnectedClients, 
		sizeof(level.sortedClients[0]), SortRanks );

	// set the rank value for all clients that are connected and not spectators
// STONELANCE - FIXME: this stuff needed anymore? (I think its for tournament stuff)
	if ( g_gametype.integer >= GT_TEAM ) {
		// in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
		for ( i = 0;  i < level.numConnectedClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			if ( level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 2;
			} else if ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 0;
			} else {
				cl->ps.persistant[PERS_RANK] = 1;
			}
		}
	} else {	
		rank = -1;
		score = 0;
		for ( i = 0;  i < level.numPlayingClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			if ( isRallyRace() ) {
				/* In rally races every player has the same PERS_SCORE (0),
				   so use STAT_POSITION to detect ties instead. Positions
				   are unique per player, so ties are extremely rare and
				   only occur when two players genuinely share a position. */
				newScore = cl->ps.stats[STAT_POSITION];
			} else {
				newScore = cl->ps.persistant[PERS_SCORE];
			}
			if ( i == 0 || newScore != score ) {
				rank = i;
				// assume we aren't tied until the next client is checked
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank;
			} else {
				// we are tied with the previous client
				level.clients[ level.sortedClients[i-1] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
			score = newScore;
			if ( g_gametype.integer == GT_SINGLE_PLAYER && level.numPlayingClients == 1 ) {
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
		}
	}

	// set the CS_SCORES1/2 configstrings, which will be visible to everyone
	if ( g_gametype.integer >= GT_TEAM ) {
		trap_SetConfigstring( CS_SCORES1, va("%i", level.teamScores[TEAM_RED] ) );
		trap_SetConfigstring( CS_SCORES2, va("%i", level.teamScores[TEAM_BLUE] ) );
// STONELANCE
		trap_SetConfigstring( CS_SCORES3, va("%i", level.teamScores[TEAM_GREEN] ) );
		trap_SetConfigstring( CS_SCORES4, va("%i", level.teamScores[TEAM_YELLOW] ) );
// END
	} else {
		if ( level.numConnectedClients == 0 ) {
			trap_SetConfigstring( CS_SCORES1, va("%i", SCORE_NOT_PRESENT) );
			trap_SetConfigstring( CS_SCORES2, va("%i", SCORE_NOT_PRESENT) );
		} else if ( level.numConnectedClients == 1 ) {
			trap_SetConfigstring( CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va("%i", SCORE_NOT_PRESENT) );
		} else {
			trap_SetConfigstring( CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va("%i", level.clients[ level.sortedClients[1] ].ps.persistant[PERS_SCORE] ) );
		}
	}

	// see if it is time to end the level
	CheckExitRules();

	// if we are at the intermission, send the new info to everyone
	if ( level.intermissiontime ) {
		SendScoreboardMessageToAllClients();
	}

	 if ( g_humanplayers.integer != level.numVotingClients ) {
            trap_Cvar_Set( "g_humanplayers", va( "%i", level.numVotingClients ) );
	}
}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DeathmatchScoreboardMessage( g_entities + i );
		}
	}
}

/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent ) {
	// take out of follow mode if needed
// STONELANCE
//	if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
	if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ||
		 ent->client->sess.spectatorState == SPECTATOR_OBSERVE ) {
// END
		StopFollowing( ent );
	}

	FindIntermissionPoint();
	// move to the spot
	VectorCopy( level.intermission_origin, ent->s.origin );
	VectorCopy( level.intermission_origin, ent->client->ps.origin );
// STONELANCE (used for viewing angles client side)
//	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	VectorCopy (level.intermission_angle, ent->client->ps.damageAngles);
	VectorCopy( level.intermission_angle, ent->s.angles2 ); // FIXME: need  this?
	ent->client->ps.damagePitch = ANGLE2BYTE(ent->client->ps.damageAngles[PITCH]);
	ent->client->ps.damageYaw = ANGLE2BYTE(ent->client->ps.damageAngles[YAW]);
// END
	ent->client->ps.pm_type = PM_INTERMISSION;

	// clean up powerup info
	memset( ent->client->ps.powerups, 0, sizeof(ent->client->ps.powerups) );

	ent->client->ps.eFlags = 0;
	ent->s.eFlags = 0;
	ent->s.eType = ET_GENERAL;
	ent->s.modelindex = 0;
	ent->s.loopSound = 0;
	ent->s.event = 0;
	ent->r.contents = 0;
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint( void ) {
	gentity_t	*ent, *target;
	vec3_t		dir;

	// find the intermission spot
	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	if ( !ent ) {	// the map creator forgot to put in an intermission point...
		SelectSpawnPoint ( vec3_origin, level.intermission_origin, level.intermission_angle, qfalse );
	} else {
		VectorCopy (ent->s.origin, level.intermission_origin);
		VectorCopy (ent->s.angles, level.intermission_angle);
		// if it has a target, look towards it
		if ( ent->target ) {
			target = G_PickTarget( ent->target );
			if ( target ) {
				VectorSubtract( target->s.origin, level.intermission_origin, dir );
				vectoangles( dir, level.intermission_angle );
			}
		}
	}

}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void ) {
	int			i;
	gentity_t	*client;

	if ( level.intermissiontime ) {
		return;		// already active
	}

        // update wins / losses for the completed match
        G_RecordMatchOutcome();

        level.intermissiontime = level.time;
	// move all clients to the intermission point
	for (i=0 ; i< level.maxclients ; i++) {
		client = g_entities + i;
		if (!client->inuse)
			continue;
		// respawn if dead
		if (client->health <= 0) {
			ClientRespawn(client);
		}
		MoveClientToIntermission( client );
	}
#ifdef MISSIONPACK
	if (g_singlePlayer.integer) {
		trap_Cvar_Set("ui_singlePlayerActive", "0");
		UpdateTournamentInfo();
	}
#else
	// if single player game
	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		UpdateTournamentInfo();
		SpawnModelsOnVictoryPads();
	}
#endif
	// send the current scoring to all clients
	SendScoreboardMessageToAllClients();

// Q3Rally Code Start
	Team_EndGame();
// Q3Rally Code END
}


/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar 

=============
*/
void ExitLevel (void) {
	int		i;
	gclient_t *cl;
	char nextmap[MAX_STRING_CHARS];
	char d1[MAX_STRING_CHARS];

	//bot interbreeding
	BotInterbreedEndMatch();

	// if we are running a tournement map, kick the loser to spectator status,
	// which will automatically grab the next spectator and restart
// STONELANCE - removed gametype
/*
	if ( g_gametype.integer == GT_TOURNAMENT  ) {
		if ( !level.restarted ) {
			RemoveTournamentLoser();
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			level.changemap = NULL;
			level.intermissiontime = 0;
		}
		return;	
	}
*/
// END

	trap_Cvar_VariableStringBuffer( "nextmap", nextmap, sizeof(nextmap) );
	trap_Cvar_VariableStringBuffer( "d1", d1, sizeof(d1) );

	if( !Q_stricmp( nextmap, "map_restart 0" ) && Q_stricmp( d1, "" ) ) {
		trap_Cvar_Set( "nextmap", "vstr d2" );
		trap_SendConsoleCommand( EXEC_APPEND, "vstr d1\n" );
	} else {
		trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
	}

	level.changemap = NULL;
	level.intermissiontime = 0;

	// reset all the scores so we don't enter the intermission again
	level.teamScores[TEAM_RED] = 0;
	level.teamScores[TEAM_BLUE] = 0;
// STONELANCE
	level.teamScores[TEAM_GREEN] = 0;
	level.teamScores[TEAM_YELLOW] = 0;
// END
	level.teamTimes[TEAM_RED] = 0;
	level.teamTimes[TEAM_BLUE] = 0;
	level.teamTimes[TEAM_GREEN] = 0;
	level.teamTimes[TEAM_YELLOW] = 0;
	for ( i=0 ; i< g_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.persistant[PERS_SCORE] = 0;
	}

	// we need to do this here before changing to CON_CONNECTING
	G_WriteSessionData();

	// change all client states to connecting, so the early players into the
	// next level will know the others aren't done reconnecting
	for (i=0 ; i< g_maxclients.integer ; i++) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			level.clients[i].pers.connected = CON_CONNECTING;
		}
	}

}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
void QDECL G_LogPrintf( const char *fmt, ... ) {
	va_list		argptr;
	char		string[1024];
	int			min, tens, sec;

	sec = ( level.time - level.startTime ) / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf( string, sizeof(string), "%3i:%i%i ", min, tens, sec );

	va_start( argptr, fmt );
	Q_vsnprintf(string + 7, sizeof(string) - 7, fmt, argptr);
	va_end( argptr );

	if ( g_dedicated.integer ) {
		G_Printf( "%s", string + 7 );
	}

	if ( !level.logFile ) {
		return;
	}

	trap_FS_Write( string, strlen( string ), level.logFile );
}


// STONELANCE
/*
=================
G_DebugLogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
void QDECL G_DebugLogPrintf( const char *fmt, ... ) {
	va_list			argptr;
	char			string[1024];
	fileHandle_t	logFile;

	va_start( argptr, fmt );
	Q_vsnprintf (string, sizeof(string), fmt, argptr);
	va_end( argptr );

	if ( g_dedicated.integer ) {
		G_Printf( "%s", string + 7 );
	}

	trap_FS_FOpenFile( "q3r_game.log", &logFile, FS_APPEND );

	if ( !logFile ) {
		return;
	}

	trap_FS_Write( string, strlen( string ), logFile );

	trap_FS_FCloseFile( logFile );
}
// END

/*
==================================================
LogExit

Append information about this game to the log file
==================================================
*/
void LogExit( const char *string ) {
	int				i, numSorted;
	gclient_t		*cl;
#ifdef MISSIONPACK
	qboolean won = qtrue;
	team_t team = TEAM_RED;
#endif
	G_LogPrintf( "Exit: %s\n", string );

        level.intermissionQueued = level.time;

	// this will keep the clients from playing any voice sounds
	// that will get cut off when the queued intermission starts
	trap_SetConfigstring( CS_INTERMISSION, "1" );

	// don't send more than 32 scores (FIXME?)
	numSorted = level.numConnectedClients;
	if ( numSorted > 32 ) {
		numSorted = 32;
	}

	if ( g_gametype.integer >= GT_TEAM ) {
// STONELANCE
//		G_LogPrintf( "red:%i  blue:%i\n",
//			level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] );
		G_LogPrintf( "red:%i  blue:%i  green:%i  yellow:%i\n",
			level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE],
			level.teamScores[TEAM_GREEN], level.teamScores[TEAM_YELLOW]);
// END
	}

	for (i=0 ; i < numSorted ; i++) {
		int		ping;

		cl = &level.clients[level.sortedClients[i]];

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		if ( cl->pers.connected == CON_CONNECTING ) {
			continue;
		}

		ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

		G_LogPrintf( "score: %i  ping: %i  client: %i %s\n", cl->ps.persistant[PERS_SCORE], ping, level.sortedClients[i],	cl->pers.netname );
#ifdef MISSIONPACK
		if (g_singlePlayer.integer && !(g_entities[cl - level.clients].r.svFlags & SVF_BOT)) {
			team = cl->sess.sessionTeam;
		}
		if (g_singlePlayer.integer && g_gametype.integer < GT_TEAM) {
			if (g_entities[cl - level.clients].r.svFlags & SVF_BOT && cl->ps.persistant[PERS_RANK] == 0) {
				won = qfalse;
			}
		}
#endif

        }

	G_Profile_FlushIfDirty();

        if ( G_Profile_IsDirty() ) {
                G_Profile_FlushIfDirty();
        }

	G_LadderSubmitMatchReport( string );

#ifdef MISSIONPACK
        if (g_singlePlayer.integer) {
                if (g_gametype.integer >= GT_TEAM) {
			if (team == TEAM_BLUE) {
				won = level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED];
			} else {
				won = level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE];
			}
		}
		trap_SendConsoleCommand( EXEC_APPEND, (won) ? "spWin\n" : "spLose\n" );
	}
#endif


}


/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit( void ) {
	int			ready, notReady, playerCount;
	int			i;
	gclient_t	*cl;
	int			readyMask;

	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		return;
	}

	// see which players are ready
	ready = 0;
	notReady = 0;
	readyMask = 0;
	playerCount = 0;
	for (i=0 ; i< g_maxclients.integer ; i++) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( g_entities[i].r.svFlags & SVF_BOT ) {
			continue;
		}

		playerCount++;
		if ( cl->readyToExit ) {
			ready++;
			if ( i < 16 ) {
				readyMask |= 1 << i;
			}
		} else {
			notReady++;
		}
	}

	// copy the readyMask to each player's stats so
	// it can be displayed on the scoreboard
	for (i=0 ; i< g_maxclients.integer ; i++) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.stats[STAT_CLIENTS_READY] = readyMask;
	}

	// never exit in less than five seconds
	if ( level.time < level.intermissiontime + 5000 ) {
		return;
	}

	// only test ready status when there are real players present
	if ( playerCount > 0 ) {
		// if nobody wants to go, clear timer
		if ( !ready ) {
			level.readyToExit = qfalse;
			return;
		}

		// if everyone wants to go, go now
		if ( !notReady ) {
			ExitLevel();
			return;
		}
	}

	// the first person to ready starts the ten second timeout
	if ( !level.readyToExit ) {
		level.readyToExit = qtrue;
		level.exitTime = level.time;
	}

	// if we have waited ten seconds since at least one player
	// wanted to exit, go ahead
	if ( level.time < level.exitTime + 10000 ) {
		return;
	}

	ExitLevel();
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void ) {
	int		a, b;

	if ( level.numPlayingClients < 2 ) {
		return qfalse;
	}
	
// STONELANCE
	// races or rallys can never be tied, otherwise then the level will not end
	if ( g_gametype.integer == GT_DERBY || g_gametype.integer == GT_LCS || isRallyRace() )
	{
/*
		a = level.clients[level.sortedClients[0]].finishRaceTime;
		b = level.clients[level.sortedClients[1]].finishRaceTime;
		return a == b;
*/
		return qfalse;
	}
// END

	if ( g_gametype.integer >= GT_TEAM ) {
// STONELANCE
//		return level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE];

		int			i, winner;
		qboolean	tied;

		tied = qfalse;
		winner = GetTeamAtRank(1) - TEAM_RED;
		for (i = 0; i < 4; i++){
			if (i == winner) continue;
			if (!TeamCount(-1, TEAM_RED + i)) continue;

			if ((isRallyRace()
					// Only a tie if both teams have actual finish times.
					// Two zeros just mean nobody has finished yet.
					&& level.teamTimes[winner + TEAM_RED] != 0
					&& level.teamTimes[winner + TEAM_RED] == level.teamTimes[i + TEAM_RED])
				|| (!isRallyRace() && level.teamScores[winner + TEAM_RED] == level.teamScores[i + TEAM_RED])){
				tied = qtrue;
				break;
			}
		}

		return tied;
// END
	}

	a = level.clients[level.sortedClients[0]].ps.persistant[PERS_SCORE];
	b = level.clients[level.sortedClients[1]].ps.persistant[PERS_SCORE];

	return a == b;
}

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
void CheckExitRules( void ) {
 	int			i;
	gclient_t	*cl;
// STONELANCE
	int			count;
// END
	// if at the intermission, wait for all non-bots to
	// signal ready, then go to next level
	if ( level.intermissiontime ) {
		CheckIntermissionExit ();
		return;
	}

	if ( level.intermissionQueued ) {
#ifdef MISSIONPACK
		int time = (g_singlePlayer.integer) ? SP_INTERMISSION_DELAY_TIME : INTERMISSION_DELAY_TIME;
		if ( level.time - level.intermissionQueued >= time ) {
			level.intermissionQueued = 0;
			BeginIntermission();
		}
#else
		if ( level.time - level.intermissionQueued >= INTERMISSION_DELAY_TIME ) {
			level.intermissionQueued = 0;
			BeginIntermission();
		}
#endif
		return;
	}

	// check for sudden death
	if ( ScoreIsTied() ) {
		// always wait for sudden death
		return;
	}

	if ( g_timelimit.integer < 0 || g_timelimit.integer > INT_MAX / 60000 ) {
		G_Printf( "timelimit %i is out of range, defaulting to 0\n", g_timelimit.integer );
		trap_Cvar_Set( "timelimit", "0" );
		trap_Cvar_Update( &g_timelimit );
	}

	if ( g_timelimit.integer && !level.warmupTime ) {
		if ( level.time - level.startTime >= g_timelimit.integer*60000 ) {
			if ( g_gametype.integer == GT_KOTH ) {
				if ( KOTH_HandleOvertime() ) {
					trap_SendServerCommand( -1, "print \"KOTH overtime finished.\n\"" );
					LogExit( "KOTH overtime finished." );
				}
				return;
			}
			trap_SendServerCommand( -1, "print \"Timelimit hit.\n\"");
			LogExit( "Timelimit hit." );
			return;
		}
	}

// STONELANCE
	if (g_gametype.integer == GT_DERBY && level.startRaceTime && !level.finishRaceTime) {
		gclient_t	*winner = NULL;

		for ( i=0, count = 0 ; i< g_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) continue;
			if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) continue;
			if ( isRaceObserver( cl->ps.clientNum ) ) continue;
			if ( cl->ps.stats[STAT_HEALTH] <= 0 ) continue;

			count++;
			winner = cl;
		}

		// Only declare a winner when more than one player participated.
		// A lone player who started with no opponents never gets a free win.
		if (winner && count == 1 && level.derbyStartPlayerCount >= g_derbyMinPlayers.integer) {
			level.winnerNumber = winner->ps.clientNum;
			level.finishRaceTime = level.time;

			trap_SendServerCommand( -1, va("print \"%s won the demolition derby!\n\"", winner->pers.netname ));
			trap_SendServerCommand( level.winnerNumber, "cp \"You won the demolition derby!\"");
		}

		return;
	}

	if (g_gametype.integer == GT_LCS && level.startRaceTime && !level.finishRaceTime) {
		gclient_t	*winner = NULL;

		for ( i=0, count = 0 ; i< g_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) continue;
			if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) continue;
			if ( isRaceObserver( cl->ps.clientNum ) ) continue;
			if ( cl->ps.stats[STAT_HEALTH] <= 0 ) continue;

			count++;
			winner = cl;
		}

		if (winner && count == 1 && level.derbyStartPlayerCount >= g_derbyMinPlayers.integer) {
			level.winnerNumber = winner->ps.clientNum;
			level.finishRaceTime = level.time;

			trap_SendServerCommand( -1, va("print \"%s won the last car standing!\n\"", winner->pers.netname ));
			trap_SendServerCommand( level.winnerNumber, "cp \"You won the last car standing!\"");
		}

		return;
	}


	for ( i = 0, count = 0 ; i< g_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) continue;
		if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) continue;
		if ( isRaceObserver( cl->ps.clientNum ) ) continue;
//		if ( cl->ps.stats[STAT_HEALTH] <= 0 ) continue;

		count++;
		break;
	}

	// if its a race and the race has started if no players left playing 
	//			or everyone is a spectator then end the race.
	if ( (isRallyRace() || g_gametype.integer == GT_SINGLE_PLAYER) && level.startRaceTime && !count ){
		LogExit( "Race finished." );
		return;
	}

	if ( level.finishRaceTime && (isRallyRace() || g_gametype.integer == GT_SINGLE_PLAYER) ){
		// if everyone has finished the race, or the finishRaceDelay time is up, then exit
		if ( (level.finishRaceTime + (g_finishRaceDelay.integer * 1000) < level.time)
			|| !count ) {
			LogExit( "Race finished." );
			return;
		}
	}

	if ( level.finishRaceTime && g_gametype.integer == GT_DERBY
		&& level.finishRaceTime + 10000 < level.time ){
		g_entities[ level.winnerNumber ].client->finishRaceTime = level.time;
		trap_SendServerCommand( -1, va("raceFinishTime %i %i", level.winnerNumber, level.time) );
		LogExit( "Derby finished." );
		return;
	}
	
	if ( level.finishRaceTime && g_gametype.integer == GT_LCS
		&& level.finishRaceTime + 10000 < level.time ){
		g_entities[ level.winnerNumber ].client->finishRaceTime = level.time;
		trap_SendServerCommand( -1, va("raceFinishTime %i %i", level.winnerNumber, level.time) );
		LogExit( "Last car standing finished." );
		return;
	}
// END

// STONELANCE
	// dont check frags or captures during a race or derby
	if ( isRallyRace() || g_gametype.integer == GT_DERBY || g_gametype.integer == GT_LCS || g_gametype.integer == GT_SINGLE_PLAYER ){
		return;
	}
// END

	if ( g_fraglimit.integer < 0 ) {
		G_Printf( "fraglimit %i is out of range, defaulting to 0\n", g_fraglimit.integer );
		trap_Cvar_Set( "fraglimit", "0" );
		trap_Cvar_Update( &g_fraglimit );
	}

	if ( g_gametype.integer < GT_CTF && g_fraglimit.integer ) {
		if ( level.teamScores[TEAM_RED] >= g_fraglimit.integer ) {
			trap_SendServerCommand( -1, "print \"Red hit the fraglimit.\n\"" );
			LogExit( "Fraglimit hit." );
			return;
		}

		if ( level.teamScores[TEAM_BLUE] >= g_fraglimit.integer ) {
			trap_SendServerCommand( -1, "print \"Blue hit the fraglimit.\n\"" );
			LogExit( "Fraglimit hit." );
			return;
		}

// STONELANCE
		if ( level.teamScores[TEAM_GREEN] >= g_fraglimit.integer ) {
			trap_SendServerCommand( -1, "print \"Green hit the fraglimit.\n\"" );
			LogExit( "Fraglimit hit." );
			return;
		}

		if ( level.teamScores[TEAM_YELLOW] >= g_fraglimit.integer ) {
			trap_SendServerCommand( -1, "print \"Yellow hit the fraglimit.\n\"" );
			LogExit( "Fraglimit hit." );
			return;
		}
// END

		for ( i=0 ; i< g_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}
			if ( cl->sess.sessionTeam != TEAM_FREE ) {
				continue;
			}

			if ( cl->ps.persistant[PERS_SCORE] >= g_fraglimit.integer ) {
				LogExit( "Fraglimit hit." );
				trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " hit the fraglimit.\n\"",
					cl->pers.netname ) );
				return;
			}
		}
	}

	if ( g_capturelimit.integer < 0 ) {
		G_Printf( "capturelimit %i is out of range, defaulting to 0\n", g_capturelimit.integer );
		trap_Cvar_Set( "capturelimit", "0" );
		trap_Cvar_Update( &g_capturelimit );
	}

	if ( g_gametype.integer >= GT_CTF && g_gametype.integer != GT_KOTH && g_capturelimit.integer ) {

		if ( level.teamScores[TEAM_RED] >= g_capturelimit.integer ) {
			trap_SendServerCommand( -1, "print \"Red hit the capturelimit.\n\"" );
			LogExit( "Capturelimit hit." );
			return;
		}

		if ( level.teamScores[TEAM_BLUE] >= g_capturelimit.integer ) {
			trap_SendServerCommand( -1, "print \"Blue hit the capturelimit.\n\"" );
			LogExit( "Capturelimit hit." );
			return;
		}
	}

	// Q3Rally Code Start - KOTH win condition
	if ( g_gametype.integer == GT_KOTH && g_kothScoreWin.integer > 0 ) {
		if ( level.teamScores[TEAM_RED] >= g_kothScoreWin.integer ) {
			trap_SendServerCommand( -1, "print \"Red team hit the KOTH score limit.\n\"" );
			LogExit( "KOTH score limit hit by Red." );
			return;
		}
		if ( level.teamScores[TEAM_BLUE] >= g_kothScoreWin.integer ) {
			trap_SendServerCommand( -1, "print \"Blue team hit the KOTH score limit.\n\"" );
			LogExit( "KOTH score limit hit by Blue." );
			return;
		}
	}
	// Q3Rally Code END - KOTH
}



/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/


/*
=============
CheckTournament

Once a frame, check for changes in tournement player state
=============
*/
void CheckTournament( void ) {
	// check because we run 3 game frames before calling Connect and/or ClientBegin
	// for clients on a map_restart
	if ( level.numPlayingClients == 0 ) {
		return;
	}

// STONELANCE - removed gametype
/*
	if ( g_gametype.integer == GT_TOURNAMENT ) {

		// pull in a spectator if needed
		if ( level.numPlayingClients < 2 ) {
			AddTournamentPlayer();
		}

		// if we don't have two players, go back to "waiting for players"
		if ( level.numPlayingClients != 2 ) {
			if ( level.warmupTime != -1 ) {
				level.warmupTime = -1;
				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
				G_LogPrintf( "Warmup:\n" );
			}
			return;
		}

		if ( level.warmupTime == 0 ) {
			return;
		}

		// if the warmup is changed at the console, restart it
		if ( g_warmup.modificationCount != level.warmupModificationCount ) {
			level.warmupModificationCount = g_warmup.modificationCount;
			level.warmupTime = -1;
		}

		// if all players have arrived, start the countdown
		if ( level.warmupTime < 0 ) {
			if ( level.numPlayingClients == 2 ) {
				// fudge by -1 to account for extra delays
				if ( g_warmup.integer > 1 ) {
					level.warmupTime = level.time + ( g_warmup.integer - 1 ) * 1000;
				} else {
					level.warmupTime = 0;
				}

				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
			}
			return;
		}

		// if the warmup time has counted down, restart
		if ( level.time > level.warmupTime ) {
			level.warmupTime += 10000;
			trap_Cvar_Set( "g_restarted", "1" );
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			return;
		}
	} else
*/
// END
	if ( g_gametype.integer != GT_SINGLE_PLAYER && level.warmupTime != 0 ) {
		int		counts[TEAM_NUM_TEAMS];
		qboolean	notEnough = qfalse;

		level.raceState = RACE_STATE_NONE;
		level.raceIntroEndTime = 0;

		if ( g_gametype.integer >= GT_TEAM ) {
			counts[TEAM_BLUE] = TeamCount( -1, TEAM_BLUE );
			counts[TEAM_RED] = TeamCount( -1, TEAM_RED );

			if (counts[TEAM_RED] < 1 || counts[TEAM_BLUE] < 1) {
				notEnough = qtrue;
			}
		} else if ( level.numPlayingClients < 2 ) {
			notEnough = qtrue;
		}

		if ( notEnough ) {
			if ( level.warmupTime != -1 ) {
				level.warmupTime = -1;
				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
				G_LogPrintf( "Warmup:\n" );
			}
			return; // still waiting for team members
		}

		if ( level.warmupTime == 0 ) {
			return;
		}

		// if the warmup is changed at the console, restart it
		if ( g_warmup.modificationCount != level.warmupModificationCount ) {
			level.warmupModificationCount = g_warmup.modificationCount;
			level.warmupTime = -1;
		}

		// if all players have arrived, start the countdown
		if ( level.warmupTime < 0 ) {
			// fudge by -1 to account for extra delays
			if ( g_warmup.integer > 1 ) {
				level.warmupTime = level.time + ( g_warmup.integer - 1 ) * 1000;
			} else {
				level.warmupTime = 0;
			}

			trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
			return;
		}

		// if the warmup time has counted down, start the game
		if ( level.time > level.warmupTime ) {
			// Q3Rally Fix: KOTH does not need map_restart - just clear warmupTime
			if ( g_gametype.integer == GT_KOTH ) {
				level.warmupTime = 0;
				trap_SetConfigstring( CS_WARMUP, "0" );
				G_LogPrintf( "KOTH game started.\n" );
			} else {
				level.warmupTime += 10000;
				trap_Cvar_Set( "g_restarted", "1" );
				trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
				level.restarted = qtrue;
			}
			return;
		}
	}
}


/*
==================
CheckVote
==================
*/
void CheckVote( void ) {
	if ( level.voteExecuteTime && level.voteExecuteTime < level.time ) {
		level.voteExecuteTime = 0;
		trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.voteString ) );
	}
	if ( !level.voteTime ) {
		return;
	}
	if ( level.time - level.voteTime >= VOTE_TIME ) {
		trap_SendServerCommand( -1, "print \"Vote failed.\n\"" );
	} else {
		// ATVI Q3 1.32 Patch #9, WNF
		if ( level.voteYes > level.numVotingClients/2 ) {
			// execute the command, then remove the vote
			trap_SendServerCommand( -1, "print \"Vote passed.\n\"" );
			level.voteExecuteTime = level.time + 3000;
		} else if ( level.voteNo >= level.numVotingClients/2 ) {
			// same behavior as a timeout
			trap_SendServerCommand( -1, "print \"Vote failed.\n\"" );
		} else {
			// still waiting for a majority
			return;
		}
	}
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );

}

/*
==================
PrintTeam
==================
*/
void PrintTeam(int team, char *message) {
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		trap_SendServerCommand( i, message );
	}
}

/*
==================
SetLeader
==================
*/
void SetLeader(int team, int client) {
	int i;

	if ( level.clients[client].pers.connected == CON_DISCONNECTED ) {
		PrintTeam(team, va("print \"%s is not connected\n\"", level.clients[client].pers.netname) );
		return;
	}
	if (level.clients[client].sess.sessionTeam != team) {
		PrintTeam(team, va("print \"%s is not on the team anymore\n\"", level.clients[client].pers.netname) );
		return;
	}
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		if (level.clients[i].sess.teamLeader) {
			level.clients[i].sess.teamLeader = qfalse;
			ClientUserinfoChanged(i);
		}
	}
	level.clients[client].sess.teamLeader = qtrue;
	ClientUserinfoChanged( client );
	PrintTeam(team, va("print \"%s is the new team leader\n\"", level.clients[client].pers.netname) );
}

/*
==================
CheckTeamLeader
==================
*/
void CheckTeamLeader( int team ) {
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		if (level.clients[i].sess.teamLeader)
			break;
	}
	if (i >= level.maxclients) {
		for ( i = 0 ; i < level.maxclients ; i++ ) {
			if (level.clients[i].sess.sessionTeam != team)
				continue;
			if (!(g_entities[i].r.svFlags & SVF_BOT)) {
				level.clients[i].sess.teamLeader = qtrue;
				break;
			}
		}

		if (i >= level.maxclients) {
			for ( i = 0 ; i < level.maxclients ; i++ ) {
				if (level.clients[i].sess.sessionTeam != team)
					continue;
				level.clients[i].sess.teamLeader = qtrue;
				break;
			}
		}
	}
}

/*
==================
CheckTeamVote
==================
*/
void CheckTeamVote( int team ) {
	int cs_offset;

	if ( team == TEAM_RED )
		cs_offset = 0;
	else if ( team == TEAM_BLUE )
		cs_offset = 1;
	else
		return;

	if ( !level.teamVoteTime[cs_offset] ) {
		return;
	}
	if ( level.time - level.teamVoteTime[cs_offset] >= VOTE_TIME ) {
		trap_SendServerCommand( -1, "print \"Team vote failed.\n\"" );
	} else {
		if ( level.teamVoteYes[cs_offset] > level.numteamVotingClients[cs_offset]/2 ) {
			// execute the command, then remove the vote
			trap_SendServerCommand( -1, "print \"Team vote passed.\n\"" );
			//
			if ( !Q_strncmp( "leader", level.teamVoteString[cs_offset], 6) ) {
				//set the team leader
				SetLeader(team, atoi(level.teamVoteString[cs_offset] + 7));
			}
			else {
				trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.teamVoteString[cs_offset] ) );
			}
		} else if ( level.teamVoteNo[cs_offset] >= level.numteamVotingClients[cs_offset]/2 ) {
			// same behavior as a timeout
			trap_SendServerCommand( -1, "print \"Team vote failed.\n\"" );
		} else {
			// still waiting for a majority
			return;
		}
	}
	level.teamVoteTime[cs_offset] = 0;
	trap_SetConfigstring( CS_TEAMVOTE_TIME + cs_offset, "" );

}


/*
==================
CheckCvars
==================
*/
void CheckCvars( void ) {
	static int lastMod = -1;

	if ( g_password.modificationCount != lastMod ) {
		lastMod = g_password.modificationCount;
		if ( *g_password.string && Q_stricmp( g_password.string, "none" ) ) {
			trap_Cvar_Set( "g_needpass", "1" );
		} else {
			trap_Cvar_Set( "g_needpass", "0" );
		}
	}
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink (gentity_t *ent) {
	int	thinktime;

	thinktime = ent->nextthink;
	if (thinktime <= 0) {
		return;
	}
	if (thinktime > level.time) {
		return;
	}
	
	ent->nextthink = 0;
	if (!ent->think) {
		G_Error ( "NULL ent->think");
	}
	ent->think (ent);
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame( int levelTime ) {
	int			i;
	gentity_t	*ent;

	// if we are waiting for the level to restart, do nothing
	if ( level.restarted ) {
// STONELANCE
		level.raceState = RACE_STATE_NONE;
		level.raceIntroEndTime = 0;
		level.startRaceTime = 0;
		level.finishRaceTime = 0;
		level.winnerNumber = -1;
		for ( i = 0; i < level.maxclients; ++i ) {
			gclient_t *client = &level.clients[i];
			if ( client->pers.connected != CON_CONNECTED ) {
				continue;
			}
			G_ResetClientLapData( client );
		}
// END
		return;
	}

	level.framenum++;
	level.previousTime = level.time;
	level.time = levelTime;

	// get any cvar changes
	G_UpdateCvars();

// STONELANCE
//	RunRallyPhysics(); // map object physics

	if ( !level.startRaceTime ){ // UPDATE: should this be done even before the race starts?
		CalculatePlayerPositions();
	}
// END

	//
	// go through all allocated objects
	//
	ent = &g_entities[0];
	for (i=0 ; i<level.num_entities ; i++, ent++) {
		if ( !ent->inuse ) {
			continue;
		}

		// clear events that are too old
		if ( level.time - ent->eventTime > EVENT_VALID_MSEC ) {
			if ( ent->s.event ) {
				ent->s.event = 0;	// &= EV_EVENT_BITS;
				if ( ent->client ) {
					ent->client->ps.externalEvent = 0;
					// predicted events should never be set to zero
					//ent->client->ps.events[0] = 0;
					//ent->client->ps.events[1] = 0;
				}
			}
			if ( ent->freeAfterEvent ) {
				// tempEntities or dropped items completely go away after their event
				G_FreeEntity( ent );
				continue;
			} else if ( ent->unlinkAfterEvent ) {
				// items that will respawn will hide themselves after their pickup event
				ent->unlinkAfterEvent = qfalse;
				trap_UnlinkEntity( ent );
			}
		}

		// temporary entities don't think
		if ( ent->freeAfterEvent ) {
			continue;
		}

		if ( !ent->r.linked && ent->neverFree ) {
			continue;
		}

		if ( ent->s.eType == ET_MISSILE ) {
			G_RunMissile( ent );
			continue;
		}

		if ( ent->s.eType == ET_ITEM || ent->physicsObject ) {
			G_RunItem( ent );
			continue;
		}

		if ( ent->s.eType == ET_MOVER ) {
			G_RunMover( ent );
			continue;
		}

		if ( i < MAX_CLIENTS ) {
			G_RunClient( ent );
			continue;
		}

		G_RunThink( ent );
	}

	// perform final fixups on the players
	ent = &g_entities[0];
	for (i=0 ; i < level.maxclients ; i++, ent++ ) {
		if ( ent->inuse ) {
			ClientEndFrame( ent );
		}
	}

	// see if it is time to do a tournement restart
	CheckTournament();

	// see if it is time to end the level
	CheckExitRules();

	// update to team status?
	CheckTeamStatus();

	// Q3Rally Code Start - KOTH
	if ( g_gametype.integer == GT_KOTH ) {
		KOTH_Think();
	}
	// Q3Rally Code END - KOTH

	// cancel vote if timed out
	CheckVote();

	// check team votes
	CheckTeamVote( TEAM_RED );
	CheckTeamVote( TEAM_BLUE );

	// for tracking changes
	CheckCvars();

	if (g_listEntity.integer) {
		for (i = 0; i < MAX_GENTITIES; i++) {
			G_Printf("%4i: %s\n", i, g_entities[i].classname);
		}
		trap_Cvar_Set("g_listEntity", "0");
	}
}
