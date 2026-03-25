#ifndef SND_ENGINE_AUDIO_H
#define SND_ENGINE_AUDIO_H

#include "snd_local.h"
#include "../qcommon/engine_audio_shared.h"

#define MAX_ENGINE_AUDIO_EMITTERS 64
#define MAX_ENGINE_AUDIO_FIRING_ORDER 16
#define MAX_ENGINE_AUDIO_RESONATORS 8
#define MAX_ENGINE_AUDIO_HARMONICS 8

typedef struct engineAudioResonator_s {
    float frequencyHz;
    float q;
    float gain;
} engineAudioResonator_t;

typedef struct engineAudioPreset_s {
    char name[64];

    int cylinderCount;
    int strokeCycle;
    int firingOrder[MAX_ENGINE_AUDIO_FIRING_ORDER];
    int firingOrderLength;

    float idleRpm;
    float redlineRpm;

    float exhaustGain;
    float intakeGain;
    float mechanicalGain;
    float transmissionGain;

    int exhaustResonatorCount;
    engineAudioResonator_t exhaustResonators[MAX_ENGINE_AUDIO_RESONATORS];

    int intakeResonatorCount;
    engineAudioResonator_t intakeResonators[MAX_ENGINE_AUDIO_RESONATORS];

    float harmonicGains[MAX_ENGINE_AUDIO_HARMONICS];

    float distortionDrive;
    float noiseGain;
    float backfireGain;
    float limiterGain;
    float cockpitLowpassHz;
    float exteriorPresenceGain;
} engineAudioPreset_t;

typedef struct engineAudioEmitterPublicState_s {
    int entityNum;
    qboolean active;

    vehicleAudioState_t control;
    engineAudioQualityTier_t quality;

    vec3_t exhaustOrigin;
    vec3_t engineBayOrigin;
    vec3_t velocity;

    const engineAudioPreset_t *preset;
} engineAudioEmitterPublicState_t;

void S_EngineAudio_Init( void );
void S_EngineAudio_Shutdown( void );
void S_EngineAudio_BeginFrame( void );
int S_RegisterEngineAudioPreset( const engineAudioPreset_t *preset );
int S_FindEngineAudioPreset( const char *name );
void S_RegisterEngineEmitter( int entityNum, int presetHandle );
void S_RemoveEngineEmitter( int entityNum );
void S_UpdateEngineEmitterState(
    int entityNum,
    const vehicleAudioState_t *state,
    const vec3_t exhaustOrigin,
    const vec3_t engineBayOrigin,
    const vec3_t velocity,
    engineAudioQualityTier_t quality );
void S_SetEngineEmitterPreset( int entityNum, int presetHandle );
void S_RenderEngineAudio( portable_samplepair_t *buffer, int sampleCount );
void S_StopAllEngineEmitters( void );

#endif /* SND_ENGINE_AUDIO_H */
