#include <assert.h>
#include <stdio.h>

#include "g_local.h"
#include "../botlib/botlib.h"
#include "../botlib/be_aas.h"
#include "../botlib/be_ea.h"
#include "../botlib/be_ai_char.h"
#include "../botlib/be_ai_chat.h"
#include "../botlib/be_ai_goal.h"
#include "../botlib/be_ai_move.h"
#include "../botlib/be_ai_weap.h"
#include "ai_main.h"
#include "ai_dmq3.h"
#include "chars.h"
#include "inv.h"
#include "ai_dm_enemyselect.h"

level_locals_t level;
gentity_t g_entities[MAX_GENTITIES];
int gametype;
float floattime;
vec3_t lastteleport_origin;
float lastteleport_time;

static aas_entityinfo_t entities[MAX_CLIENTS];
static playerState_t clients[MAX_CLIENTS];
static float visibility[MAX_CLIENTS];
static qboolean sameTeam[MAX_CLIENTS];
static float easyFragger;
static qboolean wantsRetreat;
static bot_state_t bot;

void QDECL Com_Error( int severity, const char *format, ... ) { assert( 0 ); }
void BotEntityInfo( int client, aas_entityinfo_t *info ) { *info = entities[client]; }
int BotAI_GetClientState( int client, playerState_t *state ) { *state = clients[client]; return qtrue; }
int BotSameTeam( bot_state_t *bs, int client ) { return sameTeam[client]; }
int BotTeam( bot_state_t *bs ) { return bs->cur_ps.persistant[PERS_TEAM]; }
int BotWantsToRetreat( bot_state_t *bs ) { return wantsRetreat; }
float trap_Characteristic_BFloat( int character, int index, float min, float max ) {
	return index == CHARACTERISTIC_EASY_FRAGGER ? easyFragger : 1.0f;
}
float BotEntityVisible( int viewer, vec3_t eye, vec3_t viewangles, float fov, int client ) {
	vec3_t dir, angles;
	VectorSubtract( entities[client].origin, eye, dir );
	vectoangles( dir, angles );
	return InFieldOfVision( viewangles, fov, angles ) ? visibility[client] : 0.0f;
}

#include "dm_enemy_selection_under_test.h"

static void reset( void ) {
	int i;
	memset( &bot, 0, sizeof( bot ) );
	memset( &level, 0, sizeof( level ) );
	memset( entities, 0, sizeof( entities ) );
	memset( clients, 0, sizeof( clients ) );
	memset( g_entities, 0, sizeof( g_entities ) );
	memset( sameTeam, 0, sizeof( sameTeam ) );
	for ( i = 0; i < MAX_CLIENTS; i++ ) visibility[i] = 1.0f;
	level.maxclients = 8;
	gametype = GT_DEATHMATCH;
	floattime = 100.0f;
	lastteleport_time = 0.0f;
	VectorClear( lastteleport_origin );
	easyFragger = 1.0f;
	wantsRetreat = qfalse;
	bot.enemy = -1;
	bot.lasthealth = bot.inventory[INVENTORY_HEALTH] = 100;
	bot.enemysight_time = 42.0f;
}

static void enemy( int client, float x, float y, float yaw, qboolean shooting ) {
	entities[client].number = client;
	entities[client].valid = qtrue;
	entities[client].flags = shooting ? EF_FIRING : 0;
	VectorSet( entities[client].origin, x, y, 0 );
	VectorSet( entities[client].angles, 0, yaw, 0 );
	clients[client].pm_type = PM_NORMAL;
}

static void test_range_beats_slot_order( void ) {
	reset();
	enemy( 1, 2000, 0, 180, qfalse );
	enemy( 2, 400, 0, 180, qfalse );
	assert( BotFindEnemy( &bot, -1 ) && bot.enemy == 2 );
	assert( bot.enemysight_time == floattime );
	assert( bot.enemyvisible_time == floattime );
	assert( bot.inventory[ENEMY_HORIZONTAL_DIST] == 400 );
	reset();
	enemy( 1, 400, 0, 180, qfalse );
	enemy( 2, 2000, 0, 180, qfalse );
	assert( BotFindEnemy( &bot, -1 ) && bot.enemy == 1 );
}

static void test_alignment_and_incoming_threat( void ) {
	reset();
	enemy( 1, 0, 1000, 270, qfalse );
	enemy( 2, 1100, 0, 180, qfalse );
	assert( BotFindEnemy( &bot, -1 ) && bot.enemy == 2 );
	reset();
	enemy( 1, 700, 0, 0, qtrue );
	enemy( 2, 900, 0, 180, qtrue );
	assert( BotFindEnemy( &bot, -1 ) && bot.enemy == 2 );
	reset();
	enemy( 1, 100, 0, 180, qfalse );
	enemy( 2, 1000, 0, 180, qfalse );
	visibility[1] = 0.1f;
	assert( BotFindEnemy( &bot, -1 ) && bot.enemy == 2 );
}

