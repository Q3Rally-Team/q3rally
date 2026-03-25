/*
===========================================================================
  snd_engine_audio.c

  Draft procedural engine audio emitter management and mixer entry points.
  This file is intentionally not wired into the build yet; it exists as a
  concrete starting point for implementation.
===========================================================================
*/

#include "snd_engine_audio.h"
#include "snd_engine_dsp.h"
#include "snd_engine_presets.h"

typedef struct engineAudioEmitterInternal_s {
    engineAudioEmitterPublicState_t pub;
    engineAudioSynthState_t synth;

    qboolean initialized;
    int generation;
    int lastUpdateFrame;
} engineAudioEmitterInternal_t;

static engineAudioEmitterInternal_t s_engineEmitters[MAX_ENGINE_AUDIO_EMITTERS];
static int s_engineAudioFrameCounter;
static int s_engineAudioNextDebugPrintTime;

static void S_DebugPrintEngineAudioState( void );

static engineAudioEmitterInternal_t *S_GetEngineEmitterForEntity( int entityNum ) {
    int i;

    for ( i = 0; i < MAX_ENGINE_AUDIO_EMITTERS; ++i ) {
        if ( s_engineEmitters[i].pub.active &&
             s_engineEmitters[i].pub.entityNum == entityNum ) {
            return &s_engineEmitters[i];
        }
    }

    return NULL;
}

static engineAudioEmitterInternal_t *S_AllocEngineEmitter( int entityNum ) {
    int i;

    for ( i = 0; i < MAX_ENGINE_AUDIO_EMITTERS; ++i ) {
        if ( !s_engineEmitters[i].pub.active ) {
            engineAudioEmitterInternal_t *em = &s_engineEmitters[i];

            Com_Memset( em, 0, sizeof( *em ) );
            em->pub.active = qtrue;
            em->pub.entityNum = entityNum;
            em->generation++;
            em->lastUpdateFrame = s_engineAudioFrameCounter;

            return em;
        }
    }

    return NULL;
}

static void S_FreeEngineEmitter( engineAudioEmitterInternal_t *em ) {
    if ( !em ) {
        return;
    }

    Com_Memset( em, 0, sizeof( *em ) );
}

void S_EngineAudio_Init( void ) {
    Com_Memset( s_engineEmitters, 0, sizeof( s_engineEmitters ) );
    s_engineAudioFrameCounter = 0;
    s_engineAudioNextDebugPrintTime = 0;
    S_LoadEngineAudioPresets();
}

void S_EngineAudio_Shutdown( void ) {
    Com_Memset( s_engineEmitters, 0, sizeof( s_engineEmitters ) );
    s_engineAudioFrameCounter = 0;
    s_engineAudioNextDebugPrintTime = 0;
}

void S_EngineAudio_BeginFrame( void ) {
    int i;

    ++s_engineAudioFrameCounter;

    for ( i = 0; i < MAX_ENGINE_AUDIO_EMITTERS; ++i ) {
        engineAudioEmitterInternal_t *em = &s_engineEmitters[i];

        if ( !em->pub.active ) {
            continue;
        }

        if ( em->lastUpdateFrame + 2 < s_engineAudioFrameCounter ) {
            if ( s_engineAudioDebug && s_engineAudioDebug->integer >= 3 && em->pub.preset ) {
                Com_Printf( "EngineAudio: freed stale emitter ent=%d preset=%s\n", em->pub.entityNum, em->pub.preset->name );
            }
            S_FreeEngineEmitter( em );
        }
    }
}

void S_RegisterEngineEmitter( int entityNum, int presetHandle ) {
    engineAudioEmitterInternal_t *em;
    const engineAudioPreset_t *preset;

    em = S_GetEngineEmitterForEntity( entityNum );
    if ( !em ) {
        em = S_AllocEngineEmitter( entityNum );
    }

    if ( !em ) {
        Com_Printf( S_COLOR_YELLOW "S_RegisterEngineEmitter: no free emitter slots\n" );
        return;
    }

    preset = S_GetEngineAudioPresetByHandle( presetHandle );
    if ( !preset ) {
        Com_Printf( S_COLOR_YELLOW "S_RegisterEngineEmitter: invalid preset handle %d\n", presetHandle );
        return;
    }

    em->pub.preset = preset;

    em->lastUpdateFrame = s_engineAudioFrameCounter;

    if ( !em->initialized ) {
        S_EngineDSP_Reset( &em->synth, dma.speed > 0 ? (float)dma.speed : 44100.0f );
        em->initialized = qtrue;
    }
}

void S_RemoveEngineEmitter( int entityNum ) {
    engineAudioEmitterInternal_t *em;

    em = S_GetEngineEmitterForEntity( entityNum );
    if ( em ) {
        S_FreeEngineEmitter( em );
    }
}

