/*
===========================================================================
  snd_engine_presets.c

  MVP preset registry with hardcoded fallback presets and optional
  file-based overrides.
===========================================================================
*/

#include "snd_engine_presets.h"

#define MAX_ENGINE_AUDIO_PRESETS 16
#define DEFAULT_ENGINE_AUDIO_PRESET_PATH "sound/engine_audio/presets/sport_i4.eapreset"

static engineAudioPreset_t s_enginePresets[MAX_ENGINE_AUDIO_PRESETS];
static int s_enginePresetCount = 0;

static qboolean S_ParseFloatToken( char **text, float *outValue ) {
    char *token;

    token = COM_ParseExt( text, qfalse );
    if ( !token[0] ) {
        return qfalse;
    }

    *outValue = atof( token );
    return qtrue;
}

static qboolean S_ParseIntToken( char **text, int *outValue ) {
    char *token;

    token = COM_ParseExt( text, qfalse );
    if ( !token[0] ) {
        return qfalse;
    }

    *outValue = atoi( token );
    return qtrue;
}

static qboolean S_ParseEngineAudioPresetText( char *text, engineAudioPreset_t *outPreset ) {
    char *token;

    if ( !text || !outPreset ) {
        return qfalse;
    }

    Com_Memset( outPreset, 0, sizeof( *outPreset ) );

    while ( 1 ) {
        token = COM_ParseExt( &text, qtrue );
        if ( !token[0] ) {
            break;
        }

        if ( !Q_stricmp( token, "name" ) ) {
            token = COM_ParseExt( &text, qfalse );
            if ( !token[0] ) {
                return qfalse;
            }
            Q_strncpyz( outPreset->name, token, sizeof( outPreset->name ) );
        }
        else if ( !Q_stricmp( token, "cylinderCount" ) ) {
            if ( !S_ParseIntToken( &text, &outPreset->cylinderCount ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "strokeCycle" ) ) {
            if ( !S_ParseIntToken( &text, &outPreset->strokeCycle ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "firingOrderLength" ) ) {
            if ( !S_ParseIntToken( &text, &outPreset->firingOrderLength ) ) {
                return qfalse;
            }
            if ( outPreset->firingOrderLength < 0 || outPreset->firingOrderLength > MAX_ENGINE_AUDIO_FIRING_ORDER ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "firingOrder" ) ) {
            int i;
            int count;

            count = outPreset->firingOrderLength;
            if ( count <= 0 || count > MAX_ENGINE_AUDIO_FIRING_ORDER ) {
                return qfalse;
            }

            for ( i = 0; i < count; ++i ) {
                if ( !S_ParseIntToken( &text, &outPreset->firingOrder[i] ) ) {
                    return qfalse;
                }
            }
        }
        else if ( !Q_stricmp( token, "idleRpm" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->idleRpm ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "redlineRpm" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->redlineRpm ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "exhaustGain" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->exhaustGain ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "intakeGain" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->intakeGain ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "mechanicalGain" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->mechanicalGain ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "transmissionGain" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->transmissionGain ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "harmonicGain" ) ) {
            int index;
            float value;

            if ( !S_ParseIntToken( &text, &index ) || !S_ParseFloatToken( &text, &value ) ) {
                return qfalse;
            }
            if ( index < 0 || index >= MAX_ENGINE_AUDIO_HARMONICS ) {
                return qfalse;
            }
            outPreset->harmonicGains[index] = value;
        }
        else if ( !Q_stricmp( token, "exhaustResonator" ) ) {
            int index;
            engineAudioResonator_t *resonator;

            if ( outPreset->exhaustResonatorCount >= MAX_ENGINE_AUDIO_RESONATORS ) {
                return qfalse;
            }

            index = outPreset->exhaustResonatorCount++;
            resonator = &outPreset->exhaustResonators[index];

            if ( !S_ParseFloatToken( &text, &resonator->frequencyHz ) ||
                 !S_ParseFloatToken( &text, &resonator->q ) ||
                 !S_ParseFloatToken( &text, &resonator->gain ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "intakeResonator" ) ) {
            int index;
            engineAudioResonator_t *resonator;

            if ( outPreset->intakeResonatorCount >= MAX_ENGINE_AUDIO_RESONATORS ) {
                return qfalse;
            }

            index = outPreset->intakeResonatorCount++;
            resonator = &outPreset->intakeResonators[index];

            if ( !S_ParseFloatToken( &text, &resonator->frequencyHz ) ||
                 !S_ParseFloatToken( &text, &resonator->q ) ||
                 !S_ParseFloatToken( &text, &resonator->gain ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "distortionDrive" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->distortionDrive ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "noiseGain" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->noiseGain ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "backfireGain" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->backfireGain ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "limiterGain" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->limiterGain ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "cockpitLowpassHz" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->cockpitLowpassHz ) ) {
                return qfalse;
            }
        }
        else if ( !Q_stricmp( token, "exteriorPresenceGain" ) ) {
            if ( !S_ParseFloatToken( &text, &outPreset->exteriorPresenceGain ) ) {
                return qfalse;
            }
        }
        else {
            Com_Printf( S_COLOR_YELLOW "S_ParseEngineAudioPresetFile: unknown token '%s'\n", token );
            return qfalse;
        }
    }

    return S_ValidateEngineAudioPreset( outPreset );
}

static void S_BuildDefaultSportI4Preset( engineAudioPreset_t *p ) {
    Com_Memset( p, 0, sizeof( *p ) );

    Q_strncpyz( p->name, "sport_i4", sizeof( p->name ) );

    p->cylinderCount = 4;
    p->strokeCycle = 4;
    p->firingOrder[0] = 1;
    p->firingOrder[1] = 3;
    p->firingOrder[2] = 4;
    p->firingOrder[3] = 2;
    p->firingOrderLength = 4;

    p->idleRpm = 950.0f;
    p->redlineRpm = 8000.0f;

    p->exhaustGain = 1.28f;
    p->intakeGain = 0.30f;
    p->mechanicalGain = 0.16f;
    p->transmissionGain = 0.06f;

    p->exhaustResonatorCount = 3;
    p->exhaustResonators[0].frequencyHz = 78.0f;
    p->exhaustResonators[0].q = 1.40f;
    p->exhaustResonators[0].gain = 1.15f;
    p->exhaustResonators[1].frequencyHz = 132.0f;
    p->exhaustResonators[1].q = 1.15f;
    p->exhaustResonators[1].gain = 0.70f;

    p->exhaustResonators[2].frequencyHz = 196.0f;
    p->exhaustResonators[2].q = 1.00f;
    p->exhaustResonators[2].gain = 0.30f;

    p->intakeResonatorCount = 2;
    p->intakeResonators[0].frequencyHz = 300.0f;
    p->intakeResonators[0].q = 0.90f;
    p->intakeResonators[0].gain = 0.14f;
    p->intakeResonators[1].frequencyHz = 540.0f;
    p->intakeResonators[1].q = 0.78f;
    p->intakeResonators[1].gain = 0.07f;

    p->harmonicGains[0] = 1.00f;
    p->harmonicGains[1] = 0.22f;
    p->harmonicGains[2] = 0.06f;
    p->harmonicGains[3] = 0.02f;

    p->distortionDrive = 0.03f;
    p->noiseGain = 0.035f;
    p->backfireGain = 0.32f;
    p->limiterGain = 0.20f;
    p->cockpitLowpassHz = 2400.0f;
    p->exteriorPresenceGain = 1.12f;
}

qboolean S_ValidateEngineAudioPreset( const engineAudioPreset_t *preset ) {
    if ( !preset ) {
        return qfalse;
    }

    if ( preset->cylinderCount <= 0 ) {
        return qfalse;
    }

    if ( preset->strokeCycle != 2 && preset->strokeCycle != 4 ) {
        return qfalse;
    }

    if ( preset->idleRpm <= 0.0f || preset->redlineRpm <= preset->idleRpm ) {
        return qfalse;
    }

    if ( !preset->name[0] ) {
        return qfalse;
    }

    return qtrue;
}

int S_RegisterEngineAudioPreset( const engineAudioPreset_t *preset ) {
    if ( !preset || !S_ValidateEngineAudioPreset( preset ) ) {
        return -1;
    }

    if ( s_enginePresetCount >= MAX_ENGINE_AUDIO_PRESETS ) {
        return -1;
    }

    s_enginePresets[s_enginePresetCount] = *preset;
    return s_enginePresetCount++;
}

int S_FindEngineAudioPreset( const char *name ) {
    int i;

    if ( !name || !name[0] ) {
        return -1;
    }

    for ( i = 0; i < s_enginePresetCount; ++i ) {
        if ( !Q_stricmp( s_enginePresets[i].name, name ) ) {
            return i;
        }
    }

    return -1;
}

const engineAudioPreset_t *S_GetEngineAudioPresetByHandle( int handle ) {
    if ( handle < 0 || handle >= s_enginePresetCount ) {
        return NULL;
    }

    return &s_enginePresets[handle];
}

qboolean S_ParseEngineAudioPresetFile( const char *path, engineAudioPreset_t *outPreset ) {
    char *buffer;
    int len;
    qboolean ok;

    if ( !path || !path[0] || !outPreset ) {
        return qfalse;
    }

    len = FS_ReadFile( path, (void **)&buffer );
    if ( len <= 0 || !buffer ) {
        return qfalse;
    }

    ok = S_ParseEngineAudioPresetText( buffer, outPreset );
    FS_FreeFile( buffer );

    return ok;
}

void S_LoadEngineAudioPresets( void ) {
    engineAudioPreset_t preset;

    s_enginePresetCount = 0;

    if ( S_ParseEngineAudioPresetFile( DEFAULT_ENGINE_AUDIO_PRESET_PATH, &preset ) ) {
        S_RegisterEngineAudioPreset( &preset );
        return;
    }

    S_BuildDefaultSportI4Preset( &preset );
    S_RegisterEngineAudioPreset( &preset );
}
