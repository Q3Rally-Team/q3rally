/*
===========================================================================
  snd_engine_dsp.c

  Procedural engine DSP MVP with pulse excitation, resonator coloring,
  intake noise, and a simple mechanical harmonic layer.
===========================================================================
*/

#include <math.h>

#include "snd_engine_dsp.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float S_Clamp01( float value ) {
    if ( value < 0.0f ) {
        return 0.0f;
    }

    if ( value > 1.0f ) {
        return 1.0f;
    }

    return value;
}

static float S_EngineDSP_NextNoise( engineAudioSynthState_t *state ) {
    float noise;

    state->noiseSeed = state->noiseSeed * 1664525u + 1013904223u;
    noise = ( ( ( state->noiseSeed >> 8 ) & 0xFFFFu ) / 32768.0f ) - 1.0f;

    return noise;
}

static float S_EngineDSP_ApplyResonator(
    engineAudioResonatorState_t *state,
    const engineAudioResonator_t *resonator,
    float sampleRate,
    float input ) {
    float omega;
    float sinOmega;
    float cosOmega;
    float alpha;
    float q;
    float b0;
    float b1;
    float b2;
    float a0;
    float a1;
    float a2;
    float output;

    if ( !state || !resonator || sampleRate <= 0.0f || resonator->frequencyHz <= 0.0f ) {
        return input;
    }

    q = ( resonator->q > 0.1f ) ? resonator->q : 0.1f;
    omega = 2.0f * (float)M_PI * resonator->frequencyHz / sampleRate;
    sinOmega = sinf( omega );
    cosOmega = cosf( omega );
    alpha = sinOmega / ( 2.0f * q );

    b0 = alpha;
    b1 = 0.0f;
    b2 = -alpha;
    a0 = 1.0f + alpha;
    a1 = -2.0f * cosOmega;
    a2 = 1.0f - alpha;

    output = ( b0 / a0 ) * input +
        ( b1 / a0 ) * state->biquad.x1 +
        ( b2 / a0 ) * state->biquad.x2 -
        ( a1 / a0 ) * state->biquad.y1 -
        ( a2 / a0 ) * state->biquad.y2;

    state->biquad.x2 = state->biquad.x1;
    state->biquad.x1 = input;
    state->biquad.y2 = state->biquad.y1;
    state->biquad.y1 = output;

    return output * resonator->gain;
}

static float S_EngineDSP_RenderResonatorBank(
    engineAudioResonatorState_t *states,
    const engineAudioResonator_t *resonators,
    int resonatorCount,
    float sampleRate,
    float input ) {
    int i;
    float sum;

    if ( !states || !resonators || resonatorCount <= 0 ) {
        return input;
    }

    sum = 0.0f;
    for ( i = 0; i < resonatorCount; ++i ) {
        sum += S_EngineDSP_ApplyResonator( &states[i], &resonators[i], sampleRate, input );
    }

    return sum;
}

static float S_EngineDSP_ComputeLowpassAlpha( float cutoffHz, float sampleRate ) {
    float normalizedCutoff;

    if ( sampleRate <= 0.0f || cutoffHz <= 0.0f ) {
        return 1.0f;
    }

    normalizedCutoff = 2.0f * (float)M_PI * cutoffHz / sampleRate;
    if ( normalizedCutoff > 1.0f ) {
        normalizedCutoff = 1.0f;
    }
    else if ( normalizedCutoff < 0.0f ) {
        normalizedCutoff = 0.0f;
    }

    return normalizedCutoff;
}

void S_EngineDSP_Reset( engineAudioSynthState_t *state, float sampleRate ) {
    if ( !state ) {
        return;
    }

    Com_Memset( state, 0, sizeof( *state ) );
    state->sampleRate = ( sampleRate > 0.0f ) ? sampleRate : 44100.0f;
    state->noiseSeed = 1u;
}

void S_EngineDSP_TriggerBackfire( engineAudioSynthState_t *state ) {
    if ( state ) {
        state->backfireEnvelope = 1.0f;
    }
}

