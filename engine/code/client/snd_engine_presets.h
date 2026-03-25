#ifndef SND_ENGINE_PRESETS_H
#define SND_ENGINE_PRESETS_H

#include "snd_engine_audio.h"

void S_LoadEngineAudioPresets( void );
qboolean S_ParseEngineAudioPresetFile( const char *path, engineAudioPreset_t *outPreset );
qboolean S_ValidateEngineAudioPreset( const engineAudioPreset_t *preset );
const engineAudioPreset_t *S_GetEngineAudioPresetByHandle( int handle );

#endif /* SND_ENGINE_PRESETS_H */
