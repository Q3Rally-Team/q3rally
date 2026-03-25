# Q3Rally Procedural Engine Audio System

## Status
Draft / implementation proposal.

## Goal
Replace or augment Q3Rally's current sample-loop-based engine sound system with a procedural, synthesizer-driven engine audio pipeline inspired by `engine-sim`.

## Overview
The current Q3Rally engine sound path derives engine audio from a small set of prerecorded loops indexed by RPM range and shaped with pitch scaling. A procedural system should instead derive the sound from vehicle control state and a small real-time synthesis graph.

### Design goals
- synthesize engine sound in real time
- be inspired by `engine-sim`, but adapted for a game runtime budget
- integrate cleanly into the existing client sound architecture
- support multiplayer efficiently
- scale quality by listener relevance and distance
- stay data-driven through per-vehicle presets
- preserve the classic loop system as fallback during rollout

## Current Q3Rally baseline
Useful existing ingredients:
- vehicle RPM and gear already reach the client
- the client sound layer already supports looping sounds and per-entity pitch
- local predicted state can drive a high-quality "hero" engine path

Current limitations:
- discrete loop transitions remain audible
- timbre is weakly coupled to throttle/load and transient events
- vehicle identity is limited by the loop set
- extending the system requires more samples rather than more logic

## Proposed architecture
The proposed system has four layers:

1. **Vehicle Audio Control Layer**  
   Collects vehicle state relevant for sound.
2. **Engine Synthesis Layer**  
   Converts control state into excitation signals and resonant sound layers.
3. **Emitter and Mixing Layer**  
   Maintains one shared synth per vehicle, renders the combustion model once, then splits it into exhaust and engine-bay buses before mixing.
4. **Spatialization and Playback Layer**  
   Routes synthesized audio through the existing sound backend and 3D positioning system.

## Vehicle audio control state
Suggested runtime inputs:
- rpm
- normalized rpm
- throttle
- load
- speed
- clutch slip
- wheel slip
- turbo boost
- gear
- ignition cut / fuel cut / limiter flags
- backfire event
- damage state

The multiplayer model should transmit or reconstruct these control values, not audio.

## Audio model
The first version should use a perceptual hybrid model instead of a full physical simulation.

### Main signal paths
1. combustion pulse / excitation generator
2. exhaust resonance path
3. intake resonance path
4. mechanical / engine block path
5. transmission / accessory path
6. transient event path

### MVP synthesis recommendation
For the MVP, use:
- RPM-driven pulse or tonal base
- simple resonator bank for exhaust coloration
- throttle/load-driven noise for intake
- mechanical harmonic layer
- transient envelopes for limiter and backfire
- conservative stereo mix into the existing sound path
- shared-vehicle render with rear exhaust and front engine-bay spatial outputs

## Quality tiers
Recommended runtime tiers:
- **OFF**: disabled
- **FAR**: cheap tonal/noise approximation
- **NEAR**: reduced resonator bank and simplified synthesis
- **HERO**: full-quality local or focused vehicle path

Priority order:
1. local player vehicle
2. spectated or focused vehicle
3. nearest loud vehicles
4. all others

## Data-driven presets
Vehicle sound should be configured through preset files rather than hardcoded constants.

Suggested preset content:
- cylinder count
- 2-stroke / 4-stroke cycle
- firing order
- idle RPM / redline RPM
- exhaust, intake, mechanical, transmission gains
- exhaust and intake resonators
- harmonic gains
- distortion / noise / backfire / limiter controls
- cockpit and exterior voicing controls

## Integration strategy
### Phase 1
- add procedural emitter registry to the sound backend
- add state update API
- add mixer hook
- add debug cvars

### Phase 2
- local vehicle only
- single preset
- exhaust, intake, mechanical layers with one synth feeding multiple spatial outputs
- hero-quality path only

### Phase 3
- remote vehicles
- quality tiers and prioritization
- preset mapping per vehicle

### Phase 4
- transient effects
- turbo path
- better cockpit/exterior routing
- more realistic resonator modelling

## Recommended repository structure
- `docs/procedural_engine_audio_design.md`
- `engine/code/cgame/cg_engine_audio.h`
- `engine/code/cgame/cg_engine_audio.c`
- `engine/code/client/snd_engine_audio.h`
- `engine/code/client/snd_engine_audio.c`
- `engine/code/client/snd_engine_dsp.h`
- `engine/code/client/snd_engine_dsp.c`
- `engine/code/client/snd_engine_presets.h`
- `engine/code/client/snd_engine_presets.c`

## MVP conclusion
The recommended path is to keep the classic sample-loop engine path as fallback while introducing a new procedural emitter pipeline in parallel. The first implementation should target the local player's vehicle only and prove the architecture before scaling to other vehicles.
