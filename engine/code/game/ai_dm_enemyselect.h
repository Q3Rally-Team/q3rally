#ifndef AI_DM_ENEMYSELECT_H
#define AI_DM_ENEMYSELECT_H

/* Only score candidates after the normal perception/eligibility checks. */
static ID_INLINE float Bot_DMEnemyScore( float distance, float forwardAlignment,
		float threatAlignment, qboolean shooting, float visibility, qboolean currentEnemy ) {
	float score;

	if ( distance < 0.0f ) distance = 0.0f;
	if ( forwardAlignment < 0.0f ) forwardAlignment = 0.0f;
	if ( threatAlignment < 0.0f ) threatAlignment = 0.0f;
	if ( visibility > 1.0f ) visibility = 1.0f;
	if ( visibility < 0.0f ) visibility = 0.0f;

	/* Nearby, forward targets are easier to engage with a vehicle. */
	score = 2.0f / ( 1.0f + distance / 900.0f ) + 0.8f * forwardAlignment;
	/* Prefer a visible shooter facing us over someone firing elsewhere. */
	if ( shooting ) score += 0.3f + 0.9f * threatAlignment;
	score *= visibility;
	/* Small range/angle changes should not interrupt an ongoing engagement. */
	if ( currentEnemy ) score += 0.45f;
	return score;
}

static ID_INLINE qboolean Bot_DMEnemyIsBetter( int candidate, float score,
		int bestEnemy, float bestScore ) {
	/* A stable tie break also makes results independent of scan ordering. */
	return bestEnemy < 0 || score > bestScore ||
		( score == bestScore && candidate < bestEnemy );
}

#endif
