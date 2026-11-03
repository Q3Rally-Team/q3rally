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
#include "ai_dmnet.h"
#include "inv.h"

int gametype;
float floattime;
static aas_entityinfo_t enemies[MAX_CLIENTS];
static bot_state_t bot;
static vec3_t driveTarget;
static float driveSpeed;
static int driveCalls;
static qboolean aimWaits;
static int order[8], orderCount;
enum { CHOOSE = 1, AIM, DRIVE, BLOCKED, ATTACK, LEGACY, RALLY };

void QDECL Com_Error( int severity, const char *format, ... ) { assert( 0 ); }
void BotEntityInfo( int client, aas_entityinfo_t *info ) { *info = enemies[client]; }
void BotVehicleDrive( bot_state_t *bs, const vec3_t target, float speed, bot_moveresult_t *result ) {
	VectorCopy( target, driveTarget );
	driveSpeed = speed;
	driveCalls++;
	order[orderCount++] = DRIVE;
	VectorSet( bs->ideal_viewangles, 0, 111, 0 );
	if ( result ) VectorSet( result->movedir, 1, 0, 0 );
}
void BotChooseWeapon( bot_state_t *bs ) { order[orderCount++] = CHOOSE; bs->weaponnum = WP_MACHINEGUN; }
void BotAimAtEnemy( bot_state_t *bs ) {
	assert( bs->weaponnum == WP_MACHINEGUN );
	order[orderCount++] = AIM;
	if ( aimWaits ) return;
	VectorCopy( enemies[bs->enemy].origin, bs->aimtarget );
	bs->aimtarget[1] += 30;
	VectorSet( bs->ideal_viewangles, 0, 222, 0 );
}
void BotAIBlocked( bot_state_t *bs, bot_moveresult_t *result, int activate ) { order[orderCount++] = BLOCKED; }
void BotCheckAttack( bot_state_t *bs ) { order[orderCount++] = ATTACK; }
int BotWantsToRetreat( bot_state_t *bs ) { return qfalse; }
void AIEnter_Battle_Retreat( bot_state_t *bs, char *reason ) { assert( 0 ); }
void trap_BotResetAvoidReach( int state ) { assert( 0 ); }
static qboolean BotUseRallyCombatMovement( bot_state_t *bs ) { return gametype == GT_LCS; }
static bot_moveresult_t BotRallyCombatMove( bot_state_t *bs, int flags ) {
	bot_moveresult_t result;
	memset( &result, 0, sizeof( result ) );
	order[orderCount++] = RALLY;
	return result;
}
static bot_moveresult_t LegacyCombatMove( bot_state_t *bs, int flags ) {
	bot_moveresult_t result;
	memset( &result, 0, sizeof( result ) );
	order[orderCount++] = LEGACY;
	return result;
}

#include "dm_combat_plan_under_test.h"

static void reset( int weapon, float distance ) {
	memset( &bot, 0, sizeof( bot ) );
	memset( enemies, 0, sizeof( enemies ) );
	memset( order, 0, sizeof( order ) );
	orderCount = driveCalls = 0;
	aimWaits = qfalse;
	gametype = GT_DEATHMATCH;
	floattime = 100;
	bot.settings.skill = 3;
	bot.enemy = 1;
	bot.rallyCombatEnemy = -1;
	bot.weaponnum = weapon;
	bot.cur_ps.pm_type = PM_NORMAL;
	enemies[1].number = 1;
	enemies[1].valid = qtrue;
	VectorSet( enemies[1].origin, distance, 0, 0 );
	VectorCopy( enemies[1].origin, bot.aimtarget );
}

static void move( void ) { orderCount = 0; BotDeathmatchCombatMove( &bot ); }

static void test_weapon_engagement_ranges( void ) {
	reset( WP_RAILGUN, 1500 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_FIRE );
	reset( WP_SHOTGUN, 1500 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_APPROACH );
	reset( WP_MACHINEGUN, 300 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_FIRE );
	reset( WP_ROCKET_LAUNCHER, 200 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_BREAKAWAY );
	reset( WP_GRENADE_LAUNCHER, 200 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_BREAKAWAY );
	reset( WP_BFG, 200 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_BREAKAWAY );
	reset( WP_LIGHTNING, 500 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_FIRE );
	reset( WP_GAUNTLET, 200 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_APPROACH );
	reset( WP_GAUNTLET, 40 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_FIRE );
	reset( WP_MACHINEGUN, 2000 ); bot.settings.skill = -5; move();
	assert( driveSpeed == 700 );
	reset( WP_MACHINEGUN, 2000 ); bot.settings.skill = 20; move();
	assert( driveSpeed == 1000 );
}

