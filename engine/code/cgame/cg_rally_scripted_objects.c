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

#include "cg_local.h"

#define		MAX_SCRIPT_TEXT		8192
#define GIB_VELOCITY 250
#define GIB_JUMP 100

qboolean SeekToSection( char **pointer, char *str ){
	char		*token;

	// UPDATE: using strstr instead?
	// UPDATE: check if end of file is inside of a bracket (ie bad brackets in script file)

	// seek to 'str {'
	while ( 1 ) {
		token = COM_Parse( pointer );

		if( !token || token[0] == 0 )
			return qfalse;

		if ( !Q_stricmp( token, "{" ) ){
			// loop through this
			while ( 1 ) {
				token = COM_Parse( pointer );

				if( !token || token[0] == 0 )
					return qfalse;

				if ( !Q_stricmp( token, "}" ) )
					break;
			}
		}

		if ( !Q_stricmp( token, str ) )
			break;
	}

	if( !token || token[0] == 0 ) // not found 
		return qfalse;

	return qtrue;
}

qboolean CG_ParseScriptedObject( centity_t *cent, const char *scriptName ){
	char		*text_p;
	int			len, i;
	char		*token;
	char		text[MAX_SCRIPT_TEXT];
	char		filename[MAX_QPATH];
	char		model[MAX_QPATH];
	char		deadmodel[MAX_QPATH];
	fileHandle_t	f;

	// setup defaults
//	ent->takedamage = qfalse;
//	VectorLength(ent->r.mins);
//	VectorLength(ent->r.maxs);
//	ent->elasticity = 0.1f;
//	ent->mass = 100;
//	ent->moveable = qfalse;
//	ent->number = 0;

	if (!scriptName || scriptName[0] == 0){
		Com_Printf("No Script file specified\n");
		return qfalse;
	}

	Q_strncpyz(filename, scriptName, sizeof(filename));
	token = strchr(filename, '.');
	if (!token)
		Q_strcat(filename, sizeof(filename), ".script");

//	Com_Printf("Attempting to load script %s\n", filename);

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );

	if ( !f ){
		Com_Printf("Could not find script %s\n", filename);
		return qfalse;
	}

	if ( len >= MAX_SCRIPT_TEXT ) {
		len = MAX_SCRIPT_TEXT - 1;
	}

	trap_FS_Read( text, len, f );
	text[len] = 0;

	trap_FS_FCloseFile( f );

	// parse the text
	text_p = text;

	// seek to "rally_scripted_object {"
	if ( !SeekToSection( &text_p, "rally_scripted_object" ) ){
		Com_Printf( "Script file '%s' did not contain rally_scripted_object\n", filename );
		return qfalse;
	}

       model[0] = 0;
       deadmodel[0] = 0;

       cent->numGibModels = 0;
       cent->gibsSpawned = qfalse;
       memset( cent->gibModels, 0, sizeof( cent->gibModels ) );
       memset( cent->gibSounds, 0, sizeof( cent->gibSounds ) );

	// read optional parameters
	while ( 1 ) {
		token = COM_Parse( &text_p );

		if( !token || token[0] == 0 || !Q_stricmp( token, "}" ) ) {
			break;
		}

		if ( !Q_stricmp( token, "{" ) )
			continue;

//		Com_Printf("Found token: %s\n", token);

		if ( !Q_stricmp( token, "type" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			continue;
		}
		if ( !Q_stricmp( token, "model" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			Q_strncpyz( model, token, sizeof(model) );

			continue;
		}
		else if ( !Q_stricmp( token, "deadmodel" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			Q_strncpyz( deadmodel, token, sizeof(deadmodel) );

			continue;
		}
		else if ( !Q_stricmp( token, "moveable" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			continue;
		}
		else if ( !Q_stricmp( token, "elasticity" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			continue;
		}
		else if ( !Q_stricmp( token, "mass" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			continue;
		}

		else if ( !Q_stricmp( token, "frames" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

//			ent->number = atoi(token);

			continue;
		}

		else if ( !Q_stricmp( token, "health" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			continue;
		}
		else if ( !Q_stricmp( token, "mins" ) ){
			for (i = 0; i < 3; i++){
				token = COM_Parse( &text_p );
				if ( !token ) break;
			}

			continue;
		}
		else if ( !Q_stricmp( token, "maxs" ) ){
			for (i = 0; i < 3; i++){
				token = COM_Parse( &text_p );
				if ( !token ) break;
			}

			continue;
		}
		else if ( !Q_stricmp( token, "hitsound" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			cent->hitSound = trap_S_RegisterSound( token, qfalse );

			continue;
		}
		else if ( !Q_stricmp( token, "presound" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			cent->preSoundLoop = trap_S_RegisterSound( token, qfalse );

			continue;
		}
		else if ( !Q_stricmp( token, "postsound" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			cent->postSoundLoop = trap_S_RegisterSound( token, qfalse );

			continue;
		}
		else if ( !Q_stricmp( token, "destroysound" ) ){
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}

			cent->destroySound = trap_S_RegisterSound( token, qfalse );

			continue;
		}
               else if ( !Q_stricmp( token, "gibs" ) ) {
                       token = COM_Parse( &text_p );
                       if ( !token ) {
                               break;
                       }

                       if ( !Q_stricmp( token, "{" ) ){
                               int current = -1;

                               while ( 1 ) {
                                       token = COM_Parse( &text_p );

                                       if( !token || token[0] == 0 )
                                               return qfalse;

                                       if ( !Q_stricmp( token, "}" ) )
                                               break;

                                       if ( !Q_stricmp( token, "model" ) ) {
                                               token = COM_Parse( &text_p );
                                               if ( !token ) {
                                                       break;
                                               }

                                               if ( cent->numGibModels < MAX_SCRIPT_GIBS ) {
                                                       current = cent->numGibModels++;
                                                       cent->gibModels[current] = trap_R_RegisterModel( token );
                                               }
                                               continue;
                                       }

                                       if ( !Q_stricmp( token, "sound" ) ) {
                                               token = COM_Parse( &text_p );
                                               if ( !token ) {
                                                       break;
                                               }

                                               if ( current >= 0 && current < MAX_SCRIPT_GIBS ) {
                                                       cent->gibSounds[current] = trap_S_RegisterSound( token, qfalse );
                                               }
                                               continue;
                                       }

                                       // skip any unknown token (like counts)
                               }
                       }

                       continue;
               }
		else {
			Com_Printf("Warning: Skipping unknown token %s in %s\n", token, filename);
			continue;
		}
	}

	if ( model[0] ){
		// reset the pointer
		text_p = text;

		if ( !SeekToSection( &text_p, model ) ){
//			Com_Printf( "'%s' section not found in script file, assuming it was an actual filename\n", model );

			cent->modelHandle = trap_R_RegisterModel( model );
			if ( !cent->modelHandle ) {
				Com_Printf( "Failed to register model '%s' referenced in script '%s'\n", model, filename );
				return qfalse;
			}
		}
		else {
			if (cg_developer.integer) Com_Printf( "Loading model info for '%s'\n", model );
			// load model info
		}
	}

	if ( deadmodel[0] ){
		// reset the pointer
		text_p = text;

		if ( !SeekToSection( &text_p, deadmodel ) ){
//			Com_Printf( "'%s' section not found in script file, assuming it was an actual filename\n", model );

			cent->deadModelHandle = trap_R_RegisterModel( deadmodel );
		}
		else {
			if (cg_developer.integer) Com_Printf( "Loading deadmodel info for '%s'\n", deadmodel );
			// load deadmodel info
		}
	}

//	Com_Printf("Successfully parsed script file\n");

	return qtrue;
}

/*
void CG_ScriptedObject_Destroy( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod ){
}


void CG_ScriptedObject_Touch ( gentity_t *self, gentity_t *other, trace_t *trace ){
}


void CG_ScriptedObject_Think ( gentity_t *self ){
	self->nextthink = cg.time + 100;
}


void CG_ScriptedObject_Pain ( gentity_t *self, gentity_t *attacker, int damage ){
}
*/


/*
Orientation between two snapshots via quaternion slerp. Interpolating the
three Euler angles separately breaks for anything that tumbles: a rolling
ball passes pitch +-90 all the time, where yaw and roll flip by 180 degrees,
and the in-between frames show wild orientations.
*/
static void CG_ScriptedObject_LerpAxis( centity_t *cent, vec3_t axis[3] ) {
	vec4_t from, to, q;
	float t[3][3];
	vec3_t right;
	float f, cosom, omega, sinom, scale0, scale1;
	int i;

	if ( !cent->interpolate || !cg.nextSnap ||
		cent->currentState.apos.trType != TR_INTERPOLATE ) {
		AnglesToAxis( cent->lerpAngles, axis );
		return;
	}

	AnglesToQuaternion( cent->currentState.apos.trBase, from );
	AnglesToQuaternion( cent->nextState.apos.trBase, to );
	f = cg.frameInterpolation;

	cosom = from[0] * to[0] + from[1] * to[1] + from[2] * to[2] + from[3] * to[3];
	if ( cosom < 0.0f ) {
		/* q and -q are the same rotation; take the short way round */
		cosom = -cosom;
		for ( i = 0; i < 4; i++ )
			to[i] = -to[i];
	}
	if ( cosom < 0.9995f ) {
		omega = Q_acos( cosom );
		sinom = sin( omega );
		scale0 = sin( ( 1.0f - f ) * omega ) / sinom;
		scale1 = sin( f * omega ) / sinom;
	} else {
		scale0 = 1.0f - f;
		scale1 = f;
	}
	for ( i = 0; i < 4; i++ )
		q[i] = scale0 * from[i] + scale1 * to[i];
	QuaternionNormalize( q );

	QuaternionToOrientation( q, t );
	OrientationToVectors( t, axis[0], right, axis[2] );
	VectorSubtract( vec3_origin, right, axis[1] );
}

void CG_Scripted_Object( centity_t *cent ){
	refEntity_t			ent;
	entityState_t		*s1;
	const char			*scriptName;

	s1 = &cent->currentState;

//	CG_LogPrintf("Spawning a rally_scripted_object\n");

	/* A script supplies optional destroyed-state assets; a direct model-index
	 * prop is valid without a script file. */
	if ( !s1->modelindex && !s1->modelindex2 ) {
		return;
	}

	if ( s1->modelindex && !cent->scriptLoadAttempted ) {
		cent->scriptLoadAttempted = qtrue;
		scriptName = CG_ConfigString( CS_SCRIPTS + s1->modelindex );
		if ( !scriptName[0] ) {
			if ( !s1->modelindex2 )
				return;
		} else if ( !CG_ParseScriptedObject( cent, scriptName ) && !s1->modelindex2 ) {
			return;
		}
	}

	if ( (cent->currentState.eFlags & EF_DEAD) && !cent->gibsSpawned ) {
               int i;
               vec3_t velocity;

               cent->gibsSpawned = qtrue;

               for ( i = 0; i < cent->numGibModels; i++ ) {
                       velocity[0] = crandom() * GIB_VELOCITY;
                       velocity[1] = crandom() * GIB_VELOCITY;
                       velocity[2] = GIB_JUMP + crandom() * GIB_VELOCITY;
                       CG_LaunchGib( cent->lerpOrigin, velocity, cent->gibModels[i], -1, 0, qfalse );
                       if ( cent->gibSounds[i] ) {
                               trap_S_StartSound( cent->lerpOrigin, ENTITYNUM_WORLD, CHAN_AUTO, cent->gibSounds[i] );
	}
               }
       }

	memset (&ent, 0, sizeof(ent));

	if ( cent->currentState.eFlags & EF_DEAD )
		ent.hModel = cent->deadModelHandle;
	else if ( s1->modelindex2 > 0 && s1->modelindex2 < MAX_MODELS )
		ent.hModel = cgs.gameModels[s1->modelindex2];
	else
		ent.hModel = cent->modelHandle;

	if ( !ent.hModel )
		return;

	// set frame
//	ent.oldframe = ent.frame;
//	ent.frame = s1->frame;
	ent.frame = ent.oldframe = 0;
	ent.backlerp = 0;

	VectorCopy( cent->lerpOrigin, ent.origin);
	VectorCopy( cent->lerpOrigin, ent.oldorigin);

	// convert angles to axis
	CG_ScriptedObject_LerpAxis( cent, ent.axis );

	// Autoball: a shadow on the ground shows where the ball will come down,
	// the seams glow in the colour of the last team that touched the ball
	if ( s1->generic1 & SCRIPTED_GENERIC1_NO_PREDICT ) {
		CG_Autoball_BallShadow( cent );
		CG_Autoball_BallTrail( cent );
		CG_Autoball_BallColor( s1, ent.shaderRGBA );
		// g_autoballBallScale: the model was made for the default size
		if ( s1->angles2[0] > 0.0f && s1->angles2[0] != 1.0f ) {
			VectorScale( ent.axis[0], s1->angles2[0], ent.axis[0] );
			VectorScale( ent.axis[1], s1->angles2[0], ent.axis[1] );
			VectorScale( ent.axis[2], s1->angles2[0], ent.axis[2] );
			ent.nonNormalizedAxes = qtrue;
		}
	}

	// add to refresh list
	trap_R_AddRefEntityToScene (&ent);
}
