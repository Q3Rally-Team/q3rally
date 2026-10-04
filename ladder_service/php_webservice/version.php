<?php
/**
 * Q3Rally Ladder Service – Version & Changelog
 * version.php
 */

declare(strict_types=1);

const LADDER_VERSION = '1.0.15';

const LADDER_CHANGELOG = [
    '1.0.15' => [
        'date'    => '2026-10-04',
        'changes' => [
            'Match files, the match index and profiles are written under one lock and atomically (parallel uploads lost index entries, readers could see half-written files)',
            'Approved server keys get their own POST limit (120 per minute per key); players and unknown keys keep 30 per minute per IP; rate-limit files moved to data/private/',
            'Ghosts: a new ghost that is slower than the slowest one in a full bucket is refused with stored:false / BUCKET_FULL instead of being stored and dropped again',
            'Ghosts: GET /ghosts without physics/checksum lists only the current bucket (highest physics version, newest map build) instead of mixing incomparable lap times',
            'merge_player.php --apply holds the ladder lock while merging',
        ],
    ],
    '1.0.14' => [
        'date'    => '2026-10-04',
        'changes' => [
            'Security: ladder page escapes player names, server names, modes, maps and profile values in the match and profile views (stored XSS)',
            'Security admin.php: form tokens against cross-site requests (CSRF), new session id after login, session cookie HttpOnly + SameSite=Strict (+ Secure on HTTPS)',
            'Security admin.php: failed logins limited (5 per IP, 30 overall per 15 minutes), no framing, no caching',
            'admin.php shows and posts a short key id instead of the key; logout button',
            'Leaderboard rows link the newest player id of a name (a player whose id changed opened the old profile)',
            'New command line tool merge_player.php: moves matches, ghosts, key binding and profile of an old player id to the new one (dry run by default)',
            'Offline keys belong to one player: the first upload binds the key to its player id; afterwards only that player is credited (other players stay in the match without profile credit, foreign ghosts are refused); admin.php shows the binding and can release it',
        ],
    ],
    '1.0.13' => [
        'date'    => '2026-10-04',
        'changes' => [
            'Security: server keys moved to data/private/ (deny-all .htaccess); data/server_keys.json is moved there automatically',
            'Security: match ids can no longer name internal files (server_keys, match_index, version, rl_*)',
            'Security: POST /api/v1/register always creates a pending key request (offline keys were active immediately)',
            'Security: DELETE /api/v1/matches/{id} only for the key that reported the match, never for offline keys',
            'Offline keys always report offline matches and ghosts, independent of server.dedicated',
            'Key storage changes run under an exclusive lock and are written atomically',
            'Registration throttle: 10 key requests per IP and hour (register.php and /api/v1/register)',
            'GET /api/v1/matches/{id} returns the public match view (no guid, profile snapshot or reporter id)',
        ],
    ],
    '1.0.12' => [
        'date'    => '2026-10-03',
        'changes' => [
            'New tab "Ghosts": fastest lap ghosts per map, track variant, vehicle and map version, with ghost download',
            'New: GET /api/v1/ghosts/catalog lists maps, variants, map builds and vehicles that have ghosts',
            'Fix: wide tables scroll inside their box on small screens instead of widening the page',
        ],
    ],
    '1.0.11' => [
        'date'    => '2026-10-03',
        'changes' => [
            'GET /api/v1/ghosts: perVehicle=K returns the best K ghosts per vehicle',
            'GET /api/v1/ghosts: format=text returns a tab separated list for the game engine (Ghost Race opponents)',
        ],
    ],
    '1.0.10' => [
        'date'    => '2026-10-02',
        'changes' => [
            'New: lap ghosts – POST /api/v1/ghosts stores the best lap ghost per player, map, track variant and vehicle',
            'New: GET /api/v1/ghosts ranking list and GET /api/v1/ghosts/{ghostId} (?format=raw for the .ghost text)',
            'Ghosts are grouped by physics version and map checksum',
            'Plausibility checks on ghost uploads; key usage counted as ghostCount',
        ],
    ],
    '1.0.8' => [
        'date'    => '2026-04-13',
        'changes' => [
            'Contract update: mode-aware player fields are now validated and normalized per game mode',
            'New required fields by mode: racing/sprint/team-racing require raceTimeMs + bestLapMs + checkpoints; deathmatch/team/derby/lcs require kills + deaths; objective modes require objectiveScore + objectiveTimeMs; elimination also requires eliminationRound + eliminationState',
            'Breaking change: ambiguous score-only payloads are rejected when mode-specific mandatory fields are missing',
            'Non-breaking: deprecated aliases (lapTime, frags, captures, roundState) are still accepted but normalized to canonical keys',
            'Migration guidance added for game server, Python ladder service, PHP webservice and dashboard consumers',
            'Release checklist added for coordinated rollout across all services and consumers',
        ],
    ],
    '1.0.7' => [
        'date'    => '2026-04-13',
        'changes' => [
            'Ingest parser now normalizes mode-specific player fields (race/kills/zone/elimination)',
            'Index mapping updated for score/kills/race timers/zone hold and elimination metadata',
            'Backward-compatible payload handling: legacy aliases still accepted in degraded mode',
            'Structured API errors with code/message/details payloads (no silent no-player drops)',
        ],
    ],
    '1.0.6' => [
        'date'    => '2026-03-30',
        'changes' => [
            'canonicalMode(): returns null for unknown game modes instead of falling back to gt_elimination',
            'Unknown game modes counted as __unknown__ in breakdown, excluded from all leaderboards',
            'gamesPlayed: always incremented per upload; snapshot value no longer used',
            'Overlay layout: header contains close button only; title and match info moved into body',
            'Profile overlay: rank and score shown as info strip in body, not in header bar',
            'Match details: mode and map displayed as body heading via humanizeMode / humanizeMapName',
        ],
    ],
    '1.0.5' => [
        'date'    => '2026-03-27',
        'changes' => [
            'Admin: added Offline tab – keys registered via the in-game wizard are now grouped separately',
            'Admin: added Revoked tab with permanent delete action',
            'Admin: Offline keys can be approved/revoked directly from the Offline tab',
            'register.php: JSON response branch for in-game registration wizard (Accept: application/json)',
            'Engine: ladder_register command now async (non-blocking, driven by SV_LadderFrame)',
            'Engine: sv_ladderApiKey, sv_ladderEnabled, sv_hostname set from engine code after registration',
            'Engine: writeconfig triggered automatically on successful registration',
            'Engine: SV_LadderFrame called before com_sv_running guard so registration works in main menu',
        ],
    ],
    '1.0.4' => [
        'date'    => '2026-03-20',
        'changes' => [
            'In-game ladder registration wizard (ui_rally_ladder_wizard)',
            'ladder_register / ladder_register_abort console commands',
            'Per-server API key authentication via Bearer token',
            'Auto-suspend inactive keys after 90 days',
            'Admin panel: approve / revoke keys',
            'register.php: self-service server key registration',
        ],
    ],
    '1.0.3' => [
        'date'    => '2026-02-10',
        'changes' => [
            'Player profile overlay in leaderboard frontend',
            'Achievement tier display per category',
            'Match detail overlay',
            'Online / Offline source toggle in leaderboard',
        ],
    ],
    '1.0.2' => [
        'date'    => '2026-01-15',
        'changes' => [
            'Leaderboard index endpoint (/matches/index) for fast frontend loads',
            'TGA levelshot support in frontend',
            'Map metadata from .arena files',
            'Levelshot manifest endpoint (/maps/levelshots)',
        ],
    ],
    '1.0.1' => [
        'date'    => '2025-12-01',
        'changes' => [
            'Rate limiting per IP (30 req / 60 s)',
            'Match spool with retry and exponential backoff',
            'Elimination, CTF and Deathmatch leaderboard tabs',
        ],
    ],
    '1.0.0' => [
        'date'    => '2025-11-01',
        'changes' => [
            'Initial release',
            'Match upload endpoint (POST /matches)',
            'Basic leaderboard frontend',
            'Race / Sprint leaderboard',
        ],
    ],
];