static void test_breakaway_changes_direction_and_returns( void ) {
	vec3_t away;
	reset( WP_MACHINEGUN, 500 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_FIRE );
	floattime += 4;
	move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_FIRE ); /* No blind timed retreat. */
	VectorSet( enemies[1].origin, -50, 0, 0 );
	VectorCopy( enemies[1].origin, bot.aimtarget );
	move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_BREAKAWAY );
	VectorSubtract( driveTarget, bot.origin, away );
	assert( fabs( VectorLength( away ) - 650 ) < 0.1f );
	assert( fabs( away[1] ) > 400 );
	assert( DistanceSquared( driveTarget, bot.aimtarget ) > Square( 300 ) );
	assert( bot.aimtarget[0] == -50 && bot.aimtarget[1] == 0 );
	floattime += 0.5f;
	move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_BREAKAWAY );
	floattime += 0.4f;
	move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_TURNAROUND && driveSpeed > 340 );
	VectorSet( enemies[1].origin, 500, 0, 0 );
	VectorCopy( enemies[1].origin, bot.aimtarget );
	floattime += 0.1f;
	move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_APPROACH );
	move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_FIRE );
}

static void test_hits_target_changes_and_driver_recovery( void ) {
	reset( WP_MACHINEGUN, 500 ); move();
	bot.vehicle.recovery = BOT_VEHICLE_REVERSE;
	bot.vehicle.recoveries = 7;
	bot.vehicle.recoveryUntil = 9999;
	floattime += 4;
	bot.cur_ps.persistant[PERS_HITS]++;
	move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_FIRE );
	assert( bot.rallyCombatLastHitTime == floattime );
	floattime += 4;
	move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_FIRE );
	bot.enemy = 2;
	enemies[2].valid = qtrue;
	VectorSet( enemies[2].origin, 2000, 0, 0 );
	VectorCopy( enemies[2].origin, bot.aimtarget );
	move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_APPROACH );
	assert( bot.rallyCombatEnemy == 2 && bot.rallyCombatLastHitTime == floattime );
	assert( bot.vehicle.recovery == BOT_VEHICLE_REVERSE );
	assert( bot.vehicle.recoveries == 7 && bot.vehicle.recoveryUntil == 9999 );
	bot.enemy = -1;
	driveCalls = 0; move();
	assert( driveCalls == 0 );
	bot.enemy = 2; enemies[2].valid = qfalse; move();
	assert( driveCalls == 0 );
}

static void test_fight_command_order_and_reaction_fallback( void ) {
	int dmOrder[] = { CHOOSE, AIM, DRIVE, BLOCKED, ATTACK };
	int legacyOrder[] = { CHOOSE, LEGACY, BLOCKED, AIM, ATTACK };
	reset( WP_MACHINEGUN, 500 );
	assert( FightCommandsUnderTest( &bot ) );
	assert( orderCount == 5 && memcmp( order, dmOrder, sizeof( dmOrder ) ) == 0 );
	assert( driveTarget[0] == 500 && driveTarget[1] == 30 );
	assert( bot.ideal_viewangles[YAW] == 111 );
	reset( WP_MACHINEGUN, 700 );
	aimWaits = qtrue;
	VectorSet( bot.aimtarget, -1234, 4567, 0 );
	assert( FightCommandsUnderTest( &bot ) );
	assert( driveTarget[0] == 700 && driveTarget[1] == 0 );
	reset( WP_MACHINEGUN, 500 );
	gametype = GT_TEAM;
	assert( FightCommandsUnderTest( &bot ) );
	assert( orderCount == 5 && memcmp( order, legacyOrder, sizeof( legacyOrder ) ) == 0 );
	assert( driveCalls == 0 && bot.ideal_viewangles[YAW] == 222 );
	reset( WP_MACHINEGUN, 500 ); gametype = GT_LCS;
	BotAttackMove( &bot, 0 );
	assert( orderCount == 1 && order[0] == RALLY && driveCalls == 0 );
	reset( WP_MACHINEGUN, 500 ); bot.cur_ps.pm_type = PM_DEAD;
	BotAttackMove( &bot, 0 );
	assert( orderCount == 1 && order[0] == LEGACY && driveCalls == 0 );
}

