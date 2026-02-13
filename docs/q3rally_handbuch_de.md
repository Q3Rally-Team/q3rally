# Q3Rally Handbook / Q3Rally-Handbuch

**DE:** Dieses zweisprachige Nachschlagewerk fasst den Stand der Q3Rally-Mod zusammen. Es richtet sich an Fahrer*innen, Server-Admins und Mapper und deckt Kartenpool, Fahrzeuggarage, Bewaffnung, Spielmodi, zentrale CVars, Mapping-Entities sowie Erweiterungsideen ab.
**EN:** This bilingual reference captures the current Q3Rally feature set. It supports drivers, server admins, and mappers with details about the map pool, garage lineup, weaponry, game modes, core CVars, mapping entities, and future ideas.

## 1. Map Pool / Kartenpool

**DE:** Die `.arena`-Metadaten liefern Titel, Autor*innen, Modus-Support und Loot-Verteilung. Tabellen listen Rennstrecken sowie Kampf- und Spezialarenen.
**EN:** The `.arena` metadata defines titles, authors, supported modes, and loot layouts. The tables below cover race circuits plus combat and special arenas.

### 1.1 Race Tracks / Rennstrecken

| Map ID | Title (EN/DE) | Author | Supported Modes | Default Laps | Checkpoints | Weapons | Powerups | Elimination Flag |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Race_Bot_Test | Race Bot Test | P3rlE | Racing, Team Racing, Elimination | 3 | 14 | 0 | 0 | 1 |
| q3r_a_to_b_test | A 2 B Test | P3rlE | Racing, Team Racing | 1 | 2 | 0 | 0 | 0 |
| q3r_country01 | Roll On Down The Line | Stecki | Racing, Team Racing, Elimination | 3 | 20 | 0 | 8 | 1 |
| q3r_dirtyfun | Dirty Fun | Stecki & insellium | Racing, Team Racing, Elimination | 3 | 4/5/9 | 0 | 0 | 1 |
| q3r_downtown | DownTown | OliverV | Racing, Team Racing, Elimination | 3 | 25 | 0 | 0 | 1 |
| q3r_lavafalls | Lava Falls | Cyberdemon | Racing, Team Racing, Racing DM, Team Racing DM, Elimination | 3 | 22 | FT/PG/RG/RL | 1 | 1 |
| q3r_nightcity | Night City | Amphetamine | Racing, Team Racing, Racing DM, Team Racing DM, Elimination | 3 | 9 & 12 | GL/RG/RL | 6 | 1 |
| q3r_ridgeracer | Ridge Racer | TheBigBuu & insellium | Racing, Team Racing, Racing DM, Team Racing DM, Elimination | 3 | 19 | 0 | 0 | 1 |
| q3r_sk1 | Canyon Cruise | SystemKrash & insellium | Racing, Team Racing, Elimination | 3 | 14 | 0 | 0 | 1 |
| q3r_valley | Desert Valley | Dipso | Racing, Team Racing, Racing DM, Team Racing DM, Elimination | 3 | 12 | PG/RL/SG | 3 | 1 |

> Quellen / Sources: `.arena`-Dateien für Longname, Autor, Modusliste, Runden, Checkpoints und Loot.【F:baseq3r/scripts/Race_Bot_Test.arena†L3-L40】【F:baseq3r/scripts/q3r_a_to_b_test.arena†L3-L36】【F:baseq3r/scripts/q3r_country01.arena†L3-L45】【F:baseq3r/scripts/q3r_dirtyfun.arena†L3-L39】【F:baseq3r/scripts/q3r_downtown.arena†L3-L39】【F:baseq3r/scripts/q3r_lavafalls.arena†L3-L39】【F:baseq3r/scripts/q3r_nightcity.arena†L3-L39】【F:baseq3r/scripts/q3r_ridgeracer.arena†L3-L39】【F:baseq3r/scripts/q3r_sk1.arena†L3-L39】【F:baseq3r/scripts/q3r_valley.arena†L3-L39】

### 1.2 Combat, Team, and Special Maps / Kampf-, Team- und Spezial-Maps

