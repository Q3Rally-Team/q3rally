/*
===========================================================================
Copyright (C) 2026 Q3Rally Team

This file is part of Q3Rally source code.

Q3Rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.
===========================================================================
*/
// q_engine_sound.h -- parameters shared between cgame and the engine sound mixer

#ifndef Q_ENGINE_SOUND_H
#define Q_ENGINE_SOUND_H

// engineSoundParams_t.flags
#define ENGINE_SOUND_LOCAL		0x0001	// the listener's own car: not spatialized
#define ENGINE_SOUND_LIMITER	0x0002
#define ENGINE_SOUND_SHIFTING	0x0004

// engineSoundParams_t.rank: how many voices the mixer spends on a car
#define ENGINE_RANK_FULL		0		// own car: full model
#define ENGINE_RANK_NEAR		1		// nearest opponents: both neighbouring loops
#define ENGINE_RANK_FAR			2		// further opponents: one loop

#define ENGINE_MAX_EMITTERS		16		// cars the mixer can play at once, incl. fading ones

// volume groups a sound effect can be tagged with (trap_S_SetSfxGroup)
#define SOUND_GROUP_EFFECTS		0		// s_volume only
#define SOUND_GROUP_AMBIENT		1		// map speakers and map ambience track: s_ambientVolume
#define SOUND_GROUP_WEATHER		2		// rain, snow, wind: s_weatherVolume

// Passed from cgame to the client every frame for every audible car.
// Layout is shared with the QVM: only 4-byte members.
typedef struct {
	float	rpm;			// engine rpm (debug output only)
	float	rpmFrac;		// 0 = idle rpm, 1 = max rpm
	float	load;			// 0 = off throttle, 1 = full throttle
	int		gear;
	int		flags;			// ENGINE_SOUND_*
	vec3_t	origin;
	vec3_t	velocity;
	int		rank;			// ENGINE_RANK_*
	float	doppler;		// pitch factor from relative motion, 1 = none
	float	volume;			// extra volume factor (ignition cut while shifting), 1 = normal
} engineSoundParams_t;

#endif