static void test_pressure_and_skill_without_unsafe_explosives( void ) {
	float noviceSpeed;
	reset( WP_MACHINEGUN, 500 ); bot.settings.skill = 1; move();
	noviceSpeed = driveSpeed;
	assert( noviceSpeed >= 480 );
	reset( WP_MACHINEGUN, 500 ); bot.settings.skill = 5; move();
	assert( driveSpeed > noviceSpeed );
	/* A close, aligned hitscan target remains an opportunity, not a retreat. */
	reset( WP_MACHINEGUN, 80 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_FIRE );
	reset( WP_FLAME_THROWER, 400 ); move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_FIRE );
	reset( WP_ROCKET_LAUNCHER, 200 ); bot.settings.skill = 5; move();
	assert( bot.rallyCombatPhase == RALLY_COMBAT_BREAKAWAY );
	/* Keep up with an observed escaping car, with a bounded request. */
	reset( WP_MACHINEGUN, 700 );
	enemies[1].update_time = 0.1f;
	VectorSet( enemies[1].lastvisorigin, 600, 0, 0 );
	move();
	assert( driveSpeed > 1000 && driveSpeed <= 1100 );
	enemies[1].lastvisorigin[0] = -5000;
	move(); assert( driveSpeed == 1050 );
	enemies[1].update_time = 0;
	move(); assert( driveSpeed < 1000 );
}

static void test_start_weapon_confidence_and_real_retreat_reasons( void ) {
	reset( WP_MACHINEGUN, 500 );
	bot.cur_ps.weapon = WP_MACHINEGUN;
	bot.cur_ps.stats[STAT_WEAPONS] = (1 << WP_MACHINEGUN);
	bot.cur_ps.ammo[WP_MACHINEGUN] = 50;
	bot.inventory[INVENTORY_HEALTH] = 100;
	assert( BotAggression( &bot ) > 50 );
	bot.cur_ps.ammo[WP_MACHINEGUN] = 0;
	assert( BotAggression( &bot ) == 0 );
	bot.cur_ps.ammo[WP_MACHINEGUN] = -1; /* Infinite ammunition remains usable. */
	assert( BotAggression( &bot ) > 50 );
	bot.inventory[INVENTORY_HEALTH] = 20;
	assert( BotAggression( &bot ) == 0 );
	bot.inventory[INVENTORY_HEALTH] = 30;
	bot.settings.skill = 1;
	assert( BotAggression( &bot ) == 0 );
	bot.settings.skill = 5;
	assert( BotAggression( &bot ) > 50 );
	bot.inventory[INVENTORY_HEALTH] = 100;
	bot.cur_ps.stats[STAT_WEAPONS] = (1 << WP_GAUNTLET);
	bot.cur_ps.ammo[WP_GAUNTLET] = -1;
	bot.inventory[ENEMY_HORIZONTAL_DIST] = 500;
	assert( BotAggression( &bot ) == 0 );
	bot.inventory[ENEMY_HORIZONTAL_DIST] = 100;
	assert( BotAggression( &bot ) > 50 );
	gametype = GT_TEAM;
	assert( BotAggression( &bot ) == 37 ); /* Sentinel for unchanged legacy path. */
}

static void test_current_weapon_ammo_drives_supply_decision( void ) {
	reset( WP_SHOTGUN, 500 );
	bot.cur_ps.weapon = WP_SHOTGUN;
	bot.cur_ps.stats[STAT_WEAPONS] = (1 << WP_SHOTGUN);
	bot.cur_ps.ammo[WP_SHOTGUN] = 3;
	bot.inventory[INVENTORY_SHOTGUN] = 1;
	bot.inventory[INVENTORY_SHELLS] = 3;
	assert( !BotNeedsCombatSupplies( &bot ) ); /* No bullets needed for a loaded shotgun. */
	bot.cur_ps.ammo[WP_SHOTGUN] = bot.inventory[INVENTORY_SHELLS] = 2;
	assert( BotNeedsCombatSupplies( &bot ) );
	bot.cur_ps.ammo[WP_SHOTGUN] = bot.inventory[INVENTORY_SHELLS] = 3;
	gametype = GT_TEAM;
	assert( BotNeedsCombatSupplies( &bot ) ); /* Legacy preference remains unchanged. */
}

int main( void ) {
	test_weapon_engagement_ranges();
	test_breakaway_changes_direction_and_returns();
	test_hits_target_changes_and_driver_recovery();
	test_fight_command_order_and_reaction_fallback();
	test_pressure_and_skill_without_unsafe_explosives();
	test_start_weapon_confidence_and_real_retreat_reasons();
	test_current_weapon_ammo_drives_supply_decision();
	puts( "ok" );
	return 0;
}
