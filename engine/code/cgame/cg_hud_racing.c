/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.
===========================================================================
*/

/*
===========================================================================
  cg_hud_racing.c

  All racing-specific HUD elements:
    - Checkpoint arrow / wrong-way warning
    - LCS elimination timeline
===========================================================================
*/

#include "cg_local.h"
#include "cg_hud_elements.h"

/* LCS timeline layout */
#define HUD_RIGHT_EDGE          636.0f
#define HUD_TEXT_INSET            6.0f
#define HUD_ROW_HEIGHT          ((float)TINYCHAR_HEIGHT + 4.0f)


/* -----------------------------------------------------------------------
   CG_DrawArrowToCheckpoint
   Draws the 3D arrow model and "WRONG WAY!" flash.
   ----------------------------------------------------------------------- */
float CG_DrawArrowToCheckpoint( float y ) {
	centity_t	*cent;
	vec3_t		forward, origin, angles;
	int			i;
	float		angle1, angle2, angleDiff;
	float		fx, fy, fw, fh;
	float		*color;
	refdef_t	refdef;
	refEntity_t	ent;
	vec3_t		mins, maxs, v;

	/* Suppress checkpoint arrow and Wrong Way during intro camera */
	if ( CG_IntroCam_IsActive() )
		return y;

	if ( cg_entities[cg.snap->ps.clientNum].finishRaceTime )
		return y;

	for ( i = 0; i < MAX_GENTITIES; i++ ) {
		cent = &cg_entities[i];
		if ( cent->currentState.eType != ET_CHECKPOINT ) continue;
		if ( cent->currentState.weapon != cg.snap->ps.stats[STAT_NEXT_CHECKPOINT] ) continue;
		break;
	}

	if ( i == MAX_GENTITIES )
		return y;

	trap_R_ModelBounds( cgs.inlineDrawModel[cent->currentState.modelindex], mins, maxs );

	if ( cent->currentState.frame == 0 ) {
		VectorAdd( mins, cent->currentState.origin, mins );
		VectorAdd( maxs, cent->currentState.origin, maxs );
	}

	for ( i = 0; i < 3; i++ ) {
		if      ( cg.predictedPlayerState.origin[i] < mins[i] ) v[i] = mins[i] - cg.predictedPlayerState.origin[i];
		else if ( cg.predictedPlayerState.origin[i] > maxs[i] ) v[i] = maxs[i] - cg.predictedPlayerState.origin[i];
		else                                                      v[i] = 0;
	}

	angle2 = ( v[0] == 0 && v[1] == 0 && v[2] == 0 )
	         ? cg.predictedPlayerState.viewangles[YAW]
	         : vectoyaw( v );

	if ( cg_checkpointArrowMode.integer == 1 ) {
		AngleVectors( cg.refdefViewAngles, forward, NULL, NULL );
		angle1     = vectoyaw( forward );
		angleDiff  = AngleDifference( angle1, angle2 );

		VectorSet( origin, 80, 0, 20 );
		VectorClear( angles );
		angles[YAW] = -angleDiff;

		fx = 320 - 64;  fy = 16;  fw = 128;  fh = 96;
		CG_AdjustFrom640( &fx, &fy, &fw, &fh );

		memset( &refdef, 0, sizeof( refdef ) );
		memset( &ent,    0, sizeof( ent    ) );

		AnglesToAxis( angles, ent.axis );
		VectorCopy( origin, ent.origin );
		VectorCopy( origin, ent.lightingOrigin );
		ent.hModel   = cgs.media.checkpointArrow;
		ent.renderfx = RF_NOSHADOW;

		refdef.rdflags = RDF_NOWORLDMODEL;
		vectoangles( origin, angles );
		AnglesToAxis( angles, refdef.viewaxis );
		refdef.fov_x  = 40;
		refdef.fov_y  = 30;
		refdef.x      = fx;  refdef.y      = fy;
		refdef.width  = fw;  refdef.height = fh;
		refdef.time   = cg.time;

		trap_R_ClearScene();
		trap_R_AddRefEntityToScene( &ent );
		trap_R_RenderScene( &refdef );
	}

	AngleVectors( cg.predictedPlayerEntity.lerpAngles, forward, NULL, NULL );
	angle1    = vectoyaw( forward );
	angleDiff = AngleDifference( angle1, angle2 );

	if ( fabs( angleDiff ) > 100 ) {
		cg.wrongWayTime = cg.time;
		if ( !cg.wrongWayStartTime )
			cg.wrongWayStartTime = cg.time;
	} else {
		cg.wrongWayStartTime = 0;
	}

	if ( !cg.wrongWayStartTime || cg.wrongWayStartTime > cg.time - 2000 )
		return y;

	color = CG_FadeColor( cg.wrongWayTime, 300 );
	if ( !color )
		return y;

	CG_DrawIngameString( (int)( SCREEN_WIDTH * 0.5f ),
	                      (int)( SCREEN_HEIGHT * 0.30f ), "WRONG WAY!",
	                      UI_CENTER | UI_DROPSHADOW, 0.9f, color );

	return y;
}
/* -----------------------------------------------------------------------
   CG_DrawEliminationTimeline
   ----------------------------------------------------------------------- */
float CG_DrawEliminationTimeline( float y ) {
	int			i;
	float		x;
	char		line[64];
	const float	columnWidth = CG_GetEliminationColumnWidth();
	const float	rowHeight   = HUD_ROW_HEIGHT;

	if ( !cg_elimTimeline.integer )                                       return y;
	if ( cgs.gametype != GT_LCS )                                        return y;
	if ( cg.elimTimelineCount <= 0 )                                      return y;

	x = HUD_RIGHT_EDGE - columnWidth;

	for ( i = cg.elimTimelineCount - 1; i >= 0; --i ) {
		const cgElimTimelineEvent_t *event = &cg.elimTimelineEvents[i];
		const char *name;
		int elapsedSeconds = 0;

		if ( event->clientNum < 0 || event->clientNum >= MAX_CLIENTS ) continue;

		name = cgs.clientinfo[event->clientNum].name;
		if ( !name || !name[0] ) name = va("#%d", event->clientNum);

		if ( event->timestamp > 0 && cg.time > event->timestamp )
			elapsedSeconds = ( cg.time - event->timestamp ) / 1000;

		Com_sprintf( line, sizeof(line), "R%02d LEFT%02d %s (%is)",
		             event->round, event->remaining, name, elapsedSeconds );

		CG_FillRect( x, y, columnWidth, rowHeight, bgColor );
		CG_DrawIngameSmallString( (int)( x + HUD_TEXT_INSET ), (int)y + 1, line, colorWhite );
		y += rowHeight;
	}

	return y;
}
