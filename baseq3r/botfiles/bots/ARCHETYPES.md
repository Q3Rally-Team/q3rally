# Bot Personality Archetypes

Diese Zielwerte gelten als **Identitäts-Baseline (skill 4)** und werden über
`baseq3r/botfiles/bots/tune_character_profiles.py` in alle `*_c.c`-Dateien
übertragen.

| Archetype | Aggression | SelfPreservation | Camper | Reaction | FireThrottle |
|---|---:|---:|---:|---:|---:|
| berserker | 0.92 | 0.28 | 0.05 | 0.45 | 0.16 |
| hunter | 0.68 | 0.52 | 0.18 | 0.58 | 0.22 |
| tactician | 0.56 | 0.78 | 0.26 | 0.66 | 0.30 |
| ambusher | 0.34 | 0.84 | 0.72 | 0.76 | 0.38 |
| juggernaut | 0.86 | 0.74 | 0.30 | 0.60 | 0.20 |
| trickster | 0.62 | 0.40 | 0.34 | 0.54 | 0.25 |
| balanced | 0.60 | 0.60 | 0.20 | 0.62 | 0.24 |

## Skill-Skalierung

Skill beeinflusst nur Leistungsniveau, nicht die Persona:

- `Aggression`, `SelfPreservation`, `Camper` bleiben pro Bot konstant.
- `Reaction` und `FireThrottle` werden skaliert:
  - skill 1: Reaktion `x1.45`, FireThrottle `x1.25`
  - skill 4: Baseline `x1.00`
  - skill 5: Reaktion `x0.55`, FireThrottle `x0.80`

## AI-Datei-Entkopplung

`default_c.c` nutzt nun eigene AI-Referenzen statt Reuse:

- `bots/default_w.c`
- `bots/default_i.c`
- `bots/default_t.c`

(statt `gargoyle_*`).