static void test_hysteresis_and_switch_initialization( void ) {
	reset();
	enemy( 1, 900, 0, 180, qfalse );
	enemy( 2, 800, 0, 180, qfalse );
	bot.enemy = 1;
	assert( !BotFindEnemy( &bot, 1 ) && bot.enemy == 1 );
	assert( bot.enemysight_time == 42.0f );
	assert( bot.inventory[ENEMY_HORIZONTAL_DIST] == 900 );
	entities[2].origin[0] = 100;
	bot.enemysuicide = qtrue;
	bot.enemydeath_time = 20;
	assert( BotFindEnemy( &bot, 1 ) && bot.enemy == 2 );
	assert( bot.enemysight_time == floattime - 2 );
	assert( !bot.enemysuicide && bot.enemydeath_time == 0 );
	assert( bot.enemyvisible_time == floattime );
	/* A hidden current enemy must not block a farther visible replacement. */
	visibility[2] = 0;
	entities[1].origin[0] = 2000;
	assert( BotFindEnemy( &bot, 2 ) && bot.enemy == 1 );
}

static void test_perception_filters( void ) {
	int filter;
	for ( filter = 0; filter < 11; filter++ ) {
		reset();
		enemy( 1, 30, 0, 180, qfalse );
		enemy( 2, 800, 0, 180, qfalse );
		switch ( filter ) {
		case 0: entities[1].valid = qfalse; break;
		case 1: g_entities[1].flags |= FL_NOTARGET; break;
		case 2: clients[1].pm_type = PM_DEAD; break;
		case 3: clients[1].pm_type = PM_SPECTATOR; break;
		case 4: entities[1].powerups |= 1 << PW_INVIS; break;
		case 5: easyFragger = 0; entities[1].flags |= EF_TALK; break;
		case 6: sameTeam[1] = qtrue; break;
		case 7: visibility[1] = 0; break;
		case 8: entities[1].origin[0] = 6000; break;
		case 9:
			lastteleport_time = floattime - 1;
			VectorCopy( entities[1].origin, lastteleport_origin );
			break;
		case 10: entities[1].origin[0] = -30; break;
		}
		assert( BotFindEnemy( &bot, -1 ) && bot.enemy == 2 );
	}
	reset();
	enemy( 1, -100, 0, 0, qfalse );
	assert( !BotFindEnemy( &bot, -1 ) && bot.enemy == -1 );
	bot.inventory[INVENTORY_HEALTH] = 90;
	assert( BotFindEnemy( &bot, -1 ) && bot.enemy == 1 );
	reset();
	enemy( 1, 1000, 0, 0, qfalse );
	wantsRetreat = qtrue;
	assert( !BotFindEnemy( &bot, -1 ) && bot.enemy == -1 );
}

static void test_other_modes_keep_legacy_selection( void ) {
	reset();
	gametype = GT_TEAM;
	enemy( 1, 2000, 0, 180, qfalse );
	enemy( 2, 400, 0, 180, qfalse );
	assert( BotFindEnemy( &bot, -1 ) && bot.enemy == 1 );
	entities[2].origin[0] = 2500;
	assert( !BotFindEnemy( &bot, 1 ) && bot.enemy == 1 );
	gametype = GT_RACING_DM;
	bot.enemy = -1;
	assert( !BotFindEnemy( &bot, -1 ) && bot.enemy == -1 );
}

static void test_scoring_tie_break_is_order_independent( void ) {
	int orderA[] = { 4, 1, 2 }, orderB[] = { 2, 4, 1 };
	int bestA = -1, bestB = -1, i;
	float bestScoreA = 0, bestScoreB = 0;
	for ( i = 0; i < 3; i++ ) {
		float score = Bot_DMEnemyScore( 900, 1, 0, qfalse, 1, qfalse );
		if ( Bot_DMEnemyIsBetter( orderA[i], score, bestA, bestScoreA ) ) {
			bestA = orderA[i]; bestScoreA = score;
		}
		if ( Bot_DMEnemyIsBetter( orderB[i], score, bestB, bestScoreB ) ) {
			bestB = orderB[i]; bestScoreB = score;
		}
	}
	assert( bestA == 1 && bestB == bestA );
}

int main( void ) {
	test_range_beats_slot_order();
	test_alignment_and_incoming_threat();
	test_hysteresis_and_switch_initialization();
	test_perception_filters();
	test_other_modes_keep_legacy_selection();
	test_scoring_tie_break_is_order_independent();
	puts( "ok" );
	return 0;
}
