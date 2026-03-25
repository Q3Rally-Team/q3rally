#ifndef SND_ENGINE_DSP_H
#define SND_ENGINE_DSP_H

#include "snd_engine_audio.h"

typedef struct engineAudioBiquadState_s {
    float x1, x2;
    float y1, y2;
} engineAudioBiquadState_t;

typedef struct engineAudioResonatorState_s {
    engineAudioBiquadState_t biquad;
} engineAudioResonatorState_t;

typedef struct engineAudioSynthState_s {
    float sampleRate;

    float phase;
    float crankPhase;
    float combustionPhase;
    float combustionJitter;
    float combustionTone;
    float combustionNoise;
    int firingOrderIndex;

    float smoothedRpm;
    float smoothedThrottle;
    float smoothedLoad;

    float limiterEnvelope;
    float backfireEnvelope;
    float overrunPopEnvelope;

    float cockpitLowpassL[2];
    float cockpitLowpassR[2];
    float toneLowpassL[2];
    float toneLowpassR[2];

    unsigned int noiseSeed;

    engineAudioResonatorState_t exhaustStates[MAX_ENGINE_AUDIO_RESONATORS];
    engineAudioResonatorState_t intakeStates[MAX_ENGINE_AUDIO_RESONATORS];

    float harmonicPhase[MAX_ENGINE_AUDIO_HARMONICS];
} engineAudioSynthState_t;

void S_EngineDSP_Reset( engineAudioSynthState_t *state, float sampleRate );
void S_EngineDSP_RenderVehicle(
    engineAudioSynthState_t *synth,
    const engineAudioPreset_t *preset,
    const vehicleAudioState_t *control,
    engineAudioQualityTier_t quality,
    int sampleCount,
    float *outExhaustLeft,
    float *outExhaustRight,
    float *outEngineBayLeft,
    float *outEngineBayRight );
void S_EngineDSP_TriggerBackfire( engineAudioSynthState_t *state );

#endif /* SND_ENGINE_DSP_H */
