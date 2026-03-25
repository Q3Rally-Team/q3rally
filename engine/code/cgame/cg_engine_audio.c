/*
===========================================================================
  cg_engine_audio.c

  Draft client-side vehicle audio control collection for the procedural
  engine audio system. This file is intentionally not integrated into the
  build yet; it documents the proposed runtime API and control flow.
===========================================================================
*/

#include "cg_engine_audio.h"

static cgVehicleAudioDebug_t s_cgVehicleAudioDebug;
static float s_cgLastLocalSpeed;
static float s_cgLastLocalRpmNorm;
static float s_cgLastLocalThrottle;
static int s_cgLastLocalGear;

#define CG_ENGINE_AUDIO_IDLE_RPM 900.0f
#define CG_ENGINE_AUDIO_REDLINE_RPM 8000.0f

static float CG_CalcEngineAudioRpmNorm( float rpm, float idleRpm, float redlineRpm ) {
    float norm;

    if ( redlineRpm <= idleRpm ) {
        return 0.0f;
    }

    norm = ( rpm - idleRpm ) / ( redlineRpm - idleRpm );

    if ( norm < 0.0f ) {
        norm = 0.0f;
    }
    else if ( norm > 1.0f ) {
        norm = 1.0f;
    }

    return norm;
}

static float CG_Clamp01( float value ) {
    if ( value < 0.0f ) {
        return 0.0f;
    }

    if ( value > 1.0f ) {
        return 1.0f;
    }

    return value;
}

static int CG_EstimateRemoteGear( centity_t *cent, float speed ) {
    float absSpeed;

    (void)cent;

    absSpeed = fabs( speed );

    if ( absSpeed < 15.0f ) {
        return 1;
    }
    if ( absSpeed < 28.0f ) {
        return 2;
    }
    if ( absSpeed < 40.0f ) {
        return 3;
    }
    if ( absSpeed < 56.0f ) {
        return 4;
    }
    if ( absSpeed < 74.0f ) {
        return 5;
    }

    return 6;
}

static float CG_ApproxRemoteVehicleSignedSpeed( centity_t *cent ) {
    vec3_t forward;
    float signedSpeed;

    if ( !cent ) {
        return 0.0f;
    }

    AngleVectors( cent->lerpAngles, forward, NULL, NULL );
    signedSpeed = DotProduct( cent->currentState.pos.trDelta, forward );

    if ( cent->currentState.eFlags & EF_REVERSE ) {
        signedSpeed = -fabs( signedSpeed );
    }

    return signedSpeed;
}

static float CG_EstimateRemoteRpm( float speed, int gear, float wheelSlip ) {
    static const float speedPerGear[] = { 0.0f, 15.0f, 24.0f, 33.0f, 44.0f, 58.0f, 74.0f };
    float absSpeed;
    float rpm;
    float gearNorm;

    if ( gear < 1 ) {
        gear = 1;
    }
    else if ( gear > 6 ) {
        gear = 6;
    }

    absSpeed = fabs( speed );
    gearNorm = absSpeed / speedPerGear[gear];
    rpm = CG_ENGINE_AUDIO_IDLE_RPM + gearNorm * ( CG_ENGINE_AUDIO_REDLINE_RPM - CG_ENGINE_AUDIO_IDLE_RPM ) * 0.55f;
    rpm += wheelSlip * 900.0f;

    if ( rpm < CG_ENGINE_AUDIO_IDLE_RPM ) {
        rpm = CG_ENGINE_AUDIO_IDLE_RPM;
    }
    else if ( rpm > CG_ENGINE_AUDIO_REDLINE_RPM ) {
        rpm = CG_ENGINE_AUDIO_REDLINE_RPM;
    }

    return rpm;
}