void S_UpdateEngineEmitterState(
    int entityNum,
    const vehicleAudioState_t *state,
    const vec3_t exhaustOrigin,
    const vec3_t engineBayOrigin,
    const vec3_t velocity,
    engineAudioQualityTier_t quality ) {
    engineAudioEmitterInternal_t *em;

    if ( !state ) {
        return;
    }

    em = S_GetEngineEmitterForEntity( entityNum );
    if ( !em ) {
        em = S_AllocEngineEmitter( entityNum );
        if ( !em ) {
            return;
        }
    }

    em->lastUpdateFrame = s_engineAudioFrameCounter;
    em->pub.control = *state;
    em->pub.quality = quality;
    VectorCopy( exhaustOrigin, em->pub.exhaustOrigin );
    VectorCopy( engineBayOrigin, em->pub.engineBayOrigin );
    VectorCopy( velocity, em->pub.velocity );

    if ( state->backfireEvent ) {
        S_EngineDSP_TriggerBackfire( &em->synth );
    }
}

void S_SetEngineEmitterPreset( int entityNum, int presetHandle ) {
    engineAudioEmitterInternal_t *em;
    const engineAudioPreset_t *preset;

    em = S_GetEngineEmitterForEntity( entityNum );
    preset = S_GetEngineAudioPresetByHandle( presetHandle );

    if ( em && preset ) {
        em->lastUpdateFrame = s_engineAudioFrameCounter;
        em->pub.preset = preset;
    }
}

void S_StopAllEngineEmitters( void ) {
    Com_Memset( s_engineEmitters, 0, sizeof( s_engineEmitters ) );
    s_engineAudioFrameCounter = 0;
    s_engineAudioNextDebugPrintTime = 0;
}

static const char *S_EngineAudioQualityName( engineAudioQualityTier_t quality ) {
    switch ( quality ) {
    case EA_QUALITY_HERO:
        return "hero";
    case EA_QUALITY_NEAR:
        return "near";
    case EA_QUALITY_FAR:
        return "far";
    default:
        return "off";
    }
}

static void S_DebugPrintEngineAudioState( void ) {
    int i;
    int activeCount;
    int heroCount;
    int nearCount;
    int farCount;
    int now;

    if ( !s_engineAudioDebug || !s_engineAudioDebug->integer ) {
        return;
    }

    now = Com_Milliseconds();
    if ( now < s_engineAudioNextDebugPrintTime ) {
        return;
    }

    s_engineAudioNextDebugPrintTime = now + 1000;
    activeCount = 0;
    heroCount = 0;
    nearCount = 0;
    farCount = 0;

    for ( i = 0; i < MAX_ENGINE_AUDIO_EMITTERS; ++i ) {
        engineAudioEmitterInternal_t *em = &s_engineEmitters[i];

        if ( !em->pub.active || em->pub.quality == EA_QUALITY_OFF || !em->pub.preset ) {
            continue;
        }

        ++activeCount;
        if ( em->pub.quality == EA_QUALITY_HERO ) {
            ++heroCount;
        }
        else if ( em->pub.quality == EA_QUALITY_NEAR ) {
            ++nearCount;
        }
        else if ( em->pub.quality == EA_QUALITY_FAR ) {
            ++farCount;
        }
    }

    Com_Printf( "EngineAudio: active=%d hero=%d near=%d far=%d gain=%.2f exh=%.2f int=%.2f mech=%.2f trans=%.2f srcExh=%.2f srcBay=%.2f evtExh=%.2f evtBay=%.2f cockpit=%d limiter=%d backfire=%d\n",
        activeCount,
        heroCount,
        nearCount,
        farCount,
        s_engineAudioGain ? s_engineAudioGain->value : 1.0f,
        s_engineAudioExhaustGainScale ? s_engineAudioExhaustGainScale->value : 1.0f,
        s_engineAudioIntakeGainScale ? s_engineAudioIntakeGainScale->value : 1.0f,
        s_engineAudioMechanicalGainScale ? s_engineAudioMechanicalGainScale->value : 1.0f,
        s_engineAudioTransmissionGainScale ? s_engineAudioTransmissionGainScale->value : 1.0f,
        s_engineAudioExhaustSourceGainScale ? s_engineAudioExhaustSourceGainScale->value : 1.0f,
        s_engineAudioEngineBaySourceGainScale ? s_engineAudioEngineBaySourceGainScale->value : 1.0f,
        s_engineAudioExhaustEventGainScale ? s_engineAudioExhaustEventGainScale->value : 1.0f,
        s_engineAudioEngineBayEventGainScale ? s_engineAudioEngineBayEventGainScale->value : 1.0f,
        s_engineAudioCockpitEnable ? s_engineAudioCockpitEnable->integer : 1,
        s_engineAudioLimiterEnable ? s_engineAudioLimiterEnable->integer : 1,
        s_engineAudioBackfireEnable ? s_engineAudioBackfireEnable->integer : 1 );

    if ( s_engineAudioDebug->integer < 2 ) {
        return;
    }

    for ( i = 0; i < MAX_ENGINE_AUDIO_EMITTERS; ++i ) {
        engineAudioEmitterInternal_t *em = &s_engineEmitters[i];

        if ( !em->pub.active || em->pub.quality == EA_QUALITY_OFF || !em->pub.preset ) {
            continue;
        }

        Com_Printf( "  ent=%d preset=%s quality=%s rpm=%.0f throttle=%.2f load=%.2f slip=%.2f turbo=%.2f\n",
            em->pub.entityNum,
            em->pub.preset->name,
            S_EngineAudioQualityName( em->pub.quality ),
            em->pub.control.rpm,
            em->pub.control.throttle,
            em->pub.control.load,
            em->pub.control.wheelSlip,
            em->pub.control.turboBoost );
    }
}