| Map ID | Title (EN/DE) | Author | Supported Modes | Weapons | Powerups / Notes |
| --- | --- | --- | --- | --- | --- |
| q3r_babel | Babel's Military Base | Marc Fontaine (insellium remix) | Domination | SG/RG/PG/RL/BFG | 0 Powerups |
| q3r_battlecanyon | Battle Canyon | P3rle & insellium | DM, Team DM | RL/PG/RG/SG/LG | 2 Powerups + Mines |
| q3r_citydom | Dominate the City | P3rlE | Domination | All | All Powerups |
| q3r_ctf_01 | Capture in the Jungle | P3rlE | CTF | FT/PG/RL/SG/RG | 8 Powerups |
| q3r_ctf4_01 | Flag Frenzy Speedway | P3rlE | 4-Team-CTF | n/a (arena blank) | n/a |
| q3r_david4 | Jungle Opening | |CE & P3rlE | CTF | BFG/GL/FT/RL/LG | 2 Powerups |
| q3r_david5 | Underground Battle Arena | |CE | DM, Team DM, Derby | RL/PG | 0 Powerups |
| q3r_demobowl | Destroy Yourself Now | Stecki | DM, Team DM, Derby | LG/RL/RG | 0 Powerups |
| q3r_dm02 | Classic Demolition Derby | gout | Derby, DM, Team DM | RL/PG/SG | 1 Powerup |
| q3r_lcs1 | LCS TEST MAP | insellium | Last Car Standing | All Weapons | No Powerups |
| q3r_mk | Warehouse Playground | insellium | DM, Team DM, Domination | All Weapons | All Powerups |
| test | Test | P3rlE | DM | RL/PG/SG | 1 Powerup |

> Quellen / Sources: `.arena`-Dateien für Titel, Modi und Loot.【F:baseq3r/scripts/q3r_babel.arena†L3-L33】【F:baseq3r/scripts/q3r_battlecanyon.arena†L3-L33】【F:baseq3r/scripts/q3r_citydom.arena†L3-L33】【F:baseq3r/scripts/q3r_ctf_01.arena†L3-L33】【F:baseq3r/scripts/q3r_ctf4_01.arena†L3-L30】【F:baseq3r/scripts/q3r_david4.arena†L3-L33】【F:baseq3r/scripts/q3r_david5.arena†L3-L33】【F:baseq3r/scripts/q3r_demobowl.arena†L3-L33】【F:baseq3r/scripts/q3r_dm02.arena†L3-L33】【F:baseq3r/scripts/q3r_lcs1.arena†L3-L33】【F:baseq3r/scripts/q3r_mk.arena†L3-L33】【F:baseq3r/scripts/test.arena†L3-L33】

## 2. Vehicle Garage / Fahrzeug-Garage

**DE:** Die fahrbaren Untersätze liegen als Modelle unter `baseq3r/models/players`. Bot-Profile beschreiben Handling und Charakteristika.
**EN:** Vehicles ship as player models in `baseq3r/models/players`, with bot profiles summarizing handling traits.

| Vehicle | Core Highlights |
| --- | --- |
| Alpine | Compact sports car with extensive light tags; bots praise its smooth racing line.【F:baseq3r/models/players/alpine/green.skin†L1-L21】【F:baseq3r/scripts/bots.txt†L4-L8】 |
| Camaro | American classic with multiple stripe liveries; bots note playful drifting.【F:baseq3r/models/players/camaro/green.skin†L1-L40】【F:baseq3r/scripts/bots.txt†L19-L24】 |
| Gravelord | Hearse with curtains and coffin options for stylish knockouts.【F:baseq3r/models/players/gravelord/green.skin†L1-L24】 |
| Gremlin | Compact sedan with minimalistic paintwork and consistent handling.【F:baseq3r/models/players/gremlin/blue.skin†L1-L15】【F:baseq3r/scripts/bots.txt†L23-L24】 |
| MacDaddy | Muscular dragster with supercharger; bots call it loud and spectacular.【F:baseq3r/models/players/macdaddy/red.skin†L1-L19】【F:baseq3r/scripts/bots.txt†L21-L22】 |
| Mini | Short wheelbase with multiple roof colors; bots emphasize agility.【F:baseq3r/models/players/mini/green.skin†L1-L27】【F:baseq3r/scripts/bots.txt†L3-L12】 |
| Murciélago | Wedge-shaped supercar with stripe skins; described as the “red missile.”【F:baseq3r/models/players/murcielago/red.skin†L1-L30】【F:baseq3r/scripts/bots.txt†L14-L19】 |
| Raptor | Pickup with extra lights and brush guard for aggressive passes.【F:baseq3r/models/players/raptor/black.skin†L1-L28】【F:baseq3r/scripts/bots.txt†L5-L7】【F:baseq3r/scripts/bots.txt†L17-L22】 |
| Reaper | Tube-frame buggy with mesh protection and sharp bite.【F:baseq3r/models/players/reaper/green.skin†L1-L17】【F:baseq3r/scripts/bots.txt†L2-L9】 |
| Roadster | Open-top cabrio sharing parts with the Slingshot; elegant flair.【F:baseq3r/models/players/roadster/blue.skin†L1-L15】【F:baseq3r/scripts/bots.txt†L1-L13】 |
| Sidepipe | Muscle car with side exhausts and heavy spoilers to defend track position.【F:baseq3r/models/players/sidepipe/green.skin†L1-L23】【F:baseq3r/scripts/bots.txt†L7-L16】 |
| Slingshot | Targa variant with removable roofs ideal for slipstream attacks.【F:baseq3r/models/players/slingshot/yellow.skin†L1-L20】【F:baseq3r/scripts/bots.txt†L5-L7】 |
| Viper | Aerodynamic GT coupe with graphite finishes; bots call it a lurking predator.【F:baseq3r/models/players/viper/graphite.skin†L1-L24】【F:baseq3r/scripts/bots.txt†L12-L18】 |