static float CG_ApproxVehicleWheelSlip( centity_t *cent ) {
    int i;
    int slipCount;

    if ( !cent ) {
        return 0.0f;
    }

    slipCount = 0;

    if ( cent->currentState.number == cg.predictedPlayerState.clientNum ) {
        for ( i = FL_WHEEL; i <= RR_WHEEL; ++i ) {
            if ( cg.car.sPoints[i].slipping ) {
                ++slipCount;
            }
        }
    }
    else {
        for ( i = 0; i < 4; ++i ) {
            if ( cent->wheelSkidding[i] ) {
                ++slipCount;
            }
        }
    }

    return CG_Clamp01( slipCount / 4.0f );
}

static float CG_ApproxVehicleGroundContact( centity_t *cent ) {
    int i;
    int groundedCount;

    if ( !cent ) {
        return 0.0f;
    }

    groundedCount = 0;

    if ( cent->currentState.number == cg.predictedPlayerState.clientNum ) {
        for ( i = FL_WHEEL; i <= RR_WHEEL; ++i ) {
            if ( cg.car.sPoints[i].onGround ) {
                ++groundedCount;
            }
        }
    }
    else {
        /*
        ** Remote cars do not currently expose full wheel contact state.
        ** Keep their contact assumption conservative and stable.
        */
        groundedCount = 4;
    }

    return CG_Clamp01( groundedCount / 4.0f );
}

static float CG_ApproxVehicleThrottle( centity_t *cent, float rpmNorm ) {
    float throttle;
    float signedSpeed;

    if ( !cent || !cg.snap ) {
        return 0.0f;
    }

    if ( cent->currentState.number == cg.predictedPlayerState.clientNum ) {
        throttle = fabs( cg.car.throttle );
        return CG_Clamp01( throttle );
    }

    signedSpeed = CG_ApproxRemoteVehicleSignedSpeed( cent );
    throttle = 0.35f + 0.45f * rpmNorm;
    throttle += 0.15f * CG_Clamp01( fabs( signedSpeed ) / 80.0f );
    if ( cent->currentState.powerups & ( 1 << PW_TURBO ) ) {
        throttle += 0.15f;
    }

    return CG_Clamp01( throttle );
}

static float CG_ApproxVehicleLoad( centity_t *cent, float throttle, float rpmNorm, float wheelSlip, float speed ) {
    float accelNorm;
    float contactNorm;
    float launchLoad;
    float tractionStrain;
    float rpmRiseNorm;
    float load;

    if ( !cent ) {
        return 0.0f;
    }

    accelNorm = 0.0f;
    contactNorm = CG_ApproxVehicleGroundContact( cent );
    launchLoad = 0.0f;
    tractionStrain = 0.0f;
    rpmRiseNorm = 0.0f;
    if ( cent->currentState.number == cg.predictedPlayerState.clientNum ) {
        accelNorm = CG_Clamp01( fabs( speed - s_cgLastLocalSpeed ) / 120.0f );
        rpmRiseNorm = CG_Clamp01( fabs( rpmNorm - s_cgLastLocalRpmNorm ) * 3.0f );
        launchLoad = throttle * ( 1.0f - CG_Clamp01( fabs( speed ) / 85.0f ) );
        tractionStrain = throttle * ( 1.0f - contactNorm ) + 0.50f * wheelSlip;
        load = 0.06f + 0.48f * throttle + 0.14f * rpmNorm + 0.12f * accelNorm + 0.10f * rpmRiseNorm +
            0.18f * launchLoad + 0.12f * tractionStrain;

        if ( cg.predictedPlayerState.powerups[PW_TURBO] > cg.time ) {
            load += 0.15f;
        }
    }
    else {
        accelNorm = CG_Clamp01( fabs( speed ) / 90.0f );
        load = 0.10f + 0.40f * throttle + 0.20f * rpmNorm + 0.15f * accelNorm;

        if ( cent->currentState.powerups & ( 1 << PW_TURBO ) ) {
            load += 0.10f;
        }
    }

    load += 0.20f * wheelSlip;

    return CG_Clamp01( load );
}