static void S_ComputeEngineEmitterSpatialGains(
    const vec3_t origin,
    engineAudioQualityTier_t quality,
    float *leftGain,
    float *rightGain ) {
    int leftVol;
    int rightVol;
    int masterVol;
    vec3_t spatialOrigin;
    if ( !leftGain || !rightGain ) {
        return;
    }

    *leftGain = 0.0f;
    *rightGain = 0.0f;

    masterVol = 220;
    if ( quality == EA_QUALITY_NEAR ) {
        masterVol = 180;
    }
    else if ( quality == EA_QUALITY_FAR ) {
        masterVol = 132;
    }

    VectorCopy( origin, spatialOrigin );
    S_SpatializeOrigin( spatialOrigin, masterVol, &leftVol, &rightVol );

    *leftGain = leftVol / 255.0f;
    *rightGain = rightVol / 255.0f;
}

void S_RenderEngineAudio( portable_samplepair_t *buffer, int sampleCount ) {
    int i;
    const int paintbufferClamp = 0x00ffff00;
    const float mixScale = 2000.0f * 256.0f;
    static float tempExhaustLeft[4096];
    static float tempExhaustRight[4096];
    static float tempEngineBayLeft[4096];
    static float tempEngineBayRight[4096];

    if ( !buffer || sampleCount <= 0 ) {
        return;
    }

    if ( sampleCount > 4096 ) {
        sampleCount = 4096;
    }

    S_DebugPrintEngineAudioState();

    for ( i = 0; i < MAX_ENGINE_AUDIO_EMITTERS; ++i ) {
        int s;
        float exhaustLeftGain;
        float exhaustRightGain;
        float engineBayLeftGain;
        float engineBayRightGain;
        engineAudioEmitterInternal_t *em = &s_engineEmitters[i];

        if ( !em->pub.active || em->pub.quality == EA_QUALITY_OFF || !em->pub.preset ) {
            continue;
        }

        Com_Memset( tempExhaustLeft, 0, sizeof(float) * sampleCount );
        Com_Memset( tempExhaustRight, 0, sizeof(float) * sampleCount );
        Com_Memset( tempEngineBayLeft, 0, sizeof(float) * sampleCount );
        Com_Memset( tempEngineBayRight, 0, sizeof(float) * sampleCount );

        S_ComputeEngineEmitterSpatialGains( em->pub.exhaustOrigin, em->pub.quality, &exhaustLeftGain, &exhaustRightGain );
        S_ComputeEngineEmitterSpatialGains( em->pub.engineBayOrigin, em->pub.quality, &engineBayLeftGain, &engineBayRightGain );
        if ( exhaustLeftGain <= 0.0f && exhaustRightGain <= 0.0f &&
             engineBayLeftGain <= 0.0f && engineBayRightGain <= 0.0f ) {
            continue;
        }

        S_EngineDSP_RenderVehicle(
            &em->synth,
            em->pub.preset,
            &em->pub.control,
            em->pub.quality,
            sampleCount,
            tempExhaustLeft,
            tempExhaustRight,
            tempEngineBayLeft,
            tempEngineBayRight );

        for ( s = 0; s < sampleCount; ++s ) {
            int l = buffer[s].left +
                (int)( tempExhaustLeft[s] * exhaustLeftGain * mixScale ) +
                (int)( tempEngineBayLeft[s] * engineBayLeftGain * mixScale );
            int r = buffer[s].right +
                (int)( tempExhaustRight[s] * exhaustRightGain * mixScale ) +
                (int)( tempEngineBayRight[s] * engineBayRightGain * mixScale );

            if ( l > paintbufferClamp ) l = paintbufferClamp;
            if ( l < -paintbufferClamp ) l = -paintbufferClamp;
            if ( r > paintbufferClamp ) r = paintbufferClamp;
            if ( r < -paintbufferClamp ) r = -paintbufferClamp;

            buffer[s].left = l;
            buffer[s].right = r;
        }
    }
}
