/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

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

#ifndef __CG_HUD_ELEMENTS_H__
#define __CG_HUD_ELEMENTS_H__

/*
===========================================================================
  HUD ELEMENT TOGGLE CVARS
  All cvars default to 1 (enabled). Set to 0 to hide an element.
  Accessible via the HUD Options menu (bound to a key) or console.
===========================================================================
*/

/* --- Racing HUD --- */
extern vmCvar_t  cg_hudShowTimes;          /* Lap/total/best times panel      */
extern vmCvar_t  cg_hudShowLaps;           /* Lap counter                      */
extern vmCvar_t  cg_hudShowPosition;       /* Current race position            */
extern vmCvar_t  cg_hudShowDistToFinish;   /* Distance to finish               */
/* cg_hudShowGhostDelta removed – use native cg_ghostPlayback */
/* cg_hudShowArrow removed – use native cg_checkpointArrowMode */
extern vmCvar_t  cg_hudShowCarAheadBehind; /* Nearest opponent names/gaps      */
/* cg_hudShowElimTimeline removed – use native cg_elimTimeline */
extern vmCvar_t  cg_hudShowOpponentList;   /* Opponent list (Elimination/LCS)  */
extern vmCvar_t  cg_hudShowScores;         /* DM/team scores panel             */

/* --- Vehicle HUD --- */
extern vmCvar_t  cg_hudShowSpeed;          /* Speedometer / RPM gauge          */
/* cg_hudShowRearView removed – use native cg_drawRearView */
/* cg_hudShowMiniMap removed – use native cg_drawMMap */

/* --- Derby HUD --- */
extern vmCvar_t  cg_hudShowDerbyVehicle;   /* Vehicle state panel (Derby)      */
extern vmCvar_t  cg_hudShowDerbyList;      /* Derby scoreboard                 */

/*
===========================================================================
  CVAR REGISTRATION - call once from CG_RegisterCvars()
===========================================================================
*/
void CG_HUD_RegisterCvars( void );

/*
===========================================================================
  HUD DRAW ENTRY POINTS
  Declared here so cg_hud_core.c can call across translation units.
===========================================================================
*/

/* cg_hud_racing.c */
float CG_DrawArrowToCheckpoint( float y );
float CG_DrawTimes( float y );
float CG_DrawGhostSplitDelta( float y );
float CG_DrawLaps( float y );
float CG_DrawDistanceToFinish( float y );
void  CG_DrawCurrentPosition( float y );
float CG_DrawEliminationTimeline( float y );
float CG_DrawCarAheadAndBehind( float y );
void  CG_DrawHUD_Times( float x, float y );
void  CG_DrawHUD_Positions( float x, float y );
void  CG_DrawHUD_Laps( float x, float y );
void  CG_DrawHUD_OpponentList( float x, float y );
void  CG_DrawHUD_Scores( float x, float y );

/* cg_hud_derby.c */
void  CG_DrawHUD_DerbyList( float x, float y );
void  CG_DrawHUD_DerbyHitImpact( void );
void  CG_DrawHUD_DerbyVehicleState( void );

/* cg_hud_vehicle.c */
float CG_DrawSpeed( float y );
void  CG_DrawFuelGauge( float x, float y, float w, float h );
void  CG_DrawRearviewMirror( float x, float y, float w, float h );
void  CG_DrawMMap( float x, float y, float w, float h );
void  CG_AddObjectsToScene( int renderLevel );

/* cg_hud_core.c */
float    CG_GetEliminationColumnWidth( void );
void     CG_DrawHUDOptionsMenu( void );
qboolean CG_HUDOptions_MouseEvent( int cx, int cy, qboolean clicked );
void     CG_HUDOptions_KeyEvent( int key );
qboolean CG_HUDOptionsIsOpen( void );
void     CG_HUD_RegisterCvars( void );
qboolean CG_DrawHUD( void );
float    CG_DrawUpperRightHUD( float y );
float    CG_DrawLowerRightHUD( float y );
float    CG_DrawLowerLeftHUD( float y );

#endif /* __CG_HUD_ELEMENTS_H__ */