## 3. Weapons and Powerups / Bewaffnung und Powerups

**DE:** Q3Rally erweitert die bekannte Frontbewaffnung um Rearfire-Tools und spezielle Powerups.
**EN:** Q3Rally expands classic front weapons with rearfire tools and bespoke powerups.

### 3.1 Front Weapons / Frontwaffen

| Pickup | Default Ammo | Description |
| --- | --- | --- |
| Chainsaw | Unlimited | Close combat chainsaw replacing the Gauntlet.【F:engine/code/game/bg_misc.c†L265-L279】 |
| Shotgun | 10 shells | Wide pellet spread for short to mid-range engagements.【F:engine/code/game/bg_misc.c†L281-L295】 |
| Machinegun | 40 bullets | Continuous fire to sweep narrow passages.【F:engine/code/game/bg_misc.c†L297-L311】 |
| Grenade Launcher | 10 grenades | Arcing splash projectiles useful for area denial.【F:engine/code/game/bg_misc.c†L313-L327】 |
| Rocket Launcher | 10 rockets | Classic splash damage for tunnels and jumps.【F:engine/code/game/bg_misc.c†L329-L343】 |
| Lightning Gun | 100 energy | Beam weapon that stabilizes hits while overtaking.【F:engine/code/game/bg_misc.c†L345-L359】 |
| Railgun | 10 slugs | Precision shot for long straights.【F:engine/code/game/bg_misc.c†L361-L375】 |
| Plasma Gun | 50 plasma | Rapid-fire orbs for tight corner sequences.【F:engine/code/game/bg_misc.c†L377-L391】 |
| BFG10K | 20 orbs | Massive splash damage in arena and CTF maps.【F:engine/code/game/bg_misc.c†L393-L407】 |
| Flame Thrower | 20 flames | Q3Rally-exclusive long cone to torch blocking rivals.【F:engine/code/game/bg_misc.c†L431-L445】 |

### 3.2 Rearfire Arsenal / Heckbewaffnung

| Pickup | Ammo | Description |
| --- | --- | --- |
| Rearfire Smoke Screen | 15 charges | Deploys an opaque cloud that blinds pursuers.【F:engine/code/game/bg_misc.c†L449-L463】 |
| Rearfire Oil Slick | 15 charges | Reduces friction on contact; puddles can ignite.【F:engine/code/game/bg_misc.c†L465-L478】【F:engine/code/game/g_rally_hazard.c†L24-L133】 |
| Rearfire Mine | 5 mines | Sticks to track and explodes on contact for heavy damage.【F:engine/code/game/bg_misc.c†L480-L493】 |
| Rearfire Flame Trail | 15 charges | Leaves a burning strip that inflicts fire hazards.【F:engine/code/game/bg_misc.c†L495-L508】 |
| Rearfire Bio Sludge | 15 charges | Spawns toxic trail with expanding damage radius.【F:engine/code/game/bg_misc.c†L510-L520】【F:engine/code/game/g_rally_hazard.c†L96-L133】 |

### 3.3 Powerups and Holdables / Powerups und Holdables

