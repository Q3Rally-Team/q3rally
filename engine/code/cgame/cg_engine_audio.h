#ifndef CG_ENGINE_AUDIO_H
#define CG_ENGINE_AUDIO_H

#include "cg_local.h"
#include "../qcommon/engine_audio_shared.h"

typedef struct cgVehicleAudioDebug_s {
    qboolean valid;
    int entityNum;
    engineAudioQualityTier_t quality;

    float speed;
    float rpm;
    float rpmNorm;
    float throttle;
    float load;
    float wheelSlip;
    float turboBoost;
    qboolean limiterActive;
    qboolean backfireEvent;
    int gear;
} cgVehicleAudioDebug_t;

void CG_EngineAudio_Init( void );
void CG_EngineAudio_Shutdown( void );
void CG_EngineAudio_Frame( void );
qboolean CG_BuildVehicleAudioState( centity_t *cent, vehicleAudioState_t *outState );
engineAudioQualityTier_t CG_ChooseEngineAudioQuality( centity_t *cent );
const cgVehicleAudioDebug_t *CG_GetVehicleAudioDebug( void );

#endif /* CG_ENGINE_AUDIO_H */