static qboolean CG_IsVehicleEntity( centity_t *cent ) {
    if ( !cent ) {
        return qfalse;
    }

    if ( cent->currentState.eType != ET_PLAYER ) {
        return qfalse;
    }

    return qtrue;
}

static void CG_ComputeVehicleEngineAudioOrigin( centity_t *cent, vec3_t outOrigin ) {
    vec3_t forward;
    vec3_t up;

    if ( !outOrigin ) {
        return;
    }

    if ( !cent ) {
        VectorClear( outOrigin );
        return;
    }

    AngleVectors( cent->lerpAngles, forward, NULL, up );
    VectorCopy( cent->lerpOrigin, outOrigin );

    /*
    ** Bias the emitter toward the rear/lower area of the chassis so the
    ** synthesized sound reads more like an exhaust source in third person.
    */
    VectorMA( outOrigin, -26.0f, forward, outOrigin );
    VectorMA( outOrigin, -6.0f, up, outOrigin );
}

static void CG_ComputeVehicleEngineBayAudioOrigin( centity_t *cent, vec3_t outOrigin ) {
    vec3_t forward;
    vec3_t up;

    if ( !outOrigin ) {
        return;
    }

    if ( !cent ) {
        VectorClear( outOrigin );
        return;
    }

    AngleVectors( cent->lerpAngles, forward, NULL, up );
    VectorCopy( cent->lerpOrigin, outOrigin );
    VectorMA( outOrigin, 18.0f, forward, outOrigin );
    VectorMA( outOrigin, 6.0f, up, outOrigin );
}

void CG_EngineAudio_Init( void ) {
    Com_Memset( &s_cgVehicleAudioDebug, 0, sizeof( s_cgVehicleAudioDebug ) );
    s_cgLastLocalSpeed = 0.0f;
    s_cgLastLocalRpmNorm = 0.0f;
    s_cgLastLocalThrottle = 0.0f;
    s_cgLastLocalGear = 0;
}

void CG_EngineAudio_Shutdown( void ) {
    Com_Memset( &s_cgVehicleAudioDebug, 0, sizeof( s_cgVehicleAudioDebug ) );
    s_cgLastLocalSpeed = 0.0f;
    s_cgLastLocalRpmNorm = 0.0f;
    s_cgLastLocalThrottle = 0.0f;
    s_cgLastLocalGear = 0;
}