| Pickup | Duration / Uses | Description |
| --- | --- | --- |
| Turbo | 5 s | Temporary afterburner increasing acceleration and top speed.【F:engine/code/game/bg_misc.c†L714-L729】 |
| Quad Damage | 30 s | Quadruples outgoing weapon damage.【F:engine/code/game/bg_misc.c†L736-L758】 |
| Environmental Protection | 30 s | Mitigates environmental damage such as lava.【F:engine/code/game/bg_misc.c†L760-L786】 |
| Haste | 30 s | Raises fire rate and reload speed.【F:engine/code/game/bg_misc.c†L788-L815】 |
| Invisibility | 30 s | Makes vehicles harder to see for ambush play.【F:engine/code/game/bg_misc.c†L817-L839】 |
| Auto Repair | 30 s | Continuously regenerates vehicle hit points.【F:engine/code/game/bg_misc.c†L841-L863】 |
| Shielding | 30 s | Adds extra damage absorption stacking with armor.【F:engine/code/game/bg_misc.c†L883-L898】 |
| Personal Teleporter | Single use | Instantly resets to the last safe location.【F:engine/code/game/bg_misc.c†L681-L695】 |
| Fuel Can | Single use | Refills the fuel tank to max without affecting turbo reserves.【F:engine/code/game/bg_misc.c†L244-L258】【F:engine/code/game/g_active.c†L874-L877】 |
| Medkit | Single use | Restores health on demand to avert wrecks.【F:engine/code/game/bg_misc.c†L696-L712】 |

**DE:** Zusätzlich stehen Munitionspakete inklusive Flammenwerfer-Nachschub bereit.
**EN:** Standard ammo pickups, including flamethrower fuel, remain available throughout the maps.【F:engine/code/game/bg_misc.c†L660-L674】

## 4. Game Modes and Rules / Spielmodi und Regeln

**DE:** Die Wertungslogik folgt dem `scoring.txt`-Dokument sowie ergänzenden Reviews.
**EN:** Scoring logic adheres to `scoring.txt` and supporting design notes.

- Racing (GT_RACING) – Position anhand Rennfortschritt; Frags zählen nicht.【F:scoring.txt†L3-L9】
- Racing DM (GT_RACING_DM) – Wie Racing, aber jeder Frag gewährt 500 ms Zeitbonus.【F:scoring.txt†L7-L9】
- Team Racing (GT_TEAM_RACING / _DM) – Teamzeiten summieren sich; DM-Variante vergibt Frag-Boni.【F:scoring.txt†L35-L41】
- Elimination (GT_ELIMINATION) – Letzte Plätze fliegen nach Intervallen heraus; Arenas markieren Tauglichkeit per `elimination "1"`.【F:docs/elimination_mode.md†L3-L38】【F:baseq3r/scripts/q3r_country01.arena†L12-L45】
- Demolition Derby (GT_DERBY) – Letzter fahrbereiter Wagen gewinnt; HUD trackt Schäden und Wracks.【F:scoring.txt†L15-L18】【F:docs/demolition_derby_review.md†L5-L22】
- Last Car Standing (GT_LCS) – Survival ohne Respawns; endet beim letzten aktiven Fahrzeug.【F:scoring.txt†L19-L21】
- Deathmatch & Team Deathmatch (GT_DEATHMATCH, GT_TEAM) – Klassische +1/-1 Fragwertung; Team-Scores folgen Spielerpunkten.【F:scoring.txt†L27-L34】
- Capture the Flag / 4-Team-CTF (GT_CTF, GT_CTF4) – Flaggenaktionen geben Punkte, Vier-Team-Variante färbt Führungswechsel ein.【F:scoring.txt†L43-L49】
- Domination (GT_DOMINATION) – Kontrollpunkte generieren Punkte in Intervallen laut `g_dominationScoreInterval`.【F:scoring.txt†L51-L53】

## 5. Key CVars / Wichtige CVars

