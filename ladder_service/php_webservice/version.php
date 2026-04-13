<?php
/**
 * Q3Rally Ladder Service – Version & Changelog
 * version.php
 */

declare(strict_types=1);

const LADDER_VERSION = '1.0.8';

const LADDER_CHANGELOG = [
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
