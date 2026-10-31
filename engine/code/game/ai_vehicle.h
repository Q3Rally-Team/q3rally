/* Shared vehicle commands for car-aware bot modes. */
#ifndef AI_VEHICLE_H
#define AI_VEHICLE_H

typedef enum {
	BOT_VEHICLE_DRIVE,
	BOT_VEHICLE_REVERSE,
	BOT_VEHICLE_TURN
} bot_vehicle_recovery_t;

typedef struct {
	qboolean active;
	int commandTime;
	float desiredYaw;
	float targetSpeed;
	float clearance;
	float targetClearance;
	float leftClearance;
	float rightClearance;
	float rearClearance;
	int targetBlockerEntity;
	qboolean blocked;
	qboolean onRoad;
	int reentryArea;
	bot_vehicle_recovery_t recovery;
	int recoveryUntil;
	int recoveries;
	int turnSign;
	int avoidSign;
	int avoidUntil;
	int progressTime;
	vec3_t progressOrigin;
	int lastLogTime;
} bot_vehicle_state_t;

struct bot_state_s;
struct bot_goal_s;
struct bot_moveresult_s;

void BotVehicleDrive( struct bot_state_s *bs, const vec3_t target, float targetSpeed,
	struct bot_moveresult_s *result );
void BotVehicleMoveToGoal( struct bot_state_s *bs, struct bot_moveresult_s *result,
	struct bot_goal_s *goal, int travelFlags );
void BotVehicleUpdateInput( struct bot_state_s *bs, usercmd_t *cmd );

#endif