void S_EngineDSP_RenderVehicle(
    engineAudioSynthState_t *synth,
    const engineAudioPreset_t *preset,
    const vehicleAudioState_t *control,
    engineAudioQualityTier_t quality,
    int sampleCount,
    float *outExhaustLeft,
    float *outExhaustRight,
    float *outEngineBayLeft,
    float *outEngineBayRight ) {
    int i;
    int harmonicCount;
    float sr;
    float rpm;
    float rpmNorm;
    float throttle;
    float load;
    float wheelSlip;
    float turboBoost;
    float cylPerRev;
    float pulseHz;
    float phaseStep;
    float gainScale;
    float exhaustLayerScale;
    float intakeLayerScale;
    float mechanicalLayerScale;
    float transmissionLayerScale;
    float exhaustBankMix;
    float intakeBankMix;
    float stereoWidth;
    float outputMakeupGain;
    float cockpitLowpassAlpha;
    float toneLowpassAlpha[2];
    float exhaustSourceGain[2];
    float intakeSourceGain[2];
    float mechanicalSourceGain[2];
    float transmissionSourceGain[2];
    float eventSourceGain[2];
    qboolean cockpitView;
    qboolean exteriorView;

    if ( !synth || !preset || !control || !outExhaustLeft || !outExhaustRight || !outEngineBayLeft || !outEngineBayRight || sampleCount <= 0 ) {
        return;
    }

    sr = ( synth->sampleRate > 0.0f ) ? synth->sampleRate : 44100.0f;

    synth->smoothedRpm += ( control->rpm - synth->smoothedRpm ) * 0.05f;
    synth->smoothedThrottle += ( control->throttle - synth->smoothedThrottle ) * 0.05f;
    synth->smoothedLoad += ( control->load - synth->smoothedLoad ) * 0.05f;

    rpm = synth->smoothedRpm;
    rpmNorm = S_Clamp01( control->rpmNorm );
    throttle = S_Clamp01( synth->smoothedThrottle );
    load = S_Clamp01( synth->smoothedLoad );
    wheelSlip = S_Clamp01( control->wheelSlip );
    turboBoost = S_Clamp01( control->turboBoost );

    cylPerRev = 0.5f * (float)max( preset->cylinderCount, 1 );
    if ( preset->strokeCycle == 2 ) {
        cylPerRev = (float)max( preset->cylinderCount, 1 );
    }

    pulseHz = max( 12.0f, ( rpm / 60.0f ) * cylPerRev );
    phaseStep = pulseHz / sr;

    gainScale = 0.45f;
    exhaustLayerScale = s_engineAudioExhaustGainScale ? s_engineAudioExhaustGainScale->value : 1.0f;
    intakeLayerScale = s_engineAudioIntakeGainScale ? s_engineAudioIntakeGainScale->value : 1.0f;
    mechanicalLayerScale = s_engineAudioMechanicalGainScale ? s_engineAudioMechanicalGainScale->value : 1.0f;
    transmissionLayerScale = s_engineAudioTransmissionGainScale ? s_engineAudioTransmissionGainScale->value : 1.0f;
    exhaustBankMix = 0.97f;
    intakeBankMix = 0.30f;
    stereoWidth = 0.08f;
    outputMakeupGain = 15.0f;

    if ( quality == EA_QUALITY_FAR ) {
        gainScale *= 0.35f;
        exhaustBankMix = 0.35f;
        intakeBankMix = 0.15f;
        stereoWidth = 0.02f;
    }
    else if ( quality == EA_QUALITY_NEAR ) {
        gainScale *= 0.60f;
        exhaustBankMix = 0.55f;
        intakeBankMix = 0.35f;
        stereoWidth = 0.04f;
    }

    harmonicCount = MAX_ENGINE_AUDIO_HARMONICS;
    if ( quality != EA_QUALITY_HERO ) {
        harmonicCount = 4;
    }

    exteriorView = control->exteriorView;
    cockpitView = ( quality == EA_QUALITY_HERO && !exteriorView ) ? qtrue : qfalse;
    cockpitLowpassAlpha = S_EngineDSP_ComputeLowpassAlpha( preset->cockpitLowpassHz, sr );
    toneLowpassAlpha[EA_SOURCE_EXHAUST] = S_EngineDSP_ComputeLowpassAlpha( 3400.0f, sr );
    toneLowpassAlpha[EA_SOURCE_ENGINE_BAY] = S_EngineDSP_ComputeLowpassAlpha( 2500.0f, sr );
    exhaustSourceGain[EA_SOURCE_EXHAUST] = 1.22f * ( s_engineAudioExhaustSourceGainScale ? s_engineAudioExhaustSourceGainScale->value : 1.0f );
    intakeSourceGain[EA_SOURCE_EXHAUST] = 0.24f;
    mechanicalSourceGain[EA_SOURCE_EXHAUST] = 0.46f;
    transmissionSourceGain[EA_SOURCE_EXHAUST] = 0.86f * ( 0.90f + 0.10f * ( s_engineAudioExhaustSourceGainScale ? s_engineAudioExhaustSourceGainScale->value : 1.0f ) );
    eventSourceGain[EA_SOURCE_EXHAUST] = 1.18f * ( s_engineAudioExhaustEventGainScale ? s_engineAudioExhaustEventGainScale->value : 1.0f );
    exhaustSourceGain[EA_SOURCE_ENGINE_BAY] = 0.16f;
    intakeSourceGain[EA_SOURCE_ENGINE_BAY] = 0.82f * ( s_engineAudioEngineBaySourceGainScale ? s_engineAudioEngineBaySourceGainScale->value : 1.0f );
    mechanicalSourceGain[EA_SOURCE_ENGINE_BAY] = 0.84f * ( s_engineAudioEngineBaySourceGainScale ? s_engineAudioEngineBaySourceGainScale->value : 1.0f );
    transmissionSourceGain[EA_SOURCE_ENGINE_BAY] = 0.82f * ( 0.85f + 0.15f * ( s_engineAudioEngineBaySourceGainScale ? s_engineAudioEngineBaySourceGainScale->value : 1.0f ) );
    eventSourceGain[EA_SOURCE_ENGINE_BAY] = 0.58f * ( s_engineAudioEngineBayEventGainScale ? s_engineAudioEngineBayEventGainScale->value : 1.0f );

    for ( i = 0; i < sampleCount; ++i ) {
        int h;
        float pulseWrap;
        float pulseShape;
        float combustionPhase;
        float secondaryPhase;
        float idleBurbleStrength;
        float overrunBurbleStrength;
        float burbleStrength;
        float drivenHarshness;
        float tonalWhineSuppression;
        float brightnessDamping;
        float exhaustColor;
        float intakeNoise;
        float intakeColor;
        float mechanical;
        float transmission;
        float limiterBuzz;
        float backfire;
        float overrunPop;
        float fuelCutCrackle;
        float ignitionCutChatter;
        float shiftChirp;
        float damageRattle;
        float slipNoise;
        float body[2];
        float left[2];
        float right[2];
        float widthPhase;
        float noise;

        idleBurbleStrength = S_Clamp01( ( 0.24f - rpmNorm ) * 5.0f ) * S_Clamp01( ( 0.42f - throttle ) * 2.4f );
        overrunBurbleStrength = S_Clamp01( ( rpmNorm - 0.18f ) * 1.6f ) * S_Clamp01( ( 0.24f - throttle ) * 4.0f );
        burbleStrength = idleBurbleStrength;
        if ( overrunBurbleStrength > burbleStrength ) {
            burbleStrength = overrunBurbleStrength;
        }
        drivenHarshness = S_Clamp01( ( rpmNorm - 0.32f ) * 1.8f ) * S_Clamp01( ( load - 0.45f ) * 1.8f );
        tonalWhineSuppression = S_Clamp01( ( rpmNorm - 0.08f ) * 1.4f );
        brightnessDamping = S_Clamp01( ( rpmNorm - 0.42f ) * 1.5f );
        synth->crankPhase += phaseStep;
        synth->combustionPhase += phaseStep;
        pulseWrap = floorf( synth->crankPhase );
        if ( pulseWrap > 0.0f ) {
            int firedCount;

            synth->crankPhase -= pulseWrap;
            for ( firedCount = 0; firedCount < (int)pulseWrap; ++firedCount ) {
                int firingOrderLength;
                int cylinderId;
                float cylinderNorm;

                firingOrderLength = ( preset->firingOrderLength > 0 ) ? preset->firingOrderLength : 1;
                cylinderId = ( preset->firingOrderLength > 0 ) ?
                    preset->firingOrder[synth->firingOrderIndex % firingOrderLength] :
                    ( synth->firingOrderIndex + 1 );
                cylinderNorm = (float)cylinderId / (float)firingOrderLength;

                synth->combustionPhase = 0.0f;
                synth->combustionJitter = 0.78f + 0.10f * cylinderNorm + 0.18f * burbleStrength +
                    0.14f * ( 0.5f + 0.5f * S_EngineDSP_NextNoise( synth ) );
                synth->combustionTone = 0.92f + 0.12f * cylinderNorm - 0.08f * burbleStrength;
                synth->combustionNoise = ( 0.08f + 0.18f * burbleStrength ) * S_EngineDSP_NextNoise( synth );
                if ( overrunBurbleStrength > 0.12f && S_EngineDSP_NextNoise( synth ) > 0.72f ) {
                    synth->overrunPopEnvelope = 0.45f + 0.40f * overrunBurbleStrength;
                }
                synth->firingOrderIndex = ( synth->firingOrderIndex + 1 ) % firingOrderLength;
            }
        }

        combustionPhase = synth->combustionPhase * synth->combustionTone;
        secondaryPhase = combustionPhase - ( 0.12f + 0.04f * synth->combustionJitter );
        if ( secondaryPhase < 0.0f ) {
            secondaryPhase = 0.0f;
        }

        pulseShape = expf( -13.0f * combustionPhase ) - expf( -54.0f * combustionPhase );
        pulseShape += 0.32f * ( expf( -18.0f * secondaryPhase ) - expf( -72.0f * secondaryPhase ) );
        pulseShape += burbleStrength * 0.26f * ( expf( -8.0f * secondaryPhase ) - expf( -28.0f * secondaryPhase ) );
        pulseShape += synth->combustionNoise * ( 1.0f + 0.8f * burbleStrength ) * expf( -18.0f * combustionPhase );
        pulseShape += ( 0.04f + 0.10f * tonalWhineSuppression ) * throttle * load * S_EngineDSP_NextNoise( synth ) * expf( -16.0f * combustionPhase );
        pulseShape *= synth->combustionJitter * ( 0.45f + 0.55f * throttle ) * ( 0.30f + 0.70f * load );

        exhaustColor = pulseShape;
        if ( preset->exhaustResonatorCount > 0 ) {
            exhaustColor = S_EngineDSP_RenderResonatorBank(
                synth->exhaustStates,
                preset->exhaustResonators,
                preset->exhaustResonatorCount,
                sr,
                pulseShape );
            exhaustColor = pulseShape * ( 1.0f - exhaustBankMix ) + exhaustColor * exhaustBankMix;
        }
        exhaustColor += drivenHarshness * 0.24f * ( expf( -7.0f * combustionPhase ) - expf( -24.0f * combustionPhase ) );
        exhaustColor *= 1.0f + 0.30f * tonalWhineSuppression * load;
        if ( exteriorView ) {
            exhaustColor *= 1.12f + 0.24f * load;
        }
        noise = S_EngineDSP_NextNoise( synth );
        intakeNoise = noise * preset->noiseGain * 6.5f * ( 0.15f + 0.85f * throttle ) * ( 0.30f + 0.70f * load ) * ( 1.0f - 0.30f * rpmNorm ) * ( 1.0f - 0.55f * drivenHarshness );
        intakeNoise += noise * wheelSlip * 0.25f;
        if ( exteriorView ) {
            intakeNoise *= 0.82f;
        }
        intakeColor = intakeNoise;
        if ( preset->intakeResonatorCount > 0 ) {
            intakeColor = S_EngineDSP_RenderResonatorBank(
                synth->intakeStates,
                preset->intakeResonators,
                preset->intakeResonatorCount,
                sr,
                intakeNoise );
            intakeColor = intakeNoise * ( 1.0f - intakeBankMix ) + intakeColor * intakeBankMix;
        }

        mechanical = 0.0f;
        for ( h = 0; h < harmonicCount; ++h ) {
            float harmonicHz;
            float harmonicAmp;

            harmonicAmp = preset->harmonicGains[h];
            harmonicAmp *= 1.0f - tonalWhineSuppression * ( 0.35f + 0.12f * (float)h );
            harmonicAmp *= 1.0f - brightnessDamping * ( 0.08f + 0.10f * (float)h );
            if ( harmonicAmp <= 0.0f ) {
                continue;
            }

            harmonicHz = ( rpm / 60.0f ) * (float)( h + 1 );
            synth->harmonicPhase[h] += ( 2.0f * (float)M_PI * harmonicHz ) / sr;
            if ( synth->harmonicPhase[h] > 2.0f * (float)M_PI ) {
                synth->harmonicPhase[h] -= 2.0f * (float)M_PI;
            }

            mechanical += sinf( synth->harmonicPhase[h] ) * harmonicAmp;
        }
        mechanical *= ( 0.05f + 0.04f * rpmNorm ) * ( 1.0f - 0.35f * rpmNorm ) * ( 1.0f - 0.65f * drivenHarshness ) * ( 1.0f - 0.75f * tonalWhineSuppression );

        synth->phase += ( 2.0f * (float)M_PI * ( rpm / 60.0f ) ) / sr;
        if ( synth->phase > 2.0f * (float)M_PI ) {
            synth->phase -= 2.0f * (float)M_PI;
        }

        transmission = sinf( synth->phase * 1.5f ) * ( 0.005f + 0.010f * rpmNorm + 0.020f * turboBoost ) * ( 1.0f - 0.50f * rpmNorm ) * ( 1.0f - 0.75f * drivenHarshness ) * ( 1.0f - 0.85f * tonalWhineSuppression );
        slipNoise = noise * preset->noiseGain * wheelSlip * ( 0.03f + 0.05f * load );
        if ( exteriorView ) {
            mechanical *= 0.72f;
            transmission *= 0.58f;
        }

        if ( control->limiterActive ) {
            synth->limiterEnvelope += ( 1.0f - synth->limiterEnvelope ) * 0.12f;
        }
        else {
            synth->limiterEnvelope *= 0.92f;
        }

        limiterBuzz = 0.0f;
        if ( !s_engineAudioLimiterEnable || s_engineAudioLimiterEnable->integer ) {
            limiterBuzz = noise * synth->limiterEnvelope * preset->limiterGain * 0.08f;
        }

        backfire = 0.0f;
        if ( synth->backfireEnvelope > 0.001f ) {
            if ( !s_engineAudioBackfireEnable || s_engineAudioBackfireEnable->integer ) {
                backfire = S_EngineDSP_NextNoise( synth ) * synth->backfireEnvelope * preset->backfireGain * 0.12f;
            }
            synth->backfireEnvelope *= 0.965f;
        }

        overrunPop = 0.0f;
        if ( synth->overrunPopEnvelope > 0.001f ) {
            overrunPop = S_EngineDSP_NextNoise( synth ) * synth->overrunPopEnvelope * ( 0.10f + 0.08f * overrunBurbleStrength );
            synth->overrunPopEnvelope *= 0.93f;
        }

        fuelCutCrackle = 0.0f;
        if ( control->fuelCut ) {
            fuelCutCrackle = S_EngineDSP_NextNoise( synth ) * ( 0.025f + 0.060f * rpmNorm ) *
                ( 0.55f + 0.45f * load ) * ( exteriorView ? 1.35f : 0.85f );
            fuelCutCrackle += expf( -12.0f * combustionPhase ) * ( 0.015f + 0.020f * overrunBurbleStrength );
        }

        ignitionCutChatter = 0.0f;
        if ( control->ignitionCut ) {
            ignitionCutChatter = noise * preset->limiterGain * ( 0.030f + 0.030f * rpmNorm );
            if ( exteriorView ) {
                ignitionCutChatter *= 1.20f;
            }
        }

        shiftChirp = 0.0f;
        if ( control->clutchSlip > 0.05f ) {
            shiftChirp = noise * preset->noiseGain * control->clutchSlip * ( 0.085f + 0.050f * load );
            if ( exteriorView ) {
                shiftChirp *= 1.35f;
            }
        }

        damageRattle = 0.0f;
        if ( control->damaged ) {
            damageRattle = S_EngineDSP_NextNoise( synth ) * ( 0.020f + 0.030f * rpmNorm ) * ( exteriorView ? 1.30f : 0.80f );
            exhaustColor *= exteriorView ? 1.10f : 1.05f;
            mechanical *= 1.18f;
        }

        for ( h = 0; h < 2; ++h ) {
            int sourceIndex = h;
            float sourceBrightnessDamping;
            float sourceExhaustColor;
            float sourceIntakeColor;
            float sourceMechanical;
            float sourceTransmission;
            float sourceFuelCutCrackle;
            float sourceIgnitionCutChatter;
            float sourceShiftChirp;
            float sourceDamageRattle;
            float mono;

            sourceBrightnessDamping = brightnessDamping;
            if ( sourceIndex == EA_SOURCE_ENGINE_BAY ) {
                sourceBrightnessDamping = S_Clamp01( sourceBrightnessDamping * 1.20f );
            }

            sourceExhaustColor = exhaustColor * exhaustSourceGain[sourceIndex];
            sourceIntakeColor = intakeColor * ( 1.0f - 0.38f * sourceBrightnessDamping ) * intakeSourceGain[sourceIndex];
            sourceMechanical = mechanical * mechanicalSourceGain[sourceIndex];
            sourceTransmission = transmission * transmissionSourceGain[sourceIndex];
            sourceFuelCutCrackle = fuelCutCrackle * eventSourceGain[sourceIndex];
            sourceIgnitionCutChatter = ignitionCutChatter * eventSourceGain[sourceIndex];
            sourceShiftChirp = shiftChirp * eventSourceGain[sourceIndex];
            sourceDamageRattle = damageRattle * ( 0.65f * eventSourceGain[sourceIndex] + 0.35f * mechanicalSourceGain[sourceIndex] );

            body[sourceIndex] =
                sourceExhaustColor * preset->exhaustGain * exhaustLayerScale * 0.55f +
                sourceIntakeColor * preset->intakeGain * intakeLayerScale * 0.35f +
                sourceMechanical * preset->mechanicalGain * mechanicalLayerScale * 0.40f +
                sourceTransmission * preset->transmissionGain * transmissionLayerScale * 0.30f +
                slipNoise + limiterBuzz + backfire + overrunPop +
                sourceFuelCutCrackle + sourceIgnitionCutChatter + sourceShiftChirp + sourceDamageRattle;

            if ( exteriorView ) {
                body[sourceIndex] += sourceExhaustColor * preset->exhaustGain * exhaustLayerScale * ( 0.18f + 0.10f * load );
            }
            if ( control->fuelCut ) {
                body[sourceIndex] += sourceExhaustColor * preset->exhaustGain * exhaustLayerScale * ( exteriorView ? 0.08f : 0.04f );
            }
            if ( control->ignitionCut ) {
                body[sourceIndex] *= exteriorView ? 0.94f : 0.90f;
            }

            body[sourceIndex] *= gainScale * ( s_engineAudioGain ? s_engineAudioGain->value : 1.0f ) *
                ( 0.55f + 0.45f * preset->exteriorPresenceGain );
            body[sourceIndex] *= 0.55f + 0.45f * ( 0.25f + 0.75f * load );

            if ( preset->distortionDrive > 0.0f ) {
                body[sourceIndex] = tanhf( body[sourceIndex] * ( 1.0f + preset->distortionDrive * 4.0f ) );
            }

            widthPhase = sinf( synth->phase * 0.5f );
            left[sourceIndex] = body[sourceIndex] * ( 1.0f - stereoWidth * widthPhase );
            right[sourceIndex] = body[sourceIndex] * ( 1.0f + stereoWidth * widthPhase );

            synth->toneLowpassL[sourceIndex] += ( left[sourceIndex] - synth->toneLowpassL[sourceIndex] ) * toneLowpassAlpha[sourceIndex];
            synth->toneLowpassR[sourceIndex] += ( right[sourceIndex] - synth->toneLowpassR[sourceIndex] ) * toneLowpassAlpha[sourceIndex];
            left[sourceIndex] = left[sourceIndex] * ( 1.0f - 0.55f * sourceBrightnessDamping ) + synth->toneLowpassL[sourceIndex] * ( 0.55f * sourceBrightnessDamping );
            right[sourceIndex] = right[sourceIndex] * ( 1.0f - 0.55f * sourceBrightnessDamping ) + synth->toneLowpassR[sourceIndex] * ( 0.55f * sourceBrightnessDamping );

            if ( cockpitView && sourceIndex == EA_SOURCE_ENGINE_BAY &&
                 ( !s_engineAudioCockpitEnable || s_engineAudioCockpitEnable->integer ) ) {
                mono = 0.5f * ( left[sourceIndex] + right[sourceIndex] );
                left[sourceIndex] = left[sourceIndex] * 0.25f + mono * 0.75f;
                right[sourceIndex] = right[sourceIndex] * 0.25f + mono * 0.75f;

                synth->cockpitLowpassL[sourceIndex] += ( left[sourceIndex] - synth->cockpitLowpassL[sourceIndex] ) * cockpitLowpassAlpha;
                synth->cockpitLowpassR[sourceIndex] += ( right[sourceIndex] - synth->cockpitLowpassR[sourceIndex] ) * cockpitLowpassAlpha;

                left[sourceIndex] = synth->cockpitLowpassL[sourceIndex];
                right[sourceIndex] = synth->cockpitLowpassR[sourceIndex];
            }

            left[sourceIndex] = tanhf( left[sourceIndex] * outputMakeupGain );
            right[sourceIndex] = tanhf( right[sourceIndex] * outputMakeupGain );
        }

        outExhaustLeft[i] += left[EA_SOURCE_EXHAUST];
        outExhaustRight[i] += right[EA_SOURCE_EXHAUST];
        outEngineBayLeft[i] += left[EA_SOURCE_ENGINE_BAY];
        outEngineBayRight[i] += right[EA_SOURCE_ENGINE_BAY];
    }
}