| Category | CVar | Default | Purpose |
| --- | --- | --- | --- |
| Racing | `g_laplimit` | 5 | Sets default lap count; maps may override it.【F:engine/code/game/g_main.c†L178-L186】 |
| Racing | `g_forceEngineStart` | 60 s | Forces race start after countdown with a 10 s warning.【F:engine/code/game/g_main.c†L261-L263】【F:engine/code/game/g_rally_racetools.c†L461-L467】 |
| Racing | `g_finishRaceDelay` | 30 s | Maximum grace period once finishers are registered.【F:engine/code/game/g_main.c†L261-L263】【F:engine/code/game/g_main.c†L2136-L2143】 |
| Racing | `g_trackReversed`, `g_trackLength` | 0 | Populated on load to sync direction and length across clients.【F:engine/code/game/g_main.c†L257-L260】 |
| Elimination | `g_eliminationStartDelay` | 30 s | Delay before the first knockout wave.【F:engine/code/game/g_main.c†L178-L185】 |
| Elimination | `g_eliminationInterval` | 15 s | Interval between eliminations.【F:engine/code/game/g_main.c†L178-L185】 |
| Elimination | `g_eliminationWarning` | 5 s | Warning time before the next elimination; drives HUD timer.【F:engine/code/game/g_main.c†L178-L185】 |
| Elimination | `g_eliminationWeapons` | 0 | Toggles weapons in elimination races.【F:engine/code/game/g_main.c†L178-L185】【F:engine/code/game/g_rally_tools.c†L297-L304】 |
| Vehicle Physics | `car_spring`, `car_shock_*`, `car_swaybar`, `car_wheel*` | various | Tunes suspension and damping for all cars server-side.【F:engine/code/game/g_main.c†L268-L286】 |
| Vehicle Physics | `car_frontweight_dist`, `car_IT_*`, `car_body_elasticity` | various | Controls inertia and chassis flex.【F:engine/code/game/g_main.c†L277-L283】 |
| Aerodynamics & Grip | `car_air_cof`, `car_air_frac_to_df`, `car_friction_scale` | 0.31 / 0.5 / 1.1 | Adjusts drag, downforce share, and surface grip globally.【F:engine/code/game/g_main.c†L283-L286】 |
| Damage | `g_vehicleDamageScale`, `g_vehicleDamageOffset`, `g_vehicleHealth` | 5.0 / 0 / 100 | Sets collision damage scaling and base hit points.【F:engine/code/game/g_main.c†L287-L291】 |
| Derby | `g_derbyDamageFactor`, `g_derbyRammerDamageRatio`, `g_derbyIgnoreDamageScale` | 1.0 / 1.0 / 0 | Refines derby collision evaluation and damage sharing.【F:engine/code/game/g_main.c†L291-L295】 |
| Derby | `g_derbyRamRadius`, `g_derbyRamDamage`, `g_derbyRamDamageScale`, `g_derbyRamDamageMax` | 0 / 100 / 0.05 / 50 | Controls ram hitbox and damage curve; see derby review for balance notes.【F:engine/code/game/g_main.c†L294-L297】【F:docs/demolition_derby_review.md†L5-L19】 |
| Domination | `g_dominationScoreInterval`, `g_dominationCaptureDelay`, `g_dominationSpawnStyle` | 10000 / 1500 / 0 | Sets point tick rate, capture delay, and spawn logic.【F:engine/code/game/g_main.c†L303-L305】 |
| Atmosphere | `g_enableSnow`, `g_gravity`, `pmove_fixed`, `pmove_msec` | 0 / 1400 / 1 / 18 | Toggles weather and defines base movement physics.【F:engine/code/game/g_main.c†L210-L256】 |

## 6. Mapping Entities / Mapping-Entities

### 6.1 Checkpoint and Race Volumes / Checkpoint- und Rennen-Volumes

- `rally_startfinish` – Combines start and finish in one brush. If `g_laplimit` is unset, the `laps` key takes control and syncs the lap limit before broadcasting as a checkpoint.【F:engine/code/game/g_rally_mapents.c†L533-L556】
- `rally_start` – Standalone sprint start volume that enforces a single lap (`laplimit = 1`) and emits checkpoint data.【F:engine/code/game/g_rally_mapents.c†L558-L573】
- `rally_finish` – Dedicated finish trigger marking vehicles as complete via checkpoint logic.【F:engine/code/game/g_rally_mapents.c†L575-L587】
- `rally_checkpoint` – Auto-numbers checkpoints; optional `target` can offset the volume to a `target_position` entity. Spawnflag 1 enables placement messages, Spawnflag 2 plays the global checkpoint sound.【F:engine/code/game/g_rally_mapents.c†L593-L693】

### 6.2 Weather and Atmosphere / Wetter- und Atmosphärensteuerung

- `rally_sun` – Sets sun direction; forwards position to clients.【F:engine/code/game/g_rally_mapents.c†L695-L701】
- `rally_weather_rain` – Brush volume spawning rain particles; `number` controls density, `spawnflags` add lightning or wind effects.【F:engine/code/game/g_rally_mapents.c†L705-L714】
- `rally_weather_snow` – Snow volume with fixed type (`weapon = 1`), ignoring extra spawnflags.【F:engine/code/game/g_rally_mapents.c†L717-L726】