qboolean CG_BuildVehicleAudioState( centity_t *cent, vehicleAudioState_t *outState ) {
    float rpm;
    float throttle;
    float load;
    float rpmNorm;
    float wheelSlip;

    if ( !cent || !outState || !cg.snap ) {
        return qfalse;
    }

    if ( !CG_IsVehicleEntity( cent ) ) {
        return qfalse;
    }

    Com_Memset( outState, 0, sizeof( *outState ) );

    if ( cent->currentState.number == cg.predictedPlayerState.clientNum ) {
        rpm = (float)cg.predictedPlayerState.stats[STAT_RPM];
        outState->gear = cg.predictedPlayerState.stats[STAT_GEAR];
        outState->speed = VectorLength( cg.predictedPlayerState.velocity );
    }
    else {
        float signedSpeed;

        signedSpeed = CG_ApproxRemoteVehicleSignedSpeed( cent );
        outState->speed = VectorLength( cent->currentState.pos.trDelta );
        outState->gear = CG_EstimateRemoteGear( cent, signedSpeed );
        rpm = 0.0f;

        if ( cent->currentState.eFlags & EF_REVERSE ) {
            outState->gear = -1;
        }
    }

    wheelSlip = CG_ApproxVehicleWheelSlip( cent );

    if ( cent->currentState.number != cg.predictedPlayerState.clientNum ) {
        if ( outState->gear < 0 ) {
            rpm = CG_EstimateRemoteRpm( -outState->speed, 1, wheelSlip );
        }
        else {
            rpm = CG_EstimateRemoteRpm( outState->speed, outState->gear, wheelSlip );
        }
    }

    rpmNorm = CG_CalcEngineAudioRpmNorm( rpm, CG_ENGINE_AUDIO_IDLE_RPM, CG_ENGINE_AUDIO_REDLINE_RPM );
    throttle = CG_ApproxVehicleThrottle( cent, rpmNorm );
    load = CG_ApproxVehicleLoad( cent, throttle, rpmNorm, wheelSlip, outState->speed );

    outState->rpm = rpm;
    outState->rpmNorm = rpmNorm;
    outState->throttle = throttle;
    outState->load = load;

    outState->clutchSlip = 0.0f;
    outState->wheelSlip = wheelSlip;
    outState->turboBoost = ( cent->currentState.powerups & ( 1 << PW_TURBO ) ) ? 1.0f : 0.0f;

    outState->ignitionCut = qfalse;
    outState->fuelCut = qfalse;
    outState->limiterActive = ( rpm > 7800.0f ) ? qtrue : qfalse;
    outState->backfireEvent = qfalse;
    outState->damaged = qfalse;

    if ( cent->currentState.number == cg.predictedPlayerState.clientNum ) {
        float throttleDrop;
        float maxHealth;
        qboolean shifted;
        qboolean aggressiveLift;
        qboolean loadedUpshift;

        throttleDrop = s_cgLastLocalThrottle - throttle;
        shifted = ( s_cgLastLocalGear != 0 && outState->gear != s_cgLastLocalGear ) ? qtrue : qfalse;
        aggressiveLift = ( throttleDrop > 0.24f && rpmNorm > 0.45f && outState->speed > 14.0f ) ? qtrue : qfalse;
        loadedUpshift = ( shifted && outState->gear > s_cgLastLocalGear && s_cgLastLocalThrottle > 0.45f && rpmNorm > 0.42f ) ? qtrue : qfalse;

        outState->fuelCut = ( throttle < 0.16f && rpmNorm > 0.46f && outState->speed > 12.0f ) ? qtrue : qfalse;
        outState->ignitionCut = ( outState->limiterActive || loadedUpshift ) ? qtrue : qfalse;
        outState->clutchSlip = shifted ? 1.0f : CG_Clamp01( fabs( rpmNorm - s_cgLastLocalRpmNorm ) * 3.2f );
        outState->backfireEvent = ( aggressiveLift || loadedUpshift ) ? qtrue : qfalse;
        maxHealth = (float)max( cg.predictedPlayerState.stats[STAT_MAX_HEALTH], 1 );
        outState->damaged = ( cg.predictedPlayerState.stats[STAT_HEALTH] < (int)( maxHealth * 0.55f ) || cg.car.fuelLeak ) ? qtrue : qfalse;

        s_cgLastLocalSpeed = outState->speed;
        s_cgLastLocalRpmNorm = rpmNorm;
        s_cgLastLocalThrottle = throttle;
        s_cgLastLocalGear = outState->gear;
    }
    else {
        outState->fuelCut = ( throttle < 0.10f && rpmNorm > 0.55f && outState->speed > 20.0f ) ? qtrue : qfalse;
        outState->ignitionCut = outState->limiterActive;
        outState->clutchSlip = CG_Clamp01( wheelSlip * 0.65f + ( outState->fuelCut ? 0.20f : 0.0f ) );
    }

    return qtrue;
}

engineAudioQualityTier_t CG_ChooseEngineAudioQuality( centity_t *cent ) {
    float dist;
    vec3_t delta;

    if ( !cent || !cg.snap ) {
        return EA_QUALITY_OFF;
    }

    if ( cent->currentState.number == cg.predictedPlayerState.clientNum ) {
        return EA_QUALITY_HERO;
    }

    VectorSubtract( cent->lerpOrigin, cg.refdef.vieworg, delta );
    dist = VectorLength( delta );

    if ( dist < 700.0f ) {
        return EA_QUALITY_NEAR;
    }

    if ( dist < 1800.0f ) {
        return EA_QUALITY_FAR;
    }

    return EA_QUALITY_OFF;
}

