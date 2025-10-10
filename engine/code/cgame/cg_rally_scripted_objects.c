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

	cent->hitSound = 0;
	cent->preSoundLoop = 0;
	cent->postSoundLoop = 0;
	cent->destroySound = 0;
	cent->activeScriptLoop = 0;
	cent->scriptDestroyPending = qfalse;
	cent->scriptLastEvent = cent->currentState.event;
	cent->hitSoundName[0] = '\0';
	cent->preSoundLoopName[0] = '\0';
	cent->postSoundLoopName[0] = '\0';
	cent->destroySoundName[0] = '\0';

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

	// Models can be referenced directly by path or via a section that contains a "model" token.
	cent->modelHandle = 0;
	cent->deadModelHandle = 0;

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
                        Q_strncpyz( cent->hitSoundName, token, sizeof( cent->hitSoundName ) );

                        continue;
                }
                else if ( !Q_stricmp( token, "presound" ) ){
                        token = COM_Parse( &text_p );
                        if ( !token ) {
                                break;
                        }

                        cent->preSoundLoop = trap_S_RegisterSound( token, qfalse );
                        Q_strncpyz( cent->preSoundLoopName, token, sizeof( cent->preSoundLoopName ) );

                        continue;
                }
                else if ( !Q_stricmp( token, "postsound" ) ){
                        token = COM_Parse( &text_p );
			if ( !token ) {
				break;
                        }

                        cent->postSoundLoop = trap_S_RegisterSound( token, qfalse );
                        Q_strncpyz( cent->postSoundLoopName, token, sizeof( cent->postSoundLoopName ) );

                        continue;
                }
                else if ( !Q_stricmp( token, "destroysound" ) ){
                        token = COM_Parse( &text_p );
			if ( !token ) {
				break;
                        }

                        cent->destroySound = trap_S_RegisterSound( token, qfalse );
                        Q_strncpyz( cent->destroySoundName, token, sizeof( cent->destroySoundName ) );

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
		qboolean foundModelPath = qfalse;

		// reset the pointer
		text_p = text;

		if ( !SeekToSection( &text_p, model ) ){
//			Com_Printf( "'%s' section not found in script file, assuming it was an actual filename\n", model );

			cent->modelHandle = trap_R_RegisterModel( model );
		}
		else {
			while ( 1 ) {
				token = COM_Parse( &text_p );

				if ( !token || token[0] == 0 ) {
					break;
				}

				if ( !Q_stricmp( token, "{" ) ) {
					continue;
				}

				if ( !Q_stricmp( token, "}" ) ) {
					break;
				}

				if ( !Q_stricmp( token, "model" ) ) {
					token = COM_Parse( &text_p );
					if ( token && token[0] ) {
						cent->modelHandle = trap_R_RegisterModel( token );
						foundModelPath = qtrue;
					}
					break;
				}
			}

			if ( !foundModelPath ) {
				cent->modelHandle = trap_R_RegisterModel( model );
			}
		}
	}

	if ( deadmodel[0] ){
		qboolean foundDeadModelPath = qfalse;

		// reset the pointer
		text_p = text;

		if ( !SeekToSection( &text_p, deadmodel ) ){
//			Com_Printf( "'%s' section not found in script file, assuming it was an actual filename\n", model );

			cent->deadModelHandle = trap_R_RegisterModel( deadmodel );
		}
		else {
			while ( 1 ) {
				token = COM_Parse( &text_p );

				if ( !token || token[0] == 0 ) {
					break;
				}

				if ( !Q_stricmp( token, "{" ) ) {
					continue;
				}

				if ( !Q_stricmp( token, "}" ) ) {
					break;
				}

				if ( !Q_stricmp( token, "model" ) ) {
					token = COM_Parse( &text_p );
					if ( token && token[0] ) {
						cent->deadModelHandle = trap_R_RegisterModel( token );
						foundDeadModelPath = qtrue;
					}
					break;
				}
			}

			if ( !foundDeadModelPath ) {
				cent->deadModelHandle = trap_R_RegisterModel( deadmodel );
			}
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


void CG_Scripted_Object( centity_t *cent ){
	refEntity_t			ent;
	entityState_t		*s1;
	const char			*scriptName;

	s1 = &cent->currentState;

//	CG_LogPrintf("Spawning a rally_scripted_object\n");

	// if no script file for it then return
	if (!s1->modelindex) {
		return;
	}

        if ( !cent->scriptLoadTime ){
                scriptName = CG_ConfigString( CS_SCRIPTS + s1->modelindex );
                if ( !scriptName[0] ) {
                        return;
                }

                if ( CG_ParseScriptedObject( cent, scriptName ) )
                        cent->scriptLoadTime = cg.time;
                else
                        return;
        }

        if ( cent->scriptLastEvent != s1->event ) {
                int eventType = s1->event & ~EV_EVENT_BITS;

                if ( eventType == EV_GENERAL_SOUND ) {
                        const char *eventSound = CG_ConfigString( CS_SOUNDS + s1->eventParm );

                        if ( eventSound && eventSound[0] ) {
                                if ( cent->destroySoundName[0] && !Q_stricmp( eventSound, cent->destroySoundName ) ) {
                                        cent->scriptDestroyPending = qtrue;
                                } else if ( cent->hitSoundName[0] && !Q_stricmp( eventSound, cent->hitSoundName ) ) {
                                        cent->scriptDestroyPending = qfalse;
                                }
                        }
                }

                cent->scriptLastEvent = s1->event;
        }

        if ( s1->eFlags & EF_DEAD ) {
                cent->scriptDestroyPending = qfalse;
        }

        {
                qhandle_t desiredLoop = 0;

                if ( ( s1->eFlags & EF_DEAD ) || cent->scriptDestroyPending ) {
                        if ( cent->postSoundLoop ) {
                                desiredLoop = cent->postSoundLoop;
                        }
                } else if ( cent->preSoundLoop ) {
                        desiredLoop = cent->preSoundLoop;
                }

                if ( desiredLoop != cent->activeScriptLoop ) {
                        if ( desiredLoop ) {
                                trap_S_AddLoopingSound( s1->number, cent->lerpOrigin, vec3_origin, desiredLoop );
                        } else if ( cent->activeScriptLoop ) {
                                trap_S_StopLoopingSound( s1->number );
                        }

                        cent->activeScriptLoop = desiredLoop;
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
	AnglesToAxis( cent->lerpAngles, ent.axis );

	// add to refresh list
	trap_R_AddRefEntityToScene (&ent);
}