### 6.3 Script and Hazard Utilities / Script- und Hazard-Utilities

- `rally_scripted_object` – Requires a `script` key referencing a `.script` file to drive animation and physics; registers as `ET_SCRIPTED` with collision callbacks, ideal for destructible set pieces.【F:engine/code/game/g_rally_scripted_objects.c†L80-L158】【F:engine/code/game/g_rally_scripted_objects.c†L368-L415】
- `trigger_fuel` – Brush trigger that refuels vehicles over time. `rate` sets fill speed (default 10 units/s) and `max` defines the target level; refueling stops at the cap.【F:engine/code/game/g_trigger.c†L421-L463】
- Hazard fields (Oil/Fire/Bio) – Spawned by rearfire weapons. Oil lowers friction in a radius, bio sludge expands with toxin damage, and fire ignites oil slicks; mappers can script triggers or spawn items for them.【F:engine/code/game/g_rally_hazard.c†L24-L133】

## 7. Future Ideas / Ideen für zukünftige Updates

1. Dynamic race classes – Server scripts could parse `.arena` data to auto-build Sprint, GP, or Battle playlists (e.g., Sprint = `laps <= 1`, Battle = armed).【F:baseq3r/scripts/q3r_a_to_b_test.arena†L21-L36】【F:baseq3r/scripts/q3r_lavafalls.arena†L24-L39】
2. Garage showroom with bot quotes – Surface bot descriptions alongside rotating `.skin` variants to highlight vehicle character.【F:baseq3r/scripts/bots.txt†L1-L24】【F:baseq3r/models/players/macdaddy/red.skin†L1-L19】
3. Elimination telemetry HUD – Extend HUD to expose lap, remaining drivers, and warning countdown as described in the elimination docs.【F:docs/elimination_mode.md†L30-L38】

## 8. Menu Music Player / Menü-Musikplayer

**DE:** Das Hauptmenü startet beim Laden automatisch einen zufälligen Track aus `music/menumusic*.ogg`. Die UI sammelt alle passenden Dateien, zieht per Zufall eine Auswahl und spielt sie über den `music`-Konsolenbefehl ab – so rotiert das Menü eigenständig durch eure Playlist.【F:engine/code/q3_ui/ui_menu.c†L368-L390】
**EN:** On load, the main menu now autoplays a random `music/menumusic*.ogg` track. The UI gathers every matching file, picks one at random, and fires the `music` console command so the lobby cycles through your curated playlist without extra setup.【F:engine/code/q3_ui/ui_menu.c†L368-L390】

**Tip / Hinweis:** Legt zusätzliche `.ogg`-Dateien mit dem Präfix `menumusic` im Ordner `baseq3r/music/` ab, um die Rotation um eigene Songs zu erweitern.【F:engine/code/q3_ui/ui_menu.c†L368-L390】

**Jukebox-Konsolenbefehle (Ingame):**

- `jukebox_play` – Startet/stoppt die Ingame-Jukebox.
- `jukebox_next` – Springt zum nächsten Track.
- `jukebox_prev` – Springt zum vorherigen Track.
- `jukebox_rescan` – Scannt `music/jukebox` neu und aktualisiert die Trackliste.

## 9. Menu Background via URL / Menü-Hintergrund per URL

**DE:** Der Menü-Hintergrund kann optional aus einer entfernten Bilddatei geladen werden. Die UI nutzt dabei lokale Cache-Dateien und fällt bei Fehlern automatisch auf den Standardhintergrund zurück. Aktivierung und Intervall sind vollständig deaktivierbar.
**EN:** The menu background can optionally be fetched from a remote image. The UI uses cached local files and automatically falls back to the default background on errors. The feature is fully disableable and supports an optional refresh interval.

**Cvars / Konsolenvariablen:**

- `ui_menuBackEnable` (0/1, Standard: 1) – Schaltet den URL-basierten Menü-Hintergrund ein oder aus.
- `ui_menuBackUrl` (String, Standard: `https://ladder.q3rally.com/background/bg.png`) – HTTP(S)-Adresse der Bilddatei.
- `ui_menuBackRefreshSec` (Sekunden, Standard: 0) – Aktualisierungsintervall; `0` deaktiviert automatische Refreshs.

**Konsolensteuerung / Console control:**

- `ui_menuBackRefresh` – Erzwingt einen sofortigen Download-Versuch für die aktuelle `ui_menuBackUrl`.