const cgVehicleAudioDebug_t *CG_GetVehicleAudioDebug( void ) {
    return &s_cgVehicleAudioDebug;
}

static void CG_ProcessVehicleEngineAudio( centity_t *cent ) {
    vehicleAudioState_t state;
    engineAudioQualityTier_t quality;
    vec3_t exhaustOrigin;
    vec3_t engineBayOrigin;

    if ( !cent ) {
        return;
    }

    if ( !CG_BuildVehicleAudioState( cent, &state ) ) {
        trap_S_RemoveEngineEmitter( cent->currentState.number );
        return;
    }

    quality = CG_ChooseEngineAudioQuality( cent );
    if ( quality == EA_QUALITY_OFF ) {
        trap_S_RemoveEngineEmitter( cent->currentState.number );
        return;
    }

    state.exteriorView = ( cent->currentState.number != cg.predictedPlayerState.clientNum || cg.renderingThirdPerson ) ? qtrue : qfalse;
    state.sourceType = EA_SOURCE_EXHAUST;
    CG_ComputeVehicleEngineAudioOrigin( cent, exhaustOrigin );
    CG_ComputeVehicleEngineBayAudioOrigin( cent, engineBayOrigin );
    trap_S_RegisterEngineEmitter( cent->currentState.number, 0 );
    trap_S_UpdateEngineEmitterState(
        cent->currentState.number,
        &state,
        exhaustOrigin,
        engineBayOrigin,
        cent->currentState.pos.trDelta,
        quality );

    if ( !s_cgVehicleAudioDebug.valid ||
         cent->currentState.number == cg.predictedPlayerState.clientNum ) {
        s_cgVehicleAudioDebug.valid = qtrue;
        s_cgVehicleAudioDebug.entityNum = cent->currentState.number;
        s_cgVehicleAudioDebug.quality = quality;
        s_cgVehicleAudioDebug.speed = state.speed;
        s_cgVehicleAudioDebug.rpm = state.rpm;
        s_cgVehicleAudioDebug.rpmNorm = state.rpmNorm;
        s_cgVehicleAudioDebug.throttle = state.throttle;
        s_cgVehicleAudioDebug.load = state.load;
        s_cgVehicleAudioDebug.wheelSlip = state.wheelSlip;
        s_cgVehicleAudioDebug.turboBoost = state.turboBoost;
        s_cgVehicleAudioDebug.limiterActive = state.limiterActive;
        s_cgVehicleAudioDebug.backfireEvent = state.backfireEvent;
        s_cgVehicleAudioDebug.gear = state.gear;
    }
}

void CG_EngineAudio_Frame( void ) {
    int i;
    int localClientNum;

    Com_Memset( &s_cgVehicleAudioDebug, 0, sizeof( s_cgVehicleAudioDebug ) );

    if ( !cg.snap ) {
        return;
    }

    localClientNum = cg.predictedPlayerState.clientNum;

    if ( !cg_engineSounds.integer || cg_engineAudioMode.integer != 2 ) {
        trap_S_RemoveEngineEmitter( localClientNum );

        for ( i = 0; i < cg.snap->numEntities; ++i ) {
            const entityState_t *es = &cg.snap->entities[i];
            trap_S_RemoveEngineEmitter( es->number );
        }
        return;
    }

    if ( localClientNum >= 0 && localClientNum < MAX_CLIENTS ) {
        CG_ProcessVehicleEngineAudio( &cg_entities[localClientNum] );
    }

    for ( i = 0; i < cg.snap->numEntities; ++i ) {
        const entityState_t *es;

        es = &cg.snap->entities[i];
        if ( es->number == localClientNum ) {
            continue;
        }

        CG_ProcessVehicleEngineAudio( &cg_entities[es->number] );
    }
}
